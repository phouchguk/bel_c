#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"

int main()
{
  struct cell nil = { SYM, 0 };
  struct cell t = { SYM, 4 };

  struct cell x = { SYM, 8 };

  struct cell g = { CHAR, 'g' };
  struct cell s = { CHAR, 's' };

  struct cell z1 = join(x, g);
  struct cell z2 = join(nil, z1);

  pr(z2);
  printf("\n");

  pr(car(z1));
  printf("\n");

  pr(cdr(z1));
  printf("\n");

  xar(z1, s);
  xdr(z1, t);

  xar(z2, x);

  pr(z2);
  printf("\n");

  int i = 2 + 3;
  i += 1;

  char xs[i];

  xs[0] = 'h';
  xs[1] = 'e';
  xs[2] = 'l';
  xs[3] = 'l';
  xs[4] = 'o';
  xs[5] = '\0';

  printf("%s\n", xs);
}
