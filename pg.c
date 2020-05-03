#include "type.h"
#include "pair.h"
#include "sym.h"
#include "pg.h"
#include "print.h"

cell after, ccc, err, fut, globe, loc, lock, s_globe, malformed, scope, smark, thread, vmark;

void pg_init(void)
{
  /* vmark and smark need gc */
  smark = join(0, 0);
  vmark = join(0, 0);

  /* initial global environement */
  globe = join(join(get_sym("x"), get_sym("smooth")), 0);

  /* syms used in pg */
  after = get_sym("after");
  ccc = get_sym("ccc");
  err = get_sym("err");
  fut = get_sym("fut");
  s_globe = get_sym("globe");
  loc = get_sym("loc");
  lock = get_sym("lock");
  malformed = get_sym("malformed");
  scope = get_sym("scope");
  thread = get_sym("thread");
}

int atom(cell x)
{
  return !pair(x);
}

int chr(cell x)
{
  return x >> CELL_SHIFT == CHAR;
}

int stream(cell x)
{
  return x >> CELL_SHIFT == STREAM;
}

int proper(cell x)
{
  if (atom(x)) {
    return 0;
  }

  while (x) {
    if (!pair(x)) {
      return 0;
    }

    x = cdr(x);
  }

  return 1;
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

int literal(cell e)
{
  if (e == t || e == 0 || e == o || e == apply) {
    return 1;
  }

  if (chr(e) || stream(e)) {
    return 1;
  }

  if (atom(e)) {
    return 0;
  }


  if (car(e) == lit) {
    return 1;
  }

  return string(e);
}

int variable(e)
{
  if (atom(e)) {
    return !literal(e);
  }

  return car(e) == vmark;
}

cell uvar(void)
{
  return join(vmark, 0);
}

cell l2(a, b) {
  return join(a, join(b, 0));
}

cell l3(a, b, c) {
  return join(a, join(b, join(c, 0)));
}

cell make_k(s, r, m)
{
  return l3(s, r, m);
}

cell applyf(cell f, cell args, cell a, cell s, cell r, cell m)
{
  return make_k(0, 0, 0);
}

/* checks stack for dynamic binding */
int binding(cell v, cell s)
{
  return 0;
}

cell sigerr(msg, s, r, m)
{
  cell b;
  b = binding(err, s);

  if (b) {
    return applyf(cdr(b), join(msg, 0), 0, s, r, m);
  } else {
    printf("no err - SIGERR\n");
    exit(1);
  }
}

int inwhere(cell s)
{
  cell e;

  e = car(car(s));

  if (car(e) == smark && car(cdr(e)) == loc) {
    return cdr(cdr(e));
  }

  return 0;
}

cell get(cell k, cell kvs)
{
  while (kvs) {
    if (car(car(kvs)) == k) {
      return car(kvs);
    }

    kvs = cdr(kvs);
  }

  return 0;
}

cell lookup(cell e, cell a, cell s, cell g)
{
  cell val;

  val = binding(e, s);
  if (val) {
    return val;
  }

  val = get(e, a);
  if (val) {
    return val;
  }

  val = get(e, g);
  if (val) {
    return val;
  }

  if (e == scope) {
    return join(e, a);
  }

  if (e == s_globe) {
    return join(e, g);
  }

  return 0;

}

cell vref(cell v, cell a, cell s, cell r, cell m)
{
  cell g, l;

  g = car(cdr(m));

  if (inwhere(s)) {
    return 0;
  } else {
    l = lookup(v, a, s, g);

    if (l) {
      return make_k(s, join(cdr(l), r), m);
    } else {
      return sigerr(l2(unbound, v), s, r, m);
    }
  }
}

int special(cell x)
{
  return x == quote || x == iff || x == where || x == dyn || x == after || x == ccc || x == thread || x == smark;
}

cell fu(cell op, cell args)
{
  cell l;

  l = join(smark, join(fut, join(op, args)));
  return join(l, 0);
}

cell evfut(cell e, cell s, cell r, cell m)
{
  cell a, args, es, f, result;
  f = car(e);

  if (f == iff) {
    args = cdr(e);
    es = car(args);
    a = car(cdr(args));

    if (car(r)) {
      result = car(es);
    } else {
      result = join(iff, cdr(es));
    }

    return make_k(join(l2(result, a), s), cdr(r), m);
  }

  printf("bad fut '%s' -- EVFUT\n", nom(f));
  exit(1);
}

cell evmark(cell e, cell a, cell s, cell r, cell m)
{
  cell mark;
  mark = car(e);

  if (mark == fut) {
    /* futs use their own a's */
    return evfut(cdr(e), s, r, m);
  }

  printf("bad mark '%s' -- EVMARK\n", nom(mark));
  exit(1);
}

cell ev_special(cell op, cell es, cell a, cell s, cell r, cell m)
{
  cell alt;

  if (op == smark) {
    return evmark(es, a, s, r, m);
  }

  if (op == quote) {
    return make_k(s, join(car(es), r), m);
  }

  if (op == iff) {
    if (es) {
      alt = s;

      if (cdr(es)) {
        alt = join(fu(iff, l2(cdr(es), a)), s);
      }

      return make_k(join(l2(car(es), a), alt), r, m);
    } else {
      return make_k(s, join(0, r), m);
    }
  }

  printf("bad special -- EV_SPECIAL\n");
  exit(1);
}

/* w = work ((e a .))
** e = expression
** a = environment
** s = stack ((e a) (e a) (e a) ...)
** m = (p g)

** r = return value
** p = pending threads
** global environment
*/
cell ev(cell w, cell r, cell m)
{
  cell ea, a, e, s, op;
  ea = car(w);
  s = cdr(w);

  e = car(ea);
  a = car(cdr(ea));

  if (literal(e)) {
    return make_k(s, join(e, r), m);
  }

  if (variable(e)) {
    return vref(e, a, s, r, m);
  }

  if (!proper(e)) {
    return sigerr(malformed, s, r, m);
  }

  op = car(e);

  if (special(op)) {
    return ev_special(op, cdr(e), a, s, r, m);
  }

  printf("bad e -- EV\n");
  exit(1);
}

cell pg(cell e)
{
  cell b, csr, k, sr, s, r, m, p, g, x;

  /* s stack (remaining exps)
  ** r return
  ** m list of lists of p and g
  ** p pending threads ((s1 r1) (s2 r2) ...)
  ** g globe ((k . v) (k2 . v2) ...)
  ** ---
  ** k is (s r m)
  */

  /* this is 'mev' - we don't have tail calls */
  k = ev(join(l2(e, join(join(get_sym("x"), get_sym("groovy")),0)), 0), 0, l2(0, globe));

  while (1) {
    /* unpack continuation */
    s = car(k);
    r = car(cdr(k));
    m = car(cdr(cdr(k)));

    p = car(m);
    g = car(cdr(m));

    if (s) {
      b = binding(lock, s);

      /* work to do on current thread */
      if (b && cdr(b)) {
        /* current thread is locked - reschedule current thread (do nothing) */
      } else {
        if (p) {
          /* schedule next thread */
          csr = join(s, join(r, 0));

          sr = car(p);
          s = car(sr);
          r = car(cdr(sr));

          p = cdr(p);

          /* ... push current thread to the back */
          x = p;

          while (cdr(x)) {
            x = cdr(x);
          }

          xdr(x, csr);
        } else {
          /* same thread again */
        }
      }
    } else {
      if (p) {
        /* current thread is finished. any other pending work? */
        sr = car(p);
        s = car(sr);
        r = car(cdr(sr));
        p = cdr(p);
      } else {
        /* finished - return top of return stack */
        return car(r);
      }
    }

    k = ev(s, r, l2(p, g));
  }
}
