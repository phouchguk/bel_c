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

cell make_cont4(cell t, cell k, cell a, cell b, cell c)
{
  /* (lit cont t k a b c) */
  return join(lit, join(cont, join(t, join(k, join(a, join(b, join(c, 0)))))));
}

cell make_begin_cont(cell k, cell ex, cell r, cell d)
{
  /* (lit cont do k ex r d) */
  return make_cont4(begin, k, ex, r, d);
}

cell make_if_cont(cell k, cell et, cell efx, cell r, cell d)
{
  /* (lit cont if k et efx r d) */
  return join(lit, join(cont, join(iff, join(k, join(et, join(efx, join(r, join(d, 0))))))));
}

cell make_set_cont(cell k, cell var, cell r, cell d)
{
  /* (lit cont set k var r d) */
  return make_cont4(set, k, var, r, d);
}

cell make_evfn_cont(cell k, cell args, cell r, cell d)
{
  /* (lit cont evfn k args r d) */
  return make_cont4(evfn, k, args, r, d);
}

cell make_apply_cont(cell k, cell op)
{
  /* (lit cont apply k op) */
  return join(lit, join(cont, join(apply, join(k, join(op, 0)))));
}

cell make_gather_cont(cell k, cell arg)
{
  /* (lit cont gather k arg) */
  return join(lit, join(cont, join(gather, join(k, join(arg, 0)))));
}

cell make_argument_cont(cell k, cell args, cell r, cell d)
{
  /* (lit cont evfn k args r d) */
  return make_cont4(argument, k, args, r, d);
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
  reset_loc();
  return eval(e, r, d, make_where_cont(k));
}

cell evaluate_set(cell var, cell e, cell r, cell d, cell k)
{
  return eval(e, r, d, make_set_cont(k, var, r, d));
}

cell evaluate_application(cell op, cell args, cell r, cell d, cell k)
{
  return eval(op, r, d, make_evfn_cont(k, args, r, d));
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

cell evaluate_arguments(cell args, cell r, cell d, cell k)
{
  if (pair(args)) {
    return eval(car(args), r, d, make_argument_cont(k, cdr(args), r, d));
  }

  /* no more arguments */
  return make_next(k, 0);
}

cell resume_evfn(cell k, cell this, cell op)
{
  cell args, r, d;

  args = car(this);
  r = car(cdr(this));
  d = car(cdr(cdr(this)));

  /* is it macro, is it apply ?? presume not for now */
  return evaluate_arguments(args, r, d, make_apply_cont(k, op));
}

cell resume_argument(cell k, cell this, cell arg)
{
  cell remaining, r, d;

  remaining = car(this);
  r = car(cdr(this));
  d = car(cdr(cdr(this)));

  return evaluate_arguments(remaining, r, d, make_gather_cont(k, arg));
}

cell resume_gather(cell k, cell this, cell args)
{
  cell arg = car(this);

  /* strip '(lit cont' from 'k' */
  return resume(cdr(cdr(k)), join(arg, args));
}

cell apply_prim(cell k, cell prim, cell args)
{
  cell val = 0;

  if (prim == s_join) {
    if (args) {
      val = join(car(args), cdr(args) ? car(cdr(args)) : 0);
    } else {
      val = join(0, 0);
    }
  }

  if (prim == s_car) {
    val = bel_car(car(args));
  }

  if (prim == s_cdr) {
    val = bel_cdr(car(args));
  }

  return make_next(k, val);
}

cell resume_apply(cell k, cell this, cell args)
{
  /* might be apply ??
   * prim or clo
   * only handling prim for now
   */

  cell f = car(this);
  cell t = car(cdr(f));

  if (t == prim) {
    return apply_prim(k, car(cdr(cdr(f))), args);
  }

  printf("can't apply -- RESUME_APPLY\n");
  exit(1);
}

cell resume_where(cell k)
{
  cell w = get_where();

  if (!w) {
    printf("bad where -- RESUME_WHERE\n");
    exit(1);
  }

  return make_next(k, w);
}

cell resume(cell this, cell val)
{
  /* (<t> k ...) */
  cell k, props;
  cell t = car(this);

  if (t == base) {
    pr(val);
    printf("\n\n");

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

  if (t == evfn) {
    return resume_evfn(k, props, val);
  }

  if (t == argument) {
    return resume_argument(k, props, val);
  }

  if (t == apply) {
    return resume_apply(k, props, val);
  }

  if (t == gather) {
    return resume_gather(k, props, val);
  }

  printf("unrecognised continuation '%s' -- RESUME\n", nom(t));
  exit(1);
}
