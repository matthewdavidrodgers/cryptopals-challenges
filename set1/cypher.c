#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"

#define RK_BLOCKS 2
#define PERMUTATIONS(X) ((((X-1) * (X-1)) + (X-1)) / 2)

double avg_char_frequencies[] = {0.082, 0.015, 0.028, 0.043, 0.13, 0.022, 0.02, 0.061, 0.07, 0.0015, 0.0077, 0.04, 0.024, 0.067, 0.075, 0.019, 0.00095, 0.06, 0.063, 0.091, 0.028, 0.0098, 0.024, 0.0015, 0.02, 0.00074, 0.082*0.003, 0.015*0.003, 0.028*0.003, 0.043*0.003, 0.13*0.003, 0.022*0.003, 0.02*0.003, 0.061*0.003, 0.07*0.003, 0.0015*0.003, 0.0077*0.003, 0.04*0.003, 0.024*0.003, 0.067*0.003, 0.075*0.003, 0.019*0.003, 0.00095*0.003, 0.06*0.003, 0.063*0.003, 0.091*0.003, 0.028*0.003, 0.0098*0.003, 0.024*0.003, 0.0015*0.003, 0.02*0.003, 0.00074*0.003};

size_t sorted_insert_index(keysize_t *collection, size_t count, keysize_t item)
{
    size_t i = 0;

    while (i < count && collection[i].score < item.score) i++;

    return i;
}

void insert_at(keysize_t *collection, size_t count, keysize_t item, size_t index)
{
    if (index < count - 1)
        memmove(collection + index + 1, collection + index, sizeof(keysize_t) * (count - index - 1));

    collection[index] = item;
}

void pick_rk_xor_keysizes(bbuf *cyphertext, keysize_t *keysizes, size_t count)
{
    size_t keysize, num_keysizes, insert_index, i, j, x, y;
    keysize_t curr_keysize = {};
    double block_dis;
    bbuf blocks[RK_BLOCKS];

    num_keysizes = 0;
    for (keysize = 2; keysize <= 40; keysize++)
    {
        for (i = 0; i < RK_BLOCKS; i++)
            bbuf_slice(&blocks[i], cyphertext, i * keysize, (i * keysize) + keysize);

        block_dis = 0.0;
        for (x = 0; x < RK_BLOCKS; x++)
            for (y = x + 1; y < RK_BLOCKS; y++)
                block_dis += (double)distance(&blocks[x], &blocks[y]) / keysize;
        block_dis /= PERMUTATIONS(RK_BLOCKS);

        curr_keysize.keysize = keysize;
        curr_keysize.score = block_dis;
        insert_index = sorted_insert_index(keysizes, num_keysizes, curr_keysize);

        if (num_keysizes < count || insert_index < num_keysizes) {
            insert_at(keysizes, count, curr_keysize, insert_index);
            if (num_keysizes < count) num_keysizes++;
        }

        for (i = 0; i < RK_BLOCKS; i++)
            bbuf_destroy(&blocks[i]);
    }
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

size_t max_char_frequency(bbuf *buffer)
{
    size_t buffer_char_counts[52], max_freq, char_offset, i;
    uint8_t byte;
    bool is_countable_char;

    for (i = 0; i < 52; i++)
        buffer_char_counts[i] = 0;

    for (i = 0; i < buffer->len; i++)
    {
        byte = buffer->buf[i];
        buffer_char_counts[byte]++;
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

        if (is_countable_char)
            buffer_char_counts[byte - char_offset]++;
    }

    max_freq = 0;

    for (i = 0; i < 52; i++)
        if (buffer_char_counts[i] > max_freq)
            max_freq = buffer_char_counts[i];

    return max_freq;
}

uint8_t pick_uniform_xor_byte(bbuf *buffer)
{
    size_t score;
    ssize_t best_score;
    uint8_t key, best_key;
    bbuf key_buff, result_buff;

    bbuf_initTo(&key_buff, 1);

    key = 0;
    best_score = -1;
    do
    {
        key_buff.buf[0] = key;
        result_buff = xor(buffer, &key_buff);
        bbuf_print(&result_buff);
        score = max_char_frequency(&result_buff);
        bbuf_print(&result_buff);
        if (best_score == -1 || best_score < score)
        {
            best_score = score;
            best_key = key;
        }

        bbuf_destroy(&result_buff);
    } while (++key > 0);

    bbuf_destroy(&key_buff);
    return best_key;
}

double scoreBufferDistance(bbuf *buffer)
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

    score *= (buffer->len - countable_chars);

    return score;
}

sb_xor_decode_details decodeSingleByteXOR(bbuf *buffer)
{
    sb_xor_decode_details winning, current;
    bbuf key_buffer, decoded_buffer;
    uint8_t key, winning_key;
    double score, winning_score;
    size_t i;

    bbuf_initTo(&key_buffer, 1);

    winning.score = -1.0;
    current.key = 0;
    do
    {
        key_buffer.buf[0] = current.key;
        decoded_buffer = xor(buffer, &key_buffer);    
        current.score = scoreBufferDistance(&decoded_buffer);

        if (current.score != -1.0 && (winning.score == -1.0 || current.score < winning.score))
        {
            winning.score = current.score;
            winning.key = current.key;
        }

        bbuf_destroy(&decoded_buffer);
    } while (++current.key > 0);

    bbuf_destroy(&key_buffer);

    return winning;
}
