#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"
#include "print.h"
#include "continuation.h"
#include "eval.h"

int make_lit3(cell a, cell b, cell c)
{
  return join(lit, join(a, join(b, join(c, 0))));
}

int make_base_cont(void)
{
  return make_lit3(cont, base, 0);
}

int make_begin_cont(cell k, cell ex, cell r, cell d)
{
  /* (lit cont do k ex r d) */
  return join(lit, join(cont, join(begin, join(k, join(ex, join(r, join(d, 0)))))));
}

int make_if_cont(cell k, cell et, cell efx, cell r, cell d)
{
  /* (lit cont if k et efx r d) */
  return join(lit, join(cont, join(iff, join(k, join(et, join(efx, join(r, join(d, 0))))))));
}

int make_next(cell k, cell val)
{
  return make_lit3(next, k, val);
}

int evaluate_begin(cell ex, cell r, cell d, cell k)
{
  if (!pair(ex)) {
    return make_next(k, 0);
  }

  if (pair(cdr(ex))) {
    return eval(car(ex), r, d, make_begin_cont(k, cdr(ex), r, d));
  }

  /* last exp */
  return eval(car(ex), r, d, k);
}

int evaluate_if(cell ec, cell et, cell efx, cell r, cell d, cell k)
{
  return eval(ec, r, d, make_if_cont(k, et, efx, r, d));
}

cell resume_begin(cell k, cell this, cell val)
{
  cell ex, r, d;

  /* (ex r d) */
  ex = car(this);
  r = car(cdr(this));
  d = car(cdr(cdr(this)));

  /*
  printf("do ignored: ");
  pr(val);
  printf("\n");
  */

  return evaluate_begin(ex, r, d, k);
}

cell resume_if(cell k, cell this, cell val)
{
  cell alt, et, efx, r, d;

  /* (et efx r d) */
  r = car(cdr(cdr(this)));
  d = car(cdr(cdr(cdr(this))));

  if (val) {
    /* consequent */
    et = car(this);
    return eval(et, r, d, k);
  }

  efx = car(cdr(this));

  if (!efx) {
    /* no alternative(s) specified, return nil */
    return make_next(k , 0);
  }

  alt = car(efx);

  if (!cdr(cdr(efx))) {
    /* last alternative */
    return eval(alt, r, d, k);
  }

  /* alternate condition */
  return evaluate_if(alt, car(cdr(efx)), cdr(cdr(efx)), r, d, k);
}

cell resume(cell this, cell val)
{
  /* (<t> k ...) */
  cell k = car(cdr(this));
  cell t = car(this);
  cell props = cdr(cdr(this));

  if (t == base) {
    pr(val);
    printf("\n");

    return 0; /* should return a next, but base stops execution */
  }

  if (t == iff) {
    return resume_if(k, props, val);
  }

  if (t == begin) {
    return resume_begin(k, props, val);
  }

  printf("unrecognised continuation -- RESUME\n");
  exit(1);
}
