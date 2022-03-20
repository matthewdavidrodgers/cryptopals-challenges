#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "../bbuf.h"
#include "../bytes.h"
#include "../cypher.h"
#include "../base64.h"

#define DATA_FILENAME "challenge6.txt"

int main()
{
    bbuf base64_cyphertext_b = bbuf_new(), cyphertext_b = bbuf_new();
    xor_decode_details result;
    char *plaintext, *keytext;

    base64_cyphertext_b = from_file(DATA_FILENAME);
    cyphertext_b = decode_base64(&base64_cyphertext_b);
    bbuf_destroy(&base64_cyphertext_b);

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
