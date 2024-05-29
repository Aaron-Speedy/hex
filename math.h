#ifndef MATH_H
#define MATH_H

#include <time.h>
#include <stdlib.h>

#define SQRT3 1.73205080757

float minf(float x, float y);
float maxf(float x, float y);
float randf(float min, float max);
int randi(int min, int max);
int sign(int x);
void clampf(float *x, float min, float max);
void clampi(int *x, int min, int max);
int in_bounds(int x, int max);

#ifdef MATH_IMPL
#define MATH_IMPL

float minf(float x, float y) {
  return x < y ? x : y;
}

float maxf(float x, float y) {
  return x > y ? x : y;
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

#endif

#endif
