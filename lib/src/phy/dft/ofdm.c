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

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <srslte/srslte.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/* Uncomment next line for avoiding Guru DFT call */
//#define AVOID_GURU

int srslte_ofdm_init_(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, int symbol_sz, int nof_prb, srslte_dft_dir_t dir) {
  return srslte_ofdm_init_mbsfn_(q, cp, in_buffer, out_buffer, symbol_sz, nof_prb, dir, SRSLTE_SF_NORM);
}

int srslte_ofdm_init_mbsfn_(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, int symbol_sz, int nof_prb, srslte_dft_dir_t dir, srslte_sf_t sf_type) {

  /* Set OFDM object attributes */
  q->symbol_sz = (uint32_t) symbol_sz;
  q->nof_symbols = SRSLTE_CP_NSYMB(cp);
  q->nof_symbols_mbsfn = SRSLTE_CP_NSYMB(SRSLTE_CP_EXT);
  q->cp = cp;
  q->freq_shift = false;
  q->nof_re = (uint32_t) nof_prb * SRSLTE_NRE;
  q->nof_guards = ((symbol_sz - q->nof_re) / 2);
  q->slot_sz = (uint32_t) SRSLTE_SLOT_LEN(symbol_sz);
  q->sf_sz = (uint32_t) SRSLTE_SF_LEN(symbol_sz);
  q->in_buffer = in_buffer;
  q->out_buffer= out_buffer;

  if (srslte_dft_plan_c(&q->fft_plan, symbol_sz, dir)) {
    fprintf(stderr, "Error: Creating DFT plan\n");
    return -1;
  }

#ifdef AVOID_GURU
  q->tmp = srslte_vec_malloc((uint32_t) symbol_sz * sizeof(cf_t));
  if (!q->tmp) {
    perror("malloc");
    return -1;
  }
  bzero(q->tmp, sizeof(cf_t) * symbol_sz);
#else
  int cp1 = SRSLTE_CP_ISNORM(cp)?SRSLTE_CP_LEN_NORM(0, symbol_sz):SRSLTE_CP_LEN_EXT(symbol_sz);
  int cp2 = SRSLTE_CP_ISNORM(cp)?SRSLTE_CP_LEN_NORM(1, symbol_sz):SRSLTE_CP_LEN_EXT(symbol_sz);

  q->tmp = srslte_vec_malloc(sizeof(cf_t) * q->sf_sz);
  if (!q->tmp) {
    perror("malloc");
    return -1;
  }
  bzero(q->tmp, sizeof(cf_t) * q->sf_sz);

  if (dir == SRSLTE_DFT_BACKWARD) {
    bzero(in_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_RE(nof_prb, cp));
  }else {
    bzero(in_buffer, sizeof(cf_t) * q->sf_sz);
  }

  for (int slot = 0; slot < 2; slot++) {
    //bzero(&q->fft_plan_sf[slot], sizeof(srslte_dft_plan_t));
    //bzero(q->tmp  + SRSLTE_CP_NSYMB(cp)*symbol_sz*slot, sizeof(cf_t) * (cp1 + (SRSLTE_CP_NSYMB(cp) - 1)*cp2 + SRSLTE_CP_NSYMB(cp)*symbol_sz));
    if (dir == SRSLTE_DFT_FORWARD) {
      if (srslte_dft_plan_guru_c(&q->fft_plan_sf[slot], symbol_sz, dir,
                                 in_buffer + cp1 + q->slot_sz * slot,
                                 q->tmp + q->nof_symbols * q->symbol_sz * slot,
                                 1, 1, SRSLTE_CP_NSYMB(cp), symbol_sz + cp2, symbol_sz)) {
        fprintf(stderr, "Error: Creating DFT plan (1)\n");
        return -1;
      }
    } else {
      if (srslte_dft_plan_guru_c(&q->fft_plan_sf[slot], symbol_sz, dir,
                                 q->tmp + q->nof_symbols * q->symbol_sz * slot,
                                 out_buffer + cp1 + q->slot_sz * slot,
                                 1, 1, SRSLTE_CP_NSYMB(cp), symbol_sz, symbol_sz + cp2)) {
        fprintf(stderr, "Error: Creating DFT plan (1)\n");
        return -1;
      }
    }
  }
#endif

  q->shift_buffer = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN(symbol_sz));
  if (!q->shift_buffer) {
    perror("malloc");
    return -1;
  }

  srslte_dft_plan_set_mirror(&q->fft_plan, true);
  srslte_dft_plan_set_dc(&q->fft_plan, true);

  DEBUG("Init %s symbol_sz=%d, nof_symbols=%d, cp=%s, nof_re=%d, nof_guards=%d\n",
      dir==SRSLTE_DFT_FORWARD?"FFT":"iFFT", q->symbol_sz, q->nof_symbols,
          q->cp==SRSLTE_CP_NORM?"Normal":"Extended", q->nof_re, q->nof_guards);
  
  // MBSFN logic
  if (sf_type == SRSLTE_SF_MBSFN) {
    q->mbsfn_subframe = true;
    q->non_mbsfn_region = 2; // default set to 2
  } else {
    q->mbsfn_subframe = false;
  }
  
  return SRSLTE_SUCCESS;
}

void srslte_ofdm_set_non_mbsfn_region(srslte_ofdm_t *q, uint8_t non_mbsfn_region)
{
  q->non_mbsfn_region = non_mbsfn_region;
}

int srslte_ofdm_replan_(srslte_ofdm_t *q, srslte_cp_t cp, int symbol_sz, int nof_prb) {

  if (srslte_dft_replan_c(&q->fft_plan, symbol_sz)) {
    fprintf(stderr, "Error: Creating DFT plan\n");
    return -1;
  }

  q->symbol_sz = (uint32_t) symbol_sz;
  q->nof_symbols = SRSLTE_CP_NSYMB(cp);
  q->nof_symbols_mbsfn = SRSLTE_CP_NSYMB(SRSLTE_CP_EXT);
  q->cp = cp;
  q->nof_re = (uint32_t) nof_prb * SRSLTE_NRE;
  q->nof_guards = ((symbol_sz - q->nof_re) / 2);
  q->slot_sz = (uint32_t) SRSLTE_SLOT_LEN(symbol_sz);
  q->sf_sz = (uint32_t) SRSLTE_SF_LEN(symbol_sz);

#ifndef AVOID_GURU
  cf_t *in_buffer = q->in_buffer;
  cf_t *out_buffer = q->out_buffer;

  int cp1 = SRSLTE_CP_ISNORM(cp)?SRSLTE_CP_LEN_NORM(0, symbol_sz):SRSLTE_CP_LEN_EXT(symbol_sz);
  int cp2 = SRSLTE_CP_ISNORM(cp)?SRSLTE_CP_LEN_NORM(1, symbol_sz):SRSLTE_CP_LEN_EXT(symbol_sz);

  srslte_dft_dir_t dir = q->fft_plan_sf[0].dir;

  if (q->tmp) {
    free(q->tmp);
  }

  q->tmp = srslte_vec_malloc(sizeof(cf_t) * q->sf_sz);
  if (!q->tmp) {
    perror("malloc");
    return -1;
  }
  bzero(q->tmp, sizeof(cf_t) * q->sf_sz);

  if (dir == SRSLTE_DFT_BACKWARD) {
    bzero(in_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_RE(nof_prb, cp));
  }else {
    bzero(in_buffer, sizeof(cf_t) * q->sf_sz);
  }

  for (int slot = 0; slot < 2; slot++) {
    srslte_dft_plan_free(&q->fft_plan_sf[slot]);

    if (dir == SRSLTE_DFT_FORWARD) {
      if (srslte_dft_plan_guru_c(&q->fft_plan_sf[slot], symbol_sz, dir,
                                 in_buffer + cp1 + q->slot_sz * slot,
                                 q->tmp + q->nof_symbols * q->symbol_sz * slot,
                                 1, 1, SRSLTE_CP_NSYMB(cp), symbol_sz + cp2, symbol_sz)) {
        fprintf(stderr, "Error: Creating DFT plan (1)\n");
        return -1;
      }
    } else {
      if (srslte_dft_plan_guru_c(&q->fft_plan_sf[slot], symbol_sz, dir,
                                 q->tmp + q->nof_symbols * q->symbol_sz * slot,
                                 out_buffer + cp1 + q->slot_sz * slot,
                                 1, 1, SRSLTE_CP_NSYMB(cp), symbol_sz, symbol_sz + cp2)) {
        fprintf(stderr, "Error: Creating DFT plan (1)\n");
        return -1;
      }
    }
  }
#endif /* AVOID_GURU */


  if (q->freq_shift) {
    srslte_ofdm_set_freq_shift(q, q->freq_shift_f);
  }

  DEBUG("Replan symbol_sz=%d, nof_symbols=%d, cp=%s, nof_re=%d, nof_guards=%d\n",
        q->symbol_sz, q->nof_symbols,
        q->cp==SRSLTE_CP_NORM?"Normal":"Extended", q->nof_re, q->nof_guards);

  return SRSLTE_SUCCESS;
}

void srslte_ofdm_free_(srslte_ofdm_t *q) {
  srslte_dft_plan_free(&q->fft_plan);

#ifndef AVOID_GURU
  for (int slot = 0; slot < 2; slot++) {
    if (q->fft_plan_sf[slot].init_size) {
      srslte_dft_plan_free(&q->fft_plan_sf[slot]);
    }
  }
#endif

  if (q->tmp) {
    free(q->tmp);
  }
  if (q->shift_buffer) {
    free(q->shift_buffer);
  }
  bzero(q, sizeof(srslte_ofdm_t));
}

int srslte_ofdm_rx_init(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, uint32_t max_prb) {
  int symbol_sz = srslte_symbol_sz(max_prb);
  if (symbol_sz < 0) {
    fprintf(stderr, "Error: Invalid nof_prb=%d\n", max_prb);
    return -1;
  }
  q->max_prb = max_prb;
  return srslte_ofdm_init_(q, cp, in_buffer, out_buffer, symbol_sz, max_prb, SRSLTE_DFT_FORWARD);
}

int srslte_ofdm_rx_init_mbsfn(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, uint32_t max_prb)
{
  int symbol_sz = srslte_symbol_sz(max_prb);
  if (symbol_sz < 0) {
    fprintf(stderr, "Error: Invalid nof_prb=%d\n", max_prb);
    return -1;
  }
  q->max_prb = max_prb;
  return srslte_ofdm_init_mbsfn_(q, cp, in_buffer, out_buffer, symbol_sz, max_prb, SRSLTE_DFT_FORWARD, SRSLTE_SF_MBSFN);
}


int srslte_ofdm_tx_init(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, uint32_t max_prb) {
  uint32_t i;
  int ret;
  
  int symbol_sz = srslte_symbol_sz(max_prb);
  if (symbol_sz < 0) {
    fprintf(stderr, "Error: Invalid nof_prb=%d\n", max_prb);
    return -1;
  }
  q->max_prb = max_prb;
  ret = srslte_ofdm_init_(q, cp, in_buffer, out_buffer, symbol_sz, max_prb, SRSLTE_DFT_BACKWARD);

  
  if (ret == SRSLTE_SUCCESS) {
    srslte_dft_plan_set_norm(&q->fft_plan, false);
    
    /* set now zeros at CP */
    for (i=0;i<q->nof_symbols;i++) {
      bzero(q->tmp, q->nof_guards * sizeof(cf_t));
      bzero(&q->tmp[q->nof_re + q->nof_guards], q->nof_guards * sizeof(cf_t));
    }
  }
  return ret;
}

int srslte_ofdm_tx_init_mbsfn(srslte_ofdm_t *q, srslte_cp_t cp, cf_t *in_buffer, cf_t *out_buffer, uint32_t nof_prb)
{
  uint32_t i;
  int ret;
  
  int symbol_sz = srslte_symbol_sz(nof_prb);
  if (symbol_sz < 0) { 
    fprintf(stderr, "Error: Invalid nof_prb=%d\n", nof_prb);
    return -1;
  }
  q->max_prb = nof_prb;
  ret = srslte_ofdm_init_mbsfn_(q, cp, in_buffer, out_buffer, symbol_sz, nof_prb, SRSLTE_DFT_BACKWARD, SRSLTE_SF_MBSFN);
  
  if (ret == SRSLTE_SUCCESS) {
    srslte_dft_plan_set_norm(&q->fft_plan, false);
    
    /* set now zeros at CP */
    for (i=0;i<q->nof_symbols;i++) {
      bzero(q->tmp, q->nof_guards * sizeof(cf_t));
      bzero(&q->tmp[q->nof_re + q->nof_guards], q->nof_guards * sizeof(cf_t));
    }
  }
  return ret;
}

int srslte_ofdm_rx_set_prb(srslte_ofdm_t *q, srslte_cp_t cp, uint32_t nof_prb) {
  if (nof_prb <= q->max_prb) {
    int symbol_sz = srslte_symbol_sz(nof_prb);
    if (symbol_sz < 0) {
      fprintf(stderr, "Error: Invalid nof_prb=%d\n", nof_prb);
      return -1;
    }
    return srslte_ofdm_replan_(q, cp, symbol_sz, nof_prb);
  } else {
    fprintf(stderr, "OFDM (Rx): Error calling set_prb: nof_prb (%d) must be equal or lower initialized max_prb (%d)\n",
            nof_prb, q->max_prb);
    return -1;
  }
}

int srslte_ofdm_tx_set_prb(srslte_ofdm_t *q, srslte_cp_t cp, uint32_t nof_prb) {
  uint32_t i;
  int ret;

  if (nof_prb <= q->max_prb) {
    int symbol_sz = srslte_symbol_sz(nof_prb);
    if (symbol_sz < 0) {
      fprintf(stderr, "Error: Invalid nof_prb=%d\n", nof_prb);
      return -1;
    }

    ret = srslte_ofdm_replan_(q, cp, symbol_sz, nof_prb);

    if (ret == SRSLTE_SUCCESS) {
      /* set now zeros at CP */
      for (i=0;i<q->nof_symbols;i++) {
        bzero(q->tmp, q->nof_guards * sizeof(cf_t));
        bzero(&q->tmp[q->nof_re + q->nof_guards], q->nof_guards * sizeof(cf_t));
      }
    }
    return ret;
  } else {
    fprintf(stderr, "OFDM (Tx): Error calling set_prb: nof_prb (%d) must be equal or lower initialized max_prb (%d)\n",
            nof_prb, q->max_prb);
    return -1;
  }
}


void srslte_ofdm_rx_free(srslte_ofdm_t *q) {
  srslte_ofdm_free_(q);
}
/* Shifts the signal after the iFFT or before the FFT. 
 * Freq_shift is relative to inter-carrier spacing.
 * Caution: This function shall not be called during run-time 
 */
int srslte_ofdm_set_freq_shift(srslte_ofdm_t *q, float freq_shift) {
  cf_t *ptr = q->shift_buffer;
  for (uint32_t n=0;n<2;n++) {
    for (uint32_t i=0;i<q->nof_symbols;i++) {
      uint32_t cplen = SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(i, q->symbol_sz):SRSLTE_CP_LEN_EXT(q->symbol_sz);
      for (uint32_t t=0;t<q->symbol_sz+cplen;t++) {
        ptr[t] = cexpf(I*2*M_PI*((float) t-(float)cplen)*freq_shift/q->symbol_sz);
      }
      ptr += q->symbol_sz+cplen;
    }    
  }
  
  /* Disable DC carrier addition */
  srslte_dft_plan_set_dc(&q->fft_plan, false);  
 
  q->freq_shift = true;
  q->freq_shift_f = freq_shift;
  return SRSLTE_SUCCESS;
}

void srslte_ofdm_tx_free(srslte_ofdm_t *q) {
  srslte_ofdm_free_(q);
}

void srslte_ofdm_rx_slot_ng(srslte_ofdm_t *q, cf_t *input, cf_t *output) {
  uint32_t i;
  for (i=0;i<q->nof_symbols;i++) {
    input += SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(i, q->symbol_sz):SRSLTE_CP_LEN_EXT(q->symbol_sz);
    srslte_dft_run_c(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->nof_guards], q->nof_re * sizeof(cf_t));
    input += q->symbol_sz;
    output += q->nof_re;
  }
}

/* Transforms input samples into output OFDM symbols.
 * Performs FFT on a each symbol and removes CP.
 */
void srslte_ofdm_rx_slot(srslte_ofdm_t *q, int slot_in_sf) {
  cf_t *output = q->out_buffer + slot_in_sf * q->nof_re * q->nof_symbols;

#ifdef AVOID_GURU
  srslte_ofdm_rx_slot_ng(q, q->in_buffer + slot_in_sf * q->slot_sz, q->out_buffer + slot_in_sf * q->nof_re * q->nof_symbols);
#else
  float norm = 1.0f/sqrtf(q->fft_plan.size);
  cf_t *tmp = q->tmp + slot_in_sf * q->symbol_sz * q->nof_symbols;
  uint32_t dc = (q->fft_plan.dc) ? 1:0;

  srslte_dft_run_guru_c(&q->fft_plan_sf[slot_in_sf]);

  for (int i = 0; i < q->nof_symbols; i++) {
    memcpy(output, tmp + q->symbol_sz - q->nof_re / 2, sizeof(cf_t) * q->nof_re / 2);
    memcpy(output + q->nof_re / 2, &tmp[dc], sizeof(cf_t) * q->nof_re / 2);

    if (q->fft_plan.norm) {
      srslte_vec_sc_prod_cfc(output, norm, output, q->nof_re);
    }

    tmp += q->symbol_sz;
    output += q->nof_re;
  }
#endif
}

void srslte_ofdm_rx_slot_mbsfn(srslte_ofdm_t *q, cf_t *input, cf_t *output)
{
  uint32_t i;
  for(i = 0; i < q->nof_symbols_mbsfn; i++){
    if(i == q->non_mbsfn_region) {
      input += SRSLTE_NON_MBSFN_REGION_GUARD_LENGTH(q->non_mbsfn_region,q->symbol_sz);
    }
    input += (i>=q->non_mbsfn_region)?SRSLTE_CP_LEN_EXT(q->symbol_sz):SRSLTE_CP_LEN_NORM(i, q->symbol_sz);
    srslte_dft_run_c(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->nof_guards], q->nof_re * sizeof(cf_t));
    input += q->symbol_sz;
    output += q->nof_re;
  }
}



void srslte_ofdm_rx_slot_zerocopy(srslte_ofdm_t *q, cf_t *input, cf_t *output) {
  uint32_t i;
  for (i=0;i<q->nof_symbols;i++) {
    input += SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(i, q->symbol_sz):SRSLTE_CP_LEN_EXT(q->symbol_sz);
    srslte_dft_run_c_zerocopy(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->symbol_sz/2+q->nof_guards], sizeof(cf_t)*q->nof_re/2);
    memcpy(&output[q->nof_re/2], &q->tmp[1], sizeof(cf_t)*q->nof_re/2);
    input += q->symbol_sz;
    output += q->nof_re;
  }  
}

void srslte_ofdm_rx_sf(srslte_ofdm_t *q) {
  uint32_t n;
  if (q->freq_shift) {
    srslte_vec_prod_ccc(q->in_buffer, q->shift_buffer, q->in_buffer, 2*q->slot_sz);
  }
  if(!q->mbsfn_subframe){
    for (n=0;n<2;n++) {
      srslte_ofdm_rx_slot(q, n);
    }
  }
  else{
    srslte_ofdm_rx_slot_mbsfn(q, &q->in_buffer[0*q->slot_sz], &q->out_buffer[0*q->nof_re*q->nof_symbols]);
    srslte_ofdm_rx_slot(q, 1);
  }
}

void srslte_ofdm_rx_sf_ng(srslte_ofdm_t *q, cf_t *input, cf_t *output) {
  uint32_t n;
  if (q->freq_shift) {
    srslte_vec_prod_ccc(q->in_buffer, q->shift_buffer, q->in_buffer, 2*q->slot_sz);
  }
  if(!q->mbsfn_subframe){
    for (n=0;n<2;n++) {
      srslte_ofdm_rx_slot_ng(q, &input[n*q->slot_sz], &output[n*q->nof_re*q->nof_symbols]);
    }
  }
  else{
    srslte_ofdm_rx_slot_mbsfn(q, &q->in_buffer[0*q->slot_sz], &q->out_buffer[0*q->nof_re*q->nof_symbols]);
    srslte_ofdm_rx_slot(q, 1);
  }
}

/* Transforms input OFDM symbols into output samples.
 * Performs FFT on a each symbol and adds CP.
 */
void srslte_ofdm_tx_slot(srslte_ofdm_t *q, int slot_in_sf) {
  cf_t *input = q->in_buffer + slot_in_sf * q->nof_re * q->nof_symbols;
  cf_t *output = q->out_buffer + slot_in_sf * q->slot_sz;

#ifdef AVOID_GURU
  for (int i=0;i<q->nof_symbols;i++) {
    int cp_len = SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(i, q->symbol_sz):SRSLTE_CP_LEN_EXT(q->symbol_sz);
    memcpy(&q->tmp[q->nof_guards], input, q->nof_re * sizeof(cf_t));
    srslte_dft_run_c(&q->fft_plan, q->tmp, &output[cp_len]);
    input += q->nof_re;
    /* add CP */
    memcpy(output, &output[q->symbol_sz], cp_len * sizeof(cf_t));
    output += q->symbol_sz + cp_len;
  }
#else
  float norm = 1.0f/sqrtf(q->symbol_sz);
  cf_t *tmp = q->tmp + slot_in_sf * q->symbol_sz * q->nof_symbols;

  bzero(tmp, q->slot_sz);
  uint32_t dc = (q->fft_plan.dc) ? 1:0;

  for (int i = 0; i < q->nof_symbols; i++) {
    memcpy(&tmp[dc], &input[q->nof_re / 2], q->nof_re / 2 * sizeof(cf_t));
    memcpy(&tmp[q->symbol_sz - q->nof_re / 2], &input[0], q->nof_re / 2 * sizeof(cf_t));

    input += q->nof_re;
    tmp += q->symbol_sz;
  }

  srslte_dft_run_guru_c(&q->fft_plan_sf[slot_in_sf]);

  for (int i=0;i<q->nof_symbols;i++) {
    int cp_len = SRSLTE_CP_ISNORM(q->cp) ? SRSLTE_CP_LEN_NORM(i, q->symbol_sz) : SRSLTE_CP_LEN_EXT(q->symbol_sz);

    if (q->fft_plan.norm) {
      srslte_vec_sc_prod_cfc(&output[cp_len], norm, &output[cp_len], q->symbol_sz);
    }

    /* add CP */
    memcpy(output, &output[q->symbol_sz], cp_len * sizeof(cf_t));
    output += q->symbol_sz + cp_len;
  }
#endif

  /*input = q->in_buffer + slot_in_sf * q->nof_re * q->nof_symbols;
  cf_t *output2 = srslte_vec_malloc(sizeof(cf_t) * q->slot_sz);
  cf_t *o2 = output2;
  bzero(q->tmp, sizeof(cf_t)*q->symbol_sz);
  //bzero(output2, sizeof(cf_t)*q->slot_sz);
  for (int i=0;i<q->nof_symbols;i++) {
    int cp_len = SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(i, q->symbol_sz):SRSLTE_CP_LEN_EXT(q->symbol_sz);
    memcpy(&q->tmp[q->nof_guards], input, q->nof_re * sizeof(cf_t));
    srslte_dft_run_c(&q->fft_plan, q->tmp, &o2[cp_len]);
    input += q->nof_re;
    memcpy(o2, &o2[q->symbol_sz], cp_len * sizeof(cf_t));
    o2 += q->symbol_sz + cp_len;
  }
  cf_t *output1 = q->out_buffer + slot_in_sf * q->slot_sz;//srslte_vec_malloc(sizeof(cf_t) * q->slot_sz);

  for (int i = 0; i < q->slot_sz; i++) {
    float error = cabsf(output1[i] - output2[i])/cabsf(output2[i]);
    cf_t k = output1[i]/output2[i];
    if (error > 0.1) printf("%d/%05d error=%f output=%+f%+fi gold=%+f%+fi k=%+f%+fi\n", slot_in_sf, i, error,
                            __real__ output1[i], __imag__ output1[i],
                            __real__ output2[i], __imag__ output2[i],
                            __real__ k, __imag__ k);
  }
  free(output2);/**/
}

void srslte_ofdm_tx_slot_mbsfn(srslte_ofdm_t *q, cf_t *input, cf_t *output)
{
  uint32_t i, cp_len;
  for(i=0;i<q->nof_symbols_mbsfn;i++) {
    cp_len = ( i>(q->non_mbsfn_region-1) )?SRSLTE_CP_LEN_EXT(q->symbol_sz):SRSLTE_CP_LEN_NORM(i, q->symbol_sz);
    memcpy(&q->tmp[q->nof_guards], input, q->nof_re * sizeof(cf_t));
    srslte_dft_run_c(&q->fft_plan, q->tmp, &output[cp_len]);
    input += q->nof_re;
    /* add CP */
    memcpy(output, &output[q->symbol_sz], cp_len * sizeof(cf_t));
    output += q->symbol_sz + cp_len;
    
    /*skip the small section between the non mbms region and the mbms region*/
    if(i == (q->non_mbsfn_region - 1))
      output += SRSLTE_NON_MBSFN_REGION_GUARD_LENGTH(q->non_mbsfn_region,q->symbol_sz);
  }  
}

void srslte_ofdm_set_normalize(srslte_ofdm_t *q, bool normalize_enable) {
  srslte_dft_plan_set_norm(&q->fft_plan, normalize_enable);
}

void srslte_ofdm_tx_sf(srslte_ofdm_t *q)
{
  uint32_t n;
  if(!q->mbsfn_subframe){
    for (n=0;n<2;n++) {
      srslte_ofdm_tx_slot(q, n);
    }
  }
  else{
     srslte_ofdm_tx_slot_mbsfn(q, &q->in_buffer[0*q->nof_re*q->nof_symbols], &q->out_buffer[0*q->slot_sz]);
     srslte_ofdm_tx_slot(q, 1);
  }
  if (q->freq_shift) {
    srslte_vec_prod_ccc(q->out_buffer, q->shift_buffer, q->out_buffer, 2*q->slot_sz);
  }
}

