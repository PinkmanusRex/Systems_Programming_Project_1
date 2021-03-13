# Systems Programming Project 1: Word Wrapping

Name: Michael Zhang
NetID: mz487

Name: Cyrus Majd
NetID: cm1355
 
### Description:
A word wrapping program meant to reformat a text file into a more easily digestible format, wrapping based upon a user inputted width and eliminating excess whitespace.
 
### Usage:
 - Will wrap FILE_NAME within WIDTH amount, printing results to STDOUT
    ```sh
    $./ww WIDTH FILE_NAME
    ```
 - Will wrap all files in a DIRECTORY  according to WIDTH, outputting to wrap.file_name for every file_name in DIRECTORY
    ```sh
    $./ww WIDTH DIRECTORY
    ```
 - Will read in from STDIN and print to STDOUT, according to WIDTH
    ```sh
    $./ww WIDTH
    ```
 - Note: It is recommended that if you so wish to read from STDIN, that you use piping and redirection to achieve so. Example:
    ```sh
    $cat FILE_NAME | ./ww WIDTH
    ```
### Testing Strategy:
Initially, the program was tested to check for syntax, memory leaks, and compilation errors. Upon confirming a lack of such issues, these cases were tested for:

1) Does the program print as expected when encountering excess whitespace in the beginning?\
&emsp;&emsp;Indeed it did. All excess whitespace in the beginning was eliminated in the output.

2) Does the program print as expected given multiple newlines, thus establishing new paragraphs, but not printing excessive amounts of space between paragraphs?\
&emsp;&emsp;Yes, it did. Only a singular empty line was printed between any two paragraphs.

3) Can the program handle not only directories that are scoped to ```./```, but also in other locations, such as directories above it?\
&emsp;&emsp;Yes, the program was able to handle other directory paths, both relative and absolute.

4) Does the program behave as expected when encountering files that don't exist? That is, if the file inputted into the command line does not exist, does it ```perror``` and return ```EXIT_FAILURE```?\
&emsp;&emsp;It did indeed ```perror```, and it did return ```EXIT_FAILURE``` as confirmed using ```echo $?```.

5) Does a similar behavior as described in the situation with a file that does not exist occur for a file which does not have read permission?\
&emsp;&emsp;Indeed, it did. Confirmed by the ```perror``` results as well as ```echo $?```.

6) Does the program behave correctly when one word is larger than the size of the read buffer?\
&emsp;&emsp;Yes it did, as ```echo $?``` showed the exit code to be the int value of ```EXIT_FAILURE``` on a Ubuntu machine.

7) Does the program correctly wrap larger text files?\
&emsp;&emsp;Yes, confirming that the program can handle files of large size.

8) Does the program behave correctly when in a situation where write access to a file is not allowed?\
&emsp;&emsp;Yes. It ```perror``` as expected, although it should be noted that such a situation would be more a user error, as there should be no "wrap" file being written to that has not been made by the program itself.
