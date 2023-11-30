CC = gcc
CFLAGS = -fsanitize=address -g

all: mysh pwd which middle_man output_args

mysh: parse.c exec.c DLL.c myshell.c
	$(CC) $(CFLAGS) parse.c exec.c DLL.c myshell.c -o mysh

pwd: ./built_ins/pwd.c
	$(CC) $(CLFAGS) ./built_ins/pwd.c -o ./built_ins/pwd

which: ./built_ins/which.c
	$(CC) $(CLFAGS) ./built_ins/which.c -o ./built_ins/which

middle_man: ./testing/executables/middle_man.c
	$(CC) $(CLFAGS) ./testing/executables/middle_man.c -o ./testing/executables/middle_man

output_args: ./testing/executables/output_args.c
	$(CC) $(CLFAGS) ./testing/executables/output_args.c -o ./testing/executables/output_args

.PHONY: clean

clean:
	rm -f mysh ./built_ins/pwd ./built_ins/which ./testing/executables/output_args ./testing/executables/middle_man ./testing/output/*