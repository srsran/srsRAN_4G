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

#include "srsran/srsran.h"

#include "srsran/common/standard_streams.h"
#include "srsue/hdr/phy/lte/cc_worker.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger.debug(fmt, ##__VA_ARGS__)

#define CURRENT_TTI (sf_cfg_dl.tti)
#define CURRENT_SFIDX (sf_cfg_dl.tti % 10)
#define CURRENT_TTI_TX (sf_cfg_ul.tti)

namespace srsue {
namespace lte {

/************
 *
 * Common Functions
 *
 */

cc_worker::cc_worker(uint32_t cc_idx_, uint32_t max_prb, srsue::phy_common* phy_, srslog::basic_logger& logger) :
  logger(logger)
{
  cc_idx = cc_idx_;
  phy    = phy_;

  srsran_cfr_cfg_t cfr_config = phy->get_cfr_config();

  signal_buffer_max_samples = 3 * SRSRAN_SF_LEN_PRB(max_prb);

  for (uint32_t i = 0; i < phy->args->nof_rx_ant; i++) {
    signal_buffer_rx[i] = srsran_vec_cf_malloc(signal_buffer_max_samples);
    if (!signal_buffer_rx[i]) {
      Error("Allocating memory");
      return;
    }
    signal_buffer_tx[i] = srsran_vec_cf_malloc(signal_buffer_max_samples);
    if (!signal_buffer_tx[i]) {
      Error("Allocating memory");
      return;
    }
  }

  if (srsran_ue_dl_init(&ue_dl, signal_buffer_rx, max_prb, phy->args->nof_rx_ant)) {
    Error("Initiating UE DL");
    return;
  }

  if (srsran_ue_ul_init(&ue_ul, signal_buffer_tx[0], max_prb)) {
    Error("Initiating UE UL");
    return;
  }

  if (srsran_ue_ul_set_cfr(&ue_ul, &cfr_config) < SRSRAN_SUCCESS) {
    Error("Setting the CFR");
    return;
  }

  phy->set_ue_dl_cfg(&ue_dl_cfg);
  phy->set_ue_ul_cfg(&ue_ul_cfg);
  phy->set_pdsch_cfg(&ue_dl_cfg.cfg.pdsch);
  phy->set_pdsch_cfg(&pmch_cfg.pdsch_cfg); // set same config in PMCH decoder

  // Define MBSFN subframes channel estimation and save default one
  chest_mbsfn_cfg.filter_type    = SRSRAN_CHEST_FILTER_TRIANGLE;
  chest_mbsfn_cfg.filter_coef[0] = 0.1;
  chest_mbsfn_cfg.estimator_alg  = SRSRAN_ESTIMATOR_ALG_INTERPOLATE;
  chest_mbsfn_cfg.noise_alg      = SRSRAN_NOISE_ALG_PSS;

  chest_default_cfg = ue_dl_cfg.chest_cfg;

  srsran_softbuffer_rx_init(&mch_softbuffer, 100);
  // Set default PHY params
  reset();

  if (phy->args->pdsch_8bit_decoder) {
    ue_dl.pdsch.llr_is_8bit        = true;
    ue_dl.pdsch.dl_sch.llr_is_8bit = true;
  }
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
  srsran_softbuffer_rx_free(&mch_softbuffer);
  srsran_ue_dl_free(&ue_dl);
  srsran_ue_ul_free(&ue_ul);
}

void cc_worker::reset()
{
  // constructor sets defaults
  srsran::phy_cfg_t empty_cfg;
  set_config_nolock(empty_cfg);
}

void cc_worker::reset_cell_nolock()
{
  cell_initiated = false;
}

bool cc_worker::set_cell_nolock(srsran_cell_t cell_)
{
  if (cell.id != cell_.id || !cell_initiated) {
    cell = cell_;

    if (srsran_ue_dl_set_cell(&ue_dl, cell)) {
      Error("Setting ue_dl cell");
      return false;
    }

    if (srsran_ue_dl_set_mbsfn_area_id(&ue_dl, 1)) {
      Error("Setting mbsfn id");
    }

    if (srsran_ue_ul_set_cell(&ue_ul, cell)) {
      Error("Initiating UE UL");
      return false;
    }

    if (cell.frame_type == SRSRAN_TDD && ue_dl_cfg.chest_cfg.estimator_alg != SRSRAN_ESTIMATOR_ALG_INTERPOLATE) {
      chest_default_cfg.estimator_alg = SRSRAN_ESTIMATOR_ALG_INTERPOLATE;
      srsran::console("Enabling subframe interpolation for TDD cells (recommended setting)\n");
    }

    cell_initiated = true;
  }
  return true;
}

uint32_t cc_worker::get_buffer_len()
{
  return signal_buffer_max_samples;
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

void cc_worker::set_cfo_nolock(float cfo)
{
  ue_ul_cfg.cfo_value = cfo;
}

float cc_worker::get_ref_cfo() const
{
  return ue_dl.chest_res.cfo;
}

void cc_worker::set_tdd_config_nolock(srsran_tdd_config_t config)
{
  sf_cfg_dl.tdd_config = config;
  sf_cfg_ul.tdd_config = config;
}

/************
 *
 * Downlink Functions
 *
 */

bool cc_worker::work_dl_regular()
{
  bool dl_ack[SRSRAN_MAX_CODEWORDS] = {};

  mac_interface_phy_lte::tb_action_dl_t dl_action = {};

  bool found_dl_grant = false;

  if (!cell_initiated) {
    logger.warning("Trying to access cc_worker=%d while cell not initialized (DL)", cc_idx);
    return false;
  }

  sf_cfg_dl.sf_type = SRSRAN_SF_NORM;

  // Set default channel estimation
  ue_dl_cfg.chest_cfg = chest_default_cfg;

  /* For TDD, when searching for SIB1, the ul/dl configuration is unknown and need to do blind search over
   * the possible mi values
   */
  uint32_t mi_set_len;
  if (cell.frame_type == SRSRAN_TDD && !sf_cfg_dl.tdd_config.configured) {
    mi_set_len = 3;
  } else {
    mi_set_len = 1;
  }

  // Blind search PHICH mi value
  for (uint32_t i = 0; i < mi_set_len && !found_dl_grant; i++) {
    if (mi_set_len == 1) {
      srsran_ue_dl_set_mi_auto(&ue_dl);
    } else {
      srsran_ue_dl_set_mi_manual(&ue_dl, i);
    }

    /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
    if (srsran_ue_dl_decode_fft_estimate(&ue_dl, &sf_cfg_dl, &ue_dl_cfg) < 0) {
      Error("Getting PDCCH FFT estimate");
      return false;
    }

    // Look for DL and UL dci(s) if the serving cell is active and it is NOT a secondary serving cell without
    // cross-carrier scheduling is enabled
    if (phy->cell_state.is_active(cc_idx, sf_cfg_dl.tti) and (cc_idx != 0 or not ue_dl_cfg.cfg.dci.cif_present)) {
      found_dl_grant = decode_pdcch_dl() > 0;
      decode_pdcch_ul();
    }
  }

  srsran_dci_dl_t dci_dl       = {};
  uint32_t        grant_cc_idx = 0;
  bool            has_dl_grant = phy->get_dl_pending_grant(CURRENT_TTI, cc_idx, &grant_cc_idx, &dci_dl);

  // If found a dci for this carrier, generate a grant, pass it to MAC and decode the associated PDSCH
  if (has_dl_grant) {
    // PDCCH order has no associated PDSCH to decode
    if (not dci_dl.is_pdcch_order) {
      // Read last TB from last retx for this pid
      for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
        ue_dl_cfg.cfg.pdsch.grant.last_tbs[i] = phy->last_dl_tbs[dci_dl.pid][cc_idx][i];
      }
      // Generate PHY grant
      if (srsran_ue_dl_dci_to_pdsch_grant(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, &dci_dl, &ue_dl_cfg.cfg.pdsch.grant)) {
        Info("Converting DCI message to DL dci");
        return false;
      }

      // Save TB for next retx
      for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
        phy->last_dl_tbs[dci_dl.pid][cc_idx][i] = ue_dl_cfg.cfg.pdsch.grant.last_tbs[i];
      }

      // Set RNTI
      ue_dl_cfg.cfg.pdsch.rnti = dci_dl.rnti;
    } else {
      ue_dl_cfg.cfg.pdsch.rnti            = dci_dl.rnti;
      ue_dl_cfg.cfg.pdsch.grant.tb[0].tbs = 0;
    }
    // Generate MAC grant
    mac_interface_phy_lte::mac_grant_dl_t mac_grant = {};
    dl_phy_to_mac_grant(&ue_dl_cfg.cfg.pdsch.grant, &dci_dl, &mac_grant);

    // Save ACK resource configuration
    srsran_pdsch_ack_resource_t ack_resource = {dci_dl.dai, dci_dl.location.ncce, grant_cc_idx, dci_dl.tpc_pucch};

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    phy->stack->new_grant_dl(cc_idx, mac_grant, &dl_action);

    // Decode PDSCH
    decode_pdsch(ack_resource, &dl_action, dl_ack);

    // Informs Stack about the decoding status, send NACK if cell is in process of re-selection
    if (phy->cell_is_selecting) {
      for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
        dl_ack[i] = false;
      }
    }
    phy->stack->tb_decoded(cc_idx, mac_grant, dl_ack);
  }

  /* Decode PHICH */
  decode_phich();

  return true;
}

bool cc_worker::work_dl_mbsfn(srsran_mbsfn_cfg_t mbsfn_cfg)
{
  mac_interface_phy_lte::tb_action_dl_t dl_action = {};

  if (!cell_initiated) {
    logger.warning("Trying to access cc_worker=%d while cell not initialized (MBSFN)", cc_idx);
    return false;
  }

  // Configure MBSFN settings
  srsran_ue_dl_set_mbsfn_area_id(&ue_dl, mbsfn_cfg.mbsfn_area_id);
  srsran_ue_dl_set_non_mbsfn_region(&ue_dl, mbsfn_cfg.non_mbsfn_region_length);

  sf_cfg_dl.sf_type = SRSRAN_SF_MBSFN;

  // Set MBSFN channel estimation
  chest_mbsfn_cfg.mbsfn_area_id = mbsfn_cfg.mbsfn_area_id;
  ue_dl_cfg.chest_cfg           = chest_mbsfn_cfg;

  /* Do FFT and extract PDCCH LLR, or quit if no actions are required in this subframe */
  if (srsran_ue_dl_decode_fft_estimate(&ue_dl, &sf_cfg_dl, &ue_dl_cfg) < 0) {
    Error("Getting PDCCH FFT estimate");
    return false;
  }

  // Look for DL and UL dci(s) if the serving cell is active and it is NOT a secondary serving cell without
  // cross-carrier scheduling is enabled
  if (phy->cell_state.is_active(cc_idx, sf_cfg_dl.tti) and (cc_idx != 0 or not ue_dl_cfg.cfg.dci.cif_present)) {
    decode_pdcch_dl();
    decode_pdcch_ul();
  }

  if (mbsfn_cfg.enable) {
    srsran_configure_pmch(&pmch_cfg, &cell, &mbsfn_cfg);
    srsran_ra_dl_compute_nof_re(&cell, &sf_cfg_dl, &pmch_cfg.pdsch_cfg.grant);

    // Send grant to MAC and get action for this TB, then call tb_decoded to unlock MAC
    new_mch_dl(&dl_action);
    bool mch_decoded = true;
    if (!decode_pmch(&dl_action, &mbsfn_cfg)) {
      mch_decoded = false;
    }
    phy->stack->mch_decoded((uint32_t)pmch_cfg.pdsch_cfg.grant.tb[0].tbs / 8, mch_decoded, mch_payload_buffer);
  } else if (mbsfn_cfg.is_mcch) {
    // release lock in phy_common
    phy->set_mch_period_stop(0);
  }

  /* Decode PHICH */
  decode_phich();

  return true;
}

void cc_worker::dl_phy_to_mac_grant(srsran_pdsch_grant_t*                         phy_grant,
                                    srsran_dci_dl_t*                              dl_dci,
                                    srsue::mac_interface_phy_lte::mac_grant_dl_t* mac_grant)
{
  /* Fill MAC dci structure */
  mac_grant->pid            = dl_dci->pid;
  mac_grant->rnti           = dl_dci->rnti;
  mac_grant->tti            = CURRENT_TTI;
  mac_grant->is_pdcch_order = dl_dci->is_pdcch_order;
  if (dl_dci->is_pdcch_order) {
    mac_grant->preamble_idx   = dl_dci->preamble_idx;
    mac_grant->prach_mask_idx = dl_dci->prach_mask_idx;
  }

  for (int i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    mac_grant->tb[i].ndi         = dl_dci->tb[i].ndi;
    mac_grant->tb[i].ndi_present = (dl_dci->tb[i].mcs_idx <= 28);
    mac_grant->tb[i].tbs         = phy_grant->tb[i].enabled ? (phy_grant->tb[i].tbs / (uint32_t)8) : 0;
    mac_grant->tb[i].rv          = phy_grant->tb[i].rv;
  }

  // If SIB dci, use PID to signal TTI to obtain RV from higher layers
  if (mac_grant->rnti == SRSRAN_SIRNTI) {
    mac_grant->pid = CURRENT_TTI;
  }
}

int cc_worker::decode_pdcch_dl()
{
  int nof_grants = 0;

  uint16_t dl_rnti = phy->stack->get_dl_sched_rnti(CURRENT_TTI);
  if (dl_rnti != SRSRAN_INVALID_RNTI) {
    srsran_dci_dl_t dci[SRSRAN_MAX_CARRIERS] = {};

    /* Blind search first without cross scheduling then with it if enabled */
    for (int i = 0; i < (ue_dl_cfg.cfg.dci.cif_present ? 2 : 1) && !nof_grants; i++) {
      Debug("PDCCH looking for rnti=0x%x", dl_rnti);
      ue_dl_cfg.cfg.dci.cif_enabled = i > 0;
      ue_dl_cfg.cfg.dci_common_ss   = (cc_idx == 0);
      nof_grants                    = srsran_ue_dl_find_dl_dci(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, dl_rnti, dci);
      if (nof_grants < 0) {
        Error("Looking for DL grants");
        return -1;
      }
    }

    // If RAR dci, save TTI
    if (nof_grants > 0 && SRSRAN_RNTI_ISRAR(dl_rnti)) {
      phy->set_rar_grant_tti(CURRENT_TTI);
    }

    for (int k = 0; k < nof_grants; k++) {
      // Save dci to CC index
      phy->set_dl_pending_grant(CURRENT_TTI, dci[k].cif_present ? dci[k].cif : cc_idx, cc_idx, &dci[k]);

      // Logging
      if (logger.info.enabled()) {
        char str[512];
        srsran_dci_dl_info(&dci[k], str, 512);
        logger.info("PDCCH: cc=%d, %s, snr=%.1f dB", cc_idx, str, ue_dl.chest_res.snr_db);
      }
    }
  }
  return nof_grants;
}

int cc_worker::decode_pdsch(srsran_pdsch_ack_resource_t            ack_resource,
                            mac_interface_phy_lte::tb_action_dl_t* action,
                            bool                                   mac_acks[SRSRAN_MAX_CODEWORDS])
{
  srsran_pdsch_res_t pdsch_dec[SRSRAN_MAX_CODEWORDS] = {};

  // See if at least 1 codeword needs to be decoded. If not need to be decode, resend ACK
  bool decode_enable                   = false;
  bool tb_enable[SRSRAN_MAX_CODEWORDS] = {};
  for (uint32_t tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
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
    if (srsran_ue_dl_decode_pdsch(&ue_dl, &sf_cfg_dl, &ue_dl_cfg.cfg.pdsch, pdsch_dec)) {
      Error("ERROR: Decoding PDSCH");
    }
  }

  // Generate ACKs for MAC and PUCCH
  uint32_t nof_tb                             = 0;
  uint8_t  pending_acks[SRSRAN_MAX_CODEWORDS] = {};
  for (uint32_t tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
    // For MAC, set to true if it's a duplicate
    mac_acks[tb] = action->tb[tb].enabled ? pdsch_dec[tb].crc : true;

    // For PUCCH feedback, need to send even if duplicate, but only those CW that were enabled before disabling in th
    // grant
    pending_acks[tb] = tb_enable[tb] ? mac_acks[tb] : 2;

    if (tb_enable[tb]) {
      nof_tb++;
    }
  }

  if (action->generate_ack && nof_tb > 0) {
    phy->set_dl_pending_ack(&sf_cfg_dl, cc_idx, pending_acks, ack_resource);
  }

  if (decode_enable) {
    // Metrics
    dl_metrics_t dl_metrics = {};
    if (ue_dl_cfg.cfg.pdsch.grant.nof_tb == 1) {
      dl_metrics.mcs = ue_dl_cfg.cfg.pdsch.grant.tb[0].mcs_idx;
    } else {
      dl_metrics.mcs = (ue_dl_cfg.cfg.pdsch.grant.tb[0].mcs_idx + ue_dl_cfg.cfg.pdsch.grant.tb[1].mcs_idx) / 2;
    }
    dl_metrics.fec_iters = pdsch_dec->avg_iterations_block / 2;
    phy->set_dl_metrics(cc_idx, dl_metrics);

    // Logging
    if (logger.info.enabled()) {
      char str[512];
      srsran_pdsch_rx_info(&ue_dl_cfg.cfg.pdsch, pdsch_dec, str, 512);
      logger.info("PDSCH: cc=%d, %s, snr=%.1f dB", cc_idx, str, ue_dl.chest_res.snr_db);
    }
  }

  return SRSRAN_SUCCESS;
}

int cc_worker::decode_pmch(mac_interface_phy_lte::tb_action_dl_t* action, srsran_mbsfn_cfg_t* mbsfn_cfg)
{
  srsran_pdsch_res_t pmch_dec = {};

  pmch_cfg.area_id                     = mbsfn_cfg->mbsfn_area_id;
  pmch_cfg.pdsch_cfg.softbuffers.rx[0] = action->tb[0].softbuffer.rx;
  pmch_dec.payload                     = action->tb[0].payload;

  if (action->tb[0].enabled) {
    srsran_softbuffer_rx_reset_tbs(pmch_cfg.pdsch_cfg.softbuffers.rx[0], pmch_cfg.pdsch_cfg.grant.tb[0].tbs);

    if (srsran_ue_dl_decode_pmch(&ue_dl, &sf_cfg_dl, &pmch_cfg, &pmch_dec)) {
      Error("Decoding PMCH");
      return -1;
    }

    // Store metrics
    // Metrics
    dl_metrics_t dl_metrics = {};
    dl_metrics.mcs          = ue_dl_cfg.cfg.pdsch.grant.tb[0].mcs_idx;
    dl_metrics.fec_iters    = pmch_dec.avg_iterations_block / 2;
    phy->set_dl_metrics(cc_idx, dl_metrics);

    Info("PMCH: l_crb=%2d, tbs=%d, mcs=%d, crc=%s, snr=%.1f dB, n_iter=%.1f",
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
    Warning("Received dci for TBS=0");
  }
  return 0;
}

void cc_worker::decode_phich()
{
  srsran_dci_ul_t      dci_ul      = {};
  srsran_phich_grant_t phich_grant = {};
  srsran_phich_res_t   phich_res   = {};

  // Receive PHICH, in TDD might be more than one
  for (uint32_t I_phich = 0; I_phich < 2; I_phich++) {
    phich_grant.I_phich = I_phich;
    if (phy->get_ul_pending_ack(&sf_cfg_dl, cc_idx, &phich_grant, &dci_ul)) {
      if (srsran_ue_dl_decode_phich(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, &phich_grant, &phich_res)) {
        Error("Decoding PHICH");
      }
      phy->set_ul_received_ack(&sf_cfg_dl, cc_idx, phich_res.ack_value, I_phich, &dci_ul);
      Info("PHICH: hi=%d, corr=%.1f, I_lowest=%d, n_dmrs=%d, I_phich=%d",
           phich_res.ack_value,
           phich_res.distance,
           phich_grant.n_prb_lowest,
           phich_grant.n_dmrs,
           I_phich);
    }
  }
}

void cc_worker::update_measurements(std::vector<phy_meas_t>& serving_cells, cf_t* rssi_power_buffer)
{
  // Do not update any measurement if the CC is not configured to prevent false or inaccurate data
  if (not phy->cell_state.is_configured(cc_idx)) {
    return;
  }

  phy->update_measurements(
      cc_idx, ue_dl.chest_res, sf_cfg_dl, ue_dl_cfg.cfg.pdsch.rs_power, serving_cells, rssi_power_buffer);
}

/************
 *
 * Uplink Functions
 *
 */

bool cc_worker::work_ul(srsran_uci_data_t* uci_data)
{
  bool signal_ready;

  srsran_dci_ul_t                       dci_ul       = {};
  mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant = {};
  mac_interface_phy_lte::tb_action_ul_t ul_action    = {};
  uint32_t                              pid          = 0;

  if (!cell_initiated) {
    logger.warning("Trying to access cc_worker=%d while cell not initialized (UL)", cc_idx);
    return false;
  }

  bool ul_grant_available = phy->get_ul_pending_grant(&sf_cfg_ul, cc_idx, &pid, &dci_ul);
  ul_mac_grant.phich_available =
      phy->get_ul_received_ack(&sf_cfg_ul, cc_idx, &ul_mac_grant.hi_value, ul_grant_available ? nullptr : &dci_ul);

  // If there is no grant, pid is from current TX TTI
  if (!ul_grant_available) {
    pid = phy->ul_pidof(CURRENT_TTI_TX, &sf_cfg_ul.tdd_config);
  }

  /*
   * Generate aperiodic CQI report if required, note that in case both aperiodic and periodic ones present, only
   * aperiodic is sent (36.213 section 7.2)
   */
  if (ul_grant_available and dci_ul.cqi_request and uci_data != nullptr) {
    set_uci_aperiodic_cqi(uci_data);
  }

  /* Send UL dci or HARQ information (from PHICH) to MAC and receive actions*/
  if (ul_grant_available || ul_mac_grant.phich_available) {
    // Read last TB info from last retx for this PID
    ue_ul_cfg.ul_cfg.pusch.grant.last_tb = phy->last_ul_tb[pid][cc_idx];

    // Generate PHY grant
    if (srsran_ue_ul_dci_to_pusch_grant(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &dci_ul, &ue_ul_cfg.ul_cfg.pusch.grant)) {
      if (logger.info.enabled()) {
        char str[128];
        srsran_dci_ul_info(&dci_ul, str, sizeof(str));
        Info("Converting DCI message to UL grant %s", str);
      }
      ul_grant_available = false;
    } else if (ue_ul_cfg.ul_cfg.pusch.grant.tb.mod == SRSRAN_MOD_BPSK) {
      Error("UL retransmission without valid stored grant.");
      ul_grant_available = false;
    } else {
      // Save TBS info for next retx
      phy->last_ul_tb[pid][cc_idx] = ue_ul_cfg.ul_cfg.pusch.grant.tb;

      // Fill MAC dci
      ul_phy_to_mac_grant(&ue_ul_cfg.ul_cfg.pusch.grant, &dci_ul, pid, ul_grant_available, &ul_mac_grant);

      phy->stack->new_grant_ul(cc_idx, ul_mac_grant, &ul_action);

      // Calculate PUSCH Hopping procedure
      ue_ul_cfg.ul_cfg.hopping.current_tx_nb = ul_action.current_tx_nb;
      srsran_ue_ul_pusch_hopping(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &ue_ul_cfg.ul_cfg.pusch.grant);
    }
  }

  // Set UL RNTI
  if (ul_grant_available || ul_mac_grant.phich_available) {
    ue_ul_cfg.ul_cfg.pusch.rnti = dci_ul.rnti;
  } else {
    ue_ul_cfg.ul_cfg.pucch.rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI_TX);
  }

  // PCell sends SR and ACK
  if (uci_data != nullptr) {
    set_uci_sr(uci_data);
    // This must be called after set_uci_sr() and set_uci_*_cqi
    set_uci_ack(uci_data, ul_grant_available, dci_ul.dai, ul_action.tb.enabled);
  }

  // Generate uplink signal, include uci data on only PCell
  signal_ready = encode_uplink(&ul_action, uci_data);

  // Prepare to receive ACK through PHICH
  if (ul_action.expect_ack) {
    srsran_phich_grant_t phich_grant = {};
    phich_grant.I_phich              = 0;
    if (cell.frame_type == SRSRAN_TDD && sf_cfg_ul.tdd_config.sf_config == 0) {
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

void cc_worker::ul_phy_to_mac_grant(srsran_pusch_grant_t*                         phy_grant,
                                    srsran_dci_ul_t*                              dci_ul,
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
  mac_grant->is_rar         = dci_ul->format == SRSRAN_DCI_FORMAT_RAR;
  mac_grant->tti_tx         = CURRENT_TTI_TX;
}

int cc_worker::decode_pdcch_ul()
{
  int nof_grants = 0;

  srsran_dci_ul_t dci[SRSRAN_MAX_CARRIERS];
  ZERO_OBJECT(dci);

  uint16_t ul_rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI);

  if (ul_rnti) {
    /* Blind search first without cross scheduling then with it if enabled */
    for (int i = 0; i < (ue_dl_cfg.cfg.dci.cif_present ? 2 : 1) && !nof_grants; i++) {
      ue_dl_cfg.cfg.dci.cif_enabled = i > 0;
      ue_dl_cfg.cfg.dci_common_ss   = (cc_idx == 0);
      nof_grants                    = srsran_ue_dl_find_ul_dci(&ue_dl, &sf_cfg_dl, &ue_dl_cfg, ul_rnti, dci);
      if (nof_grants < 0) {
        Error("Looking for UL grants");
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
      if (logger.info.enabled()) {
        char str[512];
        srsran_dci_ul_info(&dci[k], str, 512);
        logger.info("PDCCH: cc=%d, %s, snr=%.1f dB", cc_idx_grant, str, ue_dl.chest_res.snr_db);
      }
    }
  }

  return nof_grants;
}

bool cc_worker::encode_uplink(mac_interface_phy_lte::tb_action_ul_t* action, srsran_uci_data_t* uci_data)
{
  srsran_pusch_data_t data = {};
  ue_ul_cfg.cc_idx         = cc_idx;

  // Setup input data
  if (action != nullptr) {
    data.ptr                              = action->tb.payload;
    ue_ul_cfg.ul_cfg.pusch.softbuffers.tx = action->tb.softbuffer.tx;

    // Use RV from higher layers
    ue_ul_cfg.ul_cfg.pusch.grant.tb.rv = action->tb.rv;

    // Setup PUSCH grant
    ue_ul_cfg.grant_available = action->tb.enabled;
  }

  // Set UCI data and configuration
  if (uci_data != nullptr) {
    data.uci                       = uci_data->value;
    ue_ul_cfg.ul_cfg.pusch.uci_cfg = uci_data->cfg;
    ue_ul_cfg.ul_cfg.pucch.uci_cfg = uci_data->cfg;
  } else {
    ZERO_OBJECT(ue_ul_cfg.ul_cfg.pusch.uci_cfg);
    ZERO_OBJECT(ue_ul_cfg.ul_cfg.pucch.uci_cfg);
  }

  // Set UL RNTI
  ue_ul_cfg.ul_cfg.pucch.rnti = phy->stack->get_ul_sched_rnti(CURRENT_TTI_TX);

  // Check if the RNTI is valid. Early return without transmitting any signal if the RNTI is invalid.
  if (ue_ul_cfg.ul_cfg.pucch.rnti == SRSRAN_INVALID_RNTI) {
    return false;
  }

  // Encode signal
  int ret = srsran_ue_ul_encode(&ue_ul, &sf_cfg_ul, &ue_ul_cfg, &data);
  if (ret < 0) {
    Error("Encoding UL cc=%d", cc_idx);
  }

  // Store metrics
  if (action && action->tb.enabled) {
    ul_metrics_t ul_metrics = {};
    ul_metrics.mcs          = ue_ul_cfg.ul_cfg.pusch.grant.tb.mcs_idx;
    ul_metrics.power        = 0;
    phy->set_ul_metrics(cc_idx, ul_metrics);
  }

  // Logging
  if (logger.info.enabled()) {
    char str[512];
    if (srsran_ue_ul_info(&ue_ul_cfg, &sf_cfg_ul, &data.uci, str, 512)) {
      logger.info("%s", str);
    }
  }

  return ret > 0;
}

void cc_worker::set_uci_sr(srsran_uci_data_t* uci_data)
{
  Debug("set_uci_sr() query: sr_enabled=%d, last_tx_tti=%d", phy->sr.is_triggered(), phy->sr.get_last_tx_tti());
  if (srsran_ue_ul_gen_sr(&ue_ul_cfg, &sf_cfg_ul, uci_data, phy->sr.is_triggered())) {
    if (phy->sr.set_last_tx_tti(CURRENT_TTI_TX)) {
      Debug("set_uci_sr() sending SR: sr_enabled=true, last_tx_tti=%d", CURRENT_TTI_TX);
    }
  }
}

uint32_t cc_worker::get_wideband_cqi()
{
  int cqi_fixed = phy->args->cqi_fixed;
  int cqi_max   = phy->args->cqi_max;

  uint32_t wb_cqi_value = srsran_cqi_from_snr(phy->get_sinr_db(cc_idx) + ue_dl_cfg.snr_to_cqi_offset);

  if (cqi_fixed >= 0) {
    wb_cqi_value = cqi_fixed;
  } else if (cqi_max >= 0 && wb_cqi_value > (uint32_t)cqi_max) {
    wb_cqi_value = cqi_max;
  }

  return wb_cqi_value;
}

void cc_worker::set_uci_periodic_cqi(srsran_uci_data_t* uci_data)
{
  // Load last reported RI
  ue_dl_cfg.last_ri = phy->last_ri;

  srsran_ue_dl_gen_cqi_periodic(&ue_dl, &ue_dl_cfg, get_wideband_cqi(), CURRENT_TTI_TX, uci_data);

  // Store serving cell index for logging purposes
  uci_data->cfg.cqi.scell_index = cc_idx;

  // Store the reported RI
  phy->last_ri = ue_dl_cfg.last_ri;
}

void cc_worker::set_uci_aperiodic_cqi(srsran_uci_data_t* uci_data)
{
  if (ue_dl_cfg.cfg.cqi_report.aperiodic_configured) {
    srsran_ue_dl_gen_cqi_aperiodic(&ue_dl, &ue_dl_cfg, get_wideband_cqi(), uci_data);
  } else {
    Warning("Received CQI request but aperiodic mode is not configured");
  }
}

void cc_worker::set_uci_ack(srsran_uci_data_t* uci_data,
                            bool               is_grant_available,
                            uint32_t           V_dai_ul,
                            bool               is_pusch_available)
{
  srsran_pdsch_ack_t ack_info                = {};
  uint32_t           nof_configured_carriers = 0;

  // Only PCell generates ACK for all SCell
  for (uint32_t i = 0; i < phy->args->nof_lte_carriers; i++) {
    if (phy->cell_state.is_configured(i)) {
      phy->get_dl_pending_ack(&sf_cfg_ul, i, &ack_info.cc[i]);
      nof_configured_carriers++;
    }
  }

  // Configure ACK parameters
  ack_info.is_grant_available     = is_grant_available;
  ack_info.is_pusch_available     = is_pusch_available;
  ack_info.V_dai_ul               = V_dai_ul;
  ack_info.tdd_ack_multiplex      = ue_ul_cfg.ul_cfg.pucch.tdd_ack_multiplex;
  ack_info.simul_cqi_ack          = ue_ul_cfg.ul_cfg.pucch.simul_cqi_ack;
  ack_info.ack_nack_feedback_mode = ue_ul_cfg.ul_cfg.pucch.ack_nack_feedback_mode;
  ack_info.nof_cc                 = nof_configured_carriers;
  ack_info.transmission_mode      = ue_dl_cfg.cfg.tm;

  // Generate ACK/NACK bits
  srsran_ue_dl_gen_ack(&ue_dl.cell, &sf_cfg_dl, &ack_info, uci_data);
}

/************
 *
 * Configuration Functions
 *
 */

/* Translates RRC structs into PHY structs
 */
void cc_worker::set_config_nolock(const srsran::phy_cfg_t& phy_cfg)
{
  // Save configuration
  ue_dl_cfg.cfg    = phy_cfg.dl_cfg;
  ue_ul_cfg.ul_cfg = phy_cfg.ul_cfg;

  phy->set_pdsch_cfg(&ue_dl_cfg.cfg.pdsch);
}

void cc_worker::upd_config_dci_nolock(const srsran_dci_cfg_t& dci_cfg)
{
  ue_dl_cfg.cfg.dci = dci_cfg;
}

int cc_worker::read_ce_abs(float* ce_abs, uint32_t tx_antenna, uint32_t rx_antenna)
{
  uint32_t sz = (uint32_t)srsran_symbol_sz(cell.nof_prb);
  srsran_vec_f_zero(ce_abs, sz);
  int g = (sz - 12 * cell.nof_prb) / 2;
  srsran_vec_abs_dB_cf(ue_dl.chest_res.ce[tx_antenna][rx_antenna], -80, &ce_abs[g], SRSRAN_NRE * cell.nof_prb);
  return sz;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  memcpy(pdsch_d, ue_dl.pdsch.d[0], ue_dl_cfg.cfg.pdsch.grant.nof_re * sizeof(cf_t));
  return ue_dl_cfg.cfg.pdsch.grant.nof_re;
}

void cc_worker::new_mch_dl(mac_interface_phy_lte::tb_action_dl_t* action)
{
  action->generate_ack        = false;
  action->tb[0].enabled       = true;
  action->tb[0].payload       = mch_payload_buffer;
  action->tb[0].softbuffer.rx = &mch_softbuffer;
  srsran_softbuffer_rx_reset_cb(&mch_softbuffer, 1);
}

} // namespace lte
} // namespace srsue
