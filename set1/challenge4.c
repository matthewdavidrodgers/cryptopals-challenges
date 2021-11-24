#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"
#include "hex.h"

#define DATA_FILE_NAME "challenge4.txt"

int main()
{
    FILE *data_file;
    char *line = NULL, *winning_line = NULL, *plaintext;
    size_t line_len = 0;
    ssize_t nread;
    bbuf buffer, cyphertext_buffer, key_buffer, plaintext_buffer;
    sb_xor_decode_details decode_details, winning_decode_details;

    data_file = fopen(DATA_FILE_NAME, "r");
    if (!data_file) exit(1);

    while ((nread = getline(&line, &line_len, data_file)) != -1)
    {
        line[nread-1] = '\0';
        buffer = hex_to_bytes(line);

        decode_details = decode_sb_xor(&buffer);

        if (decode_details.score != -1 && (winning_line == NULL || decode_details.score < winning_decode_details.score))
        {
            if (winning_line == NULL)
                winning_line = (char *)malloc(nread-1);
            
            strcpy(winning_line, line);
            winning_decode_details = decode_details;
        }
        bbuf_destroy(&buffer);
    }

    cyphertext_buffer = hex_to_bytes(winning_line);

    bbuf_init(&key_buffer);
    bbuf_append(&key_buffer, winning_decode_details.key);

    plaintext_buffer = xor(&cyphertext_buffer, &key_buffer);
    plaintext = to_string(&plaintext_buffer);

    printf("decoded using key %c (%u)\n", winning_decode_details.key, winning_decode_details.key);
    printf("score %f\n", winning_decode_details.score);
    printf("from %s\n", winning_line);
    printf("to   %s\n", plaintext);

#ifdef DEBUG_VERBOSE
    bbuf_print(&plaintext_buffer, BBUF_GRID_ASCII);
#endif

    fclose(data_file);
    bbuf_destroy(&cyphertext_buffer);
    bbuf_destroy(&key_buffer);
    bbuf_destroy(&plaintext_buffer);
    free(winning_line);
    free(line);
}
