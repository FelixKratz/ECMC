#pragma once
#include <time.h>
#include "vector.h"

struct random {
    uint64_t state;
    uint64_t inc;
};

static inline void random_init(struct random* random) {
  *random = (struct random) { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL };
}

static inline uint32_t random_uint32(struct random* random) {
    uint64_t oldstate = random->state;
    random->state = oldstate * 6364136223846793005ULL + random->inc;
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static inline uint32_t random_uint32_bounded(struct random* random, uint32_t bound) {
    uint32_t threshold = -bound % bound;

    for (;;) {
        uint32_t r = random_uint32(random);
        if (r >= threshold)
            return r % bound;
    }
}

static inline double random_double_bounded(struct random* random, double bound) {
  return ((double)random_uint32(random)) / ((double)UINT32_MAX) * bound;
}

static inline struct vector random_unit_vector(struct random* random) {

  if (DIM == 1) return (struct vector) { random_double_bounded(random, 1.0) };

  struct vector spherical = { 0 };
  *vector_component(&spherical, 0) = 1.0;

  for (uint32_t i = 1; i < DIM - 1; i++) {
    *vector_component(&spherical, i) = random_double_bounded(random, M_PI);
  }

  *vector_component(&spherical, DIM - 1) = random_double_bounded(random, 2.*M_PI);
  return vector_cartesian_from_sperical(spherical);
}

static inline void random_seed(struct random* random, uint64_t seed, uint64_t seq) {
    random->state = 0U;
    random->inc = (seq << 1u) | 1u;
    random_uint32(random);
    random->state += seed;
    random_uint32(random);
}
