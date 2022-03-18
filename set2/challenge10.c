#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../bbuf.h"
#include "../bytes.h"
#include "../base64.h"
#include "../cypher.h"

#define DATA_FILENAME "challenge10.txt"

int main()
{
    bbuf encoded_cyphertext_b = bbuf_new(),
         cyphertext_b = bbuf_new(),
         key_b = bbuf_new(),
         iv_b = bbuf_new(),
         plaintext_b = bbuf_new();
    char *plaintext;

    encoded_cyphertext_b = from_file(DATA_FILENAME);
    cyphertext_b = decode_base64(&encoded_cyphertext_b);
    bbuf_destroy(&encoded_cyphertext_b);

    key_b = from_ascii("YELLOW SUBMARINE");
    bbuf_init_to(&iv_b, 16); // already zeroed by init

    plaintext_b = aes_cbc(&cyphertext_b, &key_b, &iv_b, false);

    plaintext = to_string(&plaintext_b);
    printf("decoded to \n\"%s\"\n", plaintext);

    free(plaintext);
    bbuf_destroy(&plaintext_b);
    bbuf_destroy(&key_b);
    bbuf_destroy(&iv_b);
    bbuf_destroy(&cyphertext_b);
}
