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
 * \file ldpc_dec_avx2_test.c
 * \brief Unit test for the LDPC decoder working with 8-bit integer-valued LLRs (AVX2 implementation).
 *
 * It decodes a batch of example codewords and compares the resulting messages
 * with the expected ones. Reference messages and codewords are provided in
 * files **examplesBG1.dat** and **examplesBG2.dat**.
 *
 * Synopsis: **ldpc_dec_c_test [options]**
 *
 * Options:
 *  - **-b \<number\>** Base Graph (1 or 2. Default 1).
 *  - **-l \<number\>** Lifting Size (according to 5GNR standard. Default 2).
 */

#include "srsran/phy/utils/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/phy/fec/ldpc/ldpc_common.h"
#include "srsran/phy/fec/ldpc/ldpc_decoder.h"
#include "srsran/phy/utils/debug.h"

srsran_basegraph_t base_graph = BG1; /*!< \brief Base Graph (BG1 or BG2). */
int                lift_size  = 2;   /*!< \brief Lifting Size. */
int                finalK;           /*!< \brief Number of uncoded bits (message length). */
int                finalN;           /*!< \brief Number of coded bits (codeword length). */
int                scheduling = 0;   /*!< \brief Message scheduling (0 for layered, 1 for flooded). */

#define NOF_MESSAGES 10  /*!< \brief Number of codewords in the test. */
static int nof_reps = 1; /*!< \brief Number of times tests are repeated (for computing throughput). */

/*!
 * \brief Prints test help when a wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-bX] [-lX]\n", prog);
  printf("\t-b Base Graph [(1 or 2) Default %d]\n", base_graph + 1);
  printf("\t-l Lifting Size [Default %d]\n", lift_size);
  printf("\t-x Scheduling [Default %c]\n", scheduling);
  printf("\t-R Number of times tests are repeated (for computing throughput). [Default %d]\n", nof_reps);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:l:x:R:")) != -1) {
    switch (opt) {
      case 'b':
        base_graph = (int)strtol(optarg, NULL, 10) - 1;
        break;
      case 'l':
        lift_size = (int)strtol(optarg, NULL, 10);
        break;
      case 'x':
        scheduling = (int)strtol(optarg, NULL, 10);
        break;
      case 'R':
        nof_reps = (int)strtol(optarg, NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*!
 * \brief Reads the example file.
 */
void get_examples(uint8_t* messages, //
                  uint8_t* codewords,
                  FILE*    ex_file)
{
  char mstr[15]; // message string
  char cstr[15]; // codeword string
  char tmp[15];
  int  i = 0;
  int  j = 0;

  sprintf(mstr, "ls%dmsgs", lift_size);
  sprintf(cstr, "ls%dcwds", lift_size);
  do {
    do {
      tmp[0] = fgetc(ex_file);
    } while (tmp[0] != 'l');
    fscanf(ex_file, "%[^\n]", tmp + 1);
    fgetc(ex_file); // discard newline
  } while (strcmp(tmp, mstr) != 0);

  // read messages
  for (j = 0; j < NOF_MESSAGES; j++) {
    for (i = 0; i < finalK; i++) {
      int rc                   = fgetc(ex_file);
      messages[j * finalK + i] = (uint8_t)(rc == '-' ? FILLER_BIT : rc - '0');
    }
    fgetc(ex_file); // discard newline
  }

  fscanf(ex_file, "%[^\n]", tmp);
  if (strcmp(tmp, cstr) != 0) {
    printf("Something went wrong while reading example file.\n");
    exit(-1);
  }
  fgetc(ex_file); // discard newline

  // read codewords
  for (j = 0; j < NOF_MESSAGES; j++) {
    for (i = 0; i < finalN; i++) {
      int rc                    = fgetc(ex_file);
      codewords[j * finalN + i] = (uint8_t)(rc == '-' ? FILLER_BIT : rc - '0');
    }
    fgetc(ex_file); // discard newline
  }
}

/*!
 * \brief Main test function.
 */
int main(int argc, char** argv)
{
  uint8_t* messages_true = NULL;
  uint8_t* messages_sim  = NULL;
  uint8_t* codewords     = NULL;
  int8_t*  symbols       = NULL;
  int      i             = 0;
  int      j             = 0;
  int      l             = 0;

  FILE* ex_file = NULL;
  char  file_name[1000];

  parse_args(argc, argv);

  srsran_ldpc_decoder_type_t dectype =
      (scheduling == 0) ? SRSRAN_LDPC_DECODER_C_AVX2 : SRSRAN_LDPC_DECODER_C_AVX2_FLOOD;

  // Create LDPC configuration arguments
  srsran_ldpc_decoder_args_t decoder_args = {};
  decoder_args.type                       = dectype;
  decoder_args.bg                         = base_graph;
  decoder_args.ls                         = lift_size;
  decoder_args.scaling_fctr               = 1.0f;

  // create an LDPC decoder
  srsran_ldpc_decoder_t decoder;
  if (srsran_ldpc_decoder_init(&decoder, &decoder_args) != 0) {
    perror("decoder init");
    exit(-1);
  }

  printf("Test LDPC decoder:\n");
  printf("  Base Graph      -> BG%d\n", decoder.bg + 1);
  printf("  Lifting Size    -> %d\n", decoder.ls);
  printf("  Protograph      -> M = %d, N = %d, K = %d\n", decoder.bgM, decoder.bgN, decoder.bgK);
  printf("  Lifted graph    -> M = %d, N = %d, K = %d\n", decoder.liftM, decoder.liftN, decoder.liftK);
  printf("  Final code rate -> K/(N-2) = %d/%d = 1/%d\n",
         decoder.liftK,
         decoder.liftN - 2 * lift_size,
         decoder.bg == BG1 ? 3 : 5);
  printf("  Scheduling: %s\n", scheduling ? "flooded" : "layered");

  finalK = decoder.liftK;
  finalN = decoder.liftN - 2 * lift_size;

  messages_true = srsran_vec_u8_malloc(finalK * NOF_MESSAGES);
  messages_sim  = srsran_vec_u8_malloc(finalK * NOF_MESSAGES);
  codewords     = srsran_vec_u8_malloc(finalN * NOF_MESSAGES);
  symbols       = srsran_vec_i8_malloc(finalN * NOF_MESSAGES);
  if (!messages_true || !messages_sim || !codewords || !symbols) {
    perror("malloc");
    exit(-1);
  }

  sprintf(file_name, "examplesBG%d.dat", base_graph + 1);
  printf("\nReading example file %s...\n", file_name);
  ex_file = fopen(file_name, "re");
  if (ex_file == NULL) {
    perror("fopen");
    exit(-1);
  }

  get_examples(messages_true, codewords, ex_file);

  fclose(ex_file);

  for (i = 0; i < NOF_MESSAGES * finalN; i++) {
    symbols[i] = codewords[i] == 1 ? -2 : 2;
  }

  printf("\nDecoding test messages...\n");
  struct timeval t[3];
  double         elapsed_time = 0;

  for (j = 0; j < NOF_MESSAGES; j++) {
    printf("  codeword %d\n", j);
    gettimeofday(&t[1], NULL);
    for (l = 0; l < nof_reps; l++) {
      srsran_ldpc_decoder_decode_c(&decoder, symbols + j * finalN, messages_sim + j * finalK, finalN);
    }

    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time += t[0].tv_sec + 1e-6 * t[0].tv_usec;
  }
  printf("Elapsed time: %e s\n", elapsed_time);

  printf("\nVerifing results...\n");
  for (i = 0; i < NOF_MESSAGES * finalK; i++) {
    if ((1U & messages_sim[i]) != (1U & messages_true[i])) {
      perror("wrong!!");
      exit(-1);
    }
  }

  printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
         NOF_MESSAGES / elapsed_time,
         NOF_MESSAGES * finalK / elapsed_time,
         NOF_MESSAGES * finalN / elapsed_time);

  printf("\nTest completed successfully!\n\n");

  free(symbols);
  free(codewords);
  free(messages_sim);
  free(messages_true);
  srsran_ldpc_decoder_free(&decoder);
}
