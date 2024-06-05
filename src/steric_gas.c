#include "steric_gas.h"
#include "container.h"

void steric_gas_init(struct steric_gas* steric_gas, double size) {
  event_chain_init(&steric_gas->event_chain);
  container_init(&steric_gas->container);

  struct bounds bounds;
  for (uint32_t i = 0; i < DIM; i++) {
    *vector_component(&bounds.L, i) = size;
  }

  container_set_bounds(&steric_gas->container, bounds);
  event_chain_set_container(&steric_gas->event_chain, &steric_gas->container);
}

void steric_gas_destroy(struct steric_gas* steric_gas) {
  container_destroy(&steric_gas->container);
}

double steric_gas_fill_to_packing_fraction(struct steric_gas* steric_gas, double sigma, double eta, bool verbose) {
  if (verbose) printf("[?] Trying to achieve valid configuration");

  double packing_fraction = 0.;
  while (packing_fraction < eta) {
    struct particle* particle = particle_create();
    particle->radius = sigma / 2.;

    container_set_random_position_for_particle(&steric_gas->container,
                                               particle               );

    container_add_particle(&steric_gas->container, particle);

    packing_fraction = steric_gas->container.particle_count * 1./6. * M_PI
                       * sigma * sigma * sigma
                       / (steric_gas->container.volume);

  }

  for (uint32_t i = 0; i < steric_gas->container.particle_count; i++) {
    if (verbose) {
      printf("\r[?] Resolving overlaps via EC: %d / %d",
             i + 1,
             steric_gas->container.particle_count       );
    }

    event_chain_resolve_overlaps(&steric_gas->event_chain,
                                 steric_gas->container.particles[i]);
  }

  #ifdef DEBUG
  if (!container_configuration_valid(&steric_gas->container)) {
    printf("\n[x] Configuration still invalid\n");
    exit(1);
  }
  #endif

  if (verbose) {
    printf("\r[+] Valid Initial Configuration with eta = %f\n",
                                              packing_fraction);
  }
    
  return packing_fraction;
}

struct measurements steric_gas_simulate(struct steric_gas* steric_gas, uint32_t steps, bool verbose) {
  uint32_t progress = 0;
  uint32_t prev = UINT32_MAX;
  for (uint32_t i = 0; i < steps; i++) {
    if (verbose) {
      progress = ((double)(i+1))/(double)(steps)*100.;
      if (prev != progress) {
        printf("\r[?] Progress: %d%%", progress); 
        fflush(stdout);
        prev = progress;
      }
    }

    event_chain_step(&steric_gas->event_chain);
  }

  if (!container_configuration_valid(&steric_gas->container)) {
    printf("\n[x] Configuration invalid\n");
    exit(1);
  }

  struct measurements measurements = { 0 };
  measurements.pressure = steric_gas->event_chain.total_displacement / steps;

  if (verbose) printf("\n[+] System Pressure: %f\n", measurements.pressure); 
  return measurements;
}
