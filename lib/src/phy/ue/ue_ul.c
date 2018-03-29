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

#include <complex.h>
#include <math.h>
#include <string.h>

#include "srslte/phy/ue/ue_ul.h"

#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define MAX_SFLEN     SRSLTE_SF_LEN(srslte_symbol_sz(max_prb))

#define DEFAULT_CFO_TOL   1.0 // Hz

int srslte_ue_ul_init(srslte_ue_ul_t *q,
                      cf_t *out_buffer,
                      uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q != NULL)
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_ue_ul_t));

    q->sf_symbols = srslte_vec_malloc(SRSLTE_SF_LEN_PRB(max_prb) * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    if (srslte_ofdm_tx_init(&q->fft, SRSLTE_CP_NORM, q->sf_symbols, out_buffer, max_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }
    srslte_ofdm_set_freq_shift(&q->fft, 0.5);
    srslte_ofdm_set_normalize(&q->fft, true);

    q->normalize_en = false; 

    if (srslte_cfo_init(&q->cfo, MAX_SFLEN)) {
      fprintf(stderr, "Error creating CFO object\n");
      goto clean_exit;
    }

    srslte_ue_ul_set_cfo_tol(q, DEFAULT_CFO_TOL);

    if (srslte_pusch_init_ue(&q->pusch, max_prb)) {
      fprintf(stderr, "Error creating PUSCH object\n");
      goto clean_exit;
    }
    if (srslte_pucch_init(&q->pucch)) {
      fprintf(stderr, "Error creating PUSCH object\n");
      goto clean_exit;
    }
    if (srslte_softbuffer_tx_init(&q->softbuffer, max_prb)) {
      fprintf(stderr, "Error initiating soft buffer\n");
      goto clean_exit;
    }
    if (srslte_refsignal_ul_init(&q->signals, max_prb)) {
      fprintf(stderr, "Error initiating srslte_refsignal_ul\n");
      goto clean_exit;
    }
    q->refsignal = srslte_vec_malloc(2 * SRSLTE_NRE * max_prb * sizeof(cf_t));
    if (!q->refsignal) {
      perror("malloc");
      goto clean_exit; 
    }
    
    q->srs_signal = srslte_vec_malloc(SRSLTE_NRE * max_prb * sizeof(cf_t));
    if (!q->srs_signal) {
      perror("malloc");
      goto clean_exit; 
    }
    q->signals_pregenerated = false; 
    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters\n");
  }

clean_exit: 
  if (ret == SRSLTE_ERROR) {
    srslte_ue_ul_free(q);
  }
  return ret;
}

void srslte_ue_ul_free(srslte_ue_ul_t *q) {
  if (q) {
    srslte_ofdm_tx_free(&q->fft);
    srslte_pusch_free(&q->pusch);
    srslte_pucch_free(&q->pucch);
    srslte_softbuffer_tx_free(&q->softbuffer);
    
    srslte_cfo_free(&q->cfo); 
    srslte_refsignal_ul_free(&q->signals);

    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    if (q->refsignal) {
      free(q->refsignal);
    }
    if (q->srs_signal) {
      free(q->srs_signal);
    }
    if (q->signals_pregenerated) {
      srslte_refsignal_dmrs_pusch_pregen_free(&q->signals, &q->pregen_drms);
      srslte_refsignal_srs_pregen_free(&q->signals, &q->pregen_srs);
    }
    bzero(q, sizeof(srslte_ue_ul_t));
  }
}

int srslte_ue_ul_set_cell(srslte_ue_ul_t *q,
                          srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell))
  {
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      memcpy(&q->cell, &cell, sizeof(srslte_cell_t));

      if (srslte_ofdm_tx_set_prb(&q->fft, q->cell.cp, q->cell.nof_prb)) {
        fprintf(stderr, "Error resizing FFT\n");
        return SRSLTE_ERROR;
      }
      if (srslte_cfo_resize(&q->cfo, SRSLTE_SF_LEN_PRB(q->cell.nof_prb))) {
        fprintf(stderr, "Error resizing CFO object\n");
        return SRSLTE_ERROR;
      }

      srslte_ue_ul_set_cfo_tol(q, q->current_cfo_tol);

      if (srslte_pusch_set_cell(&q->pusch, q->cell)) {
        fprintf(stderr, "Error resizing PUSCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_pucch_set_cell(&q->pucch, q->cell)) {
        fprintf(stderr, "Error resizing PUSCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_refsignal_ul_set_cell(&q->signals, q->cell)) {
        fprintf(stderr, "Error resizing srslte_refsignal_ul\n");
        return SRSLTE_ERROR;
      }
      q->signals_pregenerated = false;
    }
    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid cell properties ue_ul: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

void srslte_ue_ul_set_cfo_tol(srslte_ue_ul_t *q, float tol) {
  q->current_cfo_tol = tol;
  srslte_cfo_set_tol(&q->cfo, tol/(15000.0*srslte_symbol_sz(q->cell.nof_prb)));
}

void srslte_ue_ul_set_cfo(srslte_ue_ul_t *q, float cur_cfo) {
  q->current_cfo = cur_cfo; 
}

void srslte_ue_ul_set_cfo_enable(srslte_ue_ul_t *q, bool enabled)
{
  q->cfo_en = enabled; 
}

void srslte_ue_ul_set_normalization(srslte_ue_ul_t *q, bool enabled)
{
  q->normalize_en = enabled;
}

/* Precalculate the PUSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode_rnti() or srslte_pusch_decode_rnti() functions 
 */
void srslte_ue_ul_set_rnti(srslte_ue_ul_t *q, uint16_t rnti) {
  srslte_pusch_set_rnti(&q->pusch, rnti);
  srslte_pucch_set_crnti(&q->pucch, rnti);
  q->current_rnti = rnti; 
}

void srslte_ue_ul_reset(srslte_ue_ul_t *q) {
  srslte_softbuffer_tx_reset(&q->softbuffer);
}

int srslte_ue_ul_pregen_signals(srslte_ue_ul_t *q) {
  if (q->signals_pregenerated) {
    srslte_refsignal_dmrs_pusch_pregen_free(&q->signals, &q->pregen_drms);
    srslte_refsignal_srs_pregen_free(&q->signals, &q->pregen_srs);
  }
  if (srslte_refsignal_dmrs_pusch_pregen(&q->signals, &q->pregen_drms)) {
    return SRSLTE_ERROR;
  }
  if (srslte_refsignal_srs_pregen(&q->signals, &q->pregen_srs)) {
    return SRSLTE_ERROR;
  }
  q->signals_pregenerated = true; 
  return SRSLTE_SUCCESS;
}


void srslte_ue_ul_set_cfg(srslte_ue_ul_t *q, 
                          srslte_refsignal_dmrs_pusch_cfg_t *dmrs_cfg, 
                          srslte_refsignal_srs_cfg_t        *srs_cfg,
                          srslte_pucch_cfg_t                *pucch_cfg, 
                          srslte_pucch_sched_t              *pucch_sched, 
                          srslte_uci_cfg_t                  *uci_cfg,
                          srslte_pusch_hopping_cfg_t        *hopping_cfg, 
                          srslte_ue_ul_powerctrl_t          *power_ctrl)
{
  srslte_refsignal_ul_set_cfg(&q->signals, dmrs_cfg, pucch_cfg, srs_cfg);
  if (pucch_cfg && dmrs_cfg) {
    srslte_pucch_set_cfg(&q->pucch, pucch_cfg, dmrs_cfg->group_hopping_en); 
  }
  if (pucch_sched) {
    memcpy(&q->pucch_sched, pucch_sched, sizeof(srslte_pucch_sched_t));    
  }
  if (srs_cfg) {
    memcpy(&q->srs_cfg, srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  }
  if (uci_cfg) {
    memcpy(&q->uci_cfg, uci_cfg, sizeof(srslte_uci_cfg_t));
  }
  if (hopping_cfg) {
    memcpy(&q->hopping_cfg, hopping_cfg, sizeof(srslte_pusch_hopping_cfg_t));
  }
  if (power_ctrl) {
    memcpy(&q->power_ctrl, power_ctrl, sizeof(srslte_ue_ul_powerctrl_t));
  }
}

int srslte_ue_ul_cfg_grant(srslte_ue_ul_t *q, srslte_ra_ul_grant_t *grant,
                           uint32_t tti, uint32_t rvidx, uint32_t current_tx_nb) 
{
  return srslte_pusch_cfg(&q->pusch, &q->pusch_cfg, grant, &q->uci_cfg, &q->hopping_cfg, &q->srs_cfg, tti, rvidx, current_tx_nb);
}

// Encode bits from uci_data
void pucch_encode_bits(srslte_uci_data_t *uci_data, srslte_pucch_format_t format, 
                       uint8_t pucch_bits[SRSLTE_PUCCH_MAX_BITS], 
                       uint8_t pucch2_bits[SRSLTE_PUCCH_MAX_BITS]) 
{
  if (format == SRSLTE_PUCCH_FORMAT_1A || format == SRSLTE_PUCCH_FORMAT_1B) {
    pucch_bits[0] = uci_data->uci_ack; 
    pucch_bits[1] = uci_data->uci_ack_2; // this will be ignored in format 1a 
  }
  if (format >= SRSLTE_PUCCH_FORMAT_2) {
    /* Put RI (goes alone) */
    if (uci_data->ri_periodic_report) {
      uint8_t temp[2] = {uci_data->uci_ri, 0};
      srslte_uci_encode_cqi_pucch(temp, uci_data->uci_ri_len, pucch_bits);
    } else {
      /* Put CQI Report*/
      srslte_uci_encode_cqi_pucch(uci_data->uci_cqi, uci_data->uci_cqi_len, pucch_bits);
    }
    if (format > SRSLTE_PUCCH_FORMAT_2) {
      pucch2_bits[0] = uci_data->uci_ack; 
      pucch2_bits[1] = uci_data->uci_ack_2; // this will be ignored in format 2a 
    }
  }
}

/* Choose PUCCH format as in Sec 10.1 of 36.213 and generate PUCCH signal 
 */
int srslte_ue_ul_pucch_encode(srslte_ue_ul_t *q, srslte_uci_data_t uci_data,
                              uint32_t pdcch_n_cce, 
                              uint32_t tti, 
                              cf_t *output_signal)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q             != NULL &&
      output_signal != NULL) 
  {
    
    uint32_t sf_idx = tti%10; 
    ret = SRSLTE_ERROR; 
    bzero(q->sf_symbols, sizeof(cf_t)*SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp));
    
    
    uint8_t pucch_bits[SRSLTE_PUCCH_MAX_BITS];
    uint8_t pucch2_bits[2];
    bzero(pucch_bits, SRSLTE_PUCCH_MAX_BITS*sizeof(uint8_t));
    bzero(pucch2_bits, 2*sizeof(uint8_t));
    
    srslte_pucch_format_t format = srslte_pucch_get_format(&uci_data, q->cell.cp);
    
    // Encode UCI information 
    pucch_encode_bits(&uci_data, format, pucch_bits, pucch2_bits);
    
    // Choose n_pucch 
    uint32_t n_pucch = srslte_pucch_get_npucch(pdcch_n_cce, format, uci_data.scheduling_request, &q->pucch_sched);
    
    if (srslte_pucch_encode(&q->pucch, format, n_pucch, sf_idx, q->current_rnti, pucch_bits, q->sf_symbols)) {
      fprintf(stderr, "Error encoding TB\n");
      return ret; 
    }

    if (srslte_refsignal_dmrs_pucch_gen(&q->signals, format, n_pucch, sf_idx, pucch2_bits, q->refsignal)) 
    {
      fprintf(stderr, "Error generating PUSCH DRMS signals\n");
      return ret; 
    }
    srslte_refsignal_dmrs_pucch_put(&q->signals, format, n_pucch, q->refsignal, q->sf_symbols);                
    
    if (srslte_ue_ul_srs_tx_enabled(&q->signals.srs_cfg, tti) && q->pucch.shortened) {
      if (q->signals_pregenerated) {
        srslte_refsignal_srs_pregen_put(&q->signals, &q->pregen_srs, tti, q->sf_symbols);
      } else {
        srslte_refsignal_srs_gen(&q->signals, tti%10, q->srs_signal);
        srslte_refsignal_srs_put(&q->signals, tti, q->srs_signal, q->sf_symbols);      
      }
    }
    
    q->last_pucch_format = format; 

    srslte_ofdm_tx_sf(&q->fft);
    
    if (q->cfo_en) {
      srslte_cfo_correct(&q->cfo, output_signal, output_signal, q->current_cfo / srslte_symbol_sz(q->cell.nof_prb));
    }
    
    if (q->normalize_en) {
      float norm_factor = (float) q->cell.nof_prb/15/10;
      srslte_vec_sc_prod_cfc(output_signal, norm_factor, output_signal, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
    ret = SRSLTE_SUCCESS; 
  } 
  
  return ret; 
}

int srslte_ue_ul_pusch_encode(srslte_ue_ul_t *q, uint8_t *data, cf_t *output_signal) 
{
  srslte_uci_data_t uci_data;
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_ue_ul_pusch_uci_encode_rnti(q, data, uci_data, q->current_rnti, output_signal);    
}

int srslte_ue_ul_pusch_encode_rnti(srslte_ue_ul_t *q, uint8_t *data, uint16_t rnti, cf_t *output_signal)
{
  srslte_uci_data_t uci_data;
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_ue_ul_pusch_uci_encode_rnti(q, data, uci_data, rnti, output_signal);  
}

int srslte_ue_ul_pusch_uci_encode(srslte_ue_ul_t *q, uint8_t *data, srslte_uci_data_t uci_data, cf_t *output_signal)
{
  return srslte_ue_ul_pusch_uci_encode_rnti(q, data, uci_data, q->current_rnti, output_signal);
}

int srslte_ue_ul_pusch_uci_encode_rnti(srslte_ue_ul_t *q, 
                                       uint8_t *data, srslte_uci_data_t uci_data, 
                                       uint16_t rnti, 
                                       cf_t *output_signal)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q             != NULL &&
      output_signal != NULL) 
  {
    
    if (q->pusch_cfg.grant.L_prb == 0)  {
      fprintf(stderr, "Invalid UL PRB allocation (L_prb=0)\n");
      return SRSLTE_ERROR; 
    }      
    
    return srslte_ue_ul_pusch_encode_rnti_softbuffer(q, data, uci_data, &q->softbuffer, rnti, output_signal); 
  }
  return ret; 
}

int srslte_ue_ul_srs_encode(srslte_ue_ul_t *q, uint32_t tti, cf_t *output_signal) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q && output_signal) {
    ret = SRSLTE_ERROR; 
    
    if (srslte_ue_ul_srs_tx_enabled(&q->signals.srs_cfg, tti)) {
      if (q->signals_pregenerated) {
        srslte_refsignal_srs_pregen_put(&q->signals, &q->pregen_srs, tti, q->sf_symbols);
      } else {
        srslte_refsignal_srs_gen(&q->signals, tti%10, q->srs_signal);
        srslte_refsignal_srs_put(&q->signals, tti, q->srs_signal, q->sf_symbols);      
      }
    }
    
    srslte_ofdm_tx_sf(&q->fft);
    
    if (q->cfo_en) {
      srslte_cfo_correct(&q->cfo, output_signal, output_signal, q->current_cfo / srslte_symbol_sz(q->cell.nof_prb));
    }
    
    if (q->normalize_en) {
      float norm_factor = (float) q->cell.nof_prb/15/sqrtf(srslte_refsignal_srs_M_sc(&q->signals));
      srslte_vec_sc_prod_cfc(output_signal, norm_factor, output_signal, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
    
    ret = SRSLTE_SUCCESS; 
  } 
  return ret; 
}

int srslte_ue_ul_pusch_encode_rnti_softbuffer(srslte_ue_ul_t *q, 
                                              uint8_t *data, srslte_uci_data_t uci_data, 
                                              srslte_softbuffer_tx_t *softbuffer,
                                              uint16_t rnti, 
                                              cf_t *output_signal)
{
 
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q             != NULL &&
      output_signal != NULL) 
  {

    bzero(q->sf_symbols, sizeof(cf_t)*SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp));
    
    if (srslte_pusch_encode(&q->pusch, &q->pusch_cfg, softbuffer, data, uci_data, rnti, q->sf_symbols)) {
      fprintf(stderr, "Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    if (q->signals_pregenerated) {
      srslte_refsignal_dmrs_pusch_pregen_put(&q->signals, &q->pregen_drms, 
                                             q->pusch_cfg.grant.L_prb, 
                                             q->pusch_cfg.sf_idx, 
                                             q->pusch_cfg.grant.ncs_dmrs,
                                             q->pusch_cfg.grant.n_prb_tilde, 
                                             q->sf_symbols);
    } else {

      if (srslte_refsignal_dmrs_pusch_gen(&q->signals, q->pusch_cfg.grant.L_prb, 
                                          q->pusch_cfg.sf_idx,
                                          q->pusch_cfg.grant.ncs_dmrs,
                                          q->refsignal)) 
      {
        fprintf(stderr, "Error generating PUSCH DRMS signals\n");
        return ret; 
      }
      srslte_refsignal_dmrs_pusch_put(&q->signals, q->refsignal, 
                                      q->pusch_cfg.grant.L_prb, 
                                      q->pusch_cfg.grant.n_prb_tilde, 
                                      q->sf_symbols);                      
    }
    
    if (srslte_ue_ul_srs_tx_enabled(&q->signals.srs_cfg, q->pusch_cfg.tti)) {
      if (q->signals_pregenerated) {
        srslte_refsignal_srs_pregen_put(&q->signals, &q->pregen_srs, q->pusch_cfg.tti, q->sf_symbols);
      } else {
        srslte_refsignal_srs_gen(&q->signals, q->pusch_cfg.sf_idx, q->srs_signal);
        srslte_refsignal_srs_put(&q->signals, q->pusch_cfg.tti, q->srs_signal, q->sf_symbols);              
      }
    }
    
    srslte_ofdm_tx_sf(&q->fft);
    
    if (q->cfo_en) {
      srslte_cfo_correct(&q->cfo, output_signal, output_signal, q->current_cfo / srslte_symbol_sz(q->cell.nof_prb));
    }
    
    if (q->normalize_en) {
      float norm_factor = (float) q->cell.nof_prb/15/sqrtf(q->pusch_cfg.grant.L_prb);
      srslte_vec_sc_prod_cfc(output_signal, norm_factor, output_signal, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
    
    ret = SRSLTE_SUCCESS; 
  } 
  
  return ret;   
}

/* Returns the transmission power for PUSCH for this subframe as defined in Section 5.1.1 of 36.213 */
float srslte_ue_ul_pusch_power(srslte_ue_ul_t *q, float PL, float p0_preamble) 
{
  float p0_pusch, alpha;
  if (p0_preamble) {
    p0_pusch = p0_preamble + q->power_ctrl.delta_preamble_msg3;
    alpha = 1;
  } else {
    alpha = q->power_ctrl.alpha;
    p0_pusch = q->power_ctrl.p0_nominal_pusch + q->power_ctrl.p0_ue_pusch;
  }
  float delta=0;
  if (q->power_ctrl.delta_mcs_based) {
    float beta_offset_pusch = 1;
    float MPR = q->pusch_cfg.cb_segm.K1*q->pusch_cfg.cb_segm.C1+q->pusch_cfg.cb_segm.K2*q->pusch_cfg.cb_segm.C2;
    if (q->pusch_cfg.cb_segm.tbs == 0) {
      beta_offset_pusch = srslte_sch_beta_cqi(q->pusch_cfg.uci_cfg.I_offset_cqi);
      MPR = q->pusch_cfg.last_O_cqi;
    }
    MPR /= q->pusch_cfg.nbits.nof_re;
    delta = 10*log10((pow(2,MPR*1.25)-1)*beta_offset_pusch);  
  }
  //TODO: This implements closed-loop power control
  float f=0;  
  
  float pusch_power = 10*log10(q->pusch_cfg.grant.L_prb)+p0_pusch+alpha*PL+delta+f;
  DEBUG("PUSCH: P=%f -- 10M=%f, p0=%f,alpha=%f,PL=%f,\n", 
         pusch_power, 10*log10(q->pusch_cfg.grant.L_prb), p0_pusch, alpha, PL);
  return SRSLTE_MIN(SRSLTE_PC_MAX, pusch_power);
}

/* Returns the transmission power for PUCCH for this subframe as defined in Section 5.1.2 of 36.213 */
float srslte_ue_ul_pucch_power(srslte_ue_ul_t *q, float PL, srslte_pucch_format_t format, uint32_t n_cqi, uint32_t n_harq) {
  float p0_pucch = q->power_ctrl.p0_nominal_pucch + q->power_ctrl.p0_ue_pucch;

  uint8_t format_idx = format==0?0:((uint32_t) format-1);

  float delta_f = q->power_ctrl.delta_f_pucch[format_idx];
  
  float h;
  
  if(format <= SRSLTE_PUCCH_FORMAT_1B) {
    h = 0;
  } else {
    if (SRSLTE_CP_ISNORM(q->cell.cp)) {
      if (n_cqi >= 4) {
        h = 10*log10(n_cqi/4);
      } else {
        h = 0;
      }
    } else {
      if (n_cqi + n_harq >= 4) {
        h = 10*log10((n_cqi+n_harq)/4);
      } else {
        h = 0; 
      }
    }
  }
  
  //TODO: This implements closed-loop power control
  float g = 0; 
 
  float pucch_power = p0_pucch + PL + h + delta_f + g;
  
  DEBUG("PUCCH: P=%f -- p0=%f, PL=%f, delta_f=%f, h=%f, g=%f\n", 
         pucch_power, p0_pucch, PL, delta_f, h, g);
  
  return pucch_power;
}

/* Returns the transmission power for SRS for this subframe as defined in Section 5.1.3 of 36.213 */
float srslte_ue_ul_srs_power(srslte_ue_ul_t *q, float PL) {
  float alpha = q->power_ctrl.alpha;
  float p0_pusch = q->power_ctrl.p0_nominal_pusch + q->power_ctrl.p0_ue_pusch;

  //TODO: This implements closed-loop power control
  float f=0;  

  uint32_t M_sc = srslte_refsignal_srs_M_sc(&q->signals);
  
  float p_srs_offset; 
  if (q->power_ctrl.delta_mcs_based) {
    p_srs_offset = -3 + q->power_ctrl.p_srs_offset;
  } else {
    p_srs_offset = -10.5 + 1.5*q->power_ctrl.p_srs_offset;
  }
  
  float p_srs = p_srs_offset + 10*log10(M_sc) + p0_pusch + alpha*PL + f; 
  
  DEBUG("SRS: P=%f -- p_offset=%f, 10M=%f, p0_pusch=%f, alpha=%f, PL=%f, f=%f\n", 
    p_srs, p_srs_offset, 10*log10(M_sc), p0_pusch, alpha, PL, f);
  
  return p_srs; 
}

/* Returns 1 if a SR needs to be sent at current_tti given I_sr, as defined in Section 10.1 of 36.213 */
int srslte_ue_ul_sr_send_tti(uint32_t I_sr, uint32_t current_tti) {
  uint32_t sr_periodicity; 
  uint32_t sr_N_offset;
  if (I_sr < 5) {
    sr_periodicity = 5;
    sr_N_offset    = I_sr; 
  } else if (I_sr < 15) {
    sr_periodicity = 10;
    sr_N_offset    = I_sr-5;     
  } else if (I_sr < 35) {
    sr_periodicity = 20;
    sr_N_offset    = I_sr-15; 
  } else if (I_sr < 75) {
    sr_periodicity = 40;
    sr_N_offset    = I_sr-35; 
  } else if (I_sr < 155) {
    sr_periodicity = 80;
    sr_N_offset    = I_sr-75; 
  } else if (I_sr < 157) {
    sr_periodicity = 2;
    sr_N_offset    = I_sr-155; 
  } else if (I_sr == 157) {
    sr_periodicity = 1;
    sr_N_offset    = I_sr-157; 
  } else {
    return SRSLTE_ERROR;
  }
  if (current_tti >= sr_N_offset) {
    if ((current_tti - sr_N_offset) % sr_periodicity == 0) {
      return 1;
    }
  }
  return SRSLTE_SUCCESS;
}


bool srslte_ue_ul_srs_tx_enabled(srslte_refsignal_srs_cfg_t *srs_cfg, uint32_t tti) {
  if (srs_cfg->configured) {
    if (srslte_refsignal_srs_send_cs(srs_cfg->subframe_config, tti%10) == 1 && 
        srslte_refsignal_srs_send_ue(srs_cfg->I_srs, tti) == 1)
    {
      return true; 
    }
  }
  return false; 
}


