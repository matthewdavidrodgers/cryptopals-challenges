#ifndef cpals_bytes_h
#define cpals_bytes_h

#include "bbuf.h"

bbuf xor(bbuf *a, bbuf *b);
bbuf fromASCII(char *ascii);
char *toString(bbuf *buffer);

#endif
