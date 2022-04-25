#pragma once
#include "event_chain.h"

struct steric_gas {
  struct container container;
  struct event_chain event_chain;
};

void steric_gas_init(struct steric_gas* steric_gas);
void steric_gas_test_case(struct steric_gas* steric_gas);
double steric_gas_fill_to_packing_fraction(struct steric_gas* steric_gas, double sigma, double eta);
void steric_gas_simulate(struct steric_gas* steric_gas, uint32_t steps);
