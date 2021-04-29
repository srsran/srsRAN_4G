/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/phy/nr/cc_worker.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/srsran.h"

namespace srsue {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslog::basic_logger& log, state* phy_state_) :
  cc_idx(cc_idx_), phy(phy_state_), logger(log)
{
  cf_t* rx_buffer_c[SRSRAN_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSRAN_SF_LEN_PRB(phy->args.dl.nof_max_prb) * 5;
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_rx_antennas; i++) {
    rx_buffer[i]   = srsran_vec_cf_malloc(buffer_sz);
    rx_buffer_c[i] = rx_buffer[i];
    tx_buffer[i]   = srsran_vec_cf_malloc(buffer_sz);
  }

  if (srsran_ue_dl_nr_init(&ue_dl, rx_buffer.data(), &phy_state_->args.dl) < SRSRAN_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srsran_ue_ul_nr_init(&ue_ul, tx_buffer[0], &phy_state_->args.ul) < SRSRAN_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }
}

cc_worker::~cc_worker()
{
  srsran_ue_dl_nr_free(&ue_dl);
  srsran_ue_ul_nr_free(&ue_ul);
  for (cf_t* p : rx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
  for (cf_t* p : tx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
}

bool cc_worker::update_cfg()
{
  if (srsran_ue_dl_nr_set_carrier(&ue_dl, &phy->cfg.carrier) < SRSRAN_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  if (srsran_ue_ul_nr_set_carrier(&ue_ul, &phy->cfg.carrier) < SRSRAN_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  srsran_dci_cfg_nr_t dci_cfg = phy->cfg.get_dci_cfg();
  if (srsran_ue_dl_nr_set_pdcch_config(&ue_dl, &phy->cfg.pdcch, &dci_cfg) < SRSRAN_SUCCESS) {
    logger.error("Error setting NR PDCCH configuration");
    return false;
  }

  configured = true;

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = tti;
  ul_slot_cfg.idx = TTI_TX(tti);
  logger.set_context(tti);
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

cf_t* cc_worker::get_tx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return tx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return buffer_sz;
}

void cc_worker::decode_pdcch_dl()
{
  std::array<srsran_dci_dl_nr_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> dci_rx = {};
  srsue::mac_interface_phy_nr::sched_rnti_t rnti = phy->stack->get_dl_sched_rnti_nr(dl_slot_cfg.idx);

  // Skip search if no valid RNTI is given
  if (rnti.id == SRSRAN_INVALID_RNTI) {
    return;
  }

  // Search for grants
  int n_dl =
      srsran_ue_dl_nr_find_dl_dci(&ue_dl, &dl_slot_cfg, rnti.id, rnti.type, dci_rx.data(), (uint32_t)dci_rx.size());
  if (n_dl < SRSRAN_SUCCESS) {
    logger.error("Error decoding DL NR-PDCCH for %s=0x%x", srsran_rnti_type_str(rnti.type), rnti.id);
    return;
  }

  // Iterate over all received grants
  for (int i = 0; i < n_dl; i++) {
    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srsran_dci_dl_nr_to_str(&ue_dl.dci, &dci_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue UL grants
    phy->set_dl_pending_grant(dl_slot_cfg, dci_rx[i]);
  }

  if (logger.debug.enabled()) {
    for (uint32_t i = 0; i < ue_dl.pdcch_info_count; i++) {
      const srsran_ue_dl_nr_pdcch_info_t* info = &ue_dl.pdcch_info[i];
      logger.debug("PDCCH: dci=%s, rnti=%x, crst_id=%d, ss_type=%d, ncce=%d, al=%d, EPRE=%+.2f, RSRP=%+.2f, corr=%.3f; "
                   "nof_bits=%d; crc=%s;",
                   srsran_dci_format_nr_string(info->dci_ctx.format),
                   info->dci_ctx.rnti,
                   info->dci_ctx.coreset_id,
                   info->dci_ctx.ss_type,
                   info->dci_ctx.location.ncce,
                   info->dci_ctx.location.L,
                   info->measure.epre_dBfs,
                   info->measure.rsrp_dBfs,
                   info->measure.norm_corr,
                   info->nof_bits,
                   info->result.crc ? "OK" : "KO");
    }
  }
}

void cc_worker::decode_pdcch_ul()
{
  std::array<srsran_dci_ul_nr_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> dci_rx = {};
  srsue::mac_interface_phy_nr::sched_rnti_t rnti = phy->stack->get_ul_sched_rnti_nr(ul_slot_cfg.idx);

  // Skip search if no valid RNTI is given
  if (rnti.id == SRSRAN_INVALID_RNTI) {
    return;
  }

  // Search for grants
  int n_dl =
      srsran_ue_dl_nr_find_ul_dci(&ue_dl, &dl_slot_cfg, rnti.id, rnti.type, dci_rx.data(), (uint32_t)dci_rx.size());
  if (n_dl < SRSRAN_SUCCESS) {
    logger.error("Error decoding UL NR-PDCCH");
    return;
  }

  // Iterate over all received grants
  for (int i = 0; i < n_dl; i++) {
    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srsran_dci_ul_nr_to_str(&ue_dl.dci, &dci_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue UL grants
    phy->set_ul_pending_grant(dl_slot_cfg.idx, dci_rx[i]);
  }
}

bool cc_worker::work_dl()
{
  // Do NOT process any DL if it is not configured
  if (not configured) {
    return true;
  }

  // Check if it is a DL slot, if not skip
  if (!srsran_tdd_nr_is_dl(&phy->cfg.tdd, 0, dl_slot_cfg.idx)) {
    return true;
  }

  // Run FFT
  srsran_ue_dl_nr_estimate_fft(&ue_dl, &dl_slot_cfg);

  // Decode PDCCH DL first
  decode_pdcch_dl();

  // Decode PDCCH UL after
  decode_pdcch_ul();

  // Get DL grant for this TTI, if available
  uint32_t                       pid          = 0;
  srsran_sch_cfg_nr_t            pdsch_cfg    = {};
  srsran_pdsch_ack_resource_nr_t ack_resource = {};
  if (phy->get_dl_pending_grant(dl_slot_cfg.idx, pdsch_cfg, ack_resource, pid)) {
    // Notify MAC about PDSCH grant
    mac_interface_phy_nr::tb_action_dl_t    dl_action    = {};
    mac_interface_phy_nr::mac_nr_grant_dl_t mac_dl_grant = {};
    mac_dl_grant.rnti                                    = pdsch_cfg.grant.rnti;
    mac_dl_grant.pid                                     = pid;
    mac_dl_grant.rv                                      = pdsch_cfg.grant.tb[0].rv;
    mac_dl_grant.ndi                                     = pdsch_cfg.grant.tb[0].ndi;
    mac_dl_grant.tbs                                     = pdsch_cfg.grant.tb[0].tbs / 8;
    mac_dl_grant.tti                                     = dl_slot_cfg.idx;
    phy->stack->new_grant_dl(0, mac_dl_grant, &dl_action);

    // Early stop if MAC says it doesn't need the TB
    if (not dl_action.tb.enabled) {
      logger.info("Decoding not required. Skipping PDSCH");
      return true;
    }

    // Get data buffer
    srsran::unique_byte_buffer_t data = srsran::make_byte_buffer();
    if (data == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return false;
    }
    data->N_bytes = pdsch_cfg.grant.tb[0].tbs / 8U;

    // Initialise PDSCH Result
    srsran_pdsch_res_nr_t pdsch_res     = {};
    pdsch_res.tb[0].payload             = data->msg;
    pdsch_cfg.grant.tb[0].softbuffer.rx = dl_action.tb.softbuffer;

    // Decode actual PDSCH transmission
    if (srsran_ue_dl_nr_decode_pdsch(&ue_dl, &dl_slot_cfg, &pdsch_cfg, &pdsch_res) < SRSRAN_SUCCESS) {
      ERROR("Error decoding PDSCH");
      return false;
    }

    // Logging
    if (logger.info.enabled()) {
      str_info_t str;
      srsran_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, &pdsch_res, str.data(), (uint32_t)str.size());

      if (logger.debug.enabled()) {
        str_extra_t str_extra;
        srsran_sch_cfg_nr_info(&pdsch_cfg, str_extra.data(), (uint32_t)str_extra.size());
        logger.info(pdsch_res.tb[0].payload,
                    pdsch_cfg.grant.tb[0].tbs / 8,
                    "PDSCH: cc=%d pid=%d %s\n%s",
                    cc_idx,
                    pid,
                    str.data(),
                    str_extra.data());
      } else {
        logger.info(
            pdsch_res.tb[0].payload, pdsch_cfg.grant.tb[0].tbs / 8, "PDSCH: cc=%d pid=%d %s", cc_idx, pid, str.data());
      }
    }

    // Enqueue PDSCH ACK information only if the RNTI is type C
    if (pdsch_cfg.grant.rnti_type == srsran_rnti_type_c) {
      phy->set_pending_ack(dl_slot_cfg.idx, ack_resource, pdsch_res.tb[0].crc);
    }

    // Notify MAC about PDSCH decoding result
    mac_interface_phy_nr::tb_action_dl_result_t mac_dl_result = {};
    mac_dl_result.ack                                         = pdsch_res.tb[0].crc;
    mac_dl_result.payload = mac_dl_result.ack ? std::move(data) : nullptr; // only pass data when successful
    phy->stack->tb_decoded(cc_idx, mac_dl_grant, std::move(mac_dl_result));

    if (pdsch_cfg.grant.rnti_type == srsran_rnti_type_ra) {
      phy->rar_grant_tti = dl_slot_cfg.idx;
    }

    if (pdsch_res.tb[0].crc) {
      // Generate DL metrics
      dl_metrics_t dl_m = {};
      dl_m.mcs          = pdsch_cfg.grant.tb[0].mcs;
      dl_m.fec_iters    = pdsch_res.tb[0].avg_iter;
      dl_m.evm          = pdsch_res.evm[0];
      phy->set_dl_metrics(dl_m);

      // Generate Synch metrics
      sync_metrics_t sync_m = {};
      sync_m.cfo            = ue_dl.chest.cfo;
      phy->set_sync_metrics(sync_m);

      // Generate channel metrics
      ch_metrics_t ch_m = {};
      ch_m.n            = ue_dl.chest.noise_estimate;
      ch_m.sinr         = ue_dl.chest.snr_db;
      ch_m.rsrp         = ue_dl.chest.rsrp_dbm;
      ch_m.sync_err     = ue_dl.chest.sync_error;
      phy->set_channel_metrics(ch_m);
    }
  }

  return true;
}

bool cc_worker::work_ul()
{
  // Check if it is a UL slot, if not skip
  if (!srsran_tdd_nr_is_ul(&phy->cfg.tdd, 0, ul_slot_cfg.idx)) {
    // No NR signal shall be transmitted
    srsran_vec_cf_zero(tx_buffer[0], ue_ul.ifft.sf_sz);
    return true;
  }

  srsran_uci_data_nr_t uci_data = {};
  uint32_t             pid      = 0;

  // Gather PDSCH ACK information
  srsran_pdsch_ack_nr_t pdsch_ack  = {};
  bool                  has_ul_ack = phy->get_pending_ack(ul_slot_cfg.idx, pdsch_ack);

  // Request grant to PHY state for this transmit TTI
  srsran_sch_cfg_nr_t pusch_cfg       = {};
  bool                has_pusch_grant = phy->get_ul_pending_grant(ul_slot_cfg.idx, pusch_cfg, pid);

  // If PDSCH UL ACK is available, load into UCI
  if (has_ul_ack) {
    pdsch_ack.use_pusch = has_pusch_grant;

    if (logger.debug.enabled()) {
      std::array<char, 512> str = {};
      if (srsran_ue_dl_nr_ack_info(&pdsch_ack, str.data(), (uint32_t)str.size()) > 0) {
        logger.debug("%s", str.data());
      }
    }

    if (srsran_ue_dl_nr_gen_ack(&phy->cfg.harq_ack, &pdsch_ack, &uci_data) < SRSRAN_SUCCESS) {
      ERROR("Filling UCI ACK bits");
      return false;
    }
  }

  // Add SR to UCI data only if there is no UL grant!
  if (!has_ul_ack) {
    phy->get_pending_sr(ul_slot_cfg.idx, uci_data);
  }

  // Add CSI reports to UCI data if available
  phy->get_periodic_csi(ul_slot_cfg.idx, uci_data);

  if (has_pusch_grant) {
    // Notify MAC about PUSCH found grant
    mac_interface_phy_nr::tb_action_ul_t    ul_action    = {};
    mac_interface_phy_nr::mac_nr_grant_ul_t mac_ul_grant = {};
    mac_ul_grant.pid                                     = pid;
    mac_ul_grant.rnti                                    = pusch_cfg.grant.rnti;
    mac_ul_grant.tti                                     = ul_slot_cfg.idx;
    mac_ul_grant.tbs                                     = pusch_cfg.grant.tb[0].tbs / 8;
    mac_ul_grant.ndi                                     = pusch_cfg.grant.tb[0].ndi;
    mac_ul_grant.rv                                      = pusch_cfg.grant.tb[0].rv;
    mac_ul_grant.is_rar_grant                            = (pusch_cfg.grant.rnti_type == srsran_rnti_type_ra);
    phy->stack->new_grant_ul(0, mac_ul_grant, &ul_action);

    // Don't process further if MAC can't provide PDU
    if (not ul_action.tb.enabled) {
      ERROR("No MAC PDU provided by MAC");
      return false;
    }

    // Set UCI configuration following procedures
    srsran_ra_ul_set_grant_uci_nr(&phy->cfg.carrier, &phy->cfg.pusch, &uci_data.cfg, &pusch_cfg);

    // Assigning MAC provided values to PUSCH config structs
    pusch_cfg.grant.tb[0].softbuffer.tx = ul_action.tb.softbuffer;

    // Setup data for encoding
    srsran_pusch_data_nr_t data = {};
    data.payload[0]             = ul_action.tb.payload->msg;
    data.uci                    = uci_data.value;

    // Encode PUSCH transmission
    if (srsran_ue_ul_nr_encode_pusch(&ue_ul, &ul_slot_cfg, &pusch_cfg, &data) < SRSRAN_SUCCESS) {
      ERROR("Encoding PUSCH");
      return false;
    }

    // PUSCH Logging
    if (logger.info.enabled()) {
      str_info_t str;
      srsran_ue_ul_nr_pusch_info(&ue_ul, &pusch_cfg, &data.uci, str.data(), str.size());

      if (logger.debug.enabled()) {
        str_extra_t str_extra;
        srsran_sch_cfg_nr_info(&pusch_cfg, str_extra.data(), (uint32_t)str_extra.size());
        logger.info(ul_action.tb.payload->msg,
                    pusch_cfg.grant.tb[0].tbs / 8,
                    "PUSCH: cc=%d pid=%d %s tti_tx=%d\n%s",
                    cc_idx,
                    pid,
                    str.data(),
                    ul_slot_cfg.idx,
                    str_extra.data());
      } else {
        logger.info(ul_action.tb.payload->msg,
                    pusch_cfg.grant.tb[0].tbs / 8,
                    "PUSCH: cc=%d pid=%d %s tti_tx=%d",
                    cc_idx,
                    pid,
                    str.data(),
                    ul_slot_cfg.idx);
      }
    }

    // Set metrics
    ul_metrics_t ul_m = {};
    ul_m.mcs          = pusch_cfg.grant.tb[0].mcs;
    ul_m.power        = srsran_convert_power_to_dB(srsran_vec_avg_power_cf(tx_buffer[0], ue_ul.ifft.sf_sz));
    phy->set_ul_metrics(ul_m);

  } else if (srsran_uci_nr_total_bits(&uci_data.cfg) > 0) {
    // Get PUCCH resource
    srsran_pucch_nr_resource_t resource = {};
    if (srsran_ra_ul_nr_pucch_resource(&phy->cfg.pucch, &uci_data.cfg, &resource) < SRSRAN_SUCCESS) {
      ERROR("Selecting PUCCH resource");
      return false;
    }

    // Encode PUCCH message
    if (srsran_ue_ul_nr_encode_pucch(&ue_ul, &ul_slot_cfg, &phy->cfg.pucch.common, &resource, &uci_data) <
        SRSRAN_SUCCESS) {
      ERROR("Encoding PUCCH");
      return false;
    }

    // PUCCH Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srsran_ue_ul_nr_pucch_info(&resource, &uci_data, str.data(), str.size());
      logger.info("PUCCH: cc=%d, %s, tti_tx=%d", cc_idx, str.data(), ul_slot_cfg.idx);
    }
  } else {
    // No NR signal shall be transmitted
    srsran_vec_cf_zero(tx_buffer[0], ue_ul.ifft.sf_sz);
  }

  return true;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  uint32_t nof_re = ue_dl.carrier.nof_prb * SRSRAN_NRE * 12;
  srsran_vec_cf_copy(pdsch_d, ue_dl.pdsch.d[0], nof_re);
  return nof_re;
}

} // namespace nr
} // namespace srsue
