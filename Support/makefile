RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=%.o)

# Clean and compile .so
all: libSupport.so

libSupport.so: create-dirs $(OBJS)
	$(CC) -shared -o "libSupport.so" $(OBJS)

install: all
	cp -u libSupport.so /usr/lib
	cp --parents -u $(H_SRCS) /usr/include

uninstall:
	rm -f /usr/lib/Support.so

.PHONY: all create-dirs clean install uninstall
