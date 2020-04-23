#include<stdio.h>

#define MAX_TOKEN 256

char token[MAX_TOKEN];
int token_i = 0;

enum escapes { BELL  = '\a', BACKSPACE = '\b', TAB ='\t', NEWLINE = '\n', VTAB = '\v', RETURN = '\r' };

char is_escape(char c)
{
  return c == BELL || c == BACKSPACE || c == TAB || c == NEWLINE || c == VTAB || c == RETURN;
}

char is_whitespace(char c)
{
  return is_escape(c) || c == ' ';
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

char in_comma = 0;

void parse_token()
{
  if (token[0] == ',' && token[1] == '\0') {
    if (in_comma) {
      in_comma = 0;
    } else {
      in_comma = 1;
      return;
    }
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

  if (is_delimiter(c)) {
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

  token[token_i++] = c;
}

void main()
{
  int c;

  while ((c = getchar()) != EOF) {
    parse_char(c);
  }

  if (token_i > 0) {
    token[token_i] = '\0';
    parse_token();
  }
}
