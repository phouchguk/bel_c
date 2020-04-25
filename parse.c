#include <stdio.h>
#include <stdlib.h>

#include "parse.h"

#define MAX_TOKEN 255

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
  char tok[MAX_TOKEN];

  if (end - start < 1) {
    printf("ERR zero length token -- PARSE_TOKEN\n");
    exit(1);
  }

  for (i = start, j = 0; i < end; i++, j++) {
    tok[j] = str[i];
  }

  tok[j] = '\0';

  if (tok[0] == '\\' && tok[1] == '\0') {
    printf("bad char -- PARSE_TOKEN\n");
    exit(1);
  }

  printf("%s ", tok);
}

char op[] = "(";
char cp[] = ")";
char tt[] = "t";
char tq[] = "'";
char tupon[] = "upon";
char tcompose[] = "compose";
char tno[] = "no";
char tfn[] = "fn";
char t_[] = "_";

void expand_token(char *str, int start, int end)
{
  int i, last;
  char dot_or_bang = 0;
  char has_colon = 0;

  /* check for | */
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

  /* check for . or ! */
  for (i = start; i < end; i++) {
    if (str[i] == '.' || str[i] == '!') {
      dot_or_bang = 1;
      break;
    }
  }

  if (dot_or_bang) {
    parse_token(op, 0, 1);

    last = start;

    for (i = start; i < end; i++) {
      if (str[i] == '.' || str[i] == '!') {
        if (i == 0) {
          parse_token(tupon, 0, 4);
        }

        /* if last == i bang/dot together -- bad? */

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

  /* check for : */
  for (i = start; i < end; i++) {
    if (str[i] == ':') {
      has_colon = 1;
      break;
    }
  }

  if (has_colon) {
    parse_token(op, 0, 1);
    parse_token(tcompose, 0, 7);

    last = start;

    for (i = start; i < end; i++) {
      if (str[i] == ':') {
        if (i == 0) {
          /*  error */
        }

        /* if last == i bang/dot together -- bad? */

        expand_token(str, last, i);

        last = i + 1;
      }
    }

    expand_token(str, last, end);

    parse_token(cp, 0, 1);

    return;
  }

  if (str[start] == '~') {
    /* if only 1 long, error */
    parse_token(op, 0, 1);
    parse_token(tcompose, 0, 7);
    parse_token(tno, 0, 3);
    parse_token(str, start + 1, end);
    parse_token(cp, 0, 1);

    return;
  }

  parse_token(str, start, end);
}

char in_fn_shorthand = 0;
char in_str = 0;

void read_token(void)
{
  if (token[1] == '\0') {
    if (token[0] == ',') {
      if (in_comma) {
        in_comma = 0;
      } else {
        in_comma = 1;
        return;
      }
    }

    if (token[0] == '[') {
      if (in_fn_shorthand) {
        printf("can't nest fn shorthand -- READ_TOKEN\n");
        exit(1);
      }

      /* [id x _] -> (fn (_) (id x _))
       * this is the first part:
       * [ -> (fn (_) (
       */
      parse_token(op, 0, 1);
      parse_token(tfn, 0, 2);

      parse_token(op, 0, 1);
      parse_token(t_, 0, 1);
      parse_token(cp, 0, 1);

      parse_token(op, 0, 1);

      in_fn_shorthand = 1;
      token_i = 0;

      return;
    }

    if (token[0] == ']') {
      if (!in_fn_shorthand) {
        printf("bad ']' -- READ_TOKEN\n");
        exit(1);
      }

      /* this is the last part:
       * ] -> ))
       */
      parse_token(cp, 0, 1);
      parse_token(cp, 0, 1);

      in_fn_shorthand = 0;
      token_i = 0;

      return;
    }
  }

  if (token_expandable() && needs_expand()) {
    expand_token(token, 0, token_i);
  } else {
    parse_token(token, 0, token_i);
  }

  token_i = 0;
}

void parse_token_final(void) {
  if (in_fn_shorthand) {
    printf("ERR unclosed fn shorthand -- PARSE_TOKEN_FINAL\n");
    exit(1);
  }

  if (in_str) {
    printf("ERR unterminated string -- PARSE_TOKEN_FINAL\n");
    exit(1);
  }

  if (in_char) {
    printf("ERR unterminated char -- PARSE_TOKEN_FINAL\n");
    exit(1);
  }

  if (token_i > 0) {
    token[token_i] = '\0';
    read_token();
  }

  printf("\n");
}

char in_comment = 0;
char str_escape_quote = 0;

void parse_char(char c)
{
  if (in_comment) {
    if (c == NEWLINE) {
      in_comment = 0;
    }

    return;
  }

  if (!in_char && !in_str && c == ';') {
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

  if (!in_str && is_delimiter(c) && !(in_char && token_i == 1 && (is_paren(c) || is_quote(c)))) {
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
    if (in_str) {
      str_escape_quote = 1;
    } else {
      in_char = 1;
    }
  }

  if (!in_char && c == '"') {
    if (str_escape_quote) {
      str_escape_quote = 0;
    } else {
      in_str = in_str ? 0 : 1;
    }
  }

  token[token_i++] = c;
}
