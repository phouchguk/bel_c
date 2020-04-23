#include <stdio.h>
#include <stdlib.h>

#include "type.h";
#include "pair.h"

#define MAX_TOKEN 256
#define MAX_SYM 1024

enum escape { BELL  = '\a', TAB = '\t', NEWLINE = '\n', RETURN = '\r' };

char sym[MAX_SYM] = "nil t o apply";
int sym_i = 14;

char is_escape(char c)
{
  return c == BELL || c == TAB || c == NEWLINE || c == RETURN;
}

char is_whitespace(char c)
{
  return c < 33 || c > 126;
}

char is_quote(char c)
{
  return c == '\'' || c == '`' || c == ',';
}

char is_paren(c)
{
  return c == '(' || c == ')' || c == '[' || c == ']';
}

char is_delimiter(char c)
{
  return is_whitespace(c) || is_paren(c) || is_quote(c);
}

char token[MAX_TOKEN];
int token_i = 0;

/* if expandable and needs expand, expand into an expand buffer and pass that along instead of token buffer */
/* need to handle spaces in strings (leave escape chars until parse_string?) */

char needs_expand() {
  int i = 0;

  while (1) {
    switch (token[i++]) {
    case '\0':
      return 0;
    case '|':
    case '!':
    case '.':
    case ':':
    case '~':
      return 1;
    }
  }
}

char token_expandable() {
  if (token[1] == '\0' || token[2] == '\0') {
    /* needs to be at least 3 chars long */
    return 0;
  }

  switch (token[0]) {
  case '"':  /* string */
  case '\\': /* char */
  case '|':  /* escaped token */
    return 0;
  default:
    return 1;
  }
}

char in_char = 0;
char in_comma = 0;

void parse_token()
{
  if (token[1] == '\0' && token[0] == ',') {
    if (in_comma) {
      in_comma = 0;
    } else {
      in_comma = 1;
      return;
    }
  }

  if (token_expandable() && needs_expand()) {
    printf("EXPAND#");
  }

  printf(token);
  putchar(' ');
}

char in_comment = 0;

void parse_char(char c)
{
  if (in_comment) {
    if (c == NEWLINE) {
      in_comment = 0;
    }

    return;
  }

  if (c == ';') {
    in_comment = 1;
    return;
  }

  if (in_comma) {
    /* checking next char for @ */
    if (c == '@') {
      in_comma = 0;

      token[1] = '@';
      token[2] = '\0';

      parse_token();

      /* don't continue parsing '@' */
      return;
    }

    /* parse previous ',', continue parsing c */
    parse_token();
  }

  if (token_i == MAX_TOKEN) {
    printf("token too long -- PARSE_CHAR\n");
    exit(1);
  }

  if (is_delimiter(c) && !(in_char && (is_paren(c) || is_quote(c)))) {
    in_char = 0;

    if (token_i > 0) {
      token[token_i] = '\0';
      token_i = 0;

      parse_token();
    }

    if (is_paren(c) || is_quote(c)) {
      token[0] = c;
      token[1] = '\0';

      parse_token();
    }

    return;
  }

  if (c == '\\') {
    in_char = 1;
  }

  token[token_i++] = c;
}

void pr(struct cell c)
{
  switch (c.t) {
  case STREAM:
    printf("<stream>");
    break;

  case SYM:
    printf("%s", sym + c.val);
    break;

  case CHAR:
    printf("\\%c", c.val);
    break;

  case PAIR:
    printf("(");
    pr(car(c));
    printf(" . ");
    pr(cdr(c));
    printf(")");
    break;
  }
}

void main()
{
  int c;

  sym[3] = '\0';
  sym[5] = '\0';
  sym[7] = '\0';

  /*
  printf("%s\n", token);
  printf("%s\n", token + 4);
  printf("%s\n", token + 6);
  printf("%s\n", token + 8);

  - cell struct is bel_t and int (no point having char in an union if we have an int anyway)
  - array of file pointers (MAX_FP), when open scan for 0
  - when gc if type is file, close pointer if not closed (will be (lit fp <fp> t/nil <buff>) ), set to 0 in array

  struct cell nil = { SYM, 0 };
  struct cell t = { SYM, 4 };

  struct cell x = { SYM, 8 };

  struct cell g = { CHAR, 'g' };
  struct cell s = { CHAR, 's' };

  struct cell z1 = join(x, g);
  struct cell z2 = join(nil, z1);

  pr(z2);
  printf("\n");

  pr(car(z1));
  printf("\n");

  pr(cdr(z1));
  printf("\n");

  xar(z1, s);
  xdr(z1, t);

  xar(z2, x);

  pr(z2);
  printf("\n");
  */

  while ((c = getchar()) != EOF) {
    parse_char(c);
  }

  if (token_i > 0) {
    token[token_i] = '\0';
    parse_token();
  }
}
