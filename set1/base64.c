#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "base64.h"
#include "bytes.h"

char base64_char_indexes[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

bbuf encode_base64(bbuf *bbuffer)
{
    bbuf encoded = bbuf_new();
    uint8_t currByte, nextByte;
    int decodeState, charIndex;
    size_t encodedSize, encodedIndex, byteIndex;

    encodedSize = (bbuffer->len / 3) * 4;
    if (bbuffer->len % 3 != 0) encodedSize += 4;

    bbuf_init_to(&encoded, encodedSize);

    decodeState = 0;
    byteIndex = 0;
    encodedIndex = 0;

    while (true)
    {
        currByte = bbuffer->buf[byteIndex];
        nextByte = (byteIndex + 1 < bbuffer->len) ? bbuffer->buf[byteIndex+1] : 0;

        switch (decodeState)
        {
            case 0:
                {
                    charIndex = currByte >> 2;
                    break;
                }
            case 1:
                {
                    charIndex = ((currByte & 0x03) << 4) | (nextByte >> 4);
                    byteIndex++;
                    break;
                }
            case 2:
                {
                    charIndex = ((currByte & 0x0f) << 2)| (nextByte >> 6);
                    byteIndex++;
                    break;
                }
            case 3:
                {
                    charIndex = currByte & 0x3f;
                    byteIndex++;
                    break;
                }
        }
        encoded.buf[encodedIndex++] = base64_char_indexes[charIndex];

        if (decodeState != 0 && byteIndex == bbuffer->len) break;

        decodeState = (decodeState + 1) % 4;

        if (bbuffer->len % 3 == 2)
            encoded.buf[encodedIndex++] = '=';
        else if (bbuffer->len % 3 == 1)
        {
            encoded.buf[encodedIndex++] = '=';
            encoded.buf[encodedIndex++] = '=';
        }
    }

    return encoded;
}

char *encode_base64_str(bbuf *bbuffer)
{
    char *encoded;
    bbuf encoded_buffer = bbuf_new();

    encoded_buffer = encode_base64(bbuffer);
    encoded = to_string(&encoded_buffer);
    bbuf_destroy(&encoded_buffer);

    return encoded;
}

uint8_t base64CharValue(char c)
{
    uint8_t v = 0;
    while (v < 64 && base64_char_indexes[v] != c)
        v++;
    return v;
}

bbuf decode_base64(bbuf *encoded)
{
    bbuf decoded = bbuf_new();
    size_t decoded_size, i, char_offset;
    uint8_t byte;
    char a, b;

    decoded_size = (encoded->len / 4) * 3;
    if (encoded->len > 0 && encoded->buf[encoded->len - 1] == '=') decoded_size--;
    if (encoded->len > 1 && encoded->buf[encoded->len - 2] == '=') decoded_size--;

    bbuf_init_to(&decoded, decoded_size);

    for (i = 0; i < decoded_size; i++)
    {
        char_offset = (i / 3) * 4;
        if (i % 3 == 0)
        {
            a = encoded->buf[char_offset];
            b = encoded->buf[char_offset + 1];
            decoded.buf[i] = (base64CharValue(a) << 2) | (base64CharValue(b) >> 4);
        }
        else if (i % 3 == 1)
        {
            a = encoded->buf[char_offset + 1];
            b = encoded->buf[char_offset + 2];
            decoded.buf[i] = ((base64CharValue(a) & 0xF) << 4) | (base64CharValue(b) >> 2);
        }
        else
        {
            a = encoded->buf[char_offset + 2];
            b = encoded->buf[char_offset + 3];
            decoded.buf[i] = ((base64CharValue(a) & 0x3) << 6) | base64CharValue(b);
        }
    }

    return decoded;
}

bbuf decode_base64_str(char *encoded_str)
{
    bbuf encoded = bbuf_new(), decoded = bbuf_new();

    encoded = from_ascii(encoded_str);
    decoded = decode_base64(&encoded);

    bbuf_destroy(&encoded);
    return decoded;
}
