#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"

#define MAX_SYM 1024

char sym[MAX_SYM];
int sym_i = 0;
int uniq_i = 0;

int uniq(void)
{
  return MAX_SYM + uniq_i++;
}

struct cell get_sym(char *str)
{
  int i, idx, j;
  struct cell c;

  c.t = SYM;

  /* find existing symbol position */
  for (i = 0; i < sym_i; i++) {
    idx = i;

    for (j = 0; sym[i] == str[j] && i < sym_i; j++, i++) {
      if (sym[i] == '\0') {
        /* match ! */
        c.val = idx;
        return c;
      }
    }

    /* skip to next sym */
    while (sym[++i] != '\0' && i < sym_i);
  }

  /* create new symbol */
  c.val = sym_i;
  while ((sym[sym_i++] = *str++) != '\0');

  return c;
}

char *nom(struct cell c)
{
  if (c.t != SYM) {
    printf("can't NOM non-sym -- NOM\n");
    exit(1);
  }

  return sym + c.val;
}

void sym_init(void)
{
  /* want nil and t first */
  nil = get_sym("nil");
  t = get_sym("t");

  /* alphabetical after */
  apply = get_sym("apply");
  o = get_sym("o");
}
