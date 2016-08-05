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

#include "srslte/enb/enb_dl.h"

#include <complex.h>
#include <math.h>
#include <string.h>


#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define SRSLTE_ENB_RF_AMP 0.8

int srslte_enb_dl_init(srslte_enb_dl_t *q, srslte_cell_t cell, uint32_t nof_rnti)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL &&
      srslte_cell_isvalid(&cell))   
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_enb_dl_t));
    
    q->cell = cell;
    q->cfi  = 3; 
    q->nof_rnti = nof_rnti; 
    
    if (srslte_ofdm_tx_init(&q->ifft, q->cell.cp, q->cell.nof_prb)) {
      fprintf(stderr, "Error initiating FFT\n");
      goto clean_exit;
    }

    srslte_ofdm_set_normalize(&q->ifft, true);

    if (srslte_regs_init(&q->regs, q->cell)) {
      fprintf(stderr, "Error initiating REGs\n");
      goto clean_exit;
    }
    if (srslte_pbch_init(&q->pbch, q->cell)) {
      fprintf(stderr, "Error creating PBCH object\n");
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
    
    if (srslte_pdsch_init_rnti_multi(&q->pdsch, nof_rnti)) {
      fprintf(stderr, "Error initiating multiple RNTIs in PDSCH\n");
      goto clean_exit;
    }
        
    if (srslte_refsignal_cs_init(&q->csr_signal, q->cell)) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    for (int i=0;i<q->cell.nof_ports;i++) {
      q->sf_symbols[i] = srslte_vec_malloc(CURRENT_SFLEN_RE * sizeof(cf_t));
      if (!q->sf_symbols[i]) {
        perror("malloc");
        goto clean_exit; 
      }
      q->slot1_symbols[i] = &q->sf_symbols[i][CURRENT_SLOTLEN_RE];
    }
    
    /* Generate PSS/SSS signals */
    srslte_pss_generate(q->pss_signal, cell.id%3);
    srslte_sss_generate(q->sss_signal0, q->sss_signal5, cell.id);
    
    ret = SRSLTE_SUCCESS;
    
  } else {
    fprintf(stderr, "Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);      
  }

clean_exit: 
  if (ret == SRSLTE_ERROR) {
    srslte_enb_dl_free(q);
  }
  return ret;
}

void srslte_enb_dl_free(srslte_enb_dl_t *q)
{
  if (q) {
    srslte_ofdm_tx_free(&q->ifft);
    srslte_regs_free(&q->regs);
    srslte_pcfich_free(&q->pcfich);
    srslte_phich_free(&q->phich);
    srslte_pdcch_free(&q->pdcch);
    srslte_pdsch_free(&q->pdsch);
    
    srslte_refsignal_cs_free(&q->csr_signal);
    
    for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
      if (q->sf_symbols[i]) {
        free(q->sf_symbols[i]);
      }
    }
    bzero(q, sizeof(srslte_enb_dl_t));
  }  
}

void srslte_enb_dl_set_cfi(srslte_enb_dl_t *q, uint32_t cfi) 
{
  q->cfi = cfi; 
  srslte_regs_set_cfi(&q->regs, cfi);
}

void srslte_enb_dl_clear_sf(srslte_enb_dl_t *q)
{
  for (int i=0;i<q->cell.nof_ports;i++) {
    bzero(q->sf_symbols[i], CURRENT_SFLEN_RE * sizeof(cf_t));  
  }
}

void srslte_enb_dl_put_sync(srslte_enb_dl_t *q, uint32_t sf_idx) 
{
  if (sf_idx == 0 || sf_idx == 5) {
    srslte_pss_put_slot(q->pss_signal, q->sf_symbols[0], q->cell.nof_prb, q->cell.cp);
    srslte_sss_put_slot(sf_idx ? q->sss_signal5 : q->sss_signal0, q->sf_symbols[0], 
                        q->cell.nof_prb, SRSLTE_CP_NORM);
  }  
}

void srslte_enb_dl_put_refs(srslte_enb_dl_t *q, uint32_t sf_idx)
{
  srslte_refsignal_cs_put_sf(q->cell, 0, q->csr_signal.pilots[0][sf_idx], q->sf_symbols[0]);
}

void srslte_enb_dl_put_mib(srslte_enb_dl_t *q, uint32_t tti)
{
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  if ((tti%10) == 0) {
    srslte_pbch_mib_pack(&q->cell, tti/10, bch_payload);
    srslte_pbch_encode(&q->pbch, bch_payload, q->slot1_symbols, ((tti/10)%4));
  }  
}

void srslte_enb_dl_put_pcfich(srslte_enb_dl_t *q, uint32_t sf_idx)
{
  srslte_pcfich_encode(&q->pcfich, q->cfi, q->sf_symbols, sf_idx);         
}

void srslte_enb_dl_put_phich(srslte_enb_dl_t *q, uint8_t ack, uint32_t n_prb_lowest, 
                             uint32_t n_dmrs, uint32_t sf_idx)
{
  uint32_t ngroup, nseq; 
  srslte_phich_calc(&q->phich, n_prb_lowest, n_dmrs, &ngroup, &nseq);
  srslte_phich_encode(&q->phich, ack, ngroup, nseq, sf_idx, q->sf_symbols);
}

void srslte_enb_dl_put_base(srslte_enb_dl_t *q, uint32_t tti) 
{
  uint32_t sf_idx = tti%10;
  
  srslte_enb_dl_put_sync(q, sf_idx);
  srslte_enb_dl_put_refs(q, sf_idx);
  srslte_enb_dl_put_mib(q, tti);
  srslte_enb_dl_put_pcfich(q, sf_idx);
  
}

void srslte_enb_dl_gen_signal(srslte_enb_dl_t *q, cf_t *signal_buffer) 
{
  
  srslte_ofdm_tx_sf(&q->ifft, q->sf_symbols[0], signal_buffer);
     
  // TODO: PAPR control
  float norm_factor = (float) sqrt(q->cell.nof_prb)/15;
  srslte_vec_sc_prod_cfc(signal_buffer, SRSLTE_ENB_RF_AMP*norm_factor, signal_buffer, SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
}

int srslte_enb_dl_cfg_rnti(srslte_enb_dl_t *q, uint32_t idx, uint16_t rnti)
{
  return srslte_pdsch_set_rnti_multi(&q->pdsch, idx, rnti);
}

int srslte_enb_dl_rem_rnti(srslte_enb_dl_t *q, uint32_t idx)
{
  return srslte_pdsch_set_rnti_multi(&q->pdsch, idx, 0);
}

int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t *q, srslte_ra_dl_dci_t *grant, 
                               srslte_dci_format_t format, srslte_dci_location_t location,
                               uint32_t rnti_idx, uint32_t sf_idx) 
{
  srslte_dci_msg_t dci_msg;

  uint16_t rnti = srslte_pdsch_get_rnti_multi(&q->pdsch, rnti_idx);
  
  bool rnti_is_user = true; 
  if (rnti == SRSLTE_SIRNTI || rnti == SRSLTE_PRNTI || (rnti >= SRSLTE_RARNTI_START && rnti <= SRSLTE_RARNTI_END)) {
    rnti_is_user = false; 
  }
  
  srslte_dci_msg_pack_pdsch(grant, format, &dci_msg, q->cell.nof_prb, rnti_is_user);
  if (srslte_pdcch_encode(&q->pdcch, &dci_msg, location, rnti, q->sf_symbols, sf_idx, q->cfi)) {
    fprintf(stderr, "Error encoding DCI message\n");
    return SRSLTE_ERROR;
  }
/*  printf("format: %s, sf_idx=%d, rnti=%d, location=%d,%d, cfi=%d\n", 
	 srslte_dci_format_string(format), sf_idx, rnti, location.L, location.ncce, q->cfi);
  srslte_ra_pdsch_fprint(stdout, grant, q->cell.nof_prb);
  srslte_vec_fprint_hex(stdout, dci_msg.data, dci_msg.nof_bits);
*/

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t *q, srslte_ra_ul_dci_t *grant, 
                               srslte_dci_location_t location,
                               uint32_t rnti_idx, uint32_t sf_idx) 
{
  srslte_dci_msg_t dci_msg;

  uint16_t rnti = srslte_pdsch_get_rnti_multi(&q->pdsch, rnti_idx);
  
  srslte_dci_msg_pack_pusch(grant, &dci_msg, q->cell.nof_prb);
  if (srslte_pdcch_encode(&q->pdcch, &dci_msg, location, rnti, q->sf_symbols, sf_idx, q->cfi)) {
    fprintf(stderr, "Error encoding DCI message\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdsch(srslte_enb_dl_t *q, srslte_ra_dl_grant_t *grant, srslte_softbuffer_tx_t *softbuffer,
                            uint32_t rnti_idx, uint32_t rv_idx, uint32_t sf_idx, 
                            uint8_t *data) 
{
  //srslte_ra_dl_grant_fprint(stdout, grant);
  
  /* Configure pdsch_cfg parameters */
  if (srslte_pdsch_cfg(&q->pdsch_cfg, q->cell, grant, q->cfi, sf_idx, rv_idx)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    return SRSLTE_ERROR;
  }

  /* Encode PDSCH */
  if (srslte_pdsch_encode_rnti_idx(&q->pdsch, &q->pdsch_cfg, softbuffer, data, rnti_idx, q->sf_symbols)) {
    fprintf(stderr, "Error encoding PDSCH\n");
    return SRSLTE_ERROR;
  }        
  return SRSLTE_SUCCESS; 
}
