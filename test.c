#include <stdio.h>
#include <stdlib.h>

#define TAG_BIT_SIZE 3
#define CELL_SHIFT 29

typedef unsigned int cell;
enum belt { SYM = 0, PAIR = 1, CHAR = 2, STREAM = 3, NUM = 4, HEART = 5 };

int is_self_evaluating(cell c)
{
  return c >> CELL_SHIFT > 1;
}

int main()
{
  const cell x = ~0U;
  const cell data_mask = x >> TAG_BIT_SIZE;
  const cell sym_mask    = 0;

  const cell pair_mask   = PAIR << CELL_SHIFT;
  const cell char_mask   = CHAR << CELL_SHIFT;
  const cell stream_mask = STREAM << CELL_SHIFT;
  const cell num_mask    = NUM << CELL_SHIFT;
  const cell heart_mask  = HEART << CELL_SHIFT;

  const cell nil = 0 | sym_mask;
  const cell p = 0 | pair_mask;
  const cell c = 'g' | char_mask;
  const cell n = 42 | num_mask;

  printf("x:           %u\n", x);
  printf("data_mask:   %u\n", data_mask);
  printf("sym_mask:    %u\n", sym_mask);
  printf("pair_mask:   %u\n", pair_mask);
  printf("char_mask:   %u\n", char_mask);
  printf("stream_mask: %u\n", stream_mask);
  printf("num_mask:    %u\n", num_mask);
  printf("heart_mask:  %u\n", heart_mask);

  printf("nil: %u\n", nil);
  printf("p: %u\n", p);
  printf("c: %u\n", c);
  printf("n: %u\n", n);

  printf("val nil: %i\n", nil & data_mask);
  printf("val p: %i\n", p & data_mask);
  printf("val c: %i\n", c & data_mask);
  printf("val n: %i\n", n & data_mask);

  printf("t nil: %i\n", nil >> CELL_SHIFT);
  printf("t p: %i\n", p >> CELL_SHIFT);
  printf("t c: %i\n", c >> CELL_SHIFT);
  printf("t n: %i\n", n >> CELL_SHIFT);

  printf("se nil: %i\n", is_self_evaluating(nil));
  printf("se p: %i\n", is_self_evaluating(p));
  printf("se c: %i\n", is_self_evaluating(c));
  printf("se n: %i\n", is_self_evaluating(n));

  return 0;
}
