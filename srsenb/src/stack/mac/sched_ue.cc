/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <string.h>

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
#include "srslte/common/log_helper.h"
#include "srslte/common/logmap.h"
#include "srslte/srslte.h"

using srslte::tti_interval;

namespace srsenb {

/******************************************************
 *                 Helper Functions                   *
 ******************************************************/

#define MAC_MIN_ALLOC_SIZE 5

template <typename YType, typename Callable, typename ErrorDetect>
std::tuple<int, YType, int, YType>
false_position_method(int x1, int x2, YType y0, const Callable& f, const ErrorDetect& is_error)
{
  static_assert(std::is_same<YType, decltype(f(x1))>::value,
                "The type of the final result and callable return do not match\n");
  YType y1 = f(x1);
  if (is_error(y1) or y1 >= y0) {
    return std::make_tuple(x1, y1, x1, y1);
  }
  YType y2 = f(x2);
  if (is_error(y2) or y2 <= y0) {
    return std::make_tuple(x2, y2, x2, y2);
  }
  YType y3;
  while (x2 > x1 + 1) {
    int x3 = round(x1 - ((x2 - x1) * (y1 - y0) / (float)(y2 - y1)));
    if (x3 == x2) {
      y3 = y2;
      // check if in frontier
      YType y3_1 = f(x3 - 1);
      if (not is_error(y3_1) and y3_1 < y0) {
        return std::make_tuple(x3 - 1, y3_1, x3, y3);
      } else {
        x3--;
        y3 = y3_1;
      }
    } else if (x3 == x1) {
      y3 = y1;
      // check if in frontier
      YType y3_1 = f(x3 + 1);
      if (not is_error(y3_1) and y3_1 >= y0) {
        return std::make_tuple(x3, y3, x3 + 1, y3_1);
      } else {
        x3++;
        y3 = y3_1;
      }
    } else {
      y3 = f(x3);
      if (is_error(y3) or y3 == y0) {
        return std::make_tuple(x3, y3, x3, y3);
      }
    }
    if (y3 < y0) {
      x1 = x3;
      y1 = y3;
    } else {
      x2 = x3;
      y2 = y3;
    }
  }
  return std::make_tuple(x1, y1, x2, y2);
}
template <typename YType, typename Callable>
std::tuple<int, YType, int, YType> false_position_method(int x1, int x2, YType y0, const Callable& f)
{
  return false_position_method(x1, x2, y0, f, [](int x) { return false; });
}

/*******************************************************
 *
 * Initialization and configuration functions
 *
 *******************************************************/

sched_ue::sched_ue() : log_h(srslte::logmap::get("MAC"))
{
  reset();
}

void sched_ue::init(uint16_t rnti_, const std::vector<sched_cell_params_t>& cell_list_params_)
{
  rnti = rnti_;
  cells.reserve(cell_list_params_.size());
  for (auto& c : cell_list_params_) {
    cells.emplace_back(rnti_, c);
  }
  Info("SCHED: Added user rnti=0x%x\n", rnti);
}

void sched_ue::set_cfg(const ue_cfg_t& cfg_)
{
  // for the first configured cc, set it as primary cc
  if (cfg.supported_cc_list.empty()) {
    uint32_t primary_cc_idx = 0;
    if (not cfg_.supported_cc_list.empty()) {
      primary_cc_idx = cfg_.supported_cc_list[0].enb_cc_idx;
    } else {
      Warning("Primary cc idx not provided in scheduler ue_cfg. Defaulting to cc_idx=0\n");
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
  for (auto& c : cells) {
    c.set_ue_cfg(cfg);
  }

  // in case carriers have been removed
  while (carriers.size() > cfg.supported_cc_list.size()) {
    // TODO: distinguish cell deactivation from reconfiguration
    carriers.pop_back();
  }
  // in case carriers have been added or modified
  bool scell_activation_state_changed = false;
  for (uint32_t ue_idx = 0; ue_idx < cfg.supported_cc_list.size(); ++ue_idx) {
    auto& cc_cfg = cfg.supported_cc_list[ue_idx];

    if (ue_idx >= prev_supported_cc_list.size()) {
      // New carrier needs to be added
      carriers.emplace_back(cfg, cells[cc_cfg.enb_cc_idx], rnti, ue_idx, current_tti);
    } else if (cc_cfg.enb_cc_idx != prev_supported_cc_list[ue_idx].enb_cc_idx) {
      // One carrier was added in the place of another
      carriers[ue_idx] = cc_sched_ue{cfg, cells[cc_cfg.enb_cc_idx], rnti, ue_idx, current_tti};
      if (ue_idx == 0) {
        log_h->info("SCHED: rnti=0x%x PCell is now enb_cc_idx=%d.\n", rnti, cc_cfg.enb_cc_idx);
      }
    } else {
      // The SCell internal configuration may have changed
      carriers[ue_idx].set_cfg(cfg);
    }
    scell_activation_state_changed |= ue_idx > 0 and (carriers[ue_idx].cc_state() == cc_st::activating or
                                                      carriers[ue_idx].cc_state() == cc_st::deactivating);
  }
  if (scell_activation_state_changed) {
    lch_handler.pending_ces.emplace_back(srslte::dl_sch_lcid::SCELL_ACTIVATION);
    log_h->info("SCHED: Enqueueing SCell Activation CMD for rnti=0x%x\n", rnti);
  }

  check_ue_cfg_correctness(cfg);
}

void sched_ue::reset()
{
  cfg                          = {};
  sr                           = false;
  phy_config_dedicated_enabled = false;
  cqi_request_tti              = 0;
  carriers.clear();

  // erase all bearers
  for (uint32_t i = 0; i < cfg.ue_bearers.size(); ++i) {
    lch_handler.config_lcid(i, {});
  }
}

void sched_ue::new_subframe(tti_point tti_rx, uint32_t enb_cc_idx)
{
  if (current_tti != tti_rx) {
    current_tti = tti_rx;
    lch_handler.new_tti();
    for (auto& cc : carriers) {
      cc.harq_ent.new_tti(tti_rx);
    }
  }
  int ue_cc_idx = cells[enb_cc_idx].get_ue_cc_idx();
  if (ue_cc_idx >= 0) {
    carriers.at(ue_cc_idx).tpc_fsm.new_tti();
  }
}

/*******************************************************
 *
 * FAPI-like main scheduler interface.
 *
 *******************************************************/

void sched_ue::set_bearer_cfg(uint32_t lc_id, const bearer_cfg_t& cfg_)
{
  cfg.ue_bearers[lc_id] = cfg_;
  lch_handler.config_lcid(lc_id, cfg_);
}

void sched_ue::rem_bearer(uint32_t lc_id)
{
  cfg.ue_bearers[lc_id] = sched_interface::ue_bearer_cfg_t{};
  lch_handler.config_lcid(lc_id, sched_interface::ue_bearer_cfg_t{});
}

void sched_ue::phy_config_enabled(tti_point tti_rx, bool enabled)
{
  for (cc_sched_ue& c : carriers) {
    c.dl_cqi_tti_rx = tti_rx;
  }
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

void sched_ue::ul_phr(int phr)
{
  carriers[0].tpc_fsm.set_phr(phr);
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
  Info("SCHED: %s for rnti=0x%x needs to be scheduled\n", to_string(cmd), rnti);
}

void sched_ue::set_sr()
{
  sr = true;
}

void sched_ue::unset_sr()
{
  sr = false;
}

bool sched_ue::pucch_sr_collision(tti_point tti_tx_dl, uint32_t n_cce)
{
  if (!phy_config_dedicated_enabled) {
    return false;
  }
  if (cfg.pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, tti_tx_dl.to_uint())) {
    return (n_cce + cfg.pucch_cfg.N_pucch_1) == cfg.pucch_cfg.n_pucch_sr;
  }
  return false;
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

bool sched_ue::pdsch_enabled(srslte::tti_point tti_rx, uint32_t enb_cc_idx) const
{
  if (carriers[0].get_cell_cfg()->enb_cc_idx != enb_cc_idx) {
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
  if (carriers[0].get_cell_cfg()->enb_cc_idx != enb_cc_idx) {
    return true;
  }

  // Check measGap collision
  if (cfg.measgap_period > 0) {
    tti_point    tti_tx_ul = to_tx_ul(tti_rx);
    tti_point    mgap_tti  = nearest_meas_gap(tti_tx_ul, cfg.measgap_period, cfg.measgap_offset);
    tti_interval meas_gap{mgap_tti, mgap_tti + 6};

    // disable TTIs that leads to PUSCH tx or PHICH rx falling in measGap
    if (meas_gap.contains(tti_tx_ul) or meas_gap.contains(to_tx_ul_ack(tti_rx))) {
      return false;
    }
    // disable TTIs which respective PDCCH falls in measGap (in case PDCCH is needed)
    if (needs_pdcch and meas_gap.contains(to_tx_dl(tti_rx))) {
      return false;
    }
  }
  return true;
}

int sched_ue::set_ack_info(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int          tbs_acked = -1;
  cc_sched_ue* c         = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    std::pair<uint32_t, int> p2 = c->harq_ent.set_ack_info(tti_rx, tb_idx, ack);
    tbs_acked                   = p2.second;
    if (tbs_acked > 0) {
      Debug(
          "SCHED: Set DL ACK=%d for rnti=0x%x, pid=%d, tb=%d, tti=%d\n", ack, rnti, p2.first, tb_idx, tti_rx.to_uint());
    } else {
      Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti_rx.to_uint());
    }
  } else {
    log_h->warning("Received DL ACK for invalid cell index %d\n", enb_cc_idx);
  }
  return tbs_acked;
}

void sched_ue::set_ul_crc(tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    int ret = c->harq_ent.set_ul_crc(tti_rx, 0, crc_res);
    if (ret < 0) {
      log_h->warning("Received UL CRC for invalid tti_rx=%d\n", (int)tti_rx.to_uint());
    }
  } else {
    log_h->warning("Received UL CRC for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_ri(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t ri)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->dl_ri        = ri;
    c->dl_ri_tti_rx = tti_rx;
  } else {
    log_h->warning("Received DL RI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_pmi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t pmi)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->dl_pmi        = pmi;
    c->dl_pmi_tti_rx = tti_rx;
  } else {
    log_h->warning("Received DL PMI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_cqi(tti_point tti_rx, uint32_t enb_cc_idx, uint32_t cqi)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->set_dl_cqi(tti_rx, cqi);
  } else {
    log_h->warning("Received DL CQI for invalid enb cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_ul_snr(tti_point tti_rx, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->tpc_fsm.set_snr(snr, ul_ch_code);
    c->ul_cqi        = srslte_cqi_from_snr(snr);
    c->ul_cqi_tti_rx = tti_rx;
  } else {
    log_h->warning("Received SNR info for invalid cell index %d\n", enb_cc_idx);
  }
}

/*******************************************************
 *
 * Functions used to generate DCI grants
 *
 *******************************************************/

/**
 * Allocate MAC PDU for a UE HARQ pid
 * @param data
 * @param total_tbs
 * @param ue_cc_idx
 * @return pair with allocated tbs and mcs
 */
tbs_info sched_ue::allocate_new_dl_mac_pdu(sched::dl_sched_data_t* data,
                                           dl_harq_proc*           h,
                                           const rbgmask_t&        user_mask,
                                           tti_point               tti_tx_dl,
                                           uint32_t                ue_cc_idx,
                                           uint32_t                cfi,
                                           uint32_t                tb)
{
  srslte_dci_dl_t* dci     = &data->dci;
  uint32_t         nof_prb = count_prb_per_tb(user_mask);
  tbs_info         tb_info = compute_mcs_and_tbs(ue_cc_idx, tti_tx_dl, nof_prb, cfi, *dci);

  // Allocate MAC PDU (subheaders, CEs, and SDUS)
  int rem_tbs = tb_info.tbs_bytes;
  rem_tbs -= allocate_mac_ces(data, lch_handler, rem_tbs, ue_cc_idx);
  rem_tbs -= allocate_mac_sdus(data, lch_handler, rem_tbs, tb);

  // Allocate DL UE Harq
  if (rem_tbs != tb_info.tbs_bytes) {
    h->new_tx(
        user_mask, tb, tti_tx_dl, tb_info.mcs, tb_info.tbs_bytes, data->dci.location.ncce, get_ue_cfg().maxharq_tx);
  } else {
    uint32_t pending_bytes = lch_handler.get_dl_tx_total();
    if (pending_bytes > 0) {
      Warning("SCHED: Failed to allocate DL TB with tb_idx=%d, tbs=%d, pid=%d. Pending DL buffer data=%d\n",
              tb,
              rem_tbs,
              h->get_id(),
              lch_handler.get_dl_tx_total());
    } else {
      Info("SCHED: DL TB tb_idx=%d, tbs=%d, pid=%d did not get allocated.\n", tb, rem_tbs, h->get_id());
    }
    tb_info.tbs_bytes = 0;
    tb_info.mcs       = 0;
  }

  return tb_info;
}

int sched_ue::generate_dl_dci_format(uint32_t                          pid,
                                     sched_interface::dl_sched_data_t* data,
                                     tti_point                         tti_tx_dl,
                                     uint32_t                          ue_cc_idx,
                                     uint32_t                          cfi,
                                     const rbgmask_t&                  user_mask)
{
  srslte_dci_format_t dci_format = get_dci_format();
  int                 tbs        = 0;

  switch (dci_format) {
    case SRSLTE_DCI_FORMAT1:
      tbs = generate_format1(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    case SRSLTE_DCI_FORMAT2:
      tbs = generate_format2(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    case SRSLTE_DCI_FORMAT2A:
      tbs = generate_format2a(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    default:
      Error("DCI format (%d) not implemented\n", dci_format);
  }
  return tbs;
}

// Generates a Format1 dci
// > return 0 if allocation is invalid
int sched_ue::generate_format1(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               tti_point                         tti_tx_dl,
                               uint32_t                          ue_cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  dl_harq_proc*    h   = &carriers[ue_cc_idx].harq_ent.dl_harq_procs()[pid];
  srslte_dci_dl_t* dci = &data->dci;

  // If the size of Format1 and Format1A is ambiguous in the common SS, use Format1A since the UE assumes
  // Common SS when spaces collide
  if (cell.nof_prb == 15 && cells.size() > 1) {
    dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
    dci->type2_alloc.mode = srslte_ra_type2_t::SRSLTE_RA_TYPE2_LOC;
    rbg_interval rbg_int  = rbg_interval::rbgmask_to_rbgs(user_mask);
    prb_interval prb_int  = prb_interval::rbgs_to_prbs(rbg_int, cell.nof_prb);
    uint32_t     L_crb    = prb_int.length();
    uint32_t     RB_start = prb_int.start();
    dci->type2_alloc.riv  = srslte_ra_type2_to_riv(L_crb, RB_start, cell.nof_prb);
    dci->format           = SRSLTE_DCI_FORMAT1A;
    if (L_crb != count_prb_per_tb(user_mask)) {
      // This happens if Type0 was using distributed allocation
      Warning("SCHED: Can't use distributed RA due to DCI size ambiguity\n");
    }
  } else {
    dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
    dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();
    dci->format                  = SRSLTE_DCI_FORMAT1;
  }

  tbs_info tbinfo;
  if (h->is_empty(0)) {
    tbinfo = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, ue_cc_idx, cfi, 0);
  } else {
    h->new_retx(user_mask, 0, tti_tx_dl, &tbinfo.mcs, &tbinfo.tbs_bytes, data->dci.location.ncce);
    Debug("SCHED: Alloc format1 previous mcs=%d, tbs=%d\n", tbinfo.mcs, tbinfo.tbs_bytes);
  }

  if (tbinfo.tbs_bytes > 0) {
    dci->rnti          = rnti;
    dci->pid           = h->get_id();
    dci->ue_cc_idx     = ue_cc_idx;
    dci->tb[0].mcs_idx = (uint32_t)tbinfo.mcs;
    dci->tb[0].rv      = get_rvidx(h->nof_retx(0));
    dci->tb[0].ndi     = h->get_ndi(0);

    dci->tpc_pucch = carriers[ue_cc_idx].tpc_fsm.encode_pucch_tpc();
    data->tbs[0]   = (uint32_t)tbinfo.tbs_bytes;
    data->tbs[1]   = 0;
  }
  return tbinfo.tbs_bytes;
}

/**
 * Based on the amount of tx data, allocated PRBs, DCI params, etc. compute a valid MCS and resulting TBS
 * @param ue_cc_idx user carrier index
 * @param tti_tx_dl tti when the tx will occur
 * @param nof_alloc_prbs number of PRBs that were allocated
 * @param cfi Number of control symbols in Subframe
 * @param dci contains the RBG mask, and alloc type
 * @return pair with MCS and TBS (in bytes)
 */
tbs_info sched_ue::compute_mcs_and_tbs(uint32_t               ue_cc_idx,
                                       tti_point              tti_tx_dl,
                                       uint32_t               nof_alloc_prbs,
                                       uint32_t               cfi,
                                       const srslte_dci_dl_t& dci)
{
  srslte::interval<uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);

  // Calculate exact number of RE for this PRB allocation
  uint32_t nof_re = carriers[ue_cc_idx].get_cell_cfg()->get_dl_nof_res(tti_tx_dl, dci, cfi);

  // Compute MCS+TBS
  tbs_info tb = carriers[ue_cc_idx].alloc_tbs_dl(nof_alloc_prbs, nof_re, req_bytes.stop());

  if (tb.tbs_bytes > 0 and tb.tbs_bytes < (int)req_bytes.start()) {
    log_h->info("SCHED: Could not get PRB allocation that avoids MAC CE or RLC SRB0 PDU segmentation\n");
    // Note: This is not a warning, because the srb0 buffer can be updated after the ue sched decision
  }

  return tb;
}

// Generates a Format2a dci
int sched_ue::generate_format2a(uint32_t                          pid,
                                sched_interface::dl_sched_data_t* data,
                                tti_point                         tti_tx_dl,
                                uint32_t                          ue_cc_idx,
                                uint32_t                          cfi,
                                const rbgmask_t&                  user_mask)
{
  dl_harq_proc* h                    = &carriers[ue_cc_idx].harq_ent.dl_harq_procs()[pid];
  bool          tb_en[SRSLTE_MAX_TB] = {false};

  srslte_dci_dl_t* dci         = &data->dci;
  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  bool no_retx = true;

  if (carriers[ue_cc_idx].dl_ri == 0) {
    if (h->is_empty(1)) {
      /* One layer, tb1 buffer is empty, send tb0 only */
      tb_en[0] = true;
    } else {
      /* One layer, tb1 buffer is not empty, send tb1 only */
      tb_en[1] = true;
    }
  } else {
    /* Two layers, retransmit what TBs that have not been Acknowledged */
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
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

  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    tbs_info tbinfo;

    if (!h->is_empty(tb)) {
      h->new_retx(user_mask, tb, tti_tx_dl, &tbinfo.mcs, &tbinfo.tbs_bytes, data->dci.location.ncce);
    } else if (tb_en[tb] && no_retx) {
      tbinfo = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, ue_cc_idx, cfi, tb);
    }

    /* Fill DCI TB dedicated fields */
    if (tbinfo.tbs_bytes > 0 && tb_en[tb]) {
      dci->tb[tb].mcs_idx = (uint32_t)tbinfo.mcs;
      dci->tb[tb].rv      = get_rvidx(h->nof_retx(tb));
      if (!SRSLTE_DCI_IS_TB_EN(dci->tb[tb])) {
        dci->tb[tb].rv = 2;
      }
      dci->tb[tb].ndi    = h->get_ndi(tb);
      dci->tb[tb].cw_idx = tb;
      data->tbs[tb]      = (uint32_t)tbinfo.tbs_bytes;
    } else {
      SRSLTE_DCI_TB_DISABLE(dci->tb[tb]);
      data->tbs[tb] = 0;
    }
  }

  /* Fill common fields */
  dci->format    = SRSLTE_DCI_FORMAT2A;
  dci->rnti      = rnti;
  dci->ue_cc_idx = ue_cc_idx;
  dci->pid       = h->get_id();
  dci->tpc_pucch = carriers[ue_cc_idx].tpc_fsm.encode_pucch_tpc();

  int ret = data->tbs[0] + data->tbs[1];
  return ret;
}

// Generates a Format2 dci
int sched_ue::generate_format2(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               tti_point                         tti_tx_dl,
                               uint32_t                          cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  /* Call Format 2a (common) */
  int ret = generate_format2a(pid, data, tti_tx_dl, cc_idx, cfi, user_mask);

  /* Compute precoding information */
  data->dci.format = SRSLTE_DCI_FORMAT2;
  if ((SRSLTE_DCI_IS_TB_EN(data->dci.tb[0]) + SRSLTE_DCI_IS_TB_EN(data->dci.tb[1])) == 1) {
    data->dci.pinfo = (uint8_t)(carriers[cc_idx].dl_pmi + 1) % (uint8_t)5;
  } else {
    data->dci.pinfo = (uint8_t)(carriers[cc_idx].dl_pmi & 1u);
  }

  return ret;
}

int sched_ue::generate_format0(sched_interface::ul_sched_data_t* data,
                               tti_point                         tti_tx_ul,
                               uint32_t                          ue_cc_idx,
                               prb_interval                      alloc,
                               bool                              needs_pdcch,
                               srslte_dci_location_t             dci_pos,
                               int                               explicit_mcs,
                               uci_pusch_t                       uci_type)
{
  ul_harq_proc*    h   = get_ul_harq(tti_tx_ul, ue_cc_idx);
  srslte_dci_ul_t* dci = &data->dci;

  bool cqi_request = needs_cqi(tti_tx_ul.to_uint(), true);

  // Set DCI position
  data->needs_pdcch = needs_pdcch;
  dci->location     = dci_pos;

  tbs_info tbinfo;
  tbinfo.mcs       = (explicit_mcs >= 0) ? explicit_mcs : carriers[ue_cc_idx].fixed_mcs_ul;
  tbinfo.tbs_bytes = 0;

  bool is_newtx = h->is_empty(0);
  if (is_newtx) {
    uint32_t nof_retx;

    // If Msg3 set different nof retx
    nof_retx = (data->needs_pdcch) ? get_max_retx() : max_msg3retx;

    if (tbinfo.mcs >= 0) {
      tbinfo.tbs_bytes = get_tbs_bytes(tbinfo.mcs, alloc.length(), false, true);
    } else {
      // dynamic mcs
      uint32_t req_bytes = get_pending_ul_new_data(tti_tx_ul, ue_cc_idx);
      uint32_t N_srs     = 0;
      uint32_t nof_symb  = 2 * (SRSLTE_CP_NSYMB(cell.cp) - 1) - N_srs;
      uint32_t nof_re    = nof_symb * alloc.length() * SRSLTE_NRE;
      tbinfo             = carriers[ue_cc_idx].alloc_tbs_ul(alloc.length(), nof_re, req_bytes);

      // Reduce MCS to fit UCI if transmitted in this grant
      if (uci_type != UCI_PUSCH_NONE) {
        // Calculate an approximation of the number of RE used by UCI
        uint32_t nof_uci_re = 0;
        // Add the RE for ACK
        if (uci_type == UCI_PUSCH_ACK || uci_type == UCI_PUSCH_ACK_CQI) {
          float beta = srslte_sch_beta_ack(cfg.uci_offset.I_offset_ack);
          nof_uci_re += srslte_qprime_ack_ext(alloc.length(), nof_symb, 8 * tbinfo.tbs_bytes, carriers.size(), beta);
        }
        // Add the RE for CQI report (RI reports are transmitted on CQI slots. We do a conservative estimate here)
        if (uci_type == UCI_PUSCH_CQI || uci_type == UCI_PUSCH_ACK_CQI || cqi_request) {
          float beta = srslte_sch_beta_cqi(cfg.uci_offset.I_offset_cqi);
          nof_uci_re += srslte_qprime_cqi_ext(alloc.length(), nof_symb, 8 * tbinfo.tbs_bytes, beta);
        }
        // Recompute again the MCS and TBS with the new spectral efficiency (based on the available RE for data)
        if (nof_re >= nof_uci_re) {
          tbinfo = carriers[ue_cc_idx].alloc_tbs_ul(alloc.length(), nof_re - nof_uci_re, req_bytes);
        }
        // NOTE: if (nof_re < nof_uci_re) we should set TBS=0
      }
    }
    h->new_tx(tti_tx_ul, tbinfo.mcs, tbinfo.tbs_bytes, alloc, nof_retx);
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
    dci->format         = SRSLTE_DCI_FORMAT0;
    dci->ue_cc_idx      = ue_cc_idx;
    dci->tb.ndi         = h->get_ndi(0);
    dci->cqi_request    = cqi_request;
    dci->freq_hop_fl    = srslte_dci_ul_t::SRSLTE_RA_PUSCH_HOP_DISABLED;
    dci->tpc_pusch      = carriers[ue_cc_idx].tpc_fsm.encode_pusch_tpc();

    dci->type2_alloc.riv = srslte_ra_type2_to_riv(alloc.length(), alloc.start(), cell.nof_prb);

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
      log_h->warning("SCHED: No space for ULSCH while allocating format0. Discarding grant.\n");
    } else {
      log_h->error("SCHED: Unkown error while allocating format0\n");
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

bool sched_ue::needs_cqi(uint32_t tti, uint32_t cc_idx, bool will_send)
{
  bool ret = false;
  if (phy_config_dedicated_enabled && cfg.supported_cc_list[0].aperiodic_cqi_period &&
      lch_handler.has_pending_dl_txs()) {
    uint32_t interval = srslte_tti_interval(tti, carriers[cc_idx].dl_cqi_tti_rx.to_uint());
    bool     needscqi = interval >= cfg.supported_cc_list[0].aperiodic_cqi_period;
    if (needscqi) {
      uint32_t interval_sent = srslte_tti_interval(tti, cqi_request_tti);
      if (interval_sent >= 16) {
        if (will_send) {
          cqi_request_tti = tti;
        }
        Debug("SCHED: Needs_cqi, last_sent=%d, will_be_sent=%d\n", cqi_request_tti, will_send);
        ret = true;
      }
    }
  }
  return ret;
}

/**
 * Compute the range of RBGs that avoids segmentation of TM and MAC subheader data. Always computed for highest CFI
 * @param ue_cc_idx carrier of the UE
 * @return range of number of RBGs that a UE can allocate in a given subframe
 */
rbg_interval sched_ue::get_required_dl_rbgs(uint32_t ue_cc_idx)
{
  srslte::interval<uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);
  if (req_bytes == srslte::interval<uint32_t>{0, 0}) {
    return {0, 0};
  }
  const auto* cellparams   = carriers[ue_cc_idx].get_cell_cfg();
  int         pending_prbs = carriers[ue_cc_idx].get_required_prb_dl(to_tx_dl(current_tti), req_bytes.start());
  if (pending_prbs < 0) {
    // Cannot fit allocation in given PRBs
    log_h->error("SCHED: DL CQI=%d does now allow fitting %d non-segmentable DL tx bytes into the cell bandwidth. "
                 "Consider increasing initial CQI value.\n",
                 carriers[ue_cc_idx].dl_cqi,
                 req_bytes.start());
    return {cellparams->nof_prb(), cellparams->nof_prb()};
  }
  uint32_t min_pending_rbg = cellparams->nof_prbs_to_rbgs(pending_prbs);
  pending_prbs             = carriers[ue_cc_idx].get_required_prb_dl(to_tx_dl(current_tti), req_bytes.stop());
  pending_prbs             = (pending_prbs < 0) ? cellparams->nof_prb() : pending_prbs;
  uint32_t max_pending_rbg = cellparams->nof_prbs_to_rbgs(pending_prbs);
  return {min_pending_rbg, max_pending_rbg};
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
 * @ue_cc_idx carrier where allocation is being made
 * @return
 */
srslte::interval<uint32_t> sched_ue::get_requested_dl_bytes(uint32_t ue_cc_idx)
{
  /* Set Maximum boundary */
  // Ensure there is space for ConRes and RRC Setup
  // SRB0 is a special case due to being RLC TM (no segmentation possible)
  if (not lch_handler.is_bearer_dl(0)) {
    log_h->error("SRB0 must always be activated for DL\n");
    return {};
  }
  if (carriers[ue_cc_idx].cc_state() != cc_st::active) {
    return {};
  }

  uint32_t max_data = 0, min_data = 0;
  uint32_t srb0_data = 0, rb_data = 0, sum_ce_data = 0;

  srb0_data = lch_handler.get_dl_tx_total_with_overhead(0);
  // Add pending CEs
  if (ue_cc_idx == 0) {
    if (srb0_data == 0 and not lch_handler.pending_ces.empty() and
        lch_handler.pending_ces.front() == srslte::dl_sch_lcid::CON_RES_ID) {
      // Wait for SRB0 data to be available for Msg4 before scheduling the ConRes CE
      return {};
    }
    for (const lch_ue_manager::ce_cmd& ce : lch_handler.pending_ces) {
      sum_ce_data += srslte::ce_total_size(ce);
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
    min_data += srslte::ce_total_size(lch_handler.pending_ces.front());
  }
  if (min_data == 0) {
    if (sum_ce_data > 0) {
      min_data = srslte::ce_total_size(lch_handler.pending_ces.front());
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

uint32_t sched_ue::get_expected_dl_bitrate(uint32_t ue_cc_idx, int nof_rbgs) const
{
  const cc_sched_ue* cc     = &carriers[ue_cc_idx];
  uint32_t           nof_re = cc->get_cell_cfg()->get_dl_lb_nof_re(
      to_tx_dl(current_tti), count_prb_per_tb_approx(nof_rbgs, cc->get_cell_cfg()->nof_prb()));
  float max_coderate = srslte_cqi_to_coderate(std::min(cc->dl_cqi + 1u, 15u), cfg.use_tbs_index_alt);

  // Inverse of srslte_coderate(tbs, nof_re)
  uint32_t tbs = max_coderate * nof_re - 24;
  return tbs / tti_duration_ms;
}

uint32_t sched_ue::get_expected_ul_bitrate(uint32_t ue_cc_idx, int nof_prbs) const
{
  const cc_sched_ue* cc             = &carriers[ue_cc_idx];
  uint32_t           nof_prbs_alloc = nof_prbs < 0 ? cell.nof_prb : nof_prbs;

  uint32_t N_srs        = 0;
  uint32_t nof_symb     = 2 * (SRSLTE_CP_NSYMB(cell.cp) - 1) - N_srs;
  uint32_t nof_re       = nof_symb * nof_prbs_alloc * SRSLTE_NRE;
  float    max_coderate = srslte_cqi_to_coderate(std::min(cc->ul_cqi + 1u, 15u), false);

  // Inverse of srslte_coderate(tbs, nof_re)
  uint32_t tbs = max_coderate * nof_re - 24;
  return tbs / tti_duration_ms;
}

/// Returns nof bytes allocated to active UL HARQs in the carrier cc_idx.
/// NOTE: The returned value accounts for the MAC header and payload (RLC headers and actual data)
uint32_t sched_ue::get_pending_ul_old_data(uint32_t ue_cc_idx)
{
  uint32_t pending_data = 0;
  for (auto& h : carriers[ue_cc_idx].harq_ent.ul_harq_procs()) {
    pending_data += h.get_pending_data();
  }
  return pending_data;
}

/// Returns the total of all TB bytes allocated to UL HARQs
uint32_t sched_ue::get_pending_ul_old_data()
{
  uint32_t pending_ul_data = 0;
  for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
    pending_ul_data += get_pending_ul_old_data(cc_idx);
  }
  return pending_ul_data;
}

uint32_t sched_ue::get_pending_ul_data_total(tti_point tti_tx_ul, int this_ue_cc_idx)
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
    if (is_sr_triggered() and this_ue_cc_idx >= 0) {
      // Check if this_cc_idx is the carrier with highest CQI
      uint32_t max_cqi = 0, max_cc_idx = 0;
      for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
        uint32_t sum_cqi = carriers[cc_idx].dl_cqi + carriers[cc_idx].ul_cqi;
        if (carriers[cc_idx].cc_state() == cc_st::active and sum_cqi > max_cqi) {
          max_cqi    = sum_cqi;
          max_cc_idx = cc_idx;
        }
      }
      if ((int)max_cc_idx == this_ue_cc_idx) {
        return 512;
      }
    }
    for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
      if (needs_cqi(tti_tx_ul.to_uint(), cc_idx)) {
        return 128;
      }
    }
  }

  return pending_data;
}

uint32_t sched_ue::get_pending_ul_new_data(tti_point tti_tx_ul, int this_ue_cc_idx)
{
  uint32_t pending_data = get_pending_ul_data_total(tti_tx_ul, this_ue_cc_idx);

  // Subtract all the UL data already allocated in the UL harqs
  uint32_t pending_ul_data = get_pending_ul_old_data();
  pending_data             = (pending_data > pending_ul_data) ? pending_data - pending_ul_data : 0;

  if (pending_data > 0) {
    Debug("SCHED: pending_data=%d, in_harq_data=%d, bsr=%s\n",
          pending_data,
          pending_ul_data,
          lch_handler.get_bsr_text().c_str());
  }
  return pending_data;
}

uint32_t sched_ue::get_required_prb_ul(uint32_t cc_idx, uint32_t req_bytes)
{
  return carriers[cc_idx].get_required_prb_ul(req_bytes);
}

bool sched_ue::is_sr_triggered()
{
  return sr;
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(tti_point tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_pending_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_empty_dl_harq(tti_point tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_empty_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

ul_harq_proc* sched_ue::get_ul_harq(tti_point tti_tx_ul, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_ul_harq(tti_tx_ul);
  }
  return nullptr;
}

const dl_harq_proc& sched_ue::get_dl_harq(uint32_t idx, uint32_t ue_cc_idx) const
{
  return carriers[ue_cc_idx].harq_ent.dl_harq_procs()[idx];
}

std::pair<bool, uint32_t> sched_ue::get_active_cell_index(uint32_t enb_cc_idx) const
{
  auto it = std::find_if(
      cfg.supported_cc_list.begin(),
      cfg.supported_cc_list.end(),
      [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& u) { return u.enb_cc_idx == enb_cc_idx and u.active; });
  if (it != cfg.supported_cc_list.end()) {
    uint32_t ue_cc_idx = std::distance(cfg.supported_cc_list.begin(), it);
    return {carriers[ue_cc_idx].cc_state() == cc_st::active, ue_cc_idx};
  }
  return {false, std::numeric_limits<uint32_t>::max()};
}

uint32_t sched_ue::get_aggr_level(uint32_t ue_cc_idx, uint32_t nof_bits)
{
  return carriers[ue_cc_idx].get_aggr_level(nof_bits);
}

void sched_ue::finish_tti(tti_point tti_rx, uint32_t enb_cc_idx)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);

  if (c != nullptr) {
    // Check that scell state needs to change
    c->finish_tti(tti_rx);
  }
}

srslte_dci_format_t sched_ue::get_dci_format()
{
  srslte_dci_format_t ret = SRSLTE_DCI_FORMAT1;

  if (phy_config_dedicated_enabled) {
    /* TODO: Assumes UE-Specific Search Space (Not common) */
    switch (cfg.dl_ant_info.tx_mode) {
      case sched_interface::ant_info_ded_t::tx_mode_t::tm1:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm2:
        ret = SRSLTE_DCI_FORMAT1;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm3:
        ret = SRSLTE_DCI_FORMAT2A;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm4:
        ret = SRSLTE_DCI_FORMAT2;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm5:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm6:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm7:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm8_v920:
      default:
        Warning("Incorrect transmission mode (rnti=%04x; tm=%d)\n", rnti, static_cast<int>(cfg.dl_ant_info.tx_mode));
    }
  }

  return ret;
}

const sched_dci_cce_t* sched_ue::get_locations(uint32_t enb_cc_idx, uint32_t cfi, uint32_t sf_idx) const
{
  if (cfi > 0 && cfi <= 3) {
    return &cells[enb_cc_idx].dci_locations[cfi - 1][sf_idx];
  } else {
    Error("SCHED: Invalid CFI=%d\n", cfi);
    return &cells[enb_cc_idx].dci_locations[0][sf_idx];
  }
}

cc_sched_ue* sched_ue::find_ue_carrier(uint32_t enb_cc_idx)
{
  int ue_cc_idx = enb_to_ue_cc_idx(enb_cc_idx);
  return ue_cc_idx >= 0 ? &carriers[ue_cc_idx] : nullptr;
}

std::bitset<SRSLTE_MAX_CARRIERS> sched_ue::scell_activation_mask() const
{
  std::bitset<SRSLTE_MAX_CARRIERS> ret{0};
  for (size_t i = 1; i < carriers.size(); ++i) {
    if (carriers[i].cc_state() == cc_st::active) {
      ret[i] = true;
    }
  }
  return ret;
}

int sched_ue::enb_to_ue_cc_idx(uint32_t enb_cc_idx) const
{
  return cells.at(enb_cc_idx).get_ue_cc_idx();
}

float diff_coderate_maxcoderate(int      mcs,
                                uint32_t nof_prb,
                                uint32_t nof_re,
                                uint32_t max_Qm,
                                float    max_coderate,
                                bool     use_tbs_index_alt,
                                bool     is_ul)
{
  uint32_t     tbs_idx  = srslte_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  int          tbs      = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
  float        coderate = srslte_coderate(tbs, nof_re);
  srslte_mod_t mod      = (is_ul) ? srslte_ra_ul_mod_from_mcs(mcs) : srslte_ra_dl_mod_from_mcs(mcs, use_tbs_index_alt);
  uint32_t     Qm       = std::min(max_Qm, srslte_mod_bits_x_symbol(mod));
  return coderate - std::min(max_coderate, 0.930f * Qm);
}

int cc_sched_ue::cqi_to_tbs(uint32_t nof_prb, uint32_t nof_re, bool is_ul, uint32_t* mcs)
{
  using ul64qam_cap = sched_interface::ue_cfg_t::ul64qam_cap;
  uint32_t max_Qm;
  int      max_mcs;
  float    max_coderate;
  if (is_ul) {
    max_mcs      = max_mcs_ul;
    max_Qm       = cfg->support_ul64qam == ul64qam_cap::enabled ? 6 : 4;
    max_coderate = srslte_cqi_to_coderate(std::min(ul_cqi + 1u, 15u), false);
  } else {
    max_mcs      = max_mcs_dl;
    max_Qm       = cfg->use_tbs_index_alt ? 8 : 6;
    max_coderate = srslte_cqi_to_coderate(std::min(dl_cqi + 1u, 15u), cfg->use_tbs_index_alt);
  }

  // function with sign-flip at solution
  auto test_mcs = [&](int sel_mcs) -> float {
    return diff_coderate_maxcoderate(sel_mcs, nof_prb, nof_re, max_Qm, max_coderate, cfg->use_tbs_index_alt, is_ul);
  };

  std::tuple<int, float, int, float> ret;
  if (nof_prb > 1) {
    // for non-voip case
    ret = false_position_method(0, max_mcs, 0.0f, test_mcs);
  } else {
    // avoid 6 prbs (voip case), where function is not monotonic
    ret = false_position_method(7, max_mcs, 0.0f, test_mcs);
    if (std::get<1>(ret) > 0) {
      ret = false_position_method(0, 5, 0.0f, test_mcs);
    }
  }
  int      chosen_mcs = std::get<0>(ret);
  uint32_t tbs_idx    = srslte_ra_tbs_idx_from_mcs(chosen_mcs, cfg->use_tbs_index_alt, is_ul);
  int      chosen_tbs = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);

  if (mcs != nullptr) {
    *mcs = (uint32_t)chosen_mcs;
  }
  // If coderate > SRSLTE_MIN(max_coderate, 0.930 * Qm) we should set TBS=0. We don't because it's not correctly
  // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR

  return chosen_tbs;
}

/************************************************************************************************
 *                                sched_ue::sched_ue_carrier
 ***********************************************************************************************/

cc_sched_ue::cc_sched_ue(const sched_interface::ue_cfg_t& cfg_,
                         const sched_ue_cell&             cell_ue_,
                         uint16_t                         rnti_,
                         uint32_t                         ue_cc_idx_,
                         tti_point                        current_tti) :
  cell_ue(&cell_ue_),
  rnti(rnti_),
  log_h(srslte::logmap::get("MAC")),
  ue_cc_idx(ue_cc_idx_),
  last_tti(current_tti),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC),
  tpc_fsm(cell_ue_.cell_cfg->nof_prb(),
          cell_ue_.cell_cfg->cfg.target_ul_sinr,
          cell_ue_.cell_cfg->cfg.enable_phr_handling)
{
  dl_cqi_rx = false;
  dl_cqi    = (ue_cc_idx == 0) ? cell_ue_.cell_cfg->cfg.initial_dl_cqi : 0;
  set_cfg(cfg_);

  max_aggr_level = cell_ue->cell_cfg->sched_cfg->max_aggr_level >= 0 ? cell_ue->cell_cfg->sched_cfg->max_aggr_level : 3;

  // set fixed mcs
  fixed_mcs_dl = cell_ue->cell_cfg->sched_cfg->pdsch_mcs;
  fixed_mcs_ul = cell_ue->cell_cfg->sched_cfg->pusch_mcs;
}

void cc_sched_ue::reset()
{
  dl_ri         = 0;
  dl_ri_tti_rx  = tti_point{};
  dl_pmi        = 0;
  dl_pmi_tti_rx = tti_point{};
  dl_cqi        = 1;
  dl_cqi_tti_rx = tti_point{};
  ul_cqi        = 1;
  ul_cqi_tti_rx = tti_point{};
  harq_ent.reset();
}

void cc_sched_ue::set_cfg(const sched_interface::ue_cfg_t& cfg_)
{
  cfg     = &cfg_;
  cfg_tti = last_tti;

  // set max mcs
  max_mcs_ul = get_cell_cfg()->sched_cfg->pusch_max_mcs >= 0 ? get_cell_cfg()->sched_cfg->pusch_max_mcs : 28u;
  if (get_cell_cfg()->cfg.enable_64qam) {
    const uint32_t max_64qam_mcs[] = {20, 24, 28};
    max_mcs_ul                     = std::min(max_mcs_ul, max_64qam_mcs[(size_t)cfg->support_ul64qam]);
  }
  max_mcs_dl =
      get_cell_cfg()->sched_cfg->pdsch_max_mcs >= 0 ? std::min(get_cell_cfg()->sched_cfg->pdsch_max_mcs, 28) : 28u;
  if (cfg->use_tbs_index_alt) {
    max_mcs_dl = std::min(max_mcs_dl, 27u);
  }

  if (ue_cc_idx == 0) {
    // PCell is always active
    cc_state_ = cc_st::active;
  } else {
    switch (cc_state()) {
      case cc_st::activating:
      case cc_st::active:
        if (not cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::deactivating;
          log_h->info("SCHED: Deactivating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::activating;
          dl_cqi_rx = false;
          dl_cqi    = 0;
          log_h->info("SCHED: Activating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      default:
        break;
    }
  }
}

void cc_sched_ue::finish_tti(tti_point tti_rx)
{
  last_tti = tti_rx;

  // reset PIDs with pending data or blocked
  harq_ent.reset_pending_data(last_tti);

  // Check if cell state needs to be updated
  if (ue_cc_idx > 0 and cc_state_ == cc_st::deactivating) {
    // wait for all ACKs to be received before completely deactivating SCell
    if (last_tti > to_tx_dl_ack(cfg_tti)) {
      cc_state_ = cc_st::idle;
      reset();
    }
  }
}

uint32_t cc_sched_ue::get_aggr_level(uint32_t nof_bits)
{
  return srsenb::get_aggr_level(nof_bits, dl_cqi, max_aggr_level, get_cell_cfg()->nof_prb(), cfg->use_tbs_index_alt);
}

/* In this scheduler we tend to use all the available bandwidth and select the MCS
 * that approximates the minimum between the capacity and the requested rate
 */
tbs_info cc_sched_ue::alloc_tbs(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul)
{
  tbs_info ret;
  uint32_t sel_mcs = 0;

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  int tbs_bytes = cqi_to_tbs(nof_prb, nof_re, is_ul, &sel_mcs) / 8;

  /* If less bytes are requested, lower the MCS */
  if (tbs_bytes > (int)req_bytes && req_bytes > 0) {
    int req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes * 8, nof_prb);
    int req_mcs     = srslte_ra_mcs_from_tbs_idx(req_tbs_idx, cfg->use_tbs_index_alt, is_ul);
    while (cfg->use_tbs_index_alt and req_mcs < 0 and req_tbs_idx < 33) {
      // some tbs_idx are invalid for 256QAM. See TS 36.213 - Table 7.1.7.1-1A
      req_mcs = srslte_ra_mcs_from_tbs_idx(++req_tbs_idx, cfg->use_tbs_index_alt, is_ul);
    }

    if (req_mcs >= 0 and req_mcs < (int)sel_mcs) {
      uint32_t max_Qm = (is_ul) ? (cfg->support_ul64qam == sched_interface::ue_cfg_t::ul64qam_cap::enabled ? 6 : 4)
                                : (cfg->use_tbs_index_alt ? 8 : 6);
      float max_coderate = (is_ul) ? srslte_cqi_to_coderate(std::min(ul_cqi + 1u, 15u), false)
                                   : srslte_cqi_to_coderate(std::min(dl_cqi + 1u, 15u), cfg->use_tbs_index_alt);
      if (diff_coderate_maxcoderate(req_mcs, nof_prb, nof_re, max_Qm, max_coderate, cfg->use_tbs_index_alt, is_ul) <
          0) {
        sel_mcs   = req_mcs;
        tbs_bytes = srslte_ra_tbs_from_idx(req_tbs_idx, nof_prb) / 8;
      }
    }
  }
  // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
  if (nof_prb == 1 && sel_mcs == 6) {
    sel_mcs--;
    tbs_bytes = get_tbs_bytes(sel_mcs, nof_prb, cfg->use_tbs_index_alt, is_ul);
  }

  ret.tbs_bytes = tbs_bytes;
  if (ret.tbs_bytes >= 0) {
    ret.mcs = (int)sel_mcs;
  }

  return ret;
}

tbs_info cc_sched_ue::alloc_tbs_dl(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes)
{
  tbs_info ret;

  // Use a higher MCS for the Msg4 to fit in the 6 PRB case
  if (fixed_mcs_dl < 0 or not dl_cqi_rx) {
    // Dynamic MCS
    ret = alloc_tbs(nof_prb, nof_re, req_bytes, false);
  } else {
    // Fixed MCS
    ret.mcs       = fixed_mcs_dl;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)fixed_mcs_dl, nof_prb, cfg->use_tbs_index_alt, false);
  }
  return ret;
}

tbs_info cc_sched_ue::alloc_tbs_ul(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int explicit_mcs)
{
  tbs_info ret;
  int      mcs = explicit_mcs >= 0 ? explicit_mcs : fixed_mcs_ul;

  if (mcs < 0) {
    // Dynamic MCS
    ret = alloc_tbs(nof_prb, nof_re, req_bytes, true);
  } else {
    // Fixed MCS
    ret.mcs       = mcs;
    ret.tbs_bytes = get_tbs_bytes((uint32_t)mcs, nof_prb, false, true);
  }

  return ret;
}

int cc_sched_ue::get_required_prb_dl(tti_point tti_tx_dl, uint32_t req_bytes)
{
  auto compute_tbs_approx = [tti_tx_dl, this](uint32_t nof_prb) {
    uint32_t nof_re = cell_ue->cell_cfg->get_dl_lb_nof_re(tti_tx_dl, nof_prb);
    tbs_info tb     = alloc_tbs_dl(nof_prb, nof_re, 0);
    return tb.tbs_bytes;
  };

  std::tuple<uint32_t, int, uint32_t, int> ret = false_position_method(
      1u, get_cell_cfg()->nof_prb(), (int)req_bytes, compute_tbs_approx, [](int y) { return y == SRSLTE_ERROR; });
  int      upper_tbs  = std::get<3>(ret);
  uint32_t upper_nprb = std::get<2>(ret);
  return (upper_tbs < 0) ? 0 : ((upper_tbs < (int)req_bytes) ? -1 : upper_nprb);
}

uint32_t cc_sched_ue::get_required_prb_ul(uint32_t req_bytes)
{
  if (req_bytes == 0) {
    return 0;
  }
  auto compute_tbs_approx = [this](uint32_t nof_prb) {
    const uint32_t N_srs  = 0;
    uint32_t       nof_re = (2 * (SRSLTE_CP_NSYMB(get_cell_cfg()->cfg.cell.cp) - 1) - N_srs) * nof_prb * SRSLTE_NRE;
    return alloc_tbs_ul(nof_prb, nof_re, 0).tbs_bytes;
  };

  // find nof prbs that lead to a tbs just above req_bytes
  int                                      target_tbs = req_bytes + 4;
  uint32_t                                 max_prbs   = std::min(tpc_fsm.max_ul_prbs(), get_cell_cfg()->nof_prb());
  std::tuple<uint32_t, int, uint32_t, int> ret =
      false_position_method(1u, max_prbs, target_tbs, compute_tbs_approx, [](int y) { return y == SRSLTE_ERROR; });
  uint32_t req_prbs = std::get<2>(ret);
  while (!srslte_dft_precoding_valid_prb(req_prbs) && req_prbs < get_cell_cfg()->nof_prb()) {
    req_prbs++;
  }
  return req_prbs;
}

void cc_sched_ue::set_dl_cqi(tti_point tti_rx, uint32_t dl_cqi_)
{
  dl_cqi        = dl_cqi_;
  dl_cqi_tti_rx = tti_rx;
  dl_cqi_rx     = dl_cqi_rx or dl_cqi > 0;
  if (ue_cc_idx > 0 and cc_state_ == cc_st::activating and dl_cqi_rx) {
    // Wait for SCell to receive a positive CQI before activating it
    cc_state_ = cc_st::active;
    log_h->info("SCHED: SCell index=%d is now active\n", ue_cc_idx);
  }
}

} // namespace srsenb
