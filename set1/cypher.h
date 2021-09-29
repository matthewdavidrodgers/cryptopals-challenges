#ifndef cpals_cypher_h
#define cpals_cypher_h

#include <stdint.h>

#include "bbuf.h"

typedef struct {
  uint8_t key;
  double  score;
} sb_xor_decode_details;

double scoreBufferDistance(bbuf *buffer);
sb_xor_decode_details decodeSingleByteXOR(bbuf *buffer);

#endif
