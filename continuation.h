#ifndef CONTINUATION_H
#define CONTINUATION_H

cell resume(cell k, cell val);
int evaluate_begin(cell ex, cell r, cell d, cell k);
int evaluate_if(cell ec, cell et, cell efx, cell r, cell d, cell k);
int make_next(cell k, cell val);

#endif
