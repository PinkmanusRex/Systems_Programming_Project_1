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
		/**
		 * a null terminator might or might not be considered
		 * a whitespace character. It does not show up as a
		 * visible whitespace character, and it is not a word
		 * character either. Must ask further on the piazza for
		 * such clarification, but for now just ignore null
		 * terminators
		*/
		if (intermediate_buf[0]=='\0') {
			bytesRead = read(file_input, intermediate_buf, 1);
			intermediate_buf[1] = '\0';
			continue;
		}
		/** 
		 * regex for checking if a whitespace delimiter, which
		 * either completes a word, or in the event of wEmpty,
		 * is just ignored, leaving it in wEmpty state
		*/
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
		/** 
		 * Word character encountered. wEmpty becomes wIncomplete
		 * In the case of width 1, wEmpty -> wIncomplete immediately
		 * to wIncomplete -> wThreshold_Warn. 
		 * In other cases, still wIncomplete -> wThreshold_Warn upon 
		 * word_buf_used == word_buf_max as the word is definitely
		 * greater than or equal to the maximum acceptable width of
		 * a line.
		 * "else if wThreshold_Warn" ensures that we are already in
		 * the state where the word is at the line length limit, 
		 * and so the addition of another character in the word 
		 * is now beyond the acceptable line width. 
		 * So we have exceeded the threshold.
		*/
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
		/** 
		 * wThreshold_Warn_Complete:
		 *		comes only immediately from a wThreshold_Warn,
		 * 		therefore the buffer is currently empty. Need
		 * 		only to create a new line and reset all line
		 * 		and word states
		*/
		if (word_buf_state == wThreshold_Warn_Complete) {
			write(file_output, newline, 1);
			line_state = lEmpty;
			word_buf_state = wEmpty;
			line_used = 0;
		}
		/** 
		 * a complete word B which has length less than the maximum
		 * acceptable width of a line. But just because the word
		 * length is within the max width of a line does not mean
		 * it can fit within the line. If the line is lEmpty, then
		 * the word can fit in as the first word of the line.
		 * if line is lNonEmpty, then there is a last word A in the
		 * line already. Should word B be appended, then separate
		 * A and B with a space. So does adding a space and word B
		 * exceed the width of the line? If so, must go as the first
		 * word of a new line
		*/
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
		/** 
		 * wThreshold_Warn means that the word as discovered so far
		 * takes up the maximal width of a line, so it must occupy
		 * an entire line. If the current line is lEmpty, it already
		 * has an entire line available all to itself. Else it must
		 * get a new line. 
		*/
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
		/** 
		 * Have to dump the contents of the buffer when there
		 * is no more space. Follows from wThreshold_Warn or
		 * wThreshold_Exceed. By the production rules for this
		 * state, is just one part of an entire word, therefore
		 * no need to occupy its own line since it must be appended
		 * to the current word of the current line
		*/
		else if (word_buf_state == wThreshold_Exceed) {
			if (word_buf_used == word_buf_max) {
				write(file_output, word_buffer, word_buf_used);
				line_used += word_buf_used;
				word_buf_used = 0;
			}
			err_flag = 1;
		}
		/** 
		 * Have completed the overly long word. Reset everything as
		 * in a normal length completed word, but keep track of the
		 * error situation
		*/
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
	/** 
	 * None of the completed states would be here as they only
	 * come as a result of reading a whitespace delimiter
	 * wThreshold_Warn would always result in word_buf_used == 0
	 * wThreshold_Exceed could possibly have a non-empty buffer.
	 * The only other state which has a non-empty buffer is
	 * wIncomplete, which must be treated in the same manner
	 * as encountering a wComplete
	*/
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
	/** 
	 * But if buffer is empty, then can only consider what is currently
	 * at the line, if there is anything at all. err_flag would already
	 * have been set prior to this statement, so no use checking for
	 * the wThreshold_Exceed situation here. 
	*/
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