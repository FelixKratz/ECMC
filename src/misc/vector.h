#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DIM 3
struct vector {
   double x, y, z;
};

#define vector_component(vector, index) ((double*)vector + index)

static inline void vector_add(struct vector* restrict v1, struct vector* restrict v2) {
  for (uint32_t i = 0; i < DIM; i++)
    *vector_component(v1, i) += *vector_component(v2, i);
}

static inline void vector_subtract(struct vector* restrict v1, struct vector* restrict v2) {
  for (uint32_t i = 0; i < DIM; i++)
    *vector_component(v1, i) -= *vector_component(v2, i);
}

static inline double vector_product(struct vector* v1, struct vector* v2) {
  double sum[DIM];
  for (uint32_t i = 0; i < DIM; i++)
    sum[i] = *vector_component(v1, i) * *vector_component(v2, i);

  double product = 0.;
  for (uint32_t i = 0; i < DIM; i++) product += sum[i];

  return product;
}

static inline void vector_divide(struct vector* restrict v1, double divisor) {
  for (uint32_t i = 0; i < DIM; i++)
    *vector_component(v1, i) /= divisor;
}

static inline void vector_scale(struct vector* restrict v1, double scale) {
  for (uint32_t i = 0; i < DIM; i++)
    *vector_component(v1, i) *= scale;
}

static inline double vector_norm_squared(struct vector* restrict v1) {
  return vector_product(v1, v1); 
}

static inline double vector_norm(struct vector* restrict v1) {
  return sqrt(vector_norm_squared(v1)); 
}

static inline struct vector vector_cartesian_from_sperical(struct vector spherical) {
  struct vector cartesian = { 0 };
  for (uint32_t i = 0; i < DIM; i++) {
    double projection = 1.0;
    for (uint32_t j = 1; j <= i; j++) {
      projection *= sin(*vector_component(&spherical, j));
    }

    if (i < DIM - 1)
      projection *= cos(*vector_component(&spherical, i + 1));

    *vector_component(&cartesian, i) = projection;
  }

  vector_scale(&cartesian, *vector_component(&spherical, 0));
  return cartesian;
}
