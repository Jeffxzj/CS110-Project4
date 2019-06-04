CC=gcc
CFLAGS=-g -O2 -Wall -Wpedantic

all: shell

shell: shell.c parse.c parse.h 
	${CC} ${CFLAGS} shell.c -o shell -lreadline -ltermcap 

clean:
	rm -f shell
