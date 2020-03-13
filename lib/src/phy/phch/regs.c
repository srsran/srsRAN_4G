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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/utils/debug.h"

#define REG_IDX(r, i, n) r->k[i] + r->l* n* SRSLTE_NRE

srslte_regs_reg_t* regs_find_reg(srslte_regs_t* h, uint32_t k, uint32_t l);
int                regs_put_reg(srslte_regs_reg_t* reg, cf_t* reg_data, cf_t* slot_symbols, uint32_t nof_prb);

int regs_add_reg(srslte_regs_reg_t* reg, cf_t* reg_data, cf_t* slot_symbols, uint32_t nof_prb);

int regs_get_reg(srslte_regs_reg_t* reg, cf_t* slot_symbols, cf_t* reg_data, uint32_t nof_prb);

int regs_reset_reg(srslte_regs_reg_t* reg, cf_t* slot_symbols, uint32_t nof_prb);

/***************************************************************
 *
 * PDCCH REG ALLOCATION
 *
 ***************************************************************/

void regs_pdcch_free(srslte_regs_t* h)
{
  int i;
  for (i = 0; i < 3; i++) {
    if (h->pdcch[i].regs) {
      free(h->pdcch[i].regs);
      h->pdcch[i].regs = NULL;
    }
  }
}

#define PDCCH_NCOLS 32
const uint8_t PDCCH_PERM[PDCCH_NCOLS] = {1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31,
                                         0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30};

/** Initialize REGs for PDCCH
 * 36.211 10.3 section 6.8.5
 */
int regs_pdcch_init(srslte_regs_t* h)
{
  int                 i, m, cfi, nof_ctrl_symbols;
  int                 ret = SRSLTE_ERROR;
  int                 nrows, ndummy, j;
  uint32_t            k, kp;
  srslte_regs_reg_t** tmp = NULL;

  bzero(&h->pdcch, sizeof(srslte_regs_ch_t));

  for (cfi = 0; cfi < 3; cfi++) {
    if (h->cell.nof_prb <= 10) {
      nof_ctrl_symbols = cfi + 2;
    } else {
      nof_ctrl_symbols = cfi + 1;
    }

    tmp = malloc(sizeof(srslte_regs_reg_t*) * h->nof_regs);
    if (!tmp) {
      perror("malloc");
      goto clean_and_exit;
    }

    /* Number and count REGs for this CFI */
    m = 0;
    for (i = 0; i < h->nof_regs; i++) {
      if (h->regs[i].l < nof_ctrl_symbols && !h->regs[i].assigned) {
        tmp[m] = &h->regs[i];
        m++;
      }
    }

    h->pdcch[cfi].nof_regs = m;

    h->pdcch[cfi].regs = malloc(sizeof(srslte_regs_reg_t*) * h->pdcch[cfi].nof_regs);
    if (!h->pdcch[cfi].regs) {
      perror("malloc");
      goto clean_and_exit;
    }

    /* Interleave REGs */
    nrows  = (h->pdcch[cfi].nof_regs - 1) / PDCCH_NCOLS + 1;
    ndummy = PDCCH_NCOLS * nrows - h->pdcch[cfi].nof_regs;
    if (ndummy < 0) {
      ndummy = 0;
    }

    k = 0;
    for (j = 0; j < PDCCH_NCOLS; j++) {
      for (i = 0; i < nrows; i++) {
        if (i * PDCCH_NCOLS + PDCCH_PERM[j] >= ndummy) {
          m = i * PDCCH_NCOLS + PDCCH_PERM[j] - ndummy;
          if (k < h->cell.id) {
            kp = (h->pdcch[cfi].nof_regs + k - (h->cell.id % h->pdcch[cfi].nof_regs)) % h->pdcch[cfi].nof_regs;
          } else {
            kp = (k - h->cell.id) % h->pdcch[cfi].nof_regs;
          }
          h->pdcch[cfi].regs[m] = tmp[kp];
          k++;
        }
      }
    }
    h->pdcch[cfi].nof_regs = (h->pdcch[cfi].nof_regs / 9) * 9;
    INFO("Init PDCCH REG space CFI %d. %d useful REGs (%d CCEs)\n",
         cfi + 1,
         h->pdcch[cfi].nof_regs,
         h->pdcch[cfi].nof_regs / 9);
    free(tmp);
    tmp = NULL;
  }

  ret = SRSLTE_SUCCESS;
clean_and_exit:
  if (tmp) {
    free(tmp);
  }
  if (ret == SRSLTE_ERROR) {
    regs_pdcch_free(h);
  }
  return ret;
}

int srslte_regs_pdcch_nregs(srslte_regs_t* h, uint32_t cfi)
{
  if (cfi < 1 || cfi > 3) {
    ERROR("Invalid CFI=%d\n", cfi);
    return SRSLTE_ERROR;
  } else {
    return (int)h->pdcch[cfi - 1].nof_regs;
  }
}

int srslte_regs_pdcch_ncce(srslte_regs_t* h, uint32_t cfi)
{
  int nregs = srslte_regs_pdcch_nregs(h, cfi);
  if (nregs > 0) {
    return (uint32_t)(nregs / 9);
  } else {
    return SRSLTE_ERROR;
  }
}

/** Copy quadruplets to REGs and cyclic shift them, according to the
 * second part of 6.8.5 in 36.211
 */

int srslte_regs_pdcch_put_offset(srslte_regs_t* h,
                                 uint32_t       cfi,
                                 cf_t*          d,
                                 cf_t*          slot_symbols,
                                 uint32_t       start_reg,
                                 uint32_t       nof_regs)
{
  if (cfi < 1 || cfi > 3) {
    ERROR("Invalid CFI=%d\n", cfi);
    return SRSLTE_ERROR;
  }
  if (start_reg + nof_regs <= h->pdcch[cfi - 1].nof_regs) {
    uint32_t i, k;
    k = 0;
    for (i = start_reg; i < start_reg + nof_regs; i++) {
      regs_put_reg(h->pdcch[cfi - 1].regs[i], &d[k], slot_symbols, h->cell.nof_prb);
      k += 4;
    }
    return k;
  } else {
    ERROR("Out of range: start_reg + nof_reg must be lower than %d\n", h->pdcch[cfi - 1].nof_regs);
    return SRSLTE_ERROR;
  }
}

int srslte_regs_pdcch_put(srslte_regs_t* h, uint32_t cfi, cf_t* d, cf_t* slot_symbols)
{
  if (cfi < 1 || cfi > 3) {
    ERROR("Invalid CFI=%d\n", cfi);
    return SRSLTE_ERROR;
  }
  return srslte_regs_pdcch_put_offset(h, cfi, d, slot_symbols, 0, h->pdcch[cfi - 1].nof_regs);
}

int srslte_regs_pdcch_get_offset(srslte_regs_t* h,
                                 uint32_t       cfi,
                                 cf_t*          slot_symbols,
                                 cf_t*          d,
                                 uint32_t       start_reg,
                                 uint32_t       nof_regs)
{
  if (cfi < 1 || cfi > 3) {
    ERROR("Invalid CFI=%d\n", cfi);
    return SRSLTE_ERROR;
  }
  if (start_reg + nof_regs <= h->pdcch[cfi - 1].nof_regs) {
    uint32_t i, k;
    k = 0;
    for (i = start_reg; i < start_reg + nof_regs; i++) {
      regs_get_reg(h->pdcch[cfi - 1].regs[i], slot_symbols, &d[k], h->cell.nof_prb);
      k += 4;
    }
    return k;
  } else {
    ERROR("Out of range: start_reg + nof_reg must be lower than %d\n", h->pdcch[cfi - 1].nof_regs);
    return SRSLTE_ERROR;
  }
}

int srslte_regs_pdcch_get(srslte_regs_t* h, uint32_t cfi, cf_t* slot_symbols, cf_t* d)
{
  if (cfi < 1 || cfi > 3) {
    ERROR("Invalid CFI=%d\n", cfi);
    return SRSLTE_ERROR;
  }
  return srslte_regs_pdcch_get_offset(h, cfi, slot_symbols, d, 0, h->pdcch[cfi - 1].nof_regs);
}

/***************************************************************
 *
 * PHICH REG ALLOCATION
 *
 ***************************************************************/

/** Initialize REGs for PHICH
 * 36.211 10.3 section 6.9.3
 */
int regs_phich_init(srslte_regs_t* h, uint32_t phich_mi, bool mbsfn_or_sf1_6_tdd)
{
  float               ng;
  uint32_t            i, ni, li, n[3], nreg, mi;
  srslte_regs_reg_t** regs_phich[3];
  int                 ret = SRSLTE_ERROR;

  for (int i = 0; i < 3; i++) {
    regs_phich[i] = NULL;
  }

  switch (h->phich_res) {
    case SRSLTE_PHICH_R_1_6:
      ng = (float)1 / 6;
      break;
    case SRSLTE_PHICH_R_1_2:
      ng = (float)1 / 2;
      break;
    case SRSLTE_PHICH_R_1:
      ng = 1;
      break;
    case SRSLTE_PHICH_R_2:
      ng = 2;
      break;
    default:
      ng = 0;
      break;
  }
  h->ngroups_phich_m1 = (int)ceilf(ng * ((float)h->cell.nof_prb / 8));
  h->ngroups_phich    = (int)phich_mi * h->ngroups_phich_m1;
  h->phich            = malloc(sizeof(srslte_regs_ch_t) * h->ngroups_phich);
  if (!h->phich) {
    perror("malloc");
    return -1;
  }
  INFO("Creating %d PHICH mapping units. %s length, Ng=%.2f\n",
       h->ngroups_phich,
       h->phich_len == SRSLTE_PHICH_EXT ? "Extended" : "Normal",
       ng);
  for (i = 0; i < h->ngroups_phich; i++) {
    h->phich[i].nof_regs = REGS_PHICH_REGS_X_GROUP;
    h->phich[i].regs     = malloc(sizeof(srslte_regs_reg_t*) * REGS_PHICH_REGS_X_GROUP);
    if (!h->phich[i].regs) {
      perror("malloc");
      goto clean_and_exit;
    }
  }

  /** Here begins the mapping algorithm */

  /* Step 2. Count REGs not assigned to PCFICH */
  bzero(n, 3 * sizeof(int));
  for (i = 0; i < h->nof_regs; i++) {
    if (h->regs[i].l < 3 && !h->regs[i].assigned) {
      n[h->regs[i].l]++;
    }
  }

  bzero(regs_phich, sizeof(srslte_regs_reg_t*) * 3);
  for (i = 0; i < 3; i++) {
    regs_phich[i] = malloc(n[i] * sizeof(srslte_regs_reg_t*));
    if (!regs_phich[i]) {
      perror("malloc");
      goto clean_and_exit;
    }
  }

  bzero(n, 3 * sizeof(int));
  /* Step 3. Number REGs not assigned to PCFICH */
  for (i = 0; i < h->nof_regs; i++) {
    // they are already sorted starting from the REG with the lowest frequency-domain index
    if (h->regs[i].l < 3 && !h->regs[i].assigned) {
      regs_phich[h->regs[i].l][n[h->regs[i].l]++] = &h->regs[i];
    }
  }

  nreg = 0;
  for (mi = 0; mi < h->ngroups_phich; mi++) { // here ngroups is the number of mapping units
    for (i = 0; i < 3; i++) {
      // Step 7
      if (h->phich_len == SRSLTE_PHICH_NORM) {
        li = 0;
      } else if (h->phich_len == SRSLTE_PHICH_EXT && mbsfn_or_sf1_6_tdd) {
        li = (mi / 2 + i + 1) % 2;
      } else {
        li = i;
      }
      // Step 8
      if (h->phich_len == SRSLTE_PHICH_EXT && mbsfn_or_sf1_6_tdd) {
        ni = ((h->cell.id * n[li] / n[1]) + mi + i * n[li] / 3) % n[li];
      } else {
        ni = ((h->cell.id * n[li] / n[0]) + mi + i * n[li] / 3) % n[li];
      }
      h->phich[mi].regs[i]           = regs_phich[li][ni];
      h->phich[mi].regs[i]->assigned = true;
      DEBUG("Assigned PHICH REG#%d (%d,%d)\n", nreg, h->phich[mi].regs[i]->k0, li);
      nreg++;
    }
  }

  // now the number of mapping units = number of groups for normal cp. For extended cp
  // ngroups = 2 * number mapping units
  if (SRSLTE_CP_ISEXT(h->cell.cp)) {
    h->ngroups_phich *= 2;
  }
  ret = SRSLTE_SUCCESS;

clean_and_exit:
  if (ret == SRSLTE_ERROR) {
    if (h->phich) {
      for (i = 0; i < h->ngroups_phich; i++) {
        if (h->phich[i].regs) {
          free(h->phich[i].regs);
        }
      }
      free(h->phich);
    }
  }
  for (i = 0; i < 3; i++) {
    if (regs_phich[i]) {
      free(regs_phich[i]);
    }
  }
  return ret;
}

void regs_phich_free(srslte_regs_t* h)
{
  uint32_t i;
  if (h->phich) {
    if (SRSLTE_CP_ISEXT(h->cell.cp)) {
      h->ngroups_phich /= 2;
    }
    for (i = 0; i < h->ngroups_phich; i++) {
      if (h->phich[i].regs) {
        free(h->phich[i].regs);
        h->phich[i].regs = NULL;
      }
    }
    free(h->phich);
    h->phich = NULL;
  }
}

uint32_t srslte_regs_phich_nregs(srslte_regs_t* h)
{
  uint32_t i;
  uint32_t n;
  n = 0;
  for (i = 0; i < h->ngroups_phich; i++) {
    n += h->phich[i].nof_regs;
  }
  return n;
}

uint32_t srslte_regs_phich_ngroups(srslte_regs_t* h)
{
  return h->ngroups_phich;
}

uint32_t srslte_regs_phich_ngroups_m1(srslte_regs_t* h)
{
  return h->ngroups_phich_m1;
}

/**
 * Adds the PHICH symbols to the resource grid pointed by slot_symbols.
 *
 * Each subframe, the user shall call the srslte_v function before adding PHICH symbols.
 *
 * Returns the number of written symbols, or -1 on error
 */
int srslte_regs_phich_add(srslte_regs_t* h, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup, cf_t* slot_symbols)
{
  uint32_t i;
  if (ngroup >= h->ngroups_phich) {
    ERROR("Error invalid ngroup %d\n", ngroup);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
  if (SRSLTE_CP_ISEXT(h->cell.cp)) {
    ngroup /= 2;
  }
  srslte_regs_ch_t* rch = &h->phich[ngroup];
  for (i = 0; i < rch->nof_regs && i * REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
    regs_add_reg(rch->regs[i], &symbols[i * REGS_RE_X_REG], slot_symbols, h->cell.nof_prb);
  }
  return i * REGS_RE_X_REG;
}

/**
 * Resets the PHICH symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int srslte_regs_phich_reset(srslte_regs_t* h, cf_t* slot_symbols)
{
  uint32_t i;
  uint32_t ngroup, ng;
  for (ngroup = 0; ngroup < h->ngroups_phich; SRSLTE_CP_ISEXT(h->cell.cp) ? ngroup += 2 : ngroup++) {
    if (SRSLTE_CP_ISEXT(h->cell.cp)) {
      ng = ngroup / 2;
    } else {
      ng = ngroup;
    }
    srslte_regs_ch_t* rch = &h->phich[ng];
    for (i = 0; i < rch->nof_regs && i * REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
      regs_reset_reg(rch->regs[i], slot_symbols, h->cell.nof_prb);
    }
  }
  return SRSLTE_SUCCESS;
}

/**
 * Gets the PHICH symbols from the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int srslte_regs_phich_get(srslte_regs_t* h, cf_t* slot_symbols, cf_t symbols[REGS_PHICH_NSYM], uint32_t ngroup)
{
  uint32_t i;
  if (ngroup >= h->ngroups_phich) {
    ERROR("Error invalid ngroup %d\n", ngroup);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
  if (SRSLTE_CP_ISEXT(h->cell.cp)) {
    ngroup /= 2;
  }
  srslte_regs_ch_t* rch = &h->phich[ngroup];
  for (i = 0; i < rch->nof_regs && i * REGS_RE_X_REG < REGS_PHICH_NSYM; i++) {
    regs_get_reg(rch->regs[i], slot_symbols, &symbols[i * REGS_RE_X_REG], h->cell.nof_prb);
  }
  return i * REGS_RE_X_REG;
}

/***************************************************************
 *
 * PCFICH REG ALLOCATION
 *
 ***************************************************************/

/** Initialize REGs for PCFICH
 * 36.211 10.3 section 6.7.4
 */
int regs_pcfich_init(srslte_regs_t* h)
{
  uint32_t          i;
  uint32_t          k_hat, k;
  srslte_regs_ch_t* ch = &h->pcfich;

  ch->regs = malloc(sizeof(srslte_regs_reg_t*) * REGS_PCFICH_NREGS);
  if (!ch->regs) {
    perror("malloc");
    return SRSLTE_ERROR;
  }
  ch->nof_regs = REGS_PCFICH_NREGS;

  INFO("PCFICH allocating %d regs. CellID: %d, PRB: %d\n", ch->nof_regs, h->cell.id, h->cell.nof_prb);

  k_hat = (SRSLTE_NRE / 2) * (h->cell.id % (2 * h->cell.nof_prb));
  for (i = 0; i < REGS_PCFICH_NREGS; i++) {

    k           = (k_hat + (i * h->cell.nof_prb / 2) * (SRSLTE_NRE / 2)) % (h->cell.nof_prb * SRSLTE_NRE);
    ch->regs[i] = regs_find_reg(h, k, 0);
    if (!ch->regs[i]) {
      ERROR("Error allocating PCFICH: REG (%d,0) not found\n", k);
      return SRSLTE_ERROR;
    } else if (ch->regs[i]->assigned) {
      ERROR("Error allocating PCFICH: REG (%d,0) already allocated\n", k);
      return SRSLTE_ERROR;
    } else {
      ch->regs[i]->assigned = true;
      DEBUG("Assigned PCFICH REG#%d (%d,0)\n", i, k);
    }
  }
  return SRSLTE_SUCCESS;
}

void regs_pcfich_free(srslte_regs_t* h)
{
  if (h->pcfich.regs) {
    free(h->pcfich.regs);
    h->pcfich.regs = NULL;
  }
}

uint32_t srslte_regs_pcfich_nregs(srslte_regs_t* h)
{
  return h->pcfich.nof_regs;
}

/**
 * Maps the PCFICH symbols to the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int srslte_regs_pcfich_put(srslte_regs_t* h, cf_t symbols[REGS_PCFICH_NSYM], cf_t* slot_symbols)
{
  srslte_regs_ch_t* rch = &h->pcfich;

  uint32_t i;
  for (i = 0; i < rch->nof_regs && i * REGS_RE_X_REG < REGS_PCFICH_NSYM; i++) {
    regs_put_reg(rch->regs[i], &symbols[i * REGS_RE_X_REG], slot_symbols, h->cell.nof_prb);
  }
  return i * REGS_RE_X_REG;
}

/**
 * Gets the PCFICH symbols from the resource grid pointed by slot_symbols
 *
 * Returns the number of written symbols, or -1 on error
 */
int srslte_regs_pcfich_get(srslte_regs_t* h, cf_t* slot_symbols, cf_t ch_data[REGS_PCFICH_NSYM])
{
  srslte_regs_ch_t* rch = &h->pcfich;
  uint32_t          i;
  for (i = 0; i < rch->nof_regs && i * REGS_RE_X_REG < REGS_PCFICH_NSYM; i++) {
    regs_get_reg(rch->regs[i], slot_symbols, &ch_data[i * REGS_RE_X_REG], h->cell.nof_prb);
  }
  return i * REGS_RE_X_REG;
}

/***************************************************************
 *
 * COMMON FUNCTIONS
 *
 ***************************************************************/

srslte_regs_reg_t* regs_find_reg(srslte_regs_t* h, uint32_t k, uint32_t l)
{
  uint32_t i;
  for (i = 0; i < h->nof_regs; i++) {
    if (h->regs[i].l == l && h->regs[i].k0 == k) {
      return &h->regs[i];
    }
  }
  return NULL;
}

/**
 * Returns the number of REGs in a PRB
 * 36.211 Section 6.2.4
 */
int regs_num_x_symbol(uint32_t symbol, uint32_t nof_port, srslte_cp_t cp)
{

  switch (symbol) {
    case 0:
      return 2;
    case 1:
      switch (nof_port) {
        case 1:
        case 2:
          return 3;
        case 4:
          return 2;
        default:
          ERROR("Invalid number of ports %d\n", nof_port);
          return SRSLTE_ERROR;
      }
      break;
    case 2:
      return 3;
    case 3:
      if (SRSLTE_CP_ISNORM(cp)) {
        return 3;
      } else {
        return 2;
      }
    default:
      ERROR("Invalid symbol %d\n", symbol);
      return SRSLTE_ERROR;
  }
}

/**
 * Initializes the indices of a REG
 * 36.211 Section 6.2.4
 */
int regs_reg_init(srslte_regs_reg_t* reg, uint32_t symbol, uint32_t nreg, uint32_t k0, uint32_t maxreg, uint32_t vo)
{
  uint32_t i, j, z;

  reg->l        = symbol;
  reg->assigned = false;

  switch (maxreg) {
    case 2:
      reg->k0 = k0 + nreg * 6;
      /* there are two references in the middle */
      j = z = 0;
      for (i = 0; i < vo; i++) {
        reg->k[j] = k0 + nreg * 6 + i;
        j++;
      }
      for (i = 0; i < 2; i++) {
        reg->k[j] = k0 + nreg * 6 + i + vo + 1;
        j++;
      }
      z = j;
      for (i = 0; i < 4 - z; i++) {
        reg->k[j] = k0 + nreg * 6 + vo + 3 + i + 1;
        j++;
      }
      if (j != 4) {
        ERROR("Something went wrong: expected 2 references\n");
        return SRSLTE_ERROR;
      }
      break;

    case 3:
      reg->k0 = k0 + nreg * 4;
      /* there is no reference */
      for (i = 0; i < 4; i++) {
        reg->k[i] = k0 + nreg * 4 + i;
      }
      break;
    default:
      ERROR("Invalid number of REGs per PRB: %d\n", maxreg);
      return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

void srslte_regs_free(srslte_regs_t* h)
{
  if (h->regs) {
    free(h->regs);
  }
  regs_pcfich_free(h);
  regs_phich_free(h);
  regs_pdcch_free(h);

  bzero(h, sizeof(srslte_regs_t));
}

int srslte_regs_init(srslte_regs_t* h, srslte_cell_t cell)
{
  return srslte_regs_init_opts(h, cell, 1, false);
}

/**
 * Initializes REGs structure.
 * Sets all REG indices and initializes PCFICH, PHICH and PDCCH REGs
 * Returns 0 if OK, -1 on error
 */
int srslte_regs_init_opts(srslte_regs_t* h, srslte_cell_t cell, uint32_t phich_mi, bool mbsfn_or_sf1_6_tdd)
{
  int      ret = SRSLTE_ERROR_INVALID_INPUTS;
  uint32_t i, k;
  uint32_t j[4], jmax, prb;
  uint32_t n[4], vo;
  uint32_t max_ctrl_symbols;

  if (h != NULL && srslte_cell_isvalid(&cell)) {
    bzero(h, sizeof(srslte_regs_t));
    ret = SRSLTE_ERROR;

    max_ctrl_symbols    = cell.nof_prb <= 10 ? 4 : 3;
    vo                  = cell.id % 3;
    h->cell             = cell;
    h->max_ctrl_symbols = max_ctrl_symbols;
    h->phich_res        = cell.phich_resources;
    h->phich_len        = cell.phich_length;

    h->nof_regs = 0;
    for (i = 0; i < max_ctrl_symbols; i++) {
      n[i] = regs_num_x_symbol(i, h->cell.nof_ports, h->cell.cp);
      if (n[i] == -1) {
        goto clean_and_exit;
      }
      h->nof_regs += h->cell.nof_prb * n[i];
    }
    INFO("Indexing %d REGs. CellId: %d, %d PRB, CP: %s\n",
         h->nof_regs,
         h->cell.id,
         h->cell.nof_prb,
         SRSLTE_CP_ISNORM(h->cell.cp) ? "Normal" : "Extended");
    h->regs = malloc(sizeof(srslte_regs_reg_t) * h->nof_regs);
    if (!h->regs) {
      perror("malloc");
      goto clean_and_exit;
    }

    /* Sort REGs according to PDCCH mapping, beggining from the lowest l index then k */
    bzero(j, sizeof(int) * 4);
    k = i = prb = jmax = 0;
    while (k < h->nof_regs) {
      if (n[i] == 3 || (n[i] == 2 && jmax != 1)) {
        if (regs_reg_init(&h->regs[k], i, j[i], prb * SRSLTE_NRE, n[i], vo)) {
          ERROR("Error initializing REGs\n");
          goto clean_and_exit;
        }
        /*DEBUG("Available REG #%3d: l=%d, prb=%d, nreg=%d (k0=%d)\n", k, i, prb, j[i],
            h->regs[k].k0);
        */
        j[i]++;
        k++;
      }
      i++;
      if (i == max_ctrl_symbols) {
        i = 0;
        jmax++;
      }
      if (jmax == 3) {
        prb++;
        bzero(j, sizeof(int) * 4);
        jmax = 0;
      }
    }
    if (regs_pcfich_init(h)) {
      ERROR("Error initializing PCFICH REGs\n");
      goto clean_and_exit;
    }
    h->phich_mi = phich_mi;
    if (phich_mi > 0) {
      if (regs_phich_init(h, phich_mi, mbsfn_or_sf1_6_tdd)) {
        ERROR("Error initializing PHICH REGs\n");
        goto clean_and_exit;
      }
    }
    if (regs_pdcch_init(h)) {
      ERROR("Error initializing PDCCH REGs\n");
      goto clean_and_exit;
    }

    ret = SRSLTE_SUCCESS;
  }
clean_and_exit:
  if (h) {
    if (ret != SRSLTE_SUCCESS) {
      srslte_regs_free(h);
    }
  }
  return ret;
}

/**
 * Puts one REG data (4 symbols) in the slot symbols array
 */
int regs_put_reg(srslte_regs_reg_t* reg, cf_t* reg_data, cf_t* slot_symbols, uint32_t nof_prb)
{
  uint32_t i;
  for (i = 0; i < REGS_RE_X_REG; i++) {
    slot_symbols[REG_IDX(reg, i, nof_prb)] = reg_data[i];
  }
  return REGS_RE_X_REG;
}

/**
 * Adds one REG data (4 symbols) in the slot symbols array
 * Used by PHICH
 */
int regs_add_reg(srslte_regs_reg_t* reg, cf_t* reg_data, cf_t* slot_symbols, uint32_t nof_prb)
{
  uint32_t i;
  for (i = 0; i < REGS_RE_X_REG; i++) {
    slot_symbols[REG_IDX(reg, i, nof_prb)] += reg_data[i];
  }
  return REGS_RE_X_REG;
}

/**
 * Reset REG data (4 symbols) in the slot symbols array
 */
int regs_reset_reg(srslte_regs_reg_t* reg, cf_t* slot_symbols, uint32_t nof_prb)
{
  uint32_t i;
  for (i = 0; i < REGS_RE_X_REG; i++) {
    slot_symbols[REG_IDX(reg, i, nof_prb)] = 0;
  }
  return REGS_RE_X_REG;
}

/**
 * Gets one REG data (4 symbols) from the slot symbols array
 */
int regs_get_reg(srslte_regs_reg_t* reg, cf_t* slot_symbols, cf_t* reg_data, uint32_t nof_prb)
{
  uint32_t i;
  for (i = 0; i < REGS_RE_X_REG; i++) {
    reg_data[i] = slot_symbols[REG_IDX(reg, i, nof_prb)];
  }
  return REGS_RE_X_REG;
}
