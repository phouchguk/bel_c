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

void parse_token()
{
  printf(token);
  putchar(NEWLINE);
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

  if (token_i == MAX_TOKEN) {
    printf("token too long -- PARSE_CHAR\n");
    exit(1);
  }

  if (is_whitespace(c)) {
    if (token_i > 0) {
      token[token_i] = '\0';
      token_i = 0;

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
    token_i = 0;

    parse_token();
  }
}
