/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/phy/common/phy_common.h"
#include <srslte/phy/ch_estimation/chest_sl.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

uint32_t       nof_prb        = 25;
uint32_t       N_sl_id        = 1;
srslte_cp_t    cp             = SRSLTE_CP_NORM;
srslte_sl_tm_t tm             = SRSLTE_SIDELINK_TM4;
bool           run_psbch_test = true;

void usage(char* prog)
{
  printf("Usage: %s [recov]\n", prog);

  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-e extended cyclic prefix [Default normal]\n");

  printf("\t-c N_sl_id [Default %d]\n", N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (tm + 1));

  printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "pecotv")) != -1) {
    switch (opt) {
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cp = SRSLTE_CP_EXT;
        break;
      case 'c':
        N_sl_id = (uint32_t)strtol(argv[optind], NULL, 10);
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

  int   sf_n_re   = SRSLTE_CP_NSYMB(cp) * SRSLTE_NRE * nof_prb * 2;
  cf_t* sf_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  bzero(sf_buffer, sizeof(cf_t) * sf_n_re);

  // Variables init Rx
  cf_t* sf_buffer_rx = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
  bzero(sf_buffer_rx, sizeof(cf_t) * sf_n_re);

  cf_t* dmrs_received[SRSLTE_SL_MAX_DMRS_SYMB] = {NULL};
  for (int i = 0; i < SRSLTE_SL_MAX_DMRS_SYMB; i++) {
    dmrs_received[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_NRE * nof_prb);
  }

  // Variables init Tx
  srslte_chest_sl_t q = {};

  if (run_psbch_test) {
    // Tx
    srslte_chest_sl_init_psbch_dmrs(&q);
    srslte_chest_sl_gen_psbch_dmrs(&q, tm, N_sl_id);
    srslte_chest_sl_put_psbch_dmrs(&q, sf_buffer, tm, nof_prb, cp);

    // Rx
    srslte_chest_sl_psbch_ls_estimate_equalize(&q, sf_buffer, sf_buffer_rx, nof_prb, tm, cp);
    srslte_chest_sl_get_psbch_dmrs(&q, sf_buffer_rx, dmrs_received, tm, nof_prb, cp);

    // Test
    // TODO: add proper test
  }

  // Print of received DMRS
  printf("\n\nPrint of received DMRS\n");
  for (int i = 0; i < q.M_sc_rs; i++) {
    printf("%i\t", i + 1);
    for (int j = 0; j < q.nof_dmrs_symbols; j++) {
      printf("%f\t%f\t", __real__ dmrs_received[j][i], __imag__ dmrs_received[j][i]);
    }
    printf("\n");
  }

  srslte_chest_sl_free(&q);

  if (sf_buffer) {
    free(sf_buffer);
  }

  if (sf_buffer_rx) {
    free(sf_buffer_rx);
  }

  for (int i = 0; i < SRSLTE_SL_MAX_DMRS_SYMB; i++) {
    if (dmrs_received[i]) {
      free(dmrs_received[i]);
    }
  }

  return SRSLTE_SUCCESS;
}