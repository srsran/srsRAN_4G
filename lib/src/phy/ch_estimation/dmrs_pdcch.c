/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/ch_estimation/dmrs_pdcch.h"

/// @brief Every frequency resource is 6 Resource blocks, every resource block carries 3 pilots. So 18 possible pilots
/// per frequency resource.
#define NOF_PILOTS_X_FREQ_RES 18

#define DMRS_PDCCH_INFO_TX(...) INFO("PDCCH DMRS Tx: " __VA_ARGS__)
#define DMRS_PDCCH_INFO_RX(...) INFO("PDCCH DMRS Rx: " __VA_ARGS__)

/// @brief Enables interpolation at CCE frequency bandwidth to avoid interference with adjacent PDCCH DMRS
#define DMRS_PDCCH_INTERPOLATE_GROUP 1

static uint32_t dmrs_pdcch_get_cinit(uint32_t slot_idx, uint32_t symbol_idx, uint32_t n_id)
{
  return (uint32_t)(
      ((1UL << 17UL) * (SRSLTE_NSYMB_PER_SLOT_NR * slot_idx + symbol_idx + 1UL) * (2UL * n_id + 1UL) + 2UL * n_id) %
      INT32_MAX);
}

static void dmrs_pdcch_put_symbol_noninterleaved(const srslte_carrier_nr_t*   carrier,
                                                 const srslte_coreset_t*      coreset,
                                                 const srslte_dci_location_t* dci_location,
                                                 uint32_t                     cinit,
                                                 cf_t*                        sf_symbol)
{
  uint32_t L            = 1U << dci_location->L;
  uint32_t nof_freq_res = SRSLTE_MIN(carrier->nof_prb / 6, SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE);

  // Initialise sequence for this symbol
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);
  uint32_t sequence_skip = 0; // Accumulates pilot locations to skip

  // Calculate Resource block indexes range, every CCE is 6 REG, 1 REG is 6 RE in resource blocks
  uint32_t rb_coreset_idx_begin = (dci_location->ncce * 6) / coreset->duration;
  uint32_t rb_coreset_idx_end   = ((dci_location->ncce + L) * 6) / coreset->duration;

  // CORESET Resource Block counter
  uint32_t rb_coreset_idx = 0;
  for (uint32_t i = 0; i < nof_freq_res; i++) {
    // Skip frequency resource if outside of the CORESET
    if (!coreset->freq_resources[i]) {
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

int srslte_dmrs_pdcch_put(const srslte_carrier_nr_t*   carrier,
                          const srslte_coreset_t*      coreset,
                          const srslte_dl_slot_cfg_t*  slot_cfg,
                          const srslte_dci_location_t* dci_location,
                          cf_t*                        sf_symbols)
{
  if (carrier == NULL || coreset == NULL || slot_cfg == NULL || dci_location == NULL || sf_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (coreset->mapping_type == srslte_coreset_mapping_type_interleaved) {
    ERROR("Error interleaved CORESET mapping is not currently implemented\n");
    return SRSLTE_ERROR;
  }

  if (coreset->duration < SRSLTE_CORESET_DURATION_MIN || coreset->duration > SRSLTE_CORESET_DURATION_MAX) {
    ERROR("Error CORESET duration %d is out-of-bounds (%d,%d)\n",
          coreset->duration,
          SRSLTE_CORESET_DURATION_MIN,
          SRSLTE_CORESET_DURATION_MAX);
    return SRSLTE_ERROR;
  }

  // Use cell id if the DMR scrambling id is not provided by higher layers
  uint32_t n_id = carrier->id;
  if (coreset->dmrs_scrambling_id_present) {
    n_id = coreset->dmrs_scrambling_id;
  }

  // Bound slot index
  uint32_t slot_idx = SRSLTE_SLOT_NR_MOD(carrier->numerology, slot_cfg->idx);

  for (uint32_t l = 0; l < coreset->duration; l++) {
    // Get Cin
    uint32_t cinit = dmrs_pdcch_get_cinit(slot_idx, l, n_id);

    DMRS_PDCCH_INFO_TX("n=%d; l=%d; cinit=%08x\n", slot_idx, l, cinit);

    // Put data
    dmrs_pdcch_put_symbol_noninterleaved(
        carrier, coreset, dci_location, cinit, &sf_symbols[carrier->nof_prb * SRSLTE_NRE * l]);
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

  // Calculate new bandwidth and size
  uint32_t coreset_bw = srslte_coreset_get_bw(coreset);
  uint32_t coreset_sz = srslte_coreset_get_sz(coreset);

#if DMRS_PDCCH_INTERPOLATE_GROUP
  if (srslte_interp_linear_init(&q->interpolator, NOF_PILOTS_X_FREQ_RES / q->coreset.duration, 4)) {
#else
  if (srslte_interp_linear_init(&q->interpolator, coreset_bw * 3, 4)) {
#endif
    ERROR("Initiating interpolator\n");
    return SRSLTE_ERROR;
  }

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

  if (q->filter == NULL) {
    q->filter_len = 5U;

    q->filter = srslte_vec_f_malloc(q->filter_len);
    if (q->filter == NULL) {
      return SRSLTE_ERROR;
    }

    srslte_chest_set_smooth_filter_gauss(q->filter, q->filter_len - 1, 2);
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

  if (q->filter) {
    free(q->filter);
  }

  srslte_interp_linear_free(&q->interpolator);

  SRSLTE_MEM_ZERO(q, srslte_dmrs_pdcch_estimator_t, 1);
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
  uint32_t freq_domain_res_size = SRSLTE_MIN(q->carrier.nof_prb / 6, SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE);
  for (uint32_t i = 0; i < freq_domain_res_size; i++) {
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

int srslte_dmrs_pdcch_estimate(srslte_dmrs_pdcch_estimator_t* q,
                               const srslte_dl_slot_cfg_t*    slot_cfg,
                               const cf_t*                    sf_symbols)
{
  if (q == NULL || sf_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Use cell id if the DMR scrambling id is not provided by higher layers
  uint32_t n_id = q->carrier.id;
  if (q->coreset.dmrs_scrambling_id_present) {
    n_id = q->coreset.dmrs_scrambling_id;
  }

  // Bound slot index
  uint32_t slot_idx = SRSLTE_SLOT_NR_MOD(q->carrier.numerology, slot_cfg->idx);

  // Extract pilots
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    // Calculate PRN sequence initial state
    uint32_t cinit = dmrs_pdcch_get_cinit(slot_idx, l, n_id);

    DMRS_PDCCH_INFO_RX("n=%d; l=%d; cinit=%08x\n", slot_idx, l, cinit);

    // Extract pilots least square estimates
    srslte_dmrs_pdcch_extract(q, cinit, &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], q->lse[l]);
  }

  // Time averaging and smoothing should be implemented here
  // ...

  // Interpolation in groups
#if DMRS_PDCCH_INTERPOLATE_GROUP
  uint32_t group_count = (q->coreset.duration * q->coreset_bw * 3) / NOF_PILOTS_X_FREQ_RES;
  uint32_t group_size  = NOF_PILOTS_X_FREQ_RES / q->coreset.duration;
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    for (uint32_t j = 0; j < group_count; j++) {
      cf_t tmp[NOF_PILOTS_X_FREQ_RES];

      // Smoothing filter group
      srslte_conv_same_cf(&q->lse[l][j * group_size], q->filter, tmp, group_size, q->filter_len);

      srslte_interp_linear_offset(
          &q->interpolator, tmp, &q->ce[SRSLTE_NRE * q->coreset_bw * l + j * group_size * 4], 1, 3);
    }
  }
#else
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    srslte_interp_linear_offset(&q->interpolator, q->lse[l], &q->ce[SRSLTE_NRE * q->coreset_bw * l], 1, 3);
  }
#endif

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_get_measure(const srslte_dmrs_pdcch_estimator_t* q,
                                  const srslte_dci_location_t*         dci_location,
                                  srslte_dmrs_pdcch_measure_t*         measure)
{
  if (q == NULL || dci_location == NULL || measure == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t L = 1U << dci_location->L;
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
  uint32_t pilot_idx  = (dci_location->ncce * 18) / q->coreset.duration;
  uint32_t nof_pilots = (L * 18) / q->coreset.duration;

  float rsrp                              = 0.0f;
  float epre                              = 0.0f;
  float cfo                               = 0.0f;
  float sync_err                          = 0.0f;
  cf_t  corr[SRSLTE_CORESET_DURATION_MAX] = {};
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    if (SRSLTE_DEBUG_ENABLED && srslte_verbose >= SRSLTE_VERBOSE_INFO && !handler_registered) {
      DMRS_PDCCH_INFO_RX("Measuring PDCCH l=%d; lse=", l);
      srslte_vec_fprint_c(stdout, &q->lse[l][pilot_idx], nof_pilots);
    }

    // Correlate DMRS
    corr[l] = srslte_vec_acc_cc(&q->lse[l][pilot_idx], nof_pilots) / (float)nof_pilots;

    // Measure symbol RSRP
    rsrp += __real__ corr[l] * __real__ corr[l] + __imag__ corr[l] * __imag__ corr[l];

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

  measure->rsrp_dBfs = srslte_convert_power_to_dB(measure->rsrp);
  measure->epre_dBfs = srslte_convert_power_to_dB(measure->epre);

  if (isnormal(measure->rsrp) && isnormal(measure->epre)) {
    measure->norm_corr = measure->rsrp / measure->epre;
  } else {
    measure->norm_corr = 0.0f;
  }

  DMRS_PDCCH_INFO_RX("Measure L=%d; ncce=%d; RSRP=%+.1f dBfs; EPRE=%+.1f dBfs; Corr=%.3f\n",
                     dci_location->L,
                     dci_location->ncce,
                     measure->rsrp_dBfs,
                     measure->epre_dBfs,
                     measure->norm_corr);

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdcch_get_ce(const srslte_dmrs_pdcch_estimator_t* q,
                             const srslte_dci_location_t*         dci_location,
                             srslte_dmrs_pdcch_ce_t*              ce)
{
  if (q == NULL || dci_location == NULL || ce == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t L = 1U << dci_location->L;
  if (q->coreset.mapping_type == srslte_coreset_mapping_type_interleaved) {
    ERROR("Error interleaved mapping not implemented\n");
    return SRSLTE_ERROR;
  }

  // Check that CORESET duration is not less than minimum
  if (q->coreset.duration < SRSLTE_CORESET_DURATION_MIN) {
    ERROR("Invalid CORESET duration\n");
    return SRSLTE_ERROR;
  }

  // Calculate begin and end sub-carrier index for the selected candidate
  uint32_t k_begin = (dci_location->ncce * SRSLTE_NRE * 6) / q->coreset.duration;
  uint32_t k_end   = k_begin + (L * 6 * SRSLTE_NRE) / q->coreset.duration;

  // Extract CE for PDCCH
  uint32_t count = 0;
  for (uint32_t l = 0; l < q->coreset.duration; l++) {
    for (uint32_t k = k_begin; k < k_end; k++) {
      if (k % 4 != 1) {
        ce->ce[count++] = q->ce[q->coreset_bw * SRSLTE_NRE * l + k];
      }
    }
  }

  // Double check extracted RE match ideal count
  ce->nof_re = (SRSLTE_NRE - 3) * 6 * L;
  if (count != ce->nof_re) {
    ERROR("Incorrect number of extracted resources (%d != %d)\n", count, ce->nof_re);
  }

  // At the moment Noise is not calculated
  ce->noise_var = 0.0f;

  return SRSLTE_SUCCESS;
}
