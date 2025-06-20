CC = clang
CFLAGS = -Wall -Werror -std=c99 -I../

.PHONY: clean

run-%:
	$(CC) $(CFLAGS) $*.c -o example_$*
	./example_$*
	rm -f example_$*

clean:
	rm -f example_*