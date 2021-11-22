#ifndef cpals_bytes_h
#define cpals_bytes_h

#include "bbuf.h"

bbuf xor(bbuf *a, bbuf *b);
void xor_in_place(bbuf *result, bbuf *a, bbuf *b);
bbuf from_file(char *filename);
bbuf from_ascii(char *ascii);
size_t distance(bbuf *a, bbuf *b);
char *to_string(bbuf *buffer);

#endif
