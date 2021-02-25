#include "state_tracker.h"
#include "file_wrapper.h"
#include <unistd.h>
#include <fcntl.h>
#include <regex.h>

int word_wrapper(int file_input, int file_output, char *word_buffer, int width, regex_t delim) {
	int err_flag = 0;
	
	int word_buf_used = 0;
	int word_buf_max = width;
	
	int line_used = 0;
	int line_max = width;

	enum word_buf_states word_buf_state = wEmpty;
	enum line_states line_state = lEmpty;

	char newline[] = {'\n'};
	char spaceChar[] = {' '};

	char intermediate_buf[2];

	int bytesRead = read(file_input, intermediate_buf, 1);
	intermediate_buf[1] = '\0';

	while (bytesRead > 0) {
		if (intermediate_buf[0]=='\0') {
			bytesRead = read(file_input, intermediate_buf, 1);
			intermediate_buf[1] = '\0';
			continue;
		}
		if (!regexec(&delim, intermediate_buf, 0, NULL, 0)) {
			if (word_buf_state == wEmpty) {
				word_buf_state = wEmpty;
			}
			else if (word_buf_state == wIncomplete) {
				word_buf_state = wComplete;
			}
			else if (word_buf_state == wThreshold_Warn) {
				word_buf_state = wThreshold_Warn_Complete;
			}
			else if (word_buf_state == wThreshold_Exceed) {
				word_buf_state = wThreshold_Exceed_Complete;
			}
		}
		else {
			word_buffer[word_buf_used] = intermediate_buf[0];
			word_buf_used += 1;
			if (word_buf_state == wEmpty) {
				word_buf_state = wIncomplete;
			}
			if (word_buf_state == wIncomplete && word_buf_used == word_buf_max) {
				word_buf_state = wThreshold_Warn;
			}
			else if (word_buf_state == wThreshold_Warn) {
				word_buf_state = wThreshold_Exceed;
			}
			else if (word_buf_state == wThreshold_Exceed) {
				word_buf_state = wThreshold_Exceed;
			}
		}
		if (word_buf_state == wThreshold_Warn_Complete) {
			write(file_output, newline, 1);
			line_state = lEmpty;
			word_buf_state = wEmpty;
			line_used = 0;
		}
		else if (word_buf_state == wComplete) {
			if (line_state == lEmpty) {
				line_state = lNonEmpty;
				line_used = word_buf_used;
			} else {
				if (line_used + 1 + word_buf_used <= line_max) {
					write(file_output, spaceChar, 1);
					line_used += 1 + word_buf_used;
				} else {
					write(file_output, newline, 1);
					line_used = word_buf_used;
				}
			}
			write(file_output, word_buffer, word_buf_used);
			word_buf_used = 0;
			word_buf_state = wEmpty;
		}
		else if (word_buf_state == wThreshold_Warn) {
			if (line_state == lEmpty) {
				write(file_output, word_buffer, word_buf_used);
			}
			else if (line_state == lNonEmpty) {
				write(file_output, newline, 1);
				write(file_output, word_buffer, word_buf_used);
			}
			line_state = lNonEmpty;
			line_used = word_buf_used;
			word_buf_used = 0;
		}
		else if (word_buf_state == wThreshold_Exceed) {
			if (word_buf_used == word_buf_max) {
				write(file_output, word_buffer, word_buf_used);
				line_used += word_buf_used;
				word_buf_used = 0;
			}
			err_flag = 1;
		}
		else if (word_buf_state == wThreshold_Exceed_Complete) {
			if (word_buf_used > 0) {
				write(file_output, word_buffer, word_buf_used);
			}
			write(file_output, newline, 1);
			line_used = 0;
			line_state = lEmpty;
			word_buf_used = 0;
			word_buf_state = wEmpty;
			err_flag = 1;
		}
		bytesRead = read(file_input, intermediate_buf, 1);
		intermediate_buf[1] = '\0';
	}
	if (word_buf_used > 0) {
		if (word_buf_state == wThreshold_Exceed) {
			write(file_output, word_buffer, word_buf_used);
			write(file_output, newline, 1);
			err_flag = 1;
		}
		else {
			if (line_state == lNonEmpty) {
				if (line_used + 1 + word_buf_used <= line_max) {
					write(file_output, spaceChar, 1);
				} else {
					write(file_output, newline, 1);
				}
			}
			write(file_output, word_buffer, word_buf_used);
			write(file_output, newline, 1);
		}
	}
	else {
		if (line_state == lNonEmpty) {
			write(file_output, newline, 1);
		}
	}
	if (err_flag) {
		return 1;
	}
	else {
		return 0;
	}
}