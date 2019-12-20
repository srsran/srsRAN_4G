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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/common/test_common.h"
#include <srslte/phy/phch/mib_sl.h>
#include <srslte/phy/phch/psbch.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/vector.h>

int32_t        N_sl_id = 168;
srslte_cp_t    cp      = SRSLTE_CP_NORM;
uint32_t       nof_prb = 6;
srslte_sl_tm_t tm      = SRSLTE_SIDELINK_TM2;

void usage(char* prog)
{
  printf("Usage: %s [cdeipt]\n", prog);
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-e extended CP [Default normal]\n");
  printf("\t-c N_sl_id [Default %d]\n", N_sl_id);
  printf("\t-t Sidelink transmission mode {1,2,3,4} [Default %d]\n", (tm + 1));
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "ceiptv")) != -1) {
    switch (opt) {
      case 'c':
        N_sl_id = (int32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cp = SRSLTE_CP_EXT;
        break;
      case 'p':
        nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
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
}

int main(int argc, char** argv)
{
  int ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  uint32_t sf_n_re   = SRSLTE_SF_LEN_RE(nof_prb, cp);
  cf_t*    sf_buffer = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);

  // MIB-SL
  srslte_mib_sl_t mib_sl;
  srslte_mib_sl_init(&mib_sl, tm);
  srslte_mib_sl_set(&mib_sl, nof_prb, 0, 128, 4, false);

  // PSBCH
  srslte_psbch_t psbch;
  srslte_psbch_init(&psbch, nof_prb, N_sl_id, tm, SRSLTE_CP_NORM);

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

    ret = SRSLTE_SUCCESS;
  }

  // Sanity check
  if (tm <= SRSLTE_SIDELINK_TM2) {
    // TM1 and TM2 have always 504 PSBCH resource elements
    TESTASSERT(psbch.E / psbch.Qm == 504);
  } else {
    // TM3 and TM4 have always 432 PSBCH resource elements
    TESTASSERT(psbch.E / psbch.Qm == 432);
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
