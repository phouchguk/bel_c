#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"
#include "print.h"
#include "continuation.h"
#include "env.h"
#include "eval.h"

cell make_lit3(cell a, cell b, cell c)
{
  return join(lit, join(a, join(b, join(c, 0))));
}

cell make_base_cont(void)
{
  /* (lit cont base nil) */
  return make_lit3(cont, base, 0);
}

cell make_where_cont(cell k)
{
  /* (lit cont where k) */
  return join(lit, join(cont, join(where, join(k, 0))));
}

cell make_begin_cont(cell k, cell ex, cell r, cell d)
{
  /* (lit cont do k ex r d) */
  return join(lit, join(cont, join(begin, join(k, join(ex, join(r, join(d, 0)))))));
}

cell make_if_cont(cell k, cell et, cell efx, cell r, cell d)
{
  /* (lit cont if k et efx r d) */
  return join(lit, join(cont, join(iff, join(k, join(et, join(efx, join(r, join(d, 0))))))));
}

cell make_set_cont(cell k, cell var, cell r, cell d)
{
  /* (lit cont set k var r d) */
  return join(lit, join(cont, join(set, join(k, join(var, join(r, join(d, 0)))))));
}

cell make_next(cell k, cell val)
{
  return make_lit3(next, k, val);
}

cell evaluate_begin(cell ex, cell r, cell d, cell k)
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

cell evaluate_if(cell ec, cell et, cell efx, cell r, cell d, cell k)
{
  return eval(ec, r, d, make_if_cont(k, et, efx, r, d));
}

cell evaluate_where(cell e, cell r, cell d, cell k)
{
  return eval(e, r, d, make_where_cont(k));
}

cell evaluate_set(cell var, cell e, cell r, cell d, cell k)
{
  return eval(e, r, d, make_set_cont(k, var, r, d));
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

cell resume_set(cell k, cell this, cell val)
{
  cell var, r, d;

  /* (var r d) */
  var = car(this);
  r = car(cdr(this));
  d = car(cdr(cdr(this)));

  return set_variable(var, val, r, d, k);
}

cell resume_where(cell k)
{
  cell w = get_where();

  if (!w) {
    printf("bad where -- RESUME_WHERE");
    exit(1);
  }

  return w;
}

cell resume(cell this, cell val)
{
  /* (<t> k ...) */
  cell k, props;
  cell t = car(this);

  if (t == base) {
    pr(val);
    printf("\n");

    return 0; /* should return a next, but base stops execution */
  }

  k = car(cdr(this));

  if (t == where) {
    return resume_where(k);
  }

  props = cdr(cdr(this));

  if (t == iff) {
    return resume_if(k, props, val);
  }

  if (t == begin) {
    return resume_begin(k, props, val);
  }

  if (t == set) {
    return resume_set(k, props, val);
  }

  printf("unrecognised continuation -- RESUME\n");
  exit(1);
}
