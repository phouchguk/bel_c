#ifndef SYM_H
#define SYM_H

void dump_sym(FILE *f);
void load_sym(FILE *f);
void sym_report(void);
void sym_init(void);
int get_sym(char*);
char *nom(cell);

/* global symbol defs */
cell nil;
cell t;

cell apply;
cell base;
cell begin;
cell bquote;
cell comma;
cell comma_at;
cell cont;
cell iff;
cell lit;
cell next;
cell o;
cell quote;

#endif
