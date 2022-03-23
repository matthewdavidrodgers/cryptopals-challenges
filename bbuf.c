#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bbuf.h"

#define INIT_BUFSIZE 8

bbuf bbuf_new()
{
    bbuf buffer;

    buffer.len = 0;
    buffer.cap = 0;
    buffer.buf = NULL;

    return buffer;
}

void bbuf_init(bbuf *bbuffer)
{
    bbuffer->len = 0;
    bbuffer->cap = INIT_BUFSIZE;
    bbuffer->buf = (uint8_t *)malloc(INIT_BUFSIZE);

    if (!bbuffer->buf)
    {
        printf("Allocation failed; could not allocate %lu bytes\n", bbuffer->cap);
        exit(1);
    }
}

void bbuf_init_to(bbuf *bbuffer, size_t size)
{
    size_t i;

    bbuffer->len = size;
    bbuffer->cap = size;
    bbuffer->buf = (uint8_t *)malloc(size);

    if (!bbuffer->buf)
    {
        printf("Allocation failed; could not allocate %lu bytes\n", size);
        exit(1);
    }

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

bbuf bbuf_concat(bbuf *a, bbuf *b)
{
    bbuf output = bbuf_new();
    
    bbuf_init_to(&output, a->len + b->len);

    memcpy(output.buf, a->buf, a->len);
    memcpy(output.buf + a->len, b->buf, b->len);

    return output;
}

void bbuf_slice(bbuf *dst, bbuf *src, size_t start, size_t end)
{
    size_t i;

    if (dst->cap < end-start)
        bbuf_destroy(dst);
    if (!dst->buf)
        bbuf_init_to(dst, end-start);
    
    for (i = 0; i < end-start; i++)
        dst->buf[i] = src->buf[i+start];
    if (dst->len != end-start)
        dst->len = end-start;
}

void bbuf_destroy(bbuf *bbuffer)
{
    free(bbuffer->buf);
    bbuffer->buf = NULL;
    bbuffer->len = 0;
    bbuffer->cap = 0;
}

void bbuf_print(bbuf *bbuffer, bbuf_print_format format)
{
    size_t i, block, block_end;

    if (format == BBUF_GRID || format == BBUF_GRID_ASCII)
    {
        block = 0;
        while (block * 0x10 < bbuffer->len)
        {
            block_end = (block + 1) * 0x10 < bbuffer->len ? (block + 1) * 0x10 : bbuffer->len;
            printf("%07lx", block * 0x10);

            for (i = block * 0x10; i < block_end; i++)
                printf(" %02x", bbuffer->buf[i]);
            if (format == BBUF_GRID_ASCII)
            {
                printf("\n       ");
                for (i = block * 0x10; i < block_end; i++)
                {
                    if (isprint(bbuffer->buf[i]))
                        printf("  %c", bbuffer->buf[i]);
                    else if (bbuffer->buf[i] == 0x0a)
                        printf(" \\n");
                    else
                        printf("  @");
                }
            }
            printf("\n");

            block++;
        }
    }
    else
    {
        printf("[");
        for (i = 0; i < bbuffer->len; i++)
        {
            if (i > 0) printf(format == BBUF_HEX ? ",%x" : ",%u", bbuffer->buf[i]);
            else printf(format == BBUF_HEX ? "%x" : "%u", bbuffer->buf[i]);
        }
        printf("] (len %zu, cap %zu)\n", bbuffer->len, bbuffer->cap);
    }
}
