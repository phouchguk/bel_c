#ifndef SYM_H
#define SYM_H

void sym_init(void);
int get_sym(char*);
char *nom(cell);

/* global symbol defs */
cell nil;
cell t;

cell apply;
cell o;

#endif
