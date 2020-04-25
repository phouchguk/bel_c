#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"
#include "sym.h"

void pr(cell c)
{
  int first = 1;

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

    while (c) {
      if (!first) {
        printf(" ");
      }

      if (!pair(c)) {
        printf(". ");
        pr(c);
        break;
      }

      pr(car(c));
      c = cdr(c);

      first = 0;
    }

    printf(")");
    break;
  }
}
