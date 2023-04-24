/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/phy/enb/enb_dl.h"

#include "srsran/srsran.h"
#include <complex.h>
#include <math.h>
#include <string.h>

#define CURRENT_FFTSIZE srsran_symbol_sz(q->cell.nof_prb)
#define CURRENT_SFLEN_RE SRSRAN_NOF_RE(q->cell)

static float enb_dl_get_norm_factor(uint32_t nof_prb)
{
  return 0.05f / sqrtf(nof_prb);
}

int srsran_enb_dl_init(srsran_enb_dl_t* q, cf_t* out_buffer[SRSRAN_MAX_PORTS], uint32_t max_prb)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;

    bzero(q, sizeof(srsran_enb_dl_t));

    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->sf_symbols[i] = srsran_vec_cf_malloc(SRSRAN_SF_LEN_RE(max_prb, SRSRAN_CP_NORM));
      if (!q->sf_symbols[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }
    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->out_buffer[i] = out_buffer[i];
    }

    srsran_ofdm_cfg_t ofdm_cfg = {};
    ofdm_cfg.nof_prb           = max_prb;
    ofdm_cfg.cp                = SRSRAN_CP_EXT;
    ofdm_cfg.normalize         = false;
    ofdm_cfg.in_buffer         = q->sf_symbols[0];
    ofdm_cfg.out_buffer        = out_buffer[0];
    ofdm_cfg.sf_type           = SRSRAN_SF_MBSFN;
    if (srsran_ofdm_tx_init_cfg(&q->ifft_mbsfn, &ofdm_cfg)) {
      ERROR("Error initiating FFT");
      goto clean_exit;
    }

    if (srsran_pbch_init(&q->pbch)) {
      ERROR("Error creating PBCH object");
      goto clean_exit;
    }
    if (srsran_pcfich_init(&q->pcfich, 0)) {
      ERROR("Error creating PCFICH object");
      goto clean_exit;
    }
    if (srsran_phich_init(&q->phich, 0)) {
      ERROR("Error creating PHICH object");
      goto clean_exit;
    }
    int mbsfn_area_id = 1;

    if (srsran_pmch_init(&q->pmch, max_prb, 1)) {
      ERROR("Error creating PMCH object");
    }
    srsran_pmch_set_area_id(&q->pmch, mbsfn_area_id);

    if (srsran_pdcch_init_enb(&q->pdcch, max_prb)) {
      ERROR("Error creating PDCCH object");
      goto clean_exit;
    }

    if (srsran_pdsch_init_enb(&q->pdsch, max_prb)) {
      ERROR("Error creating PDSCH object");
      goto clean_exit;
    }

    if (srsran_refsignal_cs_init(&q->csr_signal, max_prb)) {
      ERROR("Error initializing CSR signal (%d)", ret);
      goto clean_exit;
    }

    if (srsran_refsignal_mbsfn_init(&q->mbsfnr_signal, max_prb)) {
      ERROR("Error initializing CSR signal (%d)", ret);
      goto clean_exit;
    }
    ret = SRSRAN_SUCCESS;

  } else {
    ERROR("Invalid parameters");
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_enb_dl_free(q);
  }
  return ret;
}

void srsran_enb_dl_free(srsran_enb_dl_t* q)
{
  if (q) {
    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      srsran_ofdm_tx_free(&q->ifft[i]);
    }
    srsran_ofdm_tx_free(&q->ifft_mbsfn);
    srsran_regs_free(&q->regs);
    srsran_pbch_free(&q->pbch);
    srsran_pcfich_free(&q->pcfich);
    srsran_phich_free(&q->phich);
    srsran_pdcch_free(&q->pdcch);
    srsran_pdsch_free(&q->pdsch);
    srsran_pmch_free(&q->pmch);
    srsran_refsignal_free(&q->csr_signal);
    srsran_refsignal_free(&q->mbsfnr_signal);
    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      if (q->sf_symbols[i]) {
        free(q->sf_symbols[i]);
      }
    }
    bzero(q, sizeof(srsran_enb_dl_t));
  }
}

int srsran_enb_dl_set_cell(srsran_enb_dl_t* q, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && srsran_cell_isvalid(&cell)) {
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      if (q->cell.nof_prb != 0) {
        srsran_regs_free(&q->regs);
      }
      q->cell                    = cell;
      srsran_ofdm_cfg_t ofdm_cfg = {};
      ofdm_cfg.nof_prb           = q->cell.nof_prb;
      ofdm_cfg.cp                = cell.cp;
      ofdm_cfg.normalize         = false;
      for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
        ofdm_cfg.in_buffer  = q->sf_symbols[i];
        ofdm_cfg.out_buffer = q->out_buffer[i];
        ofdm_cfg.sf_type    = SRSRAN_SF_NORM;
        ofdm_cfg.cfr_tx_cfg = q->cfr_config;
        if (srsran_ofdm_tx_init_cfg(&q->ifft[i], &ofdm_cfg)) {
          ERROR("Error initiating FFT (%d)", i);
          return SRSRAN_ERROR;
        }
      }
      if (srsran_regs_init(&q->regs, q->cell)) {
        ERROR("Error resizing REGs");
        return SRSRAN_ERROR;
      }
      for (int i = 0; i < q->cell.nof_ports; i++) {
        if (srsran_ofdm_tx_set_prb(&q->ifft[i], q->cell.cp, q->cell.nof_prb)) {
          ERROR("Error re-planning iFFT (%d)", i);
          return SRSRAN_ERROR;
        }
      }

      if (srsran_ofdm_tx_set_prb(&q->ifft_mbsfn, SRSRAN_CP_EXT, q->cell.nof_prb)) {
        ERROR("Error re-planning ifft_mbsfn");
        return SRSRAN_ERROR;
      }

      srsran_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, 2);

      if (srsran_pbch_set_cell(&q->pbch, q->cell)) {
        ERROR("Error creating PBCH object");
        return SRSRAN_ERROR;
      }
      if (srsran_pcfich_set_cell(&q->pcfich, &q->regs, q->cell)) {
        ERROR("Error creating PCFICH object");
        return SRSRAN_ERROR;
      }
      if (srsran_phich_set_cell(&q->phich, &q->regs, q->cell)) {
        ERROR("Error creating PHICH object");
        return SRSRAN_ERROR;
      }

      if (srsran_pdcch_set_cell(&q->pdcch, &q->regs, q->cell)) {
        ERROR("Error creating PDCCH object");
        return SRSRAN_ERROR;
      }

      if (srsran_pdsch_set_cell(&q->pdsch, q->cell)) {
        ERROR("Error creating PDSCH object");
        return SRSRAN_ERROR;
      }

      if (srsran_pmch_set_cell(&q->pmch, q->cell)) {
        ERROR("Error creating PMCH object");
        return SRSRAN_ERROR;
      }

      if (srsran_refsignal_cs_set_cell(&q->csr_signal, q->cell)) {
        ERROR("Error initializing CSR signal (%d)", ret);
        return SRSRAN_ERROR;
      }
      int mbsfn_area_id = 1;
      if (srsran_refsignal_mbsfn_set_cell(&q->mbsfnr_signal, q->cell, mbsfn_area_id)) {
        ERROR("Error initializing MBSFNR signal (%d)", ret);
        return SRSRAN_ERROR;
      }
      /* Generate PSS/SSS signals */
      srsran_pss_generate(q->pss_signal, cell.id % 3);
      srsran_sss_generate(q->sss_signal0, q->sss_signal5, cell.id);

      // Calculate common DCI locations
      for (int32_t cfi = 1; cfi <= 3; cfi++) {
        q->nof_common_locations[SRSRAN_CFI_IDX(cfi)] = srsran_pdcch_common_locations(
            &q->pdcch, q->common_locations[SRSRAN_CFI_IDX(cfi)], SRSRAN_MAX_CANDIDATES_COM, cfi);
      }
    }
    ret = SRSRAN_SUCCESS;

  } else {
    ERROR("Invalid cell properties: Id=%d, Ports=%d, PRBs=%d", cell.id, cell.nof_ports, cell.nof_prb);
  }
  return ret;
}

int srsran_enb_dl_set_cfr(srsran_enb_dl_t* q, const srsran_cfr_cfg_t* cfr)
{
  if (q == NULL || cfr == NULL) {
    ERROR("Error, invalid inputs");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy the cfr config into the eNB
  q->cfr_config = *cfr;

  // Set the cfr for the ifft's
  if (srsran_ofdm_set_cfr(&q->ifft_mbsfn, &q->cfr_config) < SRSRAN_SUCCESS) {
    ERROR("Error setting the CFR for ifft_mbsfn");
    return SRSRAN_ERROR;
  }
  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (srsran_ofdm_set_cfr(&q->ifft[i], &q->cfr_config) < SRSRAN_SUCCESS) {
      ERROR("Error setting the CFR for the IFFT (%d)", i);
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

#ifdef resolve
void srsran_enb_dl_apply_power_allocation(srsran_enb_dl_t* q)
{
  uint32_t nof_symbols_slot = SRSRAN_CP_NSYMB(q->cell.cp);
  uint32_t nof_re_symbol    = SRSRAN_NRE * q->cell.nof_prb;

  if (q->rho_b != 0.0f && q->rho_b != 1.0f) {
    float scaling = q->rho_b;
    for (uint32_t i = 0; i < q->cell.nof_ports; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        cf_t* ptr;
        ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 0);
        srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        if (q->cell.cp == SRSRAN_CP_NORM) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 4);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        } else {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 3);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
        if (q->cell.nof_ports == 4) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 1);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
      }
    }
  }
}

void srsran_enb_dl_prepare_power_allocation(srsran_enb_dl_t* q)
{
  uint32_t nof_symbols_slot = SRSRAN_CP_NSYMB(q->cell.cp);
  uint32_t nof_re_symbol    = SRSRAN_NRE * q->cell.nof_prb;

  if (q->rho_b != 0.0f && q->rho_b != 1.0f) {
    float scaling = 1.0f / q->rho_b;
    for (uint32_t i = 0; i < q->cell.nof_ports; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        cf_t* ptr;
        ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 0);
        srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        if (q->cell.cp == SRSRAN_CP_NORM) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 4);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        } else {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 3);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
        if (q->cell.nof_ports == 4) {
          ptr = q->sf_symbols[i] + nof_re_symbol * (j * nof_symbols_slot + 1);
          srsran_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
      }
    }
  }
}

#endif

static void clear_sf(srsran_enb_dl_t* q)
{
  for (int i = 0; i < q->cell.nof_ports; i++) {
    srsran_vec_cf_zero(q->sf_symbols[i], CURRENT_SFLEN_RE);
  }
}

static void put_sync(srsran_enb_dl_t* q)
{
  uint32_t sf_idx = q->dl_sf.tti % 10;

  if (sf_idx == 0 || sf_idx == 5) {
    for (int p = 0; p < q->cell.nof_ports; p++) {
      srsran_pss_put_slot(q->pss_signal, q->sf_symbols[p], q->cell.nof_prb, q->cell.cp);
      srsran_sss_put_slot(sf_idx ? q->sss_signal5 : q->sss_signal0, q->sf_symbols[p], q->cell.nof_prb, q->cell.cp);
    }
  }
}

static void put_refs(srsran_enb_dl_t* q)
{
  uint32_t sf_idx = q->dl_sf.tti % 10;
  if (q->dl_sf.sf_type == SRSRAN_SF_MBSFN) {
    srsran_refsignal_mbsfn_put_sf(
        q->cell, 0, q->csr_signal.pilots[0][sf_idx], q->mbsfnr_signal.pilots[0][sf_idx], q->sf_symbols[0]);
  } else {
    for (int p = 0; p < q->cell.nof_ports; p++) {
      srsran_refsignal_cs_put_sf(&q->csr_signal, &q->dl_sf, (uint32_t)p, q->sf_symbols[p]);
    }
  }
}

static void put_mib(srsran_enb_dl_t* q)
{
  uint8_t bch_payload[SRSRAN_BCH_PAYLOAD_LEN];

  uint32_t sf_idx = q->dl_sf.tti % 10;
  uint32_t sfn    = q->dl_sf.tti / 10;

  if (sf_idx == 0) {
    srsran_pbch_mib_pack(&q->cell, sfn, bch_payload);
    srsran_pbch_encode(&q->pbch, bch_payload, q->sf_symbols, sfn % 4);
  }
}

static void put_pcfich(srsran_enb_dl_t* q)
{
  srsran_pcfich_encode(&q->pcfich, &q->dl_sf, q->sf_symbols);
}

void srsran_enb_dl_put_base(srsran_enb_dl_t* q, srsran_dl_sf_cfg_t* dl_sf)
{
  srsran_ofdm_set_non_mbsfn_region(&q->ifft_mbsfn, dl_sf->non_mbsfn_region);
  q->dl_sf = *dl_sf;
  clear_sf(q);
  put_sync(q);
  put_refs(q);
  put_mib(q);
  put_pcfich(q);
}

void srsran_enb_dl_put_phich(srsran_enb_dl_t* q, srsran_phich_grant_t* grant, bool ack)
{
  srsran_phich_resource_t resource;
  srsran_phich_calc(&q->phich, grant, &resource);
  srsran_phich_encode(&q->phich, &q->dl_sf, resource, ack, q->sf_symbols);
}

bool srsran_enb_dl_location_is_common_ncce(srsran_enb_dl_t* q, const srsran_dci_location_t* loc)
{
  if (SRSRAN_CFI_ISVALID(q->dl_sf.cfi)) {
    return srsran_location_find_location(
        q->common_locations[SRSRAN_CFI_IDX(q->dl_sf.cfi)], q->nof_common_locations[SRSRAN_CFI_IDX(q->dl_sf.cfi)], loc);
  } else {
    return false;
  }
}

int srsran_enb_dl_put_pdcch_dl(srsran_enb_dl_t* q, srsran_dci_cfg_t* dci_cfg, srsran_dci_dl_t* dci_dl)
{
  srsran_dci_msg_t dci_msg;
  ZERO_OBJECT(dci_msg);

  if (srsran_dci_msg_pack_pdsch(&q->cell, &q->dl_sf, dci_cfg, dci_dl, &dci_msg)) {
    ERROR("Error packing DL DCI");
  }
  if (srsran_pdcch_encode(&q->pdcch, &q->dl_sf, &dci_msg, q->sf_symbols)) {
    ERROR("Error encoding DL DCI message");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_enb_dl_put_pdcch_ul(srsran_enb_dl_t* q, srsran_dci_cfg_t* dci_cfg, srsran_dci_ul_t* dci_ul)
{
  srsran_dci_msg_t dci_msg;
  ZERO_OBJECT(dci_msg);

  if (srsran_dci_msg_pack_pusch(&q->cell, &q->dl_sf, dci_cfg, dci_ul, &dci_msg)) {
    ERROR("Error packing UL DCI");
  }
  if (srsran_pdcch_encode(&q->pdcch, &q->dl_sf, &dci_msg, q->sf_symbols)) {
    ERROR("Error encoding UL DCI message");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_enb_dl_put_pdsch(srsran_enb_dl_t* q, srsran_pdsch_cfg_t* pdsch, uint8_t* data[SRSRAN_MAX_CODEWORDS])
{
  return srsran_pdsch_encode(&q->pdsch, &q->dl_sf, pdsch, data, q->sf_symbols);
}

int srsran_enb_dl_put_pmch(srsran_enb_dl_t* q, srsran_pmch_cfg_t* pmch_cfg, uint8_t* data)
{
  return srsran_pmch_encode(&q->pmch, &q->dl_sf, pmch_cfg, data, q->sf_symbols);
}

void srsran_enb_dl_gen_signal(srsran_enb_dl_t* q)
{
  float norm_factor = enb_dl_get_norm_factor(q->cell.nof_prb);

  // First apply the amplitude normalization, then perform the IFFT and optional CFR reduction
  if (q->dl_sf.sf_type == SRSRAN_SF_MBSFN) {
    srsran_vec_sc_prod_cfc(q->ifft_mbsfn.cfg.in_buffer,
                           norm_factor,
                           q->ifft_mbsfn.cfg.in_buffer,
                           SRSRAN_NOF_SLOTS_PER_SF * q->cell.nof_prb * SRSRAN_NRE * SRSRAN_CP_NSYMB(q->cell.cp));
    srsran_ofdm_tx_sf(&q->ifft_mbsfn);
  } else {
    for (int i = 0; i < q->cell.nof_ports; i++) {
      srsran_vec_sc_prod_cfc(q->ifft[i].cfg.in_buffer,
                             norm_factor,
                             q->ifft[i].cfg.in_buffer,
                             SRSRAN_NOF_SLOTS_PER_SF * q->cell.nof_prb * SRSRAN_NRE * SRSRAN_CP_NSYMB(q->cell.cp));
      srsran_ofdm_tx_sf(&q->ifft[i]);
    }
  }
}

bool srsran_enb_dl_gen_cqi_periodic(const srsran_cell_t*   cell,
                                    const srsran_dl_cfg_t* dl_cfg,
                                    uint32_t               tti,
                                    uint32_t               last_ri,
                                    srsran_cqi_cfg_t*      cqi_cfg)
{
  bool cqi_enabled = false;
  if (srsran_cqi_periodic_ri_send(&dl_cfg->cqi_report, tti, cell->frame_type)) {
    cqi_cfg->ri_len = srsran_ri_nof_bits(cell);
    cqi_enabled     = true;
  } else if (srsran_cqi_periodic_send(&dl_cfg->cqi_report, tti, cell->frame_type)) {
    if (dl_cfg->cqi_report.format_is_subband &&
        srsran_cqi_periodic_is_subband(&dl_cfg->cqi_report, tti, cell->nof_prb, cell->frame_type)) {
      // 36.213 table 7.2.2-1, periodic CQI supports UE-selected only
      cqi_cfg->type                 = SRSRAN_CQI_TYPE_SUBBAND_UE;
      cqi_cfg->L                    = srsran_cqi_hl_get_L(cell->nof_prb);
      cqi_cfg->subband_label_2_bits = cqi_cfg->L > 1;
    } else {
      cqi_cfg->type = SRSRAN_CQI_TYPE_WIDEBAND;
    }
    if (dl_cfg->tm == SRSRAN_TM4) {
      cqi_cfg->pmi_present     = true;
      cqi_cfg->rank_is_not_one = last_ri > 0;
    }
    cqi_enabled          = true;
    cqi_cfg->data_enable = cqi_enabled;
  }
  return cqi_enabled;
}

bool srsran_enb_dl_gen_cqi_aperiodic(const srsran_cell_t*   cell,
                                     const srsran_dl_cfg_t* dl_cfg,
                                     uint32_t               ri,
                                     srsran_cqi_cfg_t*      cqi_cfg)
{
  bool                           cqi_enabled    = false;
  const srsran_cqi_report_cfg_t* cqi_report_cfg = &dl_cfg->cqi_report;

  cqi_cfg->type = SRSRAN_CQI_TYPE_SUBBAND_HL;
  if (dl_cfg->tm == SRSRAN_TM3 || dl_cfg->tm == SRSRAN_TM4) {
    cqi_cfg->ri_len = srsran_ri_nof_bits(cell);
  }
  cqi_cfg->N                  = (cell->nof_prb > 7) ? srsran_cqi_hl_get_no_subbands(cell->nof_prb) : 0;
  cqi_cfg->four_antenna_ports = (cell->nof_ports == 4);
  cqi_cfg->pmi_present        = (cqi_report_cfg->pmi_idx != 0);
  cqi_cfg->rank_is_not_one    = ri > 0;
  cqi_cfg->data_enable        = true;

  return cqi_enabled;
}

void srsran_enb_dl_save_signal(srsran_enb_dl_t* q)
{
  char tmpstr[64];

  uint32_t tti = q->dl_sf.tti;

  snprintf(tmpstr, 64, "sf_symbols_%d", tti);
  srsran_vec_save_file(tmpstr, q->sf_symbols[0], SRSRAN_NOF_RE(q->cell) * sizeof(cf_t));

  /*
  int cb_len = q->pdsch_cfg.cb_segm[0].K1;
  for (int i=0;i<q->pdsch_cfg.cb_segm[0].C;i++) {
    snprintf(tmpstr,64,"output/rmout_%d_%d",i,tti);
    srsran_bit_unpack_vector(softbuffer->buffer_b[i], q->tmp, (3*cb_len+12));
    srsran_vec_save_file(tmpstr, q->tmp, (3*cb_len+12)*sizeof(uint8_t));
  }*/

  // printf("Saved files for tti=%d, sf=%d, cfi=%d, mcs=%d, tbs=%d, rv=%d, rnti=0x%x\n", tti, tti%10, cfi,
  //       q->dci.mcs[0].idx, q->dci.mcs[0].tbs, rv_idx, rnti);
}

void srsran_enb_dl_gen_ack(const srsran_cell_t*      cell,
                           const srsran_dl_sf_cfg_t* sf,
                           const srsran_pdsch_ack_t* ack_info,
                           srsran_uci_cfg_t*         uci_cfg)
{
  srsran_uci_data_t uci_data = {};

  // Copy UCI configuration
  uci_data.cfg = *uci_cfg;

  srsran_ue_dl_gen_ack(cell, sf, ack_info, &uci_data);

  // Copy back the result of uci configuration
  *uci_cfg = uci_data.cfg;
}

static void enb_dl_get_ack_fdd_all_spatial_bundling(const srsran_uci_value_t* uci_value,
                                                    srsran_pdsch_ack_t*       pdsch_ack,
                                                    uint32_t                  nof_tb)
{
  for (uint32_t cc_idx = 0; cc_idx < pdsch_ack->nof_cc; cc_idx++) {
    if (pdsch_ack->cc[cc_idx].m[0].present) {
      for (uint32_t tb = 0; tb < nof_tb; tb++) {
        // Check that TB was transmitted
        if (pdsch_ack->cc[cc_idx].m[0].value[tb] != 2) {
          pdsch_ack->cc[cc_idx].m[0].value[tb] = uci_value->ack.ack_value[cc_idx];
        }
      }
    }
  }
}

static void
enb_dl_get_ack_fdd_pcell_skip_drx(const srsran_uci_value_t* uci_value, srsran_pdsch_ack_t* pdsch_ack, uint32_t nof_tb)
{
  uint32_t ack_idx = 0;
  if (pdsch_ack->cc[0].m[0].present) {
    for (uint32_t tb = 0; tb < nof_tb; tb++) {
      // Check that TB was transmitted
      if (pdsch_ack->cc[0].m[0].value[tb] != 2) {
        if (uci_value->ack.valid) {
          pdsch_ack->cc[0].m[0].value[tb] = uci_value->ack.ack_value[ack_idx++];
        } else {
          pdsch_ack->cc[0].m[0].value[tb] = 0;
        }
      }
    }
  }
}

static void
enb_dl_get_ack_fdd_all_keep_drx(const srsran_uci_value_t* uci_value, srsran_pdsch_ack_t* pdsch_ack, uint32_t nof_tb)
{
  for (uint32_t cc_idx = 0; cc_idx < pdsch_ack->nof_cc; cc_idx++) {
    if (pdsch_ack->cc[cc_idx].m[0].present) {
      for (uint32_t tb = 0; tb < nof_tb; tb++) {
        // Check that TB was transmitted
        if (pdsch_ack->cc[cc_idx].m[0].value[tb] != 2) {
          if (uci_value->ack.valid) {
            pdsch_ack->cc[cc_idx].m[0].value[tb] = uci_value->ack.ack_value[cc_idx * nof_tb + tb];
          } else {
            pdsch_ack->cc[cc_idx].m[0].value[tb] = 0;
          }
        }
      }
    }
  }
}

static void
get_ack_fdd(const srsran_uci_cfg_t* uci_cfg, const srsran_uci_value_t* uci_value, srsran_pdsch_ack_t* pdsch_ack)
{
  // Number of transport blocks for the current Transmission Mode
  uint32_t nof_tb = 1;
  if (pdsch_ack->transmission_mode > SRSRAN_TM2) {
    nof_tb = SRSRAN_MAX_CODEWORDS;
  }

  // Count number of transmissions
  uint32_t tb_count     = 0; // All transmissions
  uint32_t tb_count_cc0 = 0; // Transmissions on PCell
  for (uint32_t cc_idx = 0; cc_idx < pdsch_ack->nof_cc; cc_idx++) {
    for (uint32_t tb = 0; tb < nof_tb; tb++) {
      if (pdsch_ack->cc[cc_idx].m[0].present && pdsch_ack->cc[cc_idx].m[0].value[tb] != 2) {
        tb_count++;
      }

      // Save primary cell number of TB
      if (cc_idx == 0) {
        tb_count_cc0 = tb_count;
      }
    }
  }

  // Does CSI report need to be transmitted?
  bool csi_report = uci_cfg->cqi.data_enable || uci_cfg->cqi.ri_len;

  switch (pdsch_ack->ack_nack_feedback_mode) {
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL:
      // Get ACK from PCell only, skipping DRX
      enb_dl_get_ack_fdd_pcell_skip_drx(uci_value, pdsch_ack, nof_tb);
      break;
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_CS:
      if (pdsch_ack->nof_cc == 1) {
        enb_dl_get_ack_fdd_pcell_skip_drx(uci_value, pdsch_ack, nof_tb);
      } else if (pdsch_ack->is_pusch_available) {
        enb_dl_get_ack_fdd_all_keep_drx(uci_value, pdsch_ack, nof_tb);
      } else if (uci_value->scheduling_request) {
        // For FDD with PUCCH format 1b with channel selection, when both HARQ-ACK and SR are transmitted in the same
        // sub-frame a UE shall transmit the HARQ-ACK on its assigned HARQ-ACK PUCCH resource with channel selection as
        // defined in subclause 10.1.2.2.1 for a negative SR transmission and transmit one HARQ-ACK bit per serving cell
        // on its assigned SR PUCCH resource for a positive SR transmission according to the following:
        // − if only one transport block or a PDCCH indicating downlink SPS release is detected on a serving cell, the
        //   HARQ-ACK bit for the serving cell is the HARQ-ACK bit corresponding to the transport block or the PDCCH
        //   indicating downlink SPS release;
        // − if two transport blocks are received on a serving cell, the HARQ-ACK bit for the serving cell is generated
        //   by spatially bundling the HARQ-ACK bits corresponding to the transport blocks;
        // − if neither PDSCH transmission for which HARQ-ACK response shall be provided nor PDCCH indicating
        //   downlink SPS release is detected for a serving cell, the HARQ-ACK bit for the serving cell is set to NACK;
        enb_dl_get_ack_fdd_all_spatial_bundling(uci_value, pdsch_ack, nof_tb);
      } else if (csi_report) {
        enb_dl_get_ack_fdd_pcell_skip_drx(uci_value, pdsch_ack, nof_tb);
      } else {
        enb_dl_get_ack_fdd_all_keep_drx(uci_value, pdsch_ack, nof_tb);
      }
      break;
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3:
      if (tb_count == tb_count_cc0) {
        enb_dl_get_ack_fdd_pcell_skip_drx(uci_value, pdsch_ack, nof_tb);
      } else {
        enb_dl_get_ack_fdd_all_keep_drx(uci_value, pdsch_ack, nof_tb);
      }
      break;
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_ERROR:
    default:; // Do nothing
      break;
  }
}

void srsran_enb_dl_get_ack(const srsran_cell_t*      cell,
                           const srsran_uci_cfg_t*   uci_cfg,
                           const srsran_uci_value_t* uci_value,
                           srsran_pdsch_ack_t*       pdsch_ack)
{
  if (cell->frame_type == SRSRAN_FDD) {
    get_ack_fdd(uci_cfg, uci_value, pdsch_ack);
  } else {
    ERROR("Not implemented for TDD");
  }
}

float srsran_enb_dl_get_maximum_signal_power_dBfs(uint32_t nof_prb)
{
  return srsran_convert_amplitude_to_dB(enb_dl_get_norm_factor(nof_prb)) +
         srsran_convert_power_to_dB((float)nof_prb * SRSRAN_NRE) + 3.0f;
}
