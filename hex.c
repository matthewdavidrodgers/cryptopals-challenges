#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "hex.h"
#include "bytes.h"

#define HEX_CHARS 16

char hex_char_indexes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void decode_hex_in_place(bbuf *buffer)
{
    bool charIsFound, isUpperNibble;
    int byteAlignOffset, byteIndex, charIndex;
    size_t i;

    byteAlignOffset = buffer->len % 2;

    for (i = 0; i < buffer->len; i++)
    {
        isUpperNibble = (i+byteAlignOffset)%2 == 0;
        charIsFound = false;
        for (charIndex = 0; charIndex < HEX_CHARS; charIndex++)
        {
            if (buffer->buf[i] == hex_char_indexes[charIndex])
            {
                byteIndex = (i + byteAlignOffset) / 2;
                if (isUpperNibble)
                    buffer->buf[byteIndex] = (charIndex << 4);
                else
                    buffer->buf[byteIndex] |= charIndex;

                charIsFound = true;
                break;
            }
        }
        if (!charIsFound) {
            printf("Invalid source to decode as hex\n");
            exit(1);
        }
    }

    buffer->len = (buffer->len/2)+byteAlignOffset;
}

bbuf decode_hex(bbuf *encoded_buffer)
{
    bbuf decoded_buffer = bbuf_new();
    bool charIsFound, isUpperNibble;
    int byteAlignOffset, byteIndex, charIndex;
    size_t i;

    byteAlignOffset = encoded_buffer->len % 2;

    bbuf_init_to(&decoded_buffer, (encoded_buffer->len/2)+byteAlignOffset);

    for (i = 0; i < encoded_buffer->len; i++)
    {
        isUpperNibble = (i+byteAlignOffset)%2 == 0;
        charIsFound = false;
        for (charIndex = 0; charIndex < HEX_CHARS; charIndex++)
        {
            if (encoded_buffer->buf[i] == hex_char_indexes[charIndex])
            {
                byteIndex = (i + byteAlignOffset) / 2;
                if (isUpperNibble)
                    decoded_buffer.buf[byteIndex] |= (charIndex << 4);
                else
                    decoded_buffer.buf[byteIndex] |= charIndex;

                charIsFound = true;
            }
        }
        if (!charIsFound) {
            printf("Invalid source to decode as hex\n");
            exit(1);
        }
    }

    return decoded_buffer;
}

bbuf hex_to_bytes(char *hex_str)
{
    bbuf buffer = bbuf_new();

    buffer = from_ascii(hex_str);
    decode_hex_in_place(&buffer);

    return buffer;
}

// int hex_to_bytes(bbuf *bbuffer, char *hexStr)
// {
//     bool charIsFound, isUpperNibble;
//     int hexStrLen, byteAlignOffset, byteIndex, i, charIndex;
// 
//     hexStrLen = (int)strlen(hexStr);
//     byteAlignOffset = hexStrLen % 2;
// 
//     bbuf_init_to(bbuffer, (hexStrLen/2)+byteAlignOffset);
// 
//     for (i = 0; i < hexStrLen; i++)
//     {
//         isUpperNibble = (i+byteAlignOffset)%2 == 0;
//         charIsFound = false;
//         for (charIndex = 0; charIndex < HEX_CHARS; charIndex++)
//         {
//             if (hexStr[i] == hex_char_indexes[charIndex])
//             {
//                 byteIndex = (i + byteAlignOffset) / 2;
//                 if (isUpperNibble)
//                     bbuffer->buf[byteIndex] |= (charIndex << 4);
//                 else
//                     bbuffer->buf[byteIndex] |= charIndex;
// 
//                 charIsFound = true;
//             }
//         }
//         if (!charIsFound) return 1;
//     }
// 
//     return 0;
// }

char *bytes_to_hex(bbuf *buffer)
{
    char *encoded;
    size_t i;
    uint8_t byte;

    encoded = (char *)malloc((buffer->len * 2) + 1);
    if (!encoded) return NULL;

    for (i = 0; i < buffer->len; i++)
    {
        byte = buffer->buf[i]; 
        encoded[i*2] = hex_char_indexes[byte >> 4];
        encoded[(i*2)+1] = hex_char_indexes[byte & 0x0F];
    }
    encoded[i*2] = '\0';

    return encoded;
}
