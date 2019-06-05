CC=gcc
CFLAGS=-g -O2 -Wall -Wpedantic

all: shell

shell: shell.c parse.c parse.h 
	${CC} ${CFLAGS} shell.c -o shell /lib/x86_64-linux-gnu/libreadline.so.6 -lreadline -ltermcap 

clean:
	rm -f shell
