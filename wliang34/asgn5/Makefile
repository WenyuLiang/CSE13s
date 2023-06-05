CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra 
INCLUDE = -I./
LDFLAGS = -lgmp
TARGETS = decrypt encrypt keygen
OBJS = decrypt.o encrypt.o keygen.o numtheory.o randstate.o ss.o

all: $(TARGETS)

decrypt: decrypt.o numtheory.o randstate.o ss.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@

encrypt: encrypt.o numtheory.o randstate.o ss.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@

keygen: keygen.o numtheory.o randstate.o ss.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@

%.o: %.c  
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
   
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGETS)

format:
	clang-format -i -style=file -sort-includes=false *.[ch]
