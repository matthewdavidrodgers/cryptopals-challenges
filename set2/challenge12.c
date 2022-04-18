#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../bbuf.h"
#include "../bytes.h"
#include "../base64.h"
#include "../cypher.h"

#define UNKNOWN_CONTENT_ENCODED "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK"
#define KEYSIZE 16

int main()
{
    char *plaintext;
    size_t i, byte_index, target_block, pad_len;
    uint8_t byte;
    bbuf unknown_content_encoded = bbuf_new(),
         unknown_content = bbuf_new(),
         unknown_encrypted = bbuf_new(),
         decoded = bbuf_new(),
         oracle_pad = bbuf_new(),
         oracle_output = bbuf_new(),
         oracle_output_block = bbuf_new(),
         oracle_guess_prefix = bbuf_new(),
         oracle_guess_result = bbuf_new();

    bbuf_init(&decoded);
    bbuf_init(&oracle_pad);

    unknown_content_encoded = from_ascii(UNKNOWN_CONTENT_ENCODED);
    unknown_content = decode_base64(&unknown_content_encoded);
    bbuf_destroy(&unknown_content_encoded);

    simple_oracle_setup(unknown_content);
    unknown_encrypted = simple_oracle(&decoded); // dummy empty bbuf

    for (byte_index = 0; byte_index < unknown_encrypted.len; byte_index++)
    {
        target_block = byte_index / KEYSIZE;

        pad_len = KEYSIZE - 1 - (byte_index % KEYSIZE);
        bbuf_init_to(&oracle_pad, pad_len);
        for (i = 0; i < pad_len; i++)
            oracle_pad.buf[i] = (uint8_t)'X';
        
        oracle_output = simple_oracle(&oracle_pad);
        bbuf_slice(&oracle_output_block, &oracle_output, target_block*KEYSIZE, (target_block+1)*KEYSIZE);
        bbuf_destroy(&oracle_output);

        bbuf_init_to(&oracle_guess_prefix, 16);
        if (decoded.len < 15)
        {
            for (i = 0; i < 15 - decoded.len; i++)
                oracle_guess_prefix.buf[i] = (uint8_t)'X';
            for (i = 0; i < decoded.len; i++)
                oracle_guess_prefix.buf[15 - decoded.len + i] = decoded.buf[i];
        }
        else
            for (i = 0; i < 15; i++)
                oracle_guess_prefix.buf[i] = decoded.buf[decoded.len-15+i];

        byte = 0;
        do
        {
            oracle_guess_prefix.buf[15] = byte;
            oracle_guess_result = simple_oracle(&oracle_guess_prefix);
            if (memcmp(oracle_output_block.buf, oracle_guess_result.buf, KEYSIZE) == 0)
            {
                bbuf_append(&decoded, byte);
                break;
            }

            byte++;
        } while (byte > 0);

        bbuf_destroy(&oracle_pad);
        bbuf_destroy(&oracle_output);
        bbuf_destroy(&oracle_output_block);
        bbuf_destroy(&oracle_guess_prefix);
        bbuf_destroy(&oracle_guess_result);
    }

    plaintext = to_string(&decoded);

    printf("%s\n", plaintext);

    bbuf_destroy(&decoded);
    bbuf_destroy(&unknown_encrypted);
    free(plaintext);
    simple_oracle_teardown();
}
