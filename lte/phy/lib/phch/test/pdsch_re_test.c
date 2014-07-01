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
#include <stdbool.h>

#include "liblte/phy/phy.h"

#define N_TESTS 10


const lte_cp_t test_re_cp[N_TESTS] = {CPNORM, CPNORM, CPNORM, CPNORM, CPNORM, CPNORM, CPEXT, CPEXT, CPEXT, CPEXT};
const int test_re_ports[N_TESTS] = {1, 1, 1, 2, 4, 4, 1, 4, 1, 4};
const int test_re_csymb[N_TESTS] = {2, 1, 3, 3, 1, 3, 2, 2, 1, 2};
const int test_re_prb[N_TESTS] = {6, 15, 15, 15, 15, 15, 6, 6, 15, 15};
const int test_re_num[N_TESTS][3] = {
    {408, 684, 828 }, // sf 0, 5 and the rest
    {1830, 2106, 2250},
    {1470, 1746, 1890},
    {1392, 1656, 1800},
    {1656, 1896, 2040},
    {1356, 1596, 1740},
    {276, 540, 684},
    {264, 480, 624},
    {1482, 1746, 1890},
    {1200, 1416, 1560}
};

cf_t in[200000], out[200000];

int main(int argc, char **argv) {
  int i, n, np, r;
  ra_prb_t prb_alloc;
  int ret = -1;
  pdsch_t pdsch;
  lte_cell_t cell;
  
  while (getopt(argc, argv, "v") == 'v') {
    verbose++;
  }

  for (i=0;i<110;i++) {
    prb_alloc.slot[0].prb_idx[i] = i;
    prb_alloc.slot[1].prb_idx[i] = i;
  }

  for (i=0;i<N_TESTS;i++) {
    cell.id = 0;
    cell.nof_prb = test_re_prb[i];
    cell.nof_ports = test_re_ports[i];
    cell.cp = test_re_cp[i];

    pdsch_init(&pdsch, 0, cell);

    memset(prb_alloc.re_sf, 0, sizeof(uint32_t) * 10);
    prb_alloc.slot[0].nof_prb = test_re_prb[i];
    prb_alloc.slot[1].nof_prb = test_re_prb[i];
    ra_prb_get_re_dl(&prb_alloc, test_re_prb[i], test_re_ports[i], test_re_csymb[i], test_re_cp[i]);
    for (n=0;n<10;n++) {
      switch(n) {
      case 0:
        np = 0;
        break;
      case 5:
        np = 1;
        break;
      default:
        np = 2;
        break;
      }
      r = pdsch_get(&pdsch, in, out, &prb_alloc, n);
      
      if (prb_alloc.re_sf[n] != test_re_num[i][np]) {
        goto go_out;
      }
      if (r != prb_alloc.re_sf[n]) {
        goto go_out;
      }
    }
    pdsch_free(&pdsch);
  }
  ret = 0;
go_out:
  if (ret) {
    printf("Error in SF %d test %d. %d PRB, %d ports, RE is %d, get %d and should be %d\n",
        n, i, test_re_prb[i], test_re_ports[i], prb_alloc.re_sf[n], r, test_re_num[i][np]);
  } else {
    printf("Ok\n");
  }
  exit(ret);
}


