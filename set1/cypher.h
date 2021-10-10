#ifndef cpals_cypher_h
#define cpals_cypher_h

#include <stdint.h>

#include "bbuf.h"

typedef struct {
  uint8_t key;
  double  score;
} sb_xor_decode_details;

typedef struct keysize_t {
  size_t keysize;
  double score;
} keysize_t;

double scoreBufferDistance(bbuf *buffer);
void pick_rk_xor_keysizes(bbuf *cyphertext, keysize_t *keysizes, size_t count);
bbuf *break_and_transpose_buff(bbuf *buffer, size_t blocksize);
uint8_t pick_uniform_xor_byte(bbuf *buffer);
sb_xor_decode_details decodeSingleByteXOR(bbuf *buffer);

#endif
