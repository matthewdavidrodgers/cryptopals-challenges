CC = clang
CFLAGS = -g -Wall -Wextra
LINK_OPENSSL = -L/usr/local/Cellar/openssl@3/3.0.0_1/lib -I/usr/local/Cellar/openssl@3/3.0.0_1/include -lssl -lcrypto

FILENAMES = bytes base64 bbuf hex cypher
HEADERS = $(addsuffix .h,$(FILENAMES)) common.h
OBJECTS = $(addsuffix .o,$(FILENAMES))
TARGETS = challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 challenge7

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(LINK_OPENSSL) -c $< -o $@

challenge1: $(OBJECTS) challenge1.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

challenge2: $(OBJECTS) challenge2.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

challenge3: $(OBJECTS) challenge3.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

challenge4: $(OBJECTS) challenge4.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

challenge5: $(OBJECTS) challenge5.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

challenge6: $(OBJECTS) challenge6.o
	$(CC) $(CFLAGS)  $(OBJECTS) $@.o -o $@

challenge7: $(OBJECTS) challenge7.o
	$(CC) $(CFLAGS) $(LINK_OPENSSL) $(OBJECTS) $@.o -o $@

clean:
	-rm -f $(OBJECTS) $(TARGETS) $(addsuffix .o,$(TARGETS))
