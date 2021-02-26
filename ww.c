#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
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

int main(int argc, char *argv[]) {
	/** must have [2,3] arguments. Any other number of arguments is invalid */
	if (argc < 2 || argc > 3) {
		return EXIT_FAILURE;
	}
	int col_width = atoi(argv[1]);
	/** col_width must be a positive integer */
	if (col_width <= 0) {
		return EXIT_FAILURE;
	}

	/** creates the buffer */

	char *word_buffer = malloc(sizeof(char)*col_width);
	if (!word_buffer) {
		return EXIT_FAILURE;
	}

	/** create the regex for the whitespace delimiters */

	regex_t delim;
	regcomp(&delim, "[:space:]", 0);

	int err_flag = 0;
	/** argc==2 means that only the width was provided */
	if (argc==2) {
		if (word_wrapper(0, 1, word_buffer, &delim)) {
			err_flag = 1;
		}
	} else {
		struct stat data;
		int err = stat(argv[2], &data);
		if (err) {
			perror(argv[2]);
			err_flag = 1;
		}
		/** more research on handling directories necessary */
		else if (S_ISDIR(data.st_mode)) {

		} 
		else if (S_ISREG(data.st_mode)) {
			int file_input = open(argv[2], O_RDONLY);
			if (file_input == -1) {
				perror(argv[2]);
				err_flag = 1;
			} else {
				if (word_wrapper(file_input, 1, word_buffer, &delim)) {
					err_flag = 1;
				}
			}
		} 
		/** not even sure what kind of file this could be
		 * but it sure as hell ain't gonna be able to be
		 * read here. It ain't a directory, nor is it
		 * a regular file
		*/
		else {
			err_flag = 1;
		}
	}
	if (err_flag) {
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}