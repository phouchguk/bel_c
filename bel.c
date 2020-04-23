#include <stdio.h>
#include <stdlib.h>

#include "type.h";
#include "pair.h"
#include "parse.h"

#define MAX_SYM 1024

char sym[MAX_SYM] = "nil t o apply";
int sym_i = 14;

void pr(struct cell c)
{
  switch (c.t) {
  case STREAM:
    printf("<stream>");
    break;

  case SYM:
    printf("%s", sym + c.val);
    break;

  case CHAR:
    printf("\\%c", c.val);
    break;

  case PAIR:
    printf("(");
    pr(car(c));
    printf(" . ");
    pr(cdr(c));
    printf(")");
    break;
  }
}

void main()
{
  int c;

  sym[3] = '\0';
  sym[5] = '\0';
  sym[7] = '\0';

  /*
  printf("%s\n", token);
  printf("%s\n", token + 4);
  printf("%s\n", token + 6);
  printf("%s\n", token + 8);

  - cell struct is bel_t and int (no point having char in an union if we have an int anyway)
  - array of file pointers (MAX_FP), when open scan for 0
  - when gc if type is file, close pointer if not closed (will be (lit fp <fp> t/nil <buff>) ), set to 0 in array

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
  */

  while ((c = getchar()) != EOF) {
    parse_char(c);
  }

  parse_token_final();
}
