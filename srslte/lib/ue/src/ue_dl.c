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

#include "srslte/ue/ue_dl.h"

#include <complex.h>
#include <math.h>
#include <string.h>


#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define MAX_CANDIDATES  64

#define PDSCH_DO_ZF

srslte_dci_format_t ue_formats[] = {SRSLTE_DCI_FORMAT1A, SRSLTE_DCI_FORMAT1}; // SRSLTE_DCI_FORMAT1B should go here also
const uint32_t nof_ue_formats = 2; 

srslte_dci_format_t common_formats[] = {SRSLTE_DCI_FORMAT1A,SRSLTE_DCI_FORMAT1C};
const uint32_t nof_common_formats = 2; 


int srslte_ue_dl_init(srslte_ue_dl_t *q, 
               srslte_cell_t cell) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL &&
      srslte_cell_isvalid(&cell))   
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_ue_dl_t));
    
    q->cell = cell; 
    q->pkt_errors = 0;
    q->pkts_total = 0;
    q->pending_ul_dci_rnti = 0; 
    
    if (srslte_ofdm_rx_init(&q->fft, q->cell.cp, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }
    if (srslte_chest_dl_init(&q->chest, cell)) {
      fprintf(stderr, "Error initiating channel estimator\n");
      goto clean_exit;
    }
    if (srslte_regs_init(&q->regs, q->cell)) {
      fprintf(stderr, "Error initiating REGs\n");
      goto clean_exit;
    }
    if (srslte_pcfich_init(&q->pcfich, &q->regs, q->cell)) {
      fprintf(stderr, "Error creating PCFICH object\n");
      goto clean_exit;
    }
    if (srslte_phich_init(&q->phich, &q->regs, q->cell)) {
      fprintf(stderr, "Error creating PHICH object\n");
      goto clean_exit;
    }

    if (srslte_pdcch_init(&q->pdcch, &q->regs, q->cell)) {
      fprintf(stderr, "Error creating PDCCH object\n");
      goto clean_exit;
    }

    if (srslte_pdsch_init(&q->pdsch, q->cell)) {
      fprintf(stderr, "Error creating PDSCH object\n");
      goto clean_exit;
    }
    if (srslte_softbuffer_rx_init(&q->softbuffer, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating soft buffer\n");
      goto clean_exit;
    }
    q->sf_symbols = srslte_vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit; 
    }
    for (uint32_t i=0;i<q->cell.nof_ports;i++) {
      q->ce[i] = srslte_vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit; 
      }
    }
    
    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);      
  }

clean_exit: 
  if (ret == SRSLTE_ERROR) {
    srslte_ue_dl_free(q);
  }
  return ret;
}

void srslte_ue_dl_free(srslte_ue_dl_t *q) {
  if (q) {
    srslte_ofdm_rx_free(&q->fft);
    srslte_chest_dl_free(&q->chest);
    srslte_regs_free(&q->regs);
    srslte_pcfich_free(&q->pcfich);
    srslte_phich_free(&q->phich);
    srslte_pdcch_free(&q->pdcch);
    srslte_pdsch_free(&q->pdsch);
    srslte_softbuffer_rx_free(&q->softbuffer);
    if (q->sf_symbols) {
      free(q->sf_symbols);
    }
    for (uint32_t i=0;i<q->cell.nof_ports;i++) {
      if (q->ce[i]) {
        free(q->ce[i]);
      }
    }
    bzero(q, sizeof(srslte_ue_dl_t));
  }
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode_rnti() or srslte_pusch_decode_rnti() functions 
 */
void srslte_ue_dl_set_rnti(srslte_ue_dl_t *q, uint16_t rnti) {
  srslte_pdsch_set_rnti(&q->pdsch, rnti);
  q->current_rnti = rnti; 
}

void srslte_ue_dl_reset(srslte_ue_dl_t *q) {
  srslte_softbuffer_rx_reset(&q->softbuffer);
  bzero(&q->pdsch_cfg, sizeof(srslte_pdsch_cfg_t));
}

/** Applies the following operations to a subframe of synchronized samples: 
 *    - OFDM demodulation
 *    - Channel estimation 
 *    - PCFICH decoding
 *    - PDCCH decoding: Find DCI for RNTI given by previous call to srslte_ue_dl_set_rnti()
 *    - PDSCH decoding: Decode TB scrambling with RNTI given by srslte_ue_dl_set_rnti()
 */
int srslte_ue_dl_decode(srslte_ue_dl_t *q, cf_t *input, uint8_t *data, uint32_t sf_idx) {
  return srslte_ue_dl_decode_rnti_rv(q, input, data, sf_idx, q->current_rnti, 0);
}

int srslte_ue_dl_decode_rnti(srslte_ue_dl_t *q, cf_t *input, uint8_t *data, uint32_t sf_idx, uint16_t rnti) {
  return srslte_ue_dl_decode_rnti_rv(q, input, data, sf_idx, rnti, 0);
}

int srslte_ue_dl_decode_fft_estimate(srslte_ue_dl_t *q, cf_t *input, uint32_t sf_idx, uint32_t *cfi) {
  float cfi_corr; 
  if (input && q && cfi && sf_idx < SRSLTE_NSUBFRAMES_X_FRAME) {
    
    /* Run FFT for all subframe data */
    srslte_ofdm_rx_sf(&q->fft, input, q->sf_symbols);
    
    /* Get channel estimates for each port */
    srslte_chest_dl_estimate(&q->chest, q->sf_symbols, q->ce, sf_idx);

    /* First decode PCFICH and obtain CFI */
    if (srslte_pcfich_decode(&q->pcfich, q->sf_symbols, q->ce, 
                      srslte_chest_dl_get_noise_estimate(&q->chest), sf_idx, cfi, &cfi_corr)<0) {
      fprintf(stderr, "Error decoding PCFICH\n");
      return SRSLTE_ERROR;
    }

    INFO("Decoded CFI=%d with correlation %.2f\n", *cfi, cfi_corr);

    if (srslte_regs_set_cfi(&q->regs, *cfi)) {
      fprintf(stderr, "Error setting CFI\n");
      return SRSLTE_ERROR;
    }
    
    return SRSLTE_SUCCESS; 
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

int srslte_ue_dl_cfg_grant(srslte_ue_dl_t *q, srslte_ra_dl_grant_t *grant, uint32_t cfi, uint32_t sf_idx, uint32_t rvidx) 
{
  return srslte_pdsch_cfg(&q->pdsch_cfg, q->cell, grant, cfi, sf_idx, rvidx);
}

int srslte_ue_dl_decode_rnti_rv_packet(srslte_ue_dl_t *q, srslte_ra_dl_grant_t *grant, uint8_t *data, 
                                uint32_t cfi, uint32_t sf_idx, uint16_t rnti, uint32_t rvidx) 
{
  int ret = SRSLTE_ERROR; 

  q->nof_detected++;
  
  /* Setup PDSCH configuration for this CFI, SFIDX and RVIDX */
  if (srslte_ue_dl_cfg_grant(q, grant, cfi, sf_idx, rvidx)) {
    return SRSLTE_ERROR; 
  }
  
  if (q->pdsch_cfg.rv == 0) {
    srslte_softbuffer_rx_reset(&q->softbuffer);
  }
  
#ifdef PDSCH_DO_ZF
  float noise_estimate = 0; 
#else
  float noise_estimate = srslte_chest_dl_get_noise_estimate(&q->chest);
#endif
  
  if (q->pdsch_cfg.grant.mcs.mod > 0 && q->pdsch_cfg.grant.mcs.tbs >= 0) {
    ret = srslte_pdsch_decode_rnti(&q->pdsch, &q->pdsch_cfg, &q->softbuffer, 
                                   q->sf_symbols, q->ce, 
                                   noise_estimate, 
                                   rnti, data);
    
    if (ret == SRSLTE_ERROR) {
      q->pkt_errors++;
    } else if (ret == SRSLTE_ERROR_INVALID_INPUTS) {
      fprintf(stderr, "Error calling srslte_pdsch_decode()\n");      
    } else if (ret == SRSLTE_SUCCESS) {
      if (SRSLTE_VERBOSE_ISDEBUG()) {
        INFO("Decoded Message: ", 0);
        srslte_vec_fprint_hex(stdout, data, q->pdsch_cfg.grant.mcs.tbs);
      }
    }
    q->pkts_total++;
  }
  return ret; 
}

int srslte_ue_dl_find_ul_dci(srslte_ue_dl_t *q, srslte_dci_msg_t *dci_msg, uint32_t cfi, uint32_t sf_idx, uint16_t rnti)
{
  srslte_dci_location_t locations[MAX_CANDIDATES];
  uint32_t nof_locations = srslte_pdcch_ue_locations(&q->pdcch, locations, MAX_CANDIDATES, sf_idx, cfi, rnti);    
  uint16_t crc_rem = 0; 
  
  if (rnti) {
    /* Do not search if an UL DCI is already pending */
    
    if (q->pending_ul_dci_rnti == rnti) {
      q->pending_ul_dci_rnti = 0; 
      memcpy(dci_msg, &q->pending_ul_dci_msg, sizeof(srslte_dci_msg_t));
      return 1; 
    }
    
    for (uint32_t i=0;i<nof_locations && crc_rem != rnti;i++) {
      if (srslte_pdcch_decode_msg(&q->pdcch, dci_msg, &locations[i], SRSLTE_DCI_FORMAT0, &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return SRSLTE_ERROR;
      }
      if (dci_msg->data[0] != 0) {
        crc_rem = 0; 
      }
      DEBUG("Decoded DCI message RNTI: 0x%x\n", crc_rem);
      if (crc_rem == rnti) {
        memcpy(&q->last_location, &locations[i], sizeof(srslte_dci_location_t));
      }
    } 
    if (crc_rem == rnti) {
      return 1; 
    } else {
      return 0; 
    }
  } else {
    return 0; 
  }
}

uint32_t srslte_ue_dl_get_ncce(srslte_ue_dl_t *q) {
  return q->last_n_cce; 
}

int srslte_ue_dl_find_dl_dci(srslte_ue_dl_t *q, srslte_dci_msg_t *dci_msg, uint32_t cfi, uint32_t sf_idx, uint16_t rnti)
{
  srslte_rnti_type_t rnti_type; 
  if (rnti == SRSLTE_SIRNTI) {
    rnti_type = SRSLTE_RNTI_SI;
  } else if (rnti == SRSLTE_PRNTI) {
    rnti_type = SRSLTE_RNTI_PCH;    
  } else if (rnti <= SRSLTE_RARNTI_END) {
    rnti_type = SRSLTE_RNTI_RAR;    
  } else {
    rnti_type = SRSLTE_RNTI_USER;
  }
  return srslte_ue_dl_find_dl_dci_type(q, dci_msg, cfi, sf_idx, rnti, rnti_type);
}

int srslte_ue_dl_find_dl_dci_type(srslte_ue_dl_t *q, srslte_dci_msg_t *dci_msg, uint32_t cfi, uint32_t sf_idx, 
                                  uint16_t rnti, srslte_rnti_type_t rnti_type)
{
  srslte_dci_location_t locations[MAX_CANDIDATES];
  uint32_t nof_locations;
  uint32_t nof_formats; 
  srslte_dci_format_t *formats = NULL; 

  /* Generate PDCCH candidates */
  if (rnti_type == SRSLTE_RNTI_SI || rnti_type == SRSLTE_RNTI_PCH || rnti_type == SRSLTE_RNTI_RAR) {
    nof_locations = srslte_pdcch_common_locations(&q->pdcch, locations, MAX_CANDIDATES, q->cfi);
    formats = common_formats;
    nof_formats = nof_common_formats;
  } else {
    nof_locations = srslte_pdcch_ue_locations(&q->pdcch, locations, MAX_CANDIDATES, sf_idx, q->cfi, rnti);    
    formats = ue_formats; 
    nof_formats = nof_ue_formats;
  }

  uint16_t crc_rem = 0; 
  for (int f=0;f<nof_formats && crc_rem != rnti;f++) {
    for (int i=0;i<nof_locations && crc_rem != rnti;i++) {
      INFO("Trying format %s (nbits=%d), location L=%d, ncce=%d\n", srslte_dci_format_string(formats[f]), 
             srslte_dci_format_sizeof_lut(formats[f], q->cell.nof_prb), locations[i].L, locations[i].ncce);
      q->last_n_cce = locations[i].ncce;
      if (srslte_pdcch_decode_msg(&q->pdcch, dci_msg, &locations[i], formats[f], &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return SRSLTE_ERROR;
      }
      if (crc_rem == rnti) {
        memcpy(&q->last_location, &locations[i], sizeof(srslte_dci_location_t));
      }
      if (crc_rem == rnti && formats[f] == SRSLTE_DCI_FORMAT1A && dci_msg->data[0] != 1) {
        /* Save Format 0 msg. Recovered next call to srslte_ue_dl_find_ul_dci() */
        q->pending_ul_dci_rnti = crc_rem; 
        memcpy(&q->pending_ul_dci_msg, dci_msg, sizeof(srslte_dci_msg_t));
        crc_rem = 0;         
      }
      DEBUG("Decoded DCI message RNTI: 0x%x\n", crc_rem);
    }
  } 
  if (crc_rem == rnti) {
    return 1; 
  } else {
    return 0; 
  }
}


int srslte_ue_dl_decode_rnti_rv(srslte_ue_dl_t *q, cf_t *input, uint8_t *data, uint32_t sf_idx, uint16_t rnti, uint32_t rvidx) 
{
  srslte_dci_msg_t dci_msg;
  srslte_ra_dl_dci_t dci_unpacked;
  srslte_ra_dl_grant_t grant; 
  int ret = SRSLTE_ERROR; 
  
  if ((ret = srslte_ue_dl_decode_fft_estimate(q, input, sf_idx, &q->cfi)) < 0) {
    return ret; 
  }
  
  if (srslte_pdcch_extract_llr(&q->pdcch, q->sf_symbols, q->ce, 0, sf_idx, q->cfi)) {
    fprintf(stderr, "Error extracting LLRs\n");
    return SRSLTE_ERROR;
  }

  int found_dci = srslte_ue_dl_find_dl_dci(q, &dci_msg, q->cfi, sf_idx, rnti); 
  
  if (found_dci == 1) {
    
    if (srslte_dci_msg_to_dl_grant(&dci_msg, rnti, q->cell.nof_prb, &dci_unpacked, &grant)) {
      fprintf(stderr, "Error unpacking DCI\n");
      return SRSLTE_ERROR;   
    }

    ret = srslte_ue_dl_decode_rnti_rv_packet(q, &grant, data, q->cfi, sf_idx, rnti, rvidx);    
  }
   
  if (found_dci == 1 && ret == SRSLTE_SUCCESS) { 
    return q->pdsch_cfg.grant.mcs.tbs;    
  } else {
    return 0;
  }
}

/* Computes n_group and n_seq according to Section 9.1.2 in 36.213 and calls phich processing function */
bool srslte_ue_dl_decode_phich(srslte_ue_dl_t *q, uint32_t sf_idx, uint32_t n_prb_lowest, uint32_t n_dmrs)
{
  uint8_t ack_bit; 
  float distance;
  uint32_t Ngroups = srslte_phich_ngroups(&q->phich); 
  uint32_t ngroup = (n_prb_lowest+n_dmrs)%Ngroups;
  uint32_t nseq = ((n_prb_lowest/Ngroups)+n_dmrs)%(2*srslte_phich_nsf(&q->phich));
  DEBUG("Decoding PHICH sf_idx=%d, n_prb_lowest=%d, n_dmrs=%d, n_group=%d, n_seq=%d\n", 
    sf_idx, n_prb_lowest, n_dmrs, ngroup, nseq);
  if (!srslte_phich_decode(&q->phich, q->sf_symbols, q->ce, 0, ngroup, nseq, sf_idx, &ack_bit, &distance)) {
    INFO("Decoded PHICH %d with distance %f\n", ack_bit, distance);    
  } else {
    fprintf(stderr, "Error decoding PHICH\n");
    return false; 
  }
  if (ack_bit && distance > 1.5) {
    return true; 
  } else {
    return false; 
  }
}



