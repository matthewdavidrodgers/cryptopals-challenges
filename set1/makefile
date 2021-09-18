CC = gcc

HEADERS = base64.h bbuf.h hex.h
OBJECTS = base64.o bbuf.o hex.o challenge1.o

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@

challenge1: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f challenge1
