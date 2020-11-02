/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

/*!
 * \file polar_chain_test.c
 * \brief Throughput and WER tests for the polar encoder/decoder.
 *
 * Synopsis: **polar_test [options]**
 *
 * Options:
 *
 *  - <b>-c \<number\></b> \f$log_2\f$ of the codeword length [Default 8]
 *
 *  - <b>-r \<number\></b> Rate matching size [Default 256]
 *
 *  - <b>-m \<number\></b> Message size [Default 128]
 *
 *  - <b>-p \<number\></b> Parity-set size [Default 0]
 *
 *  - <b>-w \<number\></b> nWmPC [Default 0]
 *
 *  - <b>-s \<number\></b>  SNR [dB, Default 3.00 dB] -- Use 100 for scan, and 101 for noiseless
 *
 *  - <b>-o \<number\></b>  Print output results [Default 0] -- Use 0 for detailed, Use 1 for 1 line, Use 2 for vector
 * form
 *
 * It (1) generates a random set of bits (data); (2) passes the data bits
 * through the subchannel allocation block where the input vector to the
 * encoder is generated; (3) encodes the input vector; (4) adds Gaussian channel noise
 * (optional); (5) passes the decoder output through the subchannel
 * deallocation block where data bits are extracted; (6) compares the decoded
 * bits with the original data bits and measures the throughput (in bit / s).
 *
 * The message, frozen and parity bit sets corresponding to the input
 * parameters -c, -r, -m, -p, -w must be available in the subfolder \a
 * frozensets of the execution directory.
 * These sets are stored in files with the following name convention:
 * >  polar_code_<code_size>_<rate_matching_size>_<message_size>_<parity_set_size>_<wmPC>.bin
 *
 * See \ref polar for futher details.
 *
 */

#include "math.h"

#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/phy_logger.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h" // srslte_convert_dB_to_amplitude

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// cttc utils lib
#include "srslte/phy/utils/vector.h"

//  polar libs
#include "polar_sets.h"
#include "srslte/phy/fec/polar/polar_decoder.h"
#include "srslte/phy/fec/polar/polar_encoder.h"
#include "subchannel_allocation.h"

#define SNR_POINTS 10  /*!< \brief Number of SNR evaluation points.*/
#define SNR_MIN (-2.0) /*!< \brief Min SNR [dB].*/
#define SNR_MAX 8.0    /*!< \brief Max SNR [dB].*/

#define BATCH_SIZE 100    /*!< \brief Number of codewords in a batch. */
#define MAX_N_BATCH 10000 /*!< \brief Max number of simulated batches. */
#define REQ_ERRORS 100    /*!< \brief Minimum number of errors for a significant simulation. */

// default values
uint8_t  code_size_log      = 8;   /*!< \brief \f$log_2\f$ of code size. */
uint16_t message_size       = 128; /*!< \brief Number of message bits (data and CRC). */
uint16_t rate_matching_size = 256; /*!< \brief Number of bits of the codeword after rate matching. */
uint8_t  parity_set_size    = 0;   /*!< \brief Number of parity bits. */
uint8_t  nWmPC              = 0;   /*!< \brief Number of parity bits of minimum weight type. */
double   snr_db             = 3;   /*!< \brief SNR in dB (101 for no noise, 100 for scan). */
int      print_output       = 0;   /*!< \brief print output form (0 for detailed, 1 for 1 line, 2 for vector). */

/*!
 * \brief Prints test help when a wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-cX] [-rX] [-mX] [-pX] [-wX] [-sX]\n", prog);
  printf("\t-c log2 of the codeword length [Default %d]\n", code_size_log);
  printf("\t-r Rate matching size [Default %d]\n", rate_matching_size);
  printf("\t-m Message size [Default %d]\n", message_size);
  printf("\t-p Parity-set size [Default %d]\n", parity_set_size);
  printf("\t-w nWmPC [Default %d]\n", nWmPC);
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
  while ((opt = getopt(argc, argv, "c:r:m:p:w:e:s:t:o:")) != -1) {
    switch (opt) {
      case 'c':
        code_size_log = (int)strtol(optarg, NULL, 10);
        break;
      case 'r':
        rate_matching_size = (int)strtol(optarg, NULL, 10);
        break;
      case 'm':
        message_size = (int)strtol(optarg, NULL, 10);
        break;
      case 'p':
        parity_set_size = (int)strtol(optarg, NULL, 10);
        break;
      case 'w':
        nWmPC = (int)strtol(optarg, NULL, 10);
        break;
      case 's':
        snr_db = strtof(optarg, NULL);
        break;
      case 'o':
        print_output = strtol(optarg, NULL, 10);
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
  int    i = 0;

  int j          = 0;
  int snr_points = 0;

  int errors_symb        = 0;
  int errors_symb_s      = 0;
  int errors_symb_c      = 0;
  int errors_symb_c_avx2 = 0;

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
  int16_t inf16       = (1U << 15U) - 1;
  int8_t  inf8        = (1U << 7U) - 1;
  float   gain_s      = NAN;
  float   gain_c      = NAN;
  float   gain_c_avx2 = NAN;

  srslte_polar_sets_t    sets;
  srslte_subchn_alloc_t  subch;
  srslte_polar_encoder_t enc;
  srslte_polar_decoder_t dec;
  srslte_polar_decoder_t dec_s; // 16-bit
  srslte_polar_decoder_t dec_c; // 8-bit
#ifdef LV_HAVE_AVX2
  srslte_polar_encoder_t enc_avx2;
  srslte_polar_decoder_t dec_c_avx2; // 8-bit
#endif                               // LV_HAVE_AVX2

  parse_args(argc, argv);

  uint16_t code_size = 1U << code_size_log;

  printf("Test POLAR chain:\n");
  printf("  Final code bits    -> E  = %d\n", rate_matching_size);
  printf("  Code bits          -> N  = %d\n", code_size);
  printf("  CRC + Data bits    -> K  = %d\n", message_size);
  printf("  Parity Check bits  -> PC = %d \n", parity_set_size);
  printf("  Code rate          -> (K + PC)/N = (%d + %d)/%d = %.2f\n",
         message_size,
         parity_set_size,
         code_size,
         (double)(message_size + parity_set_size) / code_size);

  // read polar index sets from a file
  srslte_polar_code_sets_read(&sets, message_size, code_size_log, rate_matching_size, parity_set_size, nWmPC);

  // subchannel allocation
  srslte_subchannel_allocation_init(&subch, code_size_log, message_size, sets.message_set);

  // initialize encoder pipeline
  srslte_polar_encoder_init(&enc, SRSLTE_POLAR_ENCODER_PIPELINED, code_size_log);

  // initialize a POLAR decoder (float)
  srslte_polar_decoder_init(&dec, SRSLTE_POLAR_DECODER_SSC_F, code_size_log, sets.frozen_set, sets.frozen_set_size);

  // initialize a POLAR decoder (16 bit)
  srslte_polar_decoder_init(&dec_s, SRSLTE_POLAR_DECODER_SSC_S, code_size_log, sets.frozen_set, sets.frozen_set_size);

  // initialize a POLAR decoder (8 bit)
  srslte_polar_decoder_init(&dec_c, SRSLTE_POLAR_DECODER_SSC_C, code_size_log, sets.frozen_set, sets.frozen_set_size);

#ifdef LV_HAVE_AVX2

  // initialize encoder  avx2
  srslte_polar_encoder_init(&enc_avx2, SRSLTE_POLAR_ENCODER_AVX2, code_size_log);

  // initialize a POLAR decoder (8 bit, avx2)
  srslte_polar_decoder_init(
      &dec_c_avx2, SRSLTE_POLAR_DECODER_SSC_C_AVX2, code_size_log, sets.frozen_set, sets.frozen_set_size);
#endif // LV_HAVE_AVX2

#ifdef DATA_ALL_ONES
#else
  srslte_random_t random_gen = srslte_random_init(0);
#endif

  data_tx        = srslte_vec_u8_malloc(message_size * BATCH_SIZE);
  data_rx        = srslte_vec_u8_malloc(message_size * BATCH_SIZE);
  data_rx_s      = srslte_vec_u8_malloc(message_size * BATCH_SIZE);
  data_rx_c      = srslte_vec_u8_malloc(message_size * BATCH_SIZE);
  data_rx_c_avx2 = srslte_vec_u8_malloc(message_size * BATCH_SIZE);

  input_enc       = srslte_vec_u8_malloc(code_size * BATCH_SIZE);
  output_enc      = srslte_vec_u8_malloc(code_size * BATCH_SIZE);
  output_enc_avx2 = srslte_vec_u8_malloc(code_size * BATCH_SIZE);

  llr        = srslte_vec_f_malloc(code_size * BATCH_SIZE);
  llr_s      = srslte_vec_i16_malloc(code_size * BATCH_SIZE);
  llr_c      = srslte_vec_i8_malloc(code_size * BATCH_SIZE);
  llr_c_avx2 = srslte_vec_i8_malloc(code_size * BATCH_SIZE);

  output_dec        = srslte_vec_u8_malloc(code_size * BATCH_SIZE);
  output_dec_s      = srslte_vec_u8_malloc(code_size * BATCH_SIZE);
  output_dec_c      = srslte_vec_u8_malloc(code_size * BATCH_SIZE);
  output_dec_c_avx2 = srslte_vec_u8_malloc(code_size * BATCH_SIZE);

  if (!data_tx || !data_rx || !data_rx_s || !data_rx_c || !data_rx_c_avx2 || !input_enc || !output_enc ||
      !output_enc_avx2 || !llr || !llr_s || !llr_c || !llr_c_avx2 || !output_dec || !output_dec_s || !output_dec_c ||
      !output_dec_c_avx2) {
    perror("malloc");
    exit(-1);
  }

  // if snr_db = 100 compute a rage from SNR_MIN to SNR_MAX with SNR_POINTS
  // else use the specified SNR.
  double snr_inc = NAN;

  snr_inc = (SNR_MAX - SNR_MIN) / SNR_POINTS;

  if (snr_db == 100.0) {
    snr_points = SNR_POINTS;
    for (int32_t i = 0; i < snr_points; i++) {
      snr_db        = SNR_MIN + i * snr_inc;
      snr_db_vec[i] = snr_db;
      var[i]        = srslte_convert_dB_to_amplitude(-snr_db);
    }
    snr_db_vec[snr_points] = 101; // include the no noise case
    snr_points++;
  } else {
    snr_db_vec[0] = snr_db;
    var[0]        = srslte_convert_dB_to_amplitude(-snr_db);
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
        for (j = 0; j < message_size; j++) {
          data_tx[i * message_size + j] = 1;
        }
      }

#else
      for (i = 0; i < BATCH_SIZE; i++) {
        for (j = 0; j < message_size; j++) {
          data_tx[i * message_size + j] = srslte_random_uniform_int_dist(random_gen, 0, 1);
        }
      }
#endif

      // subchannel_allocation block
      for (i = 0; i < BATCH_SIZE; i++) {
        srslte_subchannel_allocation(&subch, data_tx + i * message_size, input_enc + i * code_size);
      }

      // encoding pipeline
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_encoder_encode(&enc, input_enc + j * code_size, output_enc + j * code_size, code_size_log);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);

      elapsed_time_enc[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

#ifdef LV_HAVE_AVX2
      // encoding  avx2
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_encoder_encode(
            &enc_avx2, input_enc + j * code_size, output_enc_avx2 + j * code_size, code_size_log);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);

      elapsed_time_enc_avx2[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // check encoders have the same output.

      // check errors with respect the output of the pipeline encoder
      for (i = 0; i < BATCH_SIZE; i++) {
        if (srslte_bit_diff(output_enc + i * code_size, output_enc_avx2 + i * code_size, code_size) != 0) {
          printf("ERROR: Wrong avx2 encoder output. SNR= %f, Batch: %d\n", snr_db_vec[i_snr], i);
          exit(-1);
        }
      }
#endif // LV_HAVE_AVX2

      for (j = 0; j < code_size * BATCH_SIZE; j++) {
        llr[j] = output_enc[j] ? -1 : 1;
      }

      // add noise
      if (snr_db_vec[i_snr] != 101) {
        srslte_ch_awgn_f(llr, llr, var[i_snr], BATCH_SIZE * code_size);

        // Convert symbols into LLRs
        for (j = 0; j < BATCH_SIZE * code_size; j++) {
          llr[j] *= 2 / (var[i_snr] * var[i_snr]);
        }
      }

      // decoding float point
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_decoder_decode_f(&dec, llr + j * code_size, output_dec + j * code_size);
      }

      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits - float decoder
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_subchannel_deallocation(&subch, output_dec + j * code_size, data_rx + j * message_size);
      }

      // check errors - float decpder
      for (i = 0; i < BATCH_SIZE; i++) {
        errors_symb = srslte_bit_diff(data_tx + i * message_size, data_rx + i * message_size, message_size);

        if (errors_symb != 0) {
          n_error_words[i_snr]++;
        }
      }

      // decoding 16-bit
      // 16-quantization
      if (snr_db_vec[i_snr] == 101) {
        srslte_vec_quant_fs(llr, llr_s, 8192, 0, 32767, BATCH_SIZE * code_size);
      } else {
        gain_s = inf16 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        srslte_vec_quant_fs(llr, llr_s, gain_s, 0, inf16, BATCH_SIZE * code_size);
      }

      // decoding 16-bit
      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_decoder_decode_s(&dec_s, llr_s + j * code_size, output_dec_s + j * code_size);
      }

      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_s[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits  16-bit decoder
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_subchannel_deallocation(&subch, output_dec_s + j * code_size, data_rx_s + j * message_size);
      }

      // check errors 16-bit decoder
      for (i = 0; i < BATCH_SIZE; i++) {
        errors_symb_s = srslte_bit_diff(data_tx + i * message_size, data_rx_s + i * message_size, message_size);

        if (errors_symb_s != 0) {
          n_error_words_s[i_snr]++;
        }
      }

      // 8-bit decoding
      // 8-bit quantization
      if (snr_db_vec[i_snr] == 101) {
        srslte_vec_quant_fc(llr, llr_c, 32, 0, 127, BATCH_SIZE * code_size);
      } else {
        gain_c = inf8 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        srslte_vec_quant_fc(llr, llr_c, gain_c, 0, inf8, BATCH_SIZE * code_size);
      }

      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_decoder_decode_c(&dec_c, llr_c + j * code_size, output_dec_c + j * code_size);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_c[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_subchannel_deallocation(&subch, output_dec_c + j * code_size, data_rx_c + j * message_size);
      }

      // check errors 8-bits decoder
      for (i = 0; i < BATCH_SIZE; i++) {

        errors_symb_c = srslte_bit_diff(data_tx + i * message_size, data_rx_c + i * message_size, message_size);

        if (errors_symb_c != 0) {
          n_error_words_c[i_snr]++;
        }
      }

#ifdef LV_HAVE_AVX2
      // 8-bit avx2 decoding
      // 8-bit quantization
      if (snr_db_vec[i_snr] == 101) {
        srslte_vec_quant_fc(llr, llr_c_avx2, 32, 0, 127, BATCH_SIZE * code_size);
      } else {
        gain_c_avx2 = inf8 * var[i_snr] / 20 / (1 / var[i_snr] + 2);
        srslte_vec_quant_fc(llr, llr_c_avx2, gain_c_avx2, 0, inf8, BATCH_SIZE * code_size);
      }

      gettimeofday(&t[1], NULL);
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_polar_decoder_decode_c(&dec_c_avx2, llr_c_avx2 + j * code_size, output_dec_c_avx2 + j * code_size);
      }
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      elapsed_time_dec_c_avx2[i_snr] += t[0].tv_sec + 1e-6 * t[0].tv_usec;

      // extract message bits
      for (j = 0; j < BATCH_SIZE; j++) {
        srslte_subchannel_deallocation(&subch, output_dec_c_avx2 + j * code_size, data_rx_c_avx2 + j * message_size);
      }

      // check errors 8-bits decoder
      for (i = 0; i < BATCH_SIZE; i++) {

        errors_symb_c_avx2 =
            srslte_bit_diff(data_tx + i * message_size, data_rx_c_avx2 + i * message_size, message_size);

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
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_enc[i_snr]),
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_enc_avx2[i_snr]));

        printf("SNR: %3.1f\t FLOAT WER: %.8f %d/%d \t  dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size,
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_dec[i_snr]));
        printf("SNR: %3.1f\t INT16 WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_s[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size,
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_dec_s[i_snr]));
        printf("SNR: %3.1f\t INT8  WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_c[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size,
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_dec_c[i_snr]));
#ifdef LV_HAVE_AVX2
        printf("SNR: %3.1f\t INT8-AVX2  WER: %.8f %d/%d \t dec_thrput(Mbps): %.2f\n",
               snr_db_vec[i_snr],
               (double)n_error_words_c_avx2[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size,
               last_i_batch[i_snr] * BATCH_SIZE * code_size / (1000000 * elapsed_time_dec_c_avx2[i_snr]));
#endif // LV_HAVE_AVX2
        printf("\n");
      }

      break;
    default:

      for (int i_snr = 0; i_snr < snr_points; i_snr++) {
        printf("**** PIPELINE  ENCODER ****\n");
        printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_enc[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_enc[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_enc[i_snr]);

#ifdef LV_HAVE_AVX2
        printf("\n**** AVX2 ENCODER ****\n");
        printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s "
               "(encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_enc_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_enc_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_enc_avx2[i_snr]);
#endif // LV_HAVE_AVX2

        printf("\n**** FLOATING POINT ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_dec[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_dec[i_snr]);

        printf("\n**** FIXED POINT (16 bits) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_s[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_s[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_dec_s[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_dec_s[i_snr]);

        printf("\n**** FIXED POINT (8 bits) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_c[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_dec_c[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_dec_c[i_snr]);

#ifdef LV_HAVE_AVX2
        printf("\n**** FIXED POINT (8 bits, AVX2) ****");
        printf("\nEstimated word error rate:\n  %e (%d errors)\n",
               (double)n_error_words_c_avx2[i_snr] / last_i_batch[i_snr] / BATCH_SIZE,
               n_error_words_c_avx2[i_snr]);

        printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
               last_i_batch[i_snr] * BATCH_SIZE / elapsed_time_dec_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * message_size / elapsed_time_dec_c_avx2[i_snr],
               last_i_batch[i_snr] * BATCH_SIZE * code_size / elapsed_time_dec_c_avx2[i_snr]);
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
  free(output_enc_avx2);

  free(llr);
  free(llr_s);
  free(llr_c);

  free(output_dec);
  free(output_dec_s);
  free(output_dec_c);

#ifdef DATA_ALL_ONES
#else
  srslte_random_free(random_gen);
#endif
  // free sets
  srslte_polar_code_sets_free(&sets);
  srslte_polar_encoder_free(&enc);
  srslte_polar_decoder_free(&dec);
  srslte_polar_decoder_free(&dec_s);
  srslte_polar_decoder_free(&dec_c);

#ifdef LV_HAVE_AVX2
  srslte_polar_encoder_free(&enc_avx2);
  srslte_polar_decoder_free(&dec_c_avx2);
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
