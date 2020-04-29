#ifndef CONTINUATION_H
#define CONTINUATION_H

cell resume(cell k, cell val);
cell evaluate_if(cell ec, cell et, cell efx, cell r, cell d, int inwhere, cell k);
cell evaluate_where(cell e, cell r, cell d, int inwhere, cell k);
cell evaluate_application(cell op, cell args, cell r, cell d, int inwhere, cell k);
cell make_next(cell k, cell val);
cell make_base_cont(void);

#endif
