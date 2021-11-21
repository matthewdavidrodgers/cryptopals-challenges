#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "common.h"
#include "bbuf.h"
#include "bytes.h"
#include "cypher.h"
#include "base64.h"

#define DATA_FILENAME "challenge6.txt"

bbuf base64_file_cyphertext(char *filename)
{
    FILE *data_file;
    bbuf encoded_cyphertext_b = bbuf_new(), cyphertext_b = bbuf_new();
    char *line = NULL;
    size_t line_len, size, i;
    ssize_t nread;

    data_file = fopen(filename, "r");
    if (!data_file) exit(1);

    bbuf_init(&encoded_cyphertext_b);

    while ((nread = getline(&line, &line_len, data_file)) != -1)
    {
        for (i = 0; i < nread - 1; i++)
            bbuf_append(&encoded_cyphertext_b, line[i]);
#ifndef OMIT_FILE_NEWLINE
        bbuf_append(&encoded_cyphertext_b, '\n');
#endif
    }

#ifdef DEBUG_VERBOSE
    bbuf_print(&encoded_cyphertext_b, BBUF_GRID);
#endif

    cyphertext_b = decode_base64(&encoded_cyphertext_b);

    bbuf_destroy(&encoded_cyphertext_b);
    fclose(data_file);
    free(line);

    return cyphertext_b;
}

bbuf simple_cyphertext(char *plaintext, char *keytext)
{
    bbuf cyphertext_b = bbuf_new(), plaintext_b = bbuf_new(), keytext_b = bbuf_new();

    plaintext_b = from_ascii(plaintext);
    keytext_b = from_ascii(keytext);

    cyphertext_b = xor(&plaintext_b, &keytext_b);
    
    bbuf_destroy(&plaintext_b);
    bbuf_destroy(&keytext_b);

    return cyphertext_b;
}

int main()
{
    bbuf cyphertext_b = bbuf_new(), plaintext_b = bbuf_new(), keytext_b = bbuf_new();
    xor_decode_details result;
    char *plaintext, *keytext;

    cyphertext_b = base64_file_cyphertext(DATA_FILENAME);
    // cyphertext_b = simple_cyphertext("Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal", "ICE");
    // cyphertext_b = simple_cyphertext("secretattack", "$^!");

#ifdef DEBUG_VERBOSE
    bbuf_print(&cyphertext_b, BBUF_HEX);
#endif

#ifdef DECODE_KNOWN_KEYSIZE
    result = decode_rk_xor_for_size(&cyphertext_b, DECODE_KNOWN_KEYSIZE);
#else
    result = decode_rk_xor(&cyphertext_b);
#endif

    keytext = to_string(&result.key_buffer);
    plaintext = to_string(&result.plaintext_buffer);

    printf("decoded using key\n\"%s\"\nto\n\"%s\" with score %f\n", keytext, plaintext, result.score);
#ifdef DEBUG_VERBOSE
    printf("\tkey: ");
    bbuf_print(&result.key_buffer, BBUF_HEX);
    printf("\tplaintext: ");
    bbuf_print(&result.plaintext_buffer, BBUF_HEX);
#endif

    free(keytext);
    free(plaintext);
    bbuf_destroy(&result.key_buffer);
    bbuf_destroy(&result.plaintext_buffer);

    bbuf_destroy(&cyphertext_b);
    return 0;
}
