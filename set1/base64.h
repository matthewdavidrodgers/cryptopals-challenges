#ifndef cpals_base64_h
#define cpals_base64_h

#include "bbuf.h"

bbuf encode_base64(bbuf *buffer);
char *encode_base64_str(bbuf *buffer);
bbuf decode_base64(bbuf *encoded);
bbuf decode_base64_str(char *encoded_str);

#endif
