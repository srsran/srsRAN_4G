/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

/*
 * Checks the NSSS sequences against 3GPP TS 36.211 Sec. 10.2.7.2.1:
 *
 *   d(n) = b_q(m) * exp(-j*2*pi*theta_f*n) * exp(-j*pi*u*n'*(n'+1)/131)
 *   theta_f = 33/132 * ((n_f/2) mod 4)
 *
 * Since 33/132 = 1/4, the cyclic shift term equals (-j)^(k*n) with k = (n_f/2) mod 4.
 */

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "srsran/phy/sync/nsss.h"
#include "srsran/srsran.h"

// The Zadoff-Chu term is evaluated in single precision (max. error ~2e-3); a wrong cyclic shift gives an error
// of at least sqrt(2), so this tolerance separates both cases with a wide margin.
#define TOL 1e-2

static const double complex quarter_turn[4] = {1.0, -I, -1.0, I}; // (-j)^0..3

static int failures = 0;

static void check(int cond, const char* what, uint32_t cell_id, int k, int n)
{
  if (!cond) {
    if (failures < 20) {
      printf("FAIL: %s (cell_id=%d, k=%d, n=%d)\n", what, cell_id, k, n);
    }
    failures++;
  }
}

// Expected d(n) for cyclic shift index k, computed directly from the specification
static double complex nsss_expected(uint32_t cell_id, int k, int n)
{
  int u       = cell_id % 126 + 3;
  int q       = cell_id / 126;
  int n_prime = n % 131;
  int m       = n % 128;
  return b_q_m[q][m] * quarter_turn[(k * n) % 4] * cexp(-I * M_PI * u * n_prime * (n_prime + 1) / 131.0);
}

static void test_generate(uint32_t cell_id)
{
  cf_t seq[SRSRAN_NSSS_TOT_LEN] = {};
  srsran_nsss_generate(seq, cell_id);

  for (int k = 0; k < SRSRAN_NSSS_NUM_SEQ; k++) {
    for (int n = 0; n < SRSRAN_NSSS_LEN; n++) {
      double complex expected = nsss_expected(cell_id, k, n);
      double complex actual   = seq[k * SRSRAN_NSSS_LEN + n];
      check(cabs(actual - expected) < TOL, "generated sequence differs from TS 36.211", cell_id, k, n);
    }
  }
}

// NSSS is mapped by srsran_nsss_put_subframe() for nof_prb = 1 and PRB offset 0 to the 12 subcarriers of
// OFDM symbols 3..13, i.e. to the contiguous resource elements 36..167 of the subframe grid.
static void test_put_subframe(uint32_t cell_id)
{
  const uint32_t      nof_prb                  = 1;
  const uint32_t      first_re                 = 3 * SRSRAN_NRE;
  cf_t                seq[SRSRAN_NSSS_TOT_LEN] = {};
  srsran_nsss_synch_t q                        = {};
  srsran_nsss_generate(seq, cell_id);

  for (int nf = 0; nf < 16; nf += 2) {
    cf_t grid[SRSRAN_CP_NORM_SF_NSYMB * SRSRAN_NRE] = {};
    srsran_nsss_put_subframe(&q, seq, grid, nf, nof_prb, 0);

    int k = (nf / 2) % 4;
    for (int n = 0; n < SRSRAN_NSSS_LEN; n++) {
      double complex expected = nsss_expected(cell_id, k, n);
      check(cabs(grid[first_re + n] - expected) < TOL, "wrong cyclic shift mapped for n_f", cell_id, nf, n);
    }
  }
}

int main(void)
{
  // cover q = 0..3 and both ends of the root index range
  const uint32_t cell_ids[] = {0, 1, 124, 125, 126, 251, 252, 377, 378, 502, 503};

  for (uint32_t i = 0; i < sizeof(cell_ids) / sizeof(cell_ids[0]); i++) {
    test_generate(cell_ids[i]);
    test_put_subframe(cell_ids[i]);
  }

  if (failures) {
    printf("nsss_seq_test: %d check(s) failed\n", failures);
    return SRSRAN_ERROR;
  }
  printf("nsss_seq_test: Ok\n");
  return SRSRAN_SUCCESS;
}
