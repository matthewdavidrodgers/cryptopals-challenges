#include <stdlib.h>
#include <stdbool.h>

#include "base64.h"

char base64_char_indexes[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'Z', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'z', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

char *bytesToBase64(bbuf *bbuffer)
{
    char *encoded;
    uint8_t currByte, nextByte;
    int decodeState, charIndex;
    size_t encodedSize, encodedIndex, byteIndex;

    encodedSize = (bbuffer->len / 3) * 4;
    if (bbuffer->len % 3 != 0) encodedSize += 4;

    encoded = (char *)malloc(encodedSize + 1);
    if (!encoded) return NULL;

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
                    charIndex = ((currByte & 0x0F) << 2) | (nextByte >> 6);
                    byteIndex++;
                    break;
                }
            case 3:
                {
                    charIndex = currByte & 0x3F;
                    byteIndex++;
                    break;
                }
        }
        encoded[encodedIndex++] = base64_char_indexes[charIndex];

        if (decodeState != 0 && byteIndex == bbuffer->len) break;

        decodeState = (decodeState + 1) % 4;

        if (bbuffer->len % 3 == 2)
        {
            encoded[encodedIndex++] = '=';
        }
        else if (bbuffer->len % 3 == 1)
        {
            encoded[encodedIndex++] = '=';
            encoded[encodedIndex++] = '=';
        }
    }

    return encoded;
}
