#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"

#define MAX_CELL 1024

struct cell the_cars[MAX_CELL];
struct cell the_cdrs[MAX_CELL];

int cell_i = 0;

struct cell join(struct cell a, struct cell d)
{
  if (cell_i >= MAX_CELL) {
    printf("out of cell mem -- JOIN\n");
    exit(1);
  }

  the_cars[cell_i] = a;
  the_cdrs[cell_i] = d;

  struct cell p;
  p.t = PAIR;
  p.val = cell_i++;

  return p;
}

struct cell car(struct cell p)
{
  if (p.t != PAIR) {
    printf("can't CAR non-pair -- CAR\n");
    exit(1);
  }

  return the_cars[p.val];
}

struct cell cdr(struct cell p)
{
  if (p.t != PAIR) {
    printf("can't CAR non-pair -- CDR\n");
    exit(1);
  }

  return the_cdrs[p.val];
}

struct cell xar(struct cell p, struct cell a)
{
  if (p.t != PAIR) {
    printf("can't XAR non-pair -- XAR\n");
    exit(1);
  }

  the_cars[p.val] = a;

  return a;
}

struct cell xdr(struct cell p, struct cell d)
{
  if (p.t != PAIR) {
    printf("can't XDR non-pair -- XDR\n");
    exit(1);
  }

  the_cdrs[p.val] = d;

  return d;
}
