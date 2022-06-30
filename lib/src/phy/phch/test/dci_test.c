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

#include "srsran/phy/phch/dci.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include "srsran/srsran.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>

#define UE_CRNTI 0x1234

static int test_pdcch_orders()
{
  static srsran_cell_t cell = {
      52,                // nof_prb
      1,                 // nof_ports
      0,                 // cell_id
      SRSRAN_CP_NORM,    // cyclic prefix
      SRSRAN_PHICH_NORM, // PHICH length
      SRSRAN_PHICH_R_1,  // PHICH resources
      SRSRAN_FDD,
  };

  srsran_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  srsran_dci_location_t locations[SRSRAN_NOF_SF_X_FRAME][30];
  static uint32_t       cfi = 2;

  static srsran_pdcch_t pdcch;
  static srsran_regs_t  regs;

  if (srsran_regs_init(&regs, cell)) {
    ERROR("Error initiating regs");
    exit(-1);
  }

  if (srsran_pdcch_init_enb(&pdcch, cell.nof_prb)) {
    ERROR("Error creating PDCCH object");
    exit(-1);
  }
  if (srsran_pdcch_set_cell(&pdcch, &regs, cell)) {
    ERROR("Error creating PDCCH object");
    exit(-1);
  }

  /* Initiate valid DCI locations */
  for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
    dl_sf.cfi = cfi;
    dl_sf.tti = i;
    srsran_pdcch_ue_locations(&pdcch, &dl_sf, locations[i], 30, UE_CRNTI);
  }

  srsran_dci_dl_t dci_tx;
  bzero(&dci_tx, sizeof(srsran_dci_dl_t));
  dci_tx.rnti           = UE_CRNTI;
  dci_tx.location       = locations[1][0];
  dci_tx.format         = SRSRAN_DCI_FORMAT1A;
  dci_tx.cif_present    = false;
  dci_tx.is_pdcch_order = true;
  dci_tx.preamble_idx   = 0;
  dci_tx.prach_mask_idx = 0;

  srsran_dci_cfg_t cfg    = {};
  cfg.cif_enabled         = false;
  cfg.srs_request_enabled = false;

  // Pack
  srsran_dci_msg_t dci_msg = {};
  TESTASSERT(srsran_dci_msg_pack_pdsch(&cell, &dl_sf, &cfg, &dci_tx, &dci_msg) == SRSRAN_SUCCESS);

  // Unpack
  srsran_dci_dl_t dci_rx = {};
  TESTASSERT(srsran_dci_msg_unpack_pdsch(&cell, &dl_sf, &cfg, &dci_msg, &dci_rx) == SRSRAN_SUCCESS);

  // To string
  char str[128];
  srsran_dci_dl_info(&dci_tx, str, sizeof(str));
  printf("Tx: %s\n", str);
  srsran_dci_dl_info(&dci_rx, str, sizeof(str));
  printf("Rx: %s\n", str);

  // Assert
  TESTASSERT(memcmp(&dci_tx, &dci_rx, srsran_dci_format_sizeof(&cell, &dl_sf, &cfg, dci_tx.format)) == 0);

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  if (test_pdcch_orders() != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  printf("Success!\n");

  return SRSRAN_SUCCESS;
}