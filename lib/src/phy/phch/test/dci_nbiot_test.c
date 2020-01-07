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

#include "srslte/phy/ue/ue_dl_nbiot.h"
#include "srslte/phy/utils/bit.h"

void usage(char* prog)
{
  printf("Usage: %s [cpndv]\n", prog);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cpndv")) != -1) {
    switch (opt) {
      case 'v':
        srslte_verbose++;
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
  srslte_nbiot_cell_t cell = {.base       = {.nof_prb = 1, .nof_ports = 1, .cp = SRSLTE_CP_NORM, .id = 0},
                              .nbiot_prb  = 0,
                              .n_id_ncell = 0,
                              .nof_ports  = 1,
                              .mode       = SRSLTE_NBIOT_MODE_STANDALONE};

  cf_t                 rx_buff[SRSLTE_SF_LEN_PRB_NBIOT];
  cf_t*                buff_ptrs[SRSLTE_MAX_PORTS] = {rx_buff, NULL, NULL, NULL};
  srslte_nbiot_ue_dl_t ue_dl;
  if (srslte_nbiot_ue_dl_init(&ue_dl, buff_ptrs, SRSLTE_NBIOT_MAX_PRB, SRSLTE_NBIOT_NUM_RX_ANTENNAS)) {
    fprintf(stderr, "Error initializing UE DL\n");
    return SRSLTE_ERROR;
  }

  if (srslte_nbiot_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Setting cell in UE DL\n");
    return SRSLTE_ERROR;
  }

  srslte_mib_nb_t mib;
  mib.sched_info_sib1 = 2;
  srslte_nbiot_ue_dl_set_mib(&ue_dl, mib);

  // a dummy grant
  uint8_t          dci_bits_packed[] = {0x81, 0x00, 0x00};
  srslte_dci_msg_t dci_tmp;
  dci_tmp.format   = SRSLTE_DCI_FORMATN1;
  dci_tmp.nof_bits = 23;
  srslte_bit_unpack_vector(dci_bits_packed, dci_tmp.payload, dci_tmp.nof_bits);

  // turn DCI into grant
  srslte_ra_nbiot_dl_dci_t   dl_dci;
  srslte_ra_nbiot_dl_grant_t dl_grant;
  int                        sfn    = 185;
  int                        sf_idx = 9;
  srslte_nbiot_dci_msg_to_dl_grant(&dci_tmp, 0x1234, &dl_dci, &dl_grant, sfn, sf_idx, 3, SRSLTE_NBIOT_MODE_STANDALONE);

  // make sure NPDSCH start is not on a SIB1 subframe
  srslte_nbiot_ue_dl_check_grant(&ue_dl, &dl_grant);

  // print final grant
  srslte_ra_nbiot_dl_grant_fprint(stdout, &dl_grant);

  srslte_nbiot_ue_dl_free(&ue_dl);

  // only success if starting subframe is not a SIB1 subframe
  if (dl_grant.start_sfidx != 4) {
    return SRSLTE_SUCCESS;
  }

  return SRSLTE_ERROR;
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  if (dl_timing_test() != 0) {
    printf("Error running DL timing test.\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}
