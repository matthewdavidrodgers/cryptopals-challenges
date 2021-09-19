CC = gcc

HEADERS = bytes.h base64.h bbuf.h hex.h
OBJECTS = bytes.o base64.o bbuf.o hex.o

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@

challenge1: $(OBJECTS) challenge1.o
	$(CC) $(OBJECTS) challenge1.o -o $@

challenge2: $(OBJECTS) challenge2.o
	$(CC) $(OBJECTS) challenge2.o -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f challenge1
