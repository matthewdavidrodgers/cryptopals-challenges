#ifndef cpals_bbuf_h
#define cpals_bbuf_h

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t   cap;
    size_t   len;
    uint8_t *buf;
} bbuf;

typedef enum { BBUF_DECIMAL, BBUF_HEX, BBUF_GRID, BBUF_GRID_ASCII } bbuf_print_format;

bbuf bbuf_new();
void bbuf_init(bbuf *bbuffer);
void bbuf_init_to(bbuf *bbuffer, size_t size);
void bbuf_append(bbuf *bbuffer, uint8_t b);
bbuf bbuf_concat(bbuf *a, bbuf *b);
void bbuf_slice(bbuf *dst_bbuffer, bbuf *src_bbuffer, size_t start, size_t end);
void bbuf_destroy(bbuf *bbuffer);
void bbuf_print(bbuf *buffer, bbuf_print_format format);


#endif
