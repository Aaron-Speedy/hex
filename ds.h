#ifndef DS_H
#define DS_H

#ifndef CUSTOM_ASSERT
#include <assert.h>
#define CUSTOM_ASSERT(c) assert(c)
#endif

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>

typedef ptrdiff_t size;

#define arrlen(...) (size)(sizeof(__VA_ARGS__) / sizeof(*__VA_ARGS__))

#define pool_header(xs) (((PoolHeader *)xs)[-1])
typedef struct {
  size pos; // not including header
  void *free, *iter;
} PoolHeader;

typedef struct {
  char *buf;
  size cap, pos;
} Arena;

typedef struct {
  char *buf;
  size cap;
  size chunk_len;
  PoolHeader *free, *iter;
} Pool;

Arena arena_init(size cap);
void *arena_alloc(Arena *al, size len);

Pool pool_init(size num_chunks, size chunk_len);
void *pool_alloc(Pool *al);
void pool_free(Pool *al, void *men);

#define da_header(xs) (((ListHeader *)xs)[-1])
typedef struct {
  size len, cap;
} ListHeader;

#define List(T) T *

#define da_cap(xs) (da_header(xs).cap)
#define da_len(xs) (da_header(xs).len)

#define da_init(xs, cap) \
do { \
  CUSTOM_ASSERT(cap > 0); \
  (xs) = malloc((cap) * sizeof((xs)[0]) + sizeof(ListHeader)); \
  CUSTOM_ASSERT((xs) && "Malloc failed!"); \
  (xs) += (size) ceil(sizeof(ListHeader) / sizeof((xs)[0])); \
  da_cap(xs) = (cap); \
  da_len(xs) = 0; \
} while (0);

#define da_push(xs, x) \
do { \
  if (da_len(xs) >= da_cap(xs)) { \
    size ncap = da_cap(xs) * 2; \
    size c = (size) ceil(sizeof(ListHeader) / sizeof((xs)[0])); \
    (xs) -= c; \
    (xs) = realloc((xs), ncap * sizeof((xs)[0]) + sizeof(ListHeader)); \
    CUSTOM_ASSERT((xs) && "Realloc failed!"); \
    (xs) += c; \
    da_cap(xs) = ncap; \
  } \
  (xs)[da_len(xs)++] = x; \
} while (0);

#define da_pop(xs) \
do { \
  CUSTOM_ASSERT(da_len(xs) > 0); \
  da_len(xs) -= 1; \
} while (0);

#define da_last(xs) (xs[da_len(xs) - 1])

#ifdef DS_IMPL
#define DS_IMPL

Arena arena_init(size cap) {
  CUSTOM_ASSERT(cap > 0);

  Arena al = { .cap = cap, };
  al.buf = malloc(al.cap);

  return al;
}

void *arena_alloc(Arena *al, size len) {
  CUSTOM_ASSERT(al->pos + len <= al->cap && "Not enough memory in arena");
  CUSTOM_ASSERT(len > 0);

  void *mem = &al->buf[al->pos];
  al->pos += len;

  return mem;
}

Pool pool_init(size num_chunks, size chunk_len) {
  CUSTOM_ASSERT(num_chunks > 0);
  CUSTOM_ASSERT(chunk_len > 0);

  Pool al = {
    .cap = num_chunks * (chunk_len + sizeof(PoolHeader)),
    .chunk_len = chunk_len,
  };
  al.buf = malloc(al.cap);
  al.free = (PoolHeader *)(&al.buf[0]);
  al.free->pos = sizeof(PoolHeader);

  return al;
}

void *pool_alloc(Pool *al) {
  CUSTOM_ASSERT(
    al->free->pos + al->chunk_len + sizeof(PoolHeader) <= al->cap &&
    "Not enough memory in pool"
  );

  PoolHeader *header = al->free;

  if (al->free->free == NULL) {
    size pos = header->pos + al->chunk_len;
    al->free = (PoolHeader *)(&al->buf[pos]);
    al->free->pos = pos;
  } else {
    al->free = (PoolHeader *)al->free->free;
  }

  return &al->buf[header->pos];
}

void pool_free(Pool *al, void *mem) {
  CUSTOM_ASSERT(mem != NULL);

  pool_header(mem).free = al->free;
  al->free = &pool_header(mem);
}

#endif

#endif
