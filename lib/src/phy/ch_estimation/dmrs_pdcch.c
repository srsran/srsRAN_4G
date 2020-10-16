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

#include "srslte/phy/ch_estimation/dmrs_pdcch.h"

/// @brief Every frequency resource is 6 Resource blocks, every resource block carries 3 pilots. So 18 possible pilots
/// per frequency resource.
#define NOF_PILOTS_X_FREQ_RES 18

static uint32_t dmrs_pdcch_get_cinit(uint32_t slot_idx, uint32_t symbol_idx, uint32_t n_id)
{
  return (uint32_t)((((SRSLTE_NSYMB_PER_SLOT_NR * slot_idx + symbol_idx + 1UL) << 17UL) * (2 * n_id + 1) + 2 * n_id) &
                    (uint64_t)INT32_MAX);
}

static void dmrs_pdcch_put_symbol_noninterleaved(const srslte_pdcch_cfg_nr_t* cfg, uint32_t cinit, cf_t* sf_symbol)
{
  uint32_t L            = 1U << cfg->aggregation_level;
  uint32_t nof_freq_res = SRSLTE_MIN(cfg->carrier.nof_prb / 6, SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE);

  // Initialise sequence for this symbol
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);
  uint32_t sequence_skip = 0; // Accumulates pilot locations to skip

  // Calculate Resource block indexes range, every CCE is 6 REG, 1 REG is 6 RE in resource blocks
  uint32_t rb_coreset_idx_begin = (cfg->n_cce * 6) / cfg->coreset.duration;
  uint32_t rb_coreset_idx_end   = ((cfg->n_cce + L) * 6) / cfg->coreset.duration;

  // CORESET Resource Block counter
  uint32_t rb_coreset_idx = 0;
  for (uint32_t i = 0; i < nof_freq_res; i++) {
    // Skip frequency resource if outside of the CORESET
    if (!cfg->coreset.freq_resources[i]) {
      // Skip possible DMRS locations in this region
      sequence_skip += NOF_PILOTS_X_FREQ_RES;
      continue;
    }

    // Skip if the frequency resource highest RB is lower than the first CCE resource block.
    if ((rb_coreset_idx + 6) <= rb_coreset_idx_begin) {
      // Skip possible DMRS locations in this region
      sequence_skip += NOF_PILOTS_X_FREQ_RES;

      // Since this is part of the CORESET, count the RB as CORESET
      rb_coreset_idx += 6;
      continue;
    }

    // Return if the first RB of the frequency resource is greater than the last CCE resource block
    if (rb_coreset_idx > rb_coreset_idx_end) {
      return;
    }

    // Skip all discarded possible pilot locations
    srslte_sequence_state_advance(&sequence_state, 2 * sequence_skip);
    sequence_skip = 0;

    // Generate pilots
    cf_t rl[NOF_PILOTS_X_FREQ_RES];
    srslte_sequence_state_gen_f(&sequence_state, M_SQRT1_2, (float*)rl, NOF_PILOTS_X_FREQ_RES * 2);

    // For each RB in the frequency resource
    for (uint32_t j = 0; j < 6; j++) {
      // Calculate absolute RB index
      uint32_t n = i * 6 + j;

      // Skip if lower than begin
      if (rb_coreset_idx < rb_coreset_idx_begin) {
        rb_coreset_idx++;
        continue;
      }

      // Return if greater than end
      if (rb_coreset_idx >= rb_coreset_idx_end) {
        return;
      }

      // Write pilots in the symbol
      for (uint32_t k_prime = 0; k_prime < 3; k_prime++) {
        // Calculate sub-carrier index
        uint32_t k = n * SRSLTE_NRE + 4 * k_prime + 1;

        sf_symbol[k] = rl[3 * j + k_prime];
      }
      rb_coreset_idx++;
    }
  }
}

int srslte_dmrs_pdcch_put(const srslte_pdcch_cfg_nr_t* cfg, uint32_t slot_idx, cf_t* sf_symbols)
{
  if (cfg == NULL || sf_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (cfg->coreset.mapping_type == srslte_coreset_mapping_type_interleaved) {
    ERROR("Error interleaved CORESET mapping is not currently implemented\n");
    return SRSLTE_ERROR;
  }

  if (cfg->coreset.duration < SRSLTE_CORESET_DURATION_MIN || cfg->coreset.duration > SRSLTE_CORESET_DURATION_MAX) {
    ERROR("Error CORESET duration %d is out-of-bounds (%d,%d)\n",
          cfg->coreset.duration,
          SRSLTE_CORESET_DURATION_MIN,
          SRSLTE_CORESET_DURATION_MAX);
    return SRSLTE_ERROR;
  }

  // Use cell id if the DMR scrambling id is not provided by higher layers
  uint32_t n_id = cfg->carrier.id;
  if (cfg->coreset.dmrs_scrambling_id_present) {
    n_id = cfg->coreset.dmrs_scrambling_id;
  }

  for (uint32_t l = 0; l < cfg->coreset.duration; l++) {
    // Get Cin
    uint32_t cinit = dmrs_pdcch_get_cinit(slot_idx, l, n_id);

    // Put data
    dmrs_pdcch_put_symbol_noninterleaved(cfg, cinit, &sf_symbols[cfg->carrier.nof_prb * SRSLTE_NRE * l]);
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_estimator_init(srslte_dmrs_pdcch_estimator_t* q,
                                     const srslte_carrier_nr_t*     carrier,
                                     const srslte_coreset_t*        coreset)
{
  if (q == NULL || carrier == NULL || coreset == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (coreset->duration < SRSLTE_CORESET_DURATION_MIN || coreset->duration > SRSLTE_CORESET_DURATION_MAX) {
    ERROR("Error CORESET duration %d is out-of-bounds (%d,%d)\n",
          coreset->duration,
          SRSLTE_CORESET_DURATION_MIN,
          SRSLTE_CORESET_DURATION_MAX);
    return SRSLTE_ERROR;
  }

  // The carrier configuration is not used for initialization, so copy it always
  q->carrier = *carrier;

  // Detect change in CORESET, if none, return early
  if (memcmp(&q->coreset, coreset, sizeof(srslte_coreset_t)) == 0) {
    return SRSLTE_SUCCESS;
  }

  // Save new CORESET
  q->coreset = *coreset;

  // The interpolator may return without reconfiguring after the first call
  if (q->interpolator.M != 0) {
    srslte_interp_linear_free(&q->interpolator);
  }

  if (srslte_interp_linear_init(&q->interpolator, srslte_coreset_get_bw(coreset) * 3, 4)) {
    ERROR("Initiating interpolator\n");
    return SRSLTE_ERROR;
  }

  // Calculate new bandwidth and size
  uint32_t coreset_bw = srslte_coreset_get_bw(coreset);
  uint32_t coreset_sz = srslte_coreset_get_sz(coreset);

  // Reallocate only if the CORESET size or bandwidth changed
  if (q->coreset_bw != coreset_bw || q->coreset_sz != coreset_sz) {
    // Iterate all possible symbols
    for (uint32_t l = 0; l < SRSLTE_CORESET_DURATION_MAX; l++) {
      // Free if allocated
      if (q->lse[l] != NULL) {
        free(q->lse[l]);
        q->lse[l] = NULL;
      }

      // Allocate
      if (l < coreset->duration) {
        // Allocate for 3 pilots per physical resource block
        q->lse[l] = srslte_vec_cf_malloc(coreset_bw * 3);
      }
    }

    if (q->ce) {
      free(q->ce);
    }
    q->ce = srslte_vec_cf_malloc(coreset_sz);
  }

  // Save new calculated values
  q->coreset_bw = coreset_bw;
  q->coreset_sz = coreset_sz;

  return SRSLTE_SUCCESS;
}

void srslte_dmrs_pdcch_estimator_free(srslte_dmrs_pdcch_estimator_t* q)
{
  if (q == NULL) {
    return;
  }

  if (q->ce) {
    free(q->ce);
  }

  for (uint32_t i = 0; i < SRSLTE_CORESET_DURATION_MAX; i++) {
    if (q->lse[i]) {
      free(q->lse[i]);
    }
  }

  srslte_interp_linear_free(&q->interpolator);

  memset(q, 0, sizeof(srslte_dmrs_pdcch_estimator_t));
}

static void
srslte_dmrs_pdcch_extract(srslte_dmrs_pdcch_estimator_t* q, uint32_t cinit, const cf_t* sf_symbol, cf_t* lse)
{
  // Initialise pseudo-random sequence
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);

  // Counter for skipping unused pilot locations
  uint32_t sequence_skip = 0;

  // Counts enabled frequency domain resources
  uint32_t rb_coreset_idx = 0;
  // Iterate over all possible frequency resources
  for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    // Skip disabled frequency resources
    if (!q->coreset.freq_resources[i]) {
      sequence_skip += NOF_PILOTS_X_FREQ_RES;
      continue;
    }

    // Skip unused pilots
    srslte_sequence_state_advance(&sequence_state, 2 * sequence_skip);
    sequence_skip = 0;

    // Generate sequence
    cf_t rl[NOF_PILOTS_X_FREQ_RES];
    srslte_sequence_state_gen_f(&sequence_state, M_SQRT1_2, (float*)rl, NOF_PILOTS_X_FREQ_RES * 2);

    // Iterate all PRBs in the enabled frequency domain resource
    for (uint32_t j = 0, idx = rb_coreset_idx * NOF_PILOTS_X_FREQ_RES; j < 6; j++) {
      // Calculate Grid PRB index (n)
      uint32_t n = i * 6 + j;

      // For each pilot in the PRB
      for (uint32_t k_prime = 0; k_prime < 3; k_prime++, idx++) {
        // Calculate sub-carrier index
        uint32_t k = n * SRSLTE_NRE + 4 * k_prime + 1;

        // Extract symbol
        lse[idx] = sf_symbol[k];
      }
    }

    // Calculate least squared estimates
    cf_t* lse_ptr = &lse[rb_coreset_idx * NOF_PILOTS_X_FREQ_RES];
    srslte_vec_prod_conj_ccc(lse_ptr, rl, lse_ptr, NOF_PILOTS_X_FREQ_RES);

    // Increment frequency domain resource counter
    rb_coreset_idx++;
  }
}

int srslte_dmrs_pdcch_estimate(srslte_dmrs_pdcch_estimator_t* q, uint32_t slot_idx, const cf_t* sf_symbols)
{
  if (q == NULL || sf_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Saves slot index for posterior use
  q->slot_idx = slot_idx;

  // Use cell id if the DMR scrambling id is not provided by higher layers
  uint32_t n_id = q->carrier.id;
  if (q->coreset.dmrs_scrambling_id_present) {
    n_id = q->coreset.dmrs_scrambling_id;
  }

  // Extract pilots
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    // Calculate PRN sequence initial state
    uint32_t cinit = dmrs_pdcch_get_cinit(slot_idx, l, n_id);

    // Extract pilots least square estimates
    srslte_dmrs_pdcch_extract(q, cinit, &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], q->lse[l]);
  }

  // Time averaging and smoothing should be implemented here
  // ...

  // Interpolation, it assumes all frequency domain resources are contiguous
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    srslte_interp_linear_offset(&q->interpolator, q->lse[l], &q->ce[SRSLTE_NRE * q->coreset_bw * l], 1, 3);
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_get_measure(srslte_dmrs_pdcch_estimator_t* q,
                                  const srslte_search_space_t*   search_space,
                                  uint32_t                       slot_idx,
                                  uint32_t                       aggregation_level,
                                  uint32_t                       ncce,
                                  uint16_t                       rnti,
                                  srslte_dmrs_pdcch_measure_t*   measure)
{
  if (q == NULL || search_space == NULL || measure == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t L = 1U << aggregation_level;
  if (q->coreset.mapping_type == srslte_coreset_mapping_type_interleaved) {
    ERROR("Error interleaved mapping not implemented\n");
    return SRSLTE_ERROR;
  }

  // Check that CORESET duration is not less than minimum
  if (q->coreset.duration < SRSLTE_CORESET_DURATION_MIN) {
    ERROR("Invalid CORESET duration\n");
    return SRSLTE_ERROR;
  }

  // Get base pilot;
  uint32_t pilot_idx  = (ncce * 18) / q->coreset.duration;
  uint32_t nof_pilots = (L * 18) / q->coreset.duration;

  float rsrp                              = 0.0f;
  float epre                              = 0.0f;
  float cfo                               = 0.0f;
  float sync_err                          = 0.0f;
  cf_t  corr[SRSLTE_CORESET_DURATION_MAX] = {};
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    // Correlate DMRS
    corr[l] = srslte_vec_acc_cc(&q->lse[l][pilot_idx], nof_pilots) / (float)nof_pilots;

    // Measure symbol RSRP
    rsrp += cabsf(corr[l]);

    // Measure symbol EPRE
    epre += srslte_vec_avg_power_cf(&q->lse[l][pilot_idx], nof_pilots);

    // Measure CFO only from the second and third symbols
    if (l != 0) {
      cfo += cargf(corr[l] * conjf(corr[l - 1]));
    }

    // Measure synchronization error
    sync_err += srslte_vec_estimate_frequency(&q->lse[l][pilot_idx], nof_pilots);
  }

  if (q->coreset.duration > 1) {
    cfo /= (float)(q->coreset.duration - 1);
  }

  // Symbol time, including cyclic prefix. Required for CFO estimation
  float Ts = (71.3541666667f / (float)(1 << q->carrier.numerology));

  measure->rsrp   = rsrp / (float)q->coreset.duration;
  measure->epre   = epre / (float)q->coreset.duration;
  measure->cfo_hz = cfo / (2.0f * (float)M_PI * Ts);
  measure->sync_error_us =
      (float)SRSLTE_SUBC_SPACING_NR(q->carrier.numerology) * sync_err / (4.0e-6f * (float)q->coreset.duration);

  return SRSLTE_SUCCESS;
}