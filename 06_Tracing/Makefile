CC = gcc
CFLAGS = -Wall -g

all: move protect.so

move: move.c
	$(CC) $(CFLAGS) -o move move.c

protect.so: protect.c
	$(CC) $(CFLAGS) -shared -fPIC -ldl -o protect.so protect.c

test: all
	@chmod +x test.sh
	@./test.sh

clean:
	rm -f move protect.so *.o file_* PROTECT_* output_*

