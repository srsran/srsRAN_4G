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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/sync/psss.h"
#include "srslte/phy/sync/ssss.h"
#include "srslte/srslte.h"

char* input_file_name;
float          frequency_offset       = 0.0;
float          snr                    = 100.0;
srslte_cp_t    cp                     = SRSLTE_CP_NORM;
uint32_t       nof_prb                = 6;
bool           use_standard_lte_rates = false;
srslte_sl_tm_t tm                     = SRSLTE_SIDELINK_TM2;
uint32_t       max_subframes          = 10;

srslte_filesource_t fsrc;

void usage(char* prog)
{
  printf("Usage: %s [cdefiopstv]\n", prog);
  printf("\t-i input_file_name\n");
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-m max_subframes [Default %d]\n", max_subframes);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (tm + 1));
  printf("\t-d use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cdefimpstv")) != -1) {
    switch (opt) {
      case 'd':
        use_standard_lte_rates = true;
        break;
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'm':
        max_subframes = strtoul(argv[optind], NULL, 0);
        break;
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        snr = strtof(argv[optind], NULL);
        break;
      case 't':
        switch (strtol(argv[optind], NULL, 10)) {
          case 1:
            tm = SRSLTE_SIDELINK_TM1;
            break;
          case 2:
            tm = SRSLTE_SIDELINK_TM2;
            break;
          case 3:
            tm = SRSLTE_SIDELINK_TM3;
            break;
          case 4:
            tm = SRSLTE_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
        }
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

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  srslte_use_standard_symbol_size(use_standard_lte_rates);

  if (!input_file_name || srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    printf("Error opening file %s\n", input_file_name);
    return SRSLTE_ERROR;
  }

  // alloc memory
  uint32_t sf_n_samples = SRSLTE_SF_LEN_PRB(nof_prb);
  printf("I/Q samples per subframe=%d\n", sf_n_samples);

  uint32_t sf_n_re   = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * 2 * nof_prb;
  cf_t*    sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  cf_t* input_buffer      = srslte_vec_cf_malloc(sf_n_samples);
  cf_t* input_buffer_temp = srslte_vec_cf_malloc(sf_n_samples);

  // init PSSS
  srslte_psss_t psss = {};
  srslte_psss_init(&psss, nof_prb, cp);

  struct timeval t[3];
  gettimeofday(&t[1], NULL);

  bool     sync          = false;
  uint32_t num_subframes = 0;
  int32_t  samples_read  = 0;

  do {
    // Read and normalize samples from file
    samples_read = srslte_filesource_read(&fsrc, input_buffer, sf_n_samples);
    if (samples_read == 0) {
      // read entire file
      break;
    } else if (samples_read != sf_n_samples) {
      printf("Could only read %d of %d requested samples\n", samples_read, sf_n_samples);
      return SRSLTE_ERROR;
    }

    // Find PSSS signal
    if (srslte_psss_find(&psss, input_buffer, nof_prb, cp) == SRSLTE_SUCCESS) {
      printf("PSSS correlation peak pos: %d value: %f N_id_2: %d\n",
             psss.corr_peak_pos,
             psss.corr_peak_value,
             psss.N_id_2);
      sync = true;
    }
    num_subframes++;
  } while (samples_read == sf_n_samples && num_subframes < max_subframes);

  srslte_filesource_free(&fsrc);
  srslte_psss_free(&psss);
  free(input_buffer);
  free(input_buffer_temp);
  free(sf_buffer);

  return (sync == SRSLTE_SUCCESS);
}
