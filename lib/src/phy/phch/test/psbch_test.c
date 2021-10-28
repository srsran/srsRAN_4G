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
#include <strings.h>
#include <unistd.h>

#include "srsran/common/test_common.h"
#include <srsran/phy/phch/mib_sl.h>
#include <srsran/phy/phch/psbch.h>
#include <srsran/phy/utils/debug.h>
#include <srsran/phy/utils/vector.h>

srsran_cell_sl_t cell = {.nof_prb = 6, .N_sl_id = 168, .tm = SRSRAN_SIDELINK_TM2, .cp = SRSRAN_CP_NORM};

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-e extended CP [Default normal]\n");
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
      case 'e':
        cell.cp = SRSRAN_CP_EXT;
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
        increase_srsran_verbose_level();
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (SRSRAN_CP_ISEXT(cell.cp) && cell.tm >= SRSRAN_SIDELINK_TM3) {
    ERROR("Selected TM does not support extended CP");
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  parse_args(argc, argv);

  uint32_t sf_n_re   = SRSRAN_SF_LEN_RE(cell.nof_prb, cell.cp);
  cf_t*    sf_buffer = srsran_vec_cf_malloc(sf_n_re);

  // MIB-SL
  srsran_mib_sl_t mib_sl;
  srsran_mib_sl_init(&mib_sl, cell.tm);
  srsran_mib_sl_set(&mib_sl, cell.nof_prb, 0, 128, 4, false);

  // PSBCH
  srsran_psbch_t psbch;
  if (srsran_psbch_init(&psbch, cell.nof_prb, cell.N_sl_id, cell.tm, cell.cp) != SRSRAN_SUCCESS) {
    ERROR("Error in psbch init");
    return SRSRAN_ERROR;
  }

  // MIB message bits
  uint8_t mib_sl_tx[SRSRAN_MIB_SL_MAX_LEN] = {};
  srsran_mib_sl_pack(&mib_sl, mib_sl_tx);

  printf("Tx payload: ");
  srsran_vec_fprint_hex(stdout, mib_sl_tx, mib_sl.mib_sl_len);

  // Put MIB-SL into PSBCH
  srsran_psbch_encode(&psbch, mib_sl_tx, mib_sl.mib_sl_len, sf_buffer);

  // prepare Rx buffer
  uint8_t mib_sl_rx[SRSRAN_MIB_SL_MAX_LEN] = {};

  // Decode PSBCH
  if (srsran_psbch_decode(&psbch, sf_buffer, mib_sl_rx, sizeof(mib_sl_rx)) == SRSRAN_SUCCESS) {
    printf("Rx payload: ");
    srsran_vec_fprint_hex(stdout, mib_sl_rx, mib_sl.mib_sl_len);

    srsran_mib_sl_unpack(&mib_sl, mib_sl_rx);
    srsran_mib_sl_printf(stdout, &mib_sl);

    // check decoded bandwidth matches user configured value
    if (srsran_mib_sl_bandwith_to_prb[mib_sl.sl_bandwidth_r12] == cell.nof_prb) {
      ret = SRSRAN_SUCCESS;
    }
  }

  // Sanity check (less REs are transmitted than mapped)
  if (cell.tm <= SRSRAN_SIDELINK_TM2) {
    if (SRSRAN_CP_ISNORM(cell.cp)) {
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

  if (SRSRAN_VERBOSE_ISDEBUG()) {
    printf("PSBCH eq. symbols (%d), saving to psbch_rx_syms.bin\n", psbch.E / psbch.Qm);
    srsran_vec_fprint_c(stdout, psbch.mod_symbols, 8);
    srsran_vec_save_file("psbch_rx_syms.bin", psbch.mod_symbols, psbch.E / psbch.Qm * sizeof(cf_t));
  }

  srsran_mib_sl_free(&mib_sl);
  srsran_psbch_free(&psbch);
  free(sf_buffer);

  return ret;
}
