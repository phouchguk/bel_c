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

cell bel_car(cell p);
cell bel_cdr(cell p);

cell xar(cell, cell);
cell xdr(cell, cell);

void set_loc(cell l, cell lc);
void reset_loc(void);
cell get_where(void);

#endif
