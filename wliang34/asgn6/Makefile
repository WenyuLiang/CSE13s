CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra 
INCLUDE = -I./
TARGETS = decode encode 
OBJS = decode.o encode.o trie.o io.o word.o 

all: $(TARGETS)

decode: decode.o trie.o io.o word.o 
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

encode: encode.o trie.o io.o word.o 
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

%.o: %.c  
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
   
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGETS)

format:
	clang-format -i -style=file *.[ch]
