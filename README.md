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

  This program is the exector of programs, and is the second largest component of the project (behind the parser). Not much theory has been developed for this yet, and needs to be thought out before work can be done.
  This program invokes fork(), dup2(), and other similar system calls that actually execute programs and processes.




  
  
