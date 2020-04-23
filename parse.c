#include <stdio.h>
#include <stdlib.h>

#include "parse.h";

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

void parse_token_final(void) {
  if (token_i > 0) {
    token[token_i] = '\0';
    parse_token();
  }
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
