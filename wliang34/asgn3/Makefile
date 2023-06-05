SHELL := /bin/sh
CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra 
INCLUDE = -I./
TARGET = sorting
SOURCE = $(wildcard *.c)      
OBJS = $(patsubst %.c, %.o, $(SOURCE))
all: $(TARGET)

$(TARGET):$(OBJS)  
	$(CC) $(OBJS) -o $(TARGET)     
%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE)  -c $^ -o $@                                                                                                                                             
.PHONY:clean
clean:
	rm $(OBJS) $(TARGET)
format:
	clang-format -i -style=file *.[ch]

