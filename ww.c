#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include "file_wrapper.h"

/** 
 * File that runs word_wrapper.
 * 
 * Must declare and allocate space for the word_buffer in here
 * before passing it to word_wrapper
 * 
 * Have to check if file can be opened. We might not have permission,
 * therefore if we encounter an error as a permission issue, we must
 * print to perror as described in pa1.pdf
 * 
 * If file not given in command line arguments, then input is from stdin
 * 
 * directories and such related stuff as described in the pa1.pdf
*/