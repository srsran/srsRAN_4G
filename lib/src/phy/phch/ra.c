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

#include "srslte/phy/phch/ra.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/srslte.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "tbs_tables.h"

/* Convert Type2 scheduling L_crb and RB_start to RIV value */
uint32_t srslte_ra_type2_to_riv(uint32_t L_crb, uint32_t RB_start, uint32_t nof_prb)
{
  uint32_t riv;
  if ((L_crb - 1) <= nof_prb / 2) {
    riv = nof_prb * (L_crb - 1) + RB_start;
  } else {
    riv = nof_prb * (nof_prb - L_crb + 1) + nof_prb - 1 - RB_start;
  }
  return riv;
}

/* Convert Type2 scheduling RIV value to L_crb and RB_start values */
void srslte_ra_type2_from_riv(uint32_t riv, uint32_t* L_crb, uint32_t* RB_start, uint32_t nof_prb, uint32_t nof_vrb)
{
  *L_crb    = (uint32_t)(riv / nof_prb) + 1;
  *RB_start = (uint32_t)(riv % nof_prb);
  if (*L_crb > nof_vrb - *RB_start) {
    *L_crb    = nof_prb - (int)(riv / nof_prb) + 1;
    *RB_start = nof_prb - riv % nof_prb - 1;
  }
}

/* RBG size for type0 scheduling as in table 7.1.6.1-1 of 36.213 */
uint32_t srslte_ra_type0_P(uint32_t nof_prb)
{
  if (nof_prb <= 10) {
    return 1;
  } else if (nof_prb <= 26) {
    return 2;
  } else if (nof_prb <= 63) {
    return 3;
  } else {
    return 4;
  }
}

/* Returns N_rb_type1 according to section 7.1.6.2 */
uint32_t srslte_ra_type1_N_rb(uint32_t nof_prb)
{
  uint32_t P = srslte_ra_type0_P(nof_prb);
  return (uint32_t)ceilf((float)nof_prb / P) - (uint32_t)ceilf(log2f((float)P)) - 1;
}

/* Table 6.2.3.2-1 in 36.211 */
uint32_t srslte_ra_type2_ngap(uint32_t nof_prb, bool ngap_is_1)
{
  if (nof_prb <= 10) {
    return nof_prb / 2;
  } else if (nof_prb == 11) {
    return 4;
  } else if (nof_prb <= 19) {
    return 8;
  } else if (nof_prb <= 26) {
    return 12;
  } else if (nof_prb <= 44) {
    return 18;
  } else if (nof_prb <= 49) {
    return 27;
  } else if (nof_prb <= 63) {
    return ngap_is_1 ? 27 : 9;
  } else if (nof_prb <= 79) {
    return ngap_is_1 ? 32 : 16;
  } else {
    return ngap_is_1 ? 48 : 16;
  }
}

/* Table 7.1.6.3-1 in 36.213 */
uint32_t srslte_ra_type2_n_rb_step(uint32_t nof_prb)
{
  if (nof_prb < 50) {
    return 2;
  } else {
    return 4;
  }
}

/* as defined in 6.2.3.2 of 36.211 */
uint32_t srslte_ra_type2_n_vrb_dl(uint32_t nof_prb, bool ngap_is_1)
{
  uint32_t ngap = srslte_ra_type2_ngap(nof_prb, ngap_is_1);
  if (ngap_is_1) {
    return 2 * (ngap < (nof_prb - ngap) ? ngap : nof_prb - ngap);
  } else {
    return ((uint32_t)nof_prb / ngap) * 2 * ngap;
  }
}

/* Modulation and TBS index table for PDSCH from 3GPP TS 36.213 v10.3.0 table 7.1.7.1-1 */
static int srslte_ra_dl_tbs_idx_from_mcs(uint32_t mcs, bool use_tbs_index_alt)
{
  if (use_tbs_index_alt && mcs < 28) {
    return dl_mcs_tbs_idx_table2[mcs];
  } else if (!use_tbs_index_alt && mcs < 29) {
    return dl_mcs_tbs_idx_table[mcs];
  } else {
    return SRSLTE_ERROR;
  }
}

static int srslte_ra_ul_tbs_idx_from_mcs(uint32_t mcs)
{
  if (mcs < 29) {
    return ul_mcs_tbs_idx_table[mcs];
  } else {
    return SRSLTE_ERROR;
  }
}

int srslte_ra_tbs_idx_from_mcs(uint32_t mcs, bool use_tbs_index_alt, bool is_ul)
{
  return (is_ul) ? srslte_ra_ul_tbs_idx_from_mcs(mcs) : srslte_ra_dl_tbs_idx_from_mcs(mcs, use_tbs_index_alt);
}

srslte_mod_t srslte_ra_dl_mod_from_mcs(uint32_t mcs, bool use_tbs_index_alt)
{
  if (use_tbs_index_alt) {
    // 3GPP 36.213 R12 Table 7.1.7.1-1A
    if (mcs < 5 || mcs == 28) {
      return SRSLTE_MOD_QPSK;
    } else if (mcs < 11 || mcs == 29) {
      return SRSLTE_MOD_16QAM;
    } else if (mcs < 20 || mcs == 30) {
      return SRSLTE_MOD_64QAM;
    } else {
      return SRSLTE_MOD_256QAM;
    }
  } else {
    // 3GPP 36.213 R12 Table 7.1.7.1-1
    if (mcs < 10 || mcs == 29) {
      return SRSLTE_MOD_QPSK;
    } else if (mcs < 17 || mcs == 30) {
      return SRSLTE_MOD_16QAM;
    } else {
      return SRSLTE_MOD_64QAM;
    }
  }
}

srslte_mod_t srslte_ra_ul_mod_from_mcs(uint32_t mcs)
{
  /* Table 8.6.1-1 on 36.213 */
  if (mcs <= 10) {
    return SRSLTE_MOD_QPSK;
  } else if (mcs <= 20) {
    return SRSLTE_MOD_16QAM;
  } else if (mcs <= 28) {
    return SRSLTE_MOD_64QAM;
  } else {
    return SRSLTE_MOD_BPSK;
  }
}

static int srslte_ra_dl_mcs_from_tbs_idx(uint32_t tbs_idx, bool use_tbs_index_alt)
{
  if (use_tbs_index_alt) {
    for (int mcs = 27; mcs >= 0; mcs--) {
      if (tbs_idx == dl_mcs_tbs_idx_table2[mcs]) {
        return mcs;
      }
    }
  } else {
    for (int mcs = 28; mcs >= 0; mcs--) {
      if (tbs_idx == dl_mcs_tbs_idx_table[mcs]) {
        return mcs;
      }
    }
  }
  return SRSLTE_ERROR;
}

static int srslte_ra_ul_mcs_from_tbs_idx(uint32_t tbs_idx)
{
  // Note: go in descent order to find max mcs possible
  for (int mcs = 28; mcs >= 0; mcs--) {
    if (tbs_idx == ul_mcs_tbs_idx_table[mcs]) {
      return mcs;
    }
  }
  return SRSLTE_ERROR;
}

int srslte_ra_mcs_from_tbs_idx(uint32_t tbs_idx, bool use_tbs_index_alt, bool is_ul)
{
  return is_ul ? srslte_ra_ul_mcs_from_tbs_idx(tbs_idx) : srslte_ra_dl_mcs_from_tbs_idx(tbs_idx, use_tbs_index_alt);
}

/* Table 7.1.7.2.1-1: Transport block size table on 36.213 */
int srslte_ra_tbs_from_idx(uint32_t tbs_idx, uint32_t n_prb)
{
  if (tbs_idx < SRSLTE_RA_NOF_TBS_IDX && n_prb > 0 && n_prb <= SRSLTE_MAX_PRB) {
    return tbs_table[tbs_idx][n_prb - 1];
  } else {
    return SRSLTE_ERROR;
  }
}

/* Returns lowest nearest index of TBS value in table 7.1.7.2 on 36.213
 * or -1 if the TBS value is not within the valid TBS values
 */
/*
 * Returns upper bound (exclusive) of TBS index interval whose TBS value encompasses the provided TBS
 * \remark taken from table 7.1.7.2 on 36.213
 * @return upper bound of TBS index (0..27), -2 if bad arguments
 */
int srslte_ra_tbs_to_table_idx(uint32_t tbs, uint32_t n_prb, uint32_t max_tbs_idx)
{
  if (n_prb == 0 || n_prb > SRSLTE_MAX_PRB) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
  if (tbs < tbs_table[0][n_prb - 1]) {
    return 0;
  }
  for (int tbs_idx = max_tbs_idx; tbs_idx >= 0; tbs_idx--) {
    if (tbs_table[tbs_idx][n_prb - 1] <= tbs) {
      return tbs_idx + 1;
    }
  }
  return SRSLTE_ERROR;
}
