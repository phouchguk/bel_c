#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"
#include "sym.h"

void pr(cell c)
{
  switch (c >> CELL_SHIFT) {
  case STREAM:
    printf("<stream>");
    break;

  case HEART:
    printf("<broken-heart>");
    break;

  case NUM:
    printf("<num:%u>", c | NUM << CELL_SHIFT);
    break;

  case SYM:
    printf("%s", nom(c));
    break;

  case CHAR:
    printf("\\%c", c | CHAR << CELL_SHIFT);
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
