#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "parse.h"
#include "print.h"
#include "sym.h"
#include "eval.h"
#include "continuation.h"
#include "env.h"

#define MAX_TOKEN 255

enum escape { BELL  = '\a', TAB = '\t', NEWLINE = '\n', RETURN = '\r' };

int is_escape(char c)
{
  return c == BELL || c == TAB || c == NEWLINE || c == RETURN;
}

int is_whitespace(char c)
{
  return c < 33 || c > 126;
}

int is_valid(char c)
{
  return is_escape(c) || c > 31 || c < 127;
}

int is_quote(char c)
{
  return c == '\'' || c == '`' || c == ',';
}

int is_paren(char c)
{
  return c == '(' || c == ')' || c == '[' || c == ']';
}

int is_delimiter(char c)
{
  return is_whitespace(c) || is_paren(c) || is_quote(c);
}

int expect_close = 0;

cell next_k(cell n)
{
  return cdr(cdr(car(cdr(cdr(n)))));
}

cell next_val(cell n)
{
  return car(cdr(cdr(cdr(n))));
}

void got_exp(cell exp)
{
  cell n;

  /*exp = gc(exp);*/

  pr(exp);
  printf("\n");

  n = eval(exp, get_globe(), get_dyn(), make_base_cont());
  pr(n);
  printf("\n");

  while (n) {
    n = resume(next_k(n), next_val(n));
  }
}

char token[MAX_TOKEN];
int token_i = 0;

int needs_expand(void) {
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

int token_expandable(void) {
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

int in_char = 0;
int in_comma = 0;
int improper = 0;
cell list_stack = 0;
cell quote_next = 0;
cell quote_stack = 0;

const cell char_mask = CHAR << CELL_SHIFT;

void push_ls(cell x)
{
  cell list = car(list_stack);
  list = join(x, list);

  list_stack = join(list, cdr(list_stack));
}

cell unqn(cell qn, cell atom)
{
  while (qn) {
    if (car(qn)) {
      atom = join(car(qn), join(atom, 0));
    }

    qn = cdr(qn);
  }

  return atom;
}

cell get_char(const char *str, const int start, const int len)
{
  int fr = start + 1;
  int se = start + 2;
  int th = start + 3;

  switch (len) {
  case 2:
    return str[fr] | char_mask;

  case 3:
    if (str[fr] == 's' && str[se] == 'p') {
      return ' ' | char_mask;
    }

    if (str[fr] == 'l' && str[se] == 'f') {
      return '\r' | char_mask;
    }

    if (str[fr] == 'c' && str[se] == 'r') {
      return '\n' | char_mask;
    }

    break;

  case 4:
    if (str[fr] == 'b' && str[se] == 'e' && str[th] == 'l') {
      return '\a' | char_mask;
    }

    if (str[fr] == 't' && str[se] == 'a' && str[th] == 'b') {
      return '\t' | char_mask;
    }

    break;
  }

  printf("ERR bad char escape -- GET_CHAR\n");
  exit(1);
}

void parse_token(const char *str, const int start, const int end)
{
  int i, j;
  int len = end - start;
  cell atom, c, list, q, xs;
  char tok[MAX_TOKEN];

  if (len < 1) {
    printf("ERR zero length token -- PARSE_TOKEN\n");
    exit(1);
  }

  if (str[start] == '\\' && str[start + 1] == '\0') {
    printf("bad char -- PARSE_TOKEN\n");
    exit(1);
  }

  if (is_quote(str[start]) && (len == 1 || (len == 2 && str[start + 1] == '@'))) {
    switch(str[start]) {
    case '\'':
      quote_next = join(quote, quote_next);
      return;

    case '`':
      quote_next = join(bquote, quote_next);
      return;

    case ',':
      quote_next = join(len == 1 ? comma : comma_at, quote_next);
      return;

    default:
      printf("bad quote -- PARSE_TOKEN\n");
      exit(1);
    }
  }

  if (str[start] == '(' && str[start + 1] == '\0') {
    if (expect_close) {
      printf("bad ( -- PARSE_TOKEN\n");
      exit(1);
    }

    list_stack = join(0, list_stack);

    quote_stack = join(quote_next, quote_stack);
    quote_next = 0;

    return;
  }

  if (str[start] == '.' && str[start + 1] == '\0') {
    if (!list_stack || improper || expect_close) {
      printf("bad . -- PARSE_TOKEN\n");
      exit(1);
    }

    improper = 1;
    return;
  }

  if (str[start] == ')' && str[start + 1] == '\0') {
    if (!list_stack) {
      printf("bad ) -- PARSE_TOKEN\n");
      exit(1);
    }

    if (improper && !expect_close) {
      printf("bad . -- PARSE_TOKEN\n");
      exit(1);
    }

    q = car(quote_stack);
    quote_stack = cdr(quote_stack);

    /* get list off list stack */
    list = car(list_stack);
    list_stack = cdr(list_stack);

    /* reverse list */
    xs = 0;

    while (list) {
      xs = join(car(list), xs);
      list = cdr(list);
    }

    if (expect_close) {
      /* make the list improper */
      while (xs) {
        if (!cdr(cdr(xs))) {
          xdr(xs, car(cdr(xs)));

          improper = 0;
          expect_close = 0;

          break;
        }

        xs = cdr(xs);
      }
    }

    if (q) {
      xs = unqn(q, xs);
    }

    if (list_stack) {
      /* nested list */
      push_ls(xs);
    } else {
      /* finished top level list */
      got_exp(xs);
    }

    return;
  }

  /* atom - but might get quoted */
  atom = 0;

  /* TODO: might be a number */

  switch (str[start]) {
  case '"':
    /* build list of chars onto atom (reverse, check for escapes) */
    for (i = end - 2; i > start; i--) {
      if (i > start && str[i - 1] == '\\') {
        switch (str[i]) {
          case 'a':
            c = BELL;
            break;

          case 't':
            c = TAB;
            break;

          case 'n':
            c = NEWLINE;
            break;

          case 'r':
            c = RETURN;
            break;

          case '"':
            c = '"';
            break;

        default:
          printf("unrecognised string escape \\%c -- PARSE_TOKEN\n", str[i]);
          exit(1);
          break;
        }

        i--;
      } else {
        c = str[i];
      }

      atom = join(c | char_mask, atom);
    }

    break;

  case '\\':
    /* set atom to char (check escape legit) */
    atom = get_char(str, start, len);
    break;

  default:
    /* set atom to symbol */
    for (i = start, j = 0; i < end; i++, j++) {
      tok[j] = str[i];
    }

    tok[j] = '\0';
    atom = get_sym(tok);

    break;
  }

  if (quote_next) {
    atom = unqn(quote_next, atom);
    quote_next = 0;
  }

  if (improper) {
    expect_close = 1;
  }

  if (list_stack) {
    push_ls(atom);
  } else {
    got_exp(atom);
  }
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
  int dot_or_bang = 0;
  int has_colon = 0;

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

int in_fn_shorthand = 0;
int in_str = 0;

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

  /*
  if (in_char) {
    printf("ERR unterminated char -- PARSE_TOKEN_FINAL\n");
    exit(1);
  }
  */

  if (token_i > 0) {
    token[token_i] = '\0';
    read_token();
  }

  printf("\n");
}

int in_comment = 0;
int str_escape_quote = 0;

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
