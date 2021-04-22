/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
 * \file ldpc_rm_test.c
 * \brief Unit test for the LDPC RateMatcher and RateDematcher.
 *
 * A batch of example messages is randomly generated, encoded, rate-matched, 2-PAM modulated,
 * and, finally, rate-dematched and decoded by all three types of
 * rate dematchers (float, int16_t, int8_t).
 * The rate-dematched codeword is compared against the transmitted codeword
 *
 * Synopsis: **ldpc_rm_test [options]**
 *
 * Options:
 *  - **-b \<number\>** Base Graph (1 or 2. Default 1).
 *  - **-l \<number\>** Lifting Size (according to 5GNR standard. Default 2).
 *  - **-e \<number\>** Codeword length after rate matching (set to 0 [default] for full rate).
 *  - **-f \<number\>** Number of filler bits (Default 17).
 *  - **-r \<number\>** Redundancy version {0-3}.
 *  - **-m \<number\>** Modulation type BPSK = 0, QPSK =1, QAM16 = 2, QAM64 = 3, QAM256 = 4.
 *  - **-M \<number\>** Limited buffer size.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/phy/fec/ldpc/ldpc_common.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/fec/ldpc/ldpc_rm.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/phy/utils/vector.h"

static srsran_basegraph_t base_graph = BG2; /*!< \brief Base Graph (BG1 or BG2). */
static uint32_t           lift_size  = 208; /*!< \brief Lifting Size. */
static uint32_t           C          = 2;   /*!< \brief Number of code block segments (CBS). */
static uint32_t           F          = 10;  /*!< \brief Number of filler bits in each CBS. */
static uint32_t           E          = 0;   /*!< \brief Rate-matched codeword size (E = 0, no rate matching). */
static uint8_t            rv         = 0;   /*!< \brief Redundancy version {0-3}. */
static srsran_mod_t       mod_type = SRSRAN_MOD_QPSK; /*!< \brief Modulation type: BPSK, QPSK, QAM16, QAM64, QAM256. */
static uint32_t           Nref     = 0;               /*!< \brief Limited buffer size.*/

static uint32_t N = 0; /*!< \brief Codeblock size (including punctured and filler bits). */
static uint32_t K = 0; /*!< \brief Codeword size. */

/*!
 * \brief Prints test help when a wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-bX] [-lX] [-eX] [-fX] [-rX] [-mX] [-MX]\n", prog);
  printf("\t-b Base Graph [(1 or 2) Default %d]\n", base_graph + 1);
  printf("\t-l Lifting Size [Default %d]\n", lift_size);
  printf("\t-e Word length after rate matching [Default %d (no rate matching i.e. E = N - F)]\n", E);
  printf("\t-f Filler bits size (F) [Default %d]\n", F);
  printf("\t-r Redundancy version (rv) [Default %d]\n", rv);
  printf("\t-m Modulation_type BPSK=0, QPSK=1, 16QAM=2, 64QAM=3, 256QAM = 4 [Default %d]\n", mod_type);
  printf("\t-M Limited buffer size (Nref) [Default = %d (normal buffer Nref = N)]\n", Nref);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:l:e:f:r:m:M:")) != -1) {
    switch (opt) {
      case 'b':
        base_graph = (uint32_t)strtol(optarg, NULL, 10) - 1;
        break;
      case 'l':
        lift_size = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'e':
        E = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'f':
        F = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'r':
        rv = (uint8_t)strtol(optarg, NULL, 10);
        break;
      case 'm':
        mod_type = (srsran_mod_t)strtol(optarg, NULL, 10);
        break;
      case 'M':
        Nref = (uint32_t)strtol(optarg, NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*!
 * \brief Main test function.
 */
int main(int argc, char** argv)
{
  uint8_t* codeblocks     = NULL; /*  codeblocks including filler bits  */
  uint8_t* codewords      = NULL;
  uint8_t* rm_codewords   = NULL;
  float*   rm_symbols     = NULL;
  int16_t* rm_symbols_s   = NULL;
  int8_t*  rm_symbols_c   = NULL;
  float*   unrm_symbols   = NULL;
  int16_t* unrm_symbols_s = NULL;
  int8_t*  unrm_symbols_c = NULL;

  uint32_t i     = 0;
  uint32_t r     = 0;
  int      error = 0;

  parse_args(argc, argv);

  srsran_random_t random_gen = srsran_random_init(0);

  // create an LDPC encoder
  srsran_ldpc_encoder_t encoder;
  if (srsran_ldpc_encoder_init(&encoder, SRSRAN_LDPC_ENCODER_C, base_graph, lift_size) != 0) {
    perror("encoder init");
    exit(-1);
  }

  K = encoder.liftK;
  N = encoder.liftN - 2 * lift_size;
  if (E == 0) {
    E = N - F;
  }
  if (Nref == 0) {
    Nref = N;
  }

  // create a LDPC rate Matcher
  srsran_ldpc_rm_t rm_tx;
  if (srsran_ldpc_rm_tx_init(&rm_tx) != 0) {
    perror("rate matcher init");
    exit(-1);
  }

  // create a LDPC rate DeMatcher
  srsran_ldpc_rm_t rm_rx;
  if (srsran_ldpc_rm_rx_init_f(&rm_rx) != 0) {
    perror("rate dematcher init");
    exit(-1);
  }

  // create a LDPC rate DeMatcher (int16_t)
  srsran_ldpc_rm_t rm_rx_s;
  if (srsran_ldpc_rm_rx_init_s(&rm_rx_s) != 0) {
    perror("rate dematcher init (int16_t)");
    exit(-1);
  }

  // create a LDPC rate DeMatcher (int8_t)
  srsran_ldpc_rm_t rm_rx_c;
  if (srsran_ldpc_rm_rx_init_c(&rm_rx_c) != 0) {
    perror("rate dematcher init (int8_t)");
    exit(-1);
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
  printf("\n");
  printf("  Codeblock length             ->   K = %d\n", K);
  printf("  Codeword length              ->   N = %d\n", N);
  printf("  Rate matched codeword length ->   E = %d\n", E);
  printf("  Number of filler bits        ->   F = %d\n", F);
  printf("  Redundancy version           ->  rv = %d\n", rv);
  printf("  Final code rate  -> (K-F)/E = (%d - %d)/%d = %.3f\n", encoder.liftK, F, E, 1.0 * (encoder.liftK - F) / E);
  printf("\n");

  codeblocks     = srsran_vec_u8_malloc(C * K);
  codewords      = srsran_vec_u8_malloc(C * N);
  rm_codewords   = srsran_vec_u8_malloc(C * E);
  rm_symbols     = srsran_vec_f_malloc(C * E);
  rm_symbols_s   = srsran_vec_i16_malloc(C * E);
  rm_symbols_c   = srsran_vec_i8_malloc(C * E);
  unrm_symbols   = srsran_vec_f_malloc(C * N);
  unrm_symbols_s = srsran_vec_i16_malloc(C * N);
  unrm_symbols_c = srsran_vec_i8_malloc(C * N);
  if (!codeblocks || !codewords || !rm_codewords || !rm_symbols || !rm_symbols_s || !rm_symbols_c || !unrm_symbols ||
      !unrm_symbols_s || !unrm_symbols_c) {
    perror("malloc");
    exit(-1);
  }

  // Generate random bits
  for (r = 0; r < C; r++) {
    for (i = 0; i < K - F; i++) {
      codeblocks[r * K + i] = srsran_random_uniform_int_dist(random_gen, 0, 1);
    }
    for (; i < K; i++) { // add filler bits
      codeblocks[r * K + i] = FILLER_BIT;
    }
  }

  // Encode messages
  // gettimeofday(&t[1], NULL);
  for (r = 0; r < C; r++) {
    if (srsran_ldpc_encoder_encode(&encoder, codeblocks + r * K, codewords + r * N, K)) {
      exit(-1);
    }

    // LDPC rate matching
    if (srsran_ldpc_rm_tx(
            &rm_tx, codewords + r * N, rm_codewords + r * E, E, base_graph, lift_size, rv, mod_type, Nref)) {
      exit(-1);
    }

    // Modulate codewords
    // quantization

    int16_t inf16 = (1U << 15U) - 1;
    int8_t  inf8  = (1U << 7U) - 1;
    for (i = 0; i < E; i++) {
      rm_symbols[r * E + i]   = rm_codewords[r * E + i] ? -1 : 1;
      rm_symbols_s[r * E + i] = rm_codewords[r * E + i] ? -1 : 1;
      rm_symbols_c[r * E + i] = rm_codewords[r * E + i] ? -1 : 1;
    }

    // HARQ procedure. Use previous previous rate-matched outputs if available from the previous redundancy versions,
    // otherwise initialize to zeros.
    bzero(unrm_symbols + r * N, N * sizeof(float));
    bzero(unrm_symbols_s + r * N, N * sizeof(int16_t));
    bzero(unrm_symbols_c + r * N, N * sizeof(int8_t));

    if (srsran_ldpc_rm_rx_f(
            &rm_rx, rm_symbols + r * E, unrm_symbols + r * N, E, F, base_graph, lift_size, rv, mod_type, Nref)) {
      exit(-1);
    }
    if (srsran_ldpc_rm_rx_s(
            &rm_rx_s, rm_symbols_s + r * E, unrm_symbols_s + r * N, E, F, base_graph, lift_size, rv, mod_type, Nref)) {
      exit(-1);
    }
    if (srsran_ldpc_rm_rx_c(
            &rm_rx_c, rm_symbols_c + r * E, unrm_symbols_c + r * N, E, F, base_graph, lift_size, rv, mod_type, Nref) <
        0) {
      exit(-1);
    }

    // Check self correctness for the float version
    error = 0;
    for (i = 0; i < N; i++) {
      if (((unrm_symbols[i + r * N] == 0) && (codewords[i + r * N] != FILLER_BIT)) ||
          ((unrm_symbols[i + r * N] == INFINITY) && (codewords[i + r * N] == FILLER_BIT)) ||
          ((unrm_symbols[i + r * N] > 0) && (codewords[i + r * N] == 0)) ||
          ((unrm_symbols[i + r * N] < 0) && (codewords[i + r * N]))) {
        // any of these cases are ok
      } else {
        error = -1;
        break;
      }
    }

    if (error < 0) {
      printf("Error in rate-matching block at code segment: %d\n unrm_symb[%d] = %2.1f\n codeword[%d] = %d\n",
             r,
             i,
             unrm_symbols[i + r * N],
             i,
             codewords[i + r * N]);
    } else {
      printf(" No errors in rate-matching block\n");
    }
    // check against float implementation
    for (i = 0; i < N; i++) {
      if (((int16_t)unrm_symbols[i + r * N] == unrm_symbols_s[i + r * N]) ||
          (unrm_symbols[i + r * N] == INFINITY && unrm_symbols_s[i + r * N] == inf16) ||
          ((int16_t)unrm_symbols[i + r * N] == 0 && unrm_symbols_s[i + r * N] == 0)) {
      } else {
        error = -2;
        break;
      }
    }
    if (error == -2) {
      printf("Error in rate-matching block (int16_t) at code segment: %d\n unrm_symb[%d] = %d\n unrm_symb_s[%d] = %d\n",
             r,
             i,
             (int16_t)unrm_symbols[i + r * N],
             i,
             unrm_symbols_s[i + r * N]);
    } else {
      printf(" No errors in rate-matching block (int16_t)\n");
    }

    // check against float implementation
    for (i = 0; i < N; i++) {
      if (((int8_t)unrm_symbols[i + r * N] == unrm_symbols_c[i + r * N]) ||
          (unrm_symbols[i + r * N] == INFINITY && unrm_symbols_c[i + r * N] == inf8)) {
      } else {
        error = -3;
        break;
      }
    }
    if (error == -3) {
      printf(
          "Error in rate-matching block (int8_t) at code segment: %d\n unrm_symb[%d] = %2.1f\n unrm_symb_c[%d] = %d\n",
          r,
          i,
          unrm_symbols[i + r * N],
          i,
          unrm_symbols_c[i + r * N]);
    } else {
      printf(" No errors in rate-matching block: (int8_t)\n");
    }

  } // codeblocks r

  free(unrm_symbols);
  free(unrm_symbols_s);
  free(unrm_symbols_c);
  free(rm_symbols);
  free(rm_symbols_s);
  free(rm_symbols_c);
  free(rm_codewords);
  free(codewords);
  free(codeblocks);
  srsran_random_free(random_gen);
  srsran_ldpc_encoder_free(&encoder);
  srsran_ldpc_rm_tx_free(&rm_tx);
  srsran_ldpc_rm_rx_free_f(&rm_rx);
  srsran_ldpc_rm_rx_free_s(&rm_rx_s);
  srsran_ldpc_rm_rx_free_c(&rm_rx_c);
  return error;
}
