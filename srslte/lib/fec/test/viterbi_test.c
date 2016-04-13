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

#include "srslte/srslte.h"

#include "viterbi_test.h"


int frame_length = 1000, nof_frames = 128;
float ebno_db = 100.0;
uint32_t seed = 0;
bool tail_biting = false;

#define SNR_POINTS  10
#define SNR_MIN    0.0
#define SNR_MAX    5.0

void usage(char *prog) {
  printf("Usage: %s [nlest]\n", prog);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-s seed [Default 0=time]\n");
  printf("\t-t tail_bitting [Default %s]\n", tail_biting ? "yes" : "no");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nlste")) != -1) {
    switch (opt) {
    case 'n':
      nof_frames = atoi(argv[optind]);
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
    case 't':
      tail_biting = true;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  int frame_cnt;
  float *llr;
  uint8_t *llr_c;
  uint8_t *data_tx, *data_rx, *data_rx2, *symbols;
  int i, j;
  float var[SNR_POINTS], varunc[SNR_POINTS];
  int snr_points;
  uint32_t errors;
#ifdef TEST_SSE
  uint32_t errors2;
  srslte_viterbi_t dec_sse;
#endif
  srslte_viterbi_t dec; 
  srslte_convcoder_t cod;
  int coded_length;


  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  cod.poly[0] = 0x6D;
  cod.poly[1] = 0x4F;
  cod.poly[2] = 0x57;
  cod.K = 7;
  cod.tail_biting = tail_biting;

  cod.R = 3;
  coded_length = cod.R * (frame_length + ((cod.tail_biting) ? 0 : cod.K - 1));
  srslte_viterbi_init(&dec, SRSLTE_VITERBI_37, cod.poly, frame_length, cod.tail_biting);
  printf("Convolutional Code 1/3 K=%d Tail bitting: %s\n", cod.K, cod.tail_biting ? "yes" : "no");  

#ifdef TEST_SSE
  srslte_viterbi_init_sse(&dec_sse, SRSLTE_VITERBI_37, cod.poly, frame_length, cod.tail_biting);
#endif  
  
  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = malloc(frame_length * sizeof(uint8_t));
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  data_rx = malloc(frame_length * sizeof(uint8_t));
  if (!data_rx) {
    perror("malloc");
    exit(-1);
  }

  data_rx2 = malloc(frame_length * sizeof(uint8_t));
  if (!data_rx2) {
    perror("malloc");
    exit(-1);
  }

  symbols = malloc(coded_length * sizeof(uint8_t));
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = malloc(coded_length * sizeof(float));
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_c = malloc(2 * coded_length * sizeof(uint8_t));
  if (!llr_c) {
    perror("malloc");
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
      varunc[i] = sqrt(1 / (pow(10, ebno_db / 10)));
    }
  } else {
    esno_db = ebno_db + 10 * log10((double) 1 / 3);
    var[0] = sqrt(1 / (pow(10, esno_db / 10)));
    varunc[0] = sqrt(1 / (pow(10, ebno_db / 10)));
    snr_points = 1;
  }

  float Gain = 32;

  for (i = 0; i < snr_points; i++) {
    frame_cnt = 0;
    errors = 0;
#ifdef TEST_SSE
    errors2 = 0;
#endif
    while (frame_cnt < nof_frames) {

      /* generate data_tx */
      for (j = 0; j < frame_length; j++) {
        data_tx[j] = rand() % 2;
      }

      /* uncoded BER */
      for (j = 0; j < frame_length; j++) {
        llr[j] = data_tx[j] ? sqrt(2) : -sqrt(2);
      }
      srslte_ch_awgn_f(llr, llr, varunc[i], frame_length);

      /* coded BER */
      srslte_convcoder_encode(&cod, data_tx, symbols, frame_length);

      for (j = 0; j < coded_length; j++) {
        llr[j] = symbols[j] ? sqrt(2) : -sqrt(2);
      }
      
      srslte_ch_awgn_f(llr, llr, var[i], coded_length);
      
      srslte_vec_quant_fuc(llr, llr_c, Gain, 127.5, 255, coded_length);

      struct timeval t[3];
      gettimeofday(&t[1], NULL);
      int M = 1; 
      
      srslte_vec_fprint_b(stdout, data_tx, frame_length);
      
      for (int i=0;i<M;i++) {
        srslte_viterbi_decode_uc(&dec, llr_c, data_rx, frame_length);      
      }
            
#ifdef TEST_SSE
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      //printf("Execution time:\t\t%.1f us\n", (float) t[0].tv_usec/M);
      gettimeofday(&t[1], NULL);
      for (int i=0;i<M;i++) {
        srslte_viterbi_decode_uc(&dec_sse, llr_c, data_rx2, frame_length);      
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      //printf("Execution time SIMD:\t%.1f us\n", (float) t[0].tv_usec/M);
#endif

      /* check errors */
      errors += srslte_bit_diff(data_tx, data_rx, frame_length);
#ifdef TEST_SSE
      errors2 += srslte_bit_diff(data_tx, data_rx2, frame_length);
#endif      
      frame_cnt++;
      printf("Eb/No: %3.2f %10d/%d   ", SNR_MIN + i * ebno_inc,frame_cnt,nof_frames);
      printf("BER: %.2e  ", (float) errors / (frame_cnt * frame_length));
#ifdef TEST_SSE
      printf("BER2: %.2e  ", (float) errors2 / (frame_cnt * frame_length));
#endif
      printf("\r");
    }
    printf("\n");
    
    if (snr_points == 1) {
      printf("BER    :    %g\t%u errors\n", (float) errors / (frame_cnt * frame_length), errors);      
#ifdef TEST_SSE
      printf("BER SSE:    %g\t%u errors\n", (float) errors2 / (frame_cnt * frame_length), errors2);      
#endif
      
    }
  }
  srslte_viterbi_free(&dec);
#ifdef TEST_SSE  
  srslte_viterbi_free(&dec_sse);
#endif
  
  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  free(data_rx);
  
  if (snr_points == 1) {
    int expected_errors = get_expected_errors(nof_frames, seed, frame_length, tail_biting, ebno_db);
    if (expected_errors == -1) {
      fprintf(stderr, "Test parameters not defined in test_results.h\n");
      exit(-1);
    } else {
      printf("errors =%d, expected =%d\n", errors, expected_errors);
      exit(errors > expected_errors);
    }
  } else {
    printf("\n");
    printf("Done\n");
    exit(0);
  }
}
