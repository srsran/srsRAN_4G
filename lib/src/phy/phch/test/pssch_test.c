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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "srslte/phy/phch/pssch.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"

srslte_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 0, .tm = SRSLTE_SIDELINK_TM2, .cp = SRSLTE_CP_NORM};

static uint32_t        mcs_idx       = 4;
static uint32_t        prb_start_idx = 0;
static srslte_random_t random_gen    = NULL;

void usage(char* prog)
{
  printf("Usage: %s [emptv]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-m mcs_idx [Default %d]\n", mcs_idx);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "emptv")) != -1) {
    switch (opt) {
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
        break;
      case 'm':
        mcs_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        if (srslte_sl_tm_to_cell_sl_tm_t(&cell, strtol(argv[optind], NULL, 10)) != SRSLTE_SUCCESS) {
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
  if (SRSLTE_CP_ISEXT(cell.cp) && cell.tm >= SRSLTE_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  uint32_t ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  srslte_sl_comm_resource_pool_t sl_comm_resource_pool;
  if (srslte_sl_comm_resource_pool_get_default_config(&sl_comm_resource_pool, cell) != SRSLTE_SUCCESS) {
    ERROR("Error initializing sl_comm_resource_pool\n");
    return SRSLTE_ERROR;
  }

  srslte_pssch_t pssch = {};
  if (srslte_pssch_init(&pssch, cell, sl_comm_resource_pool) != SRSLTE_SUCCESS) {
    ERROR("Error initializing PSSCH\n");
    return SRSLTE_ERROR;
  }

  uint32_t nof_prb_pssch = srslte_dft_precoding_get_valid_prb(cell.nof_prb);
  uint32_t N_x_id    = 255;
  uint32_t sf_n_re   = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer = srslte_vec_cf_malloc(sf_n_re);
  if (!sf_buffer) {
    ERROR("Error allocating memory\n");
    return SRSLTE_ERROR;
  }
  srslte_vec_cf_zero(sf_buffer, sf_n_re);

  // Transport block buffer
  uint8_t tb[SRSLTE_SL_SCH_MAX_TB_LEN] = {};

  // Rx transport block buffer
  uint8_t tb_rx[SRSLTE_SL_SCH_MAX_TB_LEN] = {};

  srslte_pssch_cfg_t pssch_cfg = {prb_start_idx, nof_prb_pssch, N_x_id, mcs_idx, 0, 0};
  if (srslte_pssch_set_cfg(&pssch, pssch_cfg) != SRSLTE_SUCCESS) {
    ERROR("Error configuring PSSCH\n");
    goto clean_exit;
  }

  // Randomize data to fill the transport block
  struct timeval tv;
  gettimeofday(&tv, NULL);
  random_gen = srslte_random_init(tv.tv_usec);
  for (int i = 0; i < pssch.sl_sch_tb_len; i++) {
    tb[i] = srslte_random_uniform_int_dist(random_gen, 0, 1);
  }

  // PSSCH encoding
  if (srslte_pssch_encode(&pssch, tb, pssch.sl_sch_tb_len, sf_buffer) != SRSLTE_SUCCESS) {
    ERROR("Error encoding PSSCH\n");
    goto clean_exit;
  }

  // PSSCH decoding
  if (srslte_pssch_decode(&pssch, sf_buffer, tb_rx, pssch.sl_sch_tb_len) != SRSLTE_SUCCESS) {
    ERROR("Error decoding PSSCH\n");
    goto clean_exit;
  }

  if (memcmp(tb_rx, tb, pssch.sl_sch_tb_len) == 0) {
    ret = SRSLTE_SUCCESS;
  } else {
    ret = SRSLTE_ERROR;
  }

clean_exit:
  if (random_gen) {
    srslte_random_free(random_gen);
  }
  if (sf_buffer) {
    free(sf_buffer);
  }
  srslte_pssch_free(&pssch);

  printf("%s", ret == SRSLTE_SUCCESS ? "SUCCESS\n" : "FAILED\n");
  return ret;
}