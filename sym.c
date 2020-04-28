#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"

#define MAX_SYM 4096

char sym[MAX_SYM];
int sym_i = 0;
int uvar_i = MAX_SYM;

void dump_sym(FILE *f)
{
  int i;

  putc(sym_i >> 24, f);
  putc(sym_i >> 16, f);
  putc(sym_i >> 8, f);
  putc(sym_i, f);

  for (i = 0; i < sym_i; i++) {
    putc(sym[i], f);
  }
}

void load_sym(FILE *f)
{
  int i;

  sym_i = 0;
  sym_i += getc(f) << 24;
  sym_i += getc(f) << 16;
  sym_i += getc(f) << 8;
  sym_i += getc(f);

  for (i = 0; i < sym_i; i++) {
    sym[i] = getc(f);
  }
}

void sym_report(void)
{
  printf("sym: %i/%ib\n", sym_i, MAX_SYM);
}

int get_uvar(void)
{
  return uvar_i++;
}

int get_sym(char *str)
{
  int i, idx, j;

  /* find existing symbol position */
  for (i = 0; i < sym_i; i++) {
    idx = i;

    for (j = 0; sym[i] == str[j] && i < sym_i; j++, i++) {
      if (str[j] == '\0' && sym[i] == '\0') {
        /* match ! */
        return idx;
      }
    }

    /* skip to next sym if we haven't already scanned to the end of a word (mac/macro) */
    while (sym[i] != '\0' && sym[++i] != '\0' && i < sym_i);
  }

  /* create new symbol */
  idx = sym_i;
  while ((sym[sym_i++] = *str++) != '\0') {
    if (sym_i == MAX_SYM) {
      printf("out of sym mem -- GET_SYM\n");
      exit(1);
    }
  };

  return idx;
}

int symbol(cell x)
{
  return x >> CELL_SHIFT == SYM;
}

char *nom(cell c)
{
  if (!symbol(c)) {
    printf("can't NOM non-sym -- NOM\n");
    exit(1);
  }

  return sym + c;
}

void print_sym(cell c)
{
  if (c >= MAX_SYM) {
    printf("_g%i", c - MAX_SYM);
  } else {
    printf("%s", nom(c));
  }
}

void sym_init(void)
{
  /* want nil and t first - c code presumes sym nil is 0 */
  get_sym("nil"); /* nil refered to as '0' in c code */
  t = get_sym("t");

  /* alphabetical after */
  a = get_sym("a");
  apply = get_sym("apply");
  argument = get_sym("argument");
  base = get_sym("base");
  begin = get_sym("do");
  bquote = get_sym("bquote");
  clo = get_sym("clo");
  comma = get_sym("comma");
  comma_at = get_sym("comma_at");
  cont = get_sym("cont");
  d = get_sym("d");
  dyn = get_sym("dyn");
  evfn = get_sym("evfn");
  gather = get_sym("gather");
  iff = get_sym("if");
  lambda = get_sym("fn");
  lit = get_sym("lit");
  mac = get_sym("mac");
  macro = get_sym("macro");
  next = get_sym("next");
  o = get_sym("o");
  prim = get_sym("prim");
  quote = get_sym("quote");
  set = get_sym("set");
  s_car = get_sym("car");
  s_cdr = get_sym("cdr");
  s_id = get_sym("id");
  s_join = get_sym("join");
  s_type = get_sym("type");
  s_xar = get_sym("xar");
  s_xdr = get_sym("xdr");
  s_nom = get_sym("nom");
  s_sym = get_sym("sym");
  s_coin = get_sym("coin");
  unbound = get_sym("*unbound*");
  uvar = get_sym("uvar");
  where = get_sym("where");
}
