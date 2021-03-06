# Systems Programming Project 1 : Word Wrapping

Name: Michael Zhang
NetID: mz487

Initially, the program was tested to check for syntax, memory leaks, and compilation errors. Upon confirming a lack of such issues, these cases were manually tested for:

	1) Does the program print as expected when encountering excess whitespace in the beginning? Indeed it did.
	2) Does the program print as expected given multiple newlines, thus establishing new paragraphs, but not printing excessive amounts of space between paragraphs? Yes, it did.
	3) Can the program handle not only directories that are scoped to ./, but also in other locations, such as directories above it? Yes, the program was able to handle other locations.
	4) Does the program behave as expected when encountering files that don't exist? That is, if the file inputted into the command line does not exist, does it perror and return EXIT_FAILURE? Indeed, it did.
	5) Does a similar behavior as described in the situation with a file that does not exist occur for a file which does not have read permission? Indeed, it did.
	6) Additionally, using echo $?, I checked the exit status of the program for all the above scenerios.