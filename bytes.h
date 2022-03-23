#ifndef cpals_bytes_h
#define cpals_bytes_h

#include "bbuf.h"

bbuf xor(bbuf *a, bbuf *b);
void xor_in_place(bbuf *result, bbuf *a, bbuf *b);
bbuf from_file(char *filename);
bbuf from_ascii(char *ascii);
bbuf from_rand_bytes(size_t num_bytes);
bbuf from_rand_rand_bytes(size_t min, size_t max);
void pad_for_blocksize(bbuf *b, size_t blocksize);
void pad_for_blocksize(bbuf *b, size_t blocksize);
size_t dupe_blocks(bbuf *buffer, size_t blocksize);
size_t distance(bbuf *a, bbuf *b);
char *to_string(bbuf *buffer);

#endif
