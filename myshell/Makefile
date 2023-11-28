CC = gcc
CFLAGS = -fsanitize=address

all: mysh pwd which

mysh: parse.c exec.c DLL.c myshell.c
	$(CC) $(CFLAGS) parse.c exec.c DLL.c myshell.c -o mysh

pwd: ./built_ins/pwd.c
	$(CC) $(CLFAGS) ./built_ins/pwd.c -o ./built_ins/pwd

which: ./built_ins/which.c
	$(CC) $(CLFAGS) ./built_ins/which.c -o ./built_ins/which

.PHONY: clean

clean:
	rm -f mysh ./built_ins/pwd ./built_ins/which