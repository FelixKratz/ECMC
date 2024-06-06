#include "event_chain.h"
#include "container.h"
#include "misc/vector.h"

// Initializes the event chain and seeds the random number generator
void event_chain_init(struct event_chain* event_chain) {
  event_chain->container = NULL;
  event_chain->active_particle = NULL;
  event_chain->length = 1.;
  event_chain->total_displacement = 0.;
  event_chain->total_lift_count = 0.;
  random_init(&event_chain->random);
  random_seed(&event_chain->random,
              time(NULL) ^ (intptr_t)&printf,
              time(NULL) ^ (intptr_t)&event_chain);
}

// Sets the simulation container
void event_chain_set_container(struct event_chain* event_chain, struct container* container) {
  event_chain->container = container;
}

// Performs the move and lifting of the event chain
static struct particle* event_chain_lift(struct event_chain* event_chain) {
  // Get the first contact in a given direction
  struct contact contact
         = container_first_contact_in_direction(event_chain->container,
                                                event_chain->active_particle,
                                                event_chain->direction,
                                                event_chain->length          );

  if (contact.distance > event_chain->length) {
    // Collision not in range
    contact.target = NULL;
  }

  if (contact.target && contact.distance == 0.) {
    // The contact is immediate (< EPSILON), no movement is needed.
    return contact.target;
  }

  // Set the (normalized) direction vector for the offset
  struct vector offset = event_chain->direction;

  if (contact.target) {
    // We have a contact and move the active particle to the contact,
    // then lift to the resting particle and continue the event chain

    // The offset vector still has length 1 and we must scale it with the
    // actual contact distance
    vector_scale(&offset, contact.distance);

    // The remaining event chain distance is reduced by the contact distance
    event_chain->length -= contact.distance;
  }
  else {
    // We have no contact in range and move the particle to the end of
    // the event chain, then the event chain will be completed
    vector_scale(&offset, event_chain->length);

    // Event Chain is done.
    event_chain->length = 0.;
  }

  // The container performs the book keeping for the particle positions, so
  // we tell it to finally move the active particle
  container_move_particle(event_chain->container,
                          event_chain->active_particle,
                          offset                       );

  return contact.target;
}

// The current event chain move is performed
static double event_chain_move(struct event_chain* event_chain, int* lift_count) {
  if (lift_count) *lift_count = 0;
  double displacement = 0;
  while (event_chain->active_particle) {
    struct vector initial_position = event_chain->active_particle->position;
    struct particle* prev_particle = event_chain->active_particle;

    event_chain->active_particle = event_chain_lift(event_chain);

    struct vector distance = event_chain->active_particle
                             ? event_chain->active_particle->position
                             : prev_particle->position;
    vector_subtract(&distance, &initial_position);
    container_periodic_distance(event_chain->container, &distance);

    // We track the event chain displacement and lift count for statistics and
    // measurements, such as the pressure.
    displacement += vector_product(&distance, &event_chain->direction);
    if (lift_count && event_chain->active_particle) (*lift_count)++;
  }

  return displacement;
}

// A new event chain move is set up by randomizing particle/direction
static void event_chain_randomize(struct event_chain* event_chain) {
  // Pick random particle and random event chain direction:
  uint32_t sel = random_uint32_bounded(&event_chain->random,
                                       event_chain->container->particle_count);

  event_chain->active_particle = event_chain->container->particles[sel];
  event_chain->length = 1.;
  event_chain->direction = random_unit_vector(&event_chain->random);
}

// An event chain step is performed
void event_chain_step(struct event_chain* event_chain) {
  event_chain_randomize(event_chain);
  int lift_count = 0;
  double length = event_chain->length;
  double displacement = event_chain_move(event_chain, &lift_count);

  event_chain->total_displacement += displacement / length;
  event_chain->total_lift_count += lift_count;

  #ifdef DEBUG
  // Check if configuration is valid after every event chain step
  if (!container_configuration_valid(event_chain->container)) {
    printf("\n[x] Invalid Move! Set Breakpoint to investigate...\n");
    exit(1);
  }
  #endif
}

// Helper function to resolve particle overlaps via the event chain logic
void event_chain_resolve_overlaps(struct event_chain* event_chain, struct particle* particle) {
  for (;;) {
    struct contact contact
                     = container_overlap_for_particle(event_chain->container,
                                                      particle               );
    if (!contact.target) return;

    event_chain->direction = random_unit_vector(&event_chain->random);
    event_chain->active_particle = contact.target;
    event_chain->length = 1.;
    event_chain_move(event_chain, NULL);
  }
}
