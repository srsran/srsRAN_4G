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

#include "srslte/enb/enb_ul.h"

#include <complex.h>
#include <math.h>
#include <string.h>


#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define MAX_CANDIDATES  16

int srslte_enb_ul_init(srslte_enb_ul_t *q, srslte_cell_t cell, 
                       srslte_prach_cfg_t *prach_cfg, 
                       srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg, 
                       srslte_pucch_cfg_t *pucch_cfg, 
                       uint32_t nof_rnti)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL &&
      srslte_cell_isvalid(&cell))   
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_enb_ul_t));
    
    q->cell = cell;
    q->nof_rnti = nof_rnti; 
    
    if (srslte_ofdm_rx_init(&q->fft, q->cell.cp, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }
    srslte_ofdm_set_normalize(&q->fft, true);
    srslte_ofdm_set_freq_shift(&q->fft, -0.5);

    if (srslte_pucch_init(&q->pucch, q->cell)) {
      fprintf(stderr, "Error creating PUCCH object\n");
      goto clean_exit;
    }

    if (srslte_pusch_init(&q->pusch, q->cell)) {
      fprintf(stderr, "Error creating PUSCH object\n");
      goto clean_exit;
    }
    
    if (srslte_pusch_init_rnti_multi(&q->pusch, nof_rnti)) {
      fprintf(stderr, "Error initiating multiple RNTIs in PUSCH\n");
      goto clean_exit;
    }

    if (srslte_prach_init_cfg(&q->prach, prach_cfg, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating PRACH\n");
      goto clean_exit; 
    }
    
    srslte_prach_set_detect_factor(&q->prach, 60);
   
    if (srslte_chest_ul_init(&q->chest, cell)) {
      fprintf(stderr, "Error initiating channel estimator\n");
      goto clean_exit; 
    }
    
    // SRS is a dedicated configuration
    srslte_chest_ul_set_cfg(&q->chest, pusch_cfg, pucch_cfg, NULL);
        
    q->sf_symbols = srslte_vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit; 
    }
    
    q->ce = srslte_vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
    if (!q->ce) {
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
    srslte_enb_ul_free(q);
  }
  return ret;
}

void srslte_enb_ul_free(srslte_enb_ul_t *q)
{
  if (q) {
    srslte_prach_free(&q->prach);
    srslte_ofdm_rx_free(&q->fft);
    srslte_pucch_free(&q->pucch);
    srslte_pusch_free(&q->pusch);
    srslte_chest_ul_free(&q->chest);
    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    if (q->ce) {
      free(q->ce);
    }
    bzero(q, sizeof(srslte_enb_ul_t));
  }  
}

int srslte_enb_ul_cfg_rnti(srslte_enb_ul_t *q, uint32_t idx, uint16_t rnti)
{
  return srslte_pusch_set_rnti_multi(&q->pusch, idx, rnti);
}

int srslte_enb_ul_rem_rnti(srslte_enb_ul_t *q, uint32_t idx)
{
  return srslte_pusch_set_rnti_multi(&q->pusch, idx, 0);
}

void srslte_enb_ul_fft(srslte_enb_ul_t *q, cf_t *signal_buffer) 
{
  srslte_ofdm_rx_sf(&q->fft, signal_buffer, q->sf_symbols);
}

int srslte_enb_ul_get_pusch(srslte_enb_ul_t *q, srslte_ra_ul_grant_t *grant, srslte_softbuffer_rx_t *softbuffer, 
                            uint32_t rnti_idx, uint32_t rv_idx, uint32_t current_tx_nb, 
                            uint8_t *data, srslte_uci_data_t *uci_data, uint32_t tti)
{
     
  if (srslte_pusch_cfg(&q->pusch, &q->pusch_cfg, grant, NULL, NULL, NULL, tti, rv_idx, current_tx_nb)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    return SRSLTE_ERROR;
  }

  uint32_t cyclic_shift_for_dmrs = 0; 
  
  srslte_chest_ul_estimate(&q->chest, q->sf_symbols, q->ce, grant->L_prb, tti%10, cyclic_shift_for_dmrs, grant->n_prb);
  
  float noise_power = srslte_chest_ul_get_noise_estimate(&q->chest); 
  
  return srslte_pusch_uci_decode_rnti_idx(&q->pusch, &q->pusch_cfg, 
                                          softbuffer, q->sf_symbols, 
                                          q->ce, noise_power, 
                                          rnti_idx, data, 
                                          uci_data);
}


int srslte_enb_ul_detect_prach(srslte_enb_ul_t *q, uint32_t tti, 
                               uint32_t freq_offset, cf_t *signal, 
                               uint32_t *indices, float *offsets, float *peak2avg)
{
  uint32_t nof_detected_prach = 0; 
  // consider the number of subframes the transmission must be anticipated 
  if (srslte_prach_tti_opportunity(&q->prach, tti, -1)) 
  {
    
    if (srslte_prach_detect_offset(&q->prach,
                                   freq_offset,
                                   &signal[q->prach.N_cp],
                                   SRSLTE_SF_LEN_PRB(q->cell.nof_prb),
                                   indices, 
                                   offsets,
                                   peak2avg,
                                   &nof_detected_prach)) 
    {
      fprintf(stderr, "Error detecting PRACH\n");
      return SRSLTE_ERROR; 
    }
  } 
  return (int) nof_detected_prach; 
}





