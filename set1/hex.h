#ifndef cpals_hex_h
#define cpals_hex_h

#include "bbuf.h"

int hexToBytes(bbuf *bbuffer, char *hexStr);
char *bytesToHex(bbuf *bbuffer);

#endif
