#include "container.h"

void container_init(struct container* container) {
  container->particles = NULL;
  container->particle_count = 0;
  container->volume = 0;
  container->largest_bounding_radius = 0;

  random_init(&container->random);

  random_seed(&container->random,
              time(NULL) ^ (intptr_t)&printf,
              (intptr_t)&container           );
}

void container_add_particle(struct container* container, struct particle* particle) {
  container->particles = realloc(container->particles,
                                 sizeof(struct particle*)
                                  * ++container->particle_count);

  container->particles[container->particle_count - 1] = particle;

  double bounding_radius = particle_get_bounding_radius(particle);
  if (bounding_radius > container->largest_bounding_radius)
    container->largest_bounding_radius = bounding_radius;
}

static void container_periodic_distance(struct container* container, struct vector* vector) {
  double* restrict _vector = (double*) vector;
  double* restrict _L_half = (double*) &container->bounds.L_half;
  double* restrict _L = (double*) &container->bounds.L;

  for (uint32_t i = 0; i < DIM; i++) {
    _vector[i] += (_vector[i] < -_L_half[i]) * _L[i]
                - (_vector[i] >  _L_half[i]) * _L[i];

    // NOTE: Below is (3-4)x slower than the above (vectorization):
    // if (_vector[i] > _L_half[i])
    //   _vector[i] -= _L[i];
    // else if (_vector[i] < -_L_half[i])
    //   _vector[i] += _L[i];
  }
}

struct contact container_first_contact_in_direction(struct container* container, struct particle* source, struct vector direction, double cutoff) {
  struct contact contact = { 0 };
  struct vector connection_vector;
  struct particle* target;
  contact.source = source;
  contact.target = NULL;
  contact.distance = DOUBLE_MAX;

  double range = cutoff + 2. * container->largest_bounding_radius;
  double squared_range = range * range;

  // Naive implementation where every particle is checked for a contact
  // TODO: Implement Verlet list as a possible optimization
  for (uint32_t i = 0; i < container->particle_count; i++) {
    target = container->particles[i];

    if (target == source) continue;

    connection_vector = target->position;
    vector_subtract(&connection_vector, &source->position);
    container_periodic_distance(container, &connection_vector);

    double distance = particle_contact_distance(source,
                                                target,
                                                connection_vector,
                                                direction,
                                                squared_range     );

    if (distance >= 0. && distance < contact.distance) {
      distance = (distance > EPSILON) ? distance - EPSILON : 0.;
      contact.distance = distance;
      contact.target = target;
    }
  }

  return contact;
}

struct contact container_overlap_for_particle(struct container* container, struct particle* particle) {
  struct contact contact = { 0 };
  contact.source = particle;
  contact.target = NULL;
  contact.distance = DOUBLE_MAX;

  for (uint32_t i = 0; i < container->particle_count; i++) {
    if (particle == container->particles[i]) continue;

    struct vector distance = particle->position;
    vector_subtract(&distance, &container->particles[i]->position);
    container_periodic_distance(container, &distance);
    if (particle_intersects(container->particles[i], particle, distance)) {
      contact.target = container->particles[i];
      contact.distance = vector_norm(&distance);
      break;
    }
  }

  return contact;
}

void container_move_particle(struct container* container, struct particle* particle, struct vector offset) {
  vector_add(&particle->position, &offset);

  double* _vector = (double*) &particle->position;
  double* _L = (double*) &container->bounds.L;

  // Periodic boundaries
  for (uint32_t i = 0; i < DIM; i++)
    _vector[i] += (_vector[i] < 0) * _L[i] - (_vector[i] > _L[i]) * _L[i];
}

void container_set_bounds(struct container* container, struct bounds bounds) {
  container->bounds = bounds;
  container->volume = 1.;
  for (uint32_t i = 0; i < DIM; i++) {
    container->volume *= *vector_component(&bounds.L, i);
    *vector_component(&container->bounds.L_half, i) =
                                          *vector_component(&bounds.L, i) / 2.;
  }
}

bool container_configuration_valid(struct container* container) {
  for (uint32_t i = 0; i < container->particle_count; i++) {
    for (uint32_t j = i + 1; j < container->particle_count; j++) {
      struct vector distance = container->particles[i]->position;
      vector_subtract(&distance, &container->particles[j]->position);
      container_periodic_distance(container, &distance);

      if (particle_intersects(container->particles[i],
                              container->particles[j],
                              distance                )) {
        return false;
      }
    }
  }

  return true;
}

void container_set_random_position_for_particle(struct container* container, struct particle* particle) {
  for (uint32_t i = 0; i < DIM; i++) {
    *vector_component(&particle->position, i)
           = random_double_bounded(&container->random,
                                   *vector_component(&container->bounds.L, i));
  }
}

void container_set_random_valid_position_for_particle(struct container* container, struct particle* particle) {
  for (;;) {
    container_set_random_position_for_particle(container, particle);

    bool intersects = false;
    for (uint32_t i = 0; i < container->particle_count; i++) {
      struct vector distance = particle->position;
      vector_subtract(&distance, &container->particles[i]->position);
      container_periodic_distance(container, &distance);

      if (particle_intersects(particle, container->particles[i], distance)) {
        intersects = true;
        break;
      }
    }

    if (!intersects) return;
  }
}

void container_export(struct container* container, char* file) {
  if (!container_configuration_valid(container))
    printf("\n[!] Configuration for: '%s' invalid!\n", file);

  FILE* output = fopen(file, "w");
  char buf[32] = { '\0' };

  snprintf(buf, 32, "%d", container->particle_count);
  fputs(buf, output);
  fputs("\nKind X Y Z", output);

  for (uint32_t i = 0; i < container->particle_count; i++) {
    struct vector* position = &container->particles[i]->position;
    fputs("\nA", output);
    for (uint32_t j = 0; j < DIM; j++) {
      double x_j = *vector_component(position, j);
      snprintf(buf, 32, " %f", x_j);
      fputs(buf, output);
    }
  }
  
  fclose(output);
}

void container_destroy(struct container* container) {
  for (uint32_t i = 0; i < container->particle_count; i++)
    particle_destroy(container->particles[i]);

  if (container->particles)
    free(container->particles);
}
