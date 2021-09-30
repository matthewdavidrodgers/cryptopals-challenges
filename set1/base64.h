#ifndef cpals_base64_h
#define cpals_base64_h

#include "bbuf.h"

char *bytesToBase64(bbuf *buffer);
bbuf base64ToBytes(char *base64);

#endif
