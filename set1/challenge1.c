#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "bbuf.h"
#include "base64.h"
#include "hex.h"

int main()
{
    char *base64_str;
    char *hex_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    char *expected = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    bbuf buffer;

    hex_to_bytes(&buffer, hex_str);
    base64_str = encode_base64_str(&buffer);

    bbuf_print(&buffer, BBUF_DECIMAL);
    printf("expected:\t%s\ngot:\t\t%s\n", expected, base64_str);
    assert(strcmp(base64_str, expected) == 0);

    bbuf_destroy(&buffer);
    free(base64_str);
}
