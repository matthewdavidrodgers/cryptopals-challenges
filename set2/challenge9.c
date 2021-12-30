#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../bbuf.h"
#include "../bytes.h"

int main()
{
    bbuf buffer = from_ascii("YELLOW SUBMARINE");
    assert(buffer.len == 16);

    pad_for_blocksize(&buffer, 20);

    assert(buffer.len == 20);
    assert(buffer.buf[buffer.len - 1] == 4);
    assert(buffer.buf[buffer.len - 2] == 4);
    assert(buffer.buf[buffer.len - 3] == 4);
    assert(buffer.buf[buffer.len - 4] == 4);

    bbuf_print(&buffer, BBUF_GRID);
}
