CC = gcc

HEADERS = bytes.h base64.h bbuf.h hex.h cypher.h
OBJECTS = bytes.o base64.o bbuf.o hex.o cypher.o
TARGETS = challenge1 challenge2 challenge3 challenge4 challenge5

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@

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

clean:
	-rm -f $(OBJECTS) $(TARGETS)
