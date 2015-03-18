#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#include "srslte/phy/utils/dft.h"

typedef _Complex float cf_t;

int N = 256;
bool forward = true;
bool mirror = false;
bool norm = false;
bool dc = false;

void usage(char *prog) {
  printf("Usage: %s\n", prog);
  printf("\t-N Transform size [Default 256]\n");
  printf("\t-b Backwards transform first [Default Forwards]\n");
  printf("\t-m Mirror the transform freq bins [Default false]\n");
  printf("\t-n Normalize the transform output [Default false]\n");
  printf("\t-d Handle insertion/removal of null DC carrier internally [Default false]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "Nbmnd")) != -1) {
    switch (opt) {
    case 'N':
      N = atoi(argv[optind]);
      break;
    case 'b':
      forward = false;
      break;
    case 'm':
      mirror = true;
      break;
    case 'n':
      norm = true;
      break;
    case 'd':
      dc = true;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

void print(cf_t* in, int len)
{
  for(int i=0;i<len;i++)
  {
    float re = crealf(in[i]);
    float im = cimagf(in[i]);
    printf("%f+%fi, ", re, im);
  }
  printf("\n\n");
}

int test_dft(cf_t* in){
  int res = 0;

  dft_plan_t plan;
  if(forward){
    dft_plan(&plan, N, FORWARD, COMPLEX);
  } else {
    dft_plan(&plan, N, BACKWARD, COMPLEX);
  }
  dft_plan_set_mirror(&plan, mirror);
  dft_plan_set_norm(&plan, norm);
  dft_plan_set_dc(&plan, dc);

  cf_t* out1 = malloc(sizeof(cf_t)*N);
  cf_t* out2 = malloc(sizeof(cf_t)*N);
  bzero(out1, sizeof(cf_t)*N);
  bzero(out2, sizeof(cf_t)*N);

  print(in, N);
  dft_run(&plan, in, out1);
  print(out1, N);

  dft_plan_t plan_rev;
  if(!forward){
    dft_plan(&plan_rev, N, FORWARD, COMPLEX);
  } else {
    dft_plan(&plan_rev, N, BACKWARD, COMPLEX);
  }
  dft_plan_set_mirror(&plan_rev, mirror);
  dft_plan_set_norm(&plan_rev, norm);
  dft_plan_set_dc(&plan_rev, dc);

  dft_run(&plan_rev, out1, out2);
  print(out2, N);

  if(!norm){
    cf_t n = N+0*I;
    for(int i=0;i<N;i++)
      out2[i] /= n;
  }

  for(int i=0;i<N;i++){
    float diff = cabsf(in[i] - out2[i]);
    if(diff > 0.01)
      res = -1;
  }

  dft_plan_free(&plan);
  dft_plan_free(&plan_rev);
  free(out1);
  free(out2);

  return res;
}

int main(int argc, char **argv) {
  parse_args(argc, argv);
  cf_t* in = malloc(sizeof(cf_t)*N);
  bzero(in, sizeof(cf_t)*N);
  for(int i=1;i<N-1;i++)
  {
    float re = 100*(float)rand()/RAND_MAX;
    float im = 100*(float)rand()/RAND_MAX;
    in[i] = re + im*I;
  }

  if(test_dft(in) != 0)
    return -1;

  free(in);
	printf("Done\n");
	exit(0);
}
