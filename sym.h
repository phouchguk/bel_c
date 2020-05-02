#ifndef SYM_H
#define SYM_H

#include <stdio.h>
#include <stdlib.h>

void dump_sym(FILE *f);
void load_sym(FILE *f);
void sym_report(void);
void sym_init(void);
int get_sym(char*);
int symbol(cell);
char *nom(cell);
void print_sym(cell c);

/* global symbol defs */
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
cell s_car;
cell s_cdr;
cell s_coin;
cell s_globe;
cell s_id;
cell s_join;
cell s_nom;
cell s_sym;
cell s_type;
cell s_xar;
cell s_xdr;
cell set;
cell scope;
cell unbound;
cell where;

#endif
