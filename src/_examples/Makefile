CC = gcc
CFLAGS = -Wall -std=c23 -I../ -lcurl

.PHONY: clean

run-%:
	@$(CC) $(CFLAGS) $*.c -o example_$*
	@./example_$*
	@rm -f example_$*

clean:
	@rm -f example_*