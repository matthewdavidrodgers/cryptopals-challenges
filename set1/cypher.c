#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"

double avg_char_frequencies[] = {0.082, 0.015, 0.028, 0.043, 0.13, 0.022, 0.02, 0.061, 0.07, 0.0015, 0.0077, 0.04, 0.024, 0.067, 0.075, 0.019, 0.00095, 0.06, 0.063, 0.091, 0.028, 0.0098, 0.024, 0.0015, 0.02, 0.00074};

double scoreBufferDistance(bbuf *buffer)
{
    int buffer_char_counts[26], countable_chars;
    double buffer_char_frequencies[26], score, char_score;
    bool is_countable_char;
    uint8_t byte;
    size_t i, char_offset;

    for (i = 0; i < 26; i++)
        buffer_char_frequencies[i] = buffer_char_counts[i] = 0;

    for (i = 0; i < buffer->len; i++)
    {
        byte = buffer->buf[i];
        is_countable_char = false;
        if (byte >= 65 && byte <= 90)
        {
            is_countable_char = true;
            char_offset = 65;
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

    for (i = 0; i < 26; i++)
    {
        buffer_char_frequencies[i] = buffer_char_counts[i] / (double)countable_chars;
        char_score = (avg_char_frequencies[i] - buffer_char_frequencies[i]) / 2.0;
        score += (char_score < 0) ? (-1*char_score) : char_score;
    }

    score *= (buffer->len - countable_chars);

    return score;
}

uint8_t decodeSingleByteXOR(bbuf *buffer)
{
    bbuf key_buffer, decoded_buffer;
    uint8_t key, winning_key;
    double score, winning_score;
    size_t i;

    bbuf_initTo(&key_buffer, buffer->len);

    winning_score = -1.0;
    for (key = 0; key < 255; key++)
    {
        for (i = 0; i < key_buffer.len; i++)
            key_buffer.buf[i] = key;
    
        decoded_buffer = fixedXOR(buffer, &key_buffer);    
        score = scoreBufferDistance(&decoded_buffer);

        if (score != -1.0 && (winning_score == -1.0 || score < winning_score))
        {
            winning_score = score;
            winning_key = key;
        }

        bbuf_destroy(&decoded_buffer);
    }

    bbuf_destroy(&key_buffer);

    return winning_key;
}
