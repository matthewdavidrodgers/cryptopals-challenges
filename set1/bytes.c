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

char *toString(bbuf *buffer)
{
    char *str = (char *)malloc(buffer->len + 1);
    memcpy(str, buffer->buf, buffer->len);
    str[buffer->len] = '\0';
    return str;
}
