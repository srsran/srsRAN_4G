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

#include "srslte/phy/enb/enb_dl.h"

#include <complex.h>
#include <math.h>
#include <string.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/srslte.h>


#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)

#define SRSLTE_ENB_RF_AMP 0.1

int srslte_enb_dl_init(srslte_enb_dl_t *q, cf_t *out_buffer[SRSLTE_MAX_PORTS], uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q != NULL)
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_enb_dl_t));
    
    q->cfi  = 3;
    q->tx_amp = SRSLTE_ENB_RF_AMP;

    for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
      q->sf_symbols[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
      if (!q->sf_symbols[i]) {
        perror("malloc");
        goto clean_exit;
      }
      q->slot1_symbols[i] = &q->sf_symbols[i][SRSLTE_SLOT_LEN_RE(max_prb, SRSLTE_CP_NORM)];
    }

    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      if (srslte_ofdm_tx_init(&q->ifft[i], SRSLTE_CP_NORM, q->sf_symbols[i], out_buffer[i], max_prb)) {
        fprintf(stderr, "Error initiating FFT (%d)\n", i);
        goto clean_exit;
      }
    }

    if (srslte_ofdm_tx_init_mbsfn(&q->ifft_mbsfn, SRSLTE_CP_EXT, q->sf_symbols[0], out_buffer[0], max_prb)) {
      fprintf(stderr, "Error initiating FFT \n");
      goto clean_exit;
    }

    if (srslte_pbch_init(&q->pbch)) {
      fprintf(stderr, "Error creating PBCH object\n");
      goto clean_exit;
    }
    if (srslte_pcfich_init(&q->pcfich, 0)) {
      fprintf(stderr, "Error creating PCFICH object\n");
      goto clean_exit;
    }
    if (srslte_phich_init(&q->phich, 0)) {
      fprintf(stderr, "Error creating PHICH object\n");
      goto clean_exit;
    }
    int mbsfn_area_id = 1;
    
    
    if (srslte_pmch_init(&q->pmch, max_prb)) {
      fprintf(stderr, "Error creating PMCH object\n");
    }
    srslte_pmch_set_area_id(&q->pmch, mbsfn_area_id);

    
    if (srslte_pdcch_init_enb(&q->pdcch, max_prb)) {
      fprintf(stderr, "Error creating PDCCH object\n");
      goto clean_exit;
    }

    if (srslte_pdsch_init_enb(&q->pdsch, max_prb)) {
      fprintf(stderr, "Error creating PDSCH object\n");
      goto clean_exit;
    }
    
    if (srslte_refsignal_cs_init(&q->csr_signal, max_prb)) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }

    if (srslte_refsignal_mbsfn_init(&q->mbsfnr_signal, max_prb)) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }    
    ret = SRSLTE_SUCCESS;
    
  } else {
    fprintf(stderr, "Invalid parameters\n");
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
    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      srslte_ofdm_tx_free(&q->ifft[i]);
    }
    srslte_ofdm_tx_free(&q->ifft_mbsfn);
    srslte_regs_free(&q->regs);
    srslte_pbch_free(&q->pbch);
    srslte_pcfich_free(&q->pcfich);
    srslte_phich_free(&q->phich);
    srslte_pdcch_free(&q->pdcch);
    srslte_pdsch_free(&q->pdsch);
    srslte_pmch_free(&q->pmch);
    srslte_refsignal_free(&q->csr_signal);
    srslte_refsignal_free(&q->mbsfnr_signal);
    for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
      if (q->sf_symbols[i]) {
        free(q->sf_symbols[i]);
      }
    }
    bzero(q, sizeof(srslte_enb_dl_t));
  }  
}

int srslte_enb_dl_set_cell(srslte_enb_dl_t *q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q                 != NULL &&
      srslte_cell_isvalid(&cell))
  {
    q->tx_amp = SRSLTE_ENB_RF_AMP;

    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      if (q->cell.nof_prb != 0) {
        srslte_regs_free(&q->regs);
      }
      memcpy(&q->cell, &cell, sizeof(srslte_cell_t));
      if (srslte_regs_init(&q->regs, q->cell)) {
        fprintf(stderr, "Error resizing REGs\n");
        return SRSLTE_ERROR;
      }
      for (int i = 0; i < q->cell.nof_ports; i++) {

        q->slot1_symbols[i] = &q->sf_symbols[i][SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, SRSLTE_CP_NORM)];

        if (srslte_ofdm_tx_set_prb(&q->ifft[i], q->cell.cp, q->cell.nof_prb)) {
          fprintf(stderr, "Error re-planning iFFT (%d)\n", i);
          return SRSLTE_ERROR;
        }
      }
    
      if (srslte_ofdm_tx_set_prb(&q->ifft_mbsfn, SRSLTE_CP_EXT, q->cell.nof_prb)) {
        fprintf(stderr, "Error re-planning ifft_mbsfn\n");
        return SRSLTE_ERROR;
      }
      
      srslte_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, 2);
      //srslte_ofdm_set_normalize(&q->ifft_mbsfn, true);
      
      if (srslte_pbch_set_cell(&q->pbch, q->cell)) {
        fprintf(stderr, "Error creating PBCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_pcfich_set_cell(&q->pcfich, &q->regs, q->cell)) {
        fprintf(stderr, "Error creating PCFICH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_phich_set_cell(&q->phich, &q->regs, q->cell)) {
        fprintf(stderr, "Error creating PHICH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdcch_set_cell(&q->pdcch, &q->regs, q->cell)) {
        fprintf(stderr, "Error creating PDCCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdsch_set_cell(&q->pdsch, q->cell)) {
        fprintf(stderr, "Error creating PDSCH object\n");
        return SRSLTE_ERROR;
      }
      
      if (srslte_pmch_set_cell(&q->pmch, q->cell)) {
        fprintf(stderr, "Error creating PMCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_refsignal_cs_set_cell(&q->csr_signal, q->cell)) {
        fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
        return SRSLTE_ERROR;
      }
      int mbsfn_area_id = 1;
      if (srslte_refsignal_mbsfn_set_cell(&q->mbsfnr_signal, q->cell, mbsfn_area_id)) {
        fprintf(stderr, "Error initializing MBSFNR signal (%d)\n",ret);
        return SRSLTE_ERROR;
      }
      /* Generate PSS/SSS signals */
      srslte_pss_generate(q->pss_signal, cell.id%3);
      srslte_sss_generate(q->sss_signal0, q->sss_signal5, cell.id);
    }
    ret = SRSLTE_SUCCESS;

  } else {
    fprintf(stderr, "Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n",
            cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}



void srslte_enb_dl_set_non_mbsfn_region(srslte_enb_dl_t *q, uint8_t non_mbsfn_region)
{
  srslte_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, non_mbsfn_region);
}

void srslte_enb_dl_set_amp(srslte_enb_dl_t *q, float amp)
{
  q->tx_amp = amp; 
}

void srslte_enb_dl_set_cfi(srslte_enb_dl_t *q, uint32_t cfi)
{
  q->cfi = cfi;
}

void srslte_enb_dl_set_power_allocation(srslte_enb_dl_t *q, float rho_a, float rho_b)
{
  if (q) {
    q->rho_b = rho_b;
    srslte_pdsch_set_power_allocation(&q->pdsch, rho_a);
  }
}

void srslte_enb_dl_apply_power_allocation(srslte_enb_dl_t *q)
{
  uint32_t nof_symbols_slot = SRSLTE_CP_NSYMB(q->cell.cp);
  uint32_t nof_re_symbol = SRSLTE_NRE * q->cell.nof_prb;

  if (q->rho_b != 0.0f && q->rho_b != 1.0f) {
    float scaling = q->rho_b;
    for (uint32_t i = 0; i < q->cell.nof_ports; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        cf_t *ptr;
        ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 0);
        srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        if (q->cell.cp == SRSLTE_CP_NORM) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 4);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        } else {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 3);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
        if (q->cell.nof_ports == 4) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 1);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
      }
    }
  }
}

void srslte_enb_dl_prepare_power_allocation(srslte_enb_dl_t *q)
{
  uint32_t nof_symbols_slot = SRSLTE_CP_NSYMB(q->cell.cp);
  uint32_t nof_re_symbol = SRSLTE_NRE * q->cell.nof_prb;

  if (q->rho_b != 0.0f && q->rho_b != 1.0f) {
    float scaling = 1.0f / q->rho_b;
    for (uint32_t i = 0; i < q->cell.nof_ports; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        cf_t *ptr;
        ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 0);
        srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        if (q->cell.cp == SRSLTE_CP_NORM) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 4);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        } else {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 3);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
        if (q->cell.nof_ports == 4) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 1);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
      }
    }
  }
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
    for (int p = 0; p < q->cell.nof_ports; p++) {
      srslte_pss_put_slot(q->pss_signal, q->sf_symbols[p], q->cell.nof_prb, q->cell.cp);
      srslte_sss_put_slot(sf_idx ? q->sss_signal5 : q->sss_signal0, q->sf_symbols[p],
                          q->cell.nof_prb, SRSLTE_CP_NORM);
    }
  }  
}

void srslte_enb_dl_put_refs(srslte_enb_dl_t *q, uint32_t sf_idx)
{
  for (int p = 0; p < q->cell.nof_ports; p++) {
    srslte_refsignal_cs_put_sf(q->cell, (uint32_t) p, q->csr_signal.pilots[p / 2][sf_idx], q->sf_symbols[p]);
  }
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

void srslte_enb_dl_put_mbsfn_base(srslte_enb_dl_t *q, uint32_t tti) 
{
  uint32_t sf_idx1 = tti%10;
  srslte_enb_dl_put_pcfich(q, sf_idx1);
  srslte_refsignal_mbsfn_put_sf(q->cell, 0,q->csr_signal.pilots[0][sf_idx1], q->mbsfnr_signal.pilots[0][sf_idx1], q->sf_symbols[0]);
}

void srslte_enb_dl_gen_signal(srslte_enb_dl_t *q)
{
  // TODO: PAPR control
  float norm_factor = 0.05f / sqrt(q->cell.nof_prb);
  for (int i = 0; i < q->cell.nof_ports; i++) {
    srslte_ofdm_tx_sf(&q->ifft[i]);    
    srslte_vec_sc_prod_cfc(q->ifft[i].out_buffer, norm_factor, q->ifft[i].out_buffer, (uint32_t) SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
  }
}

void srslte_enb_dl_gen_signal_mbsfn(srslte_enb_dl_t *q)
{
  float norm_factor = 0.05f / sqrt(q->cell.nof_prb);
  srslte_ofdm_tx_sf(&q->ifft_mbsfn);
  srslte_vec_sc_prod_cfc(q->ifft_mbsfn.out_buffer, norm_factor, q->ifft_mbsfn.out_buffer, (uint32_t) SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
}

int srslte_enb_dl_add_rnti(srslte_enb_dl_t *q, uint16_t rnti)
{
  return srslte_pdsch_set_rnti(&q->pdsch, rnti);
}

void srslte_enb_dl_rem_rnti(srslte_enb_dl_t *q, uint16_t rnti)
{
  srslte_pdsch_free_rnti(&q->pdsch, rnti);
}

int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t *q, srslte_ra_dl_dci_t *grant, 
                               srslte_dci_format_t format, srslte_dci_location_t location,
                               uint16_t rnti, uint32_t sf_idx) 
{
  srslte_dci_msg_t dci_msg = {};
  
  bool rnti_is_user = true; 
  if (rnti == SRSLTE_SIRNTI || rnti == SRSLTE_PRNTI || (rnti >= SRSLTE_RARNTI_START && rnti <= SRSLTE_RARNTI_END)) {
    rnti_is_user = false; 
  }

  if (srslte_dci_msg_pack_pdsch(grant, format, &dci_msg, q->cell.nof_prb, q->cell.nof_ports, rnti_is_user)) {
    fprintf(stderr, "Error packing DCI grant\n");
  }
  if (srslte_pdcch_encode(&q->pdcch, &dci_msg, location, rnti, q->sf_symbols, sf_idx, q->cfi)) {
    fprintf(stderr, "Error encoding DCI message\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t *q, srslte_ra_ul_dci_t *grant, 
                               srslte_dci_location_t location,
                               uint16_t rnti, uint32_t sf_idx) 
{
  srslte_dci_msg_t dci_msg = {};

  srslte_dci_msg_pack_pusch(grant, &dci_msg, q->cell.nof_prb);
  if (srslte_pdcch_encode(&q->pdcch, &dci_msg, location, rnti, q->sf_symbols, sf_idx, q->cfi)) {
    fprintf(stderr, "Error encoding DCI message\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdsch(srslte_enb_dl_t *q, srslte_ra_dl_grant_t *grant, srslte_softbuffer_tx_t *softbuffer[SRSLTE_MAX_CODEWORDS],
                            uint16_t rnti, int rv_idx[SRSLTE_MAX_CODEWORDS], uint32_t sf_idx,
                            uint8_t *data[SRSLTE_MAX_CODEWORDS], srslte_mimo_type_t mimo_type)
{  
  uint32_t pmi = 0;
  uint32_t nof_tb = SRSLTE_RA_DL_GRANT_NOF_TB(grant);

  /* Translates Precoding Information (pinfo) to Precoding matrix Index (pmi) as 3GPP 36.212 Table 5.3.3.1.5-4 */
  if (mimo_type == SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX) {
    switch(nof_tb) {
      case 1:
        if (grant->pinfo == 0) {
          mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
        } else if (grant->pinfo > 0 && grant->pinfo < 5) {
          pmi = grant->pinfo - 1;
        } else {
          ERROR("Not Implemented (nof_tb=%d, pinfo=%d)", nof_tb, grant->pinfo);
          return SRSLTE_ERROR;
        }
        break;
      case 2:
        if (grant->pinfo < 2) {
          pmi = grant->pinfo;
        } else {
          ERROR("Not Implemented (nof_tb=%d, pinfo=%d)", nof_tb, grant->pinfo);
          return SRSLTE_ERROR;
        }
        break;
      default:
        ERROR("Not Implemented (nof_tb=%d, pinfo=%d)", nof_tb, grant->pinfo);
        return SRSLTE_ERROR;
    }
  }

  /* Configure pdsch_cfg parameters */
  if (srslte_pdsch_cfg_mimo(&q->pdsch_cfg, q->cell, grant, q->cfi, sf_idx, rv_idx, mimo_type, pmi)) {
    ERROR("Error configuring PDSCH (rnti=0x%04x)", rnti);
    return SRSLTE_ERROR;
  }

  /* Encode PDSCH */
  if (srslte_pdsch_encode(&q->pdsch, &q->pdsch_cfg, softbuffer, data, rnti, q->sf_symbols)) {
    fprintf(stderr, "Error encoding PDSCH\n");
    return SRSLTE_ERROR;
  }        
  return SRSLTE_SUCCESS; 
}

int srslte_enb_dl_put_pmch(srslte_enb_dl_t *q, srslte_ra_dl_grant_t *grant, srslte_softbuffer_tx_t *softbuffer, uint32_t sf_idx, uint8_t *data_mbms)
{
   /* Encode PMCH */
         
  int mbsfn_area_id = 1;
  if (srslte_pmch_cfg(&q->pmch_cfg, q->cell, grant, q->cfi, sf_idx)) {
    fprintf(stderr, "Error configuring PMCH\n");
    return SRSLTE_ERROR;
  } 
  /* Encode PMCH */
  if (srslte_pmch_encode(&q->pmch, &q->pmch_cfg, softbuffer, data_mbms, mbsfn_area_id, q->sf_symbols)) {
    fprintf(stderr, "Error encoding PDSCH\n");
    return SRSLTE_ERROR;
  }
    
 return SRSLTE_SUCCESS; 
}

void srslte_enb_dl_save_signal(srslte_enb_dl_t *q, srslte_softbuffer_tx_t *softbuffer, uint8_t *data, uint32_t tti, uint32_t rv_idx, uint16_t rnti, uint32_t cfi)
{
  char tmpstr[64];

  snprintf(tmpstr,64,"output/sf_symbols_%d",tti);
  srslte_vec_save_file(tmpstr, q->sf_symbols[0], SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));

  snprintf(tmpstr,64,"output/e_%d",tti);
  srslte_bit_unpack_vector(q->pdsch.e[0], q->tmp, q->pdsch_cfg.nbits[0].nof_bits);
  srslte_vec_save_file(tmpstr, q->tmp, q->pdsch_cfg.nbits[0].nof_bits*sizeof(uint8_t));

  /*
  int cb_len = q->pdsch_cfg.cb_segm[0].K1;
  for (int i=0;i<q->pdsch_cfg.cb_segm[0].C;i++) {
    snprintf(tmpstr,64,"output/rmout_%d_%d",i,tti);
    srslte_bit_unpack_vector(softbuffer->buffer_b[i], q->tmp, (3*cb_len+12));
    srslte_vec_save_file(tmpstr, q->tmp, (3*cb_len+12)*sizeof(uint8_t));
  }*/

  snprintf(tmpstr,64,"output/data_%d",tti);
  srslte_bit_unpack_vector(data, q->tmp, q->pdsch_cfg.grant.mcs[0].tbs);
  srslte_vec_save_file(tmpstr, q->tmp, q->pdsch_cfg.grant.mcs[0].tbs*sizeof(uint8_t));

  //printf("Saved files for tti=%d, sf=%d, cfi=%d, mcs=%d, tbs=%d, rv=%d, rnti=0x%x\n", tti, tti%10, cfi,
  //       q->pdsch_cfg.grant.mcs[0].idx, q->pdsch_cfg.grant.mcs[0].tbs, rv_idx, rnti);
}

