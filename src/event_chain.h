#pragma once
#include "particle.h"
#include "container.h"
#include "misc/random.h"

struct event_chain {
  struct container* container;
  struct particle* active_particle;
  struct vector direction;
  struct random random;
  double length;
  double total_displacement;
};

void event_chain_init(struct event_chain* event_chain);
void event_chain_set_container(struct event_chain* event_chain, struct container* container);
void event_chain_step(struct event_chain* event_chain);
void event_chain_resolve_overlaps(struct event_chain* event_chain, struct particle* particle);
