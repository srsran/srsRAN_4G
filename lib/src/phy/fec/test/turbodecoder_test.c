/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <sys/time.h>
#include <time.h>
#include "srslte/srslte.h"

#include "turbodecoder_test.h"



uint32_t frame_length = 1000, nof_frames = 100;
float ebno_db = 100.0;
uint32_t seed = 0;
int K = -1;

#define MAX_ITERATIONS  10
int nof_cb = 1; 
int nof_iterations = MAX_ITERATIONS;
int test_known_data = 0;
int test_errors = 0;
int nof_repetitions = 1; 

#define SNR_POINTS      4
#define SNR_MIN         1.0
#define SNR_MAX         8.0

void usage(char *prog) {
  printf("Usage: %s [nlesv]\n", prog);
  printf(
      "\t-k Test with known data (ignores frame_length) [Default disabled]\n");
  printf("\t-c nof_cb in parallel [Default %d]\n", nof_cb);
  printf("\t-i nof_iterations [Default %d]\n", nof_iterations);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-N nof_repetitions [Default %d]\n", nof_repetitions);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-t test: check errors on exit [Default disabled]\n");
  printf("\t-s seed [Default 0=time]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "cinNlstvekt")) != -1) {
    switch (opt) {
    case 'c':
      nof_cb = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'N':
      nof_repetitions = atoi(argv[optind]);
      break;
    case 'k':
      test_known_data = 1;
      break;
    case 't':
      test_errors = 1;
      break;
    case 'i':
      nof_iterations = atoi(argv[optind]);
      break;
    case 'l':
      frame_length = atoi(argv[optind]);
      break;
    case 'e':
      ebno_db = atof(argv[optind]);
      break;
    case 's':
      seed = (uint32_t) strtoul(argv[optind], NULL, 0);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}


int main(int argc, char **argv) {
  uint32_t frame_cnt;
  float *llr;
  short *llr_s;
  uint8_t *llr_c;
  uint8_t *data_tx, *data_rx, *data_rx_bytes[SRSLTE_TDEC_MAX_NPAR], *symbols;
  uint32_t i, j;
  float var[SNR_POINTS];
  uint32_t snr_points;
  uint32_t errors;
  uint32_t coded_length;
  struct timeval tdata[3];
  float mean_usec;
  srslte_tdec_t tdec;
  srslte_tcod_t tcod;
  
  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  if (test_known_data) {
    frame_length = KNOWN_DATA_LEN;
  } else {
    frame_length = srslte_cbsegm_cbsize(srslte_cbsegm_cbindex(frame_length));
  }

  coded_length = 3 * (frame_length) + SRSLTE_TCOD_TOTALTAIL;

  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = srslte_vec_malloc(frame_length * sizeof(uint8_t));
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  data_rx = srslte_vec_malloc(frame_length * sizeof(uint8_t));
  if (!data_rx) {
    perror("malloc");
    exit(-1);
  }
  for (int cb=0;cb<SRSLTE_TDEC_MAX_NPAR;cb++) {
    data_rx_bytes[cb] = srslte_vec_malloc(frame_length * sizeof(uint8_t));
    if (!data_rx_bytes[cb]) {
      perror("malloc");
      exit(-1);
    }    
  }

  symbols = srslte_vec_malloc(coded_length * sizeof(uint8_t));
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = srslte_vec_malloc(coded_length * sizeof(float));
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_s = srslte_vec_malloc(coded_length * sizeof(short));
  if (!llr_s) {
    perror("malloc");
    exit(-1);
  }
  llr_c = srslte_vec_malloc(coded_length * sizeof(uint8_t));
  if (!llr_c) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_tcod_init(&tcod, frame_length)) {
    fprintf(stderr, "Error initiating Turbo coder\n");
    exit(-1);
  }

  if (srslte_tdec_init(&tdec, frame_length)) {
    fprintf(stderr, "Error initiating Turbo decoder\n");
    exit(-1);
  }

  float ebno_inc, esno_db;
  ebno_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;
  if (ebno_db == 100.0) {
    snr_points = SNR_POINTS;
    for (i = 0; i < snr_points; i++) {
      ebno_db = SNR_MIN + i * ebno_inc;
      esno_db = ebno_db + 10 * log10((double) 1 / 3);
      var[i] = sqrt(1 / (pow(10, esno_db / 10)));
    }
  } else {
    esno_db = ebno_db + 10 * log10((double) 1 / 3);
    var[0] = sqrt(1 / (pow(10, esno_db / 10)));
    snr_points = 1;
  }
  for (i = 0; i < snr_points; i++) {

    mean_usec = 0;
    errors = 0; 
    frame_cnt = 0;
    while (frame_cnt < nof_frames) {
      /* generate data_tx */
      for (j = 0; j < frame_length; j++) {
        if (test_known_data) {
          data_tx[j] = known_data[j];
        } else {
          data_tx[j] = rand() % 2;
        }
      }

      /* coded BER */
      if (test_known_data) {
        for (j = 0; j < coded_length; j++) {
          symbols[j] = known_data_encoded[j];
        }
      } else {
        srslte_tcod_encode(&tcod, data_tx, symbols, frame_length);
      }

      for (j = 0; j < coded_length; j++) {
        llr[j] = symbols[j] ? 1 : -1;
      }
      srslte_ch_awgn_f(llr, llr, var[i], coded_length);

      for (j=0;j<coded_length;j++) {
        llr_s[j] = (int16_t) (100*llr[j]);
      }

      /* decoder */
      srslte_tdec_reset(&tdec, frame_length);

      uint32_t t;
      if (nof_iterations == -1) {
        t = MAX_ITERATIONS;
      } else {
        t = nof_iterations;
      }

      int16_t *input[SRSLTE_TDEC_MAX_NPAR];
      uint8_t *output[SRSLTE_TDEC_MAX_NPAR];
      
      for (int n=0;n<SRSLTE_TDEC_MAX_NPAR;n++) {
        if (n < nof_cb) {
          input[n] = llr_s;
          output[n] = data_rx_bytes[n];
        } else {
          input[n] = NULL;
          output[n] = NULL;
        }
      }

      gettimeofday(&tdata[1], NULL); 
      for (int k=0;k<nof_repetitions;k++) { 
        srslte_tdec_run_all_par(&tdec, input, output, t, frame_length);        
      }
      gettimeofday(&tdata[2], NULL);
      get_time_interval(tdata);
      mean_usec = (float) mean_usec * 0.9 + (float) (tdata[0].tv_usec/nof_repetitions) * 0.1;
      
      frame_cnt++;
      uint32_t errors_this = 0; 
      for (int cb=0;cb<nof_cb;cb++) {
        srslte_bit_unpack_vector(data_rx_bytes[cb], data_rx, frame_length);
        
        errors_this=srslte_bit_diff(data_tx, data_rx, frame_length);
        //printf("error[%d]=%d\n", cb, errors_this);
        errors += errors_this;
      }
      printf("Eb/No: %2.2f %10d/%d   ", SNR_MIN + i * ebno_inc, frame_cnt, nof_frames);
      printf("BER: %.2e  ", (float) errors / (nof_cb*frame_cnt * frame_length));
      printf("%3.1f Mbps (%6.2f usec)", (float) (nof_cb*frame_length) / mean_usec, mean_usec);
      printf("\r");        

    }    
    printf("\n");
  }

  printf("\n");
  if (snr_points == 1) {
    if (errors) {
      printf("%d Errors\n", errors/nof_cb);
    }
  }

  for (int cb=0;cb<SRSLTE_TDEC_MAX_NPAR;cb++) {
    if (data_rx_bytes[cb]) {
      free(data_rx_bytes[cb]);
    }
  }
  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  free(llr_s);
  free(data_rx);

  srslte_tdec_free(&tdec);
  srslte_tcod_free(&tcod);

  printf("\n");
  printf("Done\n");
  exit(0);
}
