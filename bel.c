#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "parse.h"
#include "sym.h"
#include "env.h"
#include "pg.h"

void load_dump()
{
  load_mem();
}

void load_stdin()
{
  int c;

  sym_init();
  pg_init();

  while ((c = getchar()) != EOF) {
    parse_char(c);
  }

  parse_token_final();
}

int main()
{
  /*
  load_dump();
  */

  load_stdin();

  pair_report();
  sym_report();

  /*
  dump_mem();
  */

  return 0;
}

/*
  - cell struct is bel_t and int (no point having char in an union if we have an int anyway)
  - array of file pointers (MAX_FP), when open scan for 0
  - when gc if type is file, close pointer if not closed (will be (lit fp <fp> t/nil <buff>) ), set to 0 in array
*/
