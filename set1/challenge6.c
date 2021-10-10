#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "bbuf.h"
#include "bytes.h"
#include "cypher.h"

#define DATA_FILENAME "challenge6.txt"
#define KEYSIZES_TAKEN 4

int main()
{
    FILE *data_file;
    bbuf cyphertext;
    bbuf *buffer_blocks;
    char *line = NULL;
    uint8_t *key;
    size_t line_len, size, i, j;
    ssize_t nread;

    keysize_t best_sizes[KEYSIZES_TAKEN];

    data_file = fopen(DATA_FILENAME, "r");
    if (!data_file) exit(1);

    bbuf_init(&cyphertext);

    while ((nread = getline(&line, &line_len, data_file)) != -1)
        for (i = 0; i < nread - 1; i++)
            bbuf_append(&cyphertext, line[i]);

    pick_rk_xor_keysizes(&cyphertext, best_sizes, KEYSIZES_TAKEN);

    for (i = 0; i < KEYSIZES_TAKEN; i++)
    {
        size = best_sizes[i].keysize;
        buffer_blocks = break_and_transpose_buff(&cyphertext, size);
        key = (uint8_t *)malloc(size);

        printf("key for size %lu: ", size);
        for (j = 0; j < size; j++)
        {
            key[j] = pick_uniform_xor_byte(&buffer_blocks[j]);
            printf("%c", key[j]);
            bbuf_destroy(&buffer_blocks[j]);
        }
        printf("\n");

        free(key);
        free(buffer_blocks);
    }

    free(line);
    bbuf_destroy(&cyphertext);
    fclose(data_file);
}
