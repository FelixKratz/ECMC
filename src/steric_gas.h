#pragma once
#include "event_chain.h"

struct measurements {
  double pressure;
};

struct steric_gas {
  struct container container;
  struct event_chain event_chain;
};

void steric_gas_init(struct steric_gas* steric_gas, double size);
void steric_gas_destroy(struct steric_gas* steric_gas);

double steric_gas_fill_to_packing_fraction(struct steric_gas* steric_gas, double sigma, double eta, bool verbose);
struct measurements steric_gas_simulate(struct steric_gas* steric_gas, uint32_t steps, bool verbose);
