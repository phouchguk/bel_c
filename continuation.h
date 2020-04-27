#ifndef CONTINUATION_H
#define CONTINUATION_H

cell resume(cell k, cell val);
cell evaluate_begin(cell ex, cell r, cell d, cell k);
cell evaluate_if(cell ec, cell et, cell efx, cell r, cell d, cell k);
cell evaluate_set(cell var, cell val, cell r, cell d, cell k);
cell evaluate_where(cell e, cell r, cell d, cell k);
cell make_next(cell k, cell val);
cell make_base_cont(void);

#endif
