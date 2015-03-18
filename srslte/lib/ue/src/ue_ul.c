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

#include <complex.h>
#include <math.h>
#include <string.h>

#include "srslte/ue/ue_ul.h"

#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)


int ue_ul_init(ue_ul_t *q, 
               srslte_cell_t cell) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL &&
      srslte_cell_isvalid(&cell))   
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(ue_ul_t));
    
    q->cell = cell; 
    
    if (lte_ifft_init(&q->fft, q->cell.cp, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }
    srslte_fft_set_freq_shift(&q->fft, 0.5);
    srslte_fft_set_normalize(&q->fft, true);
    
    q->normalize_en = true; 

    if (cfo_init(&q->cfo, CURRENT_SFLEN)) {
      fprintf(stderr, "Error creating CFO object\n");
      goto clean_exit;
    }
    
    if (pusch_init(&q->pusch, q->cell)) {
      fprintf(stderr, "Error creating PUSCH object\n");
      goto clean_exit;
    }
    for (uint32_t i=0;i<NOF_HARQ_PROCESSES; i++) {
      if (harq_init(&q->harq_process[i], q->cell)) {
        fprintf(stderr, "Error initiating HARQ process\n");
        goto clean_exit;
      }
    }
    if (srslte_refsignal_ul_init(&q->drms, cell)) {
      fprintf(stderr, "Error initiating srslte_refsignal_ul\n");
      goto clean_exit;
    }
    q->sf_symbols = vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit; 
    }
    q->refsignal = vec_malloc(2 * SRSLTE_NRE * q->cell.nof_prb * sizeof(cf_t));
    if (!q->refsignal) {
      perror("malloc");
      goto clean_exit; 
    }
    
    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);      
  }

clean_exit: 
  if (ret == SRSLTE_ERROR) {
    ue_ul_free(q);
  }
  return ret;
}

void ue_ul_free(ue_ul_t *q) {
  if (q) {
    srslte_fft_free(&q->fft);
    pusch_free(&q->pusch);
    for (uint32_t i=0;i<NOF_HARQ_PROCESSES; i++) {
      harq_free(&q->harq_process[i]);
    }
    cfo_free(&q->cfo); 
    srslte_refsignal_ul_free(&q->drms);

    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    if (q->refsignal) {
      free(q->refsignal);
    }
    bzero(q, sizeof(ue_ul_t));
  }
}

void ue_ul_set_cfo(ue_ul_t *q, float cur_cfo) {
  q->current_cfo = cur_cfo; 
}

void ue_ul_set_normalization(ue_ul_t *q, bool enabled)
{
  q->normalize_en = enabled;
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use pusch_encode_rnti() or pusch_decode_rnti() functions 
 */
void ue_ul_set_rnti(ue_ul_t *q, uint16_t rnti) {
  q->current_rnti = rnti; 
  pusch_set_rnti(&q->pusch, rnti);
}

void ue_ul_reset(ue_ul_t *q) {
  harq_reset(&q->harq_process[0]);
}

void ue_ul_set_pusch_cfg(ue_ul_t *q, srslte_refsignal_drms_pusch_cfg_t *pusch_drms_cfg, pusch_hopping_cfg_t *pusch_hopping_cfg)
{
  memcpy(&q->pusch_drms_cfg, pusch_drms_cfg, sizeof(srslte_refsignal_drms_pusch_cfg_t));
  pusch_set_hopping_cfg(&q->pusch, pusch_hopping_cfg); 
}

int ue_ul_pusch_encode(ue_ul_t *q, ra_pusch_t *ra_ul, uint8_t *data, uint32_t sf_idx, cf_t *output_signal) 
{
  uci_data_t uci_data;
  bzero(&uci_data, sizeof(uci_data_t));
  return ue_ul_pusch_uci_encode_rnti(q, ra_ul, data, uci_data, sf_idx, q->current_rnti, output_signal);    
}

int ue_ul_pusch_encode_rnti(ue_ul_t *q, ra_pusch_t *ra_ul, uint8_t *data, uint32_t sf_idx, uint16_t rnti, cf_t *output_signal)
{
  uci_data_t uci_data;
  bzero(&uci_data, sizeof(uci_data_t));
  return ue_ul_pusch_uci_encode_rnti(q, ra_ul, data, uci_data, sf_idx, rnti, output_signal);  
}

int ue_ul_pusch_uci_encode(ue_ul_t *q, ra_pusch_t *ra_ul, uint8_t *data, uci_data_t uci_data, uint32_t sf_idx, cf_t *output_signal)
{
  return ue_ul_pusch_uci_encode_rnti(q, ra_ul, data, uci_data, sf_idx, q->current_rnti, output_signal);
}

int ue_ul_pusch_uci_encode_rnti(ue_ul_t *q, ra_pusch_t *ra_ul, uint8_t *data, uci_data_t uci_data, 
                                uint32_t sf_idx, uint16_t rnti, 
                                cf_t *output_signal)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q             != NULL &&
      ra_ul         != NULL &&
      data          != NULL &&
      output_signal != NULL) 
  {
    
    if (ra_ul->prb_alloc.L_prb == 0)  {
      fprintf(stderr, "Invalid UL PRB allocation (L_prb=0)\n");
      return ret; 
    }      
    
    ret = SRSLTE_ERROR; 
    
    if (harq_setup_ul(&q->harq_process[0], ra_ul->mcs, ra_ul->rv_idx, sf_idx, &ra_ul->prb_alloc)) {
      fprintf(stderr, "Error configuring HARQ process\n");
      return ret; 
    }
    printf("sf_idx: %d, rnti: %d\n", sf_idx, rnti);
    if (pusch_encode_rnti(&q->pusch, &q->harq_process[0], data, rnti, q->sf_symbols)) {
      fprintf(stderr, "Error encoding TB\n");
      return ret; 
    }

    // FIXME: Pregenerate for all possible number of prb 
    if (srslte_refsignal_dmrs_pusch_gen(&q->drms, &q->pusch_drms_cfg, 
      q->harq_process[0].ul_alloc.L_prb, sf_idx, q->refsignal)) 
    {
      fprintf(stderr, "Error generating PUSCH DRMS signals\n");
      return ret; 
    }
    srslte_refsignal_drms_pusch_put(&q->drms, &q->pusch_drms_cfg, q->refsignal, 
                              q->harq_process[0].ul_alloc.L_prb, 
                              q->harq_process[0].ul_alloc.n_prb_tilde, 
                              q->sf_symbols);                
    
    lte_ifft_run_sf(&q->fft, q->sf_symbols, output_signal);
    
    //cfo_correct(&q->cfo, output_signal, output_signal, q->current_cfo / srslte_symbol_sz(q->cell.nof_prb));      
    
    if (q->normalize_en) {
      float norm_factor = (float) q->cell.nof_prb/10/sqrtf(q->harq_process[0].ul_alloc.L_prb);
      vec_sc_prod_cfc(output_signal, norm_factor, output_signal, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
    
    ret = SRSLTE_SUCCESS; 
  } 
  
  return ret; 
  
}