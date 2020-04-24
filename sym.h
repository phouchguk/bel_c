#ifndef SYM_H
#define SYM_H

void sym_init(void);
struct cell get_sym(char*);
char *nom(struct cell);

/* global symbol defs */
struct cell nil;
struct cell t;

struct cell apply;
struct cell o;

#endif
