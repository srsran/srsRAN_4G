/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <math.h>
#include <srsran/phy/utils/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/srsran.h"

#include "viterbi_test.h"

static int      frame_length = 1000, nof_frames = 256;
static float    ebno_db     = 100.0;
static uint32_t seed        = 0;
static bool     tail_biting = false;

#define SNR_POINTS 10
#define SNR_MIN 0.0
#define SNR_MAX 5.0

void usage(char* prog)
{
  printf("Usage: %s [nlest]\n", prog);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l frame_length [Default %d]\n", frame_length);
  printf("\t-e ebno in dB [Default scan]\n");
  printf("\t-s seed [Default 0=time]\n");
  printf("\t-t tail_bitting [Default %s]\n", tail_biting ? "yes" : "no");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nlste")) != -1) {
    switch (opt) {
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        frame_length = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        ebno_db = strtof(argv[optind], NULL);
        break;
      case 's':
        seed = (uint32_t)strtoul(argv[optind], NULL, 0);
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

#define VITERBI_TEST(FUNC, DEC, LLR, NOF_ERRORS)                                                                       \
  do {                                                                                                                 \
    struct timeval t[3] = {};                                                                                          \
    int            M    = 1;                                                                                           \
    gettimeofday(&t[1], NULL);                                                                                         \
    for (uint32_t m = 0; m < M && NOF_ERRORS >= 0; m++) {                                                              \
      int ret = FUNC(&DEC, LLR, data_rx, frame_length);                                                                \
      if (ret < SRSRAN_SUCCESS) {                                                                                      \
        NOF_ERRORS = ret;                                                                                              \
      }                                                                                                                \
    }                                                                                                                  \
    gettimeofday(&t[2], NULL);                                                                                         \
    get_time_interval(t);                                                                                              \
    /*printf("-- "#FUNC" took\t\t%.1f us\n", (float) t[0].tv_sec * 1e6f + (float) t[0].tv_usec/M);*/                   \
    if (NOF_ERRORS >= 0) {                                                                                             \
      NOF_ERRORS += srsran_bit_diff(data_tx, data_rx, frame_length);                                                   \
    }                                                                                                                  \
  } while (0)

//#define TEST_SSE

int main(int argc, char** argv)
{
  int       frame_cnt = 0;
  float*    llr       = NULL;
  uint16_t* llr_us    = NULL;
  int16_t*  llr_s     = NULL;
  uint8_t*  llr_c     = NULL;
  uint8_t * data_tx, *data_rx, *symbols;
  float     var[SNR_POINTS], varunc[SNR_POINTS];
  int       snr_points;
  int       errors_s   = 0;
  int       errors_us  = 0;
  int       errors_c   = 0;
  int       errors_f   = 0;
  int       errors_sse = 0;
#ifdef TEST_SSE
  srsran_viterbi_t dec_sse;
#endif
  srsran_viterbi_t   dec;
  srsran_convcoder_t cod;
  int                coded_length;

  parse_args(argc, argv);

  if (!seed) {
    seed = time(NULL);
  }
  srand(seed);

  cod.poly[0]     = 0x6D;
  cod.poly[1]     = 0x4F;
  cod.poly[2]     = 0x57;
  cod.K           = 7;
  cod.tail_biting = tail_biting;

  cod.R        = 3;
  coded_length = cod.R * (frame_length + ((cod.tail_biting) ? 0 : cod.K - 1));
  srsran_viterbi_init(&dec, SRSRAN_VITERBI_37, cod.poly, frame_length, cod.tail_biting);
  printf("Convolutional Code 1/3 K=%d Tail bitting: %s\n", cod.K, cod.tail_biting ? "yes" : "no");

#ifdef TEST_SSE
  srsran_viterbi_init_sse(&dec_sse, SRSRAN_VITERBI_37, cod.poly, frame_length, cod.tail_biting);
#endif

  printf("  Frame length: %d\n", frame_length);
  if (ebno_db < 100.0) {
    printf("  EbNo: %.2f\n", ebno_db);
  }

  data_tx = srsran_vec_u8_malloc(frame_length);
  if (!data_tx) {
    perror("malloc");
    exit(-1);
  }

  data_rx = srsran_vec_u8_malloc(frame_length);
  if (!data_rx) {
    perror("malloc");
    exit(-1);
  }

  symbols = srsran_vec_u8_malloc(coded_length);
  if (!symbols) {
    perror("malloc");
    exit(-1);
  }
  llr = srsran_vec_f_malloc(coded_length);
  if (!llr) {
    perror("malloc");
    exit(-1);
  }
  llr_s = srsran_vec_i16_malloc(2 * coded_length);
  if (!llr_s) {
    perror("malloc");
    exit(-1);
  }
  llr_us = srsran_vec_u16_malloc(2 * coded_length);
  if (!llr_us) {
    perror("malloc");
    exit(-1);
  }
  llr_c = srsran_vec_u8_malloc(2 * coded_length);
  if (!llr_c) {
    perror("malloc");
    exit(-1);
  }

  float ebno_inc, esno_db;
  ebno_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;
  if (ebno_db == 100.0) {
    snr_points = SNR_POINTS;
    for (uint32_t i = 0; i < snr_points; i++) {
      ebno_db   = SNR_MIN + i * ebno_inc;
      esno_db   = ebno_db + srsran_convert_power_to_dB(1.0f / 3.0f);
      var[i]    = srsran_convert_dB_to_power(-esno_db);
      varunc[i] = srsran_convert_dB_to_power(-ebno_db);
    }
  } else {
    esno_db    = ebno_db + srsran_convert_power_to_dB(1.0f / 3.0f);
    var[0]     = srsran_convert_dB_to_power(-esno_db);
    varunc[0]  = srsran_convert_dB_to_power(-ebno_db);
    snr_points = 1;
  }

  for (uint32_t i = 0; i < snr_points; i++) {
    frame_cnt  = 0;
    errors_s   = 0;
    errors_c   = 0;
    errors_f   = 0;
    errors_sse = 0;
    while (frame_cnt < nof_frames) {
      /* generate data_tx */
      srsran_random_t random_gen = srsran_random_init(0);
      for (int j = 0; j < frame_length; j++) {
        data_tx[j] = srsran_random_uniform_int_dist(random_gen, 0, 1);
      }
      srsran_random_free(random_gen);

      /* uncoded BER */
      for (int j = 0; j < frame_length; j++) {
        llr[j] = data_tx[j] ? M_SQRT2 : -M_SQRT2;
      }
      srsran_ch_awgn_f(llr, llr, varunc[i], frame_length);

      /* coded BER */
      srsran_convcoder_encode(&cod, data_tx, symbols, frame_length);

      for (int j = 0; j < coded_length; j++) {
        llr[j] = symbols[j] ? M_SQRT2 : -M_SQRT2;
      }

      srsran_ch_awgn_f(llr, llr, var[i], coded_length);
      // srsran_vec_fprint_f(stdout, llr, 100);

      srsran_vec_convert_fi(llr, 1000, llr_s, coded_length);
      srsran_vec_quant_fuc(llr, llr_c, 32, INT8_MAX, UINT8_MAX, coded_length);
      srsran_vec_quant_fus(llr, llr_us, 8192, INT16_MAX, UINT16_MAX, coded_length);

      VITERBI_TEST(srsran_viterbi_decode_s, dec, llr_s, errors_s);
      VITERBI_TEST(srsran_viterbi_decode_us, dec, llr_us, errors_us);
      VITERBI_TEST(srsran_viterbi_decode_uc, dec, llr_c, errors_c);
      VITERBI_TEST(srsran_viterbi_decode_f, dec, llr, errors_f);
#ifdef TEST_SSE
      VITERBI_TEST(srsran_viterbi_decode_uc, dec_sse, llr_c, errors_sse);
#endif
      frame_cnt++;
      printf("     Eb/No: %3.2f %10d/%d   ", SNR_MIN + i * ebno_inc, frame_cnt, nof_frames);
      if (errors_s >= 0)
        printf(" int16 BER: %.2e  ", (float)errors_s / (frame_cnt * frame_length));
      if (errors_us >= 0)
        printf("uint16 BER: %.2e  ", (float)errors_us / (frame_cnt * frame_length));
      if (errors_c >= 0)
        printf("uint8  BER: %.2e  ", (float)errors_c / (frame_cnt * frame_length));
      if (errors_f >= 0)
        printf("float  BER: %.2e  ", (float)errors_f / (frame_cnt * frame_length));
#ifdef TEST_SSE
      printf("sse    BER: %.2e  ", (float)errors_sse / (frame_cnt * frame_length));
#endif
      printf("\r\n");
    }
    printf("\n");

    if (snr_points == 1) {
      if (errors_s >= 0)
        printf(" int16 BER    :    %g\t%u errors\n", (float)errors_s / (frame_cnt * frame_length), errors_s);
      if (errors_us >= 0)
        printf("uint16 BER    :    %g\t%u errors\n", (float)errors_us / (frame_cnt * frame_length), errors_us);
      if (errors_c >= 0)
        printf("uint8  BER    :    %g\t%u errors\n", (float)errors_c / (frame_cnt * frame_length), errors_c);
      if (errors_f >= 0)
        printf("float  BER    :    %g\t%u errors\n", (float)errors_f / (frame_cnt * frame_length), errors_f);
#ifdef TEST_SSE
      printf("sse    BER    :    %g\t%u errors\n", (float)errors_sse / (frame_cnt * frame_length), errors_sse);
#endif
    }
  }
  srsran_viterbi_free(&dec);
#ifdef TEST_SSE
  srsran_viterbi_free(&dec_sse);
#endif

  free(data_tx);
  free(symbols);
  free(llr);
  free(llr_c);
  free(llr_s);
  free(llr_us);
  free(data_rx);

  if (snr_points == 1) {
    int expected_e = get_expected_errors(nof_frames, seed, frame_length, tail_biting, ebno_db);
    if (expected_e == -1) {
      ERROR("Test parameters not defined in test_results.h");
      exit(-1);
    } else {
      printf(
          "errors =(%d,%d,%d,%d,%d), expected =%d\n", errors_s, errors_us, errors_c, errors_f, errors_sse, expected_e);
      bool passed = true;
      passed &= (bool)(errors_us <= expected_e);
      passed &= (bool)(errors_s <= expected_e);
      passed &= (bool)(errors_c <= expected_e);
      passed &= (bool)(errors_f <= expected_e);
      passed &= (bool)(errors_sse <= expected_e);
      exit(!passed);
    }
  } else {
    printf("\n");
    printf("Done\n");
    exit(0);
  }
}
