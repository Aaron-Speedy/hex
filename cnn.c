#include <stdio.h>

#define MATH_IMPL
#include "math.h"

typedef struct {
  int x, y;
} A;

/*
o: output, i: input, s: stride, k: kernel_size, p: padding
same: o = ceil(i/s)
generic: o = floor((i + 2p - k)/s) + 1
*/

int main() {
  Arena al = arena_init(sizeof(float) * 100000);

  Matrix in = { .w = 11, .h = 11, };
  matrix_init(&al, &in);
  matrix_randomize(in, 0.0, 1.0);
  matrix_print(in);

  Matrix kernel = { .w = 3, .h = 3, };
  matrix_init(&al, &kernel);
  matrix_randomize(kernel, 0.0, 1.0);
  matrix_print(kernel);

  Matrix out = kernel_apply(&al, in, kernel, PADDING_VALID, 2, 2);
  matrix_print(out);

  return 0;
}
