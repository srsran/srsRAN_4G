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
int K = -1;

#define SNR_POINTS  10
#define SNR_MIN    0.0
#define SNR_MAX    5.0

#define NCODS    3
#define NTYPES    1+NCODS

void usage(char *prog) {
  printf("Usage: %s [nlestk]\n", prog);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-s seed [Default 0=time]\n");
  printf("\t-t tail_bitting [Default %s]\n", tail_biting ? "yes" : "no");
  printf("\t-k constraint length [Default both]\n", K);
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nlstek")) != -1) {
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
    case 'k':
      K = atoi(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

void output_matlab(float ber[NTYPES][SNR_POINTS], int snr_points,
    srslte_convcoder_t cod[NCODS], int ncods) {
  int i, j, n;
  FILE *f = fopen("srslte_viterbi_snr.m", "w");
  if (!f) {
    perror("fopen");
    exit(-1);
  }
  fprintf(f, "ber=[");
  for (j = 0; j < NTYPES; j++) {
    for (i = 0; i < snr_points; i++) {
      fprintf(f, "%g ", ber[j][i]);
    }
    fprintf(f, "; ");
  }
  fprintf(f, "];\n");
  fprintf(f, "snr=linspace(%g,%g-%g/%d,%d);\n", SNR_MIN, SNR_MAX, SNR_MAX,
      snr_points, snr_points);
  fprintf(f, "semilogy(snr,ber,snr,0.5*erfc(sqrt(10.^(snr/10))));\n");
  fprintf(f, "legend('uncoded',");
  for (n=0;n<ncods;n++) {
    fprintf(f,"'1/3 K=%d%s',",cod[n].K,cod[n].tail_biting?" tb":"");
  }
  fprintf(f,"'theory-uncoded');");
  fprintf(f, "grid on;\n");
  fclose(f);
}

int main(int argc, char **argv) {
  int frame_cnt;
  float *llr;
  uint8_t *llr_c;
  uint8_t *data_tx, *data_rx[NTYPES], *symbols;
  int i, j;
  float var[SNR_POINTS], varunc[SNR_POINTS];
  int snr_points;
  float ber[NTYPES][SNR_POINTS];
  uint32_t errors[NTYPES];
  srslte_viterbi_type_t srslte_viterbi_type[NCODS];
  srslte_viterbi_t dec[NCODS];
  srslte_convcoder_t cod[NCODS];
  int coded_length[NCODS];
  int n, ncods, max_coded_length;


  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  switch (K) {
  case 9:
    cod[0].poly[0] = 0x1ed;
    cod[0].poly[1] = 0x19b;
    cod[0].poly[2] = 0x127;
    cod[0].tail_biting = false;
    cod[0].K = 9;
    srslte_viterbi_type[0] = SRSLTE_VITERBI_39;
    ncods=1;
    break;
  case 7:
    cod[0].poly[0] = 0x6D;
    cod[0].poly[1] = 0x4F;
    cod[0].poly[2] = 0x57;
    cod[0].K = 7;
    cod[0].tail_biting = tail_biting;
    srslte_viterbi_type[0] = SRSLTE_VITERBI_37;
    ncods=1;
    break;
  default:
    cod[0].poly[0] = 0x1ed;
    cod[0].poly[1] = 0x19b;
    cod[0].poly[2] = 0x127;
    cod[0].tail_biting = false;
    cod[0].K = 9;
    srslte_viterbi_type[0] = SRSLTE_VITERBI_39;
    cod[1].poly[0] = 0x6D;
    cod[1].poly[1] = 0x4F;
    cod[1].poly[2] = 0x57;
    cod[1].tail_biting = false;
    cod[1].K = 7;
    srslte_viterbi_type[1] = SRSLTE_VITERBI_37;
    cod[2].poly[0] = 0x6D;
    cod[2].poly[1] = 0x4F;
    cod[2].poly[2] = 0x57;
    cod[2].tail_biting = true;
    cod[2].K = 7;
    srslte_viterbi_type[2] = SRSLTE_VITERBI_37;
    ncods=3;
  }

  max_coded_length = 0;
  for (i=0;i<ncods;i++) {
    cod[i].R = 3;
    coded_length[i] = cod[i].R * (frame_length + ((cod[i].tail_biting) ? 0 : cod[i].K - 1));
    if (coded_length[i] > max_coded_length) {
      max_coded_length = coded_length[i];
    }
    srslte_viterbi_init(&dec[i], srslte_viterbi_type[i], cod[i].poly, frame_length, cod[i].tail_biting);
    printf("Convolutional Code 1/3 K=%d Tail bitting: %s\n", cod[i].K, cod[i].tail_biting ? "yes" : "no");
  }

  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = malloc(frame_length * sizeof(uint8_t));
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  for (i = 0; i < NTYPES; i++) {
    data_rx[i] = malloc(frame_length * sizeof(uint8_t));
    if (!data_rx[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  symbols = malloc(max_coded_length * sizeof(uint8_t));
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = malloc(max_coded_length * sizeof(float));
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_c = malloc(2 * max_coded_length * sizeof(uint8_t));
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
    for (j = 0; j < NTYPES; j++) {
      errors[j] = 0;
    }
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
      for (j = 0; j < frame_length; j++) {
        data_rx[0][j] = llr[j] > 0 ? 1 : 0;
      }

      /* coded BER */
      for (n=0;n<ncods;n++) {
        srslte_convcoder_encode(&cod[n], data_tx, symbols, frame_length);

        for (j = 0; j < coded_length[n]; j++) {
          llr[j] = symbols[j] ? sqrt(2) : -sqrt(2);
        }

        srslte_ch_awgn_f(llr, llr, var[i], coded_length[n]);
        srslte_vec_quant_fuc(llr, llr_c, Gain, 127.5, 255, coded_length[n]);

        /* decoder 1 */
        srslte_viterbi_decode_uc(&dec[n], llr_c, data_rx[1+n], frame_length);
      }

      /* check errors */
      for (j = 0; j < 1+ncods; j++) {
        errors[j] += srslte_bit_diff(data_tx, data_rx[j], frame_length);
      }
      frame_cnt++;
      printf("Eb/No: %3.2f %10d/%d   ",
          SNR_MIN + i * ebno_inc,frame_cnt,nof_frames);
      for (n=0;n<1+ncods;n++) {
        printf("BER: %.2e  ",(float) errors[n] / (frame_cnt * frame_length));
      }
      printf("\r");
    }
    printf("\n");
    for (j = 0; j < 1+ncods; j++) {
      ber[j][i] = (float) errors[j] / (frame_cnt * frame_length);
    }

    if (snr_points == 1) {
          printf("BER uncoded: %g\t%u errors\n",
          (float) errors[0] / (frame_cnt * frame_length), errors[0]);
      for (n=0;n<ncods;n++) {
        printf("BER K=%d:    %g\t%u errors\n",cod[n].K,
            (float) errors[1+n] / (frame_cnt * frame_length), errors[1+n]);
      }
    }
  }
  for (n=0;n<ncods;n++) {
    srslte_viterbi_free(&dec[n]);
  }

  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  for (i = 0; i < NTYPES; i++) {
    free(data_rx[i]);
  }

  if (snr_points == 1) {
    int expected_errors = get_expected_errors(nof_frames,
        seed, frame_length, K, tail_biting, ebno_db);
    if (expected_errors == -1) {
      fprintf(stderr, "Test parameters not defined in test_results.h\n");
      exit(-1);
    } else {
      printf("errors =%d, expected =%d\n", errors[1], expected_errors);
      exit(errors[1] > expected_errors);
    }
  } else {
    printf("\n");
    output_matlab(ber, snr_points, cod, ncods);
    printf("Done\n");
    exit(0);
  }
}
