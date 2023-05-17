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

#include "srsran/phy/ch_estimation/dmrs_sch.h"
#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/common/sequence.h"
#include <complex.h>
#include <srsran/phy/utils/debug.h>

#define SRSRAN_DMRS_SCH_TYPEA_SINGLE_DURATION_MIN 3
#define SRSRAN_DMRS_SCH_TYPEA_DOUBLE_DURATION_MIN 4

/**
 * @brief Set to 1 for synchronization error pre-compensation before interpolator
 */
#define DMRS_SCH_SYNC_PRECOMPENSATE 1

/**
 * @brief Set to 1 for CFO error pre-compensation before interpolator
 */
#define DMRS_SCH_CFO_PRECOMPENSATE 1

/**
 * @brief Set Smoothing filter length, set to 0 for disabling. The recommended value is 5.
 */
#define DMRS_SCH_SMOOTH_FILTER_LEN 5

/**
 * @brief Set smoothing filter (gaussian) standard deviation
 */
#define DMRS_SCH_SMOOTH_FILTER_STDDEV 2

/**
 * @brief Default number of PRB at initialization
 */
#define DMRS_SCH_MAX_NOF_PRB 106

int srsran_dmrs_sch_cfg_to_str(const srsran_dmrs_sch_cfg_t* cfg, char* msg, uint32_t max_len)
{
  int type           = (int)cfg->type + 1;
  int typeA_pos      = (int)cfg->typeA_pos + 2;
  int additional_pos = cfg->additional_pos == srsran_dmrs_sch_add_pos_0
                           ? 0
                           : cfg->additional_pos == srsran_dmrs_sch_add_pos_1
                                 ? 1
                                 : cfg->additional_pos == srsran_dmrs_sch_add_pos_2 ? 2 : 3;
  const char* len = cfg->length == srsran_dmrs_sch_len_1 ? "single" : "double";

  return srsran_print_check(
      msg, max_len, 0, "type=%d, typeA_pos=%d, add_pos=%d, len=%s", type, typeA_pos, additional_pos, len);
}

static uint32_t
srsran_dmrs_get_pilots_type1(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, const cf_t* symbols, cf_t* pilots)
{
  uint32_t count   = 0;
  uint32_t n_begin = start_prb * 3;
  uint32_t n_enb   = n_begin + nof_prb * 3;

  for (uint32_t n = n_begin; n < n_enb; n++) {
    for (uint32_t k_prime = 0; k_prime < 2; k_prime++, count++) {
      pilots[count] = symbols[4 * n + 2 * k_prime + delta];
    }
  }

  return count;
}

static uint32_t
srsran_dmrs_get_pilots_type2(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, const cf_t* symbols, cf_t* pilots)
{
  uint32_t count   = 0;
  uint32_t n_begin = start_prb * 2;
  uint32_t n_enb   = n_begin + nof_prb * 2;

  for (uint32_t n = n_begin; n < n_enb; n++) {
    for (uint32_t k_prime = 0; k_prime < 2; k_prime++, count++) {
      pilots[count] = symbols[6 * n + k_prime + delta];
    }
  }

  return count;
}

static uint32_t srsran_dmrs_get_lse(srsran_dmrs_sch_t*       q,
                                    srsran_sequence_state_t* sequence_state,
                                    srsran_dmrs_sch_type_t   dmrs_type,
                                    uint32_t                 start_prb,
                                    uint32_t                 nof_prb,
                                    uint32_t                 delta,
                                    float                    amplitude,
                                    const cf_t*              symbols,
                                    cf_t*                    least_square_estimates)
{
  uint32_t count = 0;

  switch (dmrs_type) {
    case srsran_dmrs_sch_type_1:
      count = srsran_dmrs_get_pilots_type1(start_prb, nof_prb, delta, symbols, least_square_estimates);
      break;
    case srsran_dmrs_sch_type_2:
      count = srsran_dmrs_get_pilots_type2(start_prb, nof_prb, delta, symbols, least_square_estimates);
      break;
    default:
      ERROR("Unknown DMRS type.");
  }

  // Generate sequence for the given pilots
  srsran_sequence_state_gen_f(sequence_state, amplitude, (float*)q->temp, count * 2);

  // Calculate least square estimates
  srsran_vec_prod_conj_ccc(least_square_estimates, q->temp, least_square_estimates, count);

  return count;
}

static uint32_t
srsran_dmrs_put_pilots_type1(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, cf_t* symbols, const cf_t* pilots)
{
  uint32_t count   = 0;
  uint32_t n_begin = start_prb * 3;
  uint32_t n_enb   = n_begin + nof_prb * 3;

  for (uint32_t n = n_begin; n < n_enb; n++) {
    for (uint32_t k_prime = 0; k_prime < 2; k_prime++, count++) {
      symbols[4 * n + 2 * k_prime + delta] = pilots[count];
    }
  }

  return count;
}

static uint32_t
srsran_dmrs_put_pilots_type2(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, cf_t* symbols, const cf_t* pilots)
{
  uint32_t count   = 0;
  uint32_t n_begin = start_prb * 2;
  uint32_t n_enb   = n_begin + nof_prb * 2;

  for (uint32_t n = n_begin; n < n_enb; n++) {
    for (uint32_t k_prime = 0; k_prime < 2; k_prime++, count++) {
      symbols[6 * n + k_prime + delta] = pilots[count];
    }
  }

  return count;
}

static uint32_t srsran_dmrs_put_pilots(srsran_dmrs_sch_t*       q,
                                       srsran_sequence_state_t* sequence_state,
                                       srsran_dmrs_sch_type_t   dmrs_type,
                                       uint32_t                 start_prb,
                                       uint32_t                 nof_prb,
                                       uint32_t                 delta,
                                       float                    amplitude,
                                       cf_t*                    symbols)
{
  uint32_t count = (dmrs_type == srsran_dmrs_sch_type_1) ? nof_prb * 6 : nof_prb * 4;

  // Generate sequence for the given pilots
  srsran_sequence_state_gen_f(sequence_state, amplitude, (float*)q->temp, count * 2);

  switch (dmrs_type) {
    case srsran_dmrs_sch_type_1:
      count = srsran_dmrs_put_pilots_type1(start_prb, nof_prb, delta, symbols, q->temp);
      break;
    case srsran_dmrs_sch_type_2:
      count = srsran_dmrs_put_pilots_type2(start_prb, nof_prb, delta, symbols, q->temp);
      break;
    default:
      ERROR("Unknown DMRS type.");
  }

  return count;
}

static int srsran_dmrs_sch_put_symbol(srsran_dmrs_sch_t*           q,
                                      const srsran_sch_cfg_nr_t*   pdsch_cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      uint32_t                     cinit,
                                      uint32_t                     delta,
                                      cf_t*                        symbols)
{
  // Get signal amplitude
  float amplitude = M_SQRT1_2;
  if (isnormal(grant->beta_dmrs)) {
    amplitude *= grant->beta_dmrs;
  }

  const srsran_dmrs_sch_cfg_t* dmrs_cfg         = &pdsch_cfg->dmrs;
  uint32_t                     prb_count        = 0; // Counts consecutive used PRB
  uint32_t                     prb_start        = 0; // Start consecutive used PRB
  uint32_t                     prb_skip         = 0; // Number of PRB to skip
  uint32_t                     nof_pilots_x_prb = dmrs_cfg->type == srsran_dmrs_sch_type_1 ? 6 : 4;
  uint32_t                     pilot_count      = 0;

  // Initialise sequence
  srsran_sequence_state_t sequence_state = {};
  srsran_sequence_state_init(&sequence_state, cinit);

  // Iterate over PRBs
  for (uint32_t prb_idx = 0; prb_idx < q->carrier.nof_prb; prb_idx++) {
    // If the PRB is used for PDSCH transmission count
    if (grant->prb_idx[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... save first consecutive PRB in the group
        prb_start = prb_idx;

        // ... discard unused pilots and reset counter unless the PDSCH transmission carries SIB
        prb_skip = SRSRAN_MAX(0, (int)prb_skip - (int)dmrs_cfg->reference_point_k_rb);
        srsran_sequence_state_advance(&sequence_state, prb_skip * nof_pilots_x_prb * 2);
        prb_skip = 0;
      }
      prb_count++;

      continue;
    }

    // Increase number of PRB to skip
    prb_skip++;

    // End of consecutive PRB, skip copying if no PRB was counted
    if (prb_count == 0) {
      continue;
    }

    // Get contiguous pilots
    pilot_count +=
        srsran_dmrs_put_pilots(q, &sequence_state, dmrs_cfg->type, prb_start, prb_count, delta, amplitude, symbols);

    // Reset counter
    prb_count = 0;
  }

  if (prb_count > 0) {
    pilot_count +=
        srsran_dmrs_put_pilots(q, &sequence_state, dmrs_cfg->type, prb_start, prb_count, delta, amplitude, symbols);
  }

  return pilot_count;
}

// Implements 3GPP 38.211 R.15 Table 7.4.1.1.2-3 PDSCH mapping type A Single
static int srsran_dmrs_sch_get_symbols_idx_mapping_type_A_single(const srsran_dmrs_sch_cfg_t* dmrs_cfg,
                                                                 uint32_t                     ld,
                                                                 uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS])
{
  int count = 0;

  if (ld < SRSRAN_DMRS_SCH_TYPEA_SINGLE_DURATION_MIN) {
    ERROR("Duration is below the minimum");
    return SRSRAN_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (dmrs_cfg->typeA_pos == srsran_dmrs_sch_typeA_pos_3) ? 3 : 2;

  // For PDSCH mapping Type A single-symbol DM-RS, l1 = 11 except if all of the following conditions are fulfilled in
  // which case l1 = 12:
  // - the higher-layer parameter lte-CRS-ToMatchAround is configured; and
  // - the higher-layer parameters dmrs-AdditionalPosition is equal to 'pos1' and l0 = 3; and
  // - the UE has indicated it is capable of additionalDMRS-DL-Alt
  int l1 = 11;
  if (dmrs_cfg->lte_CRS_to_match_around && dmrs_cfg->additional_pos == srsran_dmrs_sch_add_pos_1 &&
      dmrs_cfg->typeA_pos == srsran_dmrs_sch_typeA_pos_3 && dmrs_cfg->additional_DMRS_DL_Alt) {
    l1 = 12;
  }

  symbols[count] = l0;
  count++;

  if (ld < 8 || dmrs_cfg->additional_pos == srsran_dmrs_sch_add_pos_0) {
    return count;
  }

  if (ld < 10) {
    symbols[count] = 7;
    count++;
  } else if (ld < 12) {
    if (dmrs_cfg->additional_pos > srsran_dmrs_sch_add_pos_2) {
      symbols[count] = 6;
      count++;
    }

    symbols[count] = 9;
    count++;

  } else if (ld == 12) {
    switch (dmrs_cfg->additional_pos) {
      case srsran_dmrs_sch_add_pos_1:
        symbols[count] = 9;
        count++;
        break;
      case srsran_dmrs_sch_add_pos_2:
        symbols[count] = 6;
        count++;
        symbols[count] = 9;
        count++;
        break;
      default:
        symbols[count] = 5;
        count++;
        symbols[count] = 8;
        count++;
        symbols[count] = 11;
        count++;
    }
  } else {
    switch (dmrs_cfg->additional_pos) {
      case srsran_dmrs_sch_add_pos_1:
        symbols[count] = l1;
        count++;
        break;
      case srsran_dmrs_sch_add_pos_2:
        symbols[count] = 7;
        count++;
        symbols[count] = 11;
        count++;
        break;
      default:
        symbols[count] = 5;
        count++;
        symbols[count] = 8;
        count++;
        symbols[count] = 11;
        count++;
    }
  }

  return count;
}

// Implements 3GPP 38.211 R.15 Table 7.4.1.1.2-4 PDSCH mapping type A Double
static int srsran_dmrs_sch_get_symbols_idx_mapping_type_A_double(const srsran_dmrs_sch_cfg_t* dmrs_cfg,
                                                                 uint32_t                     ld,
                                                                 uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS])
{
  int count = 0;

  if (ld < SRSRAN_DMRS_SCH_TYPEA_DOUBLE_DURATION_MIN) {
    return SRSRAN_ERROR;
  }

  // According to Table 7.4.1.1.2-4, the additional position 3 is invalid.
  if (dmrs_cfg->additional_pos == srsran_dmrs_sch_add_pos_3) {
    ERROR("Invalid additional DMRS (%d)", dmrs_cfg->additional_pos);
    return SRSRAN_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (dmrs_cfg->typeA_pos == srsran_dmrs_sch_typeA_pos_3) ? 3 : 2;

  symbols[count] = l0;
  count++;
  symbols[count] = symbols[count - 1] + 1;
  count++;

  if (ld < 10 || dmrs_cfg->additional_pos == srsran_dmrs_sch_add_pos_0) {
    return count;
  }

  if (ld < 13) {
    symbols[count] = 8;
    count++;
    symbols[count] = symbols[count - 1] + 1;
    count++;
  } else {
    symbols[count] = 10;
    count++;
    symbols[count] = symbols[count - 1] + 1;
    count++;
  }

  return count;
}

int srsran_dmrs_sch_get_symbols_idx(const srsran_dmrs_sch_cfg_t* dmrs_cfg,
                                    const srsran_sch_grant_nr_t* grant,
                                    uint32_t                     symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS])
{
  // The position(s) of the DM-RS symbols is given by l and duration ld where
  // - for PDSCH mapping type A, ld is the duration between the first OFDM symbol of the slot and the last OFDM symbol
  //   of the scheduled PDSCH resources in the slot
  // - for PDSCH mapping type B, ld is the duration of the scheduled PDSCH resources
  uint32_t ld = grant->L;
  if (grant->mapping == srsran_sch_mapping_type_A) {
    ld = grant->S + grant->L;
  }

  switch (grant->mapping) {
    case srsran_sch_mapping_type_A:
      // The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal to 'pos2'
      if (dmrs_cfg->typeA_pos != srsran_dmrs_sch_typeA_pos_2 && dmrs_cfg->additional_pos == srsran_dmrs_sch_add_pos_3) {
        ERROR("The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal "
              "to 'pos2'");
        return SRSRAN_ERROR;
      }

      // For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively is only
      // applicable when dmrs-TypeA-Position is equal to 'pos2
      if ((ld == 3 || ld == 4) && dmrs_cfg->typeA_pos != srsran_dmrs_sch_typeA_pos_2) {
        ERROR("For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively "
              "is only applicable when dmrs-TypeA-Position is equal to 'pos2");
        return SRSRAN_ERROR;
      }

      if (dmrs_cfg->length == srsran_dmrs_sch_len_1) {
        return srsran_dmrs_sch_get_symbols_idx_mapping_type_A_single(dmrs_cfg, ld, symbols);
      }
      return srsran_dmrs_sch_get_symbols_idx_mapping_type_A_double(dmrs_cfg, ld, symbols);
    case srsran_sch_mapping_type_B:
      ERROR("Error PDSCH mapping type B not supported");
      return SRSRAN_ERROR;
  }

  return SRSRAN_ERROR;
}

int srsran_dmrs_sch_rvd_re_pattern(const srsran_dmrs_sch_cfg_t* cfg,
                                   const srsran_sch_grant_nr_t* grant,
                                   srsran_re_pattern_t*         pattern)
{
  if (cfg == NULL || pattern == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Initialise pattern with zeros
  SRSRAN_MEM_ZERO(pattern, srsran_re_pattern_t, 1);

  // Fill RB bounds
  pattern->rb_begin  = 0;
  pattern->rb_end    = SRSRAN_MAX_PRB_NR;
  pattern->rb_stride = 1;

  // Fill subcarrier mask
  if (cfg->type == srsran_dmrs_sch_type_1) {
    for (uint32_t n = 0; n < 3; n++) {
      for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
        for (uint32_t delta = 0; delta < grant->nof_dmrs_cdm_groups_without_data; delta++) {
          pattern->sc[(4 * n + 2 * k_prime + delta) % SRSRAN_NRE] = true;
        }
      }
    }
  } else {
    for (uint32_t n = 0; n < 2; n++) {
      for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
        for (uint32_t delta = 0; delta < grant->nof_dmrs_cdm_groups_without_data; delta++) {
          pattern->sc[(6 * n + k_prime + 2 * delta) % SRSRAN_NRE] = true;
        }
      }
    }
  }

  // Calculate OFDM symbols
  uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS];
  int      nof_l = srsran_dmrs_sch_get_symbols_idx(cfg, grant, symbols);
  if (nof_l < SRSRAN_SUCCESS) {
    ERROR("Error calculating OFDM symbols");
    return SRSRAN_ERROR;
  }

  // Set OFDM symbol mask
  for (int i = 0; i < nof_l; i++) {
    uint32_t l                                    = symbols[i];
    pattern->symbol[l % SRSRAN_NSYMB_PER_SLOT_NR] = true;
  }

  return SRSRAN_SUCCESS;
}

int srsran_dmrs_sch_get_N_prb(const srsran_dmrs_sch_cfg_t* dmrs_cfg, const srsran_sch_grant_nr_t* grant)
{
  if (grant->nof_dmrs_cdm_groups_without_data < 1 || grant->nof_dmrs_cdm_groups_without_data > 3) {
    ERROR("Invalid number if DMRS CDM groups without data (%d). Valid values: 1, 2 , 3",
          grant->nof_dmrs_cdm_groups_without_data);
    return SRSRAN_ERROR;
  }

  // Get number of frequency domain resource elements used for DMRS
  int nof_sc = SRSRAN_DMRS_SCH_SC(grant->nof_dmrs_cdm_groups_without_data, dmrs_cfg->type);

  // Get number of symbols used for DMRS
  uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  int      ret                                  = srsran_dmrs_sch_get_symbols_idx(dmrs_cfg, grant, symbols);
  if (ret < SRSRAN_SUCCESS) {
    ERROR("Error getting PDSCH DMRS symbol indexes");
    return SRSRAN_ERROR;
  }

  return nof_sc * ret;
}

static uint32_t srsran_dmrs_sch_seed(const srsran_carrier_nr_t*   carrier,
                                     const srsran_sch_cfg_nr_t*   cfg,
                                     const srsran_sch_grant_nr_t* grant,
                                     uint32_t                     slot_idx,
                                     uint32_t                     symbol_idx)
{
  const srsran_dmrs_sch_cfg_t* dmrs_cfg = &cfg->dmrs;

  // Calculate scrambling IDs
  uint32_t n_id   = carrier->pci;
  uint32_t n_scid = (grant->n_scid) ? 1 : 0;
  if (!grant->n_scid && dmrs_cfg->scrambling_id0_present) {
    // n_scid = 0 and ID0 present
    n_id = dmrs_cfg->scrambling_id0;
  } else if (grant->n_scid && dmrs_cfg->scrambling_id1_present) {
    // n_scid = 1 and ID1 present
    n_id = dmrs_cfg->scrambling_id1;
  }

  return SRSRAN_SEQUENCE_MOD((((SRSRAN_NSYMB_PER_SLOT_NR * slot_idx + symbol_idx + 1UL) * (2UL * n_id + 1UL)) << 17UL) +
                             (2UL * n_id + n_scid));
}

static int dmrs_sch_alloc(srsran_dmrs_sch_t* q, uint32_t max_nof_prb)
{
  bool max_nof_prb_changed = q->max_nof_prb < max_nof_prb;

  // Update maximum number of PRB
  q->max_nof_prb = max_nof_prb;

  // Resize/allocate temp for gNb and UE
  if (max_nof_prb_changed) {
    if (q->temp) {
      free(q->temp);
    }

    q->temp = srsran_vec_cf_malloc(max_nof_prb * SRSRAN_NRE);
    if (!q->temp) {
      ERROR("malloc");
      return SRSRAN_ERROR;
    }
  }

  // If it is not UE, quit now
  if (!q->is_rx) {
    return SRSRAN_SUCCESS;
  }

  if (max_nof_prb_changed) {
    // Resize interpolator only if the number of PRB has increased
    srsran_interp_linear_free(&q->interpolator_type1);
    srsran_interp_linear_free(&q->interpolator_type2);

    if (srsran_interp_linear_init(&q->interpolator_type1, max_nof_prb * SRSRAN_NRE / 2, 2) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
    if (srsran_interp_linear_init(&q->interpolator_type2, max_nof_prb * SRSRAN_NRE / 3, 6) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }

    if (q->pilot_estimates) {
      free(q->pilot_estimates);
    }

    // The maximum number of pilots is for Type 1
    q->pilot_estimates = srsran_vec_cf_malloc(SRSRAN_DMRS_SCH_MAX_SYMBOLS * max_nof_prb * SRSRAN_NRE / 2);
    if (!q->pilot_estimates) {
      ERROR("malloc");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_dmrs_sch_init(srsran_dmrs_sch_t* q, bool is_rx)
{
  if (q == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  SRSRAN_MEM_ZERO(q, srsran_dmrs_sch_t, 1);

  if (is_rx) {
    q->is_rx = true;
  }

#if DMRS_SCH_SMOOTH_FILTER_LEN
  if (q->filter == NULL) {
    q->filter = srsran_vec_f_malloc(DMRS_SCH_SMOOTH_FILTER_LEN);
    if (q->filter == NULL) {
      return SRSRAN_ERROR;
    }
    srsran_chest_set_smooth_filter_gauss(q->filter, DMRS_SCH_SMOOTH_FILTER_LEN - 1, 2);
  }
#endif // DMRS_SCH_SMOOTH_FILTER_LEN

  if (dmrs_sch_alloc(q, DMRS_SCH_MAX_NOF_PRB) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_dmrs_sch_free(srsran_dmrs_sch_t* q)
{
  if (q == NULL) {
    return;
  }

  srsran_interp_linear_free(&q->interpolator_type1);
  srsran_interp_linear_free(&q->interpolator_type2);
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }
  if (q->temp) {
    free(q->temp);
  }
  if (q->filter) {
    free(q->filter);
  }

  SRSRAN_MEM_ZERO(q, srsran_dmrs_sch_t, 1);
}

int srsran_dmrs_sch_set_carrier(srsran_dmrs_sch_t* q, const srsran_carrier_nr_t* carrier)
{
  // Set carrier
  q->carrier = *carrier;

  if (dmrs_sch_alloc(q, carrier->nof_prb) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_dmrs_sch_put_sf(srsran_dmrs_sch_t*           q,
                           const srsran_slot_cfg_t*     slot_cfg,
                           const srsran_sch_cfg_nr_t*   pdsch_cfg,
                           const srsran_sch_grant_nr_t* grant,
                           cf_t*                        sf_symbols)
{
  uint32_t delta = 0;

  if (q == NULL || slot_cfg == NULL || pdsch_cfg == NULL || sf_symbols == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  uint32_t symbol_sz = q->carrier.nof_prb * SRSRAN_NRE; // Symbol size in resource elements

  // Get symbols indexes
  uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  int      nof_symbols                          = srsran_dmrs_sch_get_symbols_idx(&pdsch_cfg->dmrs, grant, symbols);
  if (nof_symbols < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Iterate symbols
  for (uint32_t i = 0; i < nof_symbols; i++) {
    uint32_t l        = symbols[i];                                        // Symbol index inside the slot
    uint32_t slot_idx = SRSRAN_SLOT_NR_MOD(q->carrier.scs, slot_cfg->idx); // Slot index in the frame
    uint32_t cinit    = srsran_dmrs_sch_seed(&q->carrier, pdsch_cfg, grant, slot_idx, l);

    srsran_dmrs_sch_put_symbol(q, pdsch_cfg, grant, cinit, delta, &sf_symbols[symbol_sz * l]);
  }

  return SRSRAN_SUCCESS;
}

static int srsran_dmrs_sch_get_symbol(srsran_dmrs_sch_t*           q,
                                      const srsran_sch_cfg_nr_t*   pdsch_cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      uint32_t                     cinit,
                                      uint32_t                     delta,
                                      const cf_t*                  symbols,
                                      cf_t*                        least_square_estimates)
{
  // Get signal amplitude
  float amplitude = M_SQRT1_2;
  if (isnormal(grant->beta_dmrs)) {
    amplitude /= grant->beta_dmrs;
  }

  const srsran_dmrs_sch_cfg_t* dmrs_cfg = &pdsch_cfg->dmrs;

  uint32_t prb_count        = 0; // Counts consecutive used PRB
  uint32_t prb_start        = 0; // Start consecutive used PRB
  uint32_t prb_skip         = 0; // Number of PRB to skip
  uint32_t nof_pilots_x_prb = dmrs_cfg->type == srsran_dmrs_sch_type_1 ? 6 : 4;
  uint32_t pilot_count      = 0;

  // Initialise sequence
  srsran_sequence_state_t sequence_state = {};
  srsran_sequence_state_init(&sequence_state, cinit);

  // Iterate over PRBs
  for (uint32_t prb_idx = 0; prb_idx < q->carrier.nof_prb; prb_idx++) {
    // If the PRB is used for PDSCH transmission count
    if (grant->prb_idx[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... save first consecutive PRB in the group
        prb_start = prb_idx;

        // ... discard unused pilots and reset counter unless the PDSCH transmission carries SIB
        prb_skip = SRSRAN_MAX(0, (int)prb_skip - (int)dmrs_cfg->reference_point_k_rb);
        srsran_sequence_state_advance(&sequence_state, prb_skip * nof_pilots_x_prb * 2);
        prb_skip = 0;
      }
      prb_count++;

      continue;
    }

    // Increase number of PRB to skip
    prb_skip++;

    // End of consecutive PRB, skip copying if no PRB was counted
    if (prb_count == 0) {
      continue;
    }

    // Get contiguous pilots
    pilot_count += srsran_dmrs_get_lse(q,
                                       &sequence_state,
                                       dmrs_cfg->type,
                                       prb_start,
                                       prb_count,
                                       delta,
                                       amplitude,
                                       symbols,
                                       &least_square_estimates[pilot_count]);

    // Reset counter
    prb_count = 0;
  }

  if (prb_count > 0) {
    pilot_count += srsran_dmrs_get_lse(q,
                                       &sequence_state,
                                       dmrs_cfg->type,
                                       prb_start,
                                       prb_count,
                                       delta,
                                       amplitude,
                                       symbols,
                                       &least_square_estimates[pilot_count]);
  }

  return pilot_count;
}

int srsran_dmrs_sch_estimate(srsran_dmrs_sch_t*           q,
                             const srsran_slot_cfg_t*     slot,
                             const srsran_sch_cfg_nr_t*   cfg,
                             const srsran_sch_grant_nr_t* grant,
                             const cf_t*                  sf_symbols,
                             srsran_chest_dl_res_t*       chest_res)
{
  const uint32_t delta = 0;

  if (q == NULL || slot == NULL || sf_symbols == NULL || chest_res == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  const srsran_dmrs_sch_cfg_t* dmrs_cfg = &cfg->dmrs;

  cf_t*    ce        = q->temp;
  uint32_t symbol_sz = q->carrier.nof_prb * SRSRAN_NRE; // Symbol size in resource elements

  // Get symbols indexes
  uint32_t symbols[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  int      nof_symbols                          = srsran_dmrs_sch_get_symbols_idx(&cfg->dmrs, grant, symbols);
  if (nof_symbols <= SRSRAN_SUCCESS) {
    ERROR("Error getting symbol indexes");
    return SRSRAN_ERROR;
  }

  // Get DMRS reserved RE pattern
  srsran_re_pattern_t dmrs_pattern = {};
  if (srsran_dmrs_sch_rvd_re_pattern(dmrs_cfg, grant, &dmrs_pattern) < SRSRAN_SUCCESS) {
    ERROR("Error computing DMRS Reserved Re pattern");
    return SRSRAN_ERROR;
  }

  uint32_t nof_pilots_x_symbol = 0;

  // Iterate symbols and extract LSE estimates
  for (uint32_t i = 0; i < nof_symbols; i++) {
    uint32_t l = symbols[i]; // Symbol index inside the slot

    uint32_t cinit = srsran_dmrs_sch_seed(&q->carrier, cfg, grant, SRSRAN_SLOT_NR_MOD(q->carrier.scs, slot->idx), l);

    nof_pilots_x_symbol = srsran_dmrs_sch_get_symbol(
        q, cfg, grant, cinit, delta, &sf_symbols[symbol_sz * l], &q->pilot_estimates[nof_pilots_x_symbol * i]);

    if (nof_pilots_x_symbol == 0) {
      ERROR("Error, no pilots extracted (i=%d, l=%d)", i, l);
      return SRSRAN_ERROR;
    }
  }

  // Estimate average synchronization error
  float dmrs_stride = (dmrs_cfg->type == srsran_dmrs_sch_type_1) ? 2 : 3;
  float sync_err    = 0.0f;
  for (uint32_t i = 0; i < nof_symbols; i++) {
    sync_err += srsran_vec_estimate_frequency(&q->pilot_estimates[nof_pilots_x_symbol * i], nof_pilots_x_symbol);
  }
  sync_err /= (float)nof_symbols;
  float delay_us = sync_err / (dmrs_stride * SRSRAN_SUBC_SPACING_NR(q->carrier.scs));

#if DMRS_SCH_SYNC_PRECOMPENSATE
  // Pre-compensate synchronization error
  if (isnormal(sync_err)) {
    for (uint32_t i = 0; i < nof_symbols; i++) {
      srsran_vec_apply_cfo(&q->pilot_estimates[nof_pilots_x_symbol * i],
                           sync_err,
                           &q->pilot_estimates[nof_pilots_x_symbol * i],
                           nof_pilots_x_symbol);
    }
  }
#endif // DMRS_SCH_SYNC_ERROR_PRECOMPENSATE

  // Perform Power measurements
  float rsrp                              = 0.0f;
  float epre                              = 0.0f;
  cf_t  corr[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  for (uint32_t i = 0; i < nof_symbols; i++) {
    corr[i] =
        srsran_vec_acc_cc(&q->pilot_estimates[nof_pilots_x_symbol * i], nof_pilots_x_symbol) / nof_pilots_x_symbol;
    rsrp += __real__ corr[i] * __real__ corr[i] + __imag__ corr[i] * __imag__ corr[i];
    epre += srsran_vec_avg_power_cf(&q->pilot_estimates[nof_pilots_x_symbol * i], nof_pilots_x_symbol);
  }
  rsrp /= nof_symbols;
  epre /= nof_symbols;
  rsrp = SRSRAN_MIN(rsrp, epre - epre * 1e-7);

  // Measure CFO if more than one symbol is used
  float cfo_avg_hz = 0.0;
  float cfo_hz_max = INFINITY;
  for (uint32_t i = 0; i < nof_symbols - 1; i++) {
    float time_diff  = srsran_symbol_distance_s(symbols[i], symbols[i + 1], q->carrier.scs);
    float phase_diff = cargf(corr[i + 1] * conjf(corr[i]));

    if (isnormal(time_diff)) {
      cfo_avg_hz += phase_diff / (2.0f * M_PI * time_diff * (nof_symbols - 1));

      // The maximum measured CFO depends on the symbol time difference
      cfo_hz_max = SRSRAN_MIN(cfo_hz_max, 1 / time_diff);
    }
  }

  // Store internal CSI
  q->csi.rsrp       = rsrp;
  q->csi.rsrp_dB    = srsran_convert_power_to_dB(rsrp);
  q->csi.epre       = epre;
  q->csi.epre_dB    = srsran_convert_power_to_dB(epre);
  q->csi.n0         = epre - rsrp;
  q->csi.n0_dB      = srsran_convert_power_to_dB(q->csi.n0);
  q->csi.snr_dB     = q->csi.rsrp_dB - q->csi.n0_dB;
  q->csi.cfo_hz     = cfo_avg_hz;
  q->csi.cfo_hz_max = cfo_hz_max;
  q->csi.delay_us   = delay_us;

  // Write CSI in estimated channel result
  chest_res->rsrp               = q->csi.rsrp;
  chest_res->rsrp_dbm           = q->csi.rsrp_dB;
  chest_res->noise_estimate     = q->csi.n0;
  chest_res->noise_estimate_dbm = q->csi.n0_dB;
  chest_res->snr_db             = q->csi.snr_dB;
  chest_res->cfo                = q->csi.cfo_hz;
  chest_res->sync_error         = q->csi.delay_us;

#if DMRS_SCH_CFO_PRECOMPENSATE
  // Pre-compensate CFO
  cf_t cfo_correction[SRSRAN_NSYMB_PER_SLOT_NR] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  if (isnormal(cfo_avg_hz)) {
    // Calculate phase of the first OFDM symbol (l = 0)
    float arg0 = cargf(corr[0]) - 2.0f * M_PI * srsran_symbol_distance_s(0, symbols[0], q->carrier.scs) * cfo_avg_hz;

    // Calculate CFO corrections
    for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
      float arg         = arg0 + 2.0f * M_PI * cfo_avg_hz * srsran_symbol_distance_s(0, l, q->carrier.scs);
      cfo_correction[l] = cexpf(I * arg);
    }

    // Remove CFO phases
    for (uint32_t i = 0; i < nof_symbols; i++) {
      uint32_t l = symbols[i];
      srsran_vec_sc_prod_ccc(&q->pilot_estimates[nof_pilots_x_symbol * i],
                             conjf(cfo_correction[l]),
                             &q->pilot_estimates[nof_pilots_x_symbol * i],
                             nof_pilots_x_symbol);
    }
  }
#endif // DMRS_SCH_CFO_PRECOMPENSATE

  INFO("PDSCH-DMRS: RSRP=%+.2fdB EPRE=%+.2fdB CFO=%+.0fHz Sync=%.3fus",
       chest_res->rsrp_dbm,
       srsran_convert_power_to_dB(epre),
       cfo_avg_hz,
       chest_res->sync_error * 1e6);

  // Average over time, only if more than one DMRS symbol
  for (uint32_t i = 1; i < nof_symbols; i++) {
    srsran_vec_sum_ccc(
        q->pilot_estimates, &q->pilot_estimates[nof_pilots_x_symbol * i], q->pilot_estimates, nof_pilots_x_symbol);
  }
  if (nof_symbols > 0) {
    srsran_vec_sc_prod_cfc(q->pilot_estimates, 1.0f / (float)nof_symbols, q->pilot_estimates, nof_pilots_x_symbol);
  }

#if DMRS_SCH_SMOOTH_FILTER_LEN
  // Apply smoothing filter
  srsran_conv_same_cf(
      q->pilot_estimates, q->filter, q->pilot_estimates, nof_pilots_x_symbol, DMRS_SCH_SMOOTH_FILTER_LEN);
#endif // DMRS_SCH_SMOOTH_FILTER_LEN

  // Frequency domain interpolate
  uint32_t nof_re_x_symbol =
      (dmrs_cfg->type == srsran_dmrs_sch_type_1) ? nof_pilots_x_symbol * 2 : nof_pilots_x_symbol * 3;
  if (dmrs_cfg->type == srsran_dmrs_sch_type_1) {
    // Prepare interpolator
    if (srsran_interp_linear_resize(&q->interpolator_type1, nof_pilots_x_symbol, 2) < SRSRAN_SUCCESS) {
      ERROR("Resizing interpolator nof_pilots_x_symbol=%d; M=%d;", nof_pilots_x_symbol, 2);
      return SRSRAN_ERROR;
    }

    // Interpolate
    srsran_interp_linear_offset(&q->interpolator_type1, q->pilot_estimates, ce, delta, 2 - delta);

  } else {
    // Prepare interpolator
    if (srsran_interp_linear_resize(&q->interpolator_type2, nof_pilots_x_symbol, 3) < SRSRAN_SUCCESS) {
      ERROR("Resizing interpolator nof_pilots_x_symbol=%d; M=%d;", nof_pilots_x_symbol, 3);
      return SRSRAN_ERROR;
    }

    // Interpolate
    srsran_interp_linear_offset(&q->interpolator_type2, q->pilot_estimates, ce, delta, 3 - delta);
  }

#if DMRS_SCH_SYNC_PRECOMPENSATE
  // Remove synchronization error pre-compensation
  if (isnormal(sync_err)) {
    srsran_vec_apply_cfo(ce, -sync_err / dmrs_stride, ce, nof_re_x_symbol);
  }
#endif // DMRS_SCH_SYNC_ERROR_PRECOMPENSATE

  // Time domain hold, extract resource elements estimates for PDSCH
  uint32_t count = 0;
  for (uint32_t l = grant->S; l < grant->S + grant->L; l++) {
    // Initialise reserved mask
    bool rvd_mask_wb[SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};

    // Compute reserved RE mask by procedures
    if (srsran_re_pattern_list_to_symbol_mask(&cfg->rvd_re, l, rvd_mask_wb) < SRSRAN_SUCCESS) {
      ERROR("Error generating reserved RE mask");
      return SRSRAN_ERROR;
    }

    // Compute reserved RE mask for DMRS
    if (srsran_re_pattern_to_symbol_mask(&dmrs_pattern, l, rvd_mask_wb) < SRSRAN_SUCCESS) {
      ERROR("Error generating reserved RE mask");
      return SRSRAN_ERROR;
    }

    // Narrow reserved subcarriers to the ones used in the transmission
    bool rvd_mask[SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};
    for (uint32_t i = 0, k = 0; i < q->carrier.nof_prb; i++) {
      if (grant->prb_idx[i]) {
        for (uint32_t j = 0; j < SRSRAN_NRE; j++) {
          rvd_mask[k++] = rvd_mask_wb[i * SRSRAN_NRE + j];
        }
      }
    }

    for (uint32_t i = 0; i < nof_re_x_symbol; i++) {
      if (!rvd_mask[i]) {
#if DMRS_SCH_CFO_PRECOMPENSATE
        chest_res->ce[0][0][count++] = ce[i] * cfo_correction[l];
#else  // DMRS_SCH_CFO_PRECOMPENSATE
        chest_res->ce[0][0][count++] = ce[i];
#endif // DMRS_SCH_CFO_PRECOMPENSATE
      }
    }
  }
  // Set other values in the estimation result
  chest_res->nof_re = count;

  return SRSRAN_SUCCESS;
}
