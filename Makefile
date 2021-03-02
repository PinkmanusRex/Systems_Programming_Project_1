CC = gcc
CFLAGS = -g -std=c99 -Wvla -Wall -fsanitize=address,undefined

all: ww

ww: ww.o file_wrapper.o
	$(CC) $(CFLAGS) -o $@ $^

ww.o: ww.c file_wrapper.h 
	$(CC) $(CFLAGS) -c $<

file_wrapper.o: file_wrapper.c file_wrapper.h state_tracker.h
	$(CC) $(CFLAGS) -c $<