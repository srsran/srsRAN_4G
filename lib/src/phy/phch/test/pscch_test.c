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
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/phch/pscch.h"
#include "srslte/phy/phch/sci.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

srslte_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 168, .tm = SRSLTE_SIDELINK_TM2, .cp = SRSLTE_CP_NORM};

uint32_t prb_start_idx = 0;

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-c N_sl_id [Default %d]\n", cell.N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "ceiptv")) != -1) {
    switch (opt) {
      case 'c':
        cell.N_sl_id = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        switch (strtol(argv[optind], NULL, 10)) {
          case 1:
            cell.tm = SRSLTE_SIDELINK_TM1;
            break;
          case 2:
            cell.tm = SRSLTE_SIDELINK_TM2;
            break;
          case 3:
            cell.tm = SRSLTE_SIDELINK_TM3;
            break;
          case 4:
            cell.tm = SRSLTE_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
            break;
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
  if (cell.cp == SRSLTE_CP_EXT && cell.tm >= SRSLTE_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  srslte_sl_comm_resource_pool_t sl_comm_resource_pool;
  if (srslte_sl_comm_resource_pool_get_default_config(&sl_comm_resource_pool, cell) != SRSLTE_SUCCESS) {
    ERROR("Error initializing sl_comm_resource_pool\n");
    return SRSLTE_ERROR;
  }

  char sci_msg[SRSLTE_SCI_MSG_MAX_LEN] = {};

  uint32_t sf_n_re   = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  // SCI
  srslte_sci_t sci;
  srslte_sci_init(&sci, cell, sl_comm_resource_pool);
  sci.mcs_idx = 2;

  // PSCCH
  srslte_pscch_t pscch;
  if (srslte_pscch_init(&pscch, SRSLTE_MAX_PRB) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH init\n");
    return SRSLTE_ERROR;
  }

  if (srslte_pscch_set_cell(&pscch, cell) != SRSLTE_SUCCESS) {
    ERROR("Error in PSCCH init\n");
    return SRSLTE_ERROR;
  }

  // SCI message bits
  uint8_t sci_tx[SRSLTE_SCI_MAX_LEN] = {};
  if (sci.format == SRSLTE_SCI_FORMAT0) {
    if (srslte_sci_format0_pack(&sci, sci_tx) != SRSLTE_SUCCESS) {
      printf("Error packing sci format 0\n");
      return SRSLTE_ERROR;
    }
  } else if (sci.format == SRSLTE_SCI_FORMAT1) {
    if (srslte_sci_format1_pack(&sci, sci_tx) != SRSLTE_SUCCESS) {
      printf("Error packing sci format 1\n");
      return SRSLTE_ERROR;
    }
  }

  printf("Tx payload: ");
  srslte_vec_fprint_hex(stdout, sci_tx, sci.sci_len);

  // Put SCI into PSCCH
  srslte_pscch_encode(&pscch, sci_tx, sf_buffer, prb_start_idx);

  // Prepare Rx buffer
  uint8_t sci_rx[SRSLTE_SCI_MAX_LEN] = {};

  // Decode PSCCH
  if (srslte_pscch_decode(&pscch, sf_buffer, sci_rx, prb_start_idx) == SRSLTE_SUCCESS) {
    printf("Rx payload: ");
    srslte_vec_fprint_hex(stdout, sci_rx, sci.sci_len);

    uint32_t riv_txed = sci.riv;
    if (sci.format == SRSLTE_SCI_FORMAT0) {
      if (srslte_sci_format0_unpack(&sci, sci_rx) != SRSLTE_SUCCESS) {
        printf("Error unpacking sci format 0\n");
        return SRSLTE_ERROR;
      }
    } else if (sci.format == SRSLTE_SCI_FORMAT1) {
      if (srslte_sci_format1_unpack(&sci, sci_rx) != SRSLTE_SUCCESS) {
        printf("Error unpacking sci format 1\n");
        return SRSLTE_ERROR;
      }
    }

    srslte_sci_info(&sci, sci_msg, sizeof(sci_msg));
    fprintf(stdout, "%s", sci_msg);
    if (sci.riv == riv_txed) {
      ret = SRSLTE_SUCCESS;
    }
  }

  free(sf_buffer);
  srslte_sci_free(&sci);
  srslte_pscch_free(&pscch);

  return ret;
}
