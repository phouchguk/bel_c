#include <stdio.h>
#include <stdlib.h>

#define TAG_BIT_SIZE 3

typedef unsigned int cell;
enum belt { SYM = 0, PAIR = 1, CHAR = 2, STREAM = 3, NUM = 4, HEART = 5 };

const size_t bc = sizeof(size_t);
const int cell_shift = (bc * 8) - TAG_BIT_SIZE;

int is_self_evaluating(cell c)
{
  return c >> cell_shift > 1;
}

int main()
{
  /* filled with 1s */
  const cell x = ~0U;
  const cell data_mask = x >> TAG_BIT_SIZE;

  const cell sym_mask    = 0;
  const cell pair_mask   = PAIR << cell_shift;
  const cell char_mask   = CHAR << cell_shift;
  const cell stream_mask = STREAM << cell_shift;
  const cell num_mask    = NUM << cell_shift;
  const cell heart_mask  = HEART << cell_shift;

  const cell nil = 0 | sym_mask;
  const cell p = 0 | pair_mask;
  const cell c = 'g' | char_mask;
  const cell n = 42 | num_mask;

  printf("%i\n", bc);
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

  printf("t nil: %i\n", nil >> cell_shift);
  printf("t p: %i\n", p >> cell_shift);
  printf("t c: %i\n", c >> cell_shift);
  printf("t n: %i\n", n >> cell_shift);

  printf("se nil: %i\n", is_self_evaluating(nil));
  printf("se p: %i\n", is_self_evaluating(p));
  printf("se c: %i\n", is_self_evaluating(c));
  printf("se n: %i\n", is_self_evaluating(n));

  return 0;
}
