#include "type.h"
#include "pair.h"
#include "sym.h"
#include "pg.h"

/* vmark and smark need gc */
cell smark, vmark;

void pg_init(void)
{
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

cell uvar(void)
{
  return join(vmark, 0);
}

cell pg(cell e)
{
  return e;
}
