#ifndef SYM_H
#define SYM_H

void dump_sym(FILE *f);
void load_sym(FILE *f);
void sym_report(void);
void sym_init(void);
int get_sym(char*);
int get_uvar(void);
char *nom(cell);
void print_sym(cell c);

/* global symbol defs */
cell nil;
cell t;

cell a;
cell apply;
cell argument;
cell base;
cell begin;
cell bquote;
cell clo;
cell comma;
cell comma_at;
cell cont;
cell d;
cell dyn;
cell evfn;
cell gather;
cell iff;
cell lambda;
cell lit;
cell mac;
cell macro;
cell next;
cell o;
cell prim;
cell quote;
cell set;
cell s_car;
cell s_cdr;
cell unbound;
cell uvar;
cell where;

#endif
