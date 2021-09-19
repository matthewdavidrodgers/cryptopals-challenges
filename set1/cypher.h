#ifndef cpals_cypher_h
#define cpals_cypher_h

#include <stdint.h>

#include "bbuf.h"

double scoreBufferDistance(bbuf *buffer);
uint8_t decodeSingleByteXOR(bbuf *buffer);

#endif
