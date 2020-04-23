#include <stdio.h>
#include <stdlib.h>

#include "parse.h"

#define MAX_TOKEN 256

enum escape { BELL  = '\a', TAB = '\t', NEWLINE = '\n', RETURN = '\r' };

char is_escape(char c)
{
  return c == BELL || c == TAB || c == NEWLINE || c == RETURN;
}

char is_whitespace(char c)
{
  return c < 33 || c > 126;
}

char is_valid(char c)
{
  return is_escape(c) || c > 31 || c < 127;
}

char is_quote(char c)
{
  return c == '\'' || c == '`' || c == ',';
}

char is_paren(char c)
{
  return c == '(' || c == ')' || c == '[' || c == ']';
}

char is_delimiter(char c)
{
  return is_whitespace(c) || is_paren(c) || is_quote(c);
}

char literal = '\0';
char token[MAX_TOKEN];
int token_i = 0;

/* if expandable and needs expand, expand into an expand buffer and pass that along instead of token buffer */
/* need to handle spaces in strings/|symbols| (leave escape chars until parse_string?) */

char needs_expand(void) {
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

char token_expandable(void) {
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

void parse_token(char *str, int start, int end)
{
  int i, j;
  char tok[(end - start) + 1];

  for (i = start, j = 0; i < end; i++, j++) {
    tok[j] = str[i];
  }

  tok[j] = '\0';

  printf(tok);
  putchar(' ');
}

char op[] = "(";
char cp[] = ")";
char tt[] = "t";
char tq[] = "'";
char upon[] = "upon";

void expand_token(char *str, int start, int end)
{
  int i;

  // check for |
  for (i = start; i < end; i++) {
    if (str[i] == '|') {
      parse_token(op, 0, 1);
      parse_token(tt, 0, 1);
      parse_token(str, start, i);
      expand_token(str, i + 1, end);
      parse_token(cp, 0, 1);

      return;
    }
  }

  char dot_or_bang = 0;

  for (i = start; i < end; i++) {
    if (str[i] == '.' || str[i] == '!') {
      dot_or_bang = 1;
      break;
    }
  }

  if (dot_or_bang) {
    parse_token(op, 0, 1);

    int last = start;

    for (i = start; i < end; i++) {
      if (str[i] == '.' || str[i] == '!') {
        if (i == 0) {
          parse_token(upon, 0, 4);
        }

        // if last == i bang/dot together -- bad?

        expand_token(str, last, i);

        if (str[i] == '!') {
          parse_token(tq, 0, 1);
        }

        last = i + 1;
      }
    }

    expand_token(str, last, end);

    parse_token(cp, 0, 1);

    return;
  }

  /*
  if (token_expandable() && needs_expand()) {
    printf("EXPAND#");
  }
  */

  parse_token(str, start, end);
}

void read_token(void)
{
  if (token[1] == '\0' && token[0] == ',') {
    if (in_comma) {
      in_comma = 0;
    } else {
      in_comma = 1;
      return;
    }
  }

  expand_token(token, 0, token_i);

  /*
  if (token_expandable() && needs_expand()) {
    expand_token(token, 0, token_i);
  } else {
    parse_token(token, 0, token_i);
  }

  printf(token);
  printf("%i %i\n", 0, token_i);
  */

  token_i = 0;
}

void parse_token_final(void) {
  if (token_i > 0) {
    token[token_i] = '\0';
    read_token();
  }

  printf("\n");
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
      token_i = 2;

      read_token();

      /* don't continue parsing '@' */
      return;
    }

    /* parse previous ',', continue parsing c */
    token_i = 1;
    read_token();
  }

  if (token_i == MAX_TOKEN) {
    printf("token too long -- PARSE_CHAR\n");
    exit(1);
  }

  if (is_delimiter(c) && !(in_char && (is_paren(c) || is_quote(c)))) {
    in_char = 0;

    if (token_i > 0) {
      token[token_i] = '\0';

      read_token();
    }

    if (is_paren(c) || is_quote(c)) {
      token[0] = c;
      token[1] = '\0';
      token_i = 1;

      read_token();
    }

    return;
  }

  if (c == '\\') {
    in_char = 1;
  }

  token[token_i++] = c;
}
