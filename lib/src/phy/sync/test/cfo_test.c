/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/srsran.h"

#define MAX_MSE 0.1

float freq        = 0;
int   num_samples = 1000;

void usage(char* prog)
{
  printf("Usage: %s -f freq -n num_samples\n", prog);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nf")) != -1) {
    switch (opt) {
      case 'n':
        num_samples = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        freq = strtof(argv[optind], NULL);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int          i;
  cf_t *       input, *output;
  srsran_cfo_t cfocorr;
  float        mse;

  if (argc < 5) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc, argv);

  input = srsran_vec_cf_malloc(num_samples);
  if (!input) {
    perror("malloc");
    exit(-1);
  }
  output = srsran_vec_cf_malloc(num_samples);
  if (!output) {
    perror("malloc");
    exit(-1);
  }

  for (i = 0; i < num_samples; i++) {
    input[i]  = 100 * (rand() / RAND_MAX + I * rand() / RAND_MAX);
    output[i] = input[i];
  }

  if (srsran_cfo_init(&cfocorr, num_samples)) {
    ERROR("Error initiating CFO");
    return -1;
  }

  srsran_cfo_correct(&cfocorr, output, output, freq);
  srsran_cfo_correct(&cfocorr, output, output, -freq);

  mse = 0;
  for (i = 0; i < num_samples; i++) {
    mse += cabsf(input[i] - output[i]) / num_samples;
  }

  srsran_cfo_free(&cfocorr);
  free(input);
  free(output);

  printf("MSE: %f\n", mse);
  if (mse > MAX_MSE) {
    printf("MSE too large\n");
    exit(-1);
  } else {
    printf("Ok\n");
    exit(0);
  }
}
