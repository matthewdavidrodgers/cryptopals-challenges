#ifndef cpals_cypher_h
#define cpals_cypher_h

#include <stdint.h>
#include <stdbool.h>

#include "bbuf.h"

typedef struct sb_xor_decode_details {
  uint8_t key;
  double  score;
} sb_xor_decode_details;

typedef struct xor_decode_details {
  bbuf   key_buffer;
  bbuf   plaintext_buffer;
  double score;
} xor_decode_details;

typedef struct keysize_t {
  size_t keysize;
  double score;
} keysize_t;

double score_buffer_as_english(bbuf *buffer);
size_t pick_rk_xor_keysizes(bbuf *cyphertext, keysize_t *keysizes, size_t count);
bbuf *break_and_transpose_buff(bbuf *buffer, size_t blocksize);
sb_xor_decode_details decode_sb_xor(bbuf *buffer);
void decode_sb_xor_ranked(bbuf *buffer, sb_xor_decode_details *top, uint8_t top_taken);
xor_decode_details decode_rk_xor_for_size(bbuf *buffer, size_t keysize);
xor_decode_details decode_rk_xor(bbuf *buffer);
bbuf aes_ecb(bbuf *input, bbuf *key, bool encrypt);
bbuf aes_cbc(bbuf *input, bbuf *key, bbuf *iv, bool encrypt);
bbuf encryption_oracle(bbuf *plaintext);

#endif
