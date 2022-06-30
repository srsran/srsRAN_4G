/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <string.h>

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
#include "srsran/common/string_helpers.h"
#include "srsran/srslog/bundled/fmt/ranges.h"

namespace srsenb {

/******************************************************
 *                 Helper Functions                   *
 ******************************************************/

#define MAC_MIN_ALLOC_SIZE 5

/*******************************************************
 *
 * Initialization and configuration functions
 *
 *******************************************************/

sched_ue::sched_ue(uint16_t rnti_, const std::vector<sched_cell_params_t>& cell_list_params_, const ue_cfg_t& cfg_) :
  logger(srslog::fetch_basic_logger("MAC")), rnti(rnti_), lch_handler(rnti_)
{
  cells.reserve(cell_list_params_.size());
  for (auto& c : cell_list_params_) {
    cells.emplace_back(rnti_, c, current_tti);
  }
  logger.info("SCHED: Added user rnti=0x%x", rnti);

  set_cfg(cfg_);
}

void sched_ue::set_cfg(const ue_cfg_t& cfg_)
{
  // for the first configured cc, set it as primary cc
  if (cfg.supported_cc_list.empty()) {
    uint32_t primary_cc_idx = 0;
    if (not cfg_.supported_cc_list.empty()) {
      primary_cc_idx = cfg_.supported_cc_list[0].enb_cc_idx;
    } else {
      logger.warning("Primary cc idx not provided in scheduler ue_cfg. Defaulting to cc_idx=0");
    }
    // setup primary cc
    main_cc_params = cells[primary_cc_idx].cell_cfg;
    cell           = main_cc_params->cfg.cell;
    max_msg3retx   = main_cc_params->cfg.maxharq_msg3tx;
  }

  // update configuration
  std::vector<sched::ue_cfg_t::cc_cfg_t> prev_supported_cc_list = std::move(cfg.supported_cc_list);
  cfg                                                           = cfg_;

  // update bearer cfgs
  lch_handler.set_cfg(cfg_);

  // update ue cells
  bool scell_activation_state_changed = false;
  for (auto& c : cells) {
    c.set_ue_cfg(cfg);
    scell_activation_state_changed |=
        c.is_scell() and (c.cc_state() == cc_st::activating or c.cc_state() == cc_st::deactivating);
  }
  bool is_handover = not prev_supported_cc_list.empty() and
                     prev_supported_cc_list[0].enb_cc_idx != cfg.supported_cc_list[0].enb_cc_idx;
  if (prev_supported_cc_list.empty() or is_handover) {
    logger.info("SCHED: rnti=0x%x PCell is now enb_cc_idx=%d", rnti, cfg.supported_cc_list[0].enb_cc_idx);
  }
  if (scell_activation_state_changed and not is_handover) {
    lch_handler.pending_ces.emplace_back(srsran::dl_sch_lcid::SCELL_ACTIVATION);
    logger.info("SCHED: Enqueueing SCell Activation CMD for rnti=0x%x", rnti);
  }

  check_ue_cfg_correctness(cfg);
}

void sched_ue::new_subframe(tti_point tti_rx, uint32_t enb_cc_idx)
{
  if (current_tti != tti_rx) {
    current_tti = tti_rx;
    lch_handler.new_tti();
    for (auto& cc : cells) {
      cc.new_tti(tti_rx);
    }
  }
}

/*******************************************************
 *
 * FAPI-like main scheduler interface.
 *
 *******************************************************/

void sched_ue::set_bearer_cfg(uint32_t lc_id, const mac_lc_ch_cfg_t& cfg_)
{
  cfg.ue_bearers[lc_id] = cfg_;
  lch_handler.config_lcid(lc_id, cfg_);
}

void sched_ue::rem_bearer(uint32_t lc_id)
{
  cfg.ue_bearers[lc_id] = mac_lc_ch_cfg_t{};
  lch_handler.config_lcid(lc_id, mac_lc_ch_cfg_t{});
}

void sched_ue::phy_config_enabled(tti_point tti_rx, bool enabled)
{
  phy_config_dedicated_enabled = enabled;
}

void sched_ue::ul_buffer_state(uint8_t lcg_id, uint32_t bsr)
{
  lch_handler.ul_bsr(lcg_id, bsr);
}

void sched_ue::ul_buffer_add(uint8_t lcid, uint32_t bytes)
{
  lch_handler.ul_buffer_add(lcid, bytes);
}

void sched_ue::ul_phr(int phr, uint32_t grant_nof_prb)
{
  cells[cfg.supported_cc_list[0].enb_cc_idx].tpc_fsm.set_phr(phr, grant_nof_prb);
}

void sched_ue::dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  lch_handler.dl_buffer_state(lc_id, tx_queue, retx_queue);
}

void sched_ue::mac_buffer_state(uint32_t ce_code, uint32_t nof_cmds)
{
  auto cmd = (lch_ue_manager::ce_cmd)ce_code;
  for (uint32_t i = 0; i < nof_cmds; ++i) {
    if (cmd == lch_ue_manager::ce_cmd::CON_RES_ID) {
      lch_handler.pending_ces.push_front(cmd);
    } else {
      lch_handler.pending_ces.push_back(cmd);
    }
  }
  logger.info("SCHED: %s for rnti=0x%x needs to be scheduled", to_string(cmd), rnti);
}

void sched_ue::set_sr()
{
  sr = true;
}

void sched_ue::unset_sr()
{
  sr = false;
}

void sched_ue::metrics_read(mac_ue_metrics_t& metrics)
{
  sched_ue_cell& pcell  = cells[cfg.supported_cc_list[0].enb_cc_idx];
  metrics.ul_snr_offset = pcell.get_ul_snr_offset();
  metrics.dl_cqi_offset = pcell.get_dl_cqi_offset();
}

tti_point prev_meas_gap_start(tti_point tti, uint32_t period, uint32_t offset)
{
  return tti_point{static_cast<uint32_t>(floor(static_cast<float>((tti - offset).to_uint()) / period)) * period +
                   offset};
}

tti_point next_meas_gap_start(tti_point tti, uint32_t period, uint32_t offset)
{
  return prev_meas_gap_start(tti, period, offset) + period;
}

tti_point nearest_meas_gap(tti_point tti, uint32_t period, uint32_t offset)
{
  return tti_point{static_cast<uint32_t>(round(static_cast<float>((tti - offset).to_uint()) / period)) * period +
                   offset};
}

bool sched_ue::pdsch_enabled(srsran::tti_point tti_rx, uint32_t enb_cc_idx) const
{
  if (cfg.supported_cc_list[0].enb_cc_idx != enb_cc_idx) {
    return true;
  }

  // Check measGap collision
  if (cfg.measgap_period > 0) {
    tti_point    tti_tx_dl = to_tx_dl(tti_rx), tti_tx_dl_ack = to_tx_dl_ack(tti_rx);
    tti_point    mgap_tti = nearest_meas_gap(tti_tx_dl, cfg.measgap_period, cfg.measgap_offset);
    tti_interval meas_gap{mgap_tti, mgap_tti + 6};

    // disable TTIs that lead to PDCCH/PDSCH or respective ACKs to fall in measGap
    if (meas_gap.contains(tti_tx_dl) or meas_gap.contains(tti_tx_dl_ack)) {
      return false;
    }
  }
  return true;
}

bool sched_ue::pusch_enabled(tti_point tti_rx, uint32_t enb_cc_idx, bool needs_pdcch) const
{
  if (cfg.supported_cc_list[0].enb_cc_idx != enb_cc_idx) {
    return true;
  }

  // Check measGap collision
  if (cfg.measgap_period > 0) {
    tti_point    tti_tx_ul = to_tx_ul(tti_rx);
    tti_point    mgap_tti  = nearest_meas_gap(tti_tx_ul, cfg.measgap_period, cfg.measgap_offset);
    tti_interval meas_gap{mgap_tti, mgap_tti + 6};

    // disable TTIs that leads to PUSCH tx or PHICH rx falling in measGap
    if (meas_gap.contains(tti_tx_ul)) {
      return false;
    }
    // disable TTIs which respective PDCCH falls in measGap (in case PDCCH is needed)
    if (needs_pdcch and meas_gap.contains(to_tx_dl(tti_rx))) {
      return false;
    }
  }
  return true;
}

bool sched_ue::phich_enabled(tti_point tti_rx, uint32_t enb_cc_idx) const
{
  if (cfg.supported_cc_list[0].enb_cc_idx != enb_cc_idx) {
    return true;
  }

  // Check measGap collision with PHICH
  if (cfg.measgap_period > 0) {
    tti_point    tti_tx_dl = to_tx_dl(tti_rx);
    tti_point    mgap_tti  = nearest_meas_gap(tti_tx_dl, cfg.measgap_period, cfg.measgap_offset);
    tti_interval meas_gap{mgap_tti, mgap_tti + 6};
    if (meas_gap.contains(tti_tx_dl)) {
      return false;
    }
  }
  return true;
}

int sched_ue::set_ack_info(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  return cells[enb_cc_idx].set_ack_info(tti_rx, tb_idx, ack);
}

void sched_ue::set_ul_crc(tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res)
{
  cells[enb_cc_idx].set_ul_crc(tti_rx, crc_res);
}

void sched_ue::set_dl_ri(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t ri)
{
  if (cells[enb_cc_idx].cc_state() != cc_st::idle) {
    cells[enb_cc_idx].dl_ri        = ri;
    cells[enb_cc_idx].dl_ri_tti_rx = tti_rx;
  } else {
    logger.warning("Received DL RI for invalid cell index %d", enb_cc_idx);
  }
}

void sched_ue::set_dl_pmi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t pmi)
{
  if (cells[enb_cc_idx].cc_state() != cc_st::idle) {
    cells[enb_cc_idx].dl_pmi        = pmi;
    cells[enb_cc_idx].dl_pmi_tti_rx = tti_rx;
  } else {
    logger.warning("Received DL PMI for invalid cell index %d", enb_cc_idx);
  }
}

void sched_ue::set_dl_cqi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t cqi)
{
  cells[enb_cc_idx].set_dl_wb_cqi(tti_rx, cqi);
}

void sched_ue::set_dl_sb_cqi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t sb_idx, uint32_t cqi)
{
  cells[enb_cc_idx].set_dl_sb_cqi(tti_rx, sb_idx, cqi);
}

void sched_ue::set_ul_snr(tti_point tti_rx, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code)
{
  cells[enb_cc_idx].set_ul_snr(tti_rx, snr, ul_ch_code);
}

/*******************************************************
 *
 * Functions used to generate DCI grants
 *
 *******************************************************/

/**
 * Allocate MAC PDU for a UE HARQ pid
 * @return pair with allocated tbs and mcs
 */
tbs_info sched_ue::allocate_new_dl_mac_pdu(sched::dl_sched_data_t* data,
                                           dl_harq_proc*           h,
                                           const rbgmask_t&        user_mask,
                                           tti_point               tti_tx_dl,
                                           uint32_t                enb_cc_idx,
                                           uint32_t                cfi,
                                           uint32_t                tb)
{
  srsran_dci_dl_t* dci     = &data->dci;
  tbs_info         tb_info = compute_mcs_and_tbs(enb_cc_idx, tti_tx_dl, user_mask, cfi, *dci);

  // Allocate MAC PDU (subheaders, CEs, and SDUS)
  int rem_tbs = tb_info.tbs_bytes;
  if (cells[enb_cc_idx].is_pcell()) {
    rem_tbs -= allocate_mac_ces(data, lch_handler, rem_tbs);
  }
  rem_tbs -= allocate_mac_sdus(data, lch_handler, rem_tbs, tb);

  // Allocate DL UE Harq
  if (rem_tbs != tb_info.tbs_bytes) {
    h->new_tx(
        user_mask, tb, tti_tx_dl, tb_info.mcs, tb_info.tbs_bytes, data->dci.location.ncce, get_ue_cfg().maxharq_tx);
  } else {
    // Note: At this point, the allocation of bytes to a TB should not fail, unless the RLC buffers have been
    //       emptied by another allocated tb_idx.
    uint32_t pending_bytes = lch_handler.get_dl_tx_total();
    if (pending_bytes > 0) {
      logger.warning("SCHED: Failed to allocate DL TB with tb_idx=%d, tbs=%d, pid=%d. Pending DL buffer data=%d",
                     tb,
                     rem_tbs,
                     h->get_id(),
                     pending_bytes);
    } else {
      logger.info("SCHED: DL TB tb_idx=%d, tbs=%d, pid=%d did not get allocated.", tb, rem_tbs, h->get_id());
    }
    tb_info.tbs_bytes = 0;
    tb_info.mcs       = 0;
  }

  return tb_info;
}

int sched_ue::generate_dl_dci_format(uint32_t                          pid,
                                     sched_interface::dl_sched_data_t* data,
                                     tti_point                         tti_tx_dl,
                                     uint32_t                          enb_cc_idx,
                                     uint32_t                          cfi,
                                     const rbgmask_t&                  user_mask)
{
  srsran_dci_format_t dci_format = get_dci_format();
  int                 tbs_bytes  = 0;

  // Set common DCI fields
  srsran_dci_dl_t* dci = &data->dci;
  dci->rnti            = rnti;
  dci->pid             = pid;
  dci->ue_cc_idx       = cells[enb_cc_idx].get_ue_cc_idx();
  dci->format          = dci_format;

  switch (dci_format) {
    case SRSRAN_DCI_FORMAT1A:
      tbs_bytes = generate_format1a(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
      break;
    case SRSRAN_DCI_FORMAT1:
      tbs_bytes = generate_format1(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
      break;
    case SRSRAN_DCI_FORMAT2:
      tbs_bytes = generate_format2(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
      break;
    case SRSRAN_DCI_FORMAT2A:
      tbs_bytes = generate_format2a(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
      break;
    default:
      logger.error("DCI format (%d) not implemented", dci_format);
  }

  // If allocation successful, encode TPC
  if (tbs_bytes > 0) {
    dci->tpc_pucch = cells[enb_cc_idx].tpc_fsm.encode_pucch_tpc();
  }

  return tbs_bytes;
}

int sched_ue::generate_format1a(uint32_t                          pid,
                                sched_interface::dl_sched_data_t* data,
                                tti_point                         tti_tx_dl,
                                uint32_t                          enb_cc_idx,
                                uint32_t                          cfi,
                                const rbgmask_t&                  user_mask)
{
  srsran_dci_dl_t* dci = &data->dci;

  dci->alloc_type       = SRSRAN_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = srsran_ra_type2_t::SRSRAN_RA_TYPE2_LOC;
  rbg_interval rbg_int  = rbg_interval::find_first_interval(user_mask);
  prb_interval prb_int  = prb_interval::rbgs_to_prbs(rbg_int, cell.nof_prb);
  uint32_t     L_crb    = prb_int.length();
  uint32_t     RB_start = prb_int.start();
  dci->type2_alloc.riv  = srsran_ra_type2_to_riv(L_crb, RB_start, cell.nof_prb);
  if (L_crb != count_prb_per_tb(user_mask)) {
    // This happens if Type0 was using distributed allocation
    logger.warning("SCHED: Can't use distributed RA due to DCI size ambiguity");
  }

  return generate_format1_common(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
}

int sched_ue::generate_format1_common(uint32_t                          pid,
                                      sched_interface::dl_sched_data_t* data,
                                      tti_point                         tti_tx_dl,
                                      uint32_t                          enb_cc_idx,
                                      uint32_t                          cfi,
                                      const rbgmask_t&                  user_mask)
{
  dl_harq_proc*    h   = &cells[enb_cc_idx].harq_ent.dl_harq_procs()[pid];
  srsran_dci_dl_t* dci = &data->dci;

  tbs_info tbinfo;
  if (h->is_empty(0)) {
    tbinfo = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, enb_cc_idx, cfi, 0);
  } else {
    h->new_retx(user_mask, 0, tti_tx_dl, &tbinfo.mcs, &tbinfo.tbs_bytes, dci->location.ncce);
    logger.debug("SCHED: Alloc format1 previous mcs=%d, tbs=%d", tbinfo.mcs, tbinfo.tbs_bytes);
  }

  if (tbinfo.tbs_bytes > 0) {
    dci->tb[0].mcs_idx = (uint32_t)tbinfo.mcs;
    dci->tb[0].rv      = get_rvidx(h->nof_retx(0));
    dci->tb[0].ndi     = h->get_ndi(0);

    data->tbs[0] = (uint32_t)tbinfo.tbs_bytes;
    data->tbs[1] = 0;
  }
  return tbinfo.tbs_bytes;
}

int sched_ue::generate_format1(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               tti_point                         tti_tx_dl,
                               uint32_t                          enb_cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  srsran_dci_dl_t* dci = &data->dci;

  dci->alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  return generate_format1_common(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);
}

/**
 * Based on the amount of tx data, allocated PRBs, DCI params, etc. compute a valid MCS and resulting TBS
 * @param enb_cc_idx user carrier index
 * @param tti_tx_dl tti when the tx will occur
 * @param rbgs RBG mask
 * @param cfi Number of control symbols in Subframe
 * @param dci contains the RBG mask, and alloc type
 * @return pair with MCS and TBS (in bytes)
 */
tbs_info sched_ue::compute_mcs_and_tbs(uint32_t               enb_cc_idx,
                                       tti_point              tti_tx_dl,
                                       const rbgmask_t&       rbg_mask,
                                       uint32_t               cfi,
                                       const srsran_dci_dl_t& dci)
{
  srsran_assert(cells[enb_cc_idx].configured(), "computation of MCS/TBS called for non-configured CC");
  srsran::interval<uint32_t> req_bytes = get_requested_dl_bytes(enb_cc_idx);

  // Calculate exact number of RE for this PRB allocation
  uint32_t nof_re = cells[enb_cc_idx].cell_cfg->get_dl_nof_res(tti_tx_dl, dci, cfi);

  // Compute MCS+TBS
  tbs_info tb = cqi_to_tbs_dl(cells[enb_cc_idx], rbg_mask, nof_re, dci.format, req_bytes.stop());

  if (tb.tbs_bytes > 0 and tb.tbs_bytes < (int)req_bytes.start()) {
    logger.info("SCHED: Could not get PRB allocation that avoids MAC CE or RLC SRB0 PDU segmentation");
    // Note: This is not a warning, because the srb0 buffer can be updated after the ue sched decision
  }

  return tb;
}

// Generates a Format2a dci
int sched_ue::generate_format2a(uint32_t                          pid,
                                sched_interface::dl_sched_data_t* data,
                                tti_point                         tti_tx_dl,
                                uint32_t                          enb_cc_idx,
                                uint32_t                          cfi,
                                const rbgmask_t&                  user_mask)
{
  dl_harq_proc* h                    = &cells[enb_cc_idx].harq_ent.dl_harq_procs()[pid];
  bool          tb_en[SRSRAN_MAX_TB] = {false};

  srsran_dci_dl_t* dci         = &data->dci;
  dci->alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  bool no_retx = true;

  if (cells[enb_cc_idx].dl_ri == 0) {
    if (h->is_empty(1)) {
      /* One layer, tb1 buffer is empty, send tb0 only */
      tb_en[0] = true;
    } else {
      /* One layer, tb1 buffer is not empty, send tb1 only */
      tb_en[1] = true;
    }
  } else {
    /* Two layers, retransmit what TBs that have not been Acknowledged */
    for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
      if (!h->is_empty(tb)) {
        tb_en[tb] = true;
        no_retx   = false;
      }
    }
    /* Two layers, no retransmissions...  */
    if (no_retx) {
      tb_en[0] = true;
      tb_en[1] = true;
    }
  }

  for (uint32_t tb = 0; tb < SRSRAN_MAX_TB; tb++) {
    tbs_info tbinfo;

    if (!h->is_empty(tb)) {
      h->new_retx(user_mask, tb, tti_tx_dl, &tbinfo.mcs, &tbinfo.tbs_bytes, data->dci.location.ncce);
    } else if (tb_en[tb] && no_retx) {
      tbinfo = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, enb_cc_idx, cfi, tb);
    }

    /* Fill DCI TB dedicated fields */
    if (tbinfo.tbs_bytes > 0 && tb_en[tb]) {
      dci->tb[tb].mcs_idx = (uint32_t)tbinfo.mcs;
      dci->tb[tb].rv      = get_rvidx(h->nof_retx(tb));
      if (!SRSRAN_DCI_IS_TB_EN(dci->tb[tb])) {
        dci->tb[tb].rv = 2;
      }
      dci->tb[tb].ndi    = h->get_ndi(tb);
      dci->tb[tb].cw_idx = tb;
      data->tbs[tb]      = (uint32_t)tbinfo.tbs_bytes;
    } else {
      SRSRAN_DCI_TB_DISABLE(dci->tb[tb]);
      data->tbs[tb] = 0;
    }
  }

  int ret = data->tbs[0] + data->tbs[1];
  return ret;
}

// Generates a Format2 dci
int sched_ue::generate_format2(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               tti_point                         tti_tx_dl,
                               uint32_t                          enb_cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  /* Call Format 2a (common) */
  int ret = generate_format2a(pid, data, tti_tx_dl, enb_cc_idx, cfi, user_mask);

  /* Compute precoding information */
  if ((SRSRAN_DCI_IS_TB_EN(data->dci.tb[0]) + SRSRAN_DCI_IS_TB_EN(data->dci.tb[1])) == 1) {
    data->dci.pinfo = (uint8_t)(cells[enb_cc_idx].dl_pmi + 1) % (uint8_t)5;
  } else {
    data->dci.pinfo = (uint8_t)(cells[enb_cc_idx].dl_pmi & 1u);
  }

  return ret;
}

int sched_ue::generate_format0(sched_interface::ul_sched_data_t* data,
                               tti_point                         tti_tx_ul,
                               uint32_t                          enb_cc_idx,
                               prb_interval                      alloc,
                               bool                              needs_pdcch,
                               srsran_dci_location_t             dci_pos,
                               int                               explicit_mcs,
                               uci_pusch_t                       uci_type)
{
  ul_harq_proc*    h   = get_ul_harq(tti_tx_ul, enb_cc_idx);
  srsran_dci_ul_t* dci = &data->dci;

  bool cqi_request = needs_cqi(tti_tx_ul.to_uint(), enb_cc_idx, true);

  // Set DCI position
  data->needs_pdcch = needs_pdcch;
  dci->location     = dci_pos;

  tbs_info tbinfo;
  tbinfo.mcs       = (explicit_mcs >= 0) ? explicit_mcs : cells[enb_cc_idx].fixed_mcs_ul;
  tbinfo.tbs_bytes = 0;

  bool is_newtx = h->is_empty(0);
  if (is_newtx) {
    if (tbinfo.mcs >= 0) {
      tbinfo.tbs_bytes = get_tbs_bytes(tbinfo.mcs, alloc.length(), false, true);
    } else {
      // dynamic mcs
      uint32_t req_bytes = get_pending_ul_new_data(tti_tx_ul, enb_cc_idx);
      uint32_t N_srs     = 0;
      uint32_t nof_symb  = 2 * (SRSRAN_CP_NSYMB(cell.cp) - 1) - N_srs;
      uint32_t nof_re    = nof_symb * alloc.length() * SRSRAN_NRE;
      tbinfo             = cqi_to_tbs_ul(cells[enb_cc_idx], alloc.length(), nof_re, req_bytes);

      // Reduce MCS to fit UCI if transmitted in this grant
      if (uci_type != UCI_PUSCH_NONE) {
        // Calculate an approximation of the number of RE used by UCI
        uint32_t nof_uci_re = 0;
        // Add the RE for ACK
        if (uci_type == UCI_PUSCH_ACK || uci_type == UCI_PUSCH_ACK_CQI) {
          float beta = srsran_sch_beta_ack(cfg.uci_offset.I_offset_ack);
          nof_uci_re +=
              srsran_qprime_ack_ext(alloc.length(), nof_symb, 8 * tbinfo.tbs_bytes, cfg.supported_cc_list.size(), beta);
        }
        // Add the RE for CQI report (RI reports are transmitted on CQI slots. We do a conservative estimate here)
        if (uci_type == UCI_PUSCH_CQI || uci_type == UCI_PUSCH_ACK_CQI || cqi_request) {
          float beta = srsran_sch_beta_cqi(cfg.uci_offset.I_offset_cqi);
          nof_uci_re += srsran_qprime_cqi_ext(alloc.length(), nof_symb, 8 * tbinfo.tbs_bytes, beta);
        }
        // Recompute again the MCS and TBS with the new spectral efficiency (based on the available RE for data)
        if (nof_re >= nof_uci_re) {
          tbinfo = cqi_to_tbs_ul(cells[enb_cc_idx], alloc.length(), nof_re - nof_uci_re, req_bytes);
        }
        // NOTE: if (nof_re < nof_uci_re) we should set TBS=0
      }
    }
    // If Msg3 set different nof retx
    uint32_t nof_retx = (data->needs_pdcch) ? get_max_retx() : max_msg3retx;
    h->new_tx(tti_tx_ul, tbinfo.mcs, tbinfo.tbs_bytes, alloc, nof_retx, not data->needs_pdcch);
    // Un-trigger the SR if data is allocated
    if (tbinfo.tbs_bytes > 0) {
      unset_sr();
    }
  } else {
    // retx
    h->new_retx(tti_tx_ul, &tbinfo.mcs, nullptr, alloc);
    tbinfo.tbs_bytes = get_tbs_bytes(tbinfo.mcs, alloc.length(), false, true);
  }

  if (tbinfo.tbs_bytes >= 0) {
    data->tbs           = tbinfo.tbs_bytes;
    data->current_tx_nb = h->nof_retx(0);
    dci->rnti           = rnti;
    dci->format         = SRSRAN_DCI_FORMAT0;
    dci->ue_cc_idx      = cells[enb_cc_idx].get_ue_cc_idx();
    dci->tb.ndi         = h->get_ndi(0);
    dci->cqi_request    = cqi_request;
    dci->freq_hop_fl    = srsran_dci_ul_t::SRSRAN_RA_PUSCH_HOP_DISABLED;
    dci->tpc_pusch      = cells[enb_cc_idx].tpc_fsm.encode_pusch_tpc();

    dci->type2_alloc.riv = srsran_ra_type2_to_riv(alloc.length(), alloc.start(), cell.nof_prb);

    // If there are no RE available for ULSCH but there is UCI to transmit, allocate PUSCH becuase
    // resources have been reserved already and in CA it will be used to ACK other carriers
    if (tbinfo.tbs_bytes == 0 && (cqi_request || uci_type != UCI_PUSCH_NONE)) {
      // 8.6.1 and 8.6.2 36.213 second paragraph
      dci->cqi_request = true;
      dci->tb.mcs_idx  = 29;
      dci->tb.rv       = 0; // No data is being transmitted

      // Empty TBS PUSCH only accepts a maximum of 4 PRB. Resize the grant. This doesn't affect the MCS selection
      // because there is no TB in this grant
      if (alloc.length() > 4) {
        alloc.set(alloc.start(), alloc.start() + 4);
      }
    } else if (tbinfo.tbs_bytes > 0) {
      dci->tb.rv = get_rvidx(h->nof_retx(0));
      if (!is_newtx && data->needs_pdcch) {
        dci->tb.mcs_idx = 28 + dci->tb.rv;
      } else {
        dci->tb.mcs_idx = tbinfo.mcs;
      }
    } else if (tbinfo.tbs_bytes == 0) {
      logger.warning("SCHED: No space for ULSCH while allocating format0. Discarding grant.");
    } else {
      logger.error("SCHED: Unkown error while allocating format0");
    }
  }

  return tbinfo.tbs_bytes;
}

/*******************************************************
 *
 * Functions used by scheduler or scheduler metric objects
 *
 *******************************************************/

uint32_t sched_ue::get_max_retx()
{
  return cfg.maxharq_tx;
}

bool sched_ue::needs_cqi(uint32_t tti, uint32_t enb_cc_idx, bool will_send)
{
  bool ret = false;
  if (phy_config_dedicated_enabled && cfg.supported_cc_list[0].aperiodic_cqi_period &&
      lch_handler.has_pending_dl_txs()) {
    bool needscqi = tti_point(tti) >=
                    cells[enb_cc_idx].dl_cqi().last_cqi_info_tti() - cfg.supported_cc_list[0].aperiodic_cqi_period;
    if (needscqi) {
      uint32_t interval_sent = srsran_tti_interval(tti, cqi_request_tti);
      if (interval_sent >= 16) {
        if (will_send) {
          cqi_request_tti = tti;
        }
        logger.debug("SCHED: Needs_cqi, last_sent=%d, will_be_sent=%d", cqi_request_tti, will_send);
        ret = true;
      }
    }
  }
  return ret;
}

/**
 * Compute the range of RBGs that avoids segmentation of TM and MAC subheader data. Always computed for highest CFI
 * @param enb_cc_idx carrier of the UE
 * @return range of number of RBGs that a UE can allocate in a given subframe
 */
rbg_interval sched_ue::get_required_dl_rbgs(uint32_t enb_cc_idx)
{
  assert(cells[enb_cc_idx].configured());
  const auto*                cellparams = cells[enb_cc_idx].cell_cfg;
  srsran::interval<uint32_t> req_bytes  = get_requested_dl_bytes(enb_cc_idx);
  if (req_bytes == srsran::interval<uint32_t>{0, 0}) {
    return {0, 0};
  }
  int pending_prbs = get_required_prb_dl(cells[enb_cc_idx], to_tx_dl(current_tti), get_dci_format(), req_bytes.start());
  if (pending_prbs < 0) {
    // Cannot fit allocation in given PRBs
    logger.error("SCHED: DL CQI=%d does now allow fitting %d non-segmentable DL tx bytes into the cell bandwidth. "
                 "Consider increasing initial CQI value.",
                 cells[enb_cc_idx].get_dl_cqi(),
                 req_bytes.start());
    return {cellparams->nof_prb(), cellparams->nof_prb()};
  }
  uint32_t min_pending_rbg = cellparams->nof_prbs_to_rbgs(pending_prbs);
  pending_prbs = get_required_prb_dl(cells[enb_cc_idx], to_tx_dl(current_tti), get_dci_format(), req_bytes.stop());
  pending_prbs = (pending_prbs < 0) ? cellparams->nof_prb() : pending_prbs;
  uint32_t max_pending_rbg = cellparams->nof_prbs_to_rbgs(pending_prbs);
  return {min_pending_rbg, max_pending_rbg};
}

uint32_t sched_ue::get_pending_dl_bytes(uint32_t enb_cc_idx)
{
  return get_requested_dl_bytes(enb_cc_idx).stop();
}

/**
 * Returns the range (min,max) of possible MAC PDU sizes.
 * - the lower boundary value is set based on the following conditions:
 *   - if there is data in SRB0, the min value is the sum of:
 *     - SRB0 RLC data (Msg4) including MAC subheader and payload (no segmentation)
 *     - ConRes CE + MAC subheader (7 bytes)
 *   - elif there is data in other RBs, the min value is either:
 *     - first pending CE (subheader+CE payload) in queue, if it exists and we are in PCell. Or,
 *     - one subheader (2B) + one RLC header (<=3B) to allow one MAC PDU alloc
 * - the upper boundary is set as a sum of:
 *   - total data in all SRBs and DRBs including the MAC subheaders
 *   - All CEs (ConRes and others) including respective MAC subheaders
 * @enb_cc_idx carrier where allocation is being made
 * @return
 */
srsran::interval<uint32_t> sched_ue::get_requested_dl_bytes(uint32_t enb_cc_idx)
{
  assert(cells.at(enb_cc_idx).configured());

  /* Set Maximum boundary */
  if (cells[enb_cc_idx].cc_state() != cc_st::active) {
    return {};
  }

  uint32_t max_data = 0, min_data = 0;
  uint32_t srb0_data = 0, rb_data = 0, sum_ce_data = 0;

  srb0_data = lch_handler.get_dl_tx_total_with_overhead(0);
  // Add pending CEs
  if (cells[enb_cc_idx].is_pcell()) {
    if (srb0_data == 0 and not lch_handler.pending_ces.empty() and
        lch_handler.pending_ces.front() == srsran::dl_sch_lcid::CON_RES_ID) {
      // Wait for SRB0 data to be available for Msg4 before scheduling the ConRes CE
      return {};
    }
    for (const lch_ue_manager::ce_cmd& ce : lch_handler.pending_ces) {
      sum_ce_data += srsran::ce_total_size(ce);
    }
  }
  // Add pending data in remaining RLC buffers
  for (int i = 1; i < sched_interface::MAX_LC; i++) {
    rb_data += lch_handler.get_dl_tx_total_with_overhead(i);
  }
  max_data = srb0_data + sum_ce_data + rb_data;

  /* Set Minimum boundary */
  min_data = srb0_data;
  if (not lch_handler.pending_ces.empty() and lch_handler.pending_ces.front() == lch_ue_manager::ce_cmd::CON_RES_ID) {
    min_data += srsran::ce_total_size(lch_handler.pending_ces.front());
  }
  if (min_data == 0) {
    if (sum_ce_data > 0) {
      min_data = srsran::ce_total_size(lch_handler.pending_ces.front());
    } else if (rb_data > 0) {
      min_data = MAC_MIN_ALLOC_SIZE;
    }
  }

  return {min_data, max_data};
}

/// Get pending RLC DL data in RLC buffers. Header sizes not accounted
uint32_t sched_ue::get_pending_dl_rlc_data() const
{
  return lch_handler.get_dl_tx_total();
}

uint32_t sched_ue::get_expected_dl_bitrate(uint32_t enb_cc_idx, int nof_rbgs) const
{
  auto&    cc = cells[enb_cc_idx];
  uint32_t nof_re =
      cc.cell_cfg->get_dl_lb_nof_re(to_tx_dl(current_tti), count_prb_per_tb_approx(nof_rbgs, cc.cell_cfg->nof_prb()));
  float max_coderate = srsran_cqi_to_coderate(std::min(cc.get_dl_cqi() + 1u, 15u), cfg.use_tbs_index_alt);

  // Inverse of srsran_coderate(tbs, nof_re)
  uint32_t tbs = max_coderate * nof_re - 24;
  return tbs / tti_duration_ms;
}

uint32_t sched_ue::get_expected_ul_bitrate(uint32_t enb_cc_idx, int nof_prbs) const
{
  uint32_t nof_prbs_alloc = nof_prbs < 0 ? cell.nof_prb : nof_prbs;

  uint32_t N_srs        = 0;
  uint32_t nof_symb     = 2 * (SRSRAN_CP_NSYMB(cell.cp) - 1) - N_srs;
  uint32_t nof_re       = nof_symb * nof_prbs_alloc * SRSRAN_NRE;
  float    max_coderate = srsran_cqi_to_coderate(std::min(cells[enb_cc_idx].get_ul_cqi() + 1u, 15u), false);

  // Inverse of srsran_coderate(tbs, nof_re)
  uint32_t tbs = max_coderate * nof_re - 24;
  return tbs / tti_duration_ms;
}

/// Returns nof bytes allocated to active UL HARQs in the carrier cc_idx.
/// NOTE: The returned value accounts for the MAC header and payload (RLC headers and actual data)
uint32_t sched_ue::get_pending_ul_old_data(uint32_t enb_cc_idx)
{
  uint32_t pending_data = 0;
  for (auto& h : cells[enb_cc_idx].harq_ent.ul_harq_procs()) {
    pending_data += h.get_pending_data();
  }
  return pending_data;
}

/// Returns the total of all TB bytes allocated to UL HARQs
uint32_t sched_ue::get_pending_ul_old_data()
{
  uint32_t pending_ul_data = 0;
  for (uint32_t i = 0; i < cells.size(); ++i) {
    if (cells[i].configured()) {
      pending_ul_data += get_pending_ul_old_data(i);
    }
  }
  return pending_ul_data;
}

uint32_t sched_ue::get_pending_ul_data_total(tti_point tti_tx_ul, int this_enb_cc_idx)
{
  static constexpr uint32_t lbsr_size = 4, sbsr_size = 2;

  // Note: If there are no active bearers, scheduling requests are also ignored.
  uint32_t pending_data = 0;
  uint32_t pending_lcgs = 0;
  for (int lcg = 0; lcg < sched_interface::MAX_LC_GROUP; lcg++) {
    uint32_t bsr = lch_handler.get_bsr_with_overhead(lcg);
    if (bsr > 0) {
      pending_data += bsr;
      pending_lcgs++;
    }
  }
  if (pending_data > 0) {
    // The scheduler needs to account for the possibility of BSRs being allocated in the UL grant.
    // Otherwise, the UL grants allocated for very small RRC messages (e.g. rrcReconfigurationComplete)
    // may be fully occupied by a BSR, and RRC the message transmission needs to be postponed.
    pending_data += (pending_lcgs <= 1) ? sbsr_size : lbsr_size;
  } else {
    if (is_sr_triggered() and this_enb_cc_idx >= 0) {
      // Check if this_cc_idx is the carrier with highest CQI
      uint32_t max_cqi = 0, max_cc_idx = 0;
      for (uint32_t cc = 0; cc < cells.size(); ++cc) {
        if (cells[cc].configured()) {
          uint32_t sum_cqi = cells[cc].get_dl_cqi() + cells[cc].get_ul_cqi();
          if (cells[cc].cc_state() == cc_st::active and sum_cqi > max_cqi) {
            max_cqi    = sum_cqi;
            max_cc_idx = cc;
          }
        }
      }
      if ((int)max_cc_idx == this_enb_cc_idx) {
        return 512;
      }
    }
    for (uint32_t i = 0; i < cells.size(); ++i) {
      if (cells[i].configured() and needs_cqi(tti_tx_ul.to_uint(), i)) {
        return 128;
      }
    }
  }

  return pending_data;
}

uint32_t sched_ue::get_pending_ul_new_data(tti_point tti_tx_ul, int this_enb_cc_idx)
{
  uint32_t pending_data = get_pending_ul_data_total(tti_tx_ul, this_enb_cc_idx);

  // Subtract all the UL data already allocated in the UL harqs
  uint32_t pending_ul_data = get_pending_ul_old_data();
  pending_data             = (pending_data > pending_ul_data) ? pending_data - pending_ul_data : 0;

  if (pending_data > 0) {
    if (logger.debug.enabled()) {
      fmt::memory_buffer str_buffer;
      fmt::format_to(str_buffer, "{}", lch_handler.get_bsr_state());
      logger.debug("SCHED: pending_data=%d, in_harq_data=%d, bsr=%s",
                   pending_data,
                   pending_ul_data,
                   srsran::to_c_str(str_buffer));
    }
  }
  return pending_data;
}

uint32_t sched_ue::get_required_prb_ul(uint32_t enb_cc_idx, uint32_t req_bytes)
{
  return srsenb::get_required_prb_ul(cells[enb_cc_idx], req_bytes);
}

bool sched_ue::is_sr_triggered()
{
  return sr;
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(tti_point tti_tx_dl, uint32_t enb_cc_idx)
{
  if (cells[enb_cc_idx].cc_state() == cc_st::active) {
    return cells[enb_cc_idx].harq_ent.get_pending_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_empty_dl_harq(tti_point tti_tx_dl, uint32_t enb_cc_idx)
{
  if (cells[enb_cc_idx].cc_state() == cc_st::active) {
    return cells[enb_cc_idx].harq_ent.get_empty_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

ul_harq_proc* sched_ue::get_ul_harq(tti_point tti_tx_ul, uint32_t enb_cc_idx)
{
  if (cells[enb_cc_idx].cc_state() == cc_st::active) {
    return cells[enb_cc_idx].harq_ent.get_ul_harq(tti_tx_ul);
  }
  return nullptr;
}

const dl_harq_proc& sched_ue::get_dl_harq(uint32_t idx, uint32_t enb_cc_idx) const
{
  return cells[enb_cc_idx].harq_ent.dl_harq_procs()[idx];
}

std::pair<bool, uint32_t> sched_ue::get_active_cell_index(uint32_t enb_cc_idx) const
{
  if (cells[enb_cc_idx].configured()) {
    return {cells[enb_cc_idx].cc_state() == cc_st::active, cells[enb_cc_idx].get_ue_cc_idx()};
  }
  return {false, std::numeric_limits<uint32_t>::max()};
}

uint32_t sched_ue::get_aggr_level(uint32_t enb_cc_idx, uint32_t nof_bits)
{
  return cells[enb_cc_idx].get_aggr_level(nof_bits);
}

void sched_ue::finish_tti(tti_point tti_rx, uint32_t enb_cc_idx)
{
  // Check that scell state needs to change
  cells[enb_cc_idx].finish_tti(tti_rx);
}

srsran_dci_format_t sched_ue::get_dci_format()
{
  srsran_dci_format_t ret = SRSRAN_DCI_FORMAT1A;

  if (phy_config_dedicated_enabled) {
    /* TODO: Assumes UE-Specific Search Space (Not common) */
    switch (cfg.dl_ant_info.tx_mode) {
      case sched_interface::ant_info_ded_t::tx_mode_t::tm1:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm2:
        // If the size of Format1 and Format1A is ambiguous in the common SS, use Format1A since the UE assumes
        // Common SS when spaces collide
        if (not(cell.nof_prb == 15 && cells.size() > 1)) {
          ret = SRSRAN_DCI_FORMAT1;
        }
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm3:
        ret = SRSRAN_DCI_FORMAT2A;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm4:
        ret = SRSRAN_DCI_FORMAT2;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm5:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm6:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm7:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm8_v920:
      default:
        logger.warning(
            "Incorrect transmission mode (rnti=%04x; tm=%d)", rnti, static_cast<int>(cfg.dl_ant_info.tx_mode));
    }
  }

  return ret;
}

const cce_cfi_position_table* sched_ue::get_locations(uint32_t enb_cc_idx, uint32_t cfi, uint32_t sf_idx) const
{
  if (cfi > 0 && cfi <= 3) {
    return &cells[enb_cc_idx].dci_locations[sf_idx][cfi - 1];
  } else {
    logger.error("SCHED: Invalid CFI=%d", cfi);
    return &cells[enb_cc_idx].dci_locations[sf_idx][0];
  }
}

sched_ue_cell* sched_ue::find_ue_carrier(uint32_t enb_cc_idx)
{
  return cells[enb_cc_idx].configured() ? &cells[enb_cc_idx] : nullptr;
}

std::bitset<SRSRAN_MAX_CARRIERS> sched_ue::scell_activation_mask() const
{
  std::bitset<SRSRAN_MAX_CARRIERS> ret{0};
  for (size_t i = 0; i < cells.size(); ++i) {
    if (cells[i].cc_state() == cc_st::active and cells[i].is_scell()) {
      ret[cells[i].get_ue_cc_idx()] = true;
    }
  }
  return ret;
}

int sched_ue::enb_to_ue_cc_idx(uint32_t enb_cc_idx) const
{
  return enb_cc_idx < cells.size() ? cells[enb_cc_idx].get_ue_cc_idx() : -1;
}

} // namespace srsenb
