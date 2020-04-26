#ifndef ENV_H
#define ENV_H

cell lookup_variable(cell n, cell r, cell d, cell k);
cell set_variable(cell var, cell val, cell r, cell d, cell k);
cell get_dyn(void);
cell get_globe(void);
void setup_environment(void);

#endif
