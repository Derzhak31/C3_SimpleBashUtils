#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} cat_options;

typedef struct {
  int lines_count;
  int empty_lines_count;
  char last_char;
} cat_counters;

void printFile(FILE *file, cat_options *opts, cat_counters *cnts);
int readOptions(int argc, char **argv, cat_options *opts);

#endif