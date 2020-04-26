#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"
#include "continuation.h"
#include "eval.h"

/*
 * exp, env, denv, k -> next
 * next: (lit next k val)
 * (lit cont base k f) where f exp->void - hardcode base to printf so don't need f
 * resume would get (base k f), assign k for all, switch on car (i.e. base)
 */

int atom(cell x)
{
  return !pair(x);
}

int chr(cell x)
{
  return x >> CELL_SHIFT == CHAR;
}

int tagged_list(cell x, int tag)
{
  return pair(x) && car(x) == tag;
}

int string(cell x)
{
  if (atom(x)) {
    return 0;
  }

  while (x) {
    if (!pair(x)) {
      return 0;
    }

    if (!chr(car(x))) {
      return 0;
    }

    x = cdr(x);
  }

  return 1;
}

int self_evaluating(cell e) {
  return e == 0 || e == o || e == t || e == apply || chr(e) || tagged_list(e, lit) || string(e);
}

int evaluate_quote(cell e, cell k)
{
  return make_next(k, e);
}

cell eval(cell e, cell r, cell d, cell k)
{
  cell op;

  if (self_evaluating(e)) {
    return evaluate_quote(e, k);
  }

  if (atom(e)) {
    /* evaluate_variable */
    return 0;
  }

  op = car(e);

  if (op == iff) {
    return evaluate_if(car(cdr(e)), car(cdr(cdr(e))), cdr(cdr(cdr(e))), r, d, k);
  }

  return 0;
}
