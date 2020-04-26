#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"
#include "sym.h"

#define MAX_CELL 40960

cell cars_a[MAX_CELL];
cell cdrs_a[MAX_CELL];

cell cars_b[MAX_CELL];
cell cdrs_b[MAX_CELL];

cell *the_cars = cars_a;
cell *the_cdrs = cdrs_a;

cell *new_cars;
cell *new_cdrs;

int cell_i = 0;
int gc_i;

const cell val_mask  = ~0U >> TAG_BIT_SIZE;
const cell pair_mask = PAIR << CELL_SHIFT;
const cell heart_mask = HEART << CELL_SHIFT;

int pkb(int n)
{
  return n * 4 * 2 / 1024;
}

void pair_report(void)
{
  printf("mem: %i/%i %i/%ikb\n", cell_i, MAX_CELL, pkb(cell_i), pkb(MAX_CELL));
}

cell the_broken_heart = heart_mask;

cell gc_cell(cell x)
{
  int new_i, new_p, old_p;
  cell old_car, old_cdr;

  if (!pair(x)) {
    return x;
  }

  old_p = x & val_mask;

  if (the_cdrs[old_p] == the_broken_heart) {
    return the_cars[old_p];
  }

  new_i = gc_i++;

  if (new_i == MAX_CELL) {
    printf("out of memory - GC_CELL");
    exit(1);
  }

  new_p = new_i | pair_mask;

  old_car = the_cars[old_p];
  old_cdr = the_cdrs[old_p];

  the_cars[old_p] = new_p;
  the_cdrs[old_p] = the_broken_heart;

  new_cars[new_i] = gc_cell(old_car);
  new_cdrs[new_i] = gc_cell(old_cdr);

  return new_p;
}

cell gc(cell x)
{
  int old_use;
  cell new_x;

  if (!pair(x)) {
    return x;
  }

  old_use = cell_i;
  gc_i = 0;

  if (the_cars == cars_a) {
    new_cars = cars_b;
    new_cdrs = cdrs_b;
  } else {
    new_cars = cars_a;
    new_cdrs = cdrs_a;
  }

  new_x = gc_cell(x);
  cell_i = gc_i;

  the_cars = new_cars;
  the_cdrs = new_cdrs;

  printf("gc complete %i -> %i (max %i)\n", old_use, cell_i, MAX_CELL);

  return new_x;
}

void dump_mem(void)
{
  int i;
  FILE *dump;

  char *car_chars = (char*)the_cars;
  char *cdr_chars = (char*)the_cdrs;

  dump = fopen("mem.dump", "w");

  if (!dump) {
    printf("can't open file -- DUMP_MEM\n");
    exit(1);
  }

  dump_sym(dump);

  putc(cell_i >> 24, dump);
  putc(cell_i >> 16, dump);
  putc(cell_i >> 8, dump);
  putc(cell_i, dump);

  for (i = 0; i < cell_i * 4; i++) {
    putc(car_chars[i], dump);
    putc(cdr_chars[i], dump);
  }

  fclose(dump);
}

void load_mem(void)
{
  int i;
  FILE *dump;
  char *car_chars = (char*)the_cars;
  char *cdr_chars = (char*)the_cdrs;

  dump = fopen("mem.dump", "r");

  if (!dump) {
    printf("can't open file -- LOAD_MEM\n");
    exit(1);
  }

  load_sym(dump);

  cell_i = 0;
  cell_i += getc(dump) << 24;
  cell_i += getc(dump) << 16;
  cell_i += getc(dump) << 8;
  cell_i += getc(dump);


  printf("reading to %i\n", cell_i);

  for (i = 0; i < cell_i * 4; i++) {
    car_chars[i] = getc(dump);
    cdr_chars[i] = getc(dump);
  }

  fclose(dump);
}

int id(cell a, cell b)
{
  return a == b;
}

int pair(cell x)
{
  return x >> CELL_SHIFT == PAIR;
}

int broken_heart(cell x)
{
  return x >> CELL_SHIFT == HEART;
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
    printf("can't CAR non-pair '");
    pr(p);
    printf("' -- CAR\n");
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
    printf("can't CDR non-pair -- CDR\n");
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
