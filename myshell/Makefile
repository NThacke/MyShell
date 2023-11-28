CC = gcc
CFLAGS = -fsanitize=address

all: mysh pwd

mysh: parse.c exec.c DLL.c myshell.c
	$(CC) $(CFLAGS) parse.c exec.c DLL.c myshell.c -o mysh

pwd: ./built_ins/pwd.c
	$(CC) $(CLFAGS) ./built_ins/pwd.c -o ./built_ins/pwd

.PHONY: clean

clean:
	rm -f mysh ./built_ins/pwd