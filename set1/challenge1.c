#include <stdio.h>
#include <stdlib.h>

#include "bbuf.h"
#include "base64.h"
#include "hex.h"

int main()
{
    char *base64_str;
    char *hex_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    bbuf buffer;

    hexToBytes(&buffer, hex_str);
    base64_str = bytesToBase64(&buffer);

    bbuf_print(&buffer);
    printf("%s\n", base64_str);

    bbuf_destroy(&buffer);
    free(base64_str);
}
