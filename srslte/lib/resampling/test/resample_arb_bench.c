#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "srslte/srslte.h"
#include "srslte/resampling/resample_arb.h"



int main(int argc, char **argv) {
  int N=10000000;
  float rate = 24.0/25.0;
  cf_t *in = malloc(N*sizeof(cf_t));
  cf_t *out = malloc(N*sizeof(cf_t));

  for(int i=0;i<N;i++)
    in[i] = sin(i*2*M_PI/100);

  srslte_resample_arb_t r;
  srslte_resample_arb_init(&r, rate);

  clock_t start = clock(), diff;
  //int n_out = srslte_resample_arb_compute(&r, in, out, N);
  diff = clock() - start;

  int msec = diff * 1000 / CLOCKS_PER_SEC;
  float thru = (CLOCKS_PER_SEC/(float)diff)*(N/1e6);
  printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
  printf("Rate = %f MS/sec\n", thru);

  free(in);
  free(out);
  printf("Done\n");
  exit(0);
}
