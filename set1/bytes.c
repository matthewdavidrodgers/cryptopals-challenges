#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bytes.h"

bbuf xor(bbuf *a, bbuf *b)
{
    bbuf result;
    size_t a_i, b_i;

    bbuf_initTo(&result, a->len);

    b_i = 0;
    for (a_i = 0; a_i < a->len; a_i++)
    {
        result.buf[a_i] = a->buf[a_i] ^ b->buf[b_i];
        b_i = (b_i + 1) % b->len;
    }

    return result;
}

bbuf fromASCII(char *ascii)
{
    bbuf result;
    size_t i, len;

    len = strlen(ascii);
    bbuf_initTo(&result, len);

    for (i = 0; i < len; i++)
        result.buf[i] = ascii[i];

    return result;
}

size_t distance(bbuf *a, bbuf *b)
{
    size_t i, dis, len;
    uint8_t xored_byte;

    if (a->len > b->len)
    {
        len = a->len;
        dis = (a->len - b->len) * 8;
    }
    else
    {
        len = b->len;
        dis = (b->len - a->len) * 8;
    }

    dis = 0;
    for (i = 0; i < len; i++)
        for (xored_byte = a->buf[i] ^ b->buf[i]; xored_byte != 0; xored_byte >>= 1)
            dis += xored_byte & 0x1;

    return dis;
}

char *toString(bbuf *buffer)
{
    char *str = (char *)malloc(buffer->len + 1);
    memcpy(str, buffer->buf, buffer->len);
    str[buffer->len] = '\0';
    return str;
}
