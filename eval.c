#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"
#include "continuation.h"
#include "env.h"
#include "print.h"
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

cell evaluate_quote(cell e, cell k)
{
  return make_next(k, e);
}

cell fn(cell parms, cell body, cell r)
{
  return join(lit, join(clo, join(r, join(parms, body))));
}

cell evaluate_lambda(cell parms, cell body, cell r, cell k)
{
  return make_next(k, fn(parms, body, r));
}

cell evaluate_macro(cell parms, cell body, cell r, cell k)
{
  return make_next(k, join(lit, join(mac, join(fn(parms, body, r), 0))));
}

cell eval(cell e, cell r, cell d, cell k)
{
  cell op;

  if (self_evaluating(e)) {
    return evaluate_quote(e, k);
  }

  if (atom(e)) {
    return lookup_variable(e, r, d, k);
  }

  op = car(e);

  if (op == quote) {
    return evaluate_quote(car(cdr(e)), k);
  }

  if (op == iff) {
    return evaluate_if(car(cdr(e)), car(cdr(cdr(e))), cdr(cdr(cdr(e))), r, d, k);
  }

  if (op == dyn) {
    return make_next(k, 0);
  }

  if (op == uvar) {
    return make_next(k, get_uvar());
  }

  if (op == where) {
    return make_next(k, 0); /*evaluate_where(car(cdr(e)), r, d, k);*/
  }

  /* looking at the bel source I don't think I actually need begin, set, lambda or macro - they're just macros */
  if (op == set) {
    return evaluate_set(car(cdr(e)), car(cdr(cdr(e))), r, d, k);
  }

  if (op == begin) {
    return evaluate_begin(cdr(e), r, d, k);
  }

  if (op == lambda) {
    return evaluate_lambda(car(cdr(e)), cdr(cdr(e)), r, k);
  }

  if (op == macro) {
    return evaluate_macro(car(cdr(e)), cdr(cdr(e)), r, k);
  }
  /* only needed til we can load binary dump */

  printf("ERR don't know how to eval %i, %i '", op, macro);
  pr(op);
  pr(op);
  printf("' -- EVAL\n");
  exit(1);
}
