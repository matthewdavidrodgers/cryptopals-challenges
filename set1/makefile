CC = clang

FILENAMES = bytes base64 bbuf hex cypher
HEADERS = $(addsuffix .h,$(FILENAMES)) common.h
OBJECTS = $(addsuffix .o,$(FILENAMES))
TARGETS = challenge1 challenge2 challenge3 challenge4 challenge5 challenge6

%.o: %.c $(HEADERS)
	$(CC) -g -c $< -o $@

challenge1: $(OBJECTS) challenge1.o
	$(CC) $(OBJECTS) $@.o -o $@

challenge2: $(OBJECTS) challenge2.o
	$(CC) $(OBJECTS) $@.o -o $@

challenge3: $(OBJECTS) challenge3.o
	$(CC) $(OBJECTS) $@.o -o $@

challenge4: $(OBJECTS) challenge4.o
	$(CC) $(OBJECTS) $@.o -o $@

challenge5: $(OBJECTS) challenge5.o
	$(CC) $(OBJECTS) $@.o -o $@

challenge6: $(OBJECTS) challenge6.o
	$(CC) -g $(OBJECTS) $@.o -o $@

clean:
	-rm -f $(OBJECTS) $(TARGETS) $(addsuffix .o,$(TARGETS))
