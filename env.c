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
        set_loc(a, vals);

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

cell make_prim(cell s)
{
  return join(lit, join(prim, join(s, 0)));
}

void setup_environment(void)
{
  cell vars = 0;
  cell vals = 0;

  cell x = join(a, join(get_sym("b"), join(get_sym("c"), 0)));

  vars = join(s_car, vars);
  vals = join(make_prim(s_car), vals);

  vars = join(s_cdr, vars);
  vals = join(make_prim(s_cdr), vals);

  vars = join(s_id, vars);
  vals = join(make_prim(s_id), vals);

  vars = join(s_join, vars);
  vals = join(make_prim(s_join), vals);

  vars = join(s_type, vars);
  vals = join(make_prim(s_type), vals);

  vars = join(s_xar, vars);
  vals = join(make_prim(s_xar), vals);

  vars = join(s_xdr, vars);
  vals = join(make_prim(s_xdr), vals);

  vars = join(s_nom, vars);
  vals = join(make_prim(s_nom), vals);

  vars = join(s_sym, vars);
  vals = join(make_prim(s_sym), vals);

  vars = join(s_coin, vars);
  vals = join(make_prim(s_coin), vals);

  vars = join(get_sym("x"), vars);
  vals = join(x, vals);

  globe = extend_env(vars, vals, the_empty_env);
  dyn = extend_env(join(get_sym("y"), 0), join(get_sym("ninety-nine"), 0), the_empty_env);
}
