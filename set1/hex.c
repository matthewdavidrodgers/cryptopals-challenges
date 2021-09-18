#include <string.h>
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
