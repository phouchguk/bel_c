#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "print.h"
#include "sym.h"

void pr(cell c)
{
  char chr;
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
    print_sym(c);

    break;

  case CHAR:
    chr = c | CHAR << CELL_SHIFT;

    switch (chr) {
    case ' ':
      printf("\\sp");
      break;
    case '\r':
      printf("\\lf");
      break;

    case '\n':
      printf("\\cr");
      break;

    case '\a':
      printf("\\bel");
      break;

    case '\t':
      printf("\\tab");
      break;

    default:
      printf("\\%c", chr);
      break;
    }

    break;

  case PAIR:
    /*
    if (car(c) == lit) {
      printf("<%s>", nom(car(cdr(c))));
      break;
    }
    */

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
