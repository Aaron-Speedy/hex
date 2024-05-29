#include <stdio.h>
#include <stdbool.h>

#define DS_IMPL
#include "ds.h"

#define var(v) (Var){ v, 0, true, }
typedef struct {
  float v, d; // value, derivative
  bool is_var;
} Var;

typedef struct {
  enum {
    MULTIPLY,
  } op;
  Var *x, *y;
} Node;

typedef struct {
  Var x;
} Func;

Func multiply(Arena *al, Var x, Var y) {
  arena_alloc(al, sizeof(Node));
}

int main() {
  Arena arena = arena_init(sizeof(Node) * 100000);

  Func f = { .x = var(0.25), };
  // Func g = multiply(&f.x, &f.x);
  // printf("%f\n", g.x);

  return 0;
}
