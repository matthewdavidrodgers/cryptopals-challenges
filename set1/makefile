CC = clang
CFLAGS = -g -Wall -Wextra
INCLUDE_OPENSSL = -I/usr/local/Cellar/openssl@3/3.0.0_1/include
LINK_OPENSSL = -L/usr/local/Cellar/openssl@3/3.0.0_1/lib -I/usr/local/Cellar/openssl@3/3.0.0_1/include -lssl -lcrypto

FILENAMES = ../bytes ../base64 ../bbuf ../hex ../cypher
HEADERS = $(addsuffix .h,$(FILENAMES)) ../common.h
OBJECTS = $(addsuffix .o,$(FILENAMES))
TARGETS = challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 challenge7 challenge8

# this rule overrides the "%.o" rule below it when it matches
# challenge7.o: challenge7.c
#	$(CC) $(CFLAGS) $(INCLUDE_OPENSSL) -c $< -o $@

../cypher.o: ../cypher.c
	$(CC) $(CFLAGS) $(INCLUDE_OPENSSL) -c $< -o $@

# transforms .c files to .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# this rule overrides the "challenge%" rule below it when it matches
challenge7: $(OBJECTS) challenge7.o
	$(CC) $(CFLAGS) $(INCLUDE_OPENSSL) $(LINK_OPENSSL) $(OBJECTS) $@.o -o $@

# executable targets 
challenge%: $(OBJECTS) challenge%.o
	$(CC) $(CFLAGS) $(OBJECTS) $@.o -o $@

clean:
	-rm -f $(OBJECTS) $(TARGETS) $(addsuffix .o,$(TARGETS))
