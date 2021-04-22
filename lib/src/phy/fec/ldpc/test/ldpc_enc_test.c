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
 * \file ldpc_enc_test.c
 * \brief Unit test for the LDPC encoder.
 *
 * It encodes a batch of example messages and compares the resulting codewords
 * with the expected ones. Reference messages and codewords are provided in
 * files **examplesBG1.dat** and **examplesBG2.dat**.
 *
 * Synopsis: **ldpc_enc_test [options]**
 *
 * Options:
 *  - **-b \<number\>** Base Graph (1 or 2. Default 1).
 *  - **-l \<number\>** Lifting Size (according to 5GNR standard. Default 2).
 *  - **-R \<number\>** Number of times tests are repeated (for computing throughput).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/phy/fec/ldpc/ldpc_common.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

static srsran_basegraph_t base_graph = BG1; /*!< \brief Base Graph (BG1 or BG2). */
static int                lift_size  = 2;   /*!< \brief Lifting Size. */
static int                finalK;           /*!< \brief Number of uncoded bits (message length). */
static int                finalN;           /*!< \brief Number of coded bits (codeword length). */

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
  printf("\t-R Number of times tests are repeated (for computing throughput). [Default %d]\n", lift_size);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "b:l:R:")) != -1) {
    switch (opt) {
      case 'b':
        base_graph = (int)strtol(optarg, NULL, 10) - 1;
        break;
      case 'l':
        lift_size = (int)strtol(optarg, NULL, 10);
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
  uint8_t* messages       = NULL;
  uint8_t* codewords_true = NULL;
  uint8_t* codewords_sim  = NULL;

  int i = 0;
  int j = 0;
  int l = 0;

  FILE* ex_file = NULL;
  char  file_name[1000];

  parse_args(argc, argv);

  // create an LDPC encoder
  srsran_ldpc_encoder_t encoder;
  if (srsran_ldpc_encoder_init(&encoder, SRSRAN_LDPC_ENCODER_C, base_graph, lift_size) != 0) {
    perror("encoder init");
    exit(-1);
  }

  printf("Test LDPC encoder:\n");
  printf("  Base Graph      -> BG%d\n", encoder.bg + 1);
  printf("  Lifting Size    -> %d\n", encoder.ls);
  printf("  Protograph      -> M = %d, N = %d, K = %d\n", encoder.bgM, encoder.bgN, encoder.bgK);
  printf("  Lifted graph    -> M = %d, N = %d, K = %d\n", encoder.liftM, encoder.liftN, encoder.liftK);
  printf("  Final code rate -> K/(N-2) = %d/%d = 1/%d\n",
         encoder.liftK,
         encoder.liftN - 2 * lift_size,
         encoder.bg == BG1 ? 3 : 5);

  finalK = encoder.liftK;
  finalN = encoder.liftN - 2 * lift_size;

  messages       = srsran_vec_u8_malloc(finalK * NOF_MESSAGES);
  codewords_true = srsran_vec_u8_malloc(finalN * NOF_MESSAGES);
  codewords_sim  = srsran_vec_u8_malloc(finalN * NOF_MESSAGES);
  if (!messages || !codewords_true || !codewords_sim) {
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

  get_examples(messages, codewords_true, ex_file);

  fclose(ex_file);

  printf("\nEncoding test messages...\n");
  struct timeval t[3];
  double         elapsed_time = 0;
  for (j = 0; j < NOF_MESSAGES; j++) {
    printf("  codeword %d\n", j);
    gettimeofday(&t[1], NULL);
    for (l = 0; l < nof_reps; l++) {
      srsran_ldpc_encoder_encode_rm(&encoder, messages + j * finalK, codewords_sim + j * finalN, finalK, finalN);
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    elapsed_time += t[0].tv_sec + 1e-6 * t[0].tv_usec;
  }
  printf("Elapsed time: %e s\n", elapsed_time / nof_reps);

  printf("\nVerifing results...\n");
  for (i = 0; i < NOF_MESSAGES * finalN; i++) {
    if (codewords_sim[i] != codewords_true[i]) {
      perror("wrong!!");
      exit(-1);
    }
  }

  printf("Estimated throughput:\n  %e word/s\n  %e bit/s (information)\n  %e bit/s (encoded)\n",
         NOF_MESSAGES / (elapsed_time / nof_reps),
         NOF_MESSAGES * finalK / (elapsed_time / nof_reps),
         NOF_MESSAGES * finalN / (elapsed_time / nof_reps));

  printf("\nTest completed successfully!\n\n");

  free(codewords_sim);
  free(codewords_true);
  free(messages);
  srsran_ldpc_encoder_free(&encoder);
}
