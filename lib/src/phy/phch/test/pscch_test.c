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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srsran/phy/phch/pscch.h"
#include "srsran/phy/phch/sci.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

srsran_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 168, .tm = SRSRAN_SIDELINK_TM2, .cp = SRSRAN_CP_NORM};

uint32_t prb_start_idx = 0;

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-c N_sl_id [Default %d]\n", cell.N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (cell.tm + 1));
  printf("\t-v [set srsran_verbose to debug, default none]\n");
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
            cell.tm = SRSRAN_SIDELINK_TM1;
            break;
          case 2:
            cell.tm = SRSRAN_SIDELINK_TM2;
            break;
          case 3:
            cell.tm = SRSRAN_SIDELINK_TM3;
            break;
          case 4:
            cell.tm = SRSRAN_SIDELINK_TM4;
            break;
          default:
            usage(argv[0]);
            exit(-1);
            break;
        }
        break;
      case 'v':
        srsran_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (cell.cp == SRSRAN_CP_EXT && cell.tm >= SRSRAN_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  parse_args(argc, argv);

  srsran_sl_comm_resource_pool_t sl_comm_resource_pool;
  if (srsran_sl_comm_resource_pool_get_default_config(&sl_comm_resource_pool, cell) != SRSRAN_SUCCESS) {
    ERROR("Error initializing sl_comm_resource_pool");
    return SRSRAN_ERROR;
  }

  char sci_msg[SRSRAN_SCI_MSG_MAX_LEN] = {};

  uint32_t sf_n_re   = SRSRAN_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer = srsran_vec_cf_malloc(sf_n_re);

  // SCI
  srsran_sci_t sci;
  srsran_sci_init(&sci, &cell, &sl_comm_resource_pool);
  sci.mcs_idx = 2;

  // PSCCH
  srsran_pscch_t pscch;
  if (srsran_pscch_init(&pscch, SRSRAN_MAX_PRB) != SRSRAN_SUCCESS) {
    ERROR("Error in PSCCH init");
    return SRSRAN_ERROR;
  }

  if (srsran_pscch_set_cell(&pscch, cell) != SRSRAN_SUCCESS) {
    ERROR("Error in PSCCH init");
    return SRSRAN_ERROR;
  }

  // SCI message bits
  uint8_t sci_tx[SRSRAN_SCI_MAX_LEN] = {};
  if (sci.format == SRSRAN_SCI_FORMAT0) {
    if (srsran_sci_format0_pack(&sci, sci_tx) != SRSRAN_SUCCESS) {
      printf("Error packing sci format 0\n");
      return SRSRAN_ERROR;
    }
  } else if (sci.format == SRSRAN_SCI_FORMAT1) {
    if (srsran_sci_format1_pack(&sci, sci_tx) != SRSRAN_SUCCESS) {
      printf("Error packing sci format 1\n");
      return SRSRAN_ERROR;
    }
  }

  printf("Tx payload: ");
  srsran_vec_fprint_hex(stdout, sci_tx, sci.sci_len);

  // Put SCI into PSCCH
  srsran_pscch_encode(&pscch, sci_tx, sf_buffer, prb_start_idx);

  // Prepare Rx buffer
  uint8_t sci_rx[SRSRAN_SCI_MAX_LEN] = {};

  // Decode PSCCH
  if (srsran_pscch_decode(&pscch, sf_buffer, sci_rx, prb_start_idx) == SRSRAN_SUCCESS) {
    printf("Rx payload: ");
    srsran_vec_fprint_hex(stdout, sci_rx, sci.sci_len);

    uint32_t riv_txed = sci.riv;
    if (sci.format == SRSRAN_SCI_FORMAT0) {
      if (srsran_sci_format0_unpack(&sci, sci_rx) != SRSRAN_SUCCESS) {
        printf("Error unpacking sci format 0\n");
        return SRSRAN_ERROR;
      }
    } else if (sci.format == SRSRAN_SCI_FORMAT1) {
      if (srsran_sci_format1_unpack(&sci, sci_rx) != SRSRAN_SUCCESS) {
        printf("Error unpacking sci format 1\n");
        return SRSRAN_ERROR;
      }
    }

    srsran_sci_info(&sci, sci_msg, sizeof(sci_msg));
    fprintf(stdout, "%s", sci_msg);
    if (sci.riv == riv_txed) {
      ret = SRSRAN_SUCCESS;
    }
  }

  free(sf_buffer);
  srsran_sci_free(&sci);
  srsran_pscch_free(&pscch);

  return ret;
}
