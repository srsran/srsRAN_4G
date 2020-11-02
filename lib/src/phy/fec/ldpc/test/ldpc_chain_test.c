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
 * \file ldpc_chain_test.c
 * \brief End-to-end test for LDPC encoder and decoder.
 *
 * A batch of example messages is randomly generated, encoded, 2-PAM modulated,
 * sent over an AWGN channel and, finally, decoded by all three types of
 * decoder. Transmitted and received messages are compared to estimate the WER.
 * Multiple batches are simulated if the number of errors is not significant
 * enough.
 *
 * A fixed number of filler bits (F) equal to encoder.bgK - 5 is added to the message.
 * if the function is called with -e0 (no rate matchign), the rm_length is set to finalN - F,
 * So that after rate-dematching (which includes filler bits) the input to the decoder has lenght N.
 *
 * Basic rate-matching simulations can be carried out by setting the codeword
 * length to a value smaller than the base one.
 *
 *
 * Synopsis: **ldpc_chain_test [options]**
 *
 * Options:
 *  - **-b \<number\>** Base Graph (1 or 2. Default 1).
 *  - **-l \<number\>** Lifting Size (according to 5GNR standard. Default 2).
 *  - **-e \<number\>** Codeword length after rate matching (set to 0 [default] for full rate).
 *  - **-s \<number\>** SNR in dB (Default 3 dB).
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/fec/ldpc/ldpc_common.h"
#include "srslte/phy/fec/ldpc/ldpc_decoder.h"
#include "srslte/phy/fec/ldpc/ldpc_encoder.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"

srslte_basegraph_t base_graph = BG1; /*!< \brief Base Graph (BG1 or BG2). */
int                lift_size  = 2;   /*!< \brief Lifting Size. */
int                rm_length  = 0;   /*!< \brief Codeword length after rate matching. */
int                finalK;           /*!< \brief Number of uncoded bits (message length). */
int                finalN;           /*!< \brief Number of coded bits (codeword length). */
float              snr = 0;          /*!< \brief Signal-to-Noise Ratio [dB]. */

#define BATCH_SIZE 100    /*!< \brief Number of codewords in a batch. */
#define MAX_N_BATCH 10000 /*!< \brief Max number of simulated batches. */
#define REQ_ERRORS 100    /*!< \brief Minimum number of errors for a significant simulation. */
#define MS_SF 0.75f       /*!< \brief Scaling factor for the normalized min-sum decoding algorithm. */

/*!
 * \brief Prints test help when wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-bX] [-lX] [-eX] [-sX]\n", prog);
  printf("\t-b Base Graph [(1 or 2) Default %d]\n", base_graph + 1);
  printf("\t-l Lifting Size [Default %d]\n", lift_size);
  printf("\t-e Word length after rate matching [Default %d (no rate matching, only filler-bits are extracted)]\n",
         rm_length);
  printf("\t-s SNR [dB, Default %.1f dB]\n", snr);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:l:e:s:")) != -1) {
    switch (opt) {
      case 'b':
        base_graph = (int)strtol(optarg, NULL, 10) - 1;
        break;
      case 'l':
        lift_size = (int)strtol(optarg, NULL, 10);
        break;
      case 'e':
        rm_length = (int)strtol(optarg, NULL, 10);
        break;
      case 's':
        snr = strtod(optarg, NULL);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*!
 * \brief Prints decoder statistics.
 */
void print_decoder(char* title, int n_batches, int n_errors, double elapsed_time);

/*!
 * \brief Main test function.
 */
int main(int argc, char** argv)
{
  uint8_t* messages_true          = NULL;
  uint8_t* messages_sim_f         = NULL;
  uint8_t* messages_sim_s         = NULL;
  uint8_t* messages_sim_c         = NULL;
  uint8_t* messages_sim_c_flood   = NULL;
  uint8_t* messages_sim_avx       = NULL;
  uint8_t* messages_sim_avx_flood = NULL;
  uint8_t* codewords              = NULL;
  float*   symbols_rm             = NULL;
  float*   symbols                = NULL;
  int16_t* symbols_s              = NULL;
  int8_t*  symbols_c              = NULL;

  int i = 0;
  int j = 0;

  parse_args(argc, argv);

  // create an LDPC encoder
  srslte_ldpc_encoder_t encoder;
#ifdef LV_HAVE_AVX2
  if (srslte_ldpc_encoder_init(&encoder, SRSLTE_LDPC_ENCODER_AVX2, base_graph, lift_size) != 0) {
    perror("encoder init");
    exit(-1);
  }
#else  // no AVX2
  if (srslte_ldpc_encoder_init(&encoder, SRSLTE_LDPC_ENCODER_C, base_graph, lift_size) != 0) {
    perror("encoder init");
    exit(-1);
  }
#endif // LV_HAVE_AVX2

  // create an LDPC decoder (float)
  srslte_ldpc_decoder_t decoder_f;
  if (srslte_ldpc_decoder_init(&decoder_f, SRSLTE_LDPC_DECODER_F, base_graph, lift_size, MS_SF) != 0) {
    perror("decoder init");
    exit(-1);
  }
  // create an LDPC decoder (16 bit)
  srslte_ldpc_decoder_t decoder_s;
  if (srslte_ldpc_decoder_init(&decoder_s, SRSLTE_LDPC_DECODER_S, base_graph, lift_size, MS_SF) != 0) {
    perror("decoder init");
    exit(-1);
  }
  // create an LDPC decoder (8 bit)
  srslte_ldpc_decoder_t decoder_c;
  if (srslte_ldpc_decoder_init(&decoder_c, SRSLTE_LDPC_DECODER_C, base_graph, lift_size, MS_SF) != 0) {
    perror("decoder init");
    exit(-1);
  }
  // create an LDPC decoder (8 bit, flooded)
  srslte_ldpc_decoder_t decoder_c_flood;
  if (srslte_ldpc_decoder_init(&decoder_c_flood, SRSLTE_LDPC_DECODER_C_FLOOD, base_graph, lift_size, MS_SF) != 0) {
    perror("decoder init");
    exit(-1);
  }
#ifdef LV_HAVE_AVX2
  // create an LDPC decoder (8 bit, AVX2 version)
  srslte_ldpc_decoder_t decoder_avx;
  if (srslte_ldpc_decoder_init(&decoder_avx, SRSLTE_LDPC_DECODER_C_AVX2, base_graph, lift_size, MS_SF) != 0) {
    perror("decoder init");
    exit(-1);
  }

  // create an LDPC decoder (8 bit, flooded scheduling, AVX2 version)
  srslte_ldpc_decoder_t decoder_avx_flood;
  if (srslte_ldpc_decoder_init(&decoder_avx_flood, SRSLTE_LDPC_DECODER_C_AVX2_FLOOD, base_graph, lift_size, MS_SF) !=
      0) {
    perror("decoder init");
    exit(-1);
  }
#endif // LV_HAVE_AVX2

  // create a random generator
  srslte_random_t random_gen = srslte_random_init(0);

  uint32_t F = encoder.bgK - 5; // This value is arbitrary

  if (rm_length == 0) {
    rm_length = finalN - F;
  }

  printf("Test LDPC chain:\n");
  printf("  Base Graph      -> BG%d\n", encoder.bg + 1);
  printf("  Lifting Size    -> %d\n", encoder.ls);
  printf("  Protograph      -> M = %d, N = %d, K = %d\n", encoder.bgM, encoder.bgN, encoder.bgK);
  printf("  Lifted graph    -> M = %d, N = %d, K = %d\n", encoder.liftM, encoder.liftN, encoder.liftK);
  printf("  Base code rate -> K/(N-2) = %d/%d = 1/%d\n",
         encoder.liftK,
         encoder.liftN - 2 * lift_size,
         encoder.bg == BG1 ? 3 : 5);
  printf("\n  Codeword length after rate matching -> E = %d\n", rm_length);
  printf("  Final code rate -> (K-F)/E = (%d - %d)/%d = %.3f\n",
         encoder.liftK,
         F,
         rm_length,
         1.0 * (encoder.liftK - F) / rm_length);
  printf("\n  Signal-to-Noise Ratio -> %.2f dB\n", snr);

  finalK = encoder.liftK;
  finalN = encoder.liftN - 2 * lift_size;

  messages_true          = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_f         = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_s         = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_c         = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_c_flood   = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_avx       = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  messages_sim_avx_flood = malloc(finalK * BATCH_SIZE * sizeof(uint8_t));
  codewords              = malloc(finalN * BATCH_SIZE * sizeof(uint8_t));
  symbols_rm             = malloc((rm_length + F) * BATCH_SIZE * sizeof(float));
  symbols                = malloc(finalN * BATCH_SIZE * sizeof(float));
  symbols_s              = malloc(finalN * BATCH_SIZE * sizeof(int16_t));
  symbols_c              = malloc(finalN * BATCH_SIZE * sizeof(int8_t));
  if (!messages_true || !messages_sim_f || !messages_sim_s || !messages_sim_c || //
      !messages_sim_avx || !messages_sim_c_flood || !messages_sim_avx_flood ||   //
      !codewords || !symbols || !symbols_s || !symbols_c) {
    perror("malloc");
    exit(-1);
  }

  int            i_bit   = 0;
  int            i_batch = 0;
  struct timeval t[3];
  double         elapsed_time_enc           = 0;
  double         elapsed_time_dec_f         = 0;
  double         elapsed_time_dec_s         = 0;
  double         elapsed_time_dec_c         = 0;
  double         elapsed_time_dec_c_flood   = 0;
  double         elapsed_time_dec_avx       = 0;
  double         elapsed_time_dec_avx_flood = 0;
  int            n_error_words_f            = 0;
  int            n_error_words_s            = 0;
  int            n_error_words_c            = 0;
  int            n_error_words_c_flood      = 0;
  int            n_error_words_avx          = 0;
  int            n_error_words_avx_flood    = 0;

  float noise_std_dev = srslte_convert_dB_to_amplitude(-snr);

  int16_t inf15  = (1U << 14U) - 1;
  float   gain_s = inf15 * noise_std_dev / 20 / (1 / noise_std_dev + 2);

  int8_t inf7   = (1U << 6U) - 1;
  float  gain_c = inf7 * noise_std_dev / 8 / (1 / noise_std_dev + 2);

  printf("\nBatch:\n  ");

  while (((n_error_words_f < REQ_ERRORS) || (n_error_words_s < REQ_ERRORS) || (n_error_words_c < REQ_ERRORS)) &&
         (i_batch < MAX_N_BATCH)) {
    i_batch++;

    if (!(i_batch % 10)) {
      printf("%8d", i_batch);
      if (!(i_batch % 90)) {
        printf("\n  ");
      }
    }

    /* generate data_tx */

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK - F; j++) {
        messages_true[i * finalK + j] = srslte_random_uniform_int_dist(random_gen, 0, 1);
      }
      for (; j < finalK; j++) {
        messages_true[i * finalK + j] = FILLER_BIT;
      }
    }

    // compute the number of symbols that we need to encode/decode: closest multiple of
    // the lifting size that is larger than rm_length
    // Extra F bits are added since filler-bits are not part of the rm_length
    int n_useful_symbols =
        (rm_length + F) % lift_size ? ((rm_length + F) / lift_size + 1) * lift_size : (rm_length + F);

    printf("n_useful_symbols = %d\n", n_useful_symbols);

    // Encode messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_encoder_encode(
          &encoder, messages_true + j * finalK, codewords + j * finalN, finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_enc += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    // Modulate codewords and match rate (puncturing)
    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < rm_length + F; j++) {
        symbols_rm[i * (rm_length + F) + j] =
            (codewords[i * finalN + j] == FILLER_BIT) ? INFINITY : 1 - 2 * codewords[i * finalN + j];
      }
    }

    // Apply AWGN
    srslte_ch_awgn_f(symbols_rm, symbols_rm, noise_std_dev, BATCH_SIZE * (rm_length + F));

    // Convert symbols into LLRs
    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < rm_length + F; j++) { //+F becouse we have alredy considered fillerbits when modulating.
        symbols[i * finalN + j] = symbols_rm[i * (rm_length + F) + j] * 2 / (noise_std_dev * noise_std_dev);
      }
      // the rest of symbols are undetermined, set LLR to 0
      for (; j < finalN; j++) {
        symbols[i * finalN + j] = 0;
      }
    }

    //////// Floating point
    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_f(&decoder_f, symbols + j * finalN, messages_sim_f + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_f += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_f[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_f++;
          break;
        }
      }
    }

    //////// Fixed point - 16 bit
    // Quantize LLRs with 16 bits
    srslte_vec_quant_fs(symbols, symbols_s, gain_s, 0, inf15, BATCH_SIZE * finalN);

    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_s(&decoder_s, symbols_s + j * finalN, messages_sim_s + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_s += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_s[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_s++;
          break;
        }
      }
    }

    //////// Fixed point - 8 bit
    // Quantize LLRs with 8 bits
    srslte_vec_quant_fc(symbols, symbols_c, gain_c, 0, inf7, BATCH_SIZE * finalN);

    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_c(&decoder_c, symbols_c + j * finalN, messages_sim_c + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_c += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_c[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_c++;
          break;
        }
      }
    }

    //////// Fixed point - 8 bit, flooded scheduling

    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_c(
          &decoder_c_flood, symbols_c + j * finalN, messages_sim_c_flood + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_c_flood += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_c_flood[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_c_flood++;
          break;
        }
      }
    }

#ifdef LV_HAVE_AVX2
    //////// Fixed point - 8 bit - AVX2 version

    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_c(
          &decoder_avx, symbols_c + j * finalN, messages_sim_avx + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_avx += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_avx[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_avx++;
          break;
        }
      }
    }

    //////// Fixed point - 8 bit, flooded scheduling - AVX2 version

    // Recover messages
    gettimeofday(&t[1], NULL);
    for (j = 0; j < BATCH_SIZE; j++) {
      srslte_ldpc_decoder_decode_c(
          &decoder_avx_flood, symbols_c + j * finalN, messages_sim_avx_flood + j * finalK, n_useful_symbols);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time_dec_avx_flood += t[0].tv_sec + 1e-6 * t[0].tv_usec;

    for (i = 0; i < BATCH_SIZE; i++) {
      for (j = 0; j < finalK; j++) {
        i_bit = i * finalK + j;
        if (messages_sim_avx_flood[i_bit] != (1U & messages_true[i_bit])) {
          n_error_words_avx_flood++;
          break;
        }
      }
    }
#endif // LV_HAVE_AVX2
  }

  printf("\nEstimated throughput encoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
         i_batch * BATCH_SIZE / elapsed_time_enc,
         i_batch * BATCH_SIZE * finalK / elapsed_time_enc,
         i_batch * BATCH_SIZE * finalN / elapsed_time_enc);

  print_decoder("FLOATING POINT", i_batch, n_error_words_f, elapsed_time_dec_f);
  print_decoder("FIXED POINT (16 bits)", i_batch, n_error_words_s, elapsed_time_dec_s);
  print_decoder("FIXED POINT (8 bits)", i_batch, n_error_words_c, elapsed_time_dec_c);
  print_decoder("FIXED POINT (8 bits, flooded scheduling)", i_batch, n_error_words_c_flood, elapsed_time_dec_c_flood);

#ifdef LV_HAVE_AVX2
  print_decoder("FIXED POINT (8 bits - AVX2)", i_batch, n_error_words_avx, elapsed_time_dec_avx);
  print_decoder(
      "FIXED POINT (8 bits, flooded scheduling - AVX2)", i_batch, n_error_words_avx_flood, elapsed_time_dec_avx_flood);
#endif // LV_HAVE_AVX2

  if (n_error_words_s > 10 * n_error_words_f) {
    perror("16-bit performance too low!");
    exit(-1);
  }
  if (n_error_words_c > 10 * n_error_words_f) {
    perror("8-bit performance too low!");
    exit(-1);
  }
  printf("\nTest completed successfully!\n\n");

  free(symbols_c);
  free(symbols_s);
  free(symbols);
  free(codewords);
  free(messages_sim_avx);
  free(messages_sim_c_flood);
  free(messages_sim_c);
  free(messages_sim_s);
  free(messages_sim_f);
  free(messages_true);
  srslte_random_free(random_gen);
#ifdef LV_HAVE_AVX2
  srslte_ldpc_decoder_free(&decoder_avx);
#endif // LV_HAVE_AVX2
  srslte_ldpc_decoder_free(&decoder_c_flood);
  srslte_ldpc_decoder_free(&decoder_c);
  srslte_ldpc_decoder_free(&decoder_s);
  srslte_ldpc_decoder_free(&decoder_f);
  srslte_ldpc_encoder_free(&encoder);
}

void print_decoder(char* title, int n_batches, int n_errors, double elapsed_time)
{
  printf("\n**** %s ****", title);
  printf("\nEstimated word error rate:\n  %e (%d errors)\n", (double)n_errors / n_batches / BATCH_SIZE, n_errors);

  printf("Estimated throughput decoder:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
         n_batches * BATCH_SIZE / elapsed_time,
         n_batches * BATCH_SIZE * finalK / elapsed_time,
         n_batches * BATCH_SIZE * finalN / elapsed_time);
}
