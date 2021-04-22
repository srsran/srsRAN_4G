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

#include "srsran/phy/ue/ue_dl_nbiot.h"
#include "srsran/phy/utils/bit.h"

void usage(char* prog)
{
  printf("Usage: %s [cpndv]\n", prog);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cpndv")) != -1) {
    switch (opt) {
      case 'v':
        srsran_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

// This timing test is supposed to test that a NPDSCH that
// is supposed to start on a SIB1 subframe is in fact postponed
// to the next valid DL subframe.
int dl_timing_test()
{
  srsran_nbiot_cell_t cell = {.base       = {.nof_prb = 1, .nof_ports = 1, .cp = SRSRAN_CP_NORM, .id = 0},
                              .nbiot_prb  = 0,
                              .n_id_ncell = 0,
                              .nof_ports  = 1,
                              .mode       = SRSRAN_NBIOT_MODE_STANDALONE};

  cf_t                 rx_buff[SRSRAN_SF_LEN_PRB_NBIOT];
  cf_t*                buff_ptrs[SRSRAN_MAX_PORTS] = {rx_buff, NULL, NULL, NULL};
  srsran_nbiot_ue_dl_t ue_dl                       = {};
  if (srsran_nbiot_ue_dl_init(&ue_dl, buff_ptrs, SRSRAN_NBIOT_MAX_PRB, SRSRAN_NBIOT_NUM_RX_ANTENNAS)) {
    fprintf(stderr, "Error initializing UE DL\n");
    return SRSRAN_ERROR;
  }

  if (srsran_nbiot_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Setting cell in UE DL\n");
    return SRSRAN_ERROR;
  }

  srsran_mib_nb_t mib = {};
  mib.sched_info_sib1 = 2;
  srsran_nbiot_ue_dl_set_mib(&ue_dl, mib);

  // a dummy grant
  uint8_t          dci_bits_packed[] = {0x81, 0x00, 0x00};
  srsran_dci_msg_t dci_tmp           = {};
  dci_tmp.format   = SRSRAN_DCI_FORMATN1;
  dci_tmp.nof_bits = 23;
  srsran_bit_unpack_vector(dci_bits_packed, dci_tmp.payload, dci_tmp.nof_bits);

  // turn DCI into grant
  srsran_ra_nbiot_dl_dci_t   dl_dci;
  srsran_ra_nbiot_dl_grant_t dl_grant;
  int                        sfn    = 185;
  int                        sf_idx = 9;
  srsran_nbiot_dci_msg_to_dl_grant(&dci_tmp, 0x1234, &dl_dci, &dl_grant, sfn, sf_idx, 3, SRSRAN_NBIOT_MODE_STANDALONE);

  // make sure NPDSCH start is not on a SIB1 subframe
  srsran_nbiot_ue_dl_check_grant(&ue_dl, &dl_grant);

  // print final grant
  srsran_ra_nbiot_dl_grant_fprint(stdout, &dl_grant);

  srsran_nbiot_ue_dl_free(&ue_dl);

  // only success if starting subframe is not a SIB1 subframe
  if (dl_grant.start_sfidx != 4) {
    return SRSRAN_SUCCESS;
  }

  return SRSRAN_ERROR;
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  if (dl_timing_test() != 0) {
    printf("Error running DL timing test.\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}
