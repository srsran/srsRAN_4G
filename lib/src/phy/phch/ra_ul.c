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

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include "srsran/srsran.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define min(a, b) (a < b ? a : b)

/**********
 * STATIC FUNCTIONS
 *
 **********/

static int f_hop_sum(srsran_ra_ul_pusch_hopping_t* q, uint32_t i)
{
  uint32_t sum = 0;
  for (uint32_t k = i * 10 + 1; k < i * 10 + 9; i++) {
    sum += (q->seq_type2_fo.c[k] << (k - (i * 10 + 1)));
  }
  return sum;
}

static int f_hop(srsran_ra_ul_pusch_hopping_t* q, srsran_pusch_hopping_cfg_t* hopping, int i)
{
  if (i == -1) {
    return 0;
  } else {
    if (hopping->n_sb == 1) {
      return 0;
    } else if (hopping->n_sb == 2) {
      return (f_hop(q, hopping, i - 1) + f_hop_sum(q, i)) % 2;
    } else {
      return (f_hop(q, hopping, i - 1) + f_hop_sum(q, i) % (hopping->n_sb - 1) + 1) % hopping->n_sb;
    }
  }
}

static int f_m(srsran_ra_ul_pusch_hopping_t* q, srsran_pusch_hopping_cfg_t* hopping, uint32_t i, uint32_t current_tx_nb)
{
  if (hopping->n_sb == 1) {
    if (hopping->hop_mode == SRSRAN_PUSCH_HOP_MODE_INTER_SF) {
      return current_tx_nb % 2;
    } else {
      return i % 2;
    }
  } else {
    return q->seq_type2_fo.c[i * 10];
  }
}
/* Computes PUSCH frequency hopping as defined in Section 8.4 of 36.213 */
static void compute_freq_hopping(srsran_ra_ul_pusch_hopping_t* q,
                                 srsran_ul_sf_cfg_t*           sf,
                                 srsran_pusch_hopping_cfg_t*   hopping_cfg,
                                 srsran_pusch_grant_t*         grant)
{
  if (q->cell.frame_type == SRSRAN_TDD) {
    ERROR("Error frequency hopping for TDD not implemented (c_init for each subframe, see end of 5.3.4 36.211)");
  }

  for (uint32_t slot = 0; slot < 2; slot++) {
    INFO("PUSCH Freq hopping: %d", grant->freq_hopping);
    uint32_t n_prb_tilde = grant->n_prb[slot];

    if (grant->freq_hopping == 1) {
      if (hopping_cfg->hop_mode == SRSRAN_PUSCH_HOP_MODE_INTER_SF) {
        n_prb_tilde = grant->n_prb[hopping_cfg->current_tx_nb % 2];
      } else {
        n_prb_tilde = grant->n_prb[slot];
      }
    }
    if (grant->freq_hopping == 2) {
      /* Freq hopping type 2 as defined in 5.3.4 of 36.211 */
      uint32_t n_vrb_tilde = grant->n_prb[0];
      if (hopping_cfg->n_sb > 1) {
        n_vrb_tilde -= (hopping_cfg->hopping_offset - 1) / 2 + 1;
      }
      int i = 0;
      if (hopping_cfg->hop_mode == SRSRAN_PUSCH_HOP_MODE_INTER_SF) {
        i = sf->tti % 10;
      } else {
        i = 2 * sf->tti % 10 + slot;
      }
      uint32_t n_rb_sb = q->cell.nof_prb;
      if (hopping_cfg->n_sb > 1) {
        n_rb_sb = (n_rb_sb - hopping_cfg->hopping_offset - hopping_cfg->hopping_offset % 2) / hopping_cfg->n_sb;
      }
      n_prb_tilde = (n_vrb_tilde + f_hop(q, hopping_cfg, i) * n_rb_sb + (n_rb_sb - 1) -
                     2 * (n_vrb_tilde % n_rb_sb) * f_m(q, hopping_cfg, i, hopping_cfg->current_tx_nb)) %
                    (n_rb_sb * hopping_cfg->n_sb);

      INFO("n_prb_tilde: %d, n_vrb_tilde: %d, n_rb_sb: %d, n_sb: %d",
           n_prb_tilde,
           n_vrb_tilde,
           n_rb_sb,
           hopping_cfg->n_sb);
      if (hopping_cfg->n_sb > 1) {
        n_prb_tilde += (hopping_cfg->hopping_offset - 1) / 2 + 1;
      }
    }
    grant->n_prb_tilde[slot] = n_prb_tilde;
  }
}

static int ra_ul_grant_to_grant_prb_allocation(srsran_dci_ul_t*      dci,
                                               srsran_pusch_grant_t* grant,
                                               uint32_t              n_rb_ho,
                                               uint32_t              nof_prb)
{
  uint32_t n_prb_1    = 0;
  uint32_t n_rb_pusch = 0;

  srsran_ra_type2_from_riv(dci->type2_alloc.riv, &grant->L_prb, &n_prb_1, nof_prb, nof_prb);
  if (n_rb_ho % 2) {
    n_rb_ho++;
  }

  if (dci->freq_hop_fl == SRSRAN_RA_PUSCH_HOP_DISABLED || dci->freq_hop_fl == SRSRAN_RA_PUSCH_HOP_TYPE2) {
    /* For no freq hopping or type2 freq hopping, n_prb is the same
     * n_prb_tilde is calculated during resource mapping
     */
    for (uint32_t i = 0; i < 2; i++) {
      grant->n_prb[i] = n_prb_1;
    }
    if (dci->freq_hop_fl == SRSRAN_RA_PUSCH_HOP_DISABLED) {
      grant->freq_hopping = 0;
    } else {
      grant->freq_hopping = 2;
    }
    INFO("prb1: %d, prb2: %d, L: %d", grant->n_prb[0], grant->n_prb[1], grant->L_prb);
  } else {
    /* Type1 frequency hopping as defined in 8.4.1 of 36.213
     * frequency offset between 1st and 2nd slot is fixed.
     */
    n_rb_pusch = nof_prb - n_rb_ho - (nof_prb % 2);

    // starting prb idx for slot 0 is as given by resource dci
    grant->n_prb[0] = n_prb_1;
    if (n_prb_1 < n_rb_ho / 2) {
      INFO("Invalid Frequency Hopping parameters. Offset: %d, n_prb_1: %d", n_rb_ho, n_prb_1);
      return SRSRAN_ERROR;
    }
    uint32_t n_prb_1_tilde = n_prb_1;

    // prb idx for slot 1
    switch (dci->freq_hop_fl) {
      case SRSRAN_RA_PUSCH_HOP_QUART:
        grant->n_prb[1] = (n_rb_pusch / 4 + n_prb_1_tilde) % n_rb_pusch;
        break;
      case SRSRAN_RA_PUSCH_HOP_QUART_NEG:
        if (n_prb_1 < n_rb_pusch / 4) {
          grant->n_prb[1] = (n_rb_pusch + n_prb_1_tilde - n_rb_pusch / 4);
        } else {
          grant->n_prb[1] = (n_prb_1_tilde - n_rb_pusch / 4);
        }
        break;
      case SRSRAN_RA_PUSCH_HOP_HALF:
        grant->n_prb[1] = (n_rb_pusch / 2 + n_prb_1_tilde) % n_rb_pusch;
        break;
      default:
        break;
    }
    INFO("n_rb_pusch: %d, prb1: %d, prb2: %d, L: %d", n_rb_pusch, grant->n_prb[0], grant->n_prb[1], grant->L_prb);
    grant->freq_hopping = 1;
  }

  /* Copy DM-RS Cyclic Shift. */
  grant->n_dmrs = dci->n_dmrs;

  if (grant->n_prb[0] + grant->L_prb <= nof_prb && grant->n_prb[1] + grant->L_prb <= nof_prb) {
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR;
  }
}

static void ul_fill_ra_mcs(srsran_ra_tb_t* tb, srsran_ra_tb_t* last_tb, uint32_t L_prb, bool cqi_request)
{
  // 8.6.2 First paragraph
  if (tb->mcs_idx <= 28) {
    /* Table 8.6.1-1 on 36.213 */
    if (tb->mcs_idx < 11) {
      tb->mod = SRSRAN_MOD_QPSK;
      tb->tbs = srsran_ra_tbs_from_idx(tb->mcs_idx, L_prb);
    } else if (tb->mcs_idx < 21) {
      tb->mod = SRSRAN_MOD_16QAM;
      tb->tbs = srsran_ra_tbs_from_idx(tb->mcs_idx - 1, L_prb);
    } else if (tb->mcs_idx < 29) {
      tb->mod = SRSRAN_MOD_64QAM;
      tb->tbs = srsran_ra_tbs_from_idx(tb->mcs_idx - 2, L_prb);
    } else {
      ERROR("Invalid MCS index %d", tb->mcs_idx);
    }
  } else if (tb->mcs_idx == 29 && cqi_request && L_prb <= 4) {
    // 8.6.1 and 8.6.2 36.213 second paragraph
    tb->mod = SRSRAN_MOD_QPSK;
    tb->tbs = 0;
    tb->rv  = 1;
  } else if (tb->mcs_idx >= 29) {
    // Else use last TBS/Modulation and use mcs to obtain rv_idx
    tb->tbs = last_tb->tbs;
    tb->mod = last_tb->mod;
    tb->rv  = tb->mcs_idx - 28;
  }
}

void srsran_ra_ul_compute_nof_re(srsran_pusch_grant_t* grant, srsran_cp_t cp, uint32_t N_srs)
{
  grant->nof_symb    = 2 * (SRSRAN_CP_NSYMB(cp) - 1) - N_srs;
  grant->nof_re      = grant->nof_symb * grant->L_prb * SRSRAN_NRE;
  grant->tb.nof_bits = grant->nof_re * srsran_mod_bits_x_symbol(grant->tb.mod);
}

/**********
 * NON-STATIC FUNCTIONS
 *
 **********/

/* Initializes the Pseudo-Random sequence to the provided cell id. Can be called multiple times without allocating new
 * memory
 */
int srsran_ra_ul_pusch_hopping_init(srsran_ra_ul_pusch_hopping_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q) {
    if (cell.id != q->cell.id || !q->initialized) {
      q->cell        = cell;
      q->initialized = true;
      /* Precompute sequence for type2 frequency hopping */
      if (srsran_sequence_LTE_pr(&q->seq_type2_fo, 210, q->cell.id)) {
        ERROR("Error initiating type2 frequency hopping sequence");
        return SRSRAN_ERROR;
      }
      ret = SRSRAN_SUCCESS;
    }
  }
  return ret;
}

void srsran_ra_ul_pusch_hopping_free(srsran_ra_ul_pusch_hopping_t* q)
{
  srsran_sequence_free(&q->seq_type2_fo);
}

void srsran_ra_ul_pusch_hopping(srsran_ra_ul_pusch_hopping_t* q,
                                srsran_ul_sf_cfg_t*           sf,
                                srsran_pusch_hopping_cfg_t*   hopping_cfg,
                                srsran_pusch_grant_t*         grant)
{
  /* Compute PUSCH frequency hopping */
  if (hopping_cfg->hopping_enabled) {
    compute_freq_hopping(q, sf, hopping_cfg, grant);
  } else {
    grant->n_prb_tilde[0] = grant->n_prb[0];
    grant->n_prb_tilde[1] = grant->n_prb[1];
  }
}

/** Compute PRB allocation for Uplink as defined in 8.1 and 8.4 of 36.213 */
int srsran_ra_ul_dci_to_grant(srsran_cell_t*              cell,
                              srsran_ul_sf_cfg_t*         sf,
                              srsran_pusch_hopping_cfg_t* hopping_cfg,
                              srsran_dci_ul_t*            dci,
                              srsran_pusch_grant_t*       grant)
{
  // Compute PRB allocation
  if (!ra_ul_grant_to_grant_prb_allocation(dci, grant, hopping_cfg->n_rb_ho, cell->nof_prb)) {
    // copy default values from DCI. RV can be updated by ul_fill_ra_mcs() in case of Adaptive retx (mcs>28)
    grant->tb.mcs_idx = dci->tb.mcs_idx;
    grant->tb.rv      = dci->tb.rv;

    // Compute MCS
    ul_fill_ra_mcs(&grant->tb, &grant->last_tb, grant->L_prb, dci->cqi_request);

    /* Compute RE assuming shortened is false*/
    srsran_ra_ul_compute_nof_re(grant, cell->cp, 0);

    // TODO: Need to compute hopping here before determining if there is collision with SRS, but only MAC knows if it's
    // a
    //  new tx or a retx. Need to split MAC interface in 2 calls. For now, assume hopping is the same
    for (uint32_t i = 0; i < 2; i++) {
      grant->n_prb_tilde[i] = grant->n_prb[i];
    }

    if (grant->nof_symb == 0 || grant->nof_re == 0) {
      INFO("Error converting ul_dci to grant, nof_symb=%d, nof_re=%d", grant->nof_symb, grant->nof_re);
      return SRSRAN_ERROR;
    }

    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR;
  }
}

uint32_t srsran_ra_ul_info(const srsran_pusch_grant_t* grant, char* info_str, uint32_t len)
{
  return srsran_print_check(info_str,
                            len,
                            0,
                            ", rb=(%d,%d), nof_re=%d, tbs=%d, mod=%d, rv=%d",
                            grant->n_prb_tilde[0],
                            grant->n_prb_tilde[0] + grant->L_prb - 1,
                            grant->nof_re,
                            grant->tb.tbs / 8,
                            srsran_mod_bits_x_symbol(grant->tb.mod),
                            grant->tb.rv);
}
