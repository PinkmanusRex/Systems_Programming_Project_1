Name: Michael Zhang
NetID: mz487

Name: Cyrus Majd
NetID: cm1355

DESCRIPTION:
	A word-wrapper program. Has two main functionalities:
		a) Wrap the contents of a text file according to a specified width. Result printed in standard output.
		b) Reformat contents of all files in a directory. Output written to file "wrap.ORIGINAL_SOURCE_NAME".

USAGE:
	To wrap a simple file,
		$ ./ww WIDTH FILE_NAME.txt

	To wrap a directory of files,
		$ ./ww WITDH DIRECTORY_PATH

TESTING STRATEGY:
	Initially, the program was tested to check for syntax, memory leaks, and compilation errors. Upon confirming a lack of such issues, these cases were manually tested for:

	1) Does the program print as expected when encountering excess whitespace in the beginning? Indeed it did.
	2) Does the program print as expected given multiple newlines, thus establishing new paragraphs, but not printing excessive amounts of space between paragraphs? Yes, it did.
	3) Can the program handle not only directories that are scoped to ./, but also in other locations, such as directories above it? Yes, the program was able to handle other locations.
	4) Does the program behave as expected when encountering files that don't exist? That is, if the file inputted into the command line does not exist, does it perror and return EXIT_FAILURE? Indeed, it did.
	5) Does a similar behavior as described in the situation with a file that does not exist occur for a file which does not have read permission? Indeed, it did.
	6) Additionally, using echo $?, I checked the exit status of the program for all the above scenerios.
	7) Does the program behave correctly when encountering any kind of ascii character? Yes it did.
	8) Does the program behave correctly when one word is larger than the size of the read buffer? Yes it did.
	9) Does the program behave correctly given large test cases? Yes it did.
	10) Does the program behave correctly when in a situation where write access to a file is not allowed? Yes.