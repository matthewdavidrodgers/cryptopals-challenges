#include <stdlib.h>
#include <stdio.h>

#include "bbuf.h"

#define INIT_BUFSIZE 8

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
