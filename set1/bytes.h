#ifndef cpals_bytes_h
#define cpals_bytes_h

#include "bbuf.h"

bbuf fixedXOR(bbuf *a, bbuf *b);
char *toString(bbuf *buffer);

#endif
