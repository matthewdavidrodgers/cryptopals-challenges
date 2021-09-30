#include <stdio.h>
#include <stddef.h>

#include "bbuf.h"
#include "bytes.h"

int main()
{
    bbuf a, b;
    size_t dis;

    a = fromASCII("this is a test");
    b = fromASCII("wokka wokka!!!");
    dis = distance(&a, &b);

    printf("distance: %lu\n", dis);
}
