CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11 -pedantic

all: s21_grep clean

s21_grep: Makefile s21_grep.h s21_grep.c
	$(CC) $(FLAGS) s21_grep.c -o s21_grep

check:
	clang-format -style=Google -n *.c

test: s21_grep
	bash grep_test.sh

clean:
	rm -rf *.o 