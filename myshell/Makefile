CC = gcc
CFLAGS = -fsanitize=address

all: mysh pwd

mysh: parse.c exec.c DLL.c myshell.c
	$(CC) $(CFLAGS) parse.c exec.c DLL.c myshell.c -o mysh

pwd: pwd.c
	$(CC) $(CLFAGS) pwd.c -o pwd

.PHONY: clean

clean:
	rm -f mysh pwd