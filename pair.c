#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"

#define MAX_CELL 1024

cell the_cars[MAX_CELL];
cell the_cdrs[MAX_CELL];

int cell_i = 0;

const cell val_mask  = ~0U >> TAG_BIT_SIZE;
const cell pair_mask = PAIR << CELL_SHIFT;

int id(cell a, cell b)
{
  return a == b;
}

int pair(cell x)
{
  return x >> CELL_SHIFT == PAIR;
}

cell join(cell a, cell d)
{
  if (cell_i >= MAX_CELL) {
    printf("out of cell mem -- JOIN\n");
    exit(1);
  }

  the_cars[cell_i] = a;
  the_cdrs[cell_i] = d;

  return cell_i++ | pair_mask;
}

cell car(cell p)
{
  if (p == 0) {
    return 0;
  }

  if (!pair(p)) {
    printf("can't CAR non-pair -- CAR\n");
    exit(1);
  }

  return the_cars[p & val_mask];
}

cell cdr(cell p)
{
  if (p == 0) {
    return 0;
  }

  if (!pair(p)) {
    printf("can't CAR non-pair -- CDR\n");
    exit(1);
  }

  return the_cdrs[p & val_mask];
}

cell xar(cell p, cell a)
{
  if (!pair(p)) {
    printf("can't XAR non-pair -- XAR\n");
    exit(1);
  }

  the_cars[p & val_mask] = a;

  return a;
}

cell xdr(cell p, cell d)
{
  if (!pair(p)) {
    printf("can't XDR non-pair -- XDR\n");
    exit(1);
  }

  the_cdrs[p & val_mask] = d;

  return d;
}
