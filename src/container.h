#pragma once
#include "particle.h"
#include "misc/random.h"

#define DOUBLE_MAX 1.79e308
#define EPSILON 1e-15

struct contact {
  struct particle* source;
  struct particle* target;

  double distance;
};

struct bounds {
  struct vector L;
  struct vector L_half;
};

struct container {
  struct bounds bounds;
  struct random random;
  double volume;

  struct particle** particles;
  uint32_t particle_count;

  double largest_bounding_radius;
};

void container_init(struct container* container);
void container_destroy(struct container* container);

void container_add_particle(struct container* container, struct particle* particle);
void container_move_particle(struct container* container, struct particle* particle, struct vector position);
void container_set_bounds(struct container* container, struct bounds bounds);
struct contact container_first_contact_in_direction(struct container* container, struct particle* particle, struct vector direction, double cutoff);
void container_periodic_distance(struct container* container, struct vector* vector);
struct contact container_overlap_for_particle(struct container* container, struct particle* particle);
void container_set_random_position_for_particle(struct container* container, struct particle* particle);
void container_set_random_valid_position_for_particle(struct container* container, struct particle* particle);
void container_set_fcc_position_for_particle(struct container* container, struct particle* particle, double spacing[DIM], uint32_t id);
bool container_configuration_valid(struct container* container);
void container_export(struct container* container, char* file);
