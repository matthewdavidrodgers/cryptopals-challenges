#include <stdio.h>

#include "../common.h"
#include "../bbuf.h"
#include "../bytes.h"
#include "../cypher.h"

int main()
{
    bbuf buffer = bbuf_new(), encrypted = bbuf_new();

    buffer = from_ascii("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    encrypted = encryption_oracle(&buffer);

#ifdef ORACLE_DEBUG
    printf("encrypted cyphertext:\n");
    bbuf_print(&encrypted, BBUF_GRID);
#endif

    if (dupe_blocks(&encrypted, 16))
        printf("ENCRYPTED WITH ECB\n");
    else
        printf("ENCRYPTED WITH CBC\n");
    
    bbuf_destroy(&buffer);
    bbuf_destroy(&encrypted);
}
