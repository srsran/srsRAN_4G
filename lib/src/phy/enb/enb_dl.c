/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/phy/enb/enb_dl.h"

#include "srslte/srslte.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#define CURRENT_FFTSIZE   srslte_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN     SRSLTE_SF_LEN(CURRENT_FFTSIZE)

#define CURRENT_SLOTLEN_RE SRSLTE_SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp)
#define CURRENT_SFLEN_RE SRSLTE_NOF_RE(q->cell)

#define SRSLTE_ENB_RF_AMP 0.1

int srslte_enb_dl_init(srslte_enb_dl_t *q, cf_t *out_buffer[SRSLTE_MAX_PORTS], uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q != NULL)
  {
    ret = SRSLTE_ERROR;
    
    bzero(q, sizeof(srslte_enb_dl_t));

    for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
      q->sf_symbols[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
      if (!q->sf_symbols[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      if (srslte_ofdm_tx_init(&q->ifft[i], SRSLTE_CP_NORM, q->sf_symbols[i], out_buffer[i], max_prb)) {
        ERROR("Error initiating FFT (%d)\n", i);
        goto clean_exit;
      }
    }

    if (srslte_ofdm_tx_init_mbsfn(&q->ifft_mbsfn, SRSLTE_CP_EXT, q->sf_symbols[0], out_buffer[0], max_prb)) {
      ERROR("Error initiating FFT \n");
      goto clean_exit;
    }

    if (srslte_pbch_init(&q->pbch)) {
      ERROR("Error creating PBCH object\n");
      goto clean_exit;
    }
    if (srslte_pcfich_init(&q->pcfich, 0)) {
      ERROR("Error creating PCFICH object\n");
      goto clean_exit;
    }
    if (srslte_phich_init(&q->phich, 0)) {
      ERROR("Error creating PHICH object\n");
      goto clean_exit;
    }
    int mbsfn_area_id = 1;

    if (srslte_pmch_init(&q->pmch, max_prb, 1)) {
      ERROR("Error creating PMCH object\n");
    }
    srslte_pmch_set_area_id(&q->pmch, mbsfn_area_id);

    if (srslte_pdcch_init_enb(&q->pdcch, max_prb)) {
      ERROR("Error creating PDCCH object\n");
      goto clean_exit;
    }

    if (srslte_pdsch_init_enb(&q->pdsch, max_prb)) {
      ERROR("Error creating PDSCH object\n");
      goto clean_exit;
    }

    if (srslte_refsignal_cs_init(&q->csr_signal, max_prb)) {
      ERROR("Error initializing CSR signal (%d)\n", ret);
      goto clean_exit;
    }

    if (srslte_refsignal_mbsfn_init(&q->mbsfnr_signal, max_prb)) {
      ERROR("Error initializing CSR signal (%d)\n", ret);
      goto clean_exit;
    }
    ret = SRSLTE_SUCCESS;
    
  } else {
    ERROR("Invalid parameters\n");
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
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      if (q->cell.nof_prb != 0) {
        srslte_regs_free(&q->regs);
      }
      q->cell = cell;
      if (srslte_regs_init(&q->regs, q->cell)) {
        ERROR("Error resizing REGs\n");
        return SRSLTE_ERROR;
      }
      for (int i = 0; i < q->cell.nof_ports; i++) {
        if (srslte_ofdm_tx_set_prb(&q->ifft[i], q->cell.cp, q->cell.nof_prb)) {
          ERROR("Error re-planning iFFT (%d)\n", i);
          return SRSLTE_ERROR;
        }
      }
    
      if (srslte_ofdm_tx_set_prb(&q->ifft_mbsfn, SRSLTE_CP_EXT, q->cell.nof_prb)) {
        ERROR("Error re-planning ifft_mbsfn\n");
        return SRSLTE_ERROR;
      }

      srslte_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, 2);

      if (srslte_pbch_set_cell(&q->pbch, q->cell)) {
        ERROR("Error creating PBCH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_pcfich_set_cell(&q->pcfich, &q->regs, q->cell)) {
        ERROR("Error creating PCFICH object\n");
        return SRSLTE_ERROR;
      }
      if (srslte_phich_set_cell(&q->phich, &q->regs, q->cell)) {
        ERROR("Error creating PHICH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdcch_set_cell(&q->pdcch, &q->regs, q->cell)) {
        ERROR("Error creating PDCCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pdsch_set_cell(&q->pdsch, q->cell)) {
        ERROR("Error creating PDSCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_pmch_set_cell(&q->pmch, q->cell)) {
        ERROR("Error creating PMCH object\n");
        return SRSLTE_ERROR;
      }

      if (srslte_refsignal_cs_set_cell(&q->csr_signal, q->cell)) {
        ERROR("Error initializing CSR signal (%d)\n", ret);
        return SRSLTE_ERROR;
      }
      int mbsfn_area_id = 1;
      if (srslte_refsignal_mbsfn_set_cell(&q->mbsfnr_signal, q->cell, mbsfn_area_id)) {
        ERROR("Error initializing MBSFNR signal (%d)\n", ret);
        return SRSLTE_ERROR;
      }
      /* Generate PSS/SSS signals */
      srslte_pss_generate(q->pss_signal, cell.id%3);
      srslte_sss_generate(q->sss_signal0, q->sss_signal5, cell.id);
    }
    ret = SRSLTE_SUCCESS;

  } else {
    ERROR("Invalid cell properties: Id=%d, Ports=%d, PRBs=%d\n", cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

int srslte_enb_dl_add_rnti(srslte_enb_dl_t* q, uint16_t rnti)
{
  return srslte_pdsch_set_rnti(&q->pdsch, rnti);
}

void srslte_enb_dl_rem_rnti(srslte_enb_dl_t* q, uint16_t rnti)
{
  srslte_pdsch_free_rnti(&q->pdsch, rnti);
}

#ifdef resolve
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

#endif

static void clear_sf(srslte_enb_dl_t* q)
{
  for (int i = 0; i < q->cell.nof_ports; i++) {
    bzero(q->sf_symbols[i], CURRENT_SFLEN_RE * sizeof(cf_t));
  }
}

static void put_sync(srslte_enb_dl_t* q)
{
  uint32_t sf_idx = q->dl_sf.tti % 10;

  if (sf_idx == 0 || sf_idx == 5) {
    for (int p = 0; p < q->cell.nof_ports; p++) {
      srslte_pss_put_slot(q->pss_signal, q->sf_symbols[p], q->cell.nof_prb, q->cell.cp);
      srslte_sss_put_slot(sf_idx ? q->sss_signal5 : q->sss_signal0, q->sf_symbols[p],
                          q->cell.nof_prb, q->cell.cp);
    }
  }  
}

static void put_refs(srslte_enb_dl_t* q)
{
  uint32_t sf_idx = q->dl_sf.tti % 10;
  if (q->dl_sf.sf_type == SRSLTE_SF_MBSFN) {
    srslte_refsignal_mbsfn_put_sf(
        q->cell, 0, q->csr_signal.pilots[0][sf_idx], q->mbsfnr_signal.pilots[0][sf_idx], q->sf_symbols[0]);
  } else {
    for (int p = 0; p < q->cell.nof_ports; p++) {
      srslte_refsignal_cs_put_sf(&q->csr_signal, &q->dl_sf, (uint32_t)p, q->sf_symbols[p]);
    }
  }
}

static void put_mib(srslte_enb_dl_t* q)
{
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  uint32_t sf_idx = q->dl_sf.tti % 10;
  uint32_t sfn    = q->dl_sf.tti / 10;

  if (sf_idx == 0) {
    srslte_pbch_mib_pack(&q->cell, sfn, bch_payload);
    srslte_pbch_encode(&q->pbch, bch_payload, q->sf_symbols, sfn % 4);
  }
}

static void put_pcfich(srslte_enb_dl_t* q)
{
  srslte_pcfich_encode(&q->pcfich, &q->dl_sf, q->sf_symbols);
}

void srslte_enb_dl_put_base(srslte_enb_dl_t* q, srslte_dl_sf_cfg_t* dl_sf)
{
  srslte_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, dl_sf->non_mbsfn_region);
  q->dl_sf = *dl_sf;
  clear_sf(q);
  put_sync(q);
  put_refs(q);
  put_mib(q);
  put_pcfich(q);
}

void srslte_enb_dl_put_phich(srslte_enb_dl_t* q, srslte_phich_grant_t* grant, bool ack)
{
  srslte_phich_resource_t resource;
  srslte_phich_calc(&q->phich, grant, &resource);
  srslte_phich_encode(&q->phich, &q->dl_sf, resource, ack, q->sf_symbols);
}

int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t* q, srslte_dci_cfg_t* dci_cfg, srslte_dci_dl_t* dci_dl)
{
  srslte_dci_msg_t dci_msg;
  ZERO_OBJECT(dci_msg);

  if (srslte_dci_msg_pack_pdsch(&q->cell, &q->dl_sf, dci_cfg, dci_dl, &dci_msg)) {
    ERROR("Error packing DL DCI\n");
  }
  if (srslte_pdcch_encode(&q->pdcch, &q->dl_sf, &dci_msg, q->sf_symbols)) {
    ERROR("Error encoding DL DCI message\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t* q, srslte_dci_cfg_t* dci_cfg, srslte_dci_ul_t* dci_ul)
{
  srslte_dci_msg_t dci_msg;
  ZERO_OBJECT(dci_msg);

  if (srslte_dci_msg_pack_pusch(&q->cell, &q->dl_sf, dci_cfg, dci_ul, &dci_msg)) {
    ERROR("Error packing UL DCI\n");
  }
  if (srslte_pdcch_encode(&q->pdcch, &q->dl_sf, &dci_msg, q->sf_symbols)) {
    ERROR("Error encoding UL DCI message\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_enb_dl_put_pdsch(srslte_enb_dl_t* q, srslte_pdsch_cfg_t* pdsch, uint8_t* data[SRSLTE_MAX_CODEWORDS])
{
  return srslte_pdsch_encode(&q->pdsch, &q->dl_sf, pdsch, data, q->sf_symbols);
}

int srslte_enb_dl_put_pmch(srslte_enb_dl_t* q, srslte_pmch_cfg_t* pmch_cfg, uint8_t* data)
{
  return srslte_pmch_encode(&q->pmch, &q->dl_sf, pmch_cfg, data, q->sf_symbols);
}

void srslte_enb_dl_gen_signal(srslte_enb_dl_t* q)
{
  // TODO: PAPR control
  float norm_factor = 0.05f / sqrt(q->cell.nof_prb);

  if (q->dl_sf.sf_type == SRSLTE_SF_MBSFN) {
    srslte_ofdm_tx_sf(&q->ifft_mbsfn);
    srslte_vec_sc_prod_cfc(
        q->ifft_mbsfn.out_buffer, norm_factor, q->ifft_mbsfn.out_buffer, (uint32_t)SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
  } else {
    for (int i = 0; i < q->cell.nof_ports; i++) {
      srslte_ofdm_tx_sf(&q->ifft[i]);
      srslte_vec_sc_prod_cfc(
          q->ifft[i].out_buffer, norm_factor, q->ifft[i].out_buffer, (uint32_t)SRSLTE_SF_LEN_PRB(q->cell.nof_prb));
    }
  }
}

bool srslte_enb_dl_gen_cqi_periodic(
    srslte_cell_t* cell, srslte_dl_cfg_t* dl_cfg, uint32_t tti, uint32_t ri, srslte_cqi_cfg_t* cqi_cfg)
{
  bool cqi_enabled = false;
  if (srslte_cqi_periodic_ri_send(&dl_cfg->cqi_report, tti, cell->frame_type)) {
    cqi_cfg->ri_len = 1; /* Asumes only 1 bit for RI */
    cqi_enabled     = true;
  } else if (srslte_cqi_periodic_send(&dl_cfg->cqi_report, tti, cell->frame_type)) {
    cqi_cfg->type = SRSLTE_CQI_TYPE_WIDEBAND;
    if (dl_cfg->tm == SRSLTE_TM4) {
      cqi_cfg->pmi_present     = true;
      cqi_cfg->rank_is_not_one = ri > 0;
    }
    cqi_enabled          = true;
    cqi_cfg->data_enable = cqi_enabled;
  }
  return cqi_enabled;
}

bool srslte_enb_dl_gen_cqi_aperiodic(srslte_cell_t*    cell,
                                     srslte_dl_cfg_t*  dl_cfg,
                                     uint32_t          ri,
                                     srslte_cqi_cfg_t* cqi_cfg)
{
  bool                     cqi_enabled    = false;
  srslte_cqi_report_cfg_t* cqi_report_cfg = &dl_cfg->cqi_report;

  cqi_cfg->type = SRSLTE_CQI_TYPE_SUBBAND_HL;
  if (dl_cfg->tm == SRSLTE_TM3 || dl_cfg->tm == SRSLTE_TM4) {
    cqi_cfg->ri_present = true;
  }
  cqi_cfg->N                  = (cell->nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(cell->nof_prb) : 0;
  cqi_cfg->four_antenna_ports = (cell->nof_ports == 4);
  cqi_cfg->pmi_present        = (cqi_report_cfg->pmi_idx != 0);
  cqi_cfg->rank_is_not_one    = ri > 0;
  cqi_cfg->data_enable        = true;

  return cqi_enabled;
}

void srslte_enb_dl_save_signal(srslte_enb_dl_t* q)
{
  char tmpstr[64];

  uint32_t tti = q->dl_sf.tti;

  snprintf(tmpstr, 64, "sf_symbols_%d", tti);
  srslte_vec_save_file(tmpstr, q->sf_symbols[0], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));

  /*
  int cb_len = q->pdsch_cfg.cb_segm[0].K1;
  for (int i=0;i<q->pdsch_cfg.cb_segm[0].C;i++) {
    snprintf(tmpstr,64,"output/rmout_%d_%d",i,tti);
    srslte_bit_unpack_vector(softbuffer->buffer_b[i], q->tmp, (3*cb_len+12));
    srslte_vec_save_file(tmpstr, q->tmp, (3*cb_len+12)*sizeof(uint8_t));
  }*/

  // printf("Saved files for tti=%d, sf=%d, cfi=%d, mcs=%d, tbs=%d, rv=%d, rnti=0x%x\n", tti, tti%10, cfi,
  //       q->dci.mcs[0].idx, q->dci.mcs[0].tbs, rv_idx, rnti);
}
