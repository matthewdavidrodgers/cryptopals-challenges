#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hex.h"

#define HEX_CHARS 16

char hex_char_indexes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

int hexToBytes(bbuf *bbuffer, char *hexStr)
{
    bool charIsFound, isUpperNibble;
    int hexStrLen, byteAlignOffset, byteIndex, i, charIndex;

    hexStrLen = (int)strlen(hexStr);
    byteAlignOffset = hexStrLen % 2;

    bbuf_initTo(bbuffer, (hexStrLen/2)+byteAlignOffset);

    for (i = 0; i < hexStrLen; i++)
    {
        isUpperNibble = (i+byteAlignOffset)%2 == 0;
        charIsFound = false;
        for (charIndex = 0; charIndex < HEX_CHARS; charIndex++)
        {
            if (hexStr[i] == hex_char_indexes[charIndex])
            {
                byteIndex = (i + byteAlignOffset) / 2;
                if (isUpperNibble)
                    bbuffer->buf[byteIndex] |= (charIndex << 4);
                else
                    bbuffer->buf[byteIndex] |= charIndex;

                charIsFound = true;
            }
        }
        if (!charIsFound) return 1;
    }

    return 0;
}

char *bytesToHex(bbuf *buffer)
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
