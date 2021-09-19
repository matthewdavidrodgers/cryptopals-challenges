#include <stdio.h>
#include <stdlib.h>

#include "bbuf.h"
#include "bytes.h"
#include "hex.h"

int main()
{
    bbuf buffer_a, buffer_b;
    char *encoded, *hex_str_a = "1c0111001f010100061a024b53535009181c", *hex_str_b = "686974207468652062756c6c277320657965";

    hexToBytes(&buffer_a, hex_str_a);
    hexToBytes(&buffer_b, hex_str_b);

    bbuf buffer_result = fixedXOR(&buffer_a, &buffer_b);

    encoded = bytesToHex(&buffer_result);
    printf("encoded result: %s\n", encoded);

    bbuf_destroy(&buffer_a);
    bbuf_destroy(&buffer_b);
    bbuf_destroy(&buffer_result);
    free(encoded);
}
