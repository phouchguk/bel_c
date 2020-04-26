#ifndef PAIR_H
#define PAIR_H

void pair_report(void);

int id(cell, cell);
int pair(cell);
cell gc(cell);
void dump_mem(void);
void load_mem(void);

cell join(cell, cell);

cell car(cell);
cell cdr(cell);

cell xar(cell, cell);
cell xdr(cell, cell);

#endif
