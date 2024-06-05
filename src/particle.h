#pragma once
#include "misc/vector.h"

struct particle {
  struct vector position;

  double radius;
};

struct particle* particle_create();
void particle_destroy(struct particle* particle);
bool particle_intersects(struct particle* source, struct particle* target, struct vector distance);
double particle_get_bounding_radius(struct particle* particle);

// NOTE: This gives a slight speed up compared to the branching implementation
static inline double particle_contact_distance(struct particle* source, struct particle* target, struct vector connection_vector, struct vector direction, double squared_range) {
  double distance_squared = vector_norm_squared(&connection_vector);
  if (distance_squared > squared_range) return -1.;

  double projection = vector_product(&direction, &connection_vector);
  double size = source->radius + target->radius;
  double xi = size * size - distance_squared + projection * projection;
  return (xi < 0 || projection < 0) ? -1. : (projection - sqrt(xi));
}
