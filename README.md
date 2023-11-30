# MyShell
A marked-down version of a shell program, similar to bash or zsh

## Functionality


### File Execution, Piping, and Input/Output Redirection

Functionality for common bash/zsh commands, such as executing files, input/output redirection, as well as piping between two (2) programs is supported. Piping between more than two programs is not currently supported.

Functionality for UNIX/POSIX programs is supported, such as cat, ls, pwd, cd, etc. That is that any program under usr/local/bin/, usr/bin/, or /bin/ is supported and can be executed similar to bash/zsh.

### Conditional Statements

MyShell supports conditional commands. If the first argument to a command begins with then or else, MyShell will execute the command if the previous command succeeded or failed, respectively. By default, MyShell initalizes itself assuming the "previous" command succeeded. That is that if the first command given to MyShell begins with a then statement, it will execute. If the first command given to MyShell is an else condition, the command will not be executed.

Use of "then" or "else" following a pipeline ("|") is forbidden, and will result in MyShell failing to execute the command.

### Wildcards

MyShell supports the use of a singular wildcard token ("*") as an argument to an executable. MyShell will find the *filenames* of files whose name matches the pattern in the given directory. MyShell only gives the *filename* as an argument, and not the full path.

Moreover, MyShell only supports wildcard tokens at the end of a directory path. Use of wildcard tokens within a directory will result in MyShell assuming that the directory name is given *as is*, that is, with the wildcard token ("*").

Use of multiple wildcard tokens (at the end of the directory path) will result in MyShell passing the argument as-is. No wildcard matching will be performed for either of the tokens.

> mysh> ./a.out /some/directory/*.txt

MyShell will execute **./a.out** with the arguments of *filenames* contained within the subdirectory /some/directory/ which end with *.txt*

> mysh> ./a.out ./some/example/something*something/test.txt

MyShell will execute **a./out** with the argument *./some/example/something*something/test.txt* That is to say that wildcard matching did not occur.

> mysh> ./a.out ./some/example/some*some/ *

If ./some/example/some*some/ is a valid subdirectory, then MyShell will search inside of it, and pass all filenames within it as arguments to ./a.out.

If ./some/example/some*some/ is an invalid subdirectory, then MyShell will pass ./some/example/some*some/ * as an argument to ./a.out as is.

> mysh> ./a.out ./some/example/test*test *.txt

MyShell will give ./a.out the argument ./some/example/test*test *.txt as-is, and no wildcard matching will be peformed.





Any other functionality present in bash/zsh is not supported within MyShell.


## Testing

Test files can be located under *./testing/*

### Batch
*./testing/batch/*

These testing files are meant to be used in batch mode. Simply run ./mysh testing/batch/test1.txt or similar to obtain batch tests.






  
  
