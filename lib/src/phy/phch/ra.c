/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/utils/bit.h"

#include "tbs_tables.h"

#define min(a,b) (a<b?a:b)

/* Returns the number of RE in a PRB in a slot and subframe */
uint32_t ra_re_x_prb(uint32_t subframe, uint32_t slot, uint32_t prb_idx, uint32_t nof_prb,
    uint32_t nof_ports, uint32_t nof_ctrl_symbols, srslte_cp_t cp, srslte_sf_t sf_type) {

  uint32_t re;
  bool skip_refs = true;
  srslte_cp_t cp_ = cp;
  if(SRSLTE_SF_MBSFN == sf_type) {
    cp_ = SRSLTE_CP_EXT;
  }

  if (slot == 0) {
    re = (SRSLTE_CP_NSYMB(cp_) - nof_ctrl_symbols) * SRSLTE_NRE;
  } else {
    re = SRSLTE_CP_NSYMB(cp_) * SRSLTE_NRE;
  }

  /* if it's the prb in the middle, there are less RE due to PBCH and PSS/SSS */
  if ((subframe == 0 || subframe == 5)
      && (prb_idx >= nof_prb / 2 - 3 && prb_idx < nof_prb / 2 + 3 + (nof_prb%2))) {
    if (subframe == 0) {
      if (slot == 0) {
        re = (SRSLTE_CP_NSYMB(cp_) - nof_ctrl_symbols - 2) * SRSLTE_NRE;
      } else {
        if (SRSLTE_CP_ISEXT(cp_)) {
          re = (SRSLTE_CP_NSYMB(cp_) - 4) * SRSLTE_NRE;
          skip_refs = false;
        } else {
          re = (SRSLTE_CP_NSYMB(cp_) - 4) * SRSLTE_NRE + 2 * nof_ports;
        }
      }
    } else if (subframe == 5) {
      if (slot == 0) {
        re = (SRSLTE_CP_NSYMB(cp_) - nof_ctrl_symbols - 2) * SRSLTE_NRE;
      }
    }
    if ((nof_prb % 2)
        && (prb_idx == nof_prb / 2 - 3 || prb_idx == nof_prb / 2 + 3)) {
      if (slot == 0) {
        re += 2 * SRSLTE_NRE / 2;
      } else if (subframe == 0) {
        re += 4 * SRSLTE_NRE / 2 - nof_ports;
        if (SRSLTE_CP_ISEXT(cp_)) {
          re -= nof_ports > 2 ? 2 : nof_ports;
        }
      }
    }
  }

  // remove references
  if (skip_refs) {
    if(sf_type == SRSLTE_SF_NORM){
      switch (nof_ports) {
      case 1:
      case 2:
        re -= 2 * (slot + 1) * nof_ports;
        break;
      case 4:
        if (slot == 1) {
          re -= 12;
        } else {
          re -= 4;
          if (nof_ctrl_symbols == 1) {
            re -= 4;
          }
        }
        break;
      }
    }
    if(sf_type == SRSLTE_SF_MBSFN){
      re -= 6*(slot + 1);
    }
  }
  return re;
}

int srslte_ra_ul_dci_to_grant_prb_allocation(srslte_ra_ul_dci_t *dci, srslte_ra_ul_grant_t *grant, uint32_t n_rb_ho, uint32_t nof_prb) 
{
  bzero(grant, sizeof(srslte_ra_ul_grant_t));  
  
  grant->ncs_dmrs = dci->n_dmrs;
  grant->L_prb = dci->type2_alloc.L_crb;
  uint32_t n_prb_1 = dci->type2_alloc.RB_start;
  uint32_t n_rb_pusch = 0;

  if (n_rb_ho%2) {
    n_rb_ho++;
  }

  if (dci->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_DISABLED || dci->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_TYPE2) {
    /* For no freq hopping or type2 freq hopping, n_prb is the same 
     * n_prb_tilde is calculated during resource mapping
     */
    for (uint32_t i=0;i<2;i++) {
      grant->n_prb[i] = n_prb_1;        
    }
    if (dci->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_DISABLED) {
      grant->freq_hopping = 0;
    } else {
      grant->freq_hopping = 2;      
    }
    INFO("prb1: %d, prb2: %d, L: %d\n", grant->n_prb[0], grant->n_prb[1], grant->L_prb);
  } else {
    /* Type1 frequency hopping as defined in 8.4.1 of 36.213 
      * frequency offset between 1st and 2nd slot is fixed. 
      */
    n_rb_pusch = nof_prb - n_rb_ho - (nof_prb%2);
    
    // starting prb idx for slot 0 is as given by resource grant
    grant->n_prb[0] = n_prb_1;
    if (n_prb_1 < n_rb_ho/2) {
      INFO("Invalid Frequency Hopping parameters. Offset: %d, n_prb_1: %d\n", n_rb_ho, n_prb_1);
      return SRSLTE_ERROR;
    }
    uint32_t n_prb_1_tilde = n_prb_1;

    // prb idx for slot 1 
    switch(dci->freq_hop_fl) {
      case SRSLTE_RA_PUSCH_HOP_QUART:
        grant->n_prb[1] = (n_rb_pusch/4+ n_prb_1_tilde)%n_rb_pusch;            
        break;
      case SRSLTE_RA_PUSCH_HOP_QUART_NEG:
        if (n_prb_1 < n_rb_pusch/4) {
          grant->n_prb[1] = (n_rb_pusch+ n_prb_1_tilde -n_rb_pusch/4);                                
        } else {
          grant->n_prb[1] = (n_prb_1_tilde -n_rb_pusch/4);                      
        }
        break;
      case SRSLTE_RA_PUSCH_HOP_HALF:
        grant->n_prb[1] = (n_rb_pusch/2+ n_prb_1_tilde)%n_rb_pusch;            
        break;
      default:
        break;        
    }
    INFO("n_rb_pusch: %d, prb1: %d, prb2: %d, L: %d\n", n_rb_pusch, grant->n_prb[0], grant->n_prb[1], grant->L_prb);
    grant->freq_hopping = 1;
  }
  
  if (grant->n_prb[0] + grant->L_prb <= nof_prb && 
      grant->n_prb[1] + grant->L_prb <= nof_prb) 
  {
    return SRSLTE_SUCCESS; 
  } else {
    return SRSLTE_ERROR;   
  }
}

static void ul_dci_to_grant_mcs(srslte_ra_ul_dci_t *dci, srslte_ra_ul_grant_t *grant) {
  // 8.6.2 First paragraph
  if (dci->mcs_idx <= 28) {
    /* Table 8.6.1-1 on 36.213 */
    if (dci->mcs_idx < 11) {
      grant->mcs.mod = SRSLTE_MOD_QPSK;
      grant->mcs.tbs = srslte_ra_tbs_from_idx(dci->mcs_idx, grant->L_prb);
    } else if (dci->mcs_idx < 21) {
      grant->mcs.mod = SRSLTE_MOD_16QAM;
      grant->mcs.tbs = srslte_ra_tbs_from_idx(dci->mcs_idx-1, grant->L_prb);
    } else if (dci->mcs_idx < 29) {
      grant->mcs.mod = SRSLTE_MOD_64QAM;
      grant->mcs.tbs = srslte_ra_tbs_from_idx(dci->mcs_idx-2, grant->L_prb);
    } else {
      fprintf(stderr, "Invalid MCS index %d\n", dci->mcs_idx);
    }
  } else if (dci->mcs_idx == 29 && dci->cqi_request && grant->L_prb <= 4) {
    // 8.6.1 and 8.6.2 36.213 second paragraph
    grant->mcs.mod = SRSLTE_MOD_QPSK;
    grant->mcs.tbs = 0;
    dci->rv_idx = 1;
  } else if (dci->mcs_idx >= 29) {
    // Else use last TBS/Modulation and use mcs to obtain rv_idx
    grant->mcs.tbs = -1;
    grant->mcs.mod = SRSLTE_MOD_LAST;
    dci->rv_idx = dci->mcs_idx - 28;
    DEBUG("mcs_idx=%d, tbs=%d, mod=%d, rv=%d\n",
           dci->mcs_idx, grant->mcs.tbs/8, grant->mcs.mod, dci->rv_idx);
  }
}

void srslte_ra_ul_grant_to_nbits(srslte_ra_ul_grant_t *grant, srslte_cp_t cp, uint32_t N_srs, srslte_ra_nbits_t *nbits)
{
  nbits->nof_symb = 2*(SRSLTE_CP_NSYMB(cp)-1) - N_srs;
  nbits->nof_re   = nbits->nof_symb*grant->M_sc;
  nbits->nof_bits = nbits->nof_re * grant->Qm;
}

/** Compute PRB allocation for Uplink as defined in 8.1 and 8.4 of 36.213 */
int srslte_ra_ul_dci_to_grant(srslte_ra_ul_dci_t *dci, uint32_t nof_prb, uint32_t n_rb_ho, srslte_ra_ul_grant_t *grant)
{

  // Compute PRB allocation
  if (!srslte_ra_ul_dci_to_grant_prb_allocation(dci, grant, n_rb_ho, nof_prb)) {

    // Compute MCS
    ul_dci_to_grant_mcs(dci, grant);

    // Fill rest of grant structure
    grant->mcs.idx = dci->mcs_idx;
    grant->M_sc = grant->L_prb*SRSLTE_NRE;
    grant->M_sc_init = grant->M_sc; // FIXME: What should M_sc_init be?
    grant->Qm = srslte_mod_bits_x_symbol(grant->mcs.mod);

  } else {
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

uint32_t srslte_ra_dl_approx_nof_re(srslte_cell_t cell, uint32_t nof_prb, uint32_t nof_ctrl_symbols)
{
  uint32_t nof_refs = 0;
  uint32_t nof_symb     = 2*SRSLTE_CP_NSYMB(cell.cp)-nof_ctrl_symbols;
  switch(cell.nof_ports) {
    case 1:
      nof_refs = 2*3;
      break;
    case 2:
      nof_refs = 4*3;
      break;
    case 4:
      nof_refs = 4*4;
      break;
  }
  return nof_prb * (nof_symb*SRSLTE_NRE-nof_refs);
}

/* Computes the number of RE for each PRB in the prb_dist structure */
uint32_t srslte_ra_dl_grant_nof_re(srslte_ra_dl_grant_t *grant, srslte_cell_t cell,
                                      uint32_t sf_idx, uint32_t nof_ctrl_symbols)
{
  uint32_t j, s;
  // Compute number of RE per PRB
  uint32_t nof_re = 0;
  for (s = 0; s < 2; s++) {
    for (j = 0; j < cell.nof_prb; j++) {
      if (grant->prb_idx[s][j]) {
        nof_re += ra_re_x_prb(sf_idx, s, j, cell.nof_prb, cell.nof_ports,
                              nof_ctrl_symbols, cell.cp, grant->sf_type);
      }
    }
  }
  return nof_re;
}


/** Compute PRB allocation for Downlink as defined in 7.1.6 of 36.213
 * Decode dci->type?_alloc to grant
 * This function only reads dci->type?_alloc and dci->alloc_type fields.
 * This function only writes grant->prb_idx and grant->nof_prb.
 */
/** Compute PRB allocation for Downlink as defined in 7.1.6 of 36.213 */
int srslte_ra_dl_dci_to_grant_prb_allocation(srslte_ra_dl_dci_t *dci, srslte_ra_dl_grant_t *grant, uint32_t nof_prb) {
  int i, j;
  uint32_t bitmask;
  uint32_t P = srslte_ra_type0_P(nof_prb);
  uint32_t n_rb_rbg_subset, n_rb_type1;

  bzero(grant, sizeof(srslte_ra_dl_grant_t));
  switch (dci->alloc_type) {
  case SRSLTE_RA_ALLOC_TYPE0:
    bitmask = dci->type0_alloc.rbg_bitmask;
    int nb = (int) ceilf((float) nof_prb / P);
    for (i = 0; i < nb; i++) {
      if (bitmask & (1 << (nb - i - 1))) {
        for (j = 0; j < P; j++) {
          if (i*P+j < nof_prb) {
            grant->prb_idx[0][i * P + j] = true;
            grant->nof_prb++;
          }
        }
      }
    }
    memcpy(&grant->prb_idx[1], &grant->prb_idx[0], SRSLTE_MAX_PRB*sizeof(bool));
    break;
  case SRSLTE_RA_ALLOC_TYPE1:
    // Make sure the rbg_subset is valid
    if (dci->type1_alloc.rbg_subset >= P) {
      return SRSLTE_ERROR;
    }
    n_rb_type1 = srslte_ra_type1_N_rb(nof_prb);
    uint32_t temp = ((nof_prb - 1) / P) % P;
    if (dci->type1_alloc.rbg_subset < temp) {
      n_rb_rbg_subset = ((nof_prb - 1) / (P * P)) * P + P;
    } else if (dci->type1_alloc.rbg_subset == temp) {
      n_rb_rbg_subset = ((nof_prb - 1) / (P * P)) * P + ((nof_prb - 1) % P) + 1;
    } else {
      n_rb_rbg_subset = ((nof_prb - 1) / (P * P)) * P;
    }
    int shift = dci->type1_alloc.shift ? (n_rb_rbg_subset - n_rb_type1) : 0;
    bitmask = dci->type1_alloc.vrb_bitmask;
    for (i = 0; i < n_rb_type1; i++) {
      if (bitmask & (1 << (n_rb_type1 - i - 1))) {
        uint32_t idx = (((i + shift) / P) * P * P + dci->type1_alloc.rbg_subset * P + (i + shift) % P);
        if (idx < nof_prb) {
          grant->prb_idx[0][idx] = true;
          grant->nof_prb++;
        } else {
          return SRSLTE_ERROR;
        }
      }
    }
    memcpy(&grant->prb_idx[1], &grant->prb_idx[0], SRSLTE_MAX_PRB*sizeof(bool));
    break;
  case SRSLTE_RA_ALLOC_TYPE2:
    if (dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC) {
      for (i = 0; i < dci->type2_alloc.L_crb; i++) {
        grant->prb_idx[0][i + dci->type2_alloc.RB_start] = true;
        grant->nof_prb++;
      }
      memcpy(&grant->prb_idx[1], &grant->prb_idx[0], SRSLTE_MAX_PRB*sizeof(bool));
    } else {
      /* Mapping of Virtual to Physical RB for distributed type is defined in
       * 6.2.3.2 of 36.211
       */
      int N_gap, N_tilde_vrb, n_tilde_vrb, n_tilde_prb, n_tilde2_prb, N_null,
          N_row, n_vrb;
      int n_tilde_prb_odd, n_tilde_prb_even;
      if (dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1) {
        N_tilde_vrb = srslte_ra_type2_n_vrb_dl(nof_prb, true);
        N_gap = srslte_ra_type2_ngap(nof_prb, true);
      } else {
        N_tilde_vrb = 2 * srslte_ra_type2_n_vrb_dl(nof_prb, true);
        N_gap = srslte_ra_type2_ngap(nof_prb, false);
      }
      N_row = (int) ceilf((float) N_tilde_vrb / (4 * P)) * P;
      N_null = 4 * N_row - N_tilde_vrb;
      for (i = 0; i < dci->type2_alloc.L_crb; i++) {
        n_vrb = i + dci->type2_alloc.RB_start;
        n_tilde_vrb = n_vrb % N_tilde_vrb;
        n_tilde_prb = 2 * N_row * (n_tilde_vrb % 2) + n_tilde_vrb / 2
            + N_tilde_vrb * (n_vrb / N_tilde_vrb);
        n_tilde2_prb = N_row * (n_tilde_vrb % 4) + n_tilde_vrb / 4
            + N_tilde_vrb * (n_vrb / N_tilde_vrb);

        if (N_null != 0 && n_tilde_vrb >= (N_tilde_vrb - N_null)
            && (n_tilde_vrb % 2) == 1) {
          n_tilde_prb_odd = n_tilde_prb - N_row;
        } else if (N_null != 0 && n_tilde_vrb >= (N_tilde_vrb - N_null)
            && (n_tilde_vrb % 2) == 0) {
          n_tilde_prb_odd = n_tilde_prb - N_row + N_null / 2;
        } else if (N_null != 0 && n_tilde_vrb < (N_tilde_vrb - N_null)
            && (n_tilde_vrb % 4) >= 2) {
          n_tilde_prb_odd = n_tilde2_prb - N_null / 2;
        } else {
          n_tilde_prb_odd = n_tilde2_prb;
        }
        n_tilde_prb_even = (n_tilde_prb_odd + N_tilde_vrb / 2) % N_tilde_vrb
            + N_tilde_vrb * (n_vrb / N_tilde_vrb);

        if (n_tilde_prb_odd < N_tilde_vrb / 2) {
          if (n_tilde_prb_odd < nof_prb) {
            grant->prb_idx[0][n_tilde_prb_odd] = true;
          } else {
            return SRSLTE_ERROR;
          }
        } else {
          if (n_tilde_prb_odd + N_gap - N_tilde_vrb / 2 < nof_prb) {
            grant->prb_idx[0][n_tilde_prb_odd + N_gap - N_tilde_vrb / 2] = true;
          } else {
            return SRSLTE_ERROR;
          }
        }
        grant->nof_prb++;
        if (n_tilde_prb_even < N_tilde_vrb / 2) {
          if(n_tilde_prb_even < nof_prb) {
            grant->prb_idx[1][n_tilde_prb_even] = true;
          } else {
            return SRSLTE_ERROR;
          }
        } else {
          if (n_tilde_prb_even + N_gap - N_tilde_vrb / 2 < nof_prb) {
            grant->prb_idx[1][n_tilde_prb_even + N_gap - N_tilde_vrb / 2] = true;
          } else {
            return SRSLTE_ERROR;
          }
        }
      }
    }
    break;
  default:
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dl_fill_ra_mcs(srslte_ra_mcs_t *mcs, uint32_t nprb) {
  int i_tbs = 0;
  if (mcs->idx < 10) {
    mcs->mod = SRSLTE_MOD_QPSK;
    i_tbs = mcs->idx;
  } else if (mcs->idx < 17) {
    mcs->mod = SRSLTE_MOD_16QAM;
    i_tbs = mcs->idx-1;
  } else if (mcs->idx < 29) {
    mcs->mod = SRSLTE_MOD_64QAM;
    i_tbs = mcs->idx-2;
  } else if (mcs->idx == 29) {
    mcs->mod = SRSLTE_MOD_QPSK;
    i_tbs = -1;
  } else if (mcs->idx == 30) {
    mcs->mod = SRSLTE_MOD_16QAM;
    i_tbs = -1;
  } else if (mcs->idx == 31) {
    mcs->mod = SRSLTE_MOD_64QAM;
    i_tbs = -1;
  }

  int tbs = -1;
  if (i_tbs >= 0) {
    tbs = srslte_ra_tbs_from_idx(i_tbs, nprb);
    mcs->tbs = tbs;
  }
  return tbs;
}

int srslte_dl_fill_ra_mcs_pmch(srslte_ra_mcs_t *mcs, uint32_t nprb) {
  uint32_t i_tbs = 0;
  int tbs = -1;
  if (mcs->idx < 5) {
    mcs->mod = SRSLTE_MOD_QPSK;
    i_tbs = mcs->idx*2;
  }else if (mcs->idx < 6) {
    mcs->mod = SRSLTE_MOD_16QAM;
    i_tbs = mcs->idx*2;
  }else if (mcs->idx < 11) {
    mcs->mod = SRSLTE_MOD_16QAM;
    i_tbs = mcs->idx + 5;
  }else if (mcs->idx < 20) {
    mcs->mod = SRSLTE_MOD_64QAM;
    i_tbs = mcs->idx + 5;
  }else if (mcs->idx < 28) {
    //mcs->mod = SRSLTE_MOD_256QAM;
    i_tbs = mcs->idx + 5;
  }else if (mcs->idx == 28) {
    mcs->mod = SRSLTE_MOD_QPSK;
    tbs = 0;
    i_tbs = 0;
  }else if (mcs->idx == 29) {
    mcs->mod = SRSLTE_MOD_16QAM;
    tbs = 0;
    i_tbs = 0;
  }else if (mcs->idx == 30) {
    mcs->mod = SRSLTE_MOD_64QAM;
    tbs = 0;
    i_tbs = 0;
  }else if (mcs->idx == 31) {
    mcs->mod = SRSLTE_MOD_64QAM;
    tbs = 0;
    i_tbs = 0;
  }


  if (tbs == -1) {
    tbs = srslte_ra_tbs_from_idx(i_tbs, nprb);
    if (tbs >= 0) {
      mcs->tbs = tbs;
    }
  }
  return tbs;
}

/* Modulation order and transport block size determination 7.1.7 in 36.213
 * This looks at DCI type, type of RNTI and reads fields dci->type?_alloc, dci->mcs_idx,
 * dci->dci_is_1a and dci->dci_is_1c
 * Reads global variable last_dl_tbs if mcs>=29
 * Writes global variable last_dl_tbs if mcs<29
 * */
static int dl_dci_to_grant_mcs(srslte_ra_dl_dci_t *dci, srslte_ra_dl_grant_t *grant, bool crc_is_crnti) {
  uint32_t n_prb=0;
  int tbs = -1;
  uint32_t i_tbs = 0;

  if (!crc_is_crnti) {
    if (dci->dci_is_1a) {
      n_prb = dci->type2_alloc.n_prb1a == SRSLTE_RA_TYPE2_NPRB1A_2 ? 2 : 3;
      i_tbs = dci->mcs_idx;
      tbs = srslte_ra_tbs_from_idx(i_tbs, n_prb);
    } else if (dci->dci_is_1c) {
      if (dci->mcs_idx < 32) {
        tbs = tbs_format1c_table[dci->mcs_idx];
      } else {
        fprintf(stderr, "Error decoding DCI: Invalid mcs_idx=%d in Format1C\n", dci->mcs_idx);
      }
    } else {
      fprintf(stderr, "Error decoding DCI: P/SI/RA-RNTI supports Format1A/1C only\n");
      return SRSLTE_ERROR;
    }
    grant->mcs[0].mod = SRSLTE_MOD_QPSK;
    grant->mcs[0].tbs = (uint32_t) tbs;
    grant->mcs[0].idx = dci->mcs_idx;
  } else {
    n_prb = grant->nof_prb;
    if (dci->tb_en[0]) {
      grant->mcs[0].idx = dci->mcs_idx;
      grant->mcs[0].tbs = srslte_dl_fill_ra_mcs(&grant->mcs[0], n_prb);
    } else {
      grant->mcs[0].tbs = 0;
    }
    if (dci->tb_en[1]) {
      grant->mcs[1].idx = dci->mcs_idx_1;
      grant->mcs[1].tbs = srslte_dl_fill_ra_mcs(&grant->mcs[1], n_prb);
    } else {
      grant->mcs[1].tbs = 0;
    }
  }
  for (int tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    grant->tb_en[tb] = dci->tb_en[tb];
    if (dci->tb_en[tb]) {
      grant->Qm[tb] = srslte_mod_bits_x_symbol(grant->mcs[tb].mod);
    }
  }
  grant->pinfo = dci->pinfo;
  grant->tb_cw_swap = dci->tb_cw_swap;

  if (grant->mcs[0].tbs < 0 || grant->mcs[1].tbs < 0) {
    return SRSLTE_ERROR; 
  } else {    
    return SRSLTE_SUCCESS; 
  }
}

void srslte_ra_dl_grant_to_nbits(srslte_ra_dl_grant_t *grant, uint32_t cfi, srslte_cell_t cell, uint32_t sf_idx,
                                 srslte_ra_nbits_t nbits[SRSLTE_MAX_CODEWORDS])
{
  // Compute number of RE 
  for (int i = 0; i < SRSLTE_MAX_TB; i++) {
    /* Compute number of RE for first transport block */
    nbits[i].nof_re = srslte_ra_dl_grant_nof_re(grant, cell, sf_idx, cell.nof_prb < 10 ? (cfi + 1) : cfi);
    nbits[i].lstart = cell.nof_prb < 10 ? (cfi + 1) : cfi;
    if (SRSLTE_SF_NORM == grant->sf_type) {
      nbits[i].nof_symb = 2 * SRSLTE_CP_NSYMB(cell.cp) - nbits[0].lstart;
    } else if (SRSLTE_SF_MBSFN == grant->sf_type) {
      nbits[i].nof_symb = 2 * SRSLTE_CP_EXT_NSYMB - nbits[0].lstart;
    }
    if (grant->tb_en[i]) {
      nbits[i].nof_bits = nbits[i].nof_re * grant->Qm[i];
    }
  }
}

/** Obtains a DL grant from a DCI grant for PDSCH */
int srslte_ra_dl_dci_to_grant(srslte_ra_dl_dci_t *dci, 
                              uint32_t nof_prb, uint16_t msg_rnti, srslte_ra_dl_grant_t *grant) 
{
  grant->sf_type = SRSLTE_SF_NORM;
  bool crc_is_crnti = false; 
  if (msg_rnti >= SRSLTE_CRNTI_START && msg_rnti <= SRSLTE_CRNTI_END) {
    crc_is_crnti = true; 
  }
  // Compute PRB allocation
  int ret =srslte_ra_dl_dci_to_grant_prb_allocation(dci, grant, nof_prb);
  if (!ret) {
    // Compute MCS
    ret = dl_dci_to_grant_mcs(dci, grant, crc_is_crnti);
    if (ret == SRSLTE_SUCCESS) {
      // Apply Section 7.1.7.3. If RA-RNTI and Format1C rv_idx=0
      if (dci->dci_is_1c) {
        if ((msg_rnti >= SRSLTE_RARNTI_START && msg_rnti <= SRSLTE_RARNTI_END) || msg_rnti == SRSLTE_PRNTI)
        {
          dci->rv_idx = 0;
        }
      }
    } else {
      return SRSLTE_ERROR; 
    }
  } else {
    return SRSLTE_ERROR; 
  }
  return SRSLTE_SUCCESS;
}

/* RBG size for type0 scheduling as in table 7.1.6.1-1 of 36.213 */
uint32_t srslte_ra_type0_P(uint32_t nof_prb) {
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
uint32_t srslte_ra_type1_N_rb(uint32_t nof_prb) {
  uint32_t P = srslte_ra_type0_P(nof_prb);
  return (uint32_t) ceilf((float) nof_prb / P) - (uint32_t) ceilf(log2f((float) P)) - 1;
}

/* Convert Type2 scheduling L_crb and RB_start to RIV value */
uint32_t srslte_ra_type2_to_riv(uint32_t L_crb, uint32_t RB_start, uint32_t nof_prb) {
  uint32_t riv;
  if ((L_crb - 1) <= nof_prb / 2) {
    riv = nof_prb * (L_crb - 1) + RB_start;
  } else {
    riv = nof_prb * (nof_prb - L_crb + 1) + nof_prb - 1 - RB_start;
  }
  return riv;
}

/* Convert Type2 scheduling RIV value to L_crb and RB_start values */
void srslte_ra_type2_from_riv(uint32_t riv, uint32_t *L_crb, uint32_t *RB_start,
    uint32_t nof_prb, uint32_t nof_vrb) {
  *L_crb = (uint32_t) (riv / nof_prb) + 1;
  *RB_start = (uint32_t) (riv % nof_prb);
  if (*L_crb > nof_vrb - *RB_start) {
    *L_crb = nof_prb - (int) (riv / nof_prb) + 1;
    *RB_start = nof_prb - riv % nof_prb - 1;
  }
}

/* Table 6.2.3.2-1 in 36.211 */
uint32_t srslte_ra_type2_ngap(uint32_t nof_prb, bool ngap_is_1) {
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
uint32_t srslte_ra_type2_n_rb_step(uint32_t nof_prb) {
  if (nof_prb < 50) {
    return 2;
  } else {
    return 4;
  }
}

/* as defined in 6.2.3.2 of 36.211 */
uint32_t srslte_ra_type2_n_vrb_dl(uint32_t nof_prb, bool ngap_is_1) {
  uint32_t ngap = srslte_ra_type2_ngap(nof_prb, ngap_is_1);
  if (ngap_is_1) {
    return 2 * (ngap < (nof_prb - ngap) ? ngap : nof_prb - ngap);
  } else {
    return ((uint32_t) nof_prb / ngap) * 2 * ngap;
  }
}

/* Modulation and TBS index table for PDSCH from 3GPP TS 36.213 v10.3.0 table 7.1.7.1-1 */
int srslte_ra_tbs_idx_from_mcs(uint32_t mcs) {
  if(mcs < 29) {
    return mcs_tbs_idx_table[mcs];
  } else {
    return SRSLTE_ERROR;
  }
}

srslte_mod_t srslte_ra_mod_from_mcs(uint32_t mcs) {
  if (mcs <= 10 || mcs == 29) {
    return SRSLTE_MOD_QPSK;
  } else if (mcs <= 17 || mcs == 30) {
    return SRSLTE_MOD_16QAM;
  } else {
    return SRSLTE_MOD_64QAM;
  } 
}

int srslte_ra_mcs_from_tbs_idx(uint32_t tbs_idx) {
  for (int i=0;i<29;i++) {
    if (tbs_idx == mcs_tbs_idx_table[i]) {
      return i; 
    }
  } 
  return SRSLTE_ERROR;
}

/* Table 7.1.7.2.1-1: Transport block size table on 36.213 */
int srslte_ra_tbs_from_idx(uint32_t tbs_idx, uint32_t n_prb) {
  if (tbs_idx < 27 && n_prb > 0 && n_prb <= SRSLTE_MAX_PRB) {
    return tbs_table[tbs_idx][n_prb - 1];
  } else {
    return SRSLTE_ERROR;
  }
}

/* Returns lowest nearest index of TBS value in table 7.1.7.2 on 36.213
 * or -1 if the TBS value is not within the valid TBS values
 */
int srslte_ra_tbs_to_table_idx(uint32_t tbs, uint32_t n_prb) {
  uint32_t idx;
  if (n_prb > 0 && n_prb <= SRSLTE_MAX_PRB) {
      
    if (tbs <= tbs_table[0][n_prb-1]) {
      return 0;
    }
    if (tbs >= tbs_table[26][n_prb-1]) {
      return 27;
    }
    for (idx = 0; idx < 26; idx++) {
      if (tbs_table[idx][n_prb-1] <= tbs && tbs_table[idx+1][n_prb-1] >= tbs) {
        return idx+1;
      }
    }
  }
  return SRSLTE_ERROR;
}

void srslte_ra_pusch_fprint(FILE *f, srslte_ra_ul_dci_t *dci, uint32_t nof_prb) {
  fprintf(f, " - Resource Allocation Type 2 mode :\t%s\n",
      dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC ? "Localized" : "Distributed");
  
  fprintf(f, "   + Frequency Hopping:\t\t\t");
  if (dci->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_DISABLED) {
    fprintf(f, "No\n");
  } else {
    fprintf(f, "Yes\n");
  }
  fprintf(f, "   + Resource Indicator Value:\t\t%d\n", dci->type2_alloc.riv);
  if (dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC) {
  fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
    dci->type2_alloc.L_crb, dci->type2_alloc.RB_start);
  } else {
  fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
    dci->type2_alloc.L_crb, dci->type2_alloc.RB_start);
  fprintf(f, "   + VRB gap selection:\t\t\tGap %d\n",
    dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1 ? 1 : 2);
  fprintf(f, "   + VRB gap:\t\t\t\t%d\n",
    srslte_ra_type2_ngap(nof_prb, dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1));

  }
  
  fprintf(f, " - Modulation and coding scheme index:\t%d\n", dci->mcs_idx);
  fprintf(f, " - New data indicator:\t\t\t%s\n", dci->ndi ? "Yes" : "No");
  fprintf(f, " - Redundancy version:\t\t\t%d\n", dci->rv_idx);
  fprintf(f, " - TPC command for PUCCH:\t\t--\n");    
}

void srslte_ra_ul_grant_fprint(FILE *f, srslte_ra_ul_grant_t *grant) {
  fprintf(f, " - Number of PRBs:\t\t\t%d\n", grant->L_prb);
  fprintf(f, " - Modulation type:\t\t\t%s\n", srslte_mod_string(grant->mcs.mod));
  fprintf(f, " - Transport block size:\t\t%d\n", grant->mcs.tbs);
}

char *ra_type_string(srslte_ra_type_t alloc_type) {
  switch (alloc_type) {
  case SRSLTE_RA_ALLOC_TYPE0:
    return "Type 0";
  case SRSLTE_RA_ALLOC_TYPE1:
    return "Type 1";
  case SRSLTE_RA_ALLOC_TYPE2:
    return "Type 2";
  default:
    return "N/A";
  }
}

void srslte_ra_pdsch_fprint(FILE *f, srslte_ra_dl_dci_t *dci, uint32_t nof_prb) {
  fprintf(f, " - Resource Allocation Type:\t\t%s\n",
      ra_type_string(dci->alloc_type));
  switch (dci->alloc_type) {
  case SRSLTE_RA_ALLOC_TYPE0:
    fprintf(f, "   + Resource Block Group Size:\t\t%d\n", srslte_ra_type0_P(nof_prb));
    fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n", dci->type0_alloc.rbg_bitmask);
    break;
  case SRSLTE_RA_ALLOC_TYPE1:
    fprintf(f, "   + Resource Block Group Size:\t\t%d\n", srslte_ra_type0_P(nof_prb));
    fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n", dci->type1_alloc.vrb_bitmask);
    fprintf(f, "   + RBG Subset:\t\t\t%d\n", dci->type1_alloc.rbg_subset);
    fprintf(f, "   + RBG Shift:\t\t\t\t%s\n",
        dci->type1_alloc.shift ? "Yes" : "No");
    break;
  case SRSLTE_RA_ALLOC_TYPE2:
    fprintf(f, "   + Type:\t\t\t\t%s\n",
        dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC ? "Localized" : "Distributed");
    fprintf(f, "   + Resource Indicator Value:\t\t%d\n", dci->type2_alloc.riv);
    if (dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC) {
      fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
          dci->type2_alloc.L_crb, dci->type2_alloc.RB_start);
    } else {
      fprintf(f, "   + VRB Assignment:\t\t\t%d VRB starting with VRB %d\n",
          dci->type2_alloc.L_crb, dci->type2_alloc.RB_start);
      fprintf(f, "   + VRB gap selection:\t\t\tGap %d\n",
          dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1 ? 1 : 2);
      fprintf(f, "   + VRB gap:\t\t\t\t%d\n",
          srslte_ra_type2_ngap(nof_prb, dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1));
    }
    break;
  }
  fprintf(f, " - HARQ process:\t\t\t%d\n", dci->harq_process);
  fprintf(f, " - TPC command for PUCCH:\t\t--\n");
  fprintf(f, " - Transport blocks swapped:\t\t%s\n", (dci->tb_cw_swap)?"true":"false");
  fprintf(f, " - Transport block 1 enabled:\t\t%s\n", (dci->tb_en[0])?"true":"false");
  if (dci->tb_en[0]) {
    fprintf(f, "   + Modulation and coding scheme index:\t%d\n", dci->mcs_idx);
    fprintf(f, "   + New data indicator:\t\t\t%s\n", dci->ndi ? "Yes" : "No");
    fprintf(f, "   + Redundancy version:\t\t\t%d\n", dci->rv_idx);
  }
  fprintf(f, " - Transport block 2 enabled:\t\t%s\n", (dci->tb_en[1])?"true":"false");
  if (dci->tb_en[1]) {
    fprintf(f, "   + Modulation and coding scheme index:\t%d\n", dci->mcs_idx_1);
    fprintf(f, "   + New data indicator:\t\t\t%s\n", dci->ndi_1 ? "Yes" : "No");
    fprintf(f, "   + Redundancy version:\t\t\t%d\n", dci->rv_idx_1);
  }
}

void srslte_ra_dl_grant_fprint(FILE *f, srslte_ra_dl_grant_t *grant) {
  srslte_ra_prb_fprint(f, grant);
  fprintf(f, " - Number of PRBs:\t\t\t%d\n", grant->nof_prb);
  fprintf(f, " - Number of TBs:\t\t\t%d\n", SRSLTE_RA_DL_GRANT_NOF_TB(grant));
  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (grant->tb_en[i]) {
      fprintf(f, "  - Transport block:\t\t\t%d\n", i);
      fprintf(f, "   -> Modulation type:\t\t\t%s\n", srslte_mod_string(grant->mcs[i].mod));
      fprintf(f, "   -> Transport block size:\t\t%d\n", grant->mcs[i].tbs);
    }
  }
}

void srslte_ra_prb_fprint(FILE *f, srslte_ra_dl_grant_t *grant) {
  if (grant->nof_prb > 0) {
    for (int j=0;j<2;j++) {
      fprintf(f, " - PRB Bitmap Assignment %dst slot:\n", j);
      for (int i=0;i<SRSLTE_MAX_PRB;i++) {
        if (grant->prb_idx[j][i]) {
          fprintf(f, "%d, ", i);
        }
      }
      fprintf(f, "\n");      
    }
  }
  
}
