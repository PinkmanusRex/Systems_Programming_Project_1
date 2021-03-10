#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "file_wrapper.h"

#define DT_REG 8

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

	/** Creates the buffer. Buffer is the size of the declared column width. The idea is to read in col_width sized chunks to word wrap nicely before a newline. */
	char *word_buffer = malloc(sizeof(char)*col_width);
	if (!word_buffer) {
		return EXIT_FAILURE;
	}

	int err_flag = 0;
	/** argc==2 means that only the width was provided */
	if (argc==2) {
	    /** Call the word wrapper and display output to standard output. No directory processing necessary. */
		if (word_wrapper(0, 1, word_buffer, col_width)) {
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
			DIR *dirp = opendir(argv[2]);
			/** Failiure condition if directory cannot be opened. */
			if (!dirp) {
				perror(argv[2]);
				return EXIT_FAILURE;
			}
			struct dirent *de;
			while ((de = readdir(dirp))) {
				if (de->d_type == DT_REG) {
					/** cannot begin with "." or "wrap.". This solved the issue of coming across already wrapped files and improperly named files. */
					int filename_len = strlen(de->d_name);
					if (filename_len > 0) {
						char can_read = 1;
						/** check file name to not start with a period */
						if (de->d_name[0] == '.') {
							can_read = 0;
						}
						/** check file name to not start with "wrap" */
						if (filename_len >= 5) {
							if (de->d_name[0]=='w' && de->d_name[1] == 'r' && de->d_name[2] == 'a' && de->d_name[3] == 'p' && de->d_name[4] == '.') {
								can_read = 0;
							}
						}
						if (can_read) {
							/** <dir_name/dir_path>/wrap.<file_name> */
							int out_name_len = strlen(argv[2]) + 1 + 5 + filename_len + 1;
							int in_name_len = out_name_len - 5;
							char *out_name = malloc(sizeof(char)*out_name_len);
							char *in_name = malloc(sizeof(char)*in_name_len);
							out_name[0] = '\0';
							in_name[0] = '\0';
							strcat(in_name, argv[2]);
							strcat(in_name, "/");
							strcat(out_name, in_name);
							strcat(out_name, "wrap.");
							strcat(out_name, de->d_name);
							strcat(in_name, de->d_name);
							int file_input = open(in_name, O_RDONLY);
							/** Failiure condition if file within directory path cannot be read. */
							if (file_input == -1) {
								perror(in_name);
								err_flag = 1;
							} else {
								int file_output = open(out_name, O_WRONLY|O_TRUNC|O_CREAT, 0600);
								/** Failiure condition for if output file cannot be created. */
								if (file_output == -1) {
									perror(out_name);
									err_flag = 1;
								} else {
								    /** Function call for wrapper code. Returns error flag 1 if something goes wrong. */
									if (word_wrapper(file_input, file_output, word_buffer, col_width)) {
										err_flag = 1;
									}
								}
							}
							free(out_name);
							free(in_name);
						}
					}
				}
			}
			closedir(dirp);

		}
		/** Checks if st_mode is a regular file. */
		else if (S_ISREG(data.st_mode)) {
			int file_input = open(argv[2], O_RDONLY);
			/** Failiure condition for opening file as read only. */
			if (file_input == -1) {
				perror(argv[2]);
				err_flag = 1;
			} else {
			    /** Word wrap with only read access to the file. */
				if (word_wrapper(file_input, 1, word_buffer, col_width)) {
					err_flag = 1;
				}
				close(file_input);
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
	free(word_buffer);
	/** Final error check of the entire program. */
	if (err_flag) {
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}
