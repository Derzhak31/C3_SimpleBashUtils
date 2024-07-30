CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11 -pedantic

all: s21_cat clean

s21_cat: Makefile s21_cat.h s21_cat.c
	$(CC) $(FLAGS) s21_cat.c -o s21_cat

check:
	clang-format -style=Google -n *.c

test: s21_cat
	bash test_cat.sh

clean:
	rm -rf *.o 