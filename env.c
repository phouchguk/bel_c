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

cell set_variable_value(cell var, cell val, cell env, int define)
{
  cell enc, frame, vals, vars;

  while (env) {
    frame = first_frame(env);
    vals = frame_vals(frame);
    vars = frame_vars(frame);

    while (vars) {
      if (car(vars) == var) {
        xar(vals, val);
        return val;
      }

      vals = cdr(vals);
      vars = cdr(vars);
    }

    enc = enclosing_env(env);

    if (!enc && define) {
      /* define a new variable at root */
      add_binding_to_frame(var, val, frame);
      return val;
    }

    env = enc;
  }

  return unbound;
}

cell set_variable(cell var, cell val, cell r, cell d, cell k)
{
  cell existing;

  existing = lookup_variable_value(var, d);

  if (existing == unbound) {
    return make_next(k, set_variable_value(var, val, r, 1));
  }

  return make_next(k, set_variable_value(var, val, d, 0));
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
  cell s_cdr = get_sym("cdr");
  cell prim_cdr = join(lit, join(prim, join(s_cdr, 0)));
  cell x = join(a, join(get_sym("b"), join(get_sym("c"), 0)));
  globe = extend_env(join(get_sym("x"), join(s_cdr, 0)), join(x, join(prim_cdr, 0)), the_empty_env);
  dyn = extend_env(join(get_sym("y"), 0), join(get_sym("ninety-nine"), 0), the_empty_env);
}
