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
#include <strings.h>
#include <unistd.h>

#include "srslte/common/test_common.h"
#include <srslte/phy/phch/mib_sl.h>
#include <srslte/phy/phch/psbch.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>

srslte_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 168, .tm = SRSLTE_SIDELINK_TM2, .cp = SRSLTE_CP_NORM};

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended CP [Default normal]\n");
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
      case 'e':
        cell.cp = SRSLTE_CP_EXT;
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
  if (SRSLTE_CP_ISEXT(cell.cp) && cell.tm >= SRSLTE_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  uint32_t sf_n_re   = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer = srslte_vec_cf_malloc(sf_n_re);

  // MIB-SL
  srslte_mib_sl_t mib_sl;
  srslte_mib_sl_init(&mib_sl, cell.tm);
  srslte_mib_sl_set(&mib_sl, cell.nof_prb, 0, 128, 4, false);

  // PSBCH
  srslte_psbch_t psbch;
  if (srslte_psbch_init(&psbch, cell.nof_prb, cell.N_sl_id, cell.tm, cell.cp) != SRSLTE_SUCCESS) {
    ERROR("Error in psbch init\n");
    return SRSLTE_ERROR;
  }

  // MIB message bits
  uint8_t mib_sl_tx[SRSLTE_MIB_SL_MAX_LEN] = {};
  srslte_mib_sl_pack(&mib_sl, mib_sl_tx);

  printf("Tx payload: ");
  srslte_vec_fprint_hex(stdout, mib_sl_tx, mib_sl.mib_sl_len);

  // Put MIB-SL into PSBCH
  srslte_psbch_encode(&psbch, mib_sl_tx, mib_sl.mib_sl_len, sf_buffer);

  // prepare Rx buffer
  uint8_t mib_sl_rx[SRSLTE_MIB_SL_MAX_LEN] = {};

  // Decode PSBCH
  if (srslte_psbch_decode(&psbch, sf_buffer, mib_sl_rx, sizeof(mib_sl_rx)) == SRSLTE_SUCCESS) {
    printf("Rx payload: ");
    srslte_vec_fprint_hex(stdout, mib_sl_rx, mib_sl.mib_sl_len);

    srslte_mib_sl_unpack(&mib_sl, mib_sl_rx);
    srslte_mib_sl_printf(stdout, &mib_sl);

    // check decoded bandwidth matches user configured value
    if (srslte_mib_sl_bandwith_to_prb[mib_sl.sl_bandwidth_r12] == cell.nof_prb) {
      ret = SRSLTE_SUCCESS;
    }
  }

  // Sanity check (less REs are transmitted than mapped)
  if (cell.tm <= SRSLTE_SIDELINK_TM2) {
    if (SRSLTE_CP_ISNORM(cell.cp)) {
      // TM1 and TM2 have always 576 mapped PSBCH resource elements of which 504 are transmitted
      TESTASSERT(psbch.nof_data_re == 576);
      TESTASSERT(psbch.nof_tx_re == 504);
    } else {
      // TM1 and TM2 with extended cp have always 432 mapped PSBCH resource elements of which 360 are transmitted
      TESTASSERT(psbch.nof_data_re == 432);
      TESTASSERT(psbch.nof_tx_re == 360);
    }
  } else {
    // TM3 and TM4 have always 504 mapped PSBCH resource elements of which 432 are transmitted
    TESTASSERT(psbch.nof_data_re == 504);
    TESTASSERT(psbch.nof_tx_re == 432);
  }

  if (SRSLTE_VERBOSE_ISDEBUG()) {
    printf("PSBCH eq. symbols (%d), saving to psbch_rx_syms.bin\n", psbch.E / psbch.Qm);
    srslte_vec_fprint_c(stdout, psbch.mod_symbols, 8);
    srslte_vec_save_file("psbch_rx_syms.bin", psbch.mod_symbols, psbch.E / psbch.Qm * sizeof(cf_t));
  }

  srslte_mib_sl_free(&mib_sl);
  srslte_psbch_free(&psbch);
  free(sf_buffer);

  return ret;
}
