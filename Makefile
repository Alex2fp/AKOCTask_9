CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

all: sender receiver

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $<

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f sender receiver

.PHONY: all clean
