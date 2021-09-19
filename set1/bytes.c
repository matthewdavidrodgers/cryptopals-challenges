#include <stddef.h>

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
