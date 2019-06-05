CC=gcc
CFLAGS=-g

all: shell

shell: shell.c parse.c parse.h 
	${CC} ${CFLAGS} shell.c -o shell

clean:
	rm -f shell
