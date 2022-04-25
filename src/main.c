#include "steric_gas.h"
#include <time.h>

void benchmark() {
  printf("[?] Starting Benchmark...\n");
  struct steric_gas steric_gas;
  steric_gas_init(&steric_gas);
  steric_gas_fill_to_packing_fraction(&steric_gas, 1.0, .5497);
  container_export(&steric_gas.container, "init.dat");

  clock_t begin = clock();
  steric_gas_simulate(&steric_gas, 100000);
  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("\r[+] Done. Took: %fs\n", time_spent);

  container_export(&steric_gas.container, "final.dat");
  container_destroy(&steric_gas.container);
  printf("\r[+] Finished.                         \n");
}

int main (int argc, char** argv) {
  benchmark();
  return 0;
}
