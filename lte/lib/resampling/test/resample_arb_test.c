#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "lte.h"
#include "lte/resampling/resample_arb.h"

typedef _Complex float cf_t;

int main(int argc, char **argv) {
  int N = 100;        // Number of sinwave samples
  int delay = 5;      // Delay of our resampling filter
  float down = 25.0;  // Downsampling rate

  for(int up=1;up<down;up++)
  {
    float rate = up/down;
    printf("Testing rate %f\n", rate);

    cf_t *in = malloc(N*sizeof(cf_t));
    if(!in) {
      perror("malloc");
      exit(-1);
    }
    cf_t *out = malloc(N*sizeof(cf_t));
    if(!out) {
      perror("malloc");
      exit(-1);
    }

    for(int i=0;i<N;i++)
      in[i] = sin(i*2*M_PI/N);

    // Resample
    resample_arb_t r;
    resample_arb_init(&r, rate);
    int n_out = resample_arb_compute(&r, in, out, N);

    // Check interp values
    for(int i=delay+1;i<n_out;i++){
      float idx = i/rate;
      int pre = floor(idx)-delay;
      int post = ceil(idx)-delay;
      int round = roundf(idx)-delay;
      float diff = fabs(creal(in[pre])-creal(in[post]));
      float diff2 = fabs(creal(out[i])-creal(in[round]));
      if(diff2 > diff && pre != post){
          printf("Interpolation failed at index %f", idx);
          exit(-1);
      }
    }


    free(in);
    free(out);
  }

  printf("Ok\n");
  exit(0);
}
