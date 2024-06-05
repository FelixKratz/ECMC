#include "steric_gas.h"
#include <time.h>

void benchmark() {
  printf("[?] Starting Benchmark...\n");
  struct steric_gas steric_gas;
  steric_gas_init(&steric_gas, 10.0);
  steric_gas_fill_to_packing_fraction(&steric_gas, 1.0, .4, true);
  container_export(&steric_gas.container, "init.dat");

  clock_t begin = clock();
  steric_gas_simulate(&steric_gas, 1000000, true);
  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("\r[+] Done. Took: %fs\n", time_spent);

  container_export(&steric_gas.container, "final.dat");
  steric_gas_destroy(&steric_gas);
  printf("\r[+] Finished.                         \n");
}

void measure_equation_of_state(char* output_path, double lower_eta_bound, double upper_eta_bound, uint32_t samples) {
  double eta_step = (upper_eta_bound - lower_eta_bound)
                    / (double)samples;
  double eta = lower_eta_bound;
  double pressures[samples];
  double etas[samples];

  uint32_t progress = 0;
  uint32_t prev = UINT32_MAX;
  for (uint32_t i = 0; i < samples; i++) {
    progress = ((double)(i+1))/(double)(samples)*100.;
    if (prev != progress) {
      printf("\r[?] Progress: %d%%", progress); 
      fflush(stdout);
      prev = progress;
    }

    eta += eta_step;
    struct steric_gas steric_gas;
    steric_gas_init(&steric_gas, 10.0);
    etas[i] = steric_gas_fill_to_packing_fraction(&steric_gas,
                                                  1.0,
                                                  eta,
                                                  false       );

    struct measurements measurements = steric_gas_simulate(&steric_gas,
                                                           10000000,
                                                           false       );
    pressures[i] = measurements.pressure;
    steric_gas_destroy(&steric_gas);
  }

  FILE* output = fopen(output_path, "w");
  char buf[128] = { '\0' };
  for (uint32_t i = 0; i < samples; i++) {
    snprintf(buf, 128, "%f %f\n", etas[i], pressures[i]);
    fputs(buf, output);
  }
}

int main (int argc, char** argv) {
  // benchmark();
  measure_equation_of_state("eos.txt", 0, 0.6, 20);
  measure_equation_of_state("eos_pt.txt", 0.5, 0.6, 20);
  return 0;
}
