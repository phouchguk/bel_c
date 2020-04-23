#include<stdio.h>

#define MAX_TOKEN 256

char token[MAX_TOKEN];
int token_i = 0;

enum escapes { BELL  = '\a', TAB ='\t', NEWLINE = '\n', RETURN = '\r' };

char is_escape(char c)
{
  return c == BELL || c == TAB || c == NEWLINE || c == RETURN;
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
  return c == '\\' || is_whitespace(c) || is_paren(c) || is_quote(c);
}

char in_char = 0;
char in_comma = 0;

void parse_token()
{
  if (token[1] == '\0') {
    switch (token[0]) {
    case '\\':
      if (in_char) {
        in_char = 0;
      } else {
        in_char = 1;
        return;
      }

    case ',':
      if (in_comma) {
        in_comma = 0;
      } else {
        in_comma = 1;
        return;
      }
    }
  }

  printf(token);
  putchar(' ');
}

char in_comment = 0;
char expect[3];
int expect_i = 0;

void parse_char(char c)
{
  if (in_comment) {
    if (c == NEWLINE) {
      in_comment = 0;
    }

    return;
  }

  if (in_char) {
    /* need to check for \bel \cr \lf \sp \tab */
    switch (c) {
    case 'b':
      expect[0] = 'e';
      expect[1] = 'l';
      expect[2] = '\0';
      return;

    case 'c':
      expect[0] = 'r';
      expect[1] = '\0';
      return;

    case 'l':
      expect[0] = 'f';
      expect[1] = '\0';
      return;

    case 's':
      expect[0] = 'p';
      expect[1] = '\0';
      return;

    case 't':
      expect[0] = 'a';
      expect[1] = 'b';
      expect[2] = '\0';
      return;
    }

    in_char = 0;

    token[1] = c;
    token[2] = '\0';

    parse_token();

    return;
  } else {
    if (c == ';') {
      in_comment = 1;
      return;
    }
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

    if (c == '\\' || is_paren(c) || is_quote(c)) {
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
