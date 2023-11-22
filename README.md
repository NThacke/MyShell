# MyShell
A marked-down version of a shell program, similar to bash or zsh


TODO ::

(1) Wildcard

Currently, wildcards are not supported and will result in undefined behavior. The code exists on a high-level point of view, but the dependent functions are not implemented.

In particular, the following functions need to be implemented for wildcards to behave properly :

  (a) char * determine_wildcard(char * buffer, size_t size, int index);

  This function determines (and returns) the wildcard string. Note that buffer[index] is guaranteed to be an '*' character if this function is invoked. This function then must look to the left and to the right of buffer[index] and create the proper wildcard string.

  For instance, if the buffer contains (only) "foo*.txt", the function will be invoked at index 3. It is then up to the function to properly return the string "foo*.txt".

  (b) struct wildcard_components * valid_wildcard(char * wildcard);

  This function determines the valid wildcards in the current working directory and returns them in a struct. The struct only contains the wildcard strings, as well as the number of wildcards.

  This function is handed a wildcard, as determined by function (a), and opens the current working directory (with opendir()) and finds any valid files that match the wildcard, and appends them to the struct.

Once the above functions have been properly implemented, wildcards "should" be able to be handled appropriately by the parser. Of course, this hasn't been tested, and only holds in my imagination / belief that I wrote correct code on a high-level.
If wildcards do not work when the functions are implemented, we may need to rethink how we go about wildcard handling.

(2) Executive Program

Redirection for one program works correctly; e.g. is as follows

input.txt > foo > output.txt

program foo will successfully have its input be from input.txt, and send its output to output.txt

Piping without redirection works correct; e.g. is as follows

example | foo

program "example" correctly sends its output to program "foo"

Piping with redirection does not work correctly; e.g. is as follows

input.txt > middle_man | foo

This results in relatively bizarre behavior. Try it and see!

input.txt > middle_man | foo > output.txt

Similar behavior as above, output.txt gets populated with junk / bizarre text.

example | foo > output.txt

output.txt does not receive any information.

What to do? Fix the file redirection when piping. Singular program works as expected, and handles input/output redirection.




  
  
