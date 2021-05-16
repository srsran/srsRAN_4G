/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/sync/pss_nr.h"
#include "srsran/phy/utils/vector.h"

/**
 * NR PSS First Subcarrier index
 */
#define PSS_NR_SUBC_BEGIN 56

/**
 * Calculates Sequence circular offset
 */
#define PSS_NR_SEQUENCE_M(N_id_2) ((43U * (N_id_2)) % SRSRAN_PSS_NR_LEN)

/**
 * Pregenerated modulated sequence
 */
static cf_t pss_nr_d[SRSRAN_PSS_NR_LEN] = {};

/**
 * Sequence generation as described in TS 38.211 clause 7.4.2.2.1
 */
__attribute__((constructor)) __attribute__((unused)) static void pss_nr_pregen()
{
  // Initialise M sequence x
  uint32_t x[SRSRAN_PSS_NR_LEN + 7];
  x[6] = 1;
  x[5] = 1;
  x[4] = 1;
  x[3] = 0;
  x[2] = 1;
  x[1] = 1;
  x[0] = 0;

  // Generate M sequence x
  for (uint32_t i = 0; i < SRSRAN_PSS_NR_LEN; i++) {
    x[i + 7] = (x[i + 4] + x[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t i = 0; i < SRSRAN_PSS_NR_LEN; i++) {
    pss_nr_d[i] = 1.0f - 2.0f * (float)x[i];
  }
}

int srsran_pss_nr_put(cf_t ssb_grid[SRSRAN_SSB_NOF_RE], uint32_t N_id_2, float beta)
{
  // Verify inputs
  if (ssb_grid == NULL || N_id_2 >= SRSRAN_NOF_NID_2_NR) {
    return SRSRAN_ERROR;
  }

  // Calculate generation parameters
  uint32_t m          = PSS_NR_SEQUENCE_M(N_id_2);
  uint32_t copy_sz_1  = SRSRAN_PSS_NR_LEN - m;
  uint32_t grid_idx_1 = SRSRAN_PSS_NR_SYMBOL_IDX * SRSRAN_SSB_BW_SUBC + PSS_NR_SUBC_BEGIN;
  uint32_t grid_idx_2 = grid_idx_1 + copy_sz_1;

  // Copy sequence from offset to the end
  srsran_vec_sc_prod_cfc(&pss_nr_d[m], beta, &ssb_grid[grid_idx_1], copy_sz_1);

  // Copy sequence from 0 to offset
  srsran_vec_sc_prod_cfc(&pss_nr_d[0], beta, &ssb_grid[grid_idx_2], m);

  return SRSRAN_SUCCESS;
}

int srsran_pss_nr_extract_lse(const cf_t* ssb_grid, uint32_t N_id_2, cf_t lse[SRSRAN_PSS_NR_LEN])
{
  // Verify inputs
  if (ssb_grid == NULL || N_id_2 >= SRSRAN_NOF_NID_2_NR || lse == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Extract PSS
  srsran_vec_cf_copy(
      lse, &ssb_grid[SRSRAN_PSS_NR_SYMBOL_IDX * SRSRAN_SSB_BW_SUBC + PSS_NR_SUBC_BEGIN], SRSRAN_PSS_NR_LEN);

  // Estimate
  uint32_t m = PSS_NR_SEQUENCE_M(N_id_2);
  srsran_vec_prod_ccc(&pss_nr_d[m], lse, lse, SRSRAN_PSS_NR_LEN - m);
  srsran_vec_prod_ccc(&pss_nr_d[0], &lse[SRSRAN_PSS_NR_LEN - m], &lse[SRSRAN_PSS_NR_LEN - m], m);

  return SRSRAN_SUCCESS;
}
