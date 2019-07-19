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

#include "srslte/srslte.h"
#include "srsue/hdr/phy/cc_worker.h"
#include "srslte/interfaces/ue_interfaces.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

#define CURRENT_TTI (sf_cfg_dl.tti)
#define CURRENT_SFIDX (sf_cfg_dl.tti % 10)
#define CURRENT_TTI_TX (sf_cfg_ul.tti)

using namespace asn1::rrc;

namespace srsue {

/************
 *
 * Common Functions
 *
 */

cc_worker::cc_worker(uint32_t cc_idx, uint32_t max_prb, srsue::phy_common* phy, srslte::log* log_h)
{
  ZERO_OBJECT(signal_buffer_rx);
  ZERO_OBJECT(signal_buffer_tx);
  ZERO_OBJECT(pending_dl_grant);
  ZERO_OBJECT(cell);
  ZERO_OBJECT(sf_cfg_dl);
  ZERO_OBJECT(sf_cfg_ul);
  ZERO_OBJECT(ue_dl);
  ZERO_OBJECT(ue_dl_cfg);
  ZERO_OBJECT(ue_dl_cfg.cfg.pdsch);
  ZERO_OBJECT(pmch_cfg);
  ZERO_OBJECT(chest_mbsfn_cfg);
  ZERO_OBJECT(chest_default_cfg);
  ZERO_OBJECT(ue_ul);
  ZERO_OBJECT(ue_ul_cfg);
  ZERO_OBJECT(dl_metrics);
  ZERO_OBJECT(ul_metrics);

  cell_initiated = false;

  this->cc_idx = cc_idx;
  this->phy    = phy;
  this->log_h  = log_h;

  for (uint32_t i = 0; i < phy->args->nof_rx_ant; i++) {
    signal_buffer_rx[i] = (cf_t*)srslte_vec_malloc(3 * sizeof(cf_t) * SRSLTE_SF_LEN_PRB(max_prb));
    if (!signal_buffer_rx[i]) {
      Error("Allocating memory\n");
      return;
    }
    signal_buffer_tx[i] = (cf_t*)srslte_vec_malloc(3 * sizeof(cf_t) * SRSLTE_SF_LEN_PRB(max_prb));
    if (!signal_buffer_tx[i]) {
      Error("Allocating memory\n");
      return;
    }
  }

  if (srslte_ue_dl_init(&ue_dl, signal_buffer_rx, max_prb, phy->args->nof_rx_ant)) {
    Error("Initiating UE DL\n");
    return;
  }

  if (srslte_ue_ul_init(&ue_ul, signal_buffer_tx[0], max_prb)) {
    Error("Initiating UE UL\n");
    return;
  }

  phy->set_ue_dl_cfg(&ue_dl_cfg);
  phy->set_ue_ul_cfg(&ue_ul_cfg);
  phy->set_pdsch_cfg(&ue_dl_cfg.cfg.pdsch);
  phy->set_pdsch_cfg(&pmch_cfg.pdsch_cfg); // set same config in PMCH decoder

  // Define MBSFN subframes channel estimation and save default one
  chest_mbsfn_cfg.filter_type          = SRSLTE_CHEST_FILTER_TRIANGLE;
  chest_mbsfn_cfg.filter_coef[0]       = 0.1;
  chest_mbsfn_cfg.interpolate_subframe = true;
  chest_mbsfn_cfg.noise_alg            = SRSLTE_NOISE_ALG_PSS;

  chest_default_cfg = ue_dl_cfg.chest_cfg;

  if (phy->args->pdsch_8bit_decoder) {
    ue_dl.pdsch.llr_is_8bit        = true;
    ue_dl.pdsch.dl_sch.llr_is_8bit = true;
  }
  pregen_enabled = false;
}

cc_worker::~cc_worker()
{
  for (uint32_t i = 0; i < phy->args->nof_rx_ant; i++) {
    if (signal_buffer_tx[i]) {
      free(signal_buffer_tx[i]);
    }
    if (signal_buffer_rx[i]) {
      free(signal_buffer_rx[i]);
    }
  }
  srslte_ue_dl_free(&ue_dl);
  srslte_ue_ul_free(&ue_ul);
}

void cc_worker::reset()
{
  bzero(&dl_metrics, sizeof(dl_metrics_t));
  bzero(&ul_metrics, sizeof(ul_metrics_t));

  phy_interface_rrc_lte::phy_cfg_t empty_cfg = {};
  // defaults
  empty_cfg.common.pucch_cnfg.delta_pucch_shift.value = pucch_cfg_common_s::delta_pucch_shift_opts::ds1;
  empty_cfg.common.ul_pwr_ctrl.alpha.value            = alpha_r12_opts::al0;
  empty_cfg.common.ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format1.value =
      delta_flist_pucch_s::delta_f_pucch_format1_opts::delta_f0;
  empty_cfg.common.ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format1b.value =
      delta_flist_pucch_s::delta_f_pucch_format1b_opts::delta_f1;
  empty_cfg.common.ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2.value =
      delta_flist_pucch_s::delta_f_pucch_format2_opts::delta_f0;
  empty_cfg.common.ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2a.value =
      delta_flist_pucch_s::delta_f_pucch_format2a_opts::delta_f0;
  empty_cfg.common.ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2b.value =
      delta_flist_pucch_s::delta_f_pucch_format2b_opts::delta_f0;
  set_pcell_config(&empty_cfg);
}

bool cc_worker::set_cell(srslte_cell_t cell)
{
  if (this->cell.id != cell.id || !cell_initiated) {
    this->cell = cell;

    if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
      Error("Initiating UE DL\n");
      return false;
    }

    if (srslte_ue_dl_set_mbsfn_area_id(&ue_dl, 1)) {
      Error("Setting mbsfn id\n");
    }

    if (srslte_ue_ul_set_cell(&ue_ul, cell)) {
      Error("Initiating UE UL\n");
      return false;
    }

    if (cell.frame_type == SRSLTE_TDD && !ue_dl_cfg.chest_cfg.interpolate_subframe) {
      chest_default_cfg.interpolate_subframe = true;
      log_h->console("Enabling subframe interpolation for TDD cells (recommended setting)\n");
    }

    cell_initiated = true;
  }
  return true;
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  return signal_buffer_rx[antenna_idx];
}

cf_t* cc_worker::get_tx_buffer(uint32_t antenna_idx)
{
  return signal_buffer_tx[antenna_idx];
}

void cc_worker::set_tti(uint32_t tti)
{
  sf_cfg_dl.tti       = tti;
  sf_cfg_ul.tti       = TTI_TX(tti);
  sf_cfg_ul.shortened = false;
}

void cc_worker::set_cfo(float cfo)
{
  ue_ul_cfg.cfo_value = cfo;
}

float cc_worker::get_ref_cfo()
{
  return ue_dl.chest_res.cfo;
}

void cc_worker::set_crnti(uint16_t rnti)
{
  srslte_ue_dl_set_rnti(&ue_dl, rnti);
  srslte_ue_ul_set_rnti(&ue_ul, rnti);
}

void cc_worker::set_tdd_config(srslte_tdd_config_t config)
{
  sf_cfg_dl.tdd_config = config;
  sf_cfg_ul.tdd_config = config;
}

void cc_worker::enable_pregen_signals(bool enabled)
{
  this->pregen_enabled = enabled;
}

void cc_worker::fill_dci_cfg(srslte_dci_cfg_t* cfg, bool rel10)
{
  bzero(cfg, sizeof(srslte_dci_cfg_t));
  if (rel10 && phy->cif_enabled) {
    cfg->cif_enabled = phy->cif_enabled;
  }
  cfg->multiple_csi_request_enabled = phy->multiple_csi_request_enabled;
  cfg->srs_request_enabled          = phy->srs_request_enabled;
}

void cc_worker::set_dl_pending_grant(uint32_t cc_idx, srslte_dci_dl_t* dl_dci)
{
  if (!pending_dl_grant[cc_idx].enable) {
    pending_dl_grant[cc_idx].dl_dci = *dl_dci;
    pending_dl_grant[cc_idx].enable = true;
  } else {
    Warning("set_dl_pending_grant: cc=%d already exists\n", cc_idx);
  }
}

bool cc_worker::get_dl_pending_grant(uint32_t cc_idx, srslte_dci_dl_t* dl_dci)
{
  if (pending_dl_grant[cc_idx].enable) {
    *dl_dci                         = pending_dl_grant[cc_idx].dl_dci;
    pending_dl_grant[cc_idx].enable = false;
    return true;
  } else {
    return false;
  }
}

/************
 *
 * Downlink Functions
 *
 */

bool cc_worker::work_dl_regular()
{
  bool dl_ack[SRSLTE_MAX_CODEWORDS];

  mac_interface_phy_lte::tb_action_dl_t dl_action;

  bool found_dl_grant = false;

  sf_cfg_dl.sf_type = SRSLTE_SF_NORM;

  // Set default channel estimation
  ue_dl_cfg.chest_cfg = chest_default_cfg;

  /* For TDD, when searching for SIB1, the ul/dl configuration is unknown and need to do blind search over
   * the possible mi values
   */
  uint32_t mi_set_len;
  if (cell.frame_type == SRSLTE_TDD && !sf_cfg_dl.tdd_config.configured) {
    mi_set_len = 3;
  } else {
    mi_set_len = 1;
  }

  // Blind search PHICH mi value
  for (uint32_t i = 0; i < mi_set_len && !found_dl_grant; i++) {

    if (mi_set_len == 1) {
      srslte_ue_dl_set_mi_auto(&ue_dl);
    } else {
      srslte_ue_dl_set_mi_manual(&ue_dl, i);
    }

    /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, &sf_cfg_dl, &ue_dl_cfg) < 0) {
      Error("Getting PDCCH FFT estimate\n");
      return false;
    }

    /* Look for DL and UL dci(s) if this is PCell, or no cross-carrier scheduling is enabled */
    if ((cc_idx == 0) || (!phy->cif_enabled)) {
      found_dl_grant = decode_pdcch_dl() > 0;
      decode_pdcch_ul();
    }
  }

  srslte_dci_dl_t dci_dl;
  bool            has_dl_grant = get_dl_pending_grant(cc_idx, &dci_dl);

  // If found a dci for this carrier, generate a grant, pass it to MAC and decode the associated PDSCH
  if (has_dl_grant) {

    // Read last TB from last retx for this pid
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      ue_dl_cfg.cfg.pdsch.grant.last_tbs[i] = phy->last_dl_tbs[dci_dl.pid][cc_idx][i];
    }
    // Generate PHY grant
    if (srslte_ue_dl_dci_to_pdsch_grant(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, &dci_dl, &ue_dl_cfg.cfg.pdsch.grant)) {
      Error("Converting DCI message to DL dci\n");
      return -1;
    }

    // Save TB for next retx
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      phy->last_dl_tbs[dci_dl.pid][cc_idx][i] = ue_dl_cfg.cfg.pdsch.grant.last_tbs[i];
    }

    // Set RNTI
    ue_dl_cfg.cfg.pdsch.rnti = dci_dl.rnti;

    // Generate MAC grant
    mac_interface_phy_lte::mac_grant_dl_t mac_grant;
    dl_phy_to_mac_grant(&ue_dl_cfg.cfg.pdsch.grant, &dci_dl, &mac_grant);

    // Save ACK resource configuration
    srslte_pdsch_ack_resource_t ack_resource = {dci_dl.dai, dci_dl.location.ncce};

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    phy->stack->new_grant_dl(cc_idx, mac_grant, &dl_action);
    decode_pdsch(ack_resource, &dl_action, dl_ack);
    phy->stack->tb_decoded(cc_idx, mac_grant, dl_ack);
  }

  /* Decode PHICH */
  decode_phich();

  return true;
}

bool cc_worker::work_dl_mbsfn(srslte_mbsfn_cfg_t mbsfn_cfg)
{
  mac_interface_phy_lte::tb_action_dl_t dl_action;

  // Configure MBSFN settings
  srslte_ue_dl_set_mbsfn_area_id(&ue_dl, mbsfn_cfg.mbsfn_area_id);
  srslte_ue_dl_set_non_mbsfn_region(&ue_dl, mbsfn_cfg.non_mbsfn_region_length);

  sf_cfg_dl.sf_type = SRSLTE_SF_MBSFN;

  // Set MBSFN channel estimation
  chest_mbsfn_cfg.mbsfn_area_id = mbsfn_cfg.mbsfn_area_id;
  ue_dl_cfg.chest_cfg = chest_mbsfn_cfg;

  /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
  if (srslte_ue_dl_decode_fft_estimate(&ue_dl, &sf_cfg_dl, &ue_dl_cfg) < 0) {
    Error("Getting PDCCH FFT estimate\n");
    return false;
  }

  decode_pdcch_ul();

  if (mbsfn_cfg.enable) {
    srslte_configure_pmch(&pmch_cfg, &cell, &mbsfn_cfg);
    srslte_ra_dl_compute_nof_re(&cell, &sf_cfg_dl, &pmch_cfg.pdsch_cfg.grant);

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    phy->stack->new_mch_dl(pmch_cfg.pdsch_cfg.grant, &dl_action);
    bool mch_decoded = true;
    if (!decode_pmch(&dl_action, &mbsfn_cfg)) {
      mch_decoded = false;
    }
    phy->stack->mch_decoded((uint32_t)pmch_cfg.pdsch_cfg.grant.tb[0].tbs / 8, mch_decoded);
  } else if (mbsfn_cfg.is_mcch) {
    // release lock in phy_common
    phy->set_mch_period_stop(0);
  }

  /* Decode PHICH */
  decode_phich();

  return true;
}

void cc_worker::dl_phy_to_mac_grant(srslte_pdsch_grant_t*                         phy_grant,
                                    srslte_dci_dl_t*                              dl_dci,
                                    srsue::mac_interface_phy_lte::mac_grant_dl_t* mac_grant)
{
  /* Fill MAC dci structure */
  mac_grant->pid  = dl_dci->pid;
  mac_grant->rnti = dl_dci->rnti;

  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    mac_grant->tb[i].ndi         = dl_dci->tb[i].ndi;
    mac_grant->tb[i].ndi_present = (dl_dci->tb[i].mcs_idx <= 28);
    mac_grant->tb[i].tbs         = phy_grant->tb[i].enabled ? (phy_grant->tb[i].tbs / (uint32_t)8) : 0;
    mac_grant->tb[i].rv          = phy_grant->tb[i].rv;
  }

  // If SIB dci, use PID to signal TTI to obtain RV from higher layers
  if (mac_grant->rnti == SRSLTE_SIRNTI) {
    mac_grant->pid = CURRENT_TTI;
  }
}

int cc_worker::decode_pdcch_dl()
{
  int nof_grants = 0;

  srslte_dci_dl_t dci[SRSLTE_MAX_CARRIERS];
  ZERO_OBJECT(dci);

  uint16_t dl_rnti = phy->stack->get_dl_sched_rnti(CURRENT_TTI);
  if (dl_rnti) {

    /* Blind search first without cross scheduling then with it if enabled */
    for (int i = 0; i < (phy->cif_enabled ? 2 : 1) && !nof_grants; i++) {
      fill_dci_cfg(&ue_dl_cfg.dci_cfg, i > 0);
      Debug("PDCCH looking for rnti=0x%x\n", dl_rnti);
      nof_grants = srslte_ue_dl_find_dl_dci(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, dl_rnti, dci);
      if (nof_grants < 0) {
        Error("Looking for DL grants\n");
        return -1;
      }
    }

    // If RAR dci, save TTI
    if (nof_grants > 0 && SRSLTE_RNTI_ISRAR(dl_rnti)) {
      phy->set_rar_grant_tti(CURRENT_TTI);
    }

    for (int k = 0; k < nof_grants; k++) {
      // Save dci to CC index
      set_dl_pending_grant(dci[k].cif_present ? dci[k].cif : cc_idx, &dci[k]);

      // Logging
      char str[512];
      srslte_dci_dl_info(&dci[k], str, 512);
      Info("PDCCH: cc=%d, %s, snr=%.1f dB\n", cc_idx, str, ue_dl.chest_res.snr_db);
    }
  }
  return nof_grants;
}

int cc_worker::decode_pdsch(srslte_pdsch_ack_resource_t            ack_resource,
                            mac_interface_phy_lte::tb_action_dl_t* action,
                            bool                                   mac_acks[SRSLTE_MAX_CODEWORDS])
{

  srslte_pdsch_res_t pdsch_dec[SRSLTE_MAX_CODEWORDS];
  ZERO_OBJECT(pdsch_dec);

  // See if at least 1 codeword needs to be decoded. If not need to be decode, resend ACK
  bool decode_enable = false;
  bool tb_enable[SRSLTE_MAX_CODEWORDS];
  for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    tb_enable[tb] = ue_dl_cfg.cfg.pdsch.grant.tb[tb].enabled;
    if (action->tb[tb].enabled) {
      decode_enable = true;

      // Prepare I/O based on action
      pdsch_dec[tb].payload                  = action->tb[tb].payload;
      ue_dl_cfg.cfg.pdsch.softbuffers.rx[tb] = action->tb[tb].softbuffer.rx;

      // Use RV from higher layers
      ue_dl_cfg.cfg.pdsch.grant.tb[tb].rv = action->tb[tb].rv;

    } else {
      // If this TB is duplicate, indicate PDSCH to skip it
      ue_dl_cfg.cfg.pdsch.grant.tb[tb].enabled = false;
    }
  }

  // Run PDSCH decoder
  if (decode_enable) {
    if (srslte_ue_dl_decode_pdsch(&ue_dl, &sf_cfg_dl, &ue_dl_cfg.cfg.pdsch, pdsch_dec)) {
      Error("ERROR: Decoding PDSCH\n");
    }
  }

  // Generate ACKs for MAC and PUCCH
  uint8_t pending_acks[SRSLTE_MAX_CODEWORDS];
  for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
    // For MAC, set to true if it's a duplicate
    mac_acks[tb] = action->tb[tb].enabled ? pdsch_dec[tb].crc : true;

    // For PUCCH feedback, need to send even if duplicate, but only those CW that were enabled before disabling in th
    // grant
    pending_acks[tb] = tb_enable[tb] ? mac_acks[tb] : 2;
  }

  if (action->generate_ack && ue_dl_cfg.cfg.pdsch.grant.nof_tb > 0) {
    phy->set_dl_pending_ack(&sf_cfg_dl, cc_idx, pending_acks, ack_resource);
  }

  if (decode_enable) {
    // Metrics
    dl_metrics.mcs         = ue_dl_cfg.cfg.pdsch.grant.tb[0].mcs_idx;
    dl_metrics.turbo_iters = pdsch_dec->avg_iterations_block / 2;

    // Logging
    char str[512];
    srslte_pdsch_rx_info(&ue_dl_cfg.cfg.pdsch, pdsch_dec, str, 512);
    Info("PDSCH: cc=%d, %s, snr=%.1f dB\n", cc_idx, str, ue_dl.chest_res.snr_db);
  }

  return SRSLTE_SUCCESS;
}

int cc_worker::decode_pmch(mac_interface_phy_lte::tb_action_dl_t* action, srslte_mbsfn_cfg_t* mbsfn_cfg)
{
  srslte_pdsch_res_t pmch_dec;

  pmch_cfg.area_id                     = mbsfn_cfg->mbsfn_area_id;
  pmch_cfg.pdsch_cfg.softbuffers.rx[0] = action->tb[0].softbuffer.rx;
  pmch_dec.payload                     = action->tb[0].payload;

  if (action->tb[0].enabled) {

    srslte_softbuffer_rx_reset_tbs(pmch_cfg.pdsch_cfg.softbuffers.rx[0], pmch_cfg.pdsch_cfg.grant.tb[0].tbs);

    if (srslte_ue_dl_decode_pmch(&ue_dl, &sf_cfg_dl, &pmch_cfg, &pmch_dec)) {
      Error("Decoding PMCH\n");
      return -1;
    }

    // Store metrics
    dl_metrics.mcs = pmch_cfg.pdsch_cfg.grant.tb[0].mcs_idx;

    Info("PMCH: l_crb=%2d, tbs=%d, mcs=%d, crc=%s, snr=%.1f dB, n_iter=%.1f\n",
         pmch_cfg.pdsch_cfg.grant.nof_prb,
         pmch_cfg.pdsch_cfg.grant.tb[0].tbs / 8,
         pmch_cfg.pdsch_cfg.grant.tb[0].mcs_idx,
         pmch_dec.crc ? "OK" : "KO",
         ue_dl.chest_res.snr_db,
         pmch_dec.avg_iterations_block);


    if (pmch_dec.crc) {
      return 1;
    }

  } else {
    Warning("Received dci for TBS=0\n");
  }
  return 0;
}

void cc_worker::decode_phich()
{
  srslte_dci_ul_t      dci_ul      = {};
  srslte_phich_grant_t phich_grant = {};
  srslte_phich_res_t   phich_res   = {};

  // Receive PHICH, in TDD might be more than one
  for (uint32_t I_phich = 0; I_phich < 2; I_phich++) {
    phich_grant.I_phich = I_phich;
    if (phy->get_ul_pending_ack(&sf_cfg_dl, cc_idx, &phich_grant, &dci_ul)) {
      if (srslte_ue_dl_decode_phich(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, &phich_grant, &phich_res)) {
        Error("Decoding PHICH\n");
      }
      phy->set_ul_received_ack(&sf_cfg_dl, cc_idx, phich_res.ack_value, I_phich, &dci_ul);
      Info("PHICH: hi=%d, corr=%.1f, I_lowest=%d, n_dmrs=%d, I_phich=%d\n",
           phich_res.ack_value,
           phich_res.distance,
           phich_grant.n_prb_lowest,
           phich_grant.n_dmrs,
           I_phich);
    }
  }
}

void cc_worker::update_measurements()
{
  float snr_ema_coeff = phy->args->snr_ema_coeff;

  // In TDD, ignore special subframes without PDSCH
  if (srslte_sfidx_tdd_type(sf_cfg_dl.tdd_config, CURRENT_SFIDX) == SRSLTE_TDD_SF_S &&
      srslte_sfidx_tdd_nof_dw(sf_cfg_dl.tdd_config) < 4) {
    return;
  }

  // Average RSRQ over DEFAULT_MEAS_PERIOD_MS then sent to RRC
  float rsrq_db = ue_dl.chest_res.rsrq_db;
  if (std::isnormal(rsrq_db)) {
    if (!(CURRENT_TTI % phy->pcell_report_period) || !phy->avg_rsrq_db) {
      phy->avg_rsrq_db = rsrq_db;
    } else {
      phy->avg_rsrq_db = SRSLTE_VEC_CMA(rsrq_db, phy->avg_rsrq_db, CURRENT_TTI % phy->pcell_report_period);
    }
  }

  // Average RSRP taken from CRS
  float rsrp_lin = ue_dl.chest_res.rsrp;
  if (std::isnormal(rsrp_lin)) {
    if (!phy->avg_rsrp[cc_idx] && !std::isnan(phy->avg_rsrp[cc_idx])) {
      phy->avg_rsrp[cc_idx] = SRSLTE_VEC_EMA(rsrp_lin, phy->avg_rsrp[cc_idx], snr_ema_coeff);
    } else {
      phy->avg_rsrp[cc_idx] = rsrp_lin;
    }
  }

  /* Correct absolute power measurements by RX gain offset */
  float rsrp_dbm = ue_dl.chest_res.rsrp_dbm - phy->rx_gain_offset;

  // Serving cell RSRP measurements are averaged over DEFAULT_MEAS_PERIOD_MS then sent to RRC
  if (std::isnormal(rsrp_dbm)) {
    if (!(CURRENT_TTI % phy->pcell_report_period) || !phy->avg_rsrp_dbm[cc_idx]) {
      phy->avg_rsrp_dbm[cc_idx] = rsrp_dbm;
    } else {
      phy->avg_rsrp_dbm[cc_idx] =
          SRSLTE_VEC_CMA(rsrp_dbm, phy->avg_rsrp_dbm[cc_idx], CURRENT_TTI % phy->pcell_report_period);
    }
  }

  // Compute PL
  float tx_crs_power    = ue_dl_cfg.cfg.pdsch.rs_power;
  phy->pathloss[cc_idx] = tx_crs_power - phy->avg_rsrp_dbm[cc_idx];

  // Average noise
  float cur_noise = ue_dl.chest_res.noise_estimate;
  if (std::isnormal(cur_noise)) {
    if (!phy->avg_noise) {
      phy->avg_noise[cc_idx] = cur_noise;
    } else {
      phy->avg_noise[cc_idx] = SRSLTE_VEC_EMA(cur_noise, phy->avg_noise[cc_idx], snr_ema_coeff);
    }
  }

  // Average snr in the log domain
  if (std::isnormal(ue_dl.chest_res.snr_db)) {
    if (!phy->avg_noise) {
      phy->avg_snr_db_cqi[cc_idx] = ue_dl.chest_res.snr_db;
    } else {
      phy->avg_snr_db_cqi[cc_idx] = SRSLTE_VEC_EMA(ue_dl.chest_res.snr_db, phy->avg_snr_db_cqi[cc_idx], snr_ema_coeff);
    }
  }

  // Store metrics
  dl_metrics.n        = phy->avg_noise[cc_idx];
  dl_metrics.rsrp     = phy->avg_rsrp_dbm[cc_idx];
  dl_metrics.rsrq     = phy->avg_rsrq_db;
  dl_metrics.rssi     = phy->avg_rssi_dbm;
  dl_metrics.pathloss = phy->pathloss[cc_idx];
  dl_metrics.sinr     = phy->avg_snr_db_cqi[cc_idx];
  dl_metrics.sync_err = ue_dl.chest_res.sync_error;

  phy->set_dl_metrics(dl_metrics, cc_idx);
  phy->set_ul_metrics(ul_metrics, cc_idx);
}

/************
 *
 * Uplink Functions
 *
 */

bool cc_worker::work_ul(srslte_uci_data_t* uci_data)
{

  bool signal_ready;

  srslte_dci_ul_t                   dci_ul       = {};
  mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant = {};
  mac_interface_phy_lte::tb_action_ul_t ul_action    = {};
  uint32_t                          pid          = 0;

  bool ul_grant_available = phy->get_ul_pending_grant(&sf_cfg_ul, cc_idx, &pid, &dci_ul);
  ul_mac_grant.phich_available =
      phy->get_ul_received_ack(&sf_cfg_ul, cc_idx, &ul_mac_grant.hi_value, ul_grant_available ? NULL : &dci_ul);

  // If there is no grant, pid is from current TX TTI
  if (!ul_grant_available) {
    pid = phy->ul_pidof(CURRENT_TTI_TX, &sf_cfg_ul.tdd_config);
  }

  /* Generate CQI reports if required, note that in case both aperiodic
   * and periodic ones present, only aperiodic is sent (36.213 section 7.2) */
  if (ul_grant_available && dci_ul.cqi_request) {
    set_uci_aperiodic_cqi(uci_data);
  } else {
    /* Check PCell and enabled secondary cells */
    if (cc_idx == 0 || phy->scell_cfg[cc_idx].enabled) {
      set_uci_periodic_cqi(uci_data);
    }
  }

  /* Send UL dci or HARQ information (from PHICH) to MAC and receive actions*/
  if (ul_grant_available || ul_mac_grant.phich_available) {

    // Read last TB info from last retx for this PID
    ue_ul_cfg.ul_cfg.pusch.grant.last_tb = phy->last_ul_tb[pid][cc_idx];

    // Generate PHY grant
    if (srslte_ue_ul_dci_to_pusch_grant(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &dci_ul, &ue_ul_cfg.ul_cfg.pusch.grant)) {
      Error("Converting DCI message to UL dci\n");
    }

    // Save TBS info for next retx
    phy->last_ul_tb[pid][cc_idx] = ue_ul_cfg.ul_cfg.pusch.grant.tb;

    // Fill MAC dci
    ul_phy_to_mac_grant(&ue_ul_cfg.ul_cfg.pusch.grant, &dci_ul, pid, ul_grant_available, &ul_mac_grant);

    phy->stack->new_grant_ul(cc_idx, ul_mac_grant, &ul_action);

    // Calculate PUSCH Hopping procedure
    ue_ul_cfg.ul_cfg.hopping.current_tx_nb = ul_action.current_tx_nb;
    srslte_ue_ul_pusch_hopping(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &ue_ul_cfg.ul_cfg.pusch.grant);
  }

  // Set UL RNTI
  if (ul_grant_available || ul_mac_grant.phich_available) {
    ue_ul_cfg.ul_cfg.pusch.rnti = dci_ul.rnti;
  } else {
    ue_ul_cfg.ul_cfg.pucch.rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI_TX);
  }

  // PCell sends SR and ACK
  if (cc_idx == 0) {
    set_uci_sr(uci_data);
    // This must be called after set_uci_sr() and set_uci_*_cqi
    set_uci_ack(uci_data, ul_grant_available, dci_ul.dai, ul_action.tb.enabled);
  }

  // Generate uplink signal, include uci data on only PCell
  signal_ready = encode_uplink(&ul_action, (cc_idx == 0) ? uci_data : NULL);

  // Prepare to receive ACK through PHICH
  if (ul_action.expect_ack) {
    srslte_phich_grant_t phich_grant = {};
    phich_grant.I_phich              = 0;
    if (cell.frame_type == SRSLTE_TDD && sf_cfg_ul.tdd_config.sf_config == 0) {
      if ((sf_cfg_ul.tti % 10) == 4 || (sf_cfg_ul.tti % 10) == 9) {
        phich_grant.I_phich = 1;
      }
    }
    phich_grant.n_prb_lowest = ue_ul_cfg.ul_cfg.pusch.grant.n_prb_tilde[0];
    phich_grant.n_dmrs       = ue_ul_cfg.ul_cfg.pusch.grant.n_dmrs;

    phy->set_ul_pending_ack(&sf_cfg_ul, cc_idx, phich_grant, &dci_ul);
  }

  return signal_ready;
}

void cc_worker::ul_phy_to_mac_grant(srslte_pusch_grant_t*                         phy_grant,
                                    srslte_dci_ul_t*                              dci_ul,
                                    uint32_t                                      pid,
                                    bool                                          ul_grant_available,
                                    srsue::mac_interface_phy_lte::mac_grant_ul_t* mac_grant)
{
  if (mac_grant->phich_available && !dci_ul->rnti) {
    mac_grant->rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI);
  } else {
    mac_grant->rnti = dci_ul->rnti;
  }
  mac_grant->tb.ndi         = dci_ul->tb.ndi;
  mac_grant->tb.ndi_present = ul_grant_available;
  mac_grant->tb.tbs         = phy_grant->tb.tbs / (uint32_t)8;
  mac_grant->tb.rv          = phy_grant->tb.rv;
  mac_grant->pid            = pid;
}

int cc_worker::decode_pdcch_ul()
{
  int nof_grants = 0;

  srslte_dci_ul_t dci[SRSLTE_MAX_CARRIERS];
  ZERO_OBJECT(dci);

  uint16_t ul_rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI);

  if (ul_rnti) {
    /* Blind search first without cross scheduling then with it if enabled */
    for (int i = 0; i < (phy->cif_enabled ? 2 : 1) && !nof_grants; i++) {
      fill_dci_cfg(&ue_dl_cfg.dci_cfg, i > 0);
      nof_grants = srslte_ue_dl_find_ul_dci(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, ul_rnti, dci);
      if (nof_grants < 0) {
        Error("Looking for UL grants\n");
        return -1;
      }
    }

    /* Convert every DCI message to UL dci */
    for (int k = 0; k < nof_grants; k++) {

      // If the DCI does not have Carrier Indicator Field then indicate in which carrier the dci was found
      uint32_t cc_idx_grant = dci[k].cif_present ? dci[k].cif : cc_idx;

      // Save DCI
      phy->set_ul_pending_grant(&sf_cfg_dl, cc_idx_grant, &dci[k]);

      // Logging
      char str[512];
      srslte_dci_ul_info(&dci[k], str, 512);
      Info("PDCCH: cc=%d, %s, snr=%.1f dB\n", cc_idx_grant, str, ue_dl.chest_res.snr_db);
    }
  }

  return nof_grants;
}

bool cc_worker::encode_uplink(mac_interface_phy_lte::tb_action_ul_t* action, srslte_uci_data_t* uci_data)
{
  srslte_pusch_data_t data = {};
  ue_ul_cfg.cc_idx         = cc_idx;

  // Setup input data
  if (action) {
    data.ptr                              = action->tb.payload;
    ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = action->tb.softbuffer.tx;
  }

  // Set UCI data and configuration
  if (uci_data) {
    data.uci                       = uci_data->value;
    ue_ul_cfg.ul_cfg.pusch.uci_cfg = uci_data->cfg;
    ue_ul_cfg.ul_cfg.pucch.uci_cfg = uci_data->cfg;
  } else {
    ZERO_OBJECT(ue_ul_cfg.ul_cfg.pusch.uci_cfg);
    ZERO_OBJECT(ue_ul_cfg.ul_cfg.pucch.uci_cfg);
  }

  // Use RV from higher layers
  ue_ul_cfg.ul_cfg.pusch.grant.tb.rv = action->tb.rv;

  // Setup PUSCH grant
  ue_ul_cfg.grant_available = action->tb.enabled;

  // Set UL RNTI
  ue_ul_cfg.ul_cfg.pucch.rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI_TX);

  // Encode signal
  int ret = srslte_ue_ul_encode(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &data);
  if (ret < 0) {
    Error("Encoding UL cc=%d\n", cc_idx);
  }

  // Store metrics
  if (action->tb.enabled) {
    ul_metrics.mcs = ue_ul_cfg.ul_cfg.pusch.grant.tb.mcs_idx;
  }

  // Logging
  char str[512];
  if (srslte_ue_ul_info(&ue_ul_cfg, &sf_cfg_ul, &data.uci, str, 512)) {
    Info("%s\n", str);
  }

  return ret > 0;
}

void cc_worker::set_uci_sr(srslte_uci_data_t* uci_data)
{
  Debug("set_uci_sr() query: sr_enabled=%d, last_tx_tti=%d\n", phy->sr_enabled, phy->sr_last_tx_tti);
  if (srslte_ue_ul_gen_sr(&ue_ul_cfg, &sf_cfg_ul, uci_data, phy->sr_enabled)) {
    if (phy->sr_enabled) {
      phy->sr_last_tx_tti = CURRENT_TTI_TX;
      phy->sr_enabled     = false;
      Debug("set_uci_sr() sending SR: sr_enabled=%d, last_tx_tti=%d\n", phy->sr_enabled, phy->sr_last_tx_tti);
    }
  }
}

uint32_t cc_worker::get_wideband_cqi()
{
  int cqi_fixed = phy->args->cqi_fixed;
  int cqi_max   = phy->args->cqi_max;

  uint32_t wb_cqi_value = srslte_cqi_from_snr(phy->avg_snr_db_cqi[cc_idx] + ue_dl_cfg.snr_to_cqi_offset);

  if (cqi_fixed >= 0) {
    wb_cqi_value = cqi_fixed;
  } else if (cqi_max >= 0 && wb_cqi_value > (uint32_t)cqi_max) {
    wb_cqi_value = cqi_max;
  }

  return wb_cqi_value;
}

void cc_worker::set_uci_periodic_cqi(srslte_uci_data_t* uci_data)
{
  srslte_ue_dl_gen_cqi_periodic(&ue_dl, &ue_dl_cfg, get_wideband_cqi(), CURRENT_TTI_TX, uci_data);
}

void cc_worker::set_uci_aperiodic_cqi(srslte_uci_data_t* uci_data)
{
  if (ue_dl_cfg.cfg.cqi_report.aperiodic_configured) {
    srslte_ue_dl_gen_cqi_aperiodic(&ue_dl, &ue_dl_cfg, get_wideband_cqi(), uci_data);
  } else {
    Warning("Received CQI request but aperiodic mode is not configured\n");
  }
}

void cc_worker::set_uci_ack(srslte_uci_data_t* uci_data,
                            bool               is_grant_available,
                            uint32_t           V_dai_ul,
                            bool               is_pusch_available)
{

  srslte_pdsch_ack_t ack_info                = {};
  uint32_t           nof_configured_carriers = 0;

  // Only PCell generates ACK for all SCell
  for (uint32_t cc_idx = 0; cc_idx < phy->args->nof_carriers; cc_idx++) {
    if (cc_idx == 0 || phy->scell_cfg[cc_idx].configured) {
      phy->get_dl_pending_ack(&sf_cfg_ul, cc_idx, &ack_info.cc[cc_idx]);
      nof_configured_carriers++;
    }
  }

  // Set ACK length for CA (default value is set to DTX)
  if (ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode != SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL) {
    if (ue_dl_cfg.cfg.tm > SRSLTE_TM2) {
      /* TM3, TM4 */
      uci_data->cfg.ack.nof_acks = nof_configured_carriers * SRSLTE_MAX_CODEWORDS;
    } else {
      /* TM1, TM2 */
      uci_data->cfg.ack.nof_acks = nof_configured_carriers;
    }
  }

  // Configure ACK parameters
  ack_info.is_grant_available     = is_grant_available;
  ack_info.is_pusch_available     = is_pusch_available;
  ack_info.V_dai_ul               = V_dai_ul;
  ack_info.tdd_ack_bundle         = ue_ul_cfg.ul_cfg.pucch.tdd_ack_bundle;
  ack_info.simul_cqi_ack          = ue_ul_cfg.ul_cfg.pucch.simul_cqi_ack;
  ack_info.ack_nack_feedback_mode = ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode;
  ack_info.nof_cc                 = nof_configured_carriers;
  ack_info.transmission_mode      = ue_dl_cfg.cfg.tm;

  // Generate ACK/NACK bits
  srslte_ue_dl_gen_ack(&ue_dl, &sf_cfg_dl, &ack_info, uci_data);
}

/************
 *
 * Configuration Functions
 *
 */

srslte_cqi_report_mode_t cc_worker::aperiodic_mode(cqi_report_mode_aperiodic_e mode)
{
  switch (mode) {
    case cqi_report_mode_aperiodic_e::rm12:
      return SRSLTE_CQI_MODE_12;
    case cqi_report_mode_aperiodic_e::rm20:
      return SRSLTE_CQI_MODE_20;
    case cqi_report_mode_aperiodic_e::rm22:
      return SRSLTE_CQI_MODE_22;
    case cqi_report_mode_aperiodic_e::rm30:
      return SRSLTE_CQI_MODE_30;
    case cqi_report_mode_aperiodic_e::rm31:
      return SRSLTE_CQI_MODE_31;
    case cqi_report_mode_aperiodic_e::rm10_v1310:
    case cqi_report_mode_aperiodic_e::rm11_v1310:
    case cqi_report_mode_aperiodic_e::rm32_v1250:
      fprintf(stderr, "Aperiodic mode %s not handled\n", mode.to_string().c_str());
    default:
      return SRSLTE_CQI_MODE_NA;
  }
}

void cc_worker::parse_antenna_info(phys_cfg_ded_s* dedicated)
{
  if (dedicated->ant_info_r10_present) {
    // Parse Release 10
    ant_info_ded_r10_s::tx_mode_r10_e_::options tx_mode =
        dedicated->ant_info_r10->explicit_value_r10().tx_mode_r10.value;
    if ((srslte_tm_t)tx_mode < SRSLTE_TMINV) {
      ue_dl_cfg.cfg.tm = (srslte_tm_t)tx_mode;
    } else {
      fprintf(stderr,
              "Transmission mode (R10) %s is not supported\n",
              dedicated->ant_info_r10->explicit_value_r10().tx_mode_r10.to_string().c_str());
    }
  } else if (dedicated->ant_info_present &&
             dedicated->ant_info.type() == phys_cfg_ded_s::ant_info_c_::types::explicit_value) {
    // Parse Release 8
    ant_info_ded_s::tx_mode_e_::options tx_mode = dedicated->ant_info.explicit_value().tx_mode.value;
    if ((srslte_tm_t)tx_mode < SRSLTE_TMINV) {
      ue_dl_cfg.cfg.tm = (srslte_tm_t)tx_mode;
    } else {
      fprintf(stderr,
              "Transmission mode (R8) %s is not supported\n",
              dedicated->ant_info.explicit_value().tx_mode.to_string().c_str());
    }
  } else {
    if (cell.nof_ports == 1) {
      // No antenna info provided
      ue_dl_cfg.cfg.tm = SRSLTE_TM1;
    } else {
      // No antenna info provided
      ue_dl_cfg.cfg.tm = SRSLTE_TM2;
    }
  }
}

void cc_worker::parse_pucch_config(phy_interface_rrc_lte::phy_cfg_t* phy_cfg)
{
  phy_interface_rrc_lte::phy_cfg_common_t* common    = &phy_cfg->common;
  phys_cfg_ded_s*                      dedicated = &phy_cfg->dedicated;

  /* PUCCH configuration */
  bzero(&ue_ul_cfg.ul_cfg.pucch, sizeof(srslte_pucch_cfg_t));
  ue_ul_cfg.ul_cfg.pucch.delta_pucch_shift = common->pucch_cnfg.delta_pucch_shift.to_number();
  ue_ul_cfg.ul_cfg.pucch.N_cs              = common->pucch_cnfg.n_cs_an;
  ue_ul_cfg.ul_cfg.pucch.n_rb_2            = common->pucch_cnfg.n_rb_cqi;

  /* PUCCH Scheduling configuration */
  ue_ul_cfg.ul_cfg.pucch.n_pucch_1[0] = 0; // TODO: n_pucch_1 for SPS
  ue_ul_cfg.ul_cfg.pucch.n_pucch_1[1] = 0;
  ue_ul_cfg.ul_cfg.pucch.n_pucch_1[2] = 0;
  ue_ul_cfg.ul_cfg.pucch.n_pucch_1[3] = 0;
  ue_ul_cfg.ul_cfg.pucch.N_pucch_1    = common->pucch_cnfg.n1_pucch_an;
  if (dedicated->cqi_report_cfg.cqi_report_periodic_present and
      dedicated->cqi_report_cfg.cqi_report_periodic.type().value == setup_e::setup) {
    ue_ul_cfg.ul_cfg.pucch.n_pucch_2     = dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx;
    ue_ul_cfg.ul_cfg.pucch.simul_cqi_ack = dedicated->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi;
  } else {
    // FIXME: put is_pucch_configured flag here?
    ue_ul_cfg.ul_cfg.pucch.n_pucch_2     = 0;
    ue_ul_cfg.ul_cfg.pucch.simul_cqi_ack = false;
  }

  /* SR configuration */
  if (dedicated->sched_request_cfg_present and dedicated->sched_request_cfg.type() == setup_e::setup) {
    ue_ul_cfg.ul_cfg.pucch.I_sr          = dedicated->sched_request_cfg.setup().sr_cfg_idx;
    ue_ul_cfg.ul_cfg.pucch.n_pucch_sr    = dedicated->sched_request_cfg.setup().sr_pucch_res_idx;
    ue_ul_cfg.ul_cfg.pucch.sr_configured = true;
  } else {
    ue_ul_cfg.ul_cfg.pucch.I_sr          = 0;
    ue_ul_cfg.ul_cfg.pucch.n_pucch_sr    = 0;
    ue_ul_cfg.ul_cfg.pucch.sr_configured = false;
  }

  if (dedicated->pucch_cfg_ded.tdd_ack_nack_feedback_mode_present) {
    ue_ul_cfg.ul_cfg.pucch.tdd_ack_bundle =
        dedicated->pucch_cfg_ded.tdd_ack_nack_feedback_mode == pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_::bundling;
  } else {
    ue_ul_cfg.ul_cfg.pucch.tdd_ack_bundle = false;
  }

  if (dedicated->pucch_cfg_ded_v1020_present) {
    pucch_cfg_ded_v1020_s* pucch_cfg_ded = dedicated->pucch_cfg_ded_v1020.get();

    if (pucch_cfg_ded->pucch_format_r10_present) {

      typedef pucch_cfg_ded_v1020_s::pucch_format_r10_c_ pucch_format_r10_t;
      pucch_format_r10_t*                                pucch_format_r10 = &pucch_cfg_ded->pucch_format_r10;

      if (pucch_format_r10->type() == pucch_format_r10_t::types::format3_r10) {
        // Select feedback mode
        ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3;

        pucch_format3_conf_r13_s* format3_r13 = &pucch_format_r10->format3_r10();
        for (uint32_t n = 0; n < SRSLTE_MIN(format3_r13->n3_pucch_an_list_r13.size(), SRSLTE_PUCCH_SIZE_AN_CS); n++) {
          ue_ul_cfg.ul_cfg.pucch.n3_pucch_an_list[n] = format3_r13->n3_pucch_an_list_r13[n];
        }
        if (format3_r13->two_ant_port_activ_pucch_format3_r13_present) {
          if (format3_r13->two_ant_port_activ_pucch_format3_r13.type() == setup_e::setup) {
            // TODO: UL MIMO Configure PUCCH two antenna port
          } else {
            // TODO: UL MIMO Disable two antenna port
          }
        }
      } else if (pucch_format_r10->type() == pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types::ch_sel_r10) {

        typedef pucch_format_r10_t::ch_sel_r10_s_ ch_sel_r10_t;
        ch_sel_r10_t*                             ch_sel_r10 = &pucch_format_r10->ch_sel_r10();

        if (ch_sel_r10->n1_pucch_an_cs_r10_present) {
          typedef ch_sel_r10_t::n1_pucch_an_cs_r10_c_ n1_pucch_an_cs_r10_t;
          n1_pucch_an_cs_r10_t*                       n1_pucch_an_cs_r10 = &ch_sel_r10->n1_pucch_an_cs_r10;

          if (n1_pucch_an_cs_r10->type() == setup_e::setup) {
            // Select feedback mode
            ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS;

            typedef n1_pucch_an_cs_r10_t::setup_s_::n1_pucch_an_cs_list_r10_l_ n1_pucch_an_cs_list_r10_t;
            n1_pucch_an_cs_list_r10_t                                          n1_pucch_an_cs_list =
                ch_sel_r10->n1_pucch_an_cs_r10.setup().n1_pucch_an_cs_list_r10;
            for (uint32_t i = 0; i < SRSLTE_MIN(n1_pucch_an_cs_list.size(), SRSLTE_PUCCH_NOF_AN_CS); i++) {
              n1_pucch_an_cs_r10_l n1_pucch_an_cs = n1_pucch_an_cs_list[i];
              for (uint32_t j = 0; j < SRSLTE_PUCCH_SIZE_AN_CS; j++) {
                ue_ul_cfg.ul_cfg.pucch.n1_pucch_an_cs[j][i] = n1_pucch_an_cs[j];
              }
            }
          } else {
            ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL;
          }
        }
      } else {
        // Do nothing
      }
    }
  }
}

/* Translates RRC structs into PHY structs
 */
void cc_worker::set_pcell_config(phy_interface_rrc_lte::phy_cfg_t* phy_cfg)
{
  phy_interface_rrc_lte::phy_cfg_common_t* common    = &phy_cfg->common;
  phys_cfg_ded_s*                      dedicated = &phy_cfg->dedicated;

  // Configure PDSCH
  if (dedicated->pdsch_cfg_ded_present && common->pdsch_cnfg.p_b < 4) {
    ue_dl_cfg.cfg.pdsch.p_a         = dedicated->pdsch_cfg_ded.p_a.to_number();
    ue_dl_cfg.cfg.pdsch.p_b         = common->pdsch_cnfg.p_b;
    ue_dl_cfg.cfg.pdsch.power_scale = true;
  } else {
    ue_dl_cfg.cfg.pdsch.power_scale = false;
  }
  ue_dl_cfg.cfg.pdsch.rs_power = (float)common->pdsch_cnfg.ref_sig_pwr;
  parse_antenna_info(dedicated);

  // Configure PUSCH
  ue_ul_cfg.ul_cfg.pusch.enable_64qam =
      phy_cfg->common.pusch_cnfg.pusch_cfg_basic.enable64_qam && phy_cfg->common.rrc_enable_64qam;

  /* PUSCH DMRS signal configuration */
  bzero(&ue_ul_cfg.ul_cfg.dmrs, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));
  ue_ul_cfg.ul_cfg.dmrs.group_hopping_en    = common->pusch_cnfg.ul_ref_sigs_pusch.group_hop_enabled;
  ue_ul_cfg.ul_cfg.dmrs.sequence_hopping_en = common->pusch_cnfg.ul_ref_sigs_pusch.seq_hop_enabled;
  ue_ul_cfg.ul_cfg.dmrs.cyclic_shift        = common->pusch_cnfg.ul_ref_sigs_pusch.cyclic_shift;
  ue_ul_cfg.ul_cfg.dmrs.delta_ss            = common->pusch_cnfg.ul_ref_sigs_pusch.group_assign_pusch;

  /* PUSCH Hopping configuration */
  bzero(&ue_ul_cfg.ul_cfg.hopping, sizeof(srslte_pusch_hopping_cfg_t));
  ue_ul_cfg.ul_cfg.hopping.n_sb = common->pusch_cnfg.pusch_cfg_basic.n_sb;
  ue_ul_cfg.ul_cfg.hopping.hop_mode =
      common->pusch_cnfg.pusch_cfg_basic.hop_mode.value ==
              pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::intra_and_inter_sub_frame
          ? ue_ul_cfg.ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF
          : ue_ul_cfg.ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF;
  ue_ul_cfg.ul_cfg.hopping.hopping_offset = common->pusch_cnfg.pusch_cfg_basic.pusch_hop_offset;

  /* PUSCH UCI configuration */
  bzero(&ue_ul_cfg.ul_cfg.pusch.uci_offset, sizeof(srslte_uci_offset_cfg_t));
  ue_ul_cfg.ul_cfg.pusch.uci_offset.I_offset_ack = dedicated->pusch_cfg_ded.beta_offset_ack_idx;
  ue_ul_cfg.ul_cfg.pusch.uci_offset.I_offset_cqi = dedicated->pusch_cfg_ded.beta_offset_cqi_idx;
  ue_ul_cfg.ul_cfg.pusch.uci_offset.I_offset_ri  = dedicated->pusch_cfg_ded.beta_offset_ri_idx;

  parse_pucch_config(phy_cfg);

  /* SRS Configuration */
  bzero(&ue_ul_cfg.ul_cfg.srs, sizeof(srslte_refsignal_srs_cfg_t));
  ue_ul_cfg.ul_cfg.srs.configured = dedicated->srs_ul_cfg_ded_present and
                                    dedicated->srs_ul_cfg_ded.type() == setup_e::setup and
                                    common->srs_ul_cnfg.type() == setup_e::setup;
  if (ue_ul_cfg.ul_cfg.srs.configured) {
    ue_ul_cfg.ul_cfg.srs.I_srs           = dedicated->srs_ul_cfg_ded.setup().srs_cfg_idx;
    ue_ul_cfg.ul_cfg.srs.B               = dedicated->srs_ul_cfg_ded.setup().srs_bw;
    ue_ul_cfg.ul_cfg.srs.b_hop           = dedicated->srs_ul_cfg_ded.setup().srs_hop_bw;
    ue_ul_cfg.ul_cfg.srs.n_rrc           = dedicated->srs_ul_cfg_ded.setup().freq_domain_position;
    ue_ul_cfg.ul_cfg.srs.k_tc            = dedicated->srs_ul_cfg_ded.setup().tx_comb;
    ue_ul_cfg.ul_cfg.srs.n_srs           = dedicated->srs_ul_cfg_ded.setup().cyclic_shift;
    ue_ul_cfg.ul_cfg.srs.simul_ack       = common->srs_ul_cnfg.setup().ack_nack_srs_simul_tx;
    ue_ul_cfg.ul_cfg.srs.bw_cfg          = common->srs_ul_cnfg.setup().srs_bw_cfg.to_number();
    ue_ul_cfg.ul_cfg.srs.subframe_config = common->srs_ul_cnfg.setup().srs_sf_cfg.to_number();
  }

  /* UL power control configuration */
  bzero(&ue_ul_cfg.ul_cfg.power_ctrl, sizeof(srslte_ue_ul_powerctrl_t));
  ue_ul_cfg.ul_cfg.power_ctrl.p0_nominal_pusch = common->ul_pwr_ctrl.p0_nominal_pusch;
  ue_ul_cfg.ul_cfg.power_ctrl.alpha            = common->ul_pwr_ctrl.alpha.to_number();
  ue_ul_cfg.ul_cfg.power_ctrl.p0_nominal_pucch = common->ul_pwr_ctrl.p0_nominal_pucch;
  ue_ul_cfg.ul_cfg.power_ctrl.delta_f_pucch[0] =
      common->ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format1.to_number();
  ue_ul_cfg.ul_cfg.power_ctrl.delta_f_pucch[1] =
      common->ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format1b.to_number();
  ue_ul_cfg.ul_cfg.power_ctrl.delta_f_pucch[2] =
      common->ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2.to_number();
  ue_ul_cfg.ul_cfg.power_ctrl.delta_f_pucch[3] =
      common->ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2a.to_number();
  ue_ul_cfg.ul_cfg.power_ctrl.delta_f_pucch[4] =
      common->ul_pwr_ctrl.delta_flist_pucch.delta_f_pucch_format2b.to_number();

  ue_ul_cfg.ul_cfg.power_ctrl.delta_preamble_msg3 = common->ul_pwr_ctrl.delta_preamb_msg3;

  ue_ul_cfg.ul_cfg.power_ctrl.p0_ue_pusch = dedicated->ul_pwr_ctrl_ded.p0_ue_pusch;
  ue_ul_cfg.ul_cfg.power_ctrl.delta_mcs_based =
      dedicated->ul_pwr_ctrl_ded.delta_mcs_enabled == ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
  ue_ul_cfg.ul_cfg.power_ctrl.acc_enabled  = dedicated->ul_pwr_ctrl_ded.accumulation_enabled;
  ue_ul_cfg.ul_cfg.power_ctrl.p0_ue_pucch  = dedicated->ul_pwr_ctrl_ded.p0_ue_pucch;
  ue_ul_cfg.ul_cfg.power_ctrl.p_srs_offset = dedicated->ul_pwr_ctrl_ded.p_srs_offset;

  /* CQI configuration */
  bzero(&ue_dl_cfg.cfg.cqi_report, sizeof(srslte_cqi_report_cfg_t));
  ue_dl_cfg.cfg.cqi_report.periodic_configured = dedicated->cqi_report_cfg.cqi_report_periodic_present and
                                                 dedicated->cqi_report_cfg.cqi_report_periodic.type() == setup_e::setup;
  if (ue_dl_cfg.cfg.cqi_report.periodic_configured) {
    ue_dl_cfg.cfg.cqi_report.pmi_idx = dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx;
    ue_dl_cfg.cfg.cqi_report.format_is_subband =
        dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.type().value ==
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::subband_cqi;
    if (ue_dl_cfg.cfg.cqi_report.format_is_subband) {
      ue_dl_cfg.cfg.cqi_report.subband_size =
          dedicated->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.subband_cqi().k;
    }
    if (dedicated->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present) {
      if (cell.nof_ports == 1) {
        log_h->warning("Received Rank Indication report configuration but only 1 antenna is available\n");
      }
      ue_dl_cfg.cfg.cqi_report.ri_idx         = dedicated->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx;
      ue_dl_cfg.cfg.cqi_report.ri_idx_present = true;
    } else {
      ue_dl_cfg.cfg.cqi_report.ri_idx_present = false;
    }
  }

  if (dedicated->cqi_report_cfg.cqi_report_mode_aperiodic_present) {
    ue_dl_cfg.cfg.cqi_report.aperiodic_configured = true;
    ue_dl_cfg.cfg.cqi_report.aperiodic_mode       = aperiodic_mode(dedicated->cqi_report_cfg.cqi_report_mode_aperiodic);
  }
  if (dedicated->cqi_report_cfg_pcell_v1250_present) {
    auto cqi_report_cfg_pcell_v1250 = dedicated->cqi_report_cfg_pcell_v1250.get();
    if (cqi_report_cfg_pcell_v1250->alt_cqi_table_r12_present) {
      ue_dl_cfg.pdsch_use_tbs_index_alt = true;
    }
  } else {
    ue_dl_cfg.pdsch_use_tbs_index_alt = false;
  }

  if (pregen_enabled) {
    Info("Pre-generating UL signals...\n");
    srslte_ue_ul_pregen_signals(&ue_ul, &ue_ul_cfg);
    Info("Done pre-generating signals worker...\n");
  }
}

void cc_worker::set_scell_config(asn1::rrc::scell_to_add_mod_r10_s* phy_cfg)
{
  if (phy_cfg->rr_cfg_common_scell_r10_present) {
    rr_cfg_common_scell_r10_s* rr_cfg_common_scell_r10 = &phy_cfg->rr_cfg_common_scell_r10;

    if (rr_cfg_common_scell_r10->ul_cfg_r10_present) {
      typedef rr_cfg_common_scell_r10_s::ul_cfg_r10_s_ ul_cfg_r10_t;
      ul_cfg_r10_t*                                    ul_cfg_r10 = &rr_cfg_common_scell_r10->ul_cfg_r10;

      // Parse Power control
      ul_pwr_ctrl_common_scell_r10_s* ul_pwr_ctrl_common_scell_r10 = &ul_cfg_r10->ul_pwr_ctrl_common_scell_r10;
      bzero(&ue_ul_cfg.ul_cfg.power_ctrl, sizeof(srslte_ue_ul_powerctrl_t));
      ue_ul_cfg.ul_cfg.power_ctrl.p0_nominal_pusch = ul_pwr_ctrl_common_scell_r10->p0_nominal_pusch_r10;
      ue_ul_cfg.ul_cfg.power_ctrl.alpha            = ul_pwr_ctrl_common_scell_r10->alpha_r10.to_number();

      // Parse SRS
      typedef srs_ul_cfg_common_c::setup_s_ srs_ul_cfg_common_t;
      if (ul_cfg_r10->srs_ul_cfg_common_r10.type() == setup_e::setup) {
        srs_ul_cfg_common_t* srs_ul_cfg_common = &ul_cfg_r10->srs_ul_cfg_common_r10.setup();
        ue_ul_cfg.ul_cfg.srs.configured        = true;
        ue_ul_cfg.ul_cfg.srs.simul_ack         = srs_ul_cfg_common->ack_nack_srs_simul_tx;
        ue_ul_cfg.ul_cfg.srs.bw_cfg            = srs_ul_cfg_common->srs_bw_cfg.to_number();
        ue_ul_cfg.ul_cfg.srs.subframe_config   = srs_ul_cfg_common->srs_sf_cfg.to_number();
      } else {
        ue_ul_cfg.ul_cfg.srs.configured = false;
      }

      // Parse PUSCH
      pusch_cfg_common_s* pusch_cfg_common = &ul_cfg_r10->pusch_cfg_common_r10;
      bzero(&ue_ul_cfg.ul_cfg.hopping, sizeof(srslte_pusch_hopping_cfg_t));
      ue_ul_cfg.ul_cfg.hopping.n_sb = pusch_cfg_common->pusch_cfg_basic.n_sb;
      ue_ul_cfg.ul_cfg.hopping.hop_mode =
          pusch_cfg_common->pusch_cfg_basic.hop_mode.value ==
                  pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::intra_and_inter_sub_frame
              ? ue_ul_cfg.ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF
              : ue_ul_cfg.ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF;
      ue_ul_cfg.ul_cfg.hopping.hopping_offset = pusch_cfg_common->pusch_cfg_basic.pusch_hop_offset;
      ue_ul_cfg.ul_cfg.pusch.enable_64qam     = pusch_cfg_common->pusch_cfg_basic.enable64_qam;
    }
  }

  if (phy_cfg->rr_cfg_ded_scell_r10_present) {
    rr_cfg_ded_scell_r10_s* rr_cfg_ded_scell_r10 = &phy_cfg->rr_cfg_ded_scell_r10;
    if (rr_cfg_ded_scell_r10->phys_cfg_ded_scell_r10_present) {
      phys_cfg_ded_scell_r10_s* phys_cfg_ded_scell_r10 = &rr_cfg_ded_scell_r10->phys_cfg_ded_scell_r10;

      // Parse nonUL Configuration
      if (phys_cfg_ded_scell_r10->non_ul_cfg_r10_present) {

        typedef phys_cfg_ded_scell_r10_s::non_ul_cfg_r10_s_ non_ul_cfg_t;
        non_ul_cfg_t*                                       non_ul_cfg = &phys_cfg_ded_scell_r10->non_ul_cfg_r10;

        // Parse Transmission mode
        if (non_ul_cfg->ant_info_r10_present) {
          ant_info_ded_r10_s::tx_mode_r10_e_::options tx_mode = non_ul_cfg->ant_info_r10.tx_mode_r10.value;
          if ((srslte_tm_t)tx_mode < SRSLTE_TMINV) {
            ue_dl_cfg.cfg.tm = (srslte_tm_t)tx_mode;
          } else {
            fprintf(stderr,
                    "Transmission mode (R10) %s is not supported\n",
                    non_ul_cfg->ant_info_r10.tx_mode_r10.to_string().c_str());
          }
        }

        // Parse Cross carrier scheduling
        if (non_ul_cfg->cross_carrier_sched_cfg_r10_present) {
          typedef cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_ sched_info_t;

          typedef sched_info_t::types cross_carrier_type_e;
          sched_info_t*               sched_info = &non_ul_cfg->cross_carrier_sched_cfg_r10.sched_cell_info_r10;

          cross_carrier_type_e cross_carrier_type = sched_info->type();
          if (cross_carrier_type == cross_carrier_type_e::own_r10) {
            ue_dl_cfg.dci_cfg.cif_enabled = sched_info->own_r10().cif_presence_r10;
          } else {
            ue_dl_cfg.dci_cfg.cif_enabled = false; // This CC does not have Carrier Indicator Field
            // ue_dl_cfg.blablabla = sched_info->other_r10().pdsch_start_r10;
            // ue_dl_cfg.blablabla = sched_info->other_r10().sched_cell_id_r10;
          }
        }

        // Parse pdsch config dedicated
        if (non_ul_cfg->pdsch_cfg_ded_r10_present) {
          ue_dl_cfg.cfg.pdsch.p_b         = phy_cfg->rr_cfg_common_scell_r10.non_ul_cfg_r10.pdsch_cfg_common_r10.p_b;
          ue_dl_cfg.cfg.pdsch.p_a         = non_ul_cfg->pdsch_cfg_ded_r10.p_a.to_number();
          ue_dl_cfg.cfg.pdsch.power_scale = true;
        }
      }

      // Parse UL Configuration
      if (phys_cfg_ded_scell_r10->ul_cfg_r10_present) {
        typedef phys_cfg_ded_scell_r10_s::ul_cfg_r10_s_ ul_cfg_t;
        ul_cfg_t*                                       ul_cfg = &phys_cfg_ded_scell_r10->ul_cfg_r10;

        // Parse CQI param
        if (ul_cfg->cqi_report_cfg_scell_r10_present) {
          cqi_report_cfg_scell_r10_s* cqi_report_cfg = &ul_cfg->cqi_report_cfg_scell_r10;

          // Aperiodic report
          if (cqi_report_cfg->cqi_report_mode_aperiodic_r10_present) {
            ue_dl_cfg.cfg.cqi_report.aperiodic_configured = true;
            ue_dl_cfg.cfg.cqi_report.aperiodic_mode = aperiodic_mode(cqi_report_cfg->cqi_report_mode_aperiodic_r10);
          }

          // Periodic report
          if (cqi_report_cfg->cqi_report_periodic_scell_r10_present) {
            if (cqi_report_cfg->cqi_report_periodic_scell_r10.type() == setup_e::setup) {
              typedef cqi_report_periodic_r10_c::setup_s_ cqi_cfg_t;
              cqi_cfg_t cqi_cfg                            = cqi_report_cfg->cqi_report_periodic_scell_r10.setup();
              ue_dl_cfg.cfg.cqi_report.periodic_configured = true;
              ue_dl_cfg.cfg.cqi_report.pmi_idx             = cqi_cfg.cqi_pmi_cfg_idx;
              ue_dl_cfg.cfg.cqi_report.format_is_subband =
                  cqi_cfg.cqi_format_ind_periodic_r10.type().value ==
                  cqi_cfg_t::cqi_format_ind_periodic_r10_c_::types::subband_cqi_r10;
              if (ue_dl_cfg.cfg.cqi_report.format_is_subband) {
                ue_dl_cfg.cfg.cqi_report.subband_size = cqi_cfg.cqi_format_ind_periodic_r10.subband_cqi_r10().k;
              }
              if (cqi_cfg.ri_cfg_idx_present) {
                ue_dl_cfg.cfg.cqi_report.ri_idx         = cqi_cfg.ri_cfg_idx;
                ue_dl_cfg.cfg.cqi_report.ri_idx_present = true;
              } else {
                ue_dl_cfg.cfg.cqi_report.ri_idx_present = false;
              }
            } else {
              // Release, disable periodic reporting
              ue_dl_cfg.cfg.cqi_report.periodic_configured = false;
            }
          }
        }

        if (ul_cfg->srs_ul_cfg_ded_r10_present) {
          // Sounding reference signals
          if (ul_cfg->srs_ul_cfg_ded_r10.type() == setup_e::setup) {
            srs_ul_cfg_ded_c::setup_s_* srs_ul_cfg_ded_r10 = &ul_cfg->srs_ul_cfg_ded_r10.setup();
            ue_ul_cfg.ul_cfg.srs.I_srs                     = srs_ul_cfg_ded_r10->srs_cfg_idx;
            ue_ul_cfg.ul_cfg.srs.B                         = srs_ul_cfg_ded_r10->srs_bw;
            ue_ul_cfg.ul_cfg.srs.b_hop                     = srs_ul_cfg_ded_r10->srs_hop_bw;
            ue_ul_cfg.ul_cfg.srs.n_rrc                     = srs_ul_cfg_ded_r10->freq_domain_position;
            ue_ul_cfg.ul_cfg.srs.k_tc                      = srs_ul_cfg_ded_r10->tx_comb;
            ue_ul_cfg.ul_cfg.srs.n_srs                     = srs_ul_cfg_ded_r10->cyclic_shift;
          } else {
            ue_ul_cfg.ul_cfg.srs.configured = false;
          }
        }
      }

      if (phys_cfg_ded_scell_r10->cqi_report_cfg_scell_v1250_present) {
        auto cqi_report_cfg_scell = phys_cfg_ded_scell_r10->cqi_report_cfg_scell_v1250.get();

        // Enable/disable PDSCH 256QAM
        ue_dl_cfg.pdsch_use_tbs_index_alt = cqi_report_cfg_scell->alt_cqi_table_r12_present;
      } else {
        // Assume there is no PDSCH 256QAM
        ue_dl_cfg.pdsch_use_tbs_index_alt = false;
      }
    }
  }
}

int cc_worker::read_ce_abs(float* ce_abs, uint32_t tx_antenna, uint32_t rx_antenna)
{
  uint32_t i  = 0;
  int      sz = srslte_symbol_sz(cell.nof_prb);
  bzero(ce_abs, sizeof(float) * sz);
  int g = (sz - 12 * cell.nof_prb) / 2;
  for (i = 0; i < 12 * cell.nof_prb; i++) {
    ce_abs[g + i] = 20 * log10f(std::abs(std::complex<float>(ue_dl.chest_res.ce[tx_antenna][rx_antenna][i])));
    if (std::isinf(ce_abs[g + i])) {
      ce_abs[g + i] = -80;
    }
  }
  return sz;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  memcpy(pdsch_d, ue_dl.pdsch.d[0], ue_dl_cfg.cfg.pdsch.grant.nof_re * sizeof(cf_t));
  return ue_dl_cfg.cfg.pdsch.grant.nof_re;
}

} // namespace srsue
