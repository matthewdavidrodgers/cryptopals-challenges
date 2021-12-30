#ifndef cpals_hex_h
#define cpals_hex_h

#include "bbuf.h"

void decode_hex_in_place(bbuf *bbuffer);
bbuf decode_hex(bbuf *bbuffer);
bbuf hex_to_bytes(char *hexStr);
char *bytes_to_hex(bbuf *bbuffer);

#endif
