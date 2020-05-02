#include "type.h"
#include "pair.h"
#include "sym.h"
#include "pg.h"

/* vmark and smark need gc */
cell globe, lock, smark, vmark;

void pg_init(void)
{
  globe = 0;
  lock = get_sym("lock");

  smark = join(0, 0);
  vmark = join(0, 0);
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
  cell ea, e, s;
  ea = car(w);
  s = cdr(w);

  e = car(ea);
  /*a = car(cdr(ea));*/

  if (literal(e)) {
    return make_k(s, join(e, r), m);
  }

  printf("bad e -- EV\n");
  exit(1);
}

/* checks stack for dynamic binding */
int binding(cell v, cell s)
{
  return 0;
}

cell pg(cell e)
{
  cell k, sr, s, r, m, p, g, x;

  /* s stack (remaining exps)
  ** r return
  ** m list of lists of p and g
  ** p pending threads ((s1 r1) (s2 r2) ...)
  ** g globe ((k . v) (k2 . v2) ...)
  ** ---
  ** k is (s r m)
  */

  /* this is 'mev' - we don't have tail calls */
  k = ev(join(l2(e, 0), 0), 0, l2(0, globe));

  while (1) {
    /* unpack continuation */
    s = car(k);
    r = car(cdr(k));
    m = car(cdr(cdr(k)));

    p = car(m);
    g = car(cdr(m));

    if (s) {
      /* work to do on current thread */
      if (cdr(binding(lock, s))) {
        /* current thread is locked - reschedule current thread (do nothing) */
      } else {
        /* schedule next thread ... */
        sr = car(p);
        s = car(sr);
        r = car(cdr(sr));

        sr = join(s, join(r, 0));
        p = cdr(p);

        if (p) {
          /* ... push current thread to the back */
          x = p;

          while (cdr(x)) {
            x = cdr(x);
          }

          xdr(x, sr);
        } else {
          p = join(sr, 0);
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
