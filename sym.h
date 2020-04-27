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
cell clo;
cell comma;
cell comma_at;
cell cont;
cell iff;
cell lambda;
cell lit;
cell mac;
cell macro;
cell next;
cell o;
cell quote;
cell set;
cell unbound;

#endif
