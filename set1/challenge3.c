#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "bbuf.h"
#include "hex.h"
#include "bytes.h"
#include "cypher.h"

int main()
{
    sb_xor_decode_details decode_details;
    bbuf cyphertext_buffer, plaintext_buffer, key_buffer;
    char *cyphertext = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736", *plaintext;
    
    cyphertext_buffer = hex_to_bytes(cyphertext);

    decode_details = decode_sb_xor(&cyphertext_buffer);

    bbuf_init(&key_buffer);
    bbuf_append(&key_buffer, decode_details.key);

    plaintext_buffer = xor(&cyphertext_buffer, &key_buffer);
    bbuf_print(&plaintext_buffer, BBUF_DECIMAL);
    plaintext = to_string(&plaintext_buffer);

    printf("decoded using key: %c (%u)\nwith score %f\n%s\n", decode_details.key, decode_details.key, decode_details.score, plaintext);

    bbuf_destroy(&plaintext_buffer);
    bbuf_destroy(&cyphertext_buffer);
    bbuf_destroy(&key_buffer);
    free(plaintext);
}
