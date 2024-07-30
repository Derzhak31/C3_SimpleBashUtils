#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n"

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
} grep_opt;

typedef struct {
  char **pattern;
  size_t count;
} grep_pat;

int readOptions(int argc, char **argv, grep_opt *opt, grep_pat *pat);
int checkPatterns(grep_pat *pat);
void addPatterns(grep_pat *pat, char *pats);
void freePatterns(grep_pat *pat);
void printFiles(int argc, char **argv, int ind, grep_opt *opt, grep_pat *pat);
void grepFile(FILE *f, grep_opt *opt, grep_pat *pat, char *filename);
void grepOflag(char *buf, int countStr, grep_opt *opt, grep_pat *pat,
               char *filename);
int grepLine(char *buf, grep_opt *opt, grep_pat *pat, int *pos, int *len);
void printLine(char *buf, int countStr, grep_opt *opt, char *filename);
int readPatterns(grep_pat *pat, char *pats_file);

#endif