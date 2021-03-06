CC = gcc
CFLAGS = -g -std=c99 -Wvla -Wall -fsanitize=address,undefined

all: ww

ww: ww.o file_wrapper.o
	$(CC) $(CFLAGS) -o $@ $^

ww.o: ww.c file_wrapper.h 
	$(CC) $(CFLAGS) -c $<

file_wrapper.o: file_wrapper.c file_wrapper.h state_tracker.h
	$(CC) $(CFLAGS) -c $<

tests: test1_width2 test1_width10

test1_width2: ww
	./ww 2 test_1.txt | diff test_1_width2.txt - 

test1_width10: ww
	./ww 10 test_1.txt | diff test_1_width10.txt -