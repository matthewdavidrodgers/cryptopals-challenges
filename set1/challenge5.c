#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../bbuf.h"
#include "../bytes.h"
#include "../hex.h"

int main()
{
    bbuf plaintext_buffer, cyphertext_buffer, key_buffer;
    char *plaintext = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    char *key = "ICE";
    char *cyphertext;

    plaintext_buffer = from_ascii(plaintext);
    key_buffer = from_ascii(key);

    cyphertext_buffer = xor(&plaintext_buffer, &key_buffer);
    cyphertext = bytes_to_hex(&cyphertext_buffer);

    printf("result: \n%s\n", cyphertext);

    bbuf_destroy(&plaintext_buffer);
    bbuf_destroy(&cyphertext_buffer);
    bbuf_destroy(&key_buffer);
    free(cyphertext);
}
