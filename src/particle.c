#include "particle.h"

struct particle* particle_create() {
  struct particle* particle = malloc(sizeof(struct particle));
  memset(particle, 0, sizeof(struct particle));
  return particle;
}

void particle_destroy(struct particle* particle) {
  if (particle) free(particle);
}

bool particle_intersects(struct particle* source, struct particle* target, struct vector distance) {
  // Check if point is in sphere with R_1 + R_2
  if (vector_norm(&distance) < source->radius + target->radius)
    return true;
  return false;
}

double particle_get_bounding_radius(struct particle* particle) {
  return particle->radius;
}
