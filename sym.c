#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"

#define MAX_SYM 1024

char sym[MAX_SYM] = "nil t o apply";
int sym_i = 14;

char *nom(struct cell c)
{
  if (c.t != SYM) {
    printf("can't NOM non-sym -- NOM\n");
    exit(1);
  }

  sym[3] = '\0';
  sym[5] = '\0';
  sym[7] = '\0';

  return sym + c.val;
}
