#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "bytes.h"

void xor_in_place(bbuf *result, bbuf *a, bbuf *b)
{
    size_t a_i, b_i;

    b_i = 0;
    for (a_i = 0; a_i < a->len; a_i++)
    {
        result->buf[a_i] = a->buf[a_i] ^ b->buf[b_i];
        b_i = (b_i + 1) % b->len;
    }
}

bbuf xor(bbuf *a, bbuf *b)
{
    bbuf result = bbuf_new();
    bbuf_init_to(&result, a->len);

    xor_in_place(&result, a, b);

    return result;
}

bbuf from_ascii(char *ascii)
{
    bbuf result;
    size_t i, len;

    len = strlen(ascii);
    bbuf_init_to(&result, len);

    for (i = 0; i < len; i++)
        result.buf[i] = ascii[i];

    return result;
}

bbuf from_file(char *filename)
{
    FILE *data_file;
    bbuf buffer = bbuf_new();
    char *line = NULL;
    size_t line_len, i;
    ssize_t nread;

    data_file = fopen(filename, "r");
    if (!data_file) exit(1);

    bbuf_init(&buffer);

    while ((nread = getline(&line, &line_len, data_file)) != -1)
    {
        for (i = 0; i < (size_t)(nread - 1); i++)
            bbuf_append(&buffer, line[i]);
#ifndef OMIT_FILE_NEWLINE
        bbuf_append(&buffer, '\n');
#endif
    }

    fclose(data_file);
    free(line);

    return buffer;
}

size_t distance(bbuf *a, bbuf *b)
{
    size_t i, dis, len;
    uint8_t xored_byte;

    len = a->len > b->len ? b->len : a->len;

    dis = 0;
    for (i = 0; i < len; i++)
        for (xored_byte = a->buf[i] ^ b->buf[i]; xored_byte != 0; xored_byte >>= 1)
            dis += xored_byte & 0x1;

    return dis;
}

char *to_string(bbuf *buffer)
{
    char *str = (char *)malloc(buffer->len + 1);
    memcpy(str, buffer->buf, buffer->len);
    str[buffer->len] = '\0';
    return str;
}
