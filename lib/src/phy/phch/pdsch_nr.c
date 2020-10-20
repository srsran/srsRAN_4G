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
#include "srslte/phy/phch/pdsch_nr.h"

/**
 * @brief copies a number of countiguous Resource Elements
 * @param sf_symbols slot symbols in frequency domain
 * @param symbols resource elements
 * @param count number of resource elements to copy
 * @param put Direction, symbols are copied into sf_symbols if put is true, otherwise sf_symbols are copied into symbols
 */
static void srslte_pdsch_re_cp(cf_t* sf_symbols, cf_t* symbols, uint32_t count, bool put)
{
  if (put) {
    srslte_vec_cf_copy(sf_symbols, symbols, count);
  } else {
    srslte_vec_cf_copy(symbols, sf_symbols, count);
  }
}

static uint32_t srslte_pdsch_nr_cp_dmrs_type1(const srslte_pdsch_nr_t*     q,
                                              const srslte_pdsch_cfg_nr_t* cfg,
                                              cf_t*                        symbols,
                                              cf_t*                        sf_symbols,
                                              bool                         put)
{
  uint32_t count = 0;
  uint32_t delta = 0;

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (cfg->grant.prb_idx[i]) {
      for (uint32_t j = 0; j < SRSLTE_NRE; j += 2) {
        if (put) {
          sf_symbols[i * SRSLTE_NRE + delta + j] = symbols[count++];
        } else {
          symbols[count++] = sf_symbols[i * SRSLTE_NRE + delta + j];
        }
      }
    }
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_dmrs_type2(const srslte_pdsch_nr_t*     q,
                                              const srslte_pdsch_cfg_nr_t* cfg,
                                              cf_t*                        symbols,
                                              cf_t*                        sf_symbols,
                                              bool                         put)
{
  uint32_t count = 0;
  uint32_t delta = 0;

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (cfg->grant.prb_idx[i]) {
      // Copy RE before first pilot pair
      if (delta > 0) {
        srslte_pdsch_re_cp(&sf_symbols[i * SRSLTE_NRE], &symbols[count], delta, put);
        count += delta;
      }

      // Copy RE between pilot pairs
      srslte_pdsch_re_cp(&sf_symbols[i * SRSLTE_NRE + delta + 2], &symbols[count], 4, put);
      count += 4;

      // Copy RE after second pilot
      srslte_pdsch_re_cp(&sf_symbols[(i + 1) * SRSLTE_NRE - 4 + delta], &symbols[count], 4 - delta, put);
      count += 4 - delta;
    }
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_dmrs(const srslte_pdsch_nr_t*     q,
                                        const srslte_pdsch_cfg_nr_t* cfg,
                                        cf_t*                        symbols,
                                        cf_t*                        sf_symbols,
                                        bool                         put)
{
  uint32_t count = 0;

  switch (cfg->dmrs_cfg.type) {
    case srslte_dmrs_pdsch_type_1:
      count = srslte_pdsch_nr_cp_dmrs_type1(q, cfg, symbols, sf_symbols, put);
      break;
    case srslte_dmrs_pdsch_type_2:
      count = srslte_pdsch_nr_cp_dmrs_type2(q, cfg, symbols, sf_symbols, put);
      break;
  }

  return count;
}

static uint32_t srslte_pdsch_nr_cp_clean(const srslte_pdsch_nr_t*     q,
                                         const srslte_pdsch_cfg_nr_t* cfg,
                                         cf_t*                        symbols,
                                         cf_t*                        sf_symbols,
                                         bool                         put)
{
  uint32_t count  = 0;
  uint32_t start  = 0; // Index of the start of continuous data
  uint32_t length = 0; // End of continuous RE

  for (uint32_t i = 0; i < q->carrier.nof_prb; i++) {
    if (cfg->grant.prb_idx[i]) {
      // If fist continuous block, save start
      if (length == 0) {
        start = i * SRSLTE_NRE;
      }
      length += SRSLTE_NRE;
    } else {
      // Consecutive block is finished
      if (put) {
        srslte_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
      } else {
        srslte_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
      }

      // Increase RE count
      count += length;

      // Reset consecutive block
      length = 0;
    }
  }

  // Copy last contiguous block
  if (length > 0) {
    if (put) {
      srslte_vec_cf_copy(&sf_symbols[start], &symbols[count], length);
    } else {
      srslte_vec_cf_copy(&symbols[count], &sf_symbols[start], length);
    }
    count += length;
  }

  return count;
}

static int srslte_pdsch_nr_cp(const srslte_pdsch_nr_t*     q,
                              const srslte_pdsch_cfg_nr_t* cfg,
                              cf_t*                        symbols,
                              cf_t*                        sf_symbols,
                              bool                         put)
{
  uint32_t count                                     = 0;
  uint32_t dmrs_l_idx[SRSLTE_DMRS_PDSCH_MAX_SYMBOLS] = {};
  uint32_t dmrs_l_count                              = 0;

  // Get symbol indexes carrying DMRS
  int32_t nof_dmrs_symbols = srslte_dmrs_pdsch_get_symbols_idx(cfg, dmrs_l_idx);
  if (nof_dmrs_symbols < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  for (uint32_t l = cfg->grant.S; l < cfg->grant.L; l++) {
    // Advance DMRS symbol counter until:
    // - the current DMRS symbol index is greater or equal than current symbol l
    // - no more DMRS symbols
    while (dmrs_l_idx[dmrs_l_count] < l && dmrs_l_count < nof_dmrs_symbols) {
      dmrs_l_count++;
    }

    if (l == dmrs_l_idx[dmrs_l_count]) {
      count += srslte_pdsch_nr_cp_dmrs(q, cfg, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], put);
    } else {
      count += srslte_pdsch_nr_cp_clean(q, cfg, &symbols[count], &sf_symbols[l * q->carrier.nof_prb * SRSLTE_NRE], put);
    }
  }

  return count;
}

int srslte_pdsch_nr_put(const srslte_pdsch_nr_t* q, const srslte_pdsch_cfg_nr_t* cfg, cf_t* symbols, cf_t* sf_symbols)
{
  return srslte_pdsch_nr_cp(q, cfg, symbols, sf_symbols, true);
}


int srslte_pdsch_nr_encode(srslte_pdsch_nr_t*     q,
                           uint32_t slot_idx,
                           srslte_pdsch_cfg_nr_t* cfg,
                           uint8_t*               data[SRSLTE_MAX_CODEWORDS],
                           cf_t*                  sf_symbols[SRSLTE_MAX_PORTS]) {



  return SRSLTE_SUCCESS;
}