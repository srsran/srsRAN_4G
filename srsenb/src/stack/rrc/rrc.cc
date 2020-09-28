/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/rrc.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srslte/asn1/asn1_utils.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/int_helpers.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/srslte.h"

using srslte::byte_buffer_t;
using srslte::uint32_to_uint8;
using srslte::uint8_to_uint32;

using namespace asn1::rrc;

namespace srsenb {

rrc::rrc(srslte::task_sched_handle task_sched_) : rrc_log("RRC"), task_sched(task_sched_)
{
  pending_paging.clear();
}

rrc::~rrc() {}

void rrc::init(const rrc_cfg_t&       cfg_,
               phy_interface_rrc_lte* phy_,
               mac_interface_rrc*     mac_,
               rlc_interface_rrc*     rlc_,
               pdcp_interface_rrc*    pdcp_,
               s1ap_interface_rrc*    s1ap_,
               gtpu_interface_rrc*    gtpu_)
{
  phy  = phy_;
  mac  = mac_;
  rlc  = rlc_;
  pdcp = pdcp_;
  gtpu = gtpu_;
  s1ap = s1ap_;

  pool = srslte::byte_buffer_pool::get_instance();

  cfg = cfg_;

  if (cfg.sibs[12].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib13_v920 &&
      cfg.enable_mbsfn) {
    configure_mbsfn_sibs(&cfg.sibs[1].sib2(), &cfg.sibs[12].sib13_v920());
  }

  pucch_res_list.reset(new freq_res_common_list{cfg});

  // Loads the PRACH root sequence
  cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.root_seq_idx = cfg.cell_list[0].root_seq_idx;

  nof_si_messages = generate_sibs();
  config_mac();

  // Check valid inactivity timeout config
  uint32_t t310 = cfg.sibs[1].sib2().ue_timers_and_consts.t310.to_number();
  uint32_t t311 = cfg.sibs[1].sib2().ue_timers_and_consts.t311.to_number();
  uint32_t n310 = cfg.sibs[1].sib2().ue_timers_and_consts.n310.to_number();
  rrc_log->info("T310 %d, T311 %d, N310 %d \n", t310, t311, n310);
  if (cfg.inactivity_timeout_ms < t310 + t311 + n310) {
    srslte::console("\nWarning: Inactivity timeout is smaller than the sum of t310, t311 and n310.\n"
                       "This may break the UE's re-establishment procedure.\n");
    rrc_log->warning("Inactivity timeout is smaller than the sum of t310, t311 and n310. This may break the UE's "
                     "re-establishment procedure.\n");
  }
  rrc_log->info("Inactivity timeout: %d ms\n", cfg.inactivity_timeout_ms);

  running = true;
}

void rrc::stop()
{
  if (running) {
    running   = false;
    rrc_pdu p = {0, LCID_EXIT, nullptr};
    rx_pdu_queue.push(std::move(p));
  }
  users.clear();
}

/*******************************************************************************
  Public functions
*******************************************************************************/

void rrc::get_metrics(rrc_metrics_t& m)
{
  if (running) {
    m.n_ues = 0;
    for (auto iter = users.begin(); m.n_ues < ENB_METRICS_MAX_USERS && iter != users.end(); ++iter) {
      ue* u                  = iter->second.get();
      m.ues[m.n_ues++].state = u->get_state();
    }
  }
}

/*******************************************************************************
  MAC interface

  Those functions that shall be called from a phch_worker should push the command
  to the queue and process later
*******************************************************************************/

uint8_t* rrc::read_pdu_bcch_dlsch(const uint8_t cc_idx, const uint32_t sib_index)
{
  if (sib_index < ASN1_RRC_MAX_SIB && cc_idx < cell_common_list->nof_cells()) {
    return cell_common_list->get_cc_idx(cc_idx)->sib_buffer.at(sib_index)->msg;
  }
  return nullptr;
}

void rrc::set_activity_user(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_ACT_USER, nullptr};
  rx_pdu_queue.push(std::move(p));
}

void rrc::rem_user_thread(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_REM_USER, nullptr};
  rx_pdu_queue.push(std::move(p));
}

uint32_t rrc::get_nof_users()
{
  return users.size();
}

void rrc::max_retx_attempted(uint16_t rnti) {}

// This function is called from PRACH worker (can wait)
void rrc::add_user(uint16_t rnti, const sched_interface::ue_cfg_t& sched_ue_cfg)
{
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    bool rnti_added = true;
    if (rnti != SRSLTE_MRNTI) {
      // only non-eMBMS RNTIs are present in user map
      auto p     = users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue{this, rnti, sched_ue_cfg})));
      rnti_added = p.second and p.first->second->is_allocated();
    }
    if (rnti_added) {
      rlc->add_user(rnti);
      pdcp->add_user(rnti);
      rrc_log->info("Added new user rnti=0x%x\n", rnti);
    } else {
      mac->bearer_ue_rem(rnti, 0);
      rrc_log->error("Adding user rnti=0x%x - Failed to allocate user resources\n", rnti);
    }
  } else {
    rrc_log->error("Adding user rnti=0x%x (already exists)\n", rnti);
  }

  if (rnti == SRSLTE_MRNTI) {
    uint32_t teid_in = 1;
    for (auto& mbms_item : mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9) {
      uint32_t lcid = mbms_item.lc_ch_id_r9;

      // adding UE object to MAC for MRNTI without scheduling configuration (broadcast not part of regular scheduling)
      mac->ue_cfg(SRSLTE_MRNTI, NULL);
      rlc->add_bearer_mrb(SRSLTE_MRNTI, lcid);
      pdcp->add_bearer(SRSLTE_MRNTI, lcid, srslte::make_drb_pdcp_config_t(1, false));
      teid_in = gtpu->add_bearer(SRSLTE_MRNTI, lcid, 1, 1);
    }
  }
}

/* Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a
 * valid RNTI.
 */
void rrc::upd_user(uint16_t new_rnti, uint16_t old_rnti)
{
  // Remove new_rnti
  rem_user_thread(new_rnti);

  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  auto old_it = users.find(old_rnti);
  if (old_it != users.end()) {
    auto ue_ptr = old_it->second.get();
    if (ue_ptr->mobility_handler->is_ho_running()) {
      ue_ptr->mobility_handler->trigger(ue::rrc_mobility::user_crnti_upd_ev{old_rnti, new_rnti});
    } else if (ue_ptr->is_connected()) {
      old_it->second->send_connection_reconf_upd(srslte::allocate_unique_buffer(*pool));
    } else {
      old_it->second->send_connection_reject();
    }
  }
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  rrc_pdu p = {rnti, lcid, std::move(pdu)};
  rx_pdu_queue.push(std::move(p));
}

/*******************************************************************************
  S1AP interface
*******************************************************************************/
void rrc::write_dl_info(uint16_t rnti, srslte::unique_byte_buffer_t sdu)
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1();
  dl_dcch_msg_type_c::c1_c_* msg_c1 = &dl_dcch_msg.msg.c1();

  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    dl_info_transfer_r8_ies_s* dl_info_r8 =
        &msg_c1->set_dl_info_transfer().crit_exts.set_c1().set_dl_info_transfer_r8();
    //    msg_c1->dl_info_transfer().rrc_transaction_id = ;
    dl_info_r8->non_crit_ext_present = false;
    dl_info_r8->ded_info_type.set_ded_info_nas();
    dl_info_r8->ded_info_type.ded_info_nas().resize(sdu->N_bytes);
    memcpy(msg_c1->dl_info_transfer().crit_exts.c1().dl_info_transfer_r8().ded_info_type.ded_info_nas().data(),
           sdu->msg,
           sdu->N_bytes);

    sdu->clear();

    user_it->second->send_dl_dcch(&dl_dcch_msg, std::move(sdu));
  } else {
    rrc_log->error("Rx SDU for unknown rnti=0x%x\n", rnti);
  }
}

void rrc::release_complete(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_REL_USER, nullptr};
  rx_pdu_queue.push(std::move(p));
}

bool rrc::setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg)
{
  rrc_log->info("Adding initial context for 0x%x\n", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  user_it->second->handle_ue_init_ctxt_setup_req(msg);
  return true;
}

bool rrc::modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)
{
  rrc_log->info("Modifying context for 0x%x\n", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  return user_it->second->handle_ue_ctxt_mod_req(msg);
}

bool rrc::setup_ue_erabs(uint16_t rnti, const asn1::s1ap::erab_setup_request_s& msg)
{
  rrc_log->info("Setting up erab(s) for 0x%x\n", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  if (msg.protocol_ies.ueaggregate_maximum_bitrate_present) {
    // UEAggregateMaximumBitrate
    user_it->second->set_bitrates(msg.protocol_ies.ueaggregate_maximum_bitrate.value);
  }

  // Setup E-RABs
  user_it->second->setup_erabs(msg.protocol_ies.erab_to_be_setup_list_bearer_su_req.value);

  return true;
}

bool rrc::release_erabs(uint32_t rnti)
{
  rrc_log->info("Releasing E-RABs for 0x%x\n", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  bool ret = user_it->second->release_erabs();
  return ret;
}

/*******************************************************************************
  Paging functions
  These functions use a different mutex because access different shared variables
  than user map
*******************************************************************************/

void rrc::add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id)
{
  std::lock_guard<std::mutex> lock(paging_mutex);
  if (pending_paging.count(ueid) > 0) {
    rrc_log->warning("Received Paging for UEID=%d but not yet transmitted\n", ueid);
    return;
  }

  paging_record_s paging_elem;
  if (ue_paging_id.type().value == asn1::s1ap::ue_paging_id_c::types_opts::imsi) {
    paging_elem.ue_id.set_imsi();
    paging_elem.ue_id.imsi().resize(ue_paging_id.imsi().size());
    memcpy(paging_elem.ue_id.imsi().data(), ue_paging_id.imsi().data(), ue_paging_id.imsi().size());
    srslte::console("Warning IMSI paging not tested\n");
  } else {
    paging_elem.ue_id.set_s_tmsi();
    paging_elem.ue_id.s_tmsi().mmec.from_number(ue_paging_id.s_tmsi().mmec[0]);
    uint32_t m_tmsi     = 0;
    uint32_t nof_octets = ue_paging_id.s_tmsi().m_tmsi.size();
    for (uint32_t i = 0; i < nof_octets; i++) {
      m_tmsi |= ue_paging_id.s_tmsi().m_tmsi[i] << (8u * (nof_octets - i - 1u));
    }
    paging_elem.ue_id.s_tmsi().m_tmsi.from_number(m_tmsi);
  }
  paging_elem.cn_domain = paging_record_s::cn_domain_e_::ps;

  pending_paging.insert(std::make_pair(ueid, paging_elem));
}

// Described in Section 7 of 36.304
bool rrc::is_paging_opportunity(uint32_t tti, uint32_t* payload_len)
{
  constexpr static int sf_pattern[4][4] = {{9, 4, -1, 0}, {-1, 9, -1, 4}, {-1, -1, -1, 5}, {-1, -1, -1, 9}};

  if (tti == paging_tti) {
    *payload_len = byte_buf_paging.N_bytes;
    rrc_log->debug("Sending paging to extra carriers. Payload len=%d, TTI=%d\n", *payload_len, tti);
    return true;
  } else {
    paging_tti = INVALID_TTI;
  }

  if (pending_paging.empty()) {
    return false;
  }

  asn1::rrc::pcch_msg_s pcch_msg;
  pcch_msg.msg.set_c1();
  paging_s* paging_rec = &pcch_msg.msg.c1().paging();

  // Default paging cycle, should get DRX from user
  uint32_t T  = cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.default_paging_cycle.to_number();
  uint32_t Nb = T * cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.nb.to_number();

  uint32_t N   = T < Nb ? T : Nb;
  uint32_t Ns  = Nb / T > 1 ? Nb / T : 1;
  uint32_t sfn = tti / 10;

  std::vector<uint32_t> ue_to_remove;

  {
    std::lock_guard<std::mutex> lock(paging_mutex);

    int n = 0;
    for (auto& item : pending_paging) {
      if (n >= ASN1_RRC_MAX_PAGE_REC) {
        break;
      }
      const asn1::rrc::paging_record_s& u    = item.second;
      uint32_t                          ueid = ((uint32_t)item.first) % 1024;
      uint32_t                          i_s  = (ueid / N) % Ns;

      if ((sfn % T) != (T / N) * (ueid % N)) {
        continue;
      }

      int sf_idx = sf_pattern[i_s % 4][(Ns - 1) % 4];
      if (sf_idx < 0) {
        rrc_log->error("SF pattern is N/A for Ns=%d, i_s=%d, imsi_decimal=%d\n", Ns, i_s, ueid);
        continue;
      }

      if ((uint32_t)sf_idx == (tti % 10)) {
        paging_rec->paging_record_list_present = true;
        paging_rec->paging_record_list.push_back(u);
        ue_to_remove.push_back(ueid);
        n++;
        rrc_log->info("Assembled paging for ue_id=%d, tti=%d\n", ueid, tti);
      }
    }

    for (unsigned int i : ue_to_remove) {
      pending_paging.erase(i);
    }
  }

  if (paging_rec->paging_record_list.size() > 0) {
    byte_buf_paging.clear();
    asn1::bit_ref bref(byte_buf_paging.msg, byte_buf_paging.get_tailroom());
    if (pcch_msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      rrc_log->error("Failed to pack PCCH\n");
      return false;
    }
    byte_buf_paging.N_bytes = (uint32_t)bref.distance_bytes();
    uint32_t N_bits         = (uint32_t)bref.distance();

    if (payload_len) {
      *payload_len = byte_buf_paging.N_bytes;
    }
    rrc_log->info("Assembling PCCH payload with %d UE identities, payload_len=%d bytes, nbits=%d\n",
                  paging_rec->paging_record_list.size(),
                  byte_buf_paging.N_bytes,
                  N_bits);
    log_rrc_message("PCCH-Message", Tx, &byte_buf_paging, pcch_msg, pcch_msg.msg.c1().type().to_string());

    paging_tti = tti; // Store paging tti for other carriers
    return true;
  }

  return false;
}

void rrc::read_pdu_pcch(uint8_t* payload, uint32_t buffer_size)
{
  std::lock_guard<std::mutex> lock(paging_mutex);
  if (byte_buf_paging.N_bytes <= buffer_size) {
    memcpy(payload, byte_buf_paging.msg, byte_buf_paging.N_bytes);
  }
}

/*******************************************************************************
  Handover functions
*******************************************************************************/

void rrc::ho_preparation_complete(uint16_t rnti, bool is_success, srslte::unique_byte_buffer_t rrc_container)
{
  users.at(rnti)->mobility_handler->handle_ho_preparation_complete(is_success, std::move(rrc_container));
}

void rrc::set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs)
{
  auto ue_it = users.find(rnti);
  if (ue_it == users.end()) {
    rrc_log->warning("rnti=0x%x does not exist\n", rnti);
    return;
  }
  ue_it->second->mobility_handler->trigger(erabs);
}

/*******************************************************************************
  Private functions
  All private functions are not mutexed and must be called from a mutexed environment
  from either a public function or the internal thread
*******************************************************************************/

void rrc::parse_ul_ccch(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  if (pdu) {
    ul_ccch_msg_s  ul_ccch_msg;
    asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      rrc_log->error("Failed to unpack UL-CCCH message\n");
      return;
    }

    log_rrc_message("SRB0", Rx, pdu.get(), ul_ccch_msg, ul_ccch_msg.msg.c1().type().to_string());

    auto user_it = users.find(rnti);
    switch (ul_ccch_msg.msg.c1().type().value) {
      case ul_ccch_msg_type_c::c1_c_::types::rrc_conn_request:
        if (user_it != users.end()) {
          user_it->second->handle_rrc_con_req(&ul_ccch_msg.msg.c1().rrc_conn_request());
        } else {
          rrc_log->error("Received ConnectionSetup for rnti=0x%x without context\n", rnti);
        }
        break;
      case ul_ccch_msg_type_c::c1_c_::types::rrc_conn_reest_request:
        if (user_it != users.end()) {
          user_it->second->handle_rrc_con_reest_req(&ul_ccch_msg.msg.c1().rrc_conn_reest_request());
        } else {
          rrc_log->error("Received ConnectionReestablishment for rnti=0x%x without context.\n", rnti);
        }
        break;
      default:
        rrc_log->error("UL CCCH message not recognised\n");
        break;
    }
  }
}

///< User mutex must be hold by caller
void rrc::parse_ul_dcch(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  if (pdu) {
    auto user_it = users.find(rnti);
    if (user_it != users.end()) {
      user_it->second->parse_ul_dcch(lcid, std::move(pdu));
    } else {
      rrc_log->error("Processing %s: Unknown rnti=0x%x\n", srsenb::to_string((rb_id_t)lcid), rnti);
    }
  }
}

///< User mutex must be hold by caller
void rrc::process_release_complete(uint16_t rnti)
{
  rrc_log->info("Received Release Complete rnti=0x%x\n", rnti);
  auto user_it = users.find(rnti);
  if (user_it == users.end()) {
    rrc_log->error("Received ReleaseComplete for unknown rnti=0x%x\n", rnti);
    return;
  }
  ue* u = user_it->second.get();

  if (u->is_idle() or u->mobility_handler->is_ho_running()) {
    rem_user_thread(rnti);
  } else if (not u->is_idle()) {
    rlc->clear_buffer(rnti);
    user_it->second->send_connection_release();
    // delay user deletion for ~50 TTI (until RRC release is sent)
    task_sched.defer_callback(50, [this, rnti]() { rem_user_thread(rnti); });
  }
}

void rrc::rem_user(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    srslte::console("Disconnecting rnti=0x%x.\n", rnti);
    rrc_log->info("Disconnecting rnti=0x%x.\n", rnti);

    /* First remove MAC and GTPU to stop processing DL/UL traffic for this user
     */
    mac->ue_rem(rnti); // MAC handles PHY
    gtpu->rem_user(rnti);

    // Now remove RLC and PDCP
    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);

    users.erase(rnti);
    rrc_log->info("Removed user rnti=0x%x\n", rnti);
  } else {
    rrc_log->error("Removing user rnti=0x%x (does not exist)\n", rnti);
  }
}

void rrc::config_mac()
{
  using sched_cell_t = sched_interface::cell_cfg_t;

  // Fill MAC scheduler configuration for SIBs
  std::vector<sched_cell_t> sched_cfg;
  sched_cfg.resize(cfg.cell_list.size());

  for (uint32_t ccidx = 0; ccidx < cfg.cell_list.size(); ++ccidx) {
    sched_interface::cell_cfg_t& item = sched_cfg[ccidx];

    // set sib/prach cfg
    for (uint32_t i = 0; i < nof_si_messages; i++) {
      item.sibs[i].len = cell_common_list->get_cc_idx(ccidx)->sib_buffer.at(i)->N_bytes;
      if (i == 0) {
        item.sibs[i].period_rf = 8; // SIB1 is always 8 rf
      } else {
        item.sibs[i].period_rf = cfg.sib1.sched_info_list[i - 1].si_periodicity.to_number();
      }
    }
    item.prach_config        = cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_cfg_idx;
    item.prach_nof_preambles = cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.preamb_info.nof_ra_preambs.to_number();
    item.si_window_ms        = cfg.sib1.si_win_len.to_number();
    item.prach_rar_window =
        cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.ra_supervision_info.ra_resp_win_size.to_number();
    item.prach_freq_offset = cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;
    item.maxharq_msg3tx    = cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.max_harq_msg3_tx;
    item.initial_dl_cqi    = cfg.cell_list[ccidx].initial_dl_cqi;

    item.nrb_pucch = SRSLTE_MAX(cfg.sr_cfg.nof_prb, cfg.cqi_cfg.nof_prb);
    rrc_log->info("Allocating %d PRBs for PUCCH\n", item.nrb_pucch);

    // Copy base cell configuration
    item.cell = cfg.cell;

    // copy secondary cell list info
    sched_cfg[ccidx].scell_list.reserve(cfg.cell_list[ccidx].scell_list.size());
    for (uint32_t scidx = 0; scidx < cfg.cell_list[ccidx].scell_list.size(); ++scidx) {
      const auto& scellitem = cfg.cell_list[ccidx].scell_list[scidx];
      // search enb_cc_idx specific to cell_id
      auto it = std::find_if(cfg.cell_list.begin(), cfg.cell_list.end(), [&scellitem](const cell_cfg_t& e) {
        return e.cell_id == scellitem.cell_id;
      });
      if (it == cfg.cell_list.end()) {
        rrc_log->warning("Secondary cell 0x%x not configured\n", scellitem.cell_id);
        continue;
      }
      sched_interface::cell_cfg_t::scell_cfg_t scellcfg;
      scellcfg.enb_cc_idx               = it - cfg.cell_list.begin();
      scellcfg.ul_allowed               = scellitem.ul_allowed;
      scellcfg.cross_carrier_scheduling = scellitem.cross_carrier_sched;
      sched_cfg[ccidx].scell_list.push_back(scellcfg);
    }
  }

  // Configure MAC scheduler
  mac->cell_cfg(sched_cfg);
}

/* This methods packs the SIBs for each component carrier and stores them
 * inside the sib_buffer, a vector of SIBs for each CC.
 *
 * Before packing the message, it patches the cell specific params of
 * the SIB, including the cellId and the PRACH config index.
 *
 * @return The number of SIBs messages per CC
 */
uint32_t rrc::generate_sibs()
{
  // nof_messages includes SIB2 by default, plus all configured SIBs
  uint32_t           nof_messages = 1 + cfg.sib1.sched_info_list.size();
  sched_info_list_l& sched_info   = cfg.sib1.sched_info_list;

  // Store configs,SIBs in common cell ctxt list
  cell_common_list.reset(new cell_info_common_list{cfg});

  // generate and pack into SIB buffers
  for (uint32_t cc_idx = 0; cc_idx < cfg.cell_list.size(); cc_idx++) {
    cell_info_common* cell_ctxt = cell_common_list->get_cc_idx(cc_idx);
    // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
    // all SIBs in a SI message msg[i] share the same periodicity
    asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

    // Copy SIB1 to first SI message
    msg[0].msg.set_c1().set_sib_type1() = cell_ctxt->sib1;

    // Copy rest of SIBs
    for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages - 1; sched_info_elem++) {
      uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

      msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info_r8();
      sys_info_r8_ies_s::sib_type_and_info_l_& sib_list =
          msg[msg_index].msg.c1().sys_info().crit_exts.sys_info_r8().sib_type_and_info;

      // SIB2 always in second SI message
      if (msg_index == 1) {
        sib_info_item_c sibitem;
        sibitem.set_sib2() = cell_ctxt->sib2;
        sib_list.push_back(sibitem);
      }

      // Add other SIBs to this message, if any
      for (auto& mapping_enum : sched_info[sched_info_elem].sib_map_info) {
        sib_list.push_back(cfg.sibs[(int)mapping_enum + 2]);
      }
    }

    // Pack payload for all messages
    for (uint32_t msg_index = 0; msg_index < nof_messages; msg_index++) {
      srslte::unique_byte_buffer_t sib_buffer = srslte::allocate_unique_buffer(*pool);
      asn1::bit_ref                bref(sib_buffer->msg, sib_buffer->get_tailroom());
      if (msg[msg_index].pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
        rrc_log->error("Failed to pack SIB message %d\n", msg_index);
      }
      sib_buffer->N_bytes = bref.distance_bytes();
      cell_ctxt->sib_buffer.push_back(std::move(sib_buffer));

      // Log SIBs in JSON format
      std::string log_msg("CC" + std::to_string(cc_idx) + " SIB payload");
      log_rrc_message(
          log_msg, Tx, cell_ctxt->sib_buffer.back().get(), msg[msg_index], msg[msg_index].msg.c1().type().to_string());
    }

    if (cfg.sibs[6].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib7) {
      sib7 = cfg.sibs[6].sib7();
    }
  }

  return nof_messages;
}

void rrc::configure_mbsfn_sibs(sib_type2_s* sib2_, sib_type13_r9_s* sib13_)
{
  // Temp assignment of MCCH, this will eventually come from a cfg file
  mcch.msg.set_c1();
  mbsfn_area_cfg_r9_s& area_cfg_r9      = mcch.msg.c1().mbsfn_area_cfg_r9();
  area_cfg_r9.common_sf_alloc_period_r9 = mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_::rf64;
  area_cfg_r9.common_sf_alloc_r9.resize(1);
  mbsfn_sf_cfg_s* sf_alloc_item          = &area_cfg_r9.common_sf_alloc_r9[0];
  sf_alloc_item->radioframe_alloc_offset = 0;
  sf_alloc_item->radioframe_alloc_period = mbsfn_sf_cfg_s::radioframe_alloc_period_e_::n1;
  sf_alloc_item->sf_alloc.set_one_frame().from_number(32 + 31);

  area_cfg_r9.pmch_info_list_r9.resize(1);
  pmch_info_r9_s* pmch_item = &area_cfg_r9.pmch_info_list_r9[0];
  pmch_item->mbms_session_info_list_r9.resize(1);

  pmch_item->mbms_session_info_list_r9[0].lc_ch_id_r9           = 1;
  pmch_item->mbms_session_info_list_r9[0].session_id_r9_present = true;
  pmch_item->mbms_session_info_list_r9[0].session_id_r9[0]      = 0;
  pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.set_explicit_value_r9();
  srslte::plmn_id_t plmn_obj;
  plmn_obj.from_string("00003");
  srslte::to_asn1(&pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9(), plmn_obj);
  uint8_t byte[] = {0x0, 0x0, 0x0};
  memcpy(&pmch_item->mbms_session_info_list_r9[0].tmgi_r9.service_id_r9[0], &byte[0], 3);

  if (pmch_item->mbms_session_info_list_r9.size() > 1) {
    pmch_item->mbms_session_info_list_r9[1].lc_ch_id_r9           = 2;
    pmch_item->mbms_session_info_list_r9[1].session_id_r9_present = true;
    pmch_item->mbms_session_info_list_r9[1].session_id_r9[0]      = 1;
    pmch_item->mbms_session_info_list_r9[1].tmgi_r9.plmn_id_r9.set_explicit_value_r9() =
        pmch_item->mbms_session_info_list_r9[0].tmgi_r9.plmn_id_r9.explicit_value_r9();
    byte[2] = 1;
    memcpy(&pmch_item->mbms_session_info_list_r9[1].tmgi_r9.service_id_r9[0],
           &byte[0],
           3); // TODO: Check if service is set to 1
  }

  uint16_t mbms_mcs = cfg.mbms_mcs;
  if (mbms_mcs > 28) {
    mbms_mcs = 28; // TS 36.213, Table 8.6.1-1
    rrc_log->warning("PMCH data MCS too high, setting it to 28\n");
  }

  rrc_log->debug("PMCH data MCS=%d\n", mbms_mcs);
  pmch_item->pmch_cfg_r9.data_mcs_r9         = mbms_mcs;
  pmch_item->pmch_cfg_r9.mch_sched_period_r9 = pmch_cfg_r9_s::mch_sched_period_r9_e_::rf64;
  pmch_item->pmch_cfg_r9.sf_alloc_end_r9     = 64 * 6;

  phy->configure_mbsfn(sib2_, sib13_, mcch);
  mac->write_mcch(sib2_, sib13_, &mcch);
}

/*******************************************************************************
  RRC run tti method
*******************************************************************************/

void rrc::tti_clock()
{
  // pop cmds from queue
  rrc_pdu p;
  while (rx_pdu_queue.try_pop(&p)) {
    // print Rx PDU
    if (p.pdu != nullptr) {
      rrc_log->info_hex(p.pdu->msg, p.pdu->N_bytes, "Rx %s PDU", to_string((rb_id_t)p.lcid));
    }

    // check if user exists
    auto user_it = users.find(p.rnti);
    if (user_it == users.end()) {
      rrc_log->warning("Discarding PDU for removed rnti=0x%x\n", p.rnti);
      continue;
    }

    // handle queue cmd
    switch (p.lcid) {
      case RB_ID_SRB0:
        parse_ul_ccch(p.rnti, std::move(p.pdu));
        break;
      case RB_ID_SRB1:
      case RB_ID_SRB2:
        parse_ul_dcch(p.rnti, p.lcid, std::move(p.pdu));
        break;
      case LCID_REM_USER:
        rem_user(p.rnti);
        break;
      case LCID_REL_USER:
        process_release_complete(p.rnti);
        break;
      case LCID_ACT_USER:
        user_it->second->set_activity();
        break;
      case LCID_EXIT:
        rrc_log->info("Exiting thread\n");
        break;
      default:
        rrc_log->error("Rx PDU with invalid bearer id: %d", p.lcid);
        break;
    }
  }
}

} // namespace srsenb
