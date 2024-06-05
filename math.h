#ifndef MATH_H
#define MATH_H

#define DS_IMPL
#include "ds.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

typedef struct {
  float *buf;
  int w, h;
} Matrix;

typedef enum {
  PADDING_SAME,
  PADDING_VALID,
} AutoPad;

#define m_at(m, i, j) (m).buf[(i) + (j) * (m).w]

void matrix_init(Arena *al, Matrix *m);
void matrix_print(Matrix m);
void matrix_randomize(Matrix m, float min, float max);
void matrix_copy(Matrix src, Matrix dst);
void matrix_fill(Matrix m, float v);
Matrix kernel_apply(Arena *al, Matrix in, Matrix k, AutoPad auto_pad, int hs, int vs);
void matrix_multiply_na(Matrix out, Matrix a, Matrix b);
Matrix matrix_multiply(Arena *al, Matrix a, Matrix b);

float randf(float min, float max);
int randi(int min, int max);
int sign(int x);
void clampf(float *x, float min, float max);
void clampi(int *x, int min, int max);
int in_bounds(int x, int max);

float maxf(float x, float y);
float minf(float x, float y);

#ifdef MATH_IMPL
#define MATH_IMPL

void matrix_init(Arena *al, Matrix *m) {
  m->buf = arena_alloc(al, m->w * m->h * sizeof(float));
  memset(m->buf, 0.0, m->w * m->h * sizeof(float));
}

void matrix_print(Matrix m) {
  for (int j = 0; j < m.h; j++) {
    for (int i = 0; i < m.w; i++) {
      printf("%.3f ", m_at(m, i, j));
    }
    printf("\n");
  }
}

void matrix_randomize(Matrix m, float min, float max) {
  assert(min <= max);
  for (int i = 0; i < m.w; i++) {
    for (int j = 0; j < m.h; j++) {
      m_at(m, i, j) = randf(min, max);
    }
  }
}

void matrix_fill(Matrix m, float v) {
  for (int i = 0; i < m.w; i++) {
    for (int j = 0; j < m.h; j++) {
      m_at(m, i, j) = v;
    }
  }
}

void matrix_copy(Matrix src, Matrix dst) {
  assert(src.w == dst.w);
  assert(src.h == dst.h);

  for (int i = 0; i < src.w; i++) {
    for (int j = 0; j < src.h; j++) {
      m_at(dst, i, j) = m_at(src, i, j);
    }
  }
}

Matrix kernel_apply(Arena *al, Matrix in, Matrix k, AutoPad auto_pad, int hs, int vs) {
  Matrix out;
  int x0, y0;

  switch (auto_pad) {
    case PADDING_SAME: {
      assert(k.w % 2);
      assert(k.h % 2);
      out.w = ceil((float) in.w / hs);
      out.h = ceil((float) in.h / vs);
      x0 = -k.w/2; 
      y0 = -k.h/2;
    } break;
    case PADDING_VALID: {
      out.w = ceil((in.w - 2 * (int) (k.w/2)) / hs);
      out.h = ceil((in.h - 2 * (int) (k.h/2)) / vs);
      x0 = y0 = 0;
    } break;
    default: assert(0);
  }

  matrix_init(al, &out);

  int x, y, i, j;
  for (y = y0, j = 0; j < out.h; y += vs, j++) {
    for (x = x0, i = 0; i < out.w; x += hs, i++) {
      for (int dy = 0; dy < k.h; dy++) {
        for (int dx = 0; dx < k.w; dx++) {
          if (!in_bounds(x + dx, in.w)) continue;
          if (!in_bounds(y + dy, in.h)) continue;
          m_at(out, i, j) += m_at(k, dx, dy) * m_at(in, x + dx, y + dy);
        }
      }
    }
  }

  return out;
}

void matrix_multiply_na(Matrix out, Matrix a, Matrix b) {
  assert(a.w == b.h);
  assert(out.w == b.w);
  assert(out.h == a.h);

  for (int i = 0; i < out.w; i++) {
    for (int j = 0; j < out.h; j++) {
      for (int k = 0; k < a.h; k++) {
        m_at(out, i, j) += m_at(b, i, k) * m_at(a, k, j);
      }
    }
  }
}

Matrix matrix_multiply(Arena *al, Matrix a, Matrix b) {
  assert(a.w == b.h);

  Matrix out = { .w = b.w, .h = a.h };
  matrix_init(al, &out);

  matrix_multiply_na(out, a, b);

  return out;
}

float randf(float min, float max) {
  float scale = rand() / (float) RAND_MAX;
  return min + scale * ( max - min );
}

int randi(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

int sign(int x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

void clampf(float *x, float min, float max) {
  if (*x < min) *x = min;
  else if (*x > max) *x = max;
}

void clampi(int *x, int min, int max) {
  if (*x < min) *x = min;
  else if (*x > max) *x = max;
}

int in_bounds(int x, int max) {
  if (x < 0) return 0;
  if (x >= max) return 0;
  return 1;
}

float maxf(float x, float y) {
  return x > y ? x : y;
}

float minf(float x, float y) {
  return x <= y ? x : y;
}
#endif

#endif
