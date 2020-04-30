#ifndef ENV_H
#define ENV_H

cell extend_env(cell vars, cell vals, cell base_env);
cell destructure_extend_env(cell vars, cell vals, cell base_env);
cell lookup_variable(cell n, cell r, cell d, int inwhere, cell k);
cell get_dyn(void);
cell get_globe(void);
void setup_environment(void);
int length(cell x);

#endif
