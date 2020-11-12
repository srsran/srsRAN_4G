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

#include "srslte/phy/ch_estimation/dmrs_pdsch.h"
#include <srslte/phy/utils/debug.h>

#define SRSLTE_DMRS_PDSCH_TYPEA_SINGLE_DURATION_MIN 3
#define SRSLTE_DMRS_PDSCH_TYPEA_DOUBLE_DURATION_MIN 4

int srslte_dmrs_pdsch_cfg_to_str(const srslte_pdsch_dmrs_cfg_t* cfg, char* msg, uint32_t max_len)
{
  int type           = (int)cfg->type + 1;
  int typeA_pos      = (int)cfg->typeA_pos + 2;
  int additional_pos = cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0
                           ? 0
                           : cfg->additional_pos == srslte_dmrs_pdsch_add_pos_1
                                 ? 1
                                 : cfg->additional_pos == srslte_dmrs_pdsch_add_pos_2 ? 2 : 3;
  const char* len = cfg->length == srslte_dmrs_pdsch_len_1 ? "single" : "double";

  return srslte_print_check(
      msg, max_len, 0, "type=%d, typeA_pos=%d, add_pos=%d, len=%s", type, typeA_pos, additional_pos, len);
}

static uint32_t
srslte_dmrs_get_pilots_type1(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, const cf_t* symbols, cf_t* pilots)
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
srslte_dmrs_get_pilots_type2(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, const cf_t* symbols, cf_t* pilots)
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

static uint32_t srslte_dmrs_get_lse(srslte_dmrs_pdsch_t*     q,
                                    srslte_sequence_state_t* sequence_state,
                                    srslte_dmrs_pdsch_type_t dmrs_type,
                                    uint32_t                 start_prb,
                                    uint32_t                 nof_prb,
                                    uint32_t                 delta,
                                    const cf_t*              symbols,
                                    cf_t*                    least_square_estimates)
{
  uint32_t count = 0;

  switch (dmrs_type) {

    case srslte_dmrs_pdsch_type_1:
      count = srslte_dmrs_get_pilots_type1(start_prb, nof_prb, delta, symbols, least_square_estimates);
      break;
    case srslte_dmrs_pdsch_type_2:
      count = srslte_dmrs_get_pilots_type2(start_prb, nof_prb, delta, symbols, least_square_estimates);
      break;
    default:
      ERROR("Unknown DMRS type.\n");
  }

  // Generate sequence for the given pilots
  srslte_sequence_state_gen_f(sequence_state, M_SQRT1_2, (float*)q->temp, count * 2);

  // Calculate least square estimates
  srslte_vec_prod_conj_ccc(least_square_estimates, q->temp, least_square_estimates, count);

  return count;
}

static uint32_t
srslte_dmrs_put_pilots_type1(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, cf_t* symbols, const cf_t* pilots)
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
srslte_dmrs_put_pilots_type2(uint32_t start_prb, uint32_t nof_prb, uint32_t delta, cf_t* symbols, const cf_t* pilots)
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

static uint32_t srslte_dmrs_put_pilots(srslte_dmrs_pdsch_t*     q,
                                       srslte_sequence_state_t* sequence_state,
                                       srslte_dmrs_pdsch_type_t dmrs_type,
                                       uint32_t                 start_prb,
                                       uint32_t                 nof_prb,
                                       uint32_t                 delta,
                                       cf_t*                    symbols)
{
  uint32_t count = (dmrs_type == srslte_dmrs_pdsch_type_1) ? nof_prb * 6 : nof_prb * 4;

  // Generate sequence for the given pilots
  srslte_sequence_state_gen_f(sequence_state, M_SQRT1_2, (float*)q->temp, count * 2);

  switch (dmrs_type) {

    case srslte_dmrs_pdsch_type_1:
      count = srslte_dmrs_put_pilots_type1(start_prb, nof_prb, delta, symbols, q->temp);
      break;
    case srslte_dmrs_pdsch_type_2:
      count = srslte_dmrs_put_pilots_type2(start_prb, nof_prb, delta, symbols, q->temp);
      break;
    default:
      ERROR("Unknown DMRS type.\n");
  }

  return count;
}

static int srslte_dmrs_pdsch_put_symbol(srslte_dmrs_pdsch_t*           q,
                                        const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                                        const srslte_pdsch_grant_nr_t* grant,
                                        uint32_t                       cinit,
                                        uint32_t                       delta,
                                        cf_t*                          symbols)
{
  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &pdsch_cfg->dmrs_cfg_typeA : &pdsch_cfg->dmrs_cfg_typeB;
  uint32_t prb_count        = 0; // Counts consecutive used PRB
  uint32_t prb_start        = 0; // Start consecutive used PRB
  uint32_t prb_skip         = 0; // Number of PRB to skip
  uint32_t nof_pilots_x_prb = dmrs_cfg->type == srslte_dmrs_pdsch_type_1 ? 6 : 4;
  uint32_t pilot_count      = 0;

  // Initialise sequence
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);

  // Iterate over PRBs
  for (uint32_t prb_idx = 0; prb_idx < q->carrier.nof_prb; prb_idx++) {
    // If the PRB is used for PDSCH transmission count
    if (grant->prb_idx[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... save first consecutive PRB in the group
        prb_start = prb_idx;

        // ... discard unused pilots and reset counter
        srslte_sequence_state_advance(&sequence_state, prb_skip * nof_pilots_x_prb * 2);
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
    pilot_count += srslte_dmrs_put_pilots(
        q, &sequence_state, dmrs_cfg->type, prb_start, prb_count, delta, &symbols[prb_start * SRSLTE_NRE]);

    // Reset counter
    prb_count = 0;
  }

  if (prb_count > 0) {
    pilot_count += srslte_dmrs_put_pilots(
        q, &sequence_state, dmrs_cfg->type, prb_start, prb_count, delta, &symbols[prb_start * SRSLTE_NRE]);
  }

  return pilot_count;
}

// Implements 3GPP 38.211 R.15 Table 7.4.1.1.2-3 PDSCH mapping type A Single
static int srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_single(const srslte_pdsch_dmrs_cfg_t* dmrs_cfg,
                                                                   uint32_t                       ld,
                                                                   uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS])
{
  int count = 0;

  if (ld < SRSLTE_DMRS_PDSCH_TYPEA_SINGLE_DURATION_MIN) {
    ERROR("Duration is below the minimum\n");
    return SRSLTE_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (dmrs_cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3) ? 3 : 2;

  // For PDSCH mapping Type A single-symbol DM-RS, l1 = 11 except if all of the following conditions are fulfilled in
  // which case l1 = 12:
  // - the higher-layer parameter lte-CRS-ToMatchAround is configured; and
  // - the higher-layer parameters dmrs-AdditionalPosition is equal to 'pos1' and l0 = 3; and
  // - the UE has indicated it is capable of additionalDMRS-DL-Alt
  int l1 = 11;
  if (dmrs_cfg->lte_CRS_to_match_around && dmrs_cfg->additional_pos == srslte_dmrs_pdsch_add_pos_1 &&
      dmrs_cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3 && dmrs_cfg->additional_DMRS_DL_Alt) {
    l1 = 12;
  }

  symbols[count] = l0;
  count++;

  if (ld < 8 || dmrs_cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0) {
    return count;
  }

  if (ld < 10) {
    symbols[count] = 7;
    count++;
  } else if (ld < 12) {
    if (dmrs_cfg->additional_pos > srslte_dmrs_pdsch_add_pos_2) {
      symbols[count] = 6;
      count++;
    }

    symbols[count] = 9;
    count++;

  } else if (ld == 12) {
    switch (dmrs_cfg->additional_pos) {
      case srslte_dmrs_pdsch_add_pos_1:
        symbols[count] = 9;
        count++;
        break;
      case srslte_dmrs_pdsch_add_pos_2:
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
      case srslte_dmrs_pdsch_add_pos_1:
        symbols[count] = l1;
        count++;
        break;
      case srslte_dmrs_pdsch_add_pos_2:
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
static int srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_double(const srslte_pdsch_dmrs_cfg_t* dmrs_cfg,
                                                                   uint32_t                       ld,
                                                                   uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS])
{
  int count = 0;

  if (ld < SRSLTE_DMRS_PDSCH_TYPEA_DOUBLE_DURATION_MIN) {
    return SRSLTE_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (dmrs_cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3) ? 3 : 2;

  symbols[count] = l0;
  count++;
  symbols[count] = symbols[count - 1] + 1;
  count++;

  if (ld < 10 || dmrs_cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0) {
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

int srslte_dmrs_pdsch_get_symbols_idx(const srslte_pdsch_cfg_nr_t*   cfg,
                                      const srslte_pdsch_grant_nr_t* grant,
                                      uint32_t                       symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS])
{
  // The position(s) of the DM-RS symbols is given by l and duration ld where
  // - for PDSCH mapping type A, ld is the duration between the first OFDM symbol of the slot and the last OFDM symbol
  //   of the scheduled PDSCH resources in the slot
  // - for PDSCH mapping type B, ld is the duration of the scheduled PDSCH resources
  uint32_t ld = grant->L;
  if (grant->mapping == srslte_pdsch_mapping_type_A) {
    ld = grant->S + grant->L;
  }

  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &cfg->dmrs_cfg_typeA : &cfg->dmrs_cfg_typeB;

  switch (grant->mapping) {
    case srslte_pdsch_mapping_type_A:
      // The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal to 'pos2'
      if (dmrs_cfg->typeA_pos != srslte_dmrs_pdsch_typeA_pos_2 &&
          dmrs_cfg->additional_pos == srslte_dmrs_pdsch_add_pos_3) {
        ERROR("The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal "
              "to 'pos2'\n");
        return SRSLTE_ERROR;
      }

      // For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively is only
      // applicable when dmrs-TypeA-Position is equal to 'pos2
      if ((ld == 3 || ld == 4) && dmrs_cfg->typeA_pos != srslte_dmrs_pdsch_typeA_pos_2) {
        ERROR("For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively "
              "is only applicable when dmrs-TypeA-Position is equal to 'pos2\n");
        return SRSLTE_ERROR;
      }

      if (dmrs_cfg->length == srslte_dmrs_pdsch_len_1) {
        return srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_single(dmrs_cfg, ld, symbols);
      }
      return srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_double(dmrs_cfg, ld, symbols);
    case srslte_pdsch_mapping_type_B:
      ERROR("Error PDSCH mapping type B not supported\n");
      return SRSLTE_ERROR;
  }

  return SRSLTE_ERROR;
}

int srslte_dmrs_pdsch_get_sc_idx(const srslte_pdsch_dmrs_cfg_t* cfg, uint32_t max_count, uint32_t* sc_idx)
{
  int      count = 0;
  uint32_t delta = 0;

  if (cfg->type == srslte_dmrs_pdsch_type_1) {
    for (uint32_t n = 0; count < max_count; n += 4) {
      for (uint32_t k_prime = 0; k_prime < 2 && count < max_count; k_prime++) {
        sc_idx[count++] = n + 2 * k_prime + delta;
      }
    }
  } else {
    for (uint32_t n = 0; count < max_count; n += 6) {
      for (uint32_t k_prime = 0; k_prime < 2 && count < max_count; k_prime++) {
        sc_idx[count++] = n + k_prime + delta;
      }
    }
  }

  return count;
}

int srslte_dmrs_pdsch_get_N_prb(const srslte_pdsch_cfg_nr_t* cfg, const srslte_pdsch_grant_nr_t* grant)
{
  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &cfg->dmrs_cfg_typeA : &cfg->dmrs_cfg_typeB;

  // Get number of frequency domain resource elements used for DMRS
  int nof_sc = dmrs_cfg->type == srslte_dmrs_pdsch_type_1 ? 6 : 4;

  // Get number of symbols used for DMRS
  uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS] = {};
  int      ret                                    = srslte_dmrs_pdsch_get_symbols_idx(cfg, grant, symbols);
  if (ret < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  return nof_sc * ret;
}

static uint32_t srslte_dmrs_pdsch_seed(const srslte_carrier_nr_t*     carrier,
                                       const srslte_pdsch_cfg_nr_t*   cfg,
                                       const srslte_pdsch_grant_nr_t* grant,
                                       uint32_t                       slot_idx,
                                       uint32_t                       symbol_idx)
{
  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &cfg->dmrs_cfg_typeA : &cfg->dmrs_cfg_typeB;

  // Calculate scrambling IDs
  uint32_t n_id   = carrier->id;
  uint32_t n_scid = (grant->n_scid) ? 1 : 0;
  if (!grant->n_scid && dmrs_cfg->scrambling_id0_present) {
    // n_scid = 0 and ID0 present
    n_id = dmrs_cfg->scrambling_id0;
  } else if (grant->n_scid && dmrs_cfg->scrambling_id1_present) {
    // n_scid = 1 and ID1 present
    n_id = dmrs_cfg->scrambling_id1;
  }

  return (uint32_t)(((((SRSLTE_MAX_NSYMB * slot_idx + symbol_idx + 1UL) * (2UL * n_id + 1UL)) << 17UL) +
                     (2UL * carrier->id + n_scid)) &
                    (uint64_t)INT32_MAX);
}

int srslte_dmrs_pdsch_init(srslte_dmrs_pdsch_t* q, bool is_ue)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (is_ue) {
    q->is_ue = true;
  }

  return SRSLTE_SUCCESS;
}

void srslte_dmrs_pdsch_free(srslte_dmrs_pdsch_t* q)
{
  if (q == NULL) {
    return;
  }

  srslte_interp_linear_free(&q->interpolator_type1);
  srslte_interp_linear_free(&q->interpolator_type2);
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }
  if (q->temp) {
    free(q->temp);
  }
}

int srslte_dmrs_pdsch_set_carrier(srslte_dmrs_pdsch_t* q, const srslte_carrier_nr_t* carrier)
{
  bool max_nof_prb_changed = q->max_nof_prb < carrier->nof_prb;

  // Set carrier and update maximum number of PRB
  q->carrier     = *carrier;
  q->max_nof_prb = SRSLTE_MAX(q->max_nof_prb, carrier->nof_prb);

  // Resize/allocate temp for gNb and UE
  if (max_nof_prb_changed) {
    if (q->temp) {
      free(q->temp);
    }

    q->temp = srslte_vec_cf_malloc(q->max_nof_prb * SRSLTE_NRE);
    if (!q->temp) {
      ERROR("malloc\n");
      return SRSLTE_ERROR;
    }
  }

  // If it is not UE, quit now
  if (!q->is_ue) {
    return SRSLTE_SUCCESS;
  }

  if (max_nof_prb_changed) {
    // Resize interpolator only if the number of PRB has increased
    srslte_interp_linear_free(&q->interpolator_type1);
    srslte_interp_linear_free(&q->interpolator_type2);

    if (srslte_interp_linear_init(&q->interpolator_type1, carrier->nof_prb * SRSLTE_NRE / 2, 2) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }
    if (srslte_interp_linear_init(&q->interpolator_type2, carrier->nof_prb * SRSLTE_NRE / 3, 6) != SRSLTE_SUCCESS) {
      return SRSLTE_ERROR;
    }

    if (q->pilot_estimates) {
      free(q->pilot_estimates);
    }

    // The maximum number of pilots is for Type 1
    q->pilot_estimates = srslte_vec_cf_malloc(SRSLTE_DMRS_PDSCH_MAX_SYMBOLS * q->max_nof_prb * SRSLTE_NRE / 2);
    if (!q->pilot_estimates) {
      ERROR("malloc\n");
      return SRSLTE_ERROR;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdsch_put_sf(srslte_dmrs_pdsch_t*           q,
                             const srslte_dl_slot_cfg_t*    slot_cfg,
                             const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                             const srslte_pdsch_grant_nr_t* grant,
                             cf_t*                          sf_symbols)
{
  uint32_t delta = 0;

  if (q == NULL || slot_cfg == NULL || pdsch_cfg == NULL || sf_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t symbol_sz = q->carrier.nof_prb * SRSLTE_NRE; // Symbol size in resource elements

  // Get symbols indexes
  uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS] = {};
  int      nof_symbols                            = srslte_dmrs_pdsch_get_symbols_idx(pdsch_cfg, grant, symbols);
  if (nof_symbols < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  // Iterate symbols
  for (uint32_t i = 0; i < nof_symbols; i++) {
    uint32_t l        = symbols[i];    // Symbol index inside the slot
    uint32_t slot_idx = slot_cfg->idx; // Slot index in the frame
    uint32_t cinit    = srslte_dmrs_pdsch_seed(&q->carrier, pdsch_cfg, grant, slot_idx, l);

    srslte_dmrs_pdsch_put_symbol(q, pdsch_cfg, grant, cinit, delta, &sf_symbols[symbol_sz * l]);
  }

  return SRSLTE_SUCCESS;
}

static int srslte_dmrs_pdsch_get_symbol(srslte_dmrs_pdsch_t*           q,
                                        const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                                        const srslte_pdsch_grant_nr_t* grant,
                                        uint32_t                       cinit,
                                        uint32_t                       delta,
                                        const cf_t*                    symbols,
                                        cf_t*                          least_square_estimates)
{
  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &pdsch_cfg->dmrs_cfg_typeA : &pdsch_cfg->dmrs_cfg_typeB;

  uint32_t prb_count        = 0; // Counts consecutive used PRB
  uint32_t prb_start        = 0; // Start consecutive used PRB
  uint32_t prb_skip         = 0; // Number of PRB to skip
  uint32_t nof_pilots_x_prb = dmrs_cfg->type == srslte_dmrs_pdsch_type_1 ? 6 : 4;
  uint32_t pilot_count      = 0;

  // Initialise sequence
  srslte_sequence_state_t sequence_state = {};
  srslte_sequence_state_init(&sequence_state, cinit);

  // Iterate over PRBs
  for (uint32_t prb_idx = 0; prb_idx < q->carrier.nof_prb; prb_idx++) {
    // If the PRB is used for PDSCH transmission count
    if (grant->prb_idx[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... save first consecutive PRB in the group
        prb_start = prb_idx;

        // ... discard unused pilots and reset counter
        srslte_sequence_state_advance(&sequence_state, prb_skip * nof_pilots_x_prb * 2);
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
    pilot_count += srslte_dmrs_get_lse(q,
                                       &sequence_state,
                                       dmrs_cfg->type,
                                       prb_start,
                                       prb_count,
                                       delta,
                                       &symbols[prb_start * SRSLTE_NRE],
                                       &least_square_estimates[pilot_count]);

    // Reset counter
    prb_count = 0;
  }

  if (prb_count > 0) {
    pilot_count += srslte_dmrs_get_lse(q,
                                       &sequence_state,
                                       dmrs_cfg->type,
                                       prb_start,
                                       prb_count,
                                       delta,
                                       &symbols[prb_start * SRSLTE_NRE],
                                       &least_square_estimates[pilot_count]);
  }

  return pilot_count;
}

int srslte_dmrs_pdsch_estimate(srslte_dmrs_pdsch_t*           q,
                               const srslte_dl_slot_cfg_t*    slot_cfg,
                               const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                               const srslte_pdsch_grant_nr_t* grant,
                               const cf_t*                    sf_symbols,
                               srslte_chest_dl_res_t*         chest_res)
{
  const uint32_t delta = 0;

  if (q == NULL || slot_cfg == NULL || sf_symbols == NULL || chest_res == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  const srslte_pdsch_dmrs_cfg_t* dmrs_cfg =
      grant->mapping == srslte_pdsch_mapping_type_A ? &pdsch_cfg->dmrs_cfg_typeA : &pdsch_cfg->dmrs_cfg_typeB;

  cf_t*    ce        = q->temp;
  uint32_t symbol_sz = q->carrier.nof_prb * SRSLTE_NRE; // Symbol size in resource elements

  // Get symbols indexes
  uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS] = {};
  int      nof_symbols                            = srslte_dmrs_pdsch_get_symbols_idx(pdsch_cfg, grant, symbols);
  if (nof_symbols < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  uint32_t nof_pilots_x_symbol = 0;

  // Iterate symbols
  for (uint32_t i = 0; i < nof_symbols; i++) {
    uint32_t l = symbols[i]; // Symbol index inside the slot

    uint32_t cinit = srslte_dmrs_pdsch_seed(&q->carrier, pdsch_cfg, grant, slot_cfg->idx, l);

    nof_pilots_x_symbol = srslte_dmrs_pdsch_get_symbol(
        q, pdsch_cfg, grant, cinit, delta, &sf_symbols[symbol_sz * l], &q->pilot_estimates[nof_pilots_x_symbol * i]);
  }

  // Perform measurements here
  // ...

  // Average over time, only if more than one DMRS symbol
  for (uint32_t i = 1; i < nof_symbols; i++) {
    srslte_vec_sum_ccc(
        q->pilot_estimates, &q->pilot_estimates[nof_pilots_x_symbol * i], q->pilot_estimates, nof_pilots_x_symbol);
  }
  if (nof_symbols > 0) {
    srslte_vec_sc_prod_cfc(q->pilot_estimates, 1.0f / (float)nof_symbols, q->pilot_estimates, nof_pilots_x_symbol);
  }

  // Frequency domain interpolate
  uint32_t nof_re_x_symbol =
      (dmrs_cfg->type == srslte_dmrs_pdsch_type_1) ? nof_pilots_x_symbol * 2 : nof_pilots_x_symbol * 3;
  if (dmrs_cfg->type == srslte_dmrs_pdsch_type_1) {
    // Prepare interpolator
    srslte_interp_linear_resize(&q->interpolator_type1, nof_pilots_x_symbol, 2);

    // Interpolate
    srslte_interp_linear_offset(&q->interpolator_type1, q->pilot_estimates, ce, delta, 2 - delta);

  } else {
    // Prepare interpolator
    srslte_interp_linear_resize(&q->interpolator_type2, nof_pilots_x_symbol, 3);

    // Interpolate
    srslte_interp_linear_offset(&q->interpolator_type2, q->pilot_estimates, ce, delta, 3 - delta);
  }

  // Time domain hold, extract resource elements estimates for PDSCH
  uint32_t symbol_idx = 0;
  uint32_t count      = 0;
  for (uint32_t l = grant->S; l < grant->S + grant->L; l++) {
    while (symbols[symbol_idx] < l && symbol_idx < nof_symbols) {
      symbol_idx++;
    }

    if (symbols[symbol_idx] == l) {
      switch (dmrs_cfg->type) {

        case srslte_dmrs_pdsch_type_1:
          for (uint32_t i = 0; i < nof_re_x_symbol; i++) {
            if (i % 2 != delta) {
              chest_res->ce[0][0][count] = ce[i];
              count++;
            }
          }
          break;
        case srslte_dmrs_pdsch_type_2:
          for (uint32_t i = 0; i < nof_re_x_symbol; i++) {
            if ((i % 6 != delta) && (i % 6 != delta + 1)) {
              chest_res->ce[0][0][count] = ce[i];
              count++;
            }
          }
          break;
      }
    } else {
      srslte_vec_cf_copy(&chest_res->ce[0][0][count], ce, nof_re_x_symbol);
      count += nof_re_x_symbol;
    }
  }
  // Set other values in the estimation result
  chest_res->nof_re = count;

  return SRSLTE_SUCCESS;
}