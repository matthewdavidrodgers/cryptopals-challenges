#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"

#include <stdio.h>

double avg_char_frequencies[] = {0.082, 0.015, 0.028, 0.043, 0.13, 0.022, 0.02, 0.061, 0.07, 0.0015, 0.0077, 0.04, 0.024, 0.067, 0.075, 0.019, 0.00095, 0.06, 0.063, 0.091, 0.028, 0.0098, 0.024, 0.0015, 0.02, 0.00074, 0.082*0.003, 0.015*0.003, 0.028*0.003, 0.043*0.003, 0.13*0.003, 0.022*0.003, 0.02*0.003, 0.061*0.003, 0.07*0.003, 0.0015*0.003, 0.0077*0.003, 0.04*0.003, 0.024*0.003, 0.067*0.003, 0.075*0.003, 0.019*0.003, 0.00095*0.003, 0.06*0.003, 0.063*0.003, 0.091*0.003, 0.028*0.003, 0.0098*0.003, 0.024*0.003, 0.0015*0.003, 0.02*0.003, 0.00074*0.003};

double scoreBufferDistance(bbuf *buffer)
{
    int buffer_char_counts[52], countable_chars, invalid_chars;
    double buffer_char_frequencies[52], score, char_score;
    bool is_countable_char, has_reached_null_term;
    uint8_t byte;
    size_t i, char_offset;

    countable_chars = 0;
    invalid_chars = 0;
    has_reached_null_term = false;

    for (i = 0; i < 52; i++)
        buffer_char_frequencies[i] = buffer_char_counts[i] = 0;

    for (i = 0; i < buffer->len; i++)
    {
        byte = buffer->buf[i];
        is_countable_char = false;
        if (byte >= 65 && byte <= 90)
        {
            is_countable_char = true;
            // char_offset = 65 + 26;
            char_offset = 65;
        }
        else if (byte >= 97 && byte <= 122)
        {
            is_countable_char = true;
            char_offset = 97;
        }
        else if (!has_reached_null_term)
        {
            if (byte == 0)
                has_reached_null_term = true;
            else if (byte < 32 || byte > 127)
                invalid_chars++;
        }

        if (is_countable_char)
        {
            countable_chars++;
            buffer_char_counts[byte - char_offset]++;
        }
    }

    if (countable_chars == 0) return -1.0;

    score = (double)invalid_chars;

    // for (i = 0; i < 52; i++)
    for (i = 0; i < 26; i++)
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

    bbuf_initTo(&key_buffer, buffer->len);

    winning.score = -1.0;
    current.key = 0;
    do
    {
        for (i = 0; i < key_buffer.len; i++)
            key_buffer.buf[i] = current.key;

        decoded_buffer = fixedXOR(buffer, &key_buffer);    
        current.score = scoreBufferDistance(&decoded_buffer);

        printf("%c (%u) %f\n", current.key, current.key, current.score);
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
