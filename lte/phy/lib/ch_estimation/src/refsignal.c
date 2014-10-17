/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <math.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <complex.h>

#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/ch_estimation/refsignal.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/common/sequence.h"

#include "ul_rs_tables.h"

#define idx(x, y) (l*nof_refs_x_symbol+i)

int refsignal_v(uint32_t port_id, uint32_t ns, uint32_t symbol_id)
{
  int v = -1;
  switch (port_id) {
    case 0:
      if (symbol_id == 0) {
        v = 0;
      } else {
        v = 3;
      }
      break;
    case 1:
      if (symbol_id == 0) {
        v = 3;
      } else {
        v = 0;
      }
      break;
    case 2:
      v = 3 * (ns % 2);
      break;
    case 3:
      v = 3 + 3 * (ns % 2);
      break;
  }
  return v;
}

uint32_t refsignal_k(uint32_t m, uint32_t v, uint32_t cell_id)
{
  return 6 * m + ((v + (cell_id % 6)) % 6);
}

int refsignal_put(refsignal_t * q, cf_t * slot_symbols)
{
  uint32_t i;
  uint32_t fidx, tidx;
  if (q != NULL && slot_symbols != NULL) {
    for (i = 0; i < q->nof_refs; i++) {
      fidx = q->refs[i].freq_idx;       // reference frequency index
      tidx = q->refs[i].time_idx;       // reference time index
      slot_symbols[SAMPLE_IDX(q->nof_prb, tidx, fidx)] = q->refs[i].symbol;
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Initializes refsignal_t object according to 3GPP 36.211 6.10.1
 *
 */
int refsignal_init_LTEDL(refsignal_t * q, uint32_t port_id, uint32_t nslot,
                         lte_cell_t cell)
{

  uint32_t c_init;
  uint32_t ns, l, lp[2];
  uint32_t N_cp;
  uint32_t i;
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  sequence_t seq;
  int v;
  uint32_t mp;
  uint32_t nof_refs_x_symbol, nof_ref_symbols;

  if (q != NULL &&
      port_id < MAX_PORTS &&
      nslot < NSLOTS_X_FRAME && lte_cell_isvalid(&cell)) {

    bzero(q, sizeof(refsignal_t));
    bzero(&seq, sizeof(sequence_t));

    if (CP_ISNORM(cell.cp)) {
      N_cp = 1;
    } else {
      N_cp = 0;
    }

    if (port_id < 2) {
      nof_ref_symbols = 2;
      lp[0] = 0;
      lp[1] = CP_NSYMB(cell.cp) - 3;
    } else {
      nof_ref_symbols = 1;
      lp[0] = 1;
    }
    nof_refs_x_symbol = 2 * cell.nof_prb;

    q->nof_refs = nof_refs_x_symbol * nof_ref_symbols;
    q->nsymbols = nof_ref_symbols;
    q->voffset = cell.id % 6;
    q->nof_prb = cell.nof_prb;

    q->symbols_ref = malloc(sizeof(uint32_t) * nof_ref_symbols);
    if (!q->symbols_ref) {
      perror("malloc");
      goto free_and_exit;
    }

    memcpy(q->symbols_ref, lp, sizeof(uint32_t) * nof_ref_symbols);

    q->refs = vec_malloc(q->nof_refs * sizeof(ref_t));
    if (!q->refs) {
      goto free_and_exit;
    }
    q->ch_est = vec_malloc(q->nof_refs * sizeof(cf_t));
    if (!q->ch_est) {
      goto free_and_exit;
    }

    q->recv_symbol = vec_malloc(q->nof_refs * sizeof(cf_t));
    if (!q->recv_symbol) {
      goto free_and_exit;
    }

    ns = nslot;
    for (l = 0; l < nof_ref_symbols; l++) {

      c_init = 1024 * (7 * (ns + 1) + lp[l] + 1) * (2 * cell.id + 1)
        + 2 * cell.id + N_cp;
      ret = sequence_LTE_pr(&seq, 2 * 2 * MAX_PRB, c_init);
      if (ret != LIBLTE_SUCCESS) {
        goto free_and_exit;
      }

      v = refsignal_v(port_id, ns, lp[l]);

      for (i = 0; i < nof_refs_x_symbol; i++) {
        mp = i + MAX_PRB - cell.nof_prb;

        /* generate signal */
        __real__ q->refs[idx(l, i)].symbol =
          (1 - 2 * (float) seq.c[2 * mp]) / sqrt(2);
        __imag__ q->refs[idx(l, i)].symbol =
          (1 - 2 * (float) seq.c[2 * mp + 1]) / sqrt(2);

        /* mapping to resource elements */
        q->refs[idx(l, i)].freq_idx = refsignal_k(i, (uint32_t) v, cell.id);
        q->refs[idx(l, i)].time_idx = lp[l];
      }
    }
    ret = LIBLTE_SUCCESS;
  }
free_and_exit:
  if (ret != LIBLTE_ERROR_INVALID_INPUTS) {
    sequence_free(&seq);
  }
  if (ret == LIBLTE_ERROR) {
    refsignal_free(q);
  }
  return ret;
}

// n_drms_2 table 5.5.2.1.1-1 from 36.211
uint32_t n_drms_2[8] = { 0, 6, 3, 4, 2, 8, 10, 9 };

// n_drms_1 table 5.5.2.1.1-2 from 36.211
uint32_t n_drms_1[8] = { 0, 2, 3, 4, 6, 8, 9, 10 };


/* Generation of the reference signal sequence according to Section 5.5.1 of 36.211 */ 
int rs_sequence(ref_t * refs, uint32_t len, float alpha, uint32_t ns, uint32_t cell_id,
                refsignal_ul_cfg_t * cfg)
{
  uint32_t i;

  // Calculate u and v 
  uint32_t u, v;
  uint32_t f_ss = (((cell_id % 30) + cfg->delta_ss) % 30);
  if (cfg->group_hopping_en) {
    sequence_t seq; 
    sequence_LTE_pr(&seq, cell_id / 30, 160);
    uint32_t f_gh = 0;
    for (i = 0; i < 8; i++) {
      f_gh += seq.c[8 * ns + i] << i;
    }
    sequence_free(&seq);
    u = ((f_gh%30) + f_ss) % 30;
  } else {
    u = f_ss % 30;
  }

  if (len < 6 * RE_X_RB) {
    v = 0;
  } else {
    if (!cfg->group_hopping_en && cfg->sequence_hopping_en) {
      sequence_t seq; 
      sequence_LTE_pr(&seq, ((cell_id / 30) << 5) + f_ss, 20);    
      v = seq.c[ns];
      sequence_free(&seq);
    } else {
      v = 0;
    }
  }
  if (len >= 3 * RE_X_RB) {
    uint32_t n_sz=0;
    uint32_t q;
    float q_hat;
    /* get largest prime n_zc<len */
    for (i = NOF_PRIME_NUMBERS - 1; i > 0; i--) {
      if (prime_numbers[i] < len) {
        n_sz = prime_numbers[i];
        break;
      }
    }
    q_hat = (float) n_sz *(u + 1) / 31;
    if ((((uint32_t) (2 * q_hat)) % 2) == 0) {
      q = (uint32_t) (q_hat + 0.5) + v;
    } else {
      q = (uint32_t) (q_hat + 0.5) - v;
    }
    cf_t *x_q = malloc(sizeof(cf_t) * n_sz);
    if (!x_q) {
      perror("malloc");
      return LIBLTE_ERROR;
    }
    for (i = 0; i < n_sz; i++) {
      x_q[i] =
        cexpf(-I * M_PI * (float) q * (float) i * ((float) i + 1) / n_sz);
    }
    for (i = 0; i < len; i++) {
      refs[i].symbol = cfg->beta * cexpf(I * alpha * i) * x_q[i % n_sz];
    }
    free(x_q);
  } else {
    if (len == RE_X_RB) {
      for (i = 0; i < len; i++) {
        refs[i].symbol = cfg->beta * cexpf(I * (phi_M_sc_12[u][i] * M_PI / 4 + alpha * i));
      }
    } else {
      for (i = 0; i < len; i++) {
        refs[i].symbol = cfg->beta * cexpf(I * (phi_M_sc_24[u][i] * M_PI / 4 + alpha * i));
      }
    }
  }

  return LIBLTE_SUCCESS;
}

/** Initializes refsignal_t object according to 3GPP 36.211 5.5.2
 *
 */
int refsignal_init_LTEUL_drms_pusch(refsignal_t * q, uint32_t nof_prb, uint32_t prb_start, 
                             uint32_t nslot, lte_cell_t cell, refsignal_ul_cfg_t * cfg)
{

  uint32_t i;
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t n_prs;
  uint32_t M_sc; 
  float alpha;

  if (q != NULL && nslot < NSLOTS_X_FRAME && lte_cell_isvalid(&cell)) {

    bzero(q, sizeof(refsignal_t));

    M_sc = nof_prb * RE_X_RB;

    q->nof_refs = M_sc;
    q->nsymbols = 1;
    q->voffset = cell.id % 6;
    q->nof_prb = cell.nof_prb;

    q->symbols_ref = malloc(sizeof(uint32_t) * 1);
    if (!q->symbols_ref) {
      perror("malloc");
      goto free_and_exit;
    }

    if (CP_ISNORM(cell.cp)) {
      q->symbols_ref[0] = 3;
    } else {
      q->symbols_ref[0] = 2;
    }

    q->refs = vec_malloc(q->nof_refs * sizeof(ref_t));
    if (!q->refs) {
      goto free_and_exit;
    }
    q->ch_est = vec_malloc(q->nof_refs * sizeof(cf_t));
    if (!q->ch_est) {
      goto free_and_exit;
    }

    /* Calculate n_prs */
    uint32_t c_init; 
    sequence_t seq; 
    c_init = ((cell.id / 30) << 5) + (((cell.id % 30) + cfg->delta_ss) % 30);
    ret = sequence_LTE_pr(&seq, 8 * CP_NSYMB(cell.cp) * 20, c_init);
    if (ret != LIBLTE_SUCCESS) {
      goto free_and_exit;
    }
    n_prs = 0;
    for (i = 0; i < 8; i++) {
      n_prs += (seq.c[8 * CP_NSYMB(cell.cp) * nslot + i] << i);
    }
    sequence_free(&seq);
    
    // Calculate cyclic shift alpha
    uint32_t n_cs =
      (n_drms_1[cfg->cyclic_shift] +
       n_drms_2[cfg->cyclic_shift_for_drms] + n_prs) % 12;
    alpha = 2 * M_PI * (n_cs) / 12;

    if (rs_sequence(q->refs, M_sc, alpha, cell.id, nslot, cfg)) {
      fprintf(stderr, "Error generating RS sequence\n");
      goto free_and_exit; 
    }
    /* mapping to resource elements */
    for (i=0;i<M_sc;i++) {
      q->refs[i].freq_idx = prb_start*RE_X_RB + i;
      q->refs[i].time_idx = q->symbols_ref[0];      
    }
  
    ret = LIBLTE_SUCCESS;
  }
free_and_exit:
  if (ret == LIBLTE_ERROR) {
    refsignal_free(q);
  }
  return ret;
}


void refsignal_free(refsignal_t * q)
{
  if (q->symbols_ref) {
    free(q->symbols_ref);
  }
  if (q->refs) {
    free(q->refs);
  }
  if (q->ch_est) {
    free(q->ch_est);
  }
  bzero(q, sizeof(refsignal_t));
}
