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

/*!
 * \file polar_chain_test.c
 * \brief Ent-to-end test for the Polar coding chain including: subchannel allocator, encoder, rate-matcher,
 rate-dematcher, decoder and subchannel deallocation.
 *
 * A batch of example messages is randomly generated, frozen bits are added, encoded, rate-matched, 2-PAM modulated,
 * sent over an AWGN channel, rate-dematched, and, finally, decoded by all three types of
 * decoder. Transmitted and received messages are compared to estimate the WER.
 * Multiple batches are simulated if the number of errors is not significant
 * enough.
 *
 * Synopsis: **polar_chain_test [options]**
 *
 * Options:
 *
 *  - <b>-n \<number\></b> nMax,  [Default 9] -- Use 9 for downlink, and 10 for uplink configuration.
 *  - <b>-k \<number\></b> Message size (K),  [Default 128]. K includes the CRC bits if applicable.
 *  If nMax = 9, K must satisfy 165 > K > 35. If nMax = 10, K must satisfy K > 17 and K <1024, excluding 31 > K > 25.
 *  - <b>-e \<number\></b> Rate matching size (E), [Default 256]. If 17 < K < 26, E must satisfy K +3 < E < 8193.
 * If K > 30, E must satisfy K < E < 8193.
 *  - <b>-i \<number\></b> Enable bit interleaver (bil),  [Default 0] -- Set bil = 0 to disable the
 * bit interleaver at rate matching. Choose 0 for downlink and 1 for uplink configuration.
 *  - <b>-s \<number\></b>  SNR [dB, Default 3.00 dB] -- Use 100 for scan, and 101 for noiseless.
 *  - <b>-o \<number\></b>  Print output results [Default 0] -- Use 0 for detailed, Use 1 for 1 line, Use 2 for vector
 * form.
 *
 * Example 1: BCH - ./polar_chain_test -n9 -k56 -e864 -i0 -s101 -o1
 *
 * Example 2: DCI - ./polar_chain_test -n9 -k40 -e100 -i0 -s101 -o1
 *
 * Example 3: UCI - PC bits - ./polar_chain_test -n10 -k20 -e256 -i1 -s101 -o1
 *
 * Example 4: UCI - puncturing 19 first bits - ./polar_chain_test -n10 -k18 -e45 -i1 -s101 -o1
 *
 * Example 5: UCI - shortening 26 last bits - ./polar_chain_test -n10 -k18 -e38 -i1 -s101 -o1
 *
 *
 */

#include "math.h"

#include "srsran/phy/channel/ch_awgn.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/phy_logger.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// utils lib
#include "srsran/phy/utils/vector.h"

//  polar libs
#include "srsran/phy/fec/polar/polar_chanalloc.h"
#include "srsran/phy/fec/polar/polar_code.h"
#include "srsran/phy/fec/polar/polar_decoder.h"
#include "srsran/phy/fec/polar/polar_encoder.h"
#include "srsran/phy/fec/polar/polar_rm.h"

//#define debug
//#define DATA_ALL_ONES

#define SNR_POINTS 10  /*!< \brief Number of SNR evaluation points.*/
#define SNR_MIN (-2.0) /*!< \brief Min SNR [dB].*/
#define SNR_MAX 8.0    /*!< \brief Max SNR [dB].*/

#define BATCH_SIZE 10    /*!< \brief Number of codewords in a batch. */
#define MAX_N_BATCH 1000 /*!< \brief Max number of simulated batches. */
#define REQ_ERRORS 10    /*!< \brief Minimum number of errors for a significant simulation. */

// default values
static uint16_t K            = 128; /*!< \brief Number of message bits (data and CRC). */
static uint16_t E            = 256; /*!< \brief Number of bits of the codeword after rate matching. */
static uint8_t  nMax         = 9;   /*!< \brief Maximum \f$log_2(N)\f$, where \f$N\f$ is the codeword size.*/
static uint8_t  bil          = 0;   /*!< \brief If bil = 0 channel interleaver disabled. */
static double   snr_db       = 3;   /*!< \brief SNR in dB (101 for no noise, 100 for scan). */
static int      print_output = 0;   /*!< \brief print output form (0 for detailed, 1 for one line, 2 for vector). */

/*!
 * \brief Prints test help when a wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-nX] [-kX] [-eX] [-iX] [-sX] [-oX]\n", prog);
  printf("\t-n nMax [Default %d]\n", nMax);
  printf("\t-k Message size [Default %d]\n", K);
  printf("\t-e Rate matching size [Default %d]\n", E);
  printf("\t-i Bit interleaver indicator [Default %d]\n", bil);
  printf("\t-s SNR [dB, Default %.2f dB] -- Use 100 for scan, and 101 for noiseless\n", snr_db);
  printf("\t-o Print output results [Default %d] -- Use 0 for detailed, Use 1 for 1 line, Use 2 for vector form\n",
         print_output);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:k:e:i:s:o:")) != -1) {
    //  printf("opt : %d\n", opt);
    switch (opt) {
      case 'e':
        E = (int)strtol(optarg, NULL, 10);
        break;
      case 'k':
        K = (int)strtol(optarg, NULL, 10);
        break;
      case 'n':
        nMax = (int)strtol(optarg, NULL, 10);
        break;
      case 'i':
        bil = (int)strtol(optarg, NULL, 10);
        break;
      case 's':
        snr_db = strtof(optarg, NULL);
        break;
      case 'o':
        print_output = (int)strtol(optarg, NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*!
 * \brief Main function.
 */
int main(int argc, char** argv)
{
  uint8_t* data_tx        = NULL;
  uint8_t* data_rx        = NULL;
  uint8_t* data_rx_s      = NULL;
  uint8_t* data_rx_c      = NULL;
  uint8_t* data_rx_c_avx2 = NULL;

  uint8_t* input_enc       = NULL; // input encoder
  uint8_t* output_enc      = NULL; // output encoder
  uint8_t* output_enc_avx2 = NULL; // output encoder

  uint8_t* rm_codeword = NULL; // output rate-matcher

  float*   rm_llr        = NULL; // rate-matched llr
  int16_t* rm_llr_s      = NULL; // rate-matched llr
  int8_t*  rm_llr_c      = NULL; // rate-matched llr
  int8_t*  rm_llr_c_avx2 = NULL; // rate-matched llr

  float*   llr        = NULL; // input decoder
  int16_t* llr_s      = NULL; // input decoder
  int8_t*  llr_c      = NULL; // input decoder
  int8_t*  llr_c_avx2 = NULL; // input decoder

  uint8_t* output_dec        = NULL; // output decoder
  uint8_t* output_dec_s      = NULL; // output decoder
  uint8_t* output_dec_c      = NULL; // output decoder
  uint8_t* output_dec_c_avx2 = NULL; // output decoder

  double var[SNR_POINTS + 1];

  double snr_db_vec[SNR_POINTS + 1];

  int reportinfo = 0;

  int j          = 0;
  int snr_points = 0;

  int errors_symb   = 0;
  int errors_symb_s = 0;
  int errors_symb_c = 0;
#ifdef LV_HAVE_AVX2
  int errors_symb_c_avx2 = 0;
#endif

  int n_error_words[SNR_POINTS + 1];
  int n_error_words_s[SNR_POINTS + 1];
  int n_error_words_c[SNR_POINTS + 1];
  int n_error_words_c_avx2[SNR_POINTS + 1];

  int last_i_batch[SNR_POINTS + 1];

  struct timeval t[3];
  double         elapsed_time_dec[SNR_POINTS + 1];
  double         elapsed_time_dec_s[SNR_POINTS + 1];
  double         elapsed_time_dec_c[SNR_POINTS + 1];
  double         elapsed_time_dec_c_avx2[SNR_POINTS + 1];

  double elapsed_time_enc[SNR_POINTS + 1];
  double elapsed_time_enc_avx2[SNR_POINTS + 1];

  // 16-bit quantizer
  int16_t inf16  = (1U << 15U) - 1;
  int8_t  inf8   = (1U << 7U) - 1;
  float   gain_s = NAN;
  float   gain_c = NAN;
#ifdef LV_HAVE_AVX2
  float gain_c_avx2 = NAN;
#endif

  srsran_polar_code_t    code;
  srsran_polar_encoder_t enc;
  srsran_polar_decoder_t dec;
  srsran_polar_decoder_t dec_s; // 16-bit
  srsran_polar_decoder_t dec_c; // 8-bit
  srsran_polar_rm_t      rm_tx;
  srsran_polar_rm_t      rm_rx_f;
  srsran_polar_rm_t      rm_rx_s;
  srsran_polar_rm_t      rm_rx_c;

#ifdef LV_HAVE_AVX2
  srsran_polar_encoder_t enc_avx2;
  srsran_polar_decoder_t dec_c_avx2; // 8-bit
#endif                               // LV_HAVE_AVX2

  parse_args(argc, argv);

  // uinitialize polar code
  srsran_polar_code_init(&code);

  // initialize encoder pipeline
  srsran_polar_encoder_init(&enc, SRSRAN_POLAR_ENCODER_PIPELINED, nMax);

  // initialize rate-matcher
  srsran_polar_rm_tx_init(&rm_tx);

  // initialize rate-matcher
  srsran_polar_rm_rx_init_f(&rm_rx_f);

  // initialize rate-matcher
  srsran_polar_rm_rx_init_s(&rm_rx_s);

  // initialize rate-matcher
  srsran_polar_rm_rx_init_c(&rm_rx_c);

  // initialize a POLAR decoder (float)
  srsran_polar_decoder_init(&dec, SRSRAN_POLAR_DECODER_SSC_F, nMax);

  // initialize a POLAR decoder (16 bit)
  srsran_polar_decoder_init(&dec_s, SRSRAN_POLAR_DECODER_SSC_S, nMax);

  // initialize a POLAR decoder (8 bit)
  srsran_polar_decoder_init(&dec_c, SRSRAN_POLAR_DECODER_SSC_C, nMax);

#ifdef LV_HAVE_AVX2

  // initialize encoder  avx2
  srsran_polar_encoder_init(&enc_avx2, SRSRAN_POLAR_ENCODER_AVX2, nMax);

  // initialize a POLAR decoder (8 bit, avx2)
  srsran_polar_decoder_init(&dec_c_avx2, SRSRAN_POLAR_DECODER_SSC_C_AVX2, nMax);
#endif // LV_HAVE_AVX2

#ifdef DATA_ALL_ONES
#else
  srsran_random_t random_gen = srsran_random_init(0);
#endif

  data_tx        = srsran_vec_u8_malloc(K * BATCH_SIZE);
  data_rx        = srsran_vec_u8_malloc(K * BATCH_SIZE);
  data_rx_s      = srsran_vec_u8_malloc(K * BATCH_SIZE);
  data_rx_c      = srsran_vec_u8_malloc(K * BATCH_SIZE);
  data_rx_c_avx2 = srsran_vec_u8_malloc(K * BATCH_SIZE);

  input_enc       = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);
  output_enc      = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);
  output_enc_avx2 = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);

  rm_codeword = srsran_vec_u8_malloc(E * BATCH_SIZE);

  rm_llr        = srsran_vec_f_malloc(E * BATCH_SIZE);
  rm_llr_s      = srsran_vec_i16_malloc(E * BATCH_SIZE);
  rm_llr_c      = srsran_vec_i8_malloc(E * BATCH_SIZE);
  rm_llr_c_avx2 = srsran_vec_i8_malloc(E * BATCH_SIZE);

  llr        = srsran_vec_f_malloc(NMAX * BATCH_SIZE);
  llr_s      = srsran_vec_i16_malloc(NMAX * BATCH_SIZE);
  llr_c      = srsran_vec_i8_malloc(NMAX * BATCH_SIZE);
  llr_c_avx2 = srsran_vec_i8_malloc(NMAX * BATCH_SIZE);

  output_dec        = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);
  output_dec_s      = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);
  output_dec_c      = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);
  output_dec_c_avx2 = srsran_vec_u8_malloc(NMAX * BATCH_SIZE);

  if (!data_tx || !data_rx || !data_rx_s || !data_rx_c || !data_rx_c_avx2 || !input_enc || !output_enc ||
      !output_enc_avx2 || !rm_codeword || !rm_llr || !rm_llr_s || !rm_llr_c || !rm_llr_c_avx2 || !llr || !llr_s ||
      !llr_c || !llr_c_avx2 || !output_dec || !output_dec_s || !output_dec_c || !output_dec_c_avx2) {
    perror("malloc");
    exit(-1);
  }

  // if snr_db = 100 compute a rage from SNR_MIN to SNR_MAX with SNR_POINTS
  // else use the specified SNR.
  double snr_inc = NAN;

  snr_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;

  if (snr_db == 100.0) {
    snr_points = SNR_POINTS;
    for (int i = 0; i < snr_points; i++) {
      snr_db        = SNR_MIN + i * snr_inc;
      snr_db_vec[i] = snr_db;
      var[i]        = srsran_convert_dB_to_power(-snr_db);
    }
    snr_db_vec[snr_points] = 101; // include the no noise case
    snr_points++;
  } else {
    snr_db_vec[0] = snr_db;
    var[0]        = srsran_convert_dB_to_power(-snr_db);
    snr_points    = 1;
  }

  if (snr_db == 100) { // scan
    printf("  SNR_MIN = %f, SNR_INC = %f, SNR_MAX = %f, snr_points: %d\n",
           SNR_MIN,
           snr_inc,
           SNR_MIN + snr_inc * snr_points,
           snr_points);
  }

  for (int32_t i_snr = 0; i_snr < snr_points; i_snr++) {
    if (snr_db_vec[i_snr] == 101) {
      printf("\n  Signal-to-Noise Ratio -> infinite\n");
    } else {
      printf("\n  Signal-to-Noise Ratio -> %.1f dB\n", snr_db_vec[i_snr]);
    }

    elapsed_time_enc[i_snr]        = 0;
    elapsed_time_enc_avx2[i_snr]   = 0;
    elapsed_time_dec[i_snr]        = 0;
    elapsed_time_dec_s[i_snr]      = 0;
    elapsed_time_dec_c[i_snr]      = 0;
    elapsed_time_dec_c_avx2[i_snr] = 0;

    n_error_words[i_snr]        = 0;
    n_error_words_s[i_snr]      = 0;
    n_error_words_c[i_snr]      = 0;
    n_error_words_c_avx2[i_snr] = 0;

    int i_batch = 0;
    printf("\nBatch:\n  ");

    int req_errors  = 0;
    int max_n_batch = 0;
    if (snr_db_vec[i_snr] == 101) {
      req_errors  = 1;
      max_n_batch = 1;
    } else {
      req_errors  = REQ_ERRORS;
      max_n_batch = MAX_N_BATCH;
    }

    while ((n_error_words[i_snr] < req_errors) && (i_batch < max_n_batch)) {
      i_batch++;

      if (!(i_batch % 10)) {
        printf("%8d", i_batch);
        if (!(i_batch % 90)) {
          printf("\n  ");
        }
      }

// generate data_tx
#ifdef DATA_ALL_ONES
      for (i = 0; i < BATCH_SIZE; i++) {
        for (j = 0; j < K; j++) {
          data_tx[i * K + j] = 1;
        }
      }

#else
      for (int i = 0; i < BATCH_SIZE; i++) {
        for (j = 0; j < K; j++) {
          data_tx[i * K + j] = srsran_random_uniform_int_dist(random_gen, 0, 1);
        }
      }
#endif

      // get polar code, compute frozen_set (F_set), message_set (K_set) and parity bit set (PC_set)
      if (srsran_polar_code_get(&code, K, E, nMax) == -1) {
        return -1;
      }

      if (reportinfo == 0) {
        reportinfo = 1;
        printf("Test POLAR chain:\n");
        printf("  Final code bits    -> E  = %d\n", E);
        printf("  Code bits          -> N  = %d\n", code.N);
        printf("  CRC + Data bits    -> K  = %d\n", K);
        printf("  Parity Check bits  -> PC = %d \n", code.nPC);
        printf("  Code rate          -> (K + PC)/N = (%d + %d)/%d = %.2f\n",
               K,
               code.nPC,
               code.N,
               (double)(K + code.nPC) / code.N);
      }
      // subchannel_allocation block
      for (int i = 0; i < BATCH_SIZE; i++) {
        srsran_polar_chanalloc_tx(
            data_tx + i * K, input_enc + i * code.N, code.N, code.K, code.nPC, code.K_set, code.PC_set);
      }

      // encoding pipeline
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_encoder_encode(&enc, input_enc + j * code.N, output_enc + j * code.N, code.n);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);

      elapsed_time_enc[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // rate matcher
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_rm_tx(&rm_tx, output_enc + j * code.N, rm_codeword + j * E, code.n, E, K, bil);
      }

#ifdef LV_HAVE_AVX2
      // encoding  avx2
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_encoder_encode(&enc_avx2, input_enc + j * code.N, output_enc_avx2 + j * code.N, code.n);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);

      elapsed_time_enc_avx2[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // check errors with respect the output of the pipeline encoder
      for (int i = 0; i < BATCH_SIZE; i++) {
        if (srsran_bit_diff(output_enc + i * code.N, output_enc_avx2 + i * code.N, code.N) != 0) {
          printf("ERROR: Wrong avx2 encoder output. SNR= %f, Batch: %d\n", snr_db_vec[i_snr], i);
          exit(-1);
        }
      }
#endif // LV_HAVE_AVX2

      for (j = 0; j < E * BATCH_SIZE; j++) {
        rm_llr[j] = rm_codeword[j] ? -1 : 1;
      }

      // add noise
      if (snr_db_vec[i_snr] != 101) {
        srsran_ch_awgn_f(rm_llr, rm_llr, var[i_snr], BATCH_SIZE * E);

        // Convert symbols into LLRs
        for (j = 0; j < BATCH_SIZE * code.N; j++) {
          rm_llr[j] *= 2 / (var[i_snr] * var[i_snr]);
        }
      }

      // rate-Dematcher

      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_rm_rx_f(&rm_rx_f, rm_llr + j * E, llr + j * code.N, E, code.n, K, bil);
      }

      // decoding float point
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_decoder_decode_f(
            &dec, llr + j * code.N, output_dec + j * code.N, code.n, code.F_set, code.F_set_size);
      }

      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits - float decoder
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_chanalloc_rx(output_dec + j * code.N, data_rx + j * K, code.K, code.nPC, code.K_set, code.PC_set);
      }

// check errors - float decpder
#ifdef debug
      int i_error = 0;
#endif
      for (int i = 0; i < BATCH_SIZE; i++) {
        errors_symb = srsran_bit_diff(data_tx + i * K, data_rx + i * K, K);

        if (errors_symb != 0) {
          n_error_words[i_snr]++;
        }
      }

      // decoding 16-bit
      // 16-quantization
      if (snr_db_vec[i_snr] == 101) {
        srsran_vec_quant_fs(rm_llr, rm_llr_s, 8192, 0, 32767, BATCH_SIZE * E);
      } else {
        gain_s = inf16 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        // printf("gain_s: %f, inf16:%d\n", gain_s, inf16);
        srsran_vec_quant_fs(rm_llr, rm_llr_s, gain_s, 0, inf16, BATCH_SIZE * E);
      }

      // Rate dematcher
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_rm_rx_s(&rm_rx_s, rm_llr_s + j * E, llr_s + j * code.N, E, code.n, K, bil);
      }

      // decoding 16-bit
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_decoder_decode_s(
            &dec_s, llr_s + j * code.N, output_dec_s + j * code.N, code.n, code.F_set, code.F_set_size);
      }

      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_s[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits  16-bit decoder
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_chanalloc_rx(
            output_dec_s + j * code.N, data_rx_s + j * K, code.K, code.nPC, code.K_set, code.PC_set);
      }

      // check errors 16-bit decoder
      for (int i = 0; i < BATCH_SIZE; i++) {
        errors_symb_s = srsran_bit_diff(data_tx + i * K, data_rx_s + i * K, K);

        if (errors_symb_s != 0) {
          n_error_words_s[i_snr]++;
        }
      }

      // 8-bit decoding
      // 8-bit quantization
      if (snr_db_vec[i_snr] == 101) {
        srsran_vec_quant_fc(rm_llr, rm_llr_c, 32, 0, 127, BATCH_SIZE * E);
      } else {
        gain_c = inf8 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        srsran_vec_quant_fc(rm_llr, rm_llr_c, gain_c, 0, inf8, BATCH_SIZE * E);
      }

      // Rate dematcher
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_rm_rx_c(&rm_rx_c, rm_llr_c + j * E, llr_c + j * code.N, E, code.n, K, bil);
      }

      // Decoding
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_decoder_decode_c(
            &dec_c, llr_c + j * code.N, output_dec_c + j * code.N, code.n, code.F_set, code.F_set_size);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_c[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_chanalloc_rx(
            output_dec_c + j * code.N, data_rx_c + j * K, code.K, code.nPC, code.K_set, code.PC_set);
      }

      // check errors 8-bits decoder
      for (int i = 0; i < BATCH_SIZE; i++) {
        errors_symb_c = srsran_bit_diff(data_tx + i * K, data_rx_c + i * K, K);

        if (errors_symb_c != 0) {
          n_error_words_c[i_snr]++;
        }
      }

#ifdef LV_HAVE_AVX2
      // 8-bit avx2 decoding
      // 8-bit quantization
      if (snr_db_vec[i_snr] == 101) {
        srsran_vec_quant_fc(rm_llr, rm_llr_c_avx2, 32, 0, 127, BATCH_SIZE * E);
      } else {
        gain_c_avx2 = inf8 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        srsran_vec_quant_fc(rm_llr, rm_llr_c_avx2, gain_c_avx2, 0, inf8, BATCH_SIZE * E);
      }

      // Rate dematcher
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_rm_rx_c(&rm_rx_c, rm_llr_c_avx2 + j * E, llr_c_avx2 + j * code.N, E, code.n, K, bil);
      }

      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_decoder_decode_c(
            &dec_c_avx2, llr_c_avx2 + j * code.N, output_dec_c_avx2 + j * code.N, code.n, code.F_set, code.F_set_size);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_c_avx2[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits
      for (j = 0; j < BATCH_SIZE; j++) {
        srsran_polar_chanalloc_rx(
            output_dec_c_avx2 + j * code.N, data_rx_c_avx2 + j * K, code.K, code.nPC, code.K_set, code.PC_set);
      }

      // check errors 8-bits decoder
      for (int i = 0; i < BATCH_SIZE; i++) {
        errors_symb_c_avx2 = srsran_bit_diff(data_tx + i * K, data_rx_c_avx2 + i * K, K);

        if (errors_symb_c_avx2 != 0) {
          n_error_words_c_avx2[i_snr]++;
        }
      }
#endif // LV_HAVE_AVX2

      last_i_batch[i_snr] = i_batch;
    } // end while BATCH

  } // snr_db

  printf("\n");
  switch (print_output) {
    case 2:

      printf("SNR=[");
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("%3.1f ", snr_db_vec[i_snr] - 3);
      }
      printf("];\n");
      printf("WER=[");
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("%e ", (float)n_error_words[i_snr] / last_i_batch[i_snr] / BATCH_SIZE);
      }
      printf("];\n");

      printf("WER_16=[");
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("%e ", (float)n_error_words_s[i_snr] / last_i_batch[i_snr] / BATCH_SIZE);
      }
      printf("];\n");

      printf("WER_8=[");
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("%e ", (float)n_error_words_c[i_snr] / last_i_batch[i_snr] / BATCH_SIZE);
      }
      printf("];\n");

#ifdef LV_HAVE_AVX2
      printf("WER_8_AVX2=[");
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("%e ", (float)n_error_words_c_avx2[i_snr] / last_i_batch[i_snr] / BATCH_SIZE);
      }
      printf("];\n");
#endif // LV_HAVE_AVX2
      break;
    case 1:
      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("SNR: %3.1f\t enc_pipe_thrpt(Mbps):  %.2f\t  enc_avx2_thrpt(Mbps):  "
               "%.2f\n",
               snr_db_vec[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_enc[i_snr]),
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_enc_avx2[i_snr]));

        printf("SNR: %3.1f\t FLOAT WER: %.8f %d/%d \t  dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N,
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_dec[i_snr]));
        printf("SNR: %3.1f\t INT16 WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_s[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N,
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_dec_s[i_snr]));
        printf("SNR: %3.1f\t INT8  WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_c[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N,
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_dec_c[i_snr]));
#ifdef LV_HAVE_AVX2
        printf("SNR: %3.1f\t INT8-AVX2  WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_c_avx2[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N,
               last_i_batch[i_snr] * BATCH_SIZE * code.N / (1000000 * elapsed_time_dec_c_avx2[i_snr]));
#endif // LV_HAVE_AVX2
        printf("\n");
      }

      break;
    default:

      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("**** PIPELINE  ENCODER ****\n");
        printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_enc[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_enc[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_enc[i_snr]);

#ifdef LV_HAVE_AVX2
        printf("\n**** AVX2 ENCODER ****\n");
        printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s "
               "(encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_enc_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_enc_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_enc_avx2[i_snr]);
#endif // LV_HAVE_AVX2

        printf("\n**** FLOATING POINT ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_dec[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_dec[i_snr]);

        printf("\n**** FIXED POINT (16 bits) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_s[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_s[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_dec_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_dec_s[i_snr]);

        printf("\n**** FIXED POINT (8 bits) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_c[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_dec_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_dec_c[i_snr]);

#ifdef LV_HAVE_AVX2
        printf("\n**** FIXED POINT (8 bits, AVX2) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_c_avx2[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c_avx2[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * K / elapsed_time_dec_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code.N / elapsed_time_dec_c_avx2[i_snr]);
#endif // LV_HAVE_AVX2

        printf("\n");
      }
      break;
  }
  free(data_tx);
  free(data_rx);
  free(data_rx_s);
  free(data_rx_c);

  free(input_enc);
  free(output_enc);

  free(rm_codeword);

  free(rm_llr);
  free(rm_llr_s);
  free(rm_llr_c);
  free(rm_llr_c_avx2);

  free(llr);
  free(llr_s);

  free(llr_c);
  free(llr_c_avx2);

  free(output_dec);
  free(output_dec_s);
  free(output_dec_c);

  free(output_dec_c_avx2);
  free(output_enc_avx2);
  free(data_rx_c_avx2);

#ifdef DATA_ALL_ONES
#else
  srsran_random_free(random_gen);
#endif
  // free code
  srsran_polar_code_free(&code);
  srsran_polar_encoder_free(&enc);
  srsran_polar_decoder_free(&dec);
  srsran_polar_decoder_free(&dec_s);
  srsran_polar_decoder_free(&dec_c);
  srsran_polar_rm_rx_free_f(&rm_rx_f);
  srsran_polar_rm_rx_free_s(&rm_rx_s);
  srsran_polar_rm_rx_free_c(&rm_rx_c);
  srsran_polar_rm_tx_free(&rm_tx);
#ifdef LV_HAVE_AVX2
  srsran_polar_encoder_free(&enc_avx2);
  srsran_polar_decoder_free(&dec_c_avx2);
#endif // LV_HAVE_AVX2

  int expected_errors = 0;
  int i_snr           = 0;
  if (snr_db_vec[i_snr] == 101) {
    if (n_error_words[0] > expected_errors) {
      printf("\n(float) Test failed!\n\n");
    } else {
      printf("\n(float) Test completed successfully!\n\n");
    }
    printf("\r");

    if (n_error_words_s[0] > expected_errors) {
      printf("\n(16 bit) Test failed!\n\n");
    } else {
      printf("\n(16 bit) Test completed successfully!\n\n");
    }
    printf("\r");

    if (n_error_words_c[0] > expected_errors) {
      printf("\n(8 bit) Test failed!\n\n");
    } else {
      printf("\n(8 bit) Test completed successfully!\n\n");
    }
    printf("\r");

#ifdef LV_HAVE_AVX2
    if (n_error_words_c_avx2[0] > expected_errors) {
      printf("\n(8 bit, avx2) Test failed!\n\n");
    } else {
      printf("\n(8 bit, avx2) Test completed successfully!\n\n");
    }
#endif // LV_HAVE_AVX2
    printf("\r");

    exit((n_error_words[0] > expected_errors) || (n_error_words_s[0] > expected_errors) ||
         (n_error_words_c[0] > expected_errors)
#ifdef LV_HAVE_AVX2
         || (n_error_words_c_avx2[0] > expected_errors)
#endif // LV_HAVE_AVX2
    );

  } else {
    for (int i_snr = 0; i_snr < snr_points; i_snr++) {
      if (n_error_words_s[i_snr] > 10 * n_error_words[i_snr]) {
        perror("16-bit performance at SNR = %d too low!");
        exit(-1);
      }
      if (n_error_words_c[i_snr] > 10 * n_error_words[i_snr]) {
        perror("8-bit performance at SNR = %d too low!");
        exit(-1);
      }
#ifdef LV_HAVE_AVX2
      if (n_error_words_c_avx2[i_snr] > 10 * n_error_words[i_snr]) {
        perror("8-bit avx2 performance at SNR = %d too low!");
        exit(-1);
      }
#endif // LV_HAVE_AVX2
    }

    printf("\nTest completed successfully!\n\n");
    printf("\r");
  }
}
