CC=gcc
CFLAGS=-g -O2 -Wall -Wpedantic -I /usr/lib/x86_64-linux-gnu/

all: shell

shell: shell.c parse.c parse.h 
	${CC} ${CFLAGS} shell.c -o shell -lreadline -ltermcap 

clean:
	rm -f shell
