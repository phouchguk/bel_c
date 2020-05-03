#include "type.h"
#include "pair.h"
#include "sym.h"
#include "pg.h"
#include "print.h"

cell after, bind, cannot_bind, ccc, err, fut, globe, loc, lock, s_d, s_globe, malformed, scope, smark, thread, unfindable, vmark;

void pg_init(void)
{
  /* vmark and smark need gc */
  smark = join(0, 0);
  vmark = join(0, 0);

  /* initial global environement */
  globe = join(join(get_sym("x"), get_sym("smooth")), 0);

  /* syms used in pg */
  after = get_sym("after");
  bind = get_sym("bind");
  cannot_bind = get_sym("cannot-bind");
  ccc = get_sym("ccc");
  err = get_sym("err");
  fut = get_sym("fut");
  s_d = get_sym("d");
  s_globe = get_sym("globe");
  loc = get_sym("loc");
  lock = get_sym("lock");
  malformed = get_sym("malformed");
  scope = get_sym("scope");
  thread = get_sym("thread");
  unfindable = get_sym("unfindable");
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
  cell b, x;

  while (s) {
    x = car(s);

    if (pair(car(x))) {
      x = car(x);

      /*
      pr(car(x));
      printf("\n%u\n", pair(car(x)) && car(car(x)) == smark);
      */

      if (car(x) == smark && car(cdr(x)) == bind) {
        b = car(cdr(cdr(x)));

        if (car(b) == v) {
          return b;
        }
      }
    }

    s = cdr(s);
  }

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
  cell g, it, l, w;

  g = car(cdr(m));
  l = lookup(v, a, s, g);
  w = inwhere(s);

  if (w) {
    if (l) {
      it = l;
    } else {
      /* if new create variable */
      if (car(w)) {
        it = join(v, 0);
        xdr(g, join(it, cdr(g)));
      } else {
        return sigerr(unbound, s, r, m);
      }
    }

    return make_k(cdr(s), join(l2(it, s_d), r), m);
  } else {
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
  cell a, b, args, e2, es, f, result, v;

  f = car(e);
  args = cdr(e);

  if (f == iff) {
    es = car(args);
    a = car(cdr(args));

    if (car(r)) {
      result = car(es);
    } else {
      result = join(iff, cdr(es));
    }

    return make_k(join(l2(result, a), s), cdr(r), m);
  }

  if (f == dyn) {
    v = car(args);
    e2 = car(cdr(args));
    a = car(cdr(cdr(args)));

    e = l2(e2, a);
    b = l2(join(smark, join(bind, join(join(v, car(r)), 0))), 0);

    return make_k(join(e, join(b, s)), r, m);
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

  if (mark == bind) {
    return make_k(s, r, m);
  }

  if (mark == loc) {
    return sigerr(unfindable, s, r, m);
  }

  printf("bad mark '%s' -- EVMARK\n", nom(mark));
  exit(1);
}

cell ev_special(cell op, cell es, cell a, cell s, cell r, cell m)
{
  cell alt, e, e1, e2, f, mark, new, v;

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

  if (op == where) {
    e = car(es);
    new = pair(cdr(es)) ? car(cdr(es)) : 0;
    mark = join(smark, join(loc, join(new, 0)));

    return make_k(join(l2(e, a), join(join(mark, 0), s)), r, m);
  }

  if (op == dyn) {
    v = car(es);

    if (!variable(v)) {
      return sigerr(cannot_bind, s, r, m);
    }

    e1 = car(cdr(es));
    e2 = car(cdr(cdr(es)));

    e = l2(e1, a);
    f = fu(dyn, l3(v, e2, a));

    return make_k(join(e, join(f, s)), r, m);
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
  x = join(get_sym("a"), join(get_sym("b"), join(get_sym("c"), 0)));
  k = ev(join(l2(e, join(join(get_sym("x"), x), 0)), 0), 0, l2(0, globe));

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
