#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../common.h"
#include "../bbuf.h"
#include "../bytes.h"
#include "../hex.h"

#define DATA_FILENAME "challenge8.txt"
#define BLOCKSIZE 16

int main()
{
    FILE *data_file;
    bbuf buffer = bbuf_new(), best_buffer = bbuf_new();
    char *line = NULL;
    size_t line_len, dupes, highest_dupes;
    ssize_t nread;

    data_file = fopen(DATA_FILENAME, "r");
    if (!data_file) exit(1);

    bbuf_init(&buffer);

    highest_dupes = 0;

    while ((nread = getline(&line, &line_len, data_file)) != -1)
    {
        if (buffer.len != (size_t)(nread - 1))
            bbuf_init_to(&buffer, (size_t)(nread - 1));

        memcpy(buffer.buf, line, (size_t)(nread - 1));
        decode_hex_in_place(&buffer);

        dupes = dupe_blocks(&buffer, BLOCKSIZE);
        if (dupes > highest_dupes)
        {
            highest_dupes = dupes;
            bbuf_slice(&best_buffer, &buffer, 0, buffer.len);
        }
    }

    printf("best buffer has %lu duplicate blocks\n", highest_dupes);
    bbuf_print(&best_buffer, BBUF_GRID);
     
    free(line);
    bbuf_destroy(&buffer);
    bbuf_destroy(&best_buffer);
}
