#ifndef TYPE_H
#define TYPE_H

#define CELL_SHIFT 29
#define TAG_BIT_SIZE 3

typedef unsigned int cell;
enum bel_t { SYM = 0, PAIR = 1, CHAR = 2, STREAM = 3, NUM = 4, HEART = 5 };

#endif
