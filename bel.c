#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "parse.h"

int main()
{
  int c;

  while ((c = getchar()) != EOF) {
    parse_char(c);
  }

  parse_token_final();
}

/*
  - cell struct is bel_t and int (no point having char in an union if we have an int anyway)
  - array of file pointers (MAX_FP), when open scan for 0
  - when gc if type is file, close pointer if not closed (will be (lit fp <fp> t/nil <buff>) ), set to 0 in array
*/
