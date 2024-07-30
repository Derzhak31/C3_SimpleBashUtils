
#include "s21_cat.h"

int main(int argc, char **argv) {
  cat_options opts = {0, 0, 0, 0, 0, 0};
  cat_counters cnts = {1, 0, '\n'};

  if (argc > 1 && readOptions(argc, argv, &opts)) {
    if (optind < argc) {
      while (optind < argc) {
        FILE *file = fopen(argv[optind++], "r");
        if (file != NULL) {
          printFile(file, &opts, &cnts);
          fclose(file);
        } else
          fprintf(stderr, "cat: %s: No such file or directory\n",
                  argv[optind - 1]);
      }

    } else {
      printf("no files\n");
    }
  } else {
    printf("Invalid options\n");
  }
}

int readOptions(int argc, char **argv, cat_options *opts) {
  int c = 0;
  int result = 1;
  struct option long_options[] = {{"number-nonblank", 0, 0, 'b'},
                                  {"number", 0, 0, 'n'},
                                  {"squeeze-blank", 0, 0, 's'},
                                  {0, 0, 0, 0}};
  while ((c = getopt_long(argc, argv, "vbenstTE", long_options, NULL)) != -1) {
    switch (c) {
      case 'v':
        opts->v = 1;
        break;
      case 'b':
        opts->b = 1;
        break;
      case 'e':
        opts->e = 1;
        opts->v = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 't':
        opts->t = 1;
        opts->v = 1;
        break;
      case 'T':
        opts->t = 1;
        break;
      case 'E':
        opts->e = 1;
        break;
      default:
        result = 0;
    }
  }
  if (opts->n && opts->b) opts->n = 0;
  return result;
}

void printFile(FILE *file, cat_options *opts, cat_counters *cnts) {
  int c;
  while ((c = getc(file)) != EOF) {
    int print_line = 1;
    if (opts->s && c == '\n' && cnts->last_char == '\n') {
      if (++cnts->empty_lines_count > 1) print_line = 0;
    } else {
      cnts->empty_lines_count = 0;
    }
    if (print_line) {
      if (opts->n && cnts->last_char == '\n')
        printf("%6d\t", cnts->lines_count++);
      if (opts->b && cnts->last_char == '\n' && c != '\n')
        printf("%6d\t", cnts->lines_count++);
      if (opts->t && c == '\t') {
        printf("^");
        c = 'I';
      }
      if (opts->v && c != '\t' && c != '\n') {
        if (c >= 128) {
          c -= 128;
          printf("M-");
        }
        if (c < 32 || c == 127) {
          c ^= 0x40;
          printf("^");
        }
      }

      if (opts->e && c == '\n') printf("$");
      cnts->last_char = c;
      printf("%c", c);
    }
  }
}
