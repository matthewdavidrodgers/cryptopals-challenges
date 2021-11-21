#ifndef cpals_hex_h
#define cpals_hex_h

#include "bbuf.h"

int hex_to_bytes(bbuf *bbuffer, char *hexStr);
char *bytes_to_hex(bbuf *bbuffer);

#endif
