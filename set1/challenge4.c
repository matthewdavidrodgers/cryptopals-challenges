#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cypher.h"
#include "bbuf.h"
#include "bytes.h"
#include "hex.h"

#define DATA_FILE_NAME "challenge4.txt"

int main()
{
    FILE *data_file;
    char *line = NULL, *winning_line = NULL, *plaintext;
    ssize_t line_len = 0;
    size_t nread, i;
    bbuf buffer, cyphertext_buffer, key_buffer, plaintext_buffer;
    sb_xor_decode_details decode_details, winning_decode_details;

    data_file = fopen(DATA_FILE_NAME, "r");
    if (!data_file) exit(1);

    while ((nread = getline(&line, &line_len, data_file)) != -1)
    {
        line[nread-1] = '\0';
        hexToBytes(&buffer, line);

        decode_details = decodeSingleByteXOR(&buffer);
        // start
        printf("%s %c (%u) %f\n", line, decode_details.key, decode_details.key, decode_details.score);
        // end

        if (decode_details.score != -1 && (winning_line == NULL || decode_details.score < winning_decode_details.score))
        {
            if (winning_line == NULL)
                winning_line = (char *)malloc(nread-1);
            
            strcpy(winning_line, line);
            winning_decode_details = decode_details;
        }
    }

    hexToBytes(&cyphertext_buffer, winning_line);

    bbuf_initTo(&key_buffer, cyphertext_buffer.len);
    for (i = 0; i < key_buffer.len; i++)
        key_buffer.buf[i] = winning_decode_details.key;

    plaintext_buffer = fixedXOR(&cyphertext_buffer, &key_buffer);
    plaintext = toString(&plaintext_buffer);

    printf("decoded using key %c (%u)\n", winning_decode_details.key, winning_decode_details.key);
    printf("score %f\n", winning_decode_details.score);
    printf("from %s\n", winning_line);
    printf("to   %s\n", plaintext);

    fclose(data_file);
    bbuf_destroy(&buffer);
    bbuf_destroy(&cyphertext_buffer);
    bbuf_destroy(&key_buffer);
    bbuf_destroy(&plaintext_buffer);
    free(winning_line);
    free(line);
}
