#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bytes.h"

bbuf fixedXOR(bbuf *a, bbuf *b)
{
    bbuf result;
    size_t i;

    bbuf_initTo(&result, a->len);

    for (i = 0; i < a->len; i++)
        result.buf[i] = a->buf[i] ^ b->buf[i];

    return result;
}

char *toString(bbuf *buffer)
{
    char *str = (char *)malloc(buffer->len + 1);
    memcpy(str, buffer->buf, buffer->len);
    str[buffer->len] = '\0';
    return str;
}
