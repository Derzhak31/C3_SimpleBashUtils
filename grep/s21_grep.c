#include "s21_grep.h"

int main(int argc, char **argv) {
  grep_opt opt = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  grep_pat pat = {NULL, 0};
  if (argc > 0 && (readOptions(argc, argv, &opt, &pat)) > 0) {
    if (optind < argc) {
      if (!opt.e && !opt.f) addPatterns(&pat, argv[optind++]);
      if (checkPatterns(&pat)) printFiles(argc, argv, optind, &opt, &pat);
    } else
      printf(USAGE);
  } else
    printf(USAGE);
  freePatterns(&pat);
}

void addPatterns(grep_pat *pat, char *pats) {
  size_t len = strlen(pats);
  if (len > 0) {
    pat->pattern = realloc(pat->pattern, (pat->count + 1) * sizeof(char *));
    pat->pattern[pat->count] = malloc(len + 1);
    memcpy(pat->pattern[pat->count], pats, len);
    pat->pattern[pat->count][len] = '\0';
    pat->count++;
  }
}

void freePatterns(grep_pat *pat) {
  if (pat->pattern) {
    for (size_t i = 0; i < pat->count; i++) free(pat->pattern[i]);
    free(pat->pattern);
  }
  pat->count = 0;
  pat->pattern = NULL;
}

int checkPatterns(grep_pat *pat) {
  int result = 1, regerr;
  regex_t reegex;
  for (size_t i = 0; i < pat->count; i++) {
    if ((regerr = regcomp(&reegex, pat->pattern[i], 0)) != 0) {
      result = 0;
      char err[256];
      regerror(regerr, &reegex, err, sizeof(err));
      printf("s21_grep:%s\n", err);
    }
    regfree(&reegex);
  }
  return result;
}

void printLine(char *buf, int countStr, grep_opt *opt, char *filename) {
  if (!opt->h) printf("%s:", filename);
  if (opt->n) printf("%d:", countStr);
  printf("%s", buf);
  if (buf[strlen(buf) - 1] != '\n') printf("\n");
}

void grepOflag(char *buf, int countStr, grep_opt *opt, grep_pat *pat,
               char *filename) {
  int no_match = 0;
  char *tbuf = buf;
  if (opt->o && !opt->v)
    while (strlen(tbuf) > 0 && !no_match) {
      int pos = -1;
      int len = 0;
      if (grepLine(tbuf, opt, pat, &pos, &len)) {
        char *obuffer = malloc(len + 1);
        memcpy(obuffer, tbuf + pos, len);
        obuffer[len] = '\0';
        printLine(obuffer, countStr, opt, filename);
        free(obuffer);
        tbuf += pos + len;
      } else
        no_match = 1;
    }
}

int grepLine(char *buf, grep_opt *opt, grep_pat *pat, int *pos, int *len) {
  int res = 0, flag = 0;
  regex_t reegex;
  regmatch_t pmatch[100];
  if (opt->i) flag |= REG_ICASE;
  for (size_t i = 0; i < pat->count; i++) {
    if (regcomp(&reegex, pat->pattern[i], flag) == 0)
      if (regexec(&reegex, buf, 1, pmatch, 0) == 0) {
        res = 1;
        int olen = pmatch[0].rm_eo - pmatch[0].rm_so;
        if (len && pos)
          if (*pos == -1 || pmatch[0].rm_so < *pos ||
              (pmatch[0].rm_so == *pos && olen > *len)) {
            *pos = pmatch[0].rm_so;
            *len = olen;
          }
      }
    regfree(&reegex);
  }
  return res;
}

void grepFile(FILE *f, grep_opt *opt, grep_pat *pat, char *filename) {
  char buf[10000];
  int stop = 0, countStr = 0, coin = 0;

  while (fgets(buf, sizeof(buf), f) && !stop) {
    countStr++;
    if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] == '\0';
    int res = 0;
    if (opt->o) {
      grepOflag(buf, countStr, opt, pat, filename);
    } else
      res = grepLine(buf, opt, pat, NULL, NULL);

    if ((!res && opt->v) || (res && !opt->v)) {
      coin++;
      if (!opt->l && !opt->c && !opt->o)
        printLine(buf, countStr, opt, filename);
      else if (opt->l) {
        printf("%s\n", filename);
        stop = 1;
      }
    }
  }
  if (opt->c) {
    if (!opt->h) printf("%s:", filename);
    printf("%d\n", coin);
  }
}

void printFiles(int argc, char **argv, int ind, grep_opt *opt, grep_pat *pat) {
  if (argc - ind == 1) opt->h = 1;
  while (ind < argc) {
    FILE *f;
    if ((f = fopen(argv[ind++], "r")) != NULL) {
      grepFile(f, opt, pat, argv[ind - 1]);
      fclose(f);
    } else if (!opt->s)
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[ind - 1]);
  }
}

int readPatterns(grep_pat *pat, char *pats_file) {
  FILE *f;
  char line[256];
  int res = 1;

  if ((f = fopen(pats_file, "r")) == NULL) {
    printf("file %s not found", pats_file);
    res = 0;
  } else {
    while (fgets(line, sizeof(line), f)) {
      if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
      if (*line != '\n') addPatterns(pat, line);
    }
    fclose(f);
  }
  return res;
}

int readOptions(int argc, char **argv, grep_opt *opt, grep_pat *pat) {
  int c = 0, result = 1;
  while ((c = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (c) {
      case 'e':
        opt->e = 1;
        addPatterns(pat, optarg);
        break;
      case 'i':
        opt->i = 1;
        break;
      case 'v':
        opt->v = 1;
        break;
      case 'c':
        opt->c = 1;
        break;
      case 'l':
        opt->l = 1;
        break;
      case 'n':
        opt->n = 1;
        break;
      case 'h':
        opt->h = 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 'f':
        opt->f = 1;
        result = readPatterns(pat, optarg);
        break;
      case 'o':
        opt->o = 1;
        break;
      default:
        result = 0;
    }
  }
  if (opt->l) {
    opt->c = 0;
    opt->o = 0;
    opt->h = 0;
    opt->n = 0;
  }
  if (opt->c) {
    opt->o = 0;
    opt->n = 0;
  }
  if (result) result = optind;
  return result;
}