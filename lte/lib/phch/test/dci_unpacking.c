/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

void usage(char *prog) {
  printf("Usage: %s nof_prb length_bits Word0 Word1 ...\n", prog);
}

int main(int argc, char **argv) {
  dci_msg_t msg;
  ra_pdsch_t ra_dl;
  int len, rlen;
  int nof_prb;
  int nwords;
  int i;
  char *y;

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
  unsigned int x;
  for (i = 0; i < nwords; i++) {
    x = strtoul(argv[i + 3], NULL, 16);
    if (len - rlen < 32) {
      bit_pack(x, &y, len - rlen);
    } else {
      bit_pack(x, &y, 32);
    }

  }

  printf("DCI message len %d:\n", len);
  for (i = 0; i < len; i++) {
    printf("%d, ", msg.data[i]);
  }
  printf("\n");

  dci_msg_type_t dci_type;
  msg.location.rnti = SIRNTI;
  msg.location.nof_bits = len;
  if (dci_msg_get_type(&msg, &dci_type, nof_prb, 1234)) {
    fprintf(stderr, "Can't obtain DCI message type\n");
    exit(-1);
  }
  printf("\n");
  printf("Message type:");
  dci_msg_type_fprint(stdout, dci_type);
  switch (dci_type.type) {
  case PDSCH_SCHED:
    bzero(&ra_dl, sizeof(ra_pdsch_t));
    dci_msg_unpack_pdsch(&msg, &ra_dl, nof_prb, false);
    ra_pdsch_fprint(stdout, &ra_dl, nof_prb);
    break;
  default:
    printf("Error expected PDSCH\n");
    exit(-1);
  }
  printf("\n");
}
