CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: encode decode error entropy

encode: encode.o 
	$(CC) -o encode encode.o hamming.o bm.o bv.o

encode.o:
	$(CC) $(CFLAGS) -c encode.c hamming.c bm.c bv.c

decode: decode.o 
	$(CC) $(CFLAGS) -o decode decode.o hamming.o bm.o bv.o

decode.o:
	$(CC) $(CFLAGS) -c decode.c hamming.c bm.c bv.c 

error: error.o
	$(CC) -o error error.o

error.o:
	$(CC) $(CFLAGS) -c error.c

entropy: entropy.o
	$(CC) -o entropy entropy.o -lm

entropy.o:
	$(CC) $(CFLAGS) -c entropy.c

format:
	clang-format -i -style=file *.c *.h

clean:
	rm -f encode decode error entropy ./*.o

scan-build: clean
	scan-build make
