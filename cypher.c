#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "common.h"
#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"

#include <stdio.h>

double avg_char_frequencies[] = {
    0.082389258,         0.015051398,         0.028065007,         0.042904556,
    0.128138650,         0.022476217,         0.020327458,         0.061476691,
    0.061476691,         0.001543474,         0.007787989,         0.040604477,
    0.024271893,         0.068084376,         0.075731132,         0.019459884,
    0.000958366,         0.060397268,         0.063827211,         0.091357551,
    0.027822893,         0.009866131,         0.023807842,         0.001513210,
    0.019913847,         0.000746517,

    0.082389258*.003,    0.015051398*.003,    0.028065007*.003,    0.042904556*.003,
    0.128138650*.003,    0.022476217*.003,    0.020327458*.003,    0.061476691*.003,
    0.061476691*.003,    0.001543474*.003,    0.007787989*.003,    0.040604477*.003,
    0.024271893*.003,    0.068084376*.003,    0.075731132*.003,    0.019459884*.003,
    0.000958366*.003,    0.060397268*.003,    0.063827211*.003,    0.091357551*.003,
    0.027822893*.003,    0.009866131*.003,    0.023807842*.003,    0.001513210*.003,
    0.019913847*.003,    0.000746517*.003
};

#define KEYSIZES_TAKEN 10
#define SB_XOR_BYTES_TAKEN 6
#define COMPARE_SCORES 10
#define RK_BLOCKS 2
#define PERMUTATIONS(X) ((((X-1) * (X-1)) + (X-1)) / 2)

ssize_t insert_at_sorted_details_col(sb_xor_decode_details *collection, size_t len, size_t cap, sb_xor_decode_details item)
{
    size_t insert_index = 0;

    while (insert_index < len && collection[insert_index].score < item.score)
        insert_index++;

    if (len < cap || insert_index < len)
    {
       if (len > 0 && insert_index < len - 1)
           memmove(collection + insert_index + 1, collection + insert_index,
                   sizeof(sb_xor_decode_details) * (len - insert_index - 1));
       collection[insert_index] = item;
       return (ssize_t)insert_index;
    }
    else
        return -1;
}

ssize_t insert_at_sorted_keysize_col(keysize_t *collection, size_t len, size_t cap, keysize_t item)
{
    size_t insert_index = 0;

    while (insert_index < len && collection[insert_index].score < item.score)
        insert_index++;

    if (len < cap || insert_index < len)
    {
        if (len > 0 && insert_index < len)
            memmove(collection + insert_index + 1, collection + insert_index,
                    sizeof(keysize_t) * (len - insert_index - 1));
        collection[insert_index] = item;
        return (ssize_t)insert_index;
    }
    else
        return -1;
}

size_t pick_rk_xor_keysizes(bbuf *cyphertext, keysize_t *keysizes, size_t count)
{
    size_t max_keysize, num_blocks, keysize, num_keysizes, i, x, y;
    ssize_t inserted_at;
    keysize_t curr_keysize = {};
    double block_dis;
    bbuf *blocks;
    max_keysize = cyphertext->len / 2 < 40 ? cyphertext->len / 2 : 40;

    num_keysizes = 0;
    for (keysize = 2; keysize <= max_keysize; keysize++)
    {
        num_blocks = cyphertext->len / keysize;
        blocks = (bbuf *)malloc(sizeof (bbuf) * num_blocks);
        for (i = 0; i < num_blocks; i++)
        {
            blocks[i] = bbuf_new();
            bbuf_slice(&blocks[i], cyphertext, i * keysize, (i * keysize) + keysize);
        }

        block_dis = 0.0;
        for (x = 0; x < num_blocks - 1; x++)
            for (y = x + 1; y < num_blocks; y++)
            {
                block_dis += (double)distance(&blocks[x], &blocks[y]) / (keysize * 8);
            }
        block_dis /= PERMUTATIONS(num_blocks);

        curr_keysize.keysize = keysize;
        curr_keysize.score = block_dis;

        inserted_at = insert_at_sorted_keysize_col(keysizes, num_keysizes, count, curr_keysize);
        if (inserted_at != -1 && (size_t)inserted_at == num_keysizes) num_keysizes++;

        for (i = 0; i < num_blocks; i++)
            bbuf_destroy(&blocks[i]);
        free(blocks);
    }

    return num_keysizes;
}

bbuf *break_and_transpose_buff(bbuf *buffer, size_t blocksize)
{
    size_t x, y;
    bbuf *buffers;

    buffers = (bbuf *)malloc(blocksize * sizeof(bbuf));

    for (x = 0; x < blocksize; x++)
    {
        bbuf_init(&buffers[x]);
        y = 0;
        while ((y + x) < buffer->len)
        {
            bbuf_append(&buffers[x], buffer->buf[y + x]);
            y += blocksize;
        }
    }

    return buffers;
}

double score_buffer_as_english(bbuf *buffer)
{
    int buffer_char_counts[52], countable_chars;
    double buffer_char_frequencies[52], score, char_score;
    bool is_countable_char;
    uint8_t byte;
    size_t i, char_offset;

    countable_chars = 0;

    for (i = 0; i < 52; i++)
        buffer_char_frequencies[i] = buffer_char_counts[i] = 0;

    for (i = 0; i < buffer->len; i++)
    {
        byte = buffer->buf[i];
        is_countable_char = false;
        if (byte >= 65 && byte <= 90)
        {
            is_countable_char = true;
            char_offset = 65 - 26;
        }
        else if (byte >= 97 && byte <= 122)
        {
            is_countable_char = true;
            char_offset = 97;
        }
        else if (byte == 32)
        {
            countable_chars++; // special case to give spaces weight over other non-text characters
        }

        if (is_countable_char)
        {
            countable_chars++;
            buffer_char_counts[byte - char_offset]++;
        }
    }

    if (countable_chars == 0) return -1.0;

    score = 0.0;

    for (i = 0; i < 52; i++)
    {
        buffer_char_frequencies[i] = buffer_char_counts[i] / (double)countable_chars;
        char_score = (avg_char_frequencies[i] - buffer_char_frequencies[i]) / 2.0;
        score += (char_score < 0) ? (-1*char_score) : char_score;
    }

    if ((int)buffer->len != countable_chars)
        score *= (buffer->len - countable_chars);

    return score;
}

xor_decode_details decode_rk_xor_for_size(bbuf *buffer, size_t keysize)
{
    bbuf *transposed_blocks;
    size_t i, j, *positions;
#ifdef ITERATE_TOP_KEYS
    size_t at_pos;
#endif
    xor_decode_details result;
    sb_xor_decode_details **byte_decode_entries;

    // init positions for iterating key enumerations
    positions = (size_t *)malloc(keysize * sizeof(size_t));
    for (i = 0; i < keysize; i++)
        positions[i] = 0;

    // init 2d array sb_xor_decode_details[keysize][SB_XOR_BYTES_TAKEN]
    byte_decode_entries = (sb_xor_decode_details **)malloc(keysize * sizeof(sb_xor_decode_details*));
    for (i = 0; i < keysize; i++)
    {
        byte_decode_entries[i] = (sb_xor_decode_details *)malloc(SB_XOR_BYTES_TAKEN * sizeof(sb_xor_decode_details));
        for (j = 0; j < SB_XOR_BYTES_TAKEN; j++)
            byte_decode_entries[i][j] = (sb_xor_decode_details){ .key = 0, .score = 0.0 };
    }


    transposed_blocks = break_and_transpose_buff(buffer, keysize);
#ifdef DEBUG_VERBOSE
    printf("\n\nkeysize: %lu\n", keysize);
#endif

    bbuf_init_to(&result.plaintext_buffer, buffer->len);
    bbuf_init_to(&result.key_buffer, keysize);
    result.score = -1.0;

    for (j = 0; j < keysize; j++)
    {
        decode_sb_xor_ranked(&transposed_blocks[j], byte_decode_entries[j], SB_XOR_BYTES_TAKEN);
#ifdef DEBUG_VERBOSE
        printf("top keys for block %lu:\n", j);
        bbuf_print(&transposed_blocks[j], BBUF_HEX);
        for (i = 0; i < SB_XOR_BYTES_TAKEN; i++)
            printf("key: %c (%u)\tscore %f\n", byte_decode_entries[j][i].key,
                   byte_decode_entries[j][i].key, byte_decode_entries[j][i].score);
#endif
    }

#ifdef ITERATE_TOP_KEYS
#ifdef DEBUG_VERBOSE
    printf("iterating all possible keys:\n");
#endif
    bbuf_init_to(&curr_result.plaintext_buffer, buffer->len);
    bbuf_init_to(&curr_result.key_buffer, keysize);
    
    // iterate over every enumeration of the possible keys given by
    // each transposed block's ranked best keys and score each
    // plaintext buffer resulting from that key to find the best
    for (;;)
    {
        // do
        for (i = 0; i < keysize; i++)
            curr_result.key_buffer.buf[i] = (byte_decode_entries[i][positions[i]]).key;
        xor_in_place(&curr_result.plaintext_buffer, buffer, &curr_result.key_buffer);
        curr_result.score = score_buffer_as_english(&curr_result.plaintext_buffer);

        if (result.score < 0 || curr_result.score < result.score)
        {
            result.score = curr_result.score;
            bbuf_slice(&result.key_buffer, &curr_result.key_buffer, 0,
                       curr_result.key_buffer.len);
            bbuf_slice(&result.plaintext_buffer, &curr_result.plaintext_buffer,
                       0, curr_result.plaintext_buffer.len);
        }

        // iterate
        at_pos = 0;
        positions[at_pos]++;

        // propagate addition carry
        while (positions[at_pos] == SB_XOR_BYTES_TAKEN)
        {
            positions[at_pos] = 0;
            at_pos++;
            if (at_pos == keysize) break;
            positions[at_pos]++;
        }
        if (at_pos == keysize) break;
    }
    bbuf_destroy(&curr_result.key_buffer);
    bbuf_destroy(&curr_result.plaintext_buffer);
#ifdef DEBUG_VERBOSE
    printf("iterated\n");
#endif

#else
    for (i = 0; i < keysize; i++)
        result.key_buffer.buf[i] = byte_decode_entries[i][0].key;
    xor_in_place(&result.plaintext_buffer, buffer, &result.key_buffer);
    result.score = score_buffer_as_english(&result.plaintext_buffer);
#endif

#ifdef DEBUG_VERBOSE
    char *keytext = to_string(&result.key_buffer);
    char *plaintext = to_string(&result.plaintext_buffer);
    printf("decoded using key \"%s\" to \"%s\" with score %f\n", keytext, plaintext, result.score);
    printf("\tkey: ");
    bbuf_print(&result.key_buffer, BBUF_HEX);
    printf("\tplaintext: ");
    bbuf_print(&result.plaintext_buffer, BBUF_HEX);
    free(keytext);
    free(plaintext);
#endif

    free(positions);
    for (i = 0; i < keysize; i++)
    {
        bbuf_destroy(&transposed_blocks[i]);
        free(byte_decode_entries[i]);
    }
    free(transposed_blocks);
    free(byte_decode_entries);

    return result;
}

xor_decode_details decode_rk_xor(bbuf *buffer)
{
    keysize_t best_sizes[KEYSIZES_TAKEN]; 
    xor_decode_details result, best_result;
    size_t i, actual_keysizes_taken;

    best_result = (xor_decode_details){ .key_buffer = bbuf_new(),
                                        .plaintext_buffer = bbuf_new(),
                                        .score = -1.0 };

    actual_keysizes_taken = pick_rk_xor_keysizes(buffer, best_sizes, KEYSIZES_TAKEN);

    for (i = 0; i < actual_keysizes_taken; i++)
    {
        result = decode_rk_xor_for_size(buffer, best_sizes[i].keysize);

        if (best_result.score < 0 || result.score < best_result.score)
        {
            best_result.score = result.score;
            bbuf_slice(&best_result.key_buffer, &result.key_buffer, 0,
                       result.key_buffer.len);
            bbuf_slice(&best_result.plaintext_buffer, &result.plaintext_buffer,
                       0, result.plaintext_buffer.len);
        }

        bbuf_destroy(&result.key_buffer);
        bbuf_destroy(&result.plaintext_buffer);
    }

    return best_result;
}

void decode_sb_xor_ranked(bbuf *buffer, sb_xor_decode_details *top, uint8_t top_taken)
{
    sb_xor_decode_details current;
    bbuf key_buffer = bbuf_new(), decoded_buffer = bbuf_new();
    uint8_t curr_top_taken;
    ssize_t inserted_at;

    curr_top_taken = 0;

    bbuf_init_to(&key_buffer, 1);
    current.key = 0;
    do
    {
        key_buffer.buf[0] = current.key;
        decoded_buffer = xor(buffer, &key_buffer);
        current.score = score_buffer_as_english(&decoded_buffer);

        if (current.score > 0)
        {
            inserted_at = insert_at_sorted_details_col(top, curr_top_taken, top_taken, current);
            if (inserted_at != -1 && inserted_at == curr_top_taken) curr_top_taken++;
        }

        bbuf_destroy(&decoded_buffer);
    } while (++current.key > 0);

#ifdef DEBUG_VERBOSE
    printf("took top %u keys (max %u)\n", curr_top_taken, top_taken);
#endif

    bbuf_destroy(&key_buffer);
}

sb_xor_decode_details decode_sb_xor(bbuf *buffer)
{
    sb_xor_decode_details winning, current;
    bbuf key_buffer, decoded_buffer;
    size_t i;
    ssize_t inserted_at;

    sb_xor_decode_details top_results[COMPARE_SCORES];
    size_t num_results = 0;
    for (i = 0; i < COMPARE_SCORES; i++)
        top_results[i] = (sb_xor_decode_details){ .key = 0, .score = 0.0 };

    key_buffer = bbuf_new();
    decoded_buffer = bbuf_new();

    bbuf_init_to(&key_buffer, 1);

    winning.score = -1.0;
    current.key = 0;
    do
    {
        key_buffer.buf[0] = current.key;
        decoded_buffer = xor(buffer, &key_buffer);    
        current.score = score_buffer_as_english(&decoded_buffer);

        if (current.score >= 0)
        {
            inserted_at = insert_at_sorted_details_col(top_results, num_results, COMPARE_SCORES, current);
            if (inserted_at != -1 && (size_t)inserted_at == num_results)
                num_results++;
        }

        if (current.score > 0.0 && (winning.score == -1.0 || current.score < winning.score))
        {
            winning.score = current.score;
            winning.key = current.key;
        }

        bbuf_destroy(&decoded_buffer);
    } while (++current.key > 0);

    bbuf_destroy(&key_buffer);

#ifdef DEBUG_VERBOSE
    for (i = 0; i < num_results; i++)
    {
        bbuf _key = bbuf_new(), _plaintext = bbuf_new();
        bbuf_init_to(&_key, 1);
        _key.buf[0] = top_results[i].key;
        _plaintext = xor(buffer, &_key);
        char *__plaintext = to_string(&_plaintext);

        printf("%c (%u) with score %f\t \"%s\"\n", top_results[i].key, top_results[i].key, top_results[i].score, __plaintext);
        bbuf_print(&_plaintext, BBUF_HEX);
        
        free(__plaintext);
        bbuf_destroy(&_key);
        bbuf_destroy(&_plaintext);
    }
#endif

    return winning;
}

bbuf decode_aes_ecb(bbuf *cyphertext, bbuf *key)
{
    int written, total_written;
    EVP_CIPHER_CTX *ctx;
    bbuf plaintext = bbuf_new();

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    bbuf_init_to(&plaintext, cyphertext->len);

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key->buf, NULL);

    total_written = written = 0;

    if (!EVP_CipherUpdate(ctx, plaintext.buf, &written, cyphertext->buf, cyphertext->len))
    {
        printf("OpenSSL Cipher update error\n");
        exit(1);
    }
    total_written += written;
    if (!EVP_CipherFinal(ctx, plaintext.buf + total_written, &written))
    {
        printf("OpenSSL Cipher finalize error\n");
        exit(1);
    }
    total_written += written;

    assert(((int)plaintext.len - 16) < total_written && total_written <= (int)plaintext.len);
    plaintext.len = total_written; // should i realloc off the extra bytes?

    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

bbuf aes_cbc_block(EVP_CIPHER_CTX *ctx, bbuf *block, bbuf *prev_block, bbuf *key, bool encrypt)
{
    bbuf output = bbuf_new();
    int actually_written;

    bbuf_init_to(&output, block->len);

    EVP_CIPHER_CTX_reset(ctx);
    EVP_CipherInit(ctx, EVP_aes_128_ecb(), key->buf, NULL, encrypt);
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    actually_written = 0;
    if (!EVP_CipherUpdate(ctx, output.buf, &actually_written, block->buf, 16))
    {
        printf("OpenSSL Cipher update error\n");
        exit(1);
    }
    assert(actually_written == 16);

    xor_in_place(&output, &output, prev_block);
    return output;
}

bbuf aes_cbc(bbuf *input, bbuf *key, bbuf *iv, bool encrypt)
{
    size_t block_idx, block_start;
    EVP_CIPHER_CTX *ctx;
    bbuf output = bbuf_new(), block = bbuf_new(), output_block = bbuf_new(), prev_block = bbuf_new();

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    bbuf_init_to(&output, input->len);

    ctx = EVP_CIPHER_CTX_new();
    bbuf_slice(&prev_block, iv, 0, iv->len);
    for (block_idx = 0; block_idx < (input->len / 16); block_idx++)
    {
        block_start = block_idx * 16;
        bbuf_slice(&block, input, block_start, block_start + 16);

        output_block = aes_cbc_block(ctx, &block, &prev_block, key, encrypt);
        memcpy(output.buf + block_start, output_block.buf, 16);
        bbuf_slice(&prev_block, &block, 0, block.len);

        bbuf_destroy(&output_block);
    }

    bbuf_destroy(&block);
    bbuf_destroy(&prev_block);

    EVP_CIPHER_CTX_free(ctx);

    return output;
}
