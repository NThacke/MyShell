# MyShell
A marked-down version of a shell program, similar to bash or zsh

Author : Nicholas Thacke (nct36) of Rutgers University, New Brunswick

## Exectuion

To execute *mysh*, perform the following commands.

> $ make clean

The above command cleans (deletes) any executables from particular directories (of which are expected to contain executables).

> $ make

The above command compiles all source code for *mysh* as well as testing source code.


### Interactive Mode

Once you have compiled *mysh* (above), you can enter interactive mode simply by executing the following command.

> $ ./mysh

Then, enter commands similar to bash/zsh. Specific functionality is listed under **Functionality** of this README.

> mysh > echo hello world!
> hello world!

### Batch Mode

Batch mode allows users to send *mysh* entire files containing MyShell commands, which will be executed sequentially.

To enter Batch mode, simply run the following command.

> $ ./mysh some/directory/commands.txt

MyShell will read the commands located within *some/directory/commands.txt*, executing them sequentially in order that they appear, stopping once at the EOF, or if a given command is *exit*.

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

Various tests were performed to test the fucntionality of **MyShell**. These include typical expected behavior tests, such as executing a file, redirecting input/output, as well as piping. 

Invocations of UNIX programs (those located under "/usr/local/bin/", "/usr/bin/", or "/bin/") were tested exhaustively, that is that each program located within the aforementioned directories were tested either within batch tests, or within interactive modes (where appropriate). For instance, *git* was tested interactively, as most invocations of *git* are occurred in an interactive fashion. However, *sort* and *grep* were tested using batch mode.

Error conditions were tested as well. These include, but are not limited to the following. 
  (1) Executing a file whose standard input was redirected to a file which does not exist. 
  (2) Executing a file whose standard output was redirected to a file which does not exist (in this case, the file is created, so long as the directory exists).
  (3) Executing a file whose standard output was redirected to a file, whose directory does not exist (in this case, **MyShell** refuses to execute the command).
  (4) Executing a file whose standard input and standard output was redirected to a file which does not exist (the file is contained within an invalid directory).
  (6) Executing of a pipe with any of the above properties

Furthermore, nonsensical commands were tested as well, to ensure robustness of **MyShell**. These are included within *./testing/batch/test8.txt*, and further information can be found in the associated portion of this README.

### Batch
*./testing/batch/*

These testing files are meant to be used in batch mode. Simply run ./mysh testing/batch/test1.txt or similar to obtain batch tests.

#### Test 1

  Test 1 is a very simple batch test that tests simple unix commands like echo and cat.
  
  The purpose of this test is to run simple commands without redirection that are known to exist and be executable.

#### Test 2

  Test 2 is a simple batch test that is meant to test the functionality of executing a file with various arguments.
  
  This test will execute a testing executable, which prints every argument it is given to STDOUT.


#### Test 3

  Test 3 tests the pipe functionality.
  
  This test executes two testing executables (which can be found under ./testing/batch/executables/). The first of which is *output_args*. *output_args* takes a variable amount     of input arguments, and writes each to standard output. The second executable is *middle_man*, which writes to standard output the standard input.
  
  That is that *output_args* will have its standard output redirected to the standard input of *middle_man*. The arguments given to *output_args* will go to the standard output,    piped into the standard input of *midle_man*, wherein *middle_man* will write to its standard output the arguments that were intially handed to *output_args*.
  
  In effect, this test should have the same output as test2, so long as test2 and test3 have the same arguments (which is ensured in the current state).

#### Test 4

  Test 4 is the first test to contain variuous internal tests, whose main purpose is to test redirection.
  
Test 4.a
  
  A simple redirection test; an executable *output_args* is invoked with the arguments *foo bar quux baz this is a test of sending output to a file :D* which has its standard output redirected to the file *testing/output/test4_a_output.txt*
  
Test 4.b
  
  This test is meant to fail; we are executing the same program *output_args*, and sending the output to a file which belongs to a directory that doesn't exit.
  
Test 4.c

   This test will try to open an executable file which does not exist, and send it to an output file which does exist.

Test 4.d

  The following test will try to open an executable file which does not exist, and send it to an output file which does not exist.

Test 4.e

  The following test will test redirection without use of white-space characters.

#### Test 5

Test 5 test the use of conditionals.

#### Test 6

Test 6 tests the use of conditionals with pipes. It also ensures that a conditional statement following a pipe token is not a valid command.

#### Test 7

Test 7 tests the use of unix programs, with the use of redirection and piping on them.

#### Test 8

Test 8 serves as a stress test mainly for the parser, as well as the executioner. Every test within this test file is meant to fail and cause **mysh** to print an error describing that it could not perform the given command.

Most commands in test 8 are nonsensical.

#### Test 9

Test 9 tests the use of wildcards.

### Executables

*./testing/executables/*

This directory contains executables required for testing purposes. Source code is included, but executable files are created with the use of the following command.

> make

To ensure proper testing, do not modify or delete any contents within this directory.

## Misc

It is recommended to not create any subdirectories or new files within any subdirectory of **MyShell**. It is *highly* recommended (and flat-out required) to not modify or delete any contents of *any* subdirectory or file contained. Result of which voids the functionality of **MyShell**.

In essence, don't modify/create/delete anything! Run the commands listed in the **Exectuion** section of this README to execute *mysh*.












  
  
