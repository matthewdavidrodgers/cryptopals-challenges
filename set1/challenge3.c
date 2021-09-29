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
    size_t i;
    
    hexToBytes(&cyphertext_buffer, cyphertext);

    decode_details = decodeSingleByteXOR(&cyphertext_buffer);

    bbuf_initTo(&key_buffer, cyphertext_buffer.len);

    for (i = 0; i < key_buffer.len; i++)
        key_buffer.buf[i] = decode_details.key;

    plaintext_buffer = fixedXOR(&cyphertext_buffer, &key_buffer);
    bbuf_print(&plaintext_buffer);
    plaintext = toString(&plaintext_buffer);

    printf("decoded using key: %c (%u)\nwith score %f\n%s\n", decode_details.key, decode_details.key, decode_details.score, plaintext);

    bbuf_destroy(&plaintext_buffer);
    bbuf_destroy(&cyphertext_buffer);
    bbuf_destroy(&key_buffer);
    free(plaintext);
}
