#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define INIT_BUFSIZE 8
#define HEX_CHARS 16
#define BASE64_CHARS 64

char hex_char_indexes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
char base64_char_indexes[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'Z', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'z', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

typedef struct {
    size_t   cap;
    size_t   len;
    uint8_t *buf;
} bbuf;

void bbuf_init(bbuf *bbuffer)
{
    bbuffer->len = 0;
    bbuffer->cap = INIT_BUFSIZE;
    bbuffer->buf = (uint8_t *)malloc(INIT_BUFSIZE);
}

void bbuf_initTo(bbuf *bbuffer, size_t size)
{
    size_t i;

    bbuffer->len = size;
    bbuffer->cap = size;
    bbuffer->buf = (uint8_t *)malloc(size);

    for (i = 0; i < bbuffer->len; i++)
        bbuffer->buf[i] = 0;
}

void bbuf_append(bbuf *bbuffer, uint8_t b)
{
    if (bbuffer->len == bbuffer->cap)
    {
        bbuffer->cap *= 2;
        bbuffer->buf = (uint8_t *)realloc(bbuffer->buf, bbuffer->cap);
    }
    bbuffer->buf[bbuffer->len++] = b;
}

void bbuf_destroy(bbuf *bbuffer)
{
    free(bbuffer->buf);
}

void bbuf_print(bbuf *bbuffer) 
{
    size_t i;

    printf("[");
    for (i = 0; i < bbuffer->len; i++)
    {
        if (i > 0) printf(",%u",bbuffer->buf[i]);
        else printf("%u",bbuffer->buf[i]);
    }
    printf("] (len %zu, cap %zu)\n", bbuffer->len, bbuffer->cap);
}

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

int main()
{
    char *base64_str;
    char *hex_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    bbuf buffer;

    hexToBytes(&buffer, hex_str);
    base64_str = bytesToBase64(&buffer);

    bbuf_print(&buffer);
    printf("%s\n", base64_str);

    bbuf_destroy(&buffer);
    free(base64_str);
}
