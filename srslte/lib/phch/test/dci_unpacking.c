/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/srslte.h"

void usage(char *prog) {
  printf("Usage: %s nof_prb length_bits Word0 Word1 ...\n", prog);
}

int main(int argc, char **argv) {
  srslte_dci_msg_t msg;
  srslte_ra_dl_dci_t ra_dl;
  int len, rlen;
  int nof_prb;
  int nwords;
  int i;
  uint8_t *y;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  nof_prb = atoi(argv[1]);
  len = atoi(argv[2]);

  nwords = (len - 1) / 32 + 1;

  if (argc < 3 + nwords) {
    usage(argv[0]);
    exit(-1);
  }

  y = msg.data;
  rlen = 0;
  uint32_t x;
  for (i = 0; i < nwords; i++) {
    x = strtoul(argv[i + 3], NULL, 16);
    if (len - rlen < 32) {
      srslte_bit_unpack(x, &y, len - rlen);
    } else {
      srslte_bit_unpack(x, &y, 32);
    }

  }

  printf("DCI message len %d:\n", len);
  for (i = 0; i < len; i++) {
    printf("%d, ", msg.data[i]);
  }
  printf("\n");

  srslte_dci_msg_type_t dci_type;
  msg.nof_bits = len;
  if (srslte_dci_msg_get_type(&msg, &dci_type, nof_prb, SRSLTE_SIRNTI)) {
    fprintf(stderr, "Can't obtain DCI message type\n");
    exit(-1);
  }
  printf("\n");
  printf("Message type:");
  srslte_dci_msg_type_fprint(stdout, dci_type);
  switch (dci_type.type) {
  case SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED:
    bzero(&ra_dl, sizeof(srslte_ra_dl_dci_t));
    srslte_dci_msg_unpack_pdsch(&msg, &ra_dl, nof_prb, false);
    srslte_ra_pdsch_fprint(stdout, &ra_dl, nof_prb);
    break;
  default:
    printf("Error expected PDSCH\n");
    exit(-1);
  }
  printf("\n");
}
