#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../bbuf.h"
#include "../bytes.h"
#include "../base64.h"
#include "../cypher.h"

#define DATA_FILENAME "challenge7.txt"
#define KEY "YELLOW SUBMARINE"

// things needed
// - brew install openssl and manual makefile link (avoid symlink over libressl)
// - this page https://www.openssl.org/docs/manmaster/man3/EVP_CIPHER_CTX_new.html

int main() {
    bbuf encoded_cyphertext_b = bbuf_new(), cyphertext_b = bbuf_new(), key_b = bbuf_new(), plaintext_b = bbuf_new();
    char *plaintext;

    encoded_cyphertext_b = from_file(DATA_FILENAME);
    cyphertext_b = decode_base64(&encoded_cyphertext_b);
    bbuf_destroy(&encoded_cyphertext_b);

    key_b = from_ascii("YELLOW SUBMARINE");

    plaintext_b = aes_ecb(&cyphertext_b, &key_b, false);

    plaintext = to_string(&plaintext_b);
    printf("decoded to\n\"%s\"\n", plaintext);

    free(plaintext);
    bbuf_destroy(&plaintext_b);
    bbuf_destroy(&cyphertext_b);
    bbuf_destroy(&key_b);
}
