#include <stdio.h>
#include <stdlib.h>

#include "type.h"
#include "pair.h"
#include "sym.h"
#include "continuation.h"
#include "env.h"

cell the_empty_env = 0;
cell globe;
cell dyn;

cell enclosing_env(cell env)
{
  return cdr(env);
}

cell first_frame(cell env)
{
  return car(env);
}

cell make_frame(cell vars, cell vals)
{
  return join(vars, vals);
}

cell frame_vars(cell frame)
{
  return car(frame);
}

cell frame_vals(cell frame)
{
  return cdr(frame);
}

void add_binding_to_frame(cell var, cell val, cell frame)
{
  xar(frame, join(var, car(frame)));
  xdr(frame, join(val, cdr(frame)));
}

int length(cell x)
{
  int i = 0;

  while (x) {
    i++;
    x = cdr(x);
  }

  return i;
}

cell extend_env(cell vars, cell vals, cell base_env)
{
  int varl = length(vars);
  int vall = length(vals);

  if (varl != vall) {
    printf("Too %s arguments supplied -- EXTEND_ENV\n", varl < vall ? "many" : "few");
    exit(1);
  }

  return join(make_frame(vars, vals), base_env);
}

cell lookup_variable_value(cell var, cell env)
{
  cell frame, vals, vars;

  while (env) {
    frame = first_frame(env);
    vals = frame_vals(frame);
    vars = frame_vars(frame);

    while (vars) {
      if (car(vars) == var) {
        return car(vals);
      }

      vals = cdr(vals);
      vars = cdr(vars);
    }

    env = enclosing_env(env);
  }

  return unbound;
}

cell lookup_variable(cell n, cell r, cell d, cell k)
{
  cell val;

  /* lookup in dynamic environment first */
  val = lookup_variable_value(n, d);

  if (val == unbound) {
    val = lookup_variable_value(n, r);

    if (val == unbound) {
      printf("unbound variable '%s' -- LOOKUP_VARIABLE\n", nom(n));
      exit(1);
    }
  }

  return make_next(k, val);
}

cell get_globe(void)
{
  return globe;
}

cell get_dyn(void)
{
  return dyn;
}

void setup_environment(void)
{
  globe = extend_env(join(get_sym("x"), 0), join(get_sym("forty-two"), 0), the_empty_env);
  dyn = the_empty_env;
}
