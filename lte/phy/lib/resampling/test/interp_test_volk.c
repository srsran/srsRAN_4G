#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "liblte/phy/phy.h"

typedef _Complex float cf_t;

int main(int argc, char **argv) {
  uint32_t N = 1000;        // Number of sinwave samples
  interp_t interp; 
  struct timeval t[3];
  
  if (argc < 3) {
    printf("usage: %s upsampling_rate nof_trials\n", argv[0]);
    exit(-1);
  }
  
  uint32_t M = atoi(argv[1]);
  uint32_t nof_trials = atoi(argv[2]);
  
  if (interp_init(&interp, LINEAR, N, M)) {
    exit(-1);
  }

  cf_t *in = vec_malloc(N*sizeof(cf_t));
  if(!in) {
    perror("malloc");
    exit(-1);
  }
  cf_t *out = vec_malloc(M * N*sizeof(cf_t));
  if(!out) {
    perror("malloc");
    exit(-1);
  }
  cf_t *out_volk = vec_malloc(M * N*sizeof(cf_t));
  if(!out_volk) {
    perror("malloc");
    exit(-1);
  }

  srand(time(NULL));
  for(uint32_t i=0;i<N;i++) {
    //in[i] = sin((float) i*2*M_PI/N)+10*I*cos((float) i*2*M_PI/N);
    in[i] = (float) rand()/RAND_MAX-0.5 + ((float) rand()/RAND_MAX - 0.5) * I;
  }
  uint32_t exec_normal, exec_volk;
  float mean_normal=0, mean_volk=0;
  bzero(out_volk, M * N*sizeof(cf_t));
  bzero(out, M * N*sizeof(cf_t));
  
  for (int n=0;n<nof_trials;n++) {

    gettimeofday(&t[1], NULL);
    interp_linear_c(in, out, M, N);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    exec_normal = t[0].tv_usec;
    
    gettimeofday(&t[1], NULL);
    interp_run(&interp, in, out_volk);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    exec_volk = t[0].tv_usec; 
    
    if (n == 0) {
      exec_volk = 0;
    }
    
    mean_normal = (mean_normal + exec_normal * n) / (n+1);
    mean_volk = (mean_volk + exec_volk * n) / (n+1);
  }

  // Check interp values
  float diff = 0.0;
  for(int i=0;i<N*M;i++){
    diff += cabsf(out_volk[i] - out[i])/N/M; 
  }
  printf("%d: error=%f Exec: %f - %f\n", M, diff, mean_normal, mean_volk);

  free(in);
  free(out);
  free(out_volk);
  
  interp_free(&interp);

  printf("Ok\n");
  exit(0);
}
