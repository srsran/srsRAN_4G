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

int srslte_dmrs_pdsch_cfg_to_str(const srslte_dmrs_pdsch_cfg_t* cfg, char* msg, uint32_t max_len)
{
  int type           = (int)cfg->type + 1;
  int typeA_pos      = (int)cfg->typeA_pos + 2;
  int additional_pos = cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0
                           ? 0
                           : cfg->additional_pos == srslte_dmrs_pdsch_add_pos_1
                                 ? 1
                                 : cfg->additional_pos == srslte_dmrs_pdsch_add_pos_2 ? 2 : 3;
  const char* len     = cfg->length == srslte_dmrs_pdsch_len_1 ? "single" : "double";
  const char* mapping = cfg->mapping_type == srslte_dmrs_pdsch_mapping_type_A ? "A" : "B";

  return srslte_print_check(msg,
                            max_len,
                            0,
                            "mapping=%s, type=%d, typeA_pos=%d, add_pos=%d, len=%s",
                            mapping,
                            type,
                            typeA_pos,
                            additional_pos,
                            len);
}

// Implements 3GPP 38.211 R.15 Table 7.4.1.1.2-3 PDSCH mapping type A Single
static int srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_single(const srslte_dmrs_pdsch_cfg_t* cfg,
                                                                   uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS])
{
  int count = 0;

  if (cfg->duration < SRSLTE_DMRS_PDSCH_TYPEA_SINGLE_DURATION_MIN) {
    ERROR("Duration is below the minimum\n");
    return SRSLTE_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3) ? 3 : 2;

  // For PDSCH mapping Type A single-symbol DM-RS, l1 = 11 except if all of the following conditions are fulfilled in
  // which case l1 = 12:
  // - the higher-layer parameter lte-CRS-ToMatchAround is configured; and
  // - the higher-layer parameters dmrs-AdditionalPosition is equal to 'pos1' and l0 = 3; and
  // - the UE has indicated it is capable of additionalDMRS-DL-Alt
  int l1 = 11;
  if (cfg->lte_CRS_to_match_around && cfg->additional_pos == srslte_dmrs_pdsch_add_pos_1 &&
      cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3 && cfg->additional_DMRS_DL_Alt) {
    l1 = 12;
  }

  symbols[count] = l0;
  count++;

  if (cfg->duration < 8 || cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0) {
    return count;
  }

  if (cfg->duration < 10) {
    symbols[count] = 7;
    count++;
  } else if (cfg->duration < 12) {
    if (cfg->additional_pos > srslte_dmrs_pdsch_add_pos_2) {
      symbols[count] = 6;
      count++;
    }

    symbols[count] = 9;
    count++;

  } else if (cfg->duration == 12) {
    switch (cfg->additional_pos) {
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
    switch (cfg->additional_pos) {
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
static int srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_double(const srslte_dmrs_pdsch_cfg_t* cfg,
                                                                   uint32_t symbols[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS])
{
  int count = 0;

  if (cfg->duration < SRSLTE_DMRS_PDSCH_TYPEA_DOUBLE_DURATION_MIN) {
    return SRSLTE_ERROR;
  }

  // l0 = 3 if the higher-layer parameter dmrs-TypeA-Position is equal to 'pos3' and l0 = 2 otherwise
  int l0 = (cfg->typeA_pos == srslte_dmrs_pdsch_typeA_pos_3) ? 3 : 2;

  symbols[count] = l0;
  count++;
  symbols[count] = symbols[count - 1] + 1;
  count++;

  if (cfg->duration < 10 || cfg->additional_pos == srslte_dmrs_pdsch_add_pos_0) {
    return count;
  }

  if (cfg->duration < 13) {
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

static int srslte_dmrs_pdsch_get_symbols_idx(const srslte_dmrs_pdsch_cfg_t* cfg, uint32_t* symbols)
{
  switch (cfg->mapping_type) {
    case srslte_dmrs_pdsch_mapping_type_A:
      // The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal to 'pos2'
      if (cfg->typeA_pos != srslte_dmrs_pdsch_typeA_pos_2 && cfg->additional_pos == srslte_dmrs_pdsch_add_pos_3) {
        ERROR("The case dmrs-AdditionalPosition equals to 'pos3' is only supported when dmrs-TypeA-Position is equal "
              "to 'pos2'\n");
        return SRSLTE_ERROR;
      }

      // For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively is only
      // applicable when dmrs-TypeA-Position is equal to 'pos2
      if ((cfg->duration == 3 || cfg->duration == 4) && cfg->typeA_pos != srslte_dmrs_pdsch_typeA_pos_2) {
        ERROR("For PDSCH mapping type A, ld = 3 and ld = 4 symbols in Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 respectively "
              "is only applicable when dmrs-TypeA-Position is equal to 'pos2\n");
        return SRSLTE_ERROR;
      }

      if (cfg->length == srslte_dmrs_pdsch_len_1) {
        return srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_single(cfg, symbols);
      }
      return srslte_dmrs_pdsch_get_symbols_idx_mapping_type_A_double(cfg, symbols);
    case srslte_dmrs_pdsch_mapping_type_B:
      ERROR("Error PDSCH mapping type B not supported\n");
      return SRSLTE_ERROR;
  }

  return SRSLTE_ERROR;
}

static int srslte_dmrs_pdsch_get_sc_idx(const srslte_dmrs_pdsch_cfg_t* cfg, uint32_t sc_idx[SRSLTE_NRE])
{
  int      count = 0;
  uint32_t delta = 0;

  if (cfg->type == srslte_dmrs_pdsch_type_1) {
    for (uint32_t n = 0; n < SRSLTE_NRE; n += 4) {
      for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
        sc_idx[count++] = n + 2 * k_prime + delta;
      }
    }
  } else {
    for (uint32_t n = 0; n < SRSLTE_NRE; n += 6) {
      for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
        sc_idx[count++] = n + k_prime + delta;
      }
    }
  }

  return count;
}

static uint32_t srslte_dmrs_pdsch_seed(const srslte_dmrs_pdsch_cfg_t* cfg, uint32_t slot_idx, uint32_t symbol_idx)
{
  return (uint32_t)(((((SRSLTE_MAX_NSYMB * slot_idx + symbol_idx + 1UL) * (2UL * cfg->n_id + 1UL)) << 17UL) +
                     (2UL * cfg->n_id + cfg->n_scid)) &
                    (uint64_t)INT32_MAX);
}

int srslte_dmrs_pdsch_init(srslte_dmrs_pdsch_t* q, const srslte_dmrs_pdsch_cfg_t* cfg)
{
  // Copy new configuration
  q->cfg = *cfg;

  // Calculate the symbols that carry PDSCH DMRS
  int n = srslte_dmrs_pdsch_get_symbols_idx(&q->cfg, q->symbols_idx);
  if (n < SRSLTE_SUCCESS) {
    ERROR("Getting symbols indexes\n");
    return n;
  }
  q->nof_symbols = (uint32_t)n;

  // Calculate the sub-carrier index that carry PDSCH DMRS
  n = srslte_dmrs_pdsch_get_sc_idx(&q->cfg, q->sc_idx);
  if (n < SRSLTE_SUCCESS) {
    ERROR("Getting sub-carriers indexes\n");
    return n;
  }
  q->nof_sc = (uint32_t)n;

  return SRSLTE_SUCCESS;
}
#define SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE 64

int srslte_dmrs_pdsch_put_sf(srslte_dmrs_pdsch_t* q, const srslte_dl_sf_cfg_t* sf, cf_t* sf_symbols)
{
  uint32_t delta = 0;

  // Iterate symbols
  for (uint32_t i = 0; i < q->nof_symbols; i++) {
    uint32_t l         = q->symbols_idx[i]; // Symbol index inside the sub-frame
    uint32_t slot_idx  = (sf->tti % SRSLTE_NOF_SF_X_FRAME) * SRSLTE_NOF_SLOTS_PER_SF; // Slot index in the frame
    uint32_t symbol_sz = q->cfg.nof_prb * SRSLTE_NRE; // Symbol size in resource elements

    srslte_sequence_state_t sequence_state = {};
    srslte_sequence_state_init(&sequence_state, srslte_dmrs_pdsch_seed(&q->cfg, slot_idx, l));

    // Generate
    uint32_t k_end = q->cfg.nof_prb * SRSLTE_NRE;
    for (uint32_t k = delta; k < k_end;) {

      cf_t temp_pilots[SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE] = {};
      srslte_sequence_state_gen_f(
          &sequence_state, M_SQRT1_2, (float*)temp_pilots, 2 * SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE);
      switch (q->cfg.type) {

        case srslte_dmrs_pdsch_type_1:
          for (uint32_t idx = 0; idx < SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE && k < k_end; k += 2) {
            sf_symbols[l * symbol_sz + k] = temp_pilots[idx++];
          }
          break;
        case srslte_dmrs_pdsch_type_2:
          for (uint32_t idx = 0; idx < SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE && k < k_end; k += 6) {
            sf_symbols[l * symbol_sz + k]     = temp_pilots[idx++];
            sf_symbols[l * symbol_sz + k + 1] = temp_pilots[idx++];
          }
          break;
      }
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_dmrs_pdsch_get_sf(srslte_dmrs_pdsch_t*      q,
                             const srslte_dl_sf_cfg_t* sf,
                             const cf_t*               sf_symbols,
                             cf_t*                     least_square_estimates)
{
  uint32_t delta = 0;

  // Iterate symbols
  for (uint32_t i = 0; i < q->nof_symbols; i++) {
    uint32_t l         = q->symbols_idx[i]; // Symbol index inside the sub-frame
    uint32_t slot_idx  = (sf->tti % SRSLTE_NOF_SF_X_FRAME) * SRSLTE_NOF_SLOTS_PER_SF; // Slot index in the frame
    uint32_t symbol_sz = q->cfg.nof_prb * SRSLTE_NRE; // Symbol size in resource elements

    srslte_sequence_state_t sequence_state = {};
    srslte_sequence_state_init(&sequence_state, srslte_dmrs_pdsch_seed(&q->cfg, slot_idx, l));

    uint32_t n_end = q->cfg.nof_prb * SRSLTE_NRE;
    for (uint32_t n = 0; n < n_end;) {

      cf_t temp_pilots[SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE];
      srslte_sequence_state_gen_f(
          &sequence_state, M_SQRT1_2, (float*)temp_pilots, 2 * SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE);

      switch (q->cfg.type) {

        case srslte_dmrs_pdsch_type_1:
          for (uint32_t idx = 0; idx < SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE && n < n_end; n += 4) {
            for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
              uint32_t k                  = n + 2 * k_prime + delta;
              *(least_square_estimates++) = sf_symbols[l * symbol_sz + k] * conjf(temp_pilots[idx++]);
            }
          }
          break;
        case srslte_dmrs_pdsch_type_2:
          for (uint32_t idx = 0; idx < SRSLTE_DMRS_PDSCH_TEMP_PILOT_SIZE && n < n_end; n += 6) {
            for (uint32_t k_prime = 0; k_prime < 2; k_prime++) {
              uint32_t k                  = n + k_prime + delta;
              *(least_square_estimates++) = sf_symbols[l * symbol_sz + k] * conjf(temp_pilots[idx++]);
            }
          }
          break;
      }
    }
  }

  return SRSLTE_SUCCESS;
}
