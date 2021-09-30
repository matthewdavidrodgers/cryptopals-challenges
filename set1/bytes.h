#ifndef cpals_bytes_h
#define cpals_bytes_h

#include "bbuf.h"

bbuf xor(bbuf *a, bbuf *b);
bbuf fromASCII(char *ascii);
size_t distance(bbuf *a, bbuf *b);
char *toString(bbuf *buffer);

#endif
