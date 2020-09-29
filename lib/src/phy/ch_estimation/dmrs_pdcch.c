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

uint32_t srslte_pdcch_calculate_Y_p_n(uint32_t coreset_id, uint16_t rnti, int n)
{
  const uint32_t A_p[3] = {39827, 39829, 39839};
  const uint32_t D      = 65537;

  if (n < 0) {
    return rnti;
  }

  return (A_p[coreset_id % 3] * srslte_pdcch_calculate_Y_p_n(coreset_id, rnti, n - 1)) % D;
}

/**
 * Calculates the Control Channnel Element As described in 3GPP 38.213 R15 10.1 UE procedure for determining physical
 * downlink control channel assignment
 *
 */
static int srslte_pdcch_get_ncce(const srslte_coreset_t*      coreset,
                                 const srslte_search_space_t* search_space,
                                 uint16_t                     rnti,
                                 uint32_t                     aggregation_level,
                                 uint32_t                     candidate,
                                 const uint32_t               slot_idx)
{
  if (aggregation_level >= SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS) {
    ERROR("Invalid aggregation level %d;\n", aggregation_level);
    return SRSLTE_ERROR;
  }

  uint32_t L    = 1U << aggregation_level;                         // Aggregation level
  uint32_t n_ci = 0;                                               //  Carrier indicator field
  uint32_t m    = candidate;                                       // Selected PDDCH candidate
  uint32_t M    = search_space->nof_candidates[aggregation_level]; // Number of aggregation levels

  if (M == 0) {
    ERROR("Invalid number of candidates %d for aggregation level %d\n", M, aggregation_level);
    return SRSLTE_ERROR;
  }

  // Count number of REG
  uint32_t N_cce = 0;
  for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    // Every frequency domain resource is 6 PRB, a REG is 1PRB wide and a CCE is 6 REG. So, for every frequency domain
    // resource there is one CCE.
    N_cce += coreset->freq_resources[i] ? coreset->duration : 0;
  }

  if (N_cce < L) {
    ERROR("Error number of CCE %d is lower than the aggregation level %d\n", N_cce, L);
    return SRSLTE_ERROR;
  }

  // Calculate Y_p_n
  uint32_t Y_p_n = 0;
  if (search_space->type == srslte_search_space_type_ue) {
    Y_p_n = srslte_pdcch_calculate_Y_p_n(coreset->id, rnti, slot_idx);
  }

  return (int)(L * ((Y_p_n + (m * N_cce) / (L * M) + n_ci) % (N_cce / L)));
}

static uint32_t dmrs_pdcch_get_cinit(uint32_t slot_idx, uint32_t symbol_idx, uint32_t n_id)
{
  return (uint32_t)((((SRSLTE_NR_NSYMB_PER_SLOT * slot_idx + symbol_idx + 1UL) << 17UL) * (2 * n_id + 1) + 2 * n_id) &
                    (uint64_t)INT32_MAX);
}

static void
dmrs_pdcch_put_symbol_noninterleaved(const srslte_nr_pdcch_cfg_t* cfg, uint32_t cinit, uint32_t ncce, cf_t* sf_symbol)
{
  uint32_t L            = 1U << cfg->aggregation_level;
  uint32_t nof_freq_res = SRSLTE_MIN(cfg->carrier.nof_prb / 6, SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE);

  // Initialise sequence for this symbol
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);
  uint32_t sequence_skip = 0; // Accumulates pilot locations to skip

  // Calculate Resource block indexes range, every CCE is 6 REG, 1 REG is 6 RE in resource blocks
  uint32_t rb_coreset_idx_begin = (ncce * 6) / cfg->coreset.duration;
  uint32_t rb_coreset_idx_end   = ((ncce + L) * 6) / cfg->coreset.duration;

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

int srslte_dmrs_pdcch_put(const srslte_nr_pdcch_cfg_t* cfg, uint32_t slot_idx, cf_t* sf_symbols)
{
  int ncce = srslte_pdcch_get_ncce(
      &cfg->coreset, &cfg->search_space, cfg->rnti, cfg->aggregation_level, cfg->candidate, slot_idx);
  if (ncce < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
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
    dmrs_pdcch_put_symbol_noninterleaved(cfg, cinit, ncce, &sf_symbols[cfg->carrier.nof_prb * SRSLTE_NRE * l]);
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_estimator_init(srslte_dmrs_pdcch_estimator_t* q,
                                     const srslte_nr_carrier_t*     carrier,
                                     const srslte_coreset_t*        coreset)
{
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
  if (srslte_resampler_fft_init(&q->interpolator, SRSLTE_RESAMPLER_MODE_INTERPOLATE, 4)) {
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
  if (q->ce) {
    free(q->ce);
  }

  for (uint32_t i = 0; i < SRSLTE_CORESET_DURATION_MAX; i++) {
    if (q->lse[i]) {
      free(q->lse[i]);
    }
  }

  srslte_resampler_fft_free(&q->interpolator);

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

  // Time averaging should be implemented here
  // ...

  // Interpolator impulse response
  uint32_t interpolation_delay = srslte_resampler_fft_get_delay(&q->interpolator) / 4;

  // Interpolation, it assumes all frequency domain resources are contiguous
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    cf_t* ce_ptr = &q->ce[SRSLTE_NRE * q->coreset_bw * l];

    srslte_resampler_fft_reset_state(&q->interpolator);

    // Feed inital samples
    uint32_t discard_initial = SRSLTE_MIN(interpolation_delay, q->coreset_bw * 3);
    srslte_resampler_fft_run(&q->interpolator, q->lse[l], NULL, discard_initial);
    uint32_t n = 0;

    // Pad zeroes until impulsional response is covered
    if (discard_initial < interpolation_delay) {
      srslte_resampler_fft_run(&q->interpolator, NULL, NULL, interpolation_delay - discard_initial);
    } else {
      n = q->coreset_bw * 3 - discard_initial;
      srslte_resampler_fft_run(&q->interpolator, q->lse[l], ce_ptr, n);
    }

    srslte_resampler_fft_run(&q->interpolator, NULL, &ce_ptr[n * 4], q->coreset_bw * 3 - n);
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_get_measure(srslte_dmrs_pdcch_estimator_t* q,
                                  const srslte_search_space_t*   search_space,
                                  uint32_t                       slot_idx,
                                  uint32_t                       aggregation_level,
                                  uint32_t                       candidate,
                                  uint16_t                       rnti,
                                  srslte_dmrs_pdcch_measure_t*   measure)
{
  uint32_t L    = 1U << aggregation_level;
  int      ncce = srslte_pdcch_get_ncce(&q->coreset, search_space, rnti, aggregation_level, candidate, slot_idx);
  if (ncce < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

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
      (float)SRSLTE_SUBC_SPACING(q->carrier.numerology) * sync_err / (4.0e-6f * (float)q->coreset.duration);

  return SRSLTE_SUCCESS;
}