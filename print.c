#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"
#include "sym.h"

void pr(struct cell c)
{
  switch (c.t) {
  case STREAM:
    printf("<stream>");
    break;

  case SYM:
    printf("%s", nom(c));
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
