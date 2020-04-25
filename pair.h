#ifndef PAIR_H
#define PAIR_H

struct cell {
  char t;
  int val;
};

int id(struct cell a, struct cell b);

struct cell join(struct cell a, struct cell d);

struct cell car(struct cell p);
struct cell cdr(struct cell p);

struct cell xar(struct cell p, struct cell a);
struct cell xdr(struct cell p, struct cell d);

#endif
