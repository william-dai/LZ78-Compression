CFLAGS=-Wall -Wextra -Werror -pedantic
CC=clang $(CFLAGS)

all	:
	make encode; make decode
encode	:	encode.o trie.o word.o io.o
	$(CC) encode.o trie.o word.o io.o -o encode
decode	:	decode.o trie.o word.o io.o
	$(CC) decode.o trie.o word.o io.o -o decode
encode.o	:	encode.c
	$(CC) -c encode.c
decode.o	:	decode.c
	$(CC) -c decode.c
trie.o	:	trie.c
	$(CC) -c trie.c
word.o	:	word.c
	$(CC) -c word.c
io.o	:	io.c
	$(CC) -c io.c
clean	:
	rm -f encode decode encode.o decode.o trie.o word.o io.o
infer	:
	make clean; infer-capture -- make; infer-analyze -- make

