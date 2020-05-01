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

rrc::rrc() : rrc_log("RRC")
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
               gtpu_interface_rrc*    gtpu_,
               srslte::timer_handler* timers_)
{
  phy    = phy_;
  mac    = mac_;
  rlc    = rlc_;
  pdcp   = pdcp_;
  gtpu   = gtpu_;
  s1ap   = s1ap_;
  timers = timers_;

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
  enb_mobility_cfg.reset(new enb_mobility_handler(this));

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
    return &cell_common_list->get_cc_idx(cc_idx)->sib_buffer.at(sib_index)[0];
  }
  return nullptr;
}

void rrc::rl_failure(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_RLF_USER, nullptr};
  rx_pdu_queue.push(std::move(p));
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

template <class T>
void rrc::log_rrc_message(const std::string&           source,
                          const direction_t            dir,
                          const srslte::byte_buffer_t* pdu,
                          const T&                     msg,
                          const std::string&           msg_type)
{
  if (rrc_log->get_level() == srslte::LOG_LEVEL_INFO) {
    rrc_log->info("%s - %s %s (%d B)\n", source.c_str(), dir == Tx ? "Tx" : "Rx", msg_type.c_str(), pdu->N_bytes);
  } else if (rrc_log->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    rrc_log->debug_hex(pdu->msg,
                       pdu->N_bytes,
                       "%s - %s %s (%d B)\n",
                       source.c_str(),
                       dir == Tx ? "Tx" : "Rx",
                       msg_type.c_str(),
                       pdu->N_bytes);
    rrc_log->debug_long("Content:\n%s\n", json_writer.to_string().c_str());
  }
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
      auto p = users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue{this, rnti, sched_ue_cfg})));
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
      gtpu->add_bearer(SRSLTE_MRNTI, lcid, 1, 1, &teid_in);
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
    if (old_it->second->is_connected()) {
      old_it->second->send_connection_reconf_upd(srslte::allocate_unique_buffer(*pool));
    } else {
      old_it->second->send_connection_release();
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

  if (msg.protocol_ies.add_cs_fallback_ind_present) {
    rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
  }
  if (msg.protocol_ies.csg_membership_status_present) {
    rrc_log->warning("Not handling CSGMembershipStatus\n");
  }
  if (msg.protocol_ies.gummei_id_present) {
    rrc_log->warning("Not handling GUMMEI_ID\n");
  }
  if (msg.protocol_ies.ho_restrict_list_present) {
    rrc_log->warning("Not handling HandoverRestrictionList\n");
  }
  if (msg.protocol_ies.management_based_mdt_allowed_present) {
    rrc_log->warning("Not handling ManagementBasedMDTAllowed\n");
  }
  if (msg.protocol_ies.management_based_mdtplmn_list_present) {
    rrc_log->warning("Not handling ManagementBasedMDTPLMNList\n");
  }
  if (msg.protocol_ies.mme_ue_s1ap_id_minus2_present) {
    rrc_log->warning("Not handling MME_UE_S1AP_ID_2\n");
  }
  if (msg.protocol_ies.registered_lai_present) {
    rrc_log->warning("Not handling RegisteredLAI\n");
  }
  if (msg.protocol_ies.srvcc_operation_possible_present) {
    rrc_log->warning("Not handling SRVCCOperationPossible\n");
  }
  if (msg.protocol_ies.subscriber_profile_idfor_rfp_present) {
    rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
  }
  if (msg.protocol_ies.trace_activation_present) {
    rrc_log->warning("Not handling TraceActivation\n");
  }
  if (msg.protocol_ies.ue_radio_cap_present) {
    rrc_log->warning("Not handling UERadioCapability\n");
  }

  // UEAggregateMaximumBitrate
  user_it->second->set_bitrates(msg.protocol_ies.ueaggregate_maximum_bitrate.value);

  // UESecurityCapabilities
  user_it->second->set_security_capabilities(msg.protocol_ies.ue_security_cap.value);

  // SecurityKey
  user_it->second->set_security_key(msg.protocol_ies.security_key.value);

  // CSFB
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_required or
        msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_high_prio) {
      user_it->second->is_csfb = true;
    }
  }

  // Send RRC security mode command
  user_it->second->send_security_mode_command();

  // Setup E-RABs
  user_it->second->setup_erabs(msg.protocol_ies.erab_to_be_setup_list_ctxt_su_req.value);

  return true;
}

bool rrc::modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)
{
  bool err = false;

  rrc_log->info("Modifying context for 0x%x\n", rnti);
  auto user_it = users.find(rnti);

  if (user_it == users.end()) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_required ||
        msg.protocol_ies.cs_fallback_ind.value.value == asn1::s1ap::cs_fallback_ind_opts::cs_fallback_high_prio) {
      /* Remember that we are in a CSFB right now */
      user_it->second->is_csfb = true;
    }
  }

  if (msg.protocol_ies.add_cs_fallback_ind_present) {
    rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
    err = true;
  }
  if (msg.protocol_ies.csg_membership_status_present) {
    rrc_log->warning("Not handling CSGMembershipStatus\n");
    err = true;
  }
  if (msg.protocol_ies.registered_lai_present) {
    rrc_log->warning("Not handling RegisteredLAI\n");
  }
  if (msg.protocol_ies.subscriber_profile_idfor_rfp_present) {
    rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
    err = true;
  }

  if (err) {
    // maybe pass a cause value?
    return false;
  }

  // UEAggregateMaximumBitrate
  if (msg.protocol_ies.ueaggregate_maximum_bitrate_present) {
    user_it->second->set_bitrates(msg.protocol_ies.ueaggregate_maximum_bitrate.value);
  }

  // UESecurityCapabilities
  if (msg.protocol_ies.ue_security_cap_present) {
    user_it->second->set_security_capabilities(msg.protocol_ies.ue_security_cap.value);
  }

  // SecurityKey
  if (msg.protocol_ies.security_key_present) {
    user_it->second->set_security_key(msg.protocol_ies.security_key.value);

    // Send RRC security mode command ??
    user_it->second->send_security_mode_command();
  }

  return true;
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
    rrc_log->console("Warning IMSI paging not tested\n");
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

/*******************************************************************************
  Private functions
  All private functions are not mutexed and must be called from a mutexed environment
  from either a public function or the internal thread
*******************************************************************************/

void rrc::parse_ul_ccch(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  uint16_t old_rnti = 0;

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
        rrc_log->debug("rnti=0x%x, phyid=0x%x, smac=0x%x, cause=%s\n",
                       (uint32_t)ul_ccch_msg.msg.c1()
                           .rrc_conn_reest_request()
                           .crit_exts.rrc_conn_reest_request_r8()
                           .ue_id.c_rnti.to_number(),
                       ul_ccch_msg.msg.c1().rrc_conn_reest_request().crit_exts.rrc_conn_reest_request_r8().ue_id.pci,
                       (uint32_t)ul_ccch_msg.msg.c1()
                           .rrc_conn_reest_request()
                           .crit_exts.rrc_conn_reest_request_r8()
                           .ue_id.short_mac_i.to_number(),
                       ul_ccch_msg.msg.c1()
                           .rrc_conn_reest_request()
                           .crit_exts.rrc_conn_reest_request_r8()
                           .reest_cause.to_string()
                           .c_str());
        if (user_it->second->is_idle()) {
          old_rnti = (uint16_t)ul_ccch_msg.msg.c1()
                         .rrc_conn_reest_request()
                         .crit_exts.rrc_conn_reest_request_r8()
                         .ue_id.c_rnti.to_number();
          if (users.count(old_rnti)) {
            rrc_log->error("Not supported: ConnectionReestablishment for rnti=0x%x. Sending Connection Reject\n",
                           old_rnti);
            user_it->second->send_connection_reest_rej();
            s1ap->user_release(old_rnti, asn1::s1ap::cause_radio_network_opts::release_due_to_eutran_generated_reason);
          } else {
            rrc_log->error("Received ConnectionReestablishment for rnti=0x%x without context\n", old_rnti);
            user_it->second->send_connection_reest_rej();
          }
          // remove temporal rnti
          rrc_log->warning(
              "Received ConnectionReestablishment for rnti=0x%x. Removing temporal rnti=0x%x\n", old_rnti, rnti);
          rem_user_thread(rnti);
        } else {
          rrc_log->error("Received ReestablishmentRequest from an rnti=0x%x not in IDLE\n", rnti);
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
      rrc_log->error("Processing %s: Unknown rnti=0x%x\n", rb_id_text[lcid], rnti);
    }
  }
}

///< User mutex must be hold by caller
void rrc::process_rl_failure(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    uint32_t n_rfl = user_it->second->rl_failure();
    if (n_rfl == 1) {
      rrc_log->info("Radio-Link failure detected rnti=0x%x\n", rnti);
      if (s1ap->user_exists(rnti)) {
        if (!s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::radio_conn_with_ue_lost)) {
          rrc_log->info("Removing rnti=0x%x\n", rnti);
        }
      } else {
        rrc_log->warning("User rnti=0x%x context not existing in S1AP. Removing user\n", rnti);
        // Remove user from separate thread to wait to close all resources
        rem_user_thread(rnti);
      }
    } else {
      rrc_log->info("%d Radio-Link failure detected rnti=0x%x\n", n_rfl, rnti);
    }
  } else {
    rrc_log->error("Radio-Link failure detected for unknown rnti=0x%x\n", rnti);
  }
}

///< User mutex must be hold by caller
void rrc::process_release_complete(uint16_t rnti)
{
  rrc_log->info("Received Release Complete rnti=0x%x\n", rnti);
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    if (!user_it->second->is_idle()) {
      rlc->clear_buffer(rnti);
      user_it->second->send_connection_release();
      // There is no RRCReleaseComplete message from UE thus wait ~50 subframes for tx
      usleep(50000);
    }
    rem_user_thread(rnti);
  } else {
    rrc_log->error("Received ReleaseComplete for unknown rnti=0x%x\n", rnti);
  }
}

void rrc::rem_user(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    rrc_log->console("Disconnecting rnti=0x%x.\n", rnti);
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
      item.sibs[i].len = cell_common_list->get_cc_idx(ccidx)->sib_buffer.at(i).size();
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
      cell_ctxt->sib_buffer.emplace_back(sib_buffer->msg, sib_buffer->msg + sib_buffer->N_bytes);

      // Log SIBs in JSON format
      std::string log_msg("CC" + std::to_string(cc_idx) + " SIB payload");
      log_rrc_message(log_msg, Tx, sib_buffer.get(), msg[msg_index], msg[msg_index].msg.c1().type().to_string());
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
  pmch_item->pmch_cfg_r9.data_mcs_r9         = 20;
  pmch_item->pmch_cfg_r9.mch_sched_period_r9 = pmch_cfg_r9_s::mch_sched_period_r9_e_::rf64;
  pmch_item->pmch_cfg_r9.sf_alloc_end_r9     = 64 * 6;

  phy->configure_mbsfn(sib2_, sib13_, mcch);
  mac->write_mcch(sib2_, sib13_, &mcch);
}

void rrc::configure_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg)
{
  pdcp->config_security(rnti, lcid, sec_cfg);
}

void rrc::enable_integrity(uint16_t rnti, uint32_t lcid)
{
  pdcp->enable_integrity(rnti, lcid);
}

void rrc::enable_encryption(uint16_t rnti, uint32_t lcid)
{
  pdcp->enable_encryption(rnti, lcid);
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
      rrc_log->info_hex(p.pdu->msg, p.pdu->N_bytes, "Rx %s PDU", rb_id_text[p.lcid]);
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
      case LCID_RLF_USER:
        process_rl_failure(p.rnti);
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

/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/

rrc::ue::ue(rrc* outer_rrc, uint16_t rnti_, const sched_interface::ue_cfg_t& sched_ue_cfg) :
  parent(outer_rrc),
  rnti(rnti_),
  pool(srslte::byte_buffer_pool::get_instance()),
  current_sched_ue_cfg(sched_ue_cfg),
  phy_rrc_dedicated_list(sched_ue_cfg.supported_cc_list.size()),
  cell_ded_list(parent->cfg, *outer_rrc->pucch_res_list, *outer_rrc->cell_common_list)
{
  if (current_sched_ue_cfg.supported_cc_list.empty() or not current_sched_ue_cfg.supported_cc_list[0].active) {
    parent->rrc_log->warning("No PCell set. Picking eNBccIdx=0 as PCell\n");
    current_sched_ue_cfg.supported_cc_list.resize(1);
    current_sched_ue_cfg.supported_cc_list[0].active     = true;
    current_sched_ue_cfg.supported_cc_list[0].enb_cc_idx = UE_PCELL_CC_IDX;
  }

  activity_timer = outer_rrc->timers->get_unique_timer();
  set_activity_timeout(MSG3_RX_TIMEOUT); // next UE response is Msg3
  mobility_handler.reset(new rrc_mobility(this));

  // Configure
  apply_setup_phy_common(parent->cfg.sibs[1].sib2().rr_cfg_common);

  // Allocate cell and PUCCH resources
  if (cell_ded_list.add_cell(sched_ue_cfg.supported_cc_list[0].enb_cc_idx) == nullptr) {
    return;
  }
}

rrc::ue::~ue() {}

rrc_state_t rrc::ue::get_state()
{
  return state;
}

uint32_t rrc::ue::rl_failure()
{
  rlf_cnt++;
  return rlf_cnt;
}

void rrc::ue::set_activity()
{
  // re-start activity timer with current timeout value
  activity_timer.run();

  if (parent && parent->rrc_log) {
    parent->rrc_log->debug("Activity registered for rnti=0x%x (timeout_value=%dms)\n", rnti, activity_timer.duration());
  }
}

void rrc::ue::activity_timer_expired()
{
  if (parent) {
    if (parent->rrc_log) {
      parent->rrc_log->warning(
          "Activity timer for rnti=0x%x expired after %d ms\n", rnti, activity_timer.time_elapsed());
    }

    if (parent->s1ap->user_exists(rnti)) {
      parent->s1ap->user_release(rnti, asn1::s1ap::cause_radio_network_opts::user_inactivity);
    } else {
      if (rnti != SRSLTE_MRNTI) {
        parent->rem_user_thread(rnti);
      }
    }
  }

  state = RRC_STATE_RELEASE_REQUEST;
}

void rrc::ue::set_activity_timeout(const activity_timeout_type_t type)
{
  uint32_t deadline_s  = 0;
  uint32_t deadline_ms = 0;

  switch (type) {
    case MSG3_RX_TIMEOUT:
      deadline_s  = 0;
      deadline_ms = static_cast<uint32_t>(
          (get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2.rr_cfg_common.rach_cfg_common.max_harq_msg3_tx + 1) * 16);
      break;
    case UE_RESPONSE_RX_TIMEOUT:
      // Arbitrarily chosen value to complete each UE config step, i.e. security, bearer setup, etc.
      deadline_s  = 1;
      deadline_ms = 0;
      break;
    case UE_INACTIVITY_TIMEOUT:
      deadline_s  = parent->cfg.inactivity_timeout_ms / 1000;
      deadline_ms = parent->cfg.inactivity_timeout_ms % 1000;
      break;
    default:
      parent->rrc_log->error("Unknown timeout type %d", type);
  }

  uint32_t deadline = deadline_s * 1e3 + deadline_ms;
  activity_timer.set(deadline, [this](uint32_t tid) { activity_timer_expired(); });
  parent->rrc_log->debug("Setting timer for %s for rnti=0x%x to %dms\n", to_string(type).c_str(), rnti, deadline);

  set_activity();
}

bool rrc::ue::is_connected()
{
  return state == RRC_STATE_REGISTERED;
}

bool rrc::ue::is_idle()
{
  return state == RRC_STATE_IDLE;
}

void rrc::ue::parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  set_activity();

  ul_dcch_msg_s  ul_dcch_msg;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
    parent->rrc_log->error("Failed to unpack UL-DCCH message\n");
    return;
  }

  parent->log_rrc_message(rb_id_text[lcid], Rx, pdu.get(), ul_dcch_msg, ul_dcch_msg.msg.c1().type().to_string());

  // reuse PDU
  pdu->clear(); // TODO: name collision with byte_buffer reset

  transaction_id = 0;

  switch (ul_dcch_msg.msg.c1().type()) {
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_setup_complete:
      handle_rrc_con_setup_complete(&ul_dcch_msg.msg.c1().rrc_conn_setup_complete(), std::move(pdu));
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ul_info_transfer:
      pdu->N_bytes = ul_dcch_msg.msg.c1()
                         .ul_info_transfer()
                         .crit_exts.c1()
                         .ul_info_transfer_r8()
                         .ded_info_type.ded_info_nas()
                         .size();
      memcpy(pdu->msg,
             ul_dcch_msg.msg.c1()
                 .ul_info_transfer()
                 .crit_exts.c1()
                 .ul_info_transfer_r8()
                 .ded_info_type.ded_info_nas()
                 .data(),
             pdu->N_bytes);
      parent->s1ap->write_pdu(rnti, std::move(pdu));
      break;
    case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg_complete:
      handle_rrc_reconf_complete(&ul_dcch_msg.msg.c1().rrc_conn_recfg_complete(), std::move(pdu));
      parent->rrc_log->console("User 0x%x connected\n", rnti);
      state = RRC_STATE_REGISTERED;
      set_activity_timeout(UE_INACTIVITY_TIMEOUT);
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_complete:
      handle_security_mode_complete(&ul_dcch_msg.msg.c1().security_mode_complete());
      send_ue_cap_enquiry();
      state = RRC_STATE_WAIT_FOR_UE_CAP_INFO;
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_fail:
      handle_security_mode_failure(&ul_dcch_msg.msg.c1().security_mode_fail());
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ue_cap_info:
      if (handle_ue_cap_info(&ul_dcch_msg.msg.c1().ue_cap_info())) {
        notify_s1ap_ue_ctxt_setup_complete();
        send_connection_reconf(std::move(pdu));
        state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      } else {
        send_connection_reject();
        state = RRC_STATE_IDLE;
      }
      break;
    case ul_dcch_msg_type_c::c1_c_::types::meas_report:
      if (mobility_handler != nullptr) {
        mobility_handler->handle_ue_meas_report(ul_dcch_msg.msg.c1().meas_report());
      } else {
        parent->rrc_log->warning("Received MeasReport but no mobility configuration is available\n");
      }
      break;
    default:
      parent->rrc_log->error("Msg: %s not supported\n", ul_dcch_msg.msg.c1().type().to_string().c_str());
      break;
  }
}

void rrc::ue::handle_rrc_con_req(rrc_conn_request_s* msg)
{
  if (not parent->s1ap->is_mme_connected()) {
    parent->rrc_log->error("MME isn't connected. Sending Connection Reject\n");
    send_connection_reject();
    return;
  }

  rrc_conn_request_r8_ies_s* msg_r8 = &msg->crit_exts.rrc_conn_request_r8();

  if (msg_r8->ue_id.type() == init_ue_id_c::types::s_tmsi) {
    mmec     = (uint8_t)msg_r8->ue_id.s_tmsi().mmec.to_number();
    m_tmsi   = (uint32_t)msg_r8->ue_id.s_tmsi().m_tmsi.to_number();
    has_tmsi = true;
  }
  establishment_cause = msg_r8->establishment_cause;
  send_connection_setup();
  state = RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE;

  set_activity_timeout(UE_RESPONSE_RX_TIMEOUT);
}

std::string rrc::ue::to_string(const activity_timeout_type_t& type)
{
  constexpr static const char* options[] = {"Msg3 reception", "UE response reception", "UE inactivity"};
  return srslte::enum_to_text(options, (uint32_t)activity_timeout_type_t::nulltype, (uint32_t)type);
}

void rrc::ue::handle_rrc_con_reest_req(rrc_conn_reest_request_r8_ies_s* msg)
{
  // TODO: Check Short-MAC-I value
  parent->rrc_log->error("Not Supported: ConnectionReestablishment.\n");
}

void rrc::ue::handle_rrc_con_setup_complete(rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config_dedicated(rnti);

  parent->rrc_log->info("RRCConnectionSetupComplete transaction ID: %d\n", msg->rrc_transaction_id);
  rrc_conn_setup_complete_r8_ies_s* msg_r8 = &msg->crit_exts.c1().rrc_conn_setup_complete_r8();

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  pdu->N_bytes = msg_r8->ded_info_nas.size();
  memcpy(pdu->msg, msg_r8->ded_info_nas.data(), pdu->N_bytes);

  // Acknowledge Dedicated Configuration
  parent->mac->phy_config_enabled(rnti, true);

  asn1::s1ap::rrc_establishment_cause_e s1ap_cause;
  s1ap_cause.value = (asn1::s1ap::rrc_establishment_cause_opts::options)establishment_cause.value;
  if (has_tmsi) {
    parent->s1ap->initial_ue(rnti, s1ap_cause, std::move(pdu), m_tmsi, mmec);
  } else {
    parent->s1ap->initial_ue(rnti, s1ap_cause, std::move(pdu));
  }
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::handle_rrc_reconf_complete(rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  // Inform PHY about the configuration completion
  parent->phy->complete_config_dedicated(rnti);

  if (last_rrc_conn_recfg.rrc_transaction_id == msg->rrc_transaction_id) {
    // Finally, add secondary carriers to MAC
    auto& list = current_sched_ue_cfg.supported_cc_list;
    for (const auto& ue_cell : cell_ded_list) {
      uint32_t ue_cc_idx = ue_cell.ue_cc_idx;

      if (ue_cc_idx >= list.size()) {
        list.resize(ue_cc_idx + 1);
      }
      list[ue_cc_idx].active     = true;
      list[ue_cc_idx].enb_cc_idx = ue_cell.cell_common.enb_cc_idx;
    }
    parent->mac->ue_cfg(rnti, &current_sched_ue_cfg);

    // Finally, add SRB2 and DRB1 and any dedicated DRBs to the scheduler
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg = {};
    bearer_cfg.direction                                = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    bearer_cfg.group                                    = 0;
    parent->mac->bearer_ue_cfg(rnti, 2, &bearer_cfg);
    bearer_cfg.group = last_rrc_conn_recfg.crit_exts.c1()
                           .rrc_conn_recfg_r8()
                           .rr_cfg_ded.drb_to_add_mod_list[0]
                           .lc_ch_cfg.ul_specific_params.lc_ch_group;
    for (const std::pair<const uint8_t, erab_t>& erab_pair : erabs) {
      parent->mac->bearer_ue_cfg(rnti, erab_pair.second.id - 2, &bearer_cfg);
    }

    // Acknowledge Dedicated Configuration
    parent->mac->phy_config_enabled(rnti, true);
  } else {
    parent->rrc_log->error("Expected RRCReconfigurationComplete with transaction ID: %d, got %d\n",
                           last_rrc_conn_recfg.rrc_transaction_id,
                           msg->rrc_transaction_id);
  }
}

void rrc::ue::handle_security_mode_complete(security_mode_complete_s* msg)
{
  parent->rrc_log->info("SecurityModeComplete transaction ID: %d\n", msg->rrc_transaction_id);
  parent->enable_encryption(rnti, RB_ID_SRB1);
}

void rrc::ue::handle_security_mode_failure(security_mode_fail_s* msg)
{
  parent->rrc_log->info("SecurityModeFailure transaction ID: %d\n", msg->rrc_transaction_id);
}

bool rrc::ue::handle_ue_cap_info(ue_cap_info_s* msg)
{
  parent->rrc_log->info("UECapabilityInformation transaction ID: %d\n", msg->rrc_transaction_id);
  ue_cap_info_r8_ies_s* msg_r8 = &msg->crit_exts.c1().ue_cap_info_r8();

  for (uint32_t i = 0; i < msg_r8->ue_cap_rat_container_list.size(); i++) {
    if (msg_r8->ue_cap_rat_container_list[i].rat_type != rat_type_e::eutra) {
      parent->rrc_log->warning("Not handling UE capability information for RAT type %s\n",
                               msg_r8->ue_cap_rat_container_list[i].rat_type.to_string().c_str());
    } else {
      asn1::cbit_ref bref(msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.data(),
                          msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.size());
      if (eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
        parent->rrc_log->error("Failed to unpack EUTRA capabilities message\n");
        return false;
      }
      eutra_capabilities_unpacked = true;
      srslte::set_rrc_ue_capabilities_t(ue_capabilities, eutra_capabilities);

      parent->rrc_log->info("UE rnti: 0x%x category: %d\n", rnti, eutra_capabilities.ue_category);
    }
  }

  return true;

  // TODO: Add liblte_rrc support for unpacking UE cap info and repacking into
  //       inter-node UERadioAccessCapabilityInformation (36.331 v10.0.0 Section 10.2.2).
  //       This is then passed to S1AP for transfer to EPC.
  // parent->s1ap->ue_capabilities(rnti, &eutra_capabilities);
}

void rrc::ue::set_bitrates(const asn1::s1ap::ue_aggregate_maximum_bitrate_s& rates)
{
  bitrates = rates;
}

void rrc::ue::set_security_capabilities(const asn1::s1ap::ue_security_cap_s& caps)
{
  security_capabilities = caps;
}

void rrc::ue::set_security_key(const asn1::fixed_bitstring<256, false, true>& key)
{
  for (uint32_t i = 0; i < key.nof_octets(); ++i) {
    k_enb[i] = key.data()[key.nof_octets() - 1 - i];
  }
  parent->rrc_log->info_hex(k_enb, 32, "Key eNodeB (k_enb)");
  // Selects security algorithms (cipher_algo and integ_algo) based on capabilities and config preferences
  select_security_algorithms();

  parent->rrc_log->info(
      "Selected security algorithms EEA: EEA%d EIA: EIA%d\n", sec_cfg.cipher_algo, sec_cfg.integ_algo);

  // Generate K_rrc_enc and K_rrc_int
  srslte::security_generate_k_rrc(
      k_enb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_rrc_enc.data(), sec_cfg.k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_up_enc.data(), sec_cfg.k_up_int.data());

  parent->configure_security(rnti, RB_ID_SRB1, sec_cfg);

  parent->enable_integrity(rnti, RB_ID_SRB1);

  parent->rrc_log->info_hex(sec_cfg.k_rrc_enc.data(), 32, "RRC Encryption Key (k_rrc_enc)");
  parent->rrc_log->info_hex(sec_cfg.k_rrc_int.data(), 32, "RRC Integrity Key (k_rrc_int)");
  parent->rrc_log->info_hex(sec_cfg.k_up_enc.data(), 32, "UP Encryption Key (k_up_enc)");
}

bool rrc::ue::setup_erabs(const asn1::s1ap::erab_to_be_setup_list_ctxt_su_req_l& e)
{
  for (const auto& item : e) {
    auto& erab = item.value.erab_to_be_setup_item_ctxt_su_req();
    if (erab.ext) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListCtxtSURequest extensions\n");
    }
    if (erab.ie_exts_present) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListCtxtSURequest extensions\n");
    }
    if (erab.transport_layer_address.length() > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    const asn1::unbounded_octstring<true>* nas_pdu = erab.nas_pdu_present ? &erab.nas_pdu : nullptr;
    setup_erab(erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, nas_pdu);
  }
  return true;
}

bool rrc::ue::setup_erabs(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e)
{
  for (const auto& item : e) {
    auto& erab = item.value.erab_to_be_setup_item_bearer_su_req();
    if (erab.ext) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListBearerSUReq extensions\n");
    }
    if (erab.ie_exts_present) {
      parent->rrc_log->warning("Not handling E-RABToBeSetupListBearerSUReq extensions\n");
    }
    if (erab.transport_layer_address.length() > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    uint8_to_uint32(erab.gtp_teid.data(), &teid_out);
    setup_erab(erab.erab_id, erab.erab_level_qos_params, erab.transport_layer_address, teid_out, &erab.nas_pdu);
  }

  // Work in progress
  notify_s1ap_ue_erab_setup_response(e);
  send_connection_reconf_new_bearer(e);
  return true;
}

void rrc::ue::setup_erab(uint8_t                                            id,
                         const asn1::s1ap::erab_level_qos_params_s&         qos,
                         const asn1::bounded_bitstring<1, 160, true, true>& addr,
                         uint32_t                                           teid_out,
                         const asn1::unbounded_octstring<true>*             nas_pdu)
{
  erabs[id].id         = id;
  erabs[id].qos_params = qos;
  erabs[id].address    = addr;
  erabs[id].teid_out   = teid_out;

  if (addr.length() > 32) {
    parent->rrc_log->error("Only addresses with length <= 32 are supported\n");
    return;
  }
  uint32_t addr_ = addr.to_number();
  uint8_t  lcid  = id - 2; // Map e.g. E-RAB 5 to LCID 3 (==DRB1)
  parent->gtpu->add_bearer(rnti, lcid, addr_, erabs[id].teid_out, &(erabs[id].teid_in));

  if (nas_pdu != nullptr) {
    erab_info_list[id] = allocate_unique_buffer(*pool);
    memcpy(erab_info_list[id]->msg, nas_pdu->data(), nas_pdu->size());
    erab_info_list[id]->N_bytes = nas_pdu->size();
    parent->rrc_log->info_hex(
        erab_info_list[id]->msg, erab_info_list[id]->N_bytes, "setup_erab nas_pdu -> erab_info rnti 0x%x", rnti);
  }
}

bool rrc::ue::release_erabs()
{
  // TODO: notify GTPU layer for each ERAB
  erabs.clear();
  return true;
}

void rrc::ue::notify_s1ap_ue_ctxt_setup_complete()
{
  asn1::s1ap::init_context_setup_resp_s res;

  res.protocol_ies.erab_setup_list_ctxt_su_res.value.resize(erabs.size());
  uint32_t i = 0;
  for (auto& erab : erabs) {
    res.protocol_ies.erab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item   = res.protocol_ies.erab_setup_list_ctxt_su_res.value[i].value.erab_setup_item_ctxt_su_res();
    item.erab_id = erab.second.id;
    uint32_to_uint8(erab.second.teid_in, item.gtp_teid.data());
    i++;
  }

  parent->s1ap->ue_ctxt_setup_complete(rnti, res);
}

void rrc::ue::notify_s1ap_ue_erab_setup_response(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e)
{
  asn1::s1ap::erab_setup_resp_s res;

  res.protocol_ies.erab_setup_list_bearer_su_res.value.resize(e.size());
  for (uint32_t i = 0; i < e.size(); ++i) {
    res.protocol_ies.erab_setup_list_bearer_su_res_present = true;
    auto& item                                             = res.protocol_ies.erab_setup_list_bearer_su_res.value[i];
    item.load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_BEARER_SU_RES);
    uint8_t id                                         = e[i].value.erab_to_be_setup_item_bearer_su_req().erab_id;
    item.value.erab_setup_item_bearer_su_res().erab_id = id;
    uint32_to_uint8(erabs[id].teid_in, &item.value.erab_setup_item_bearer_su_res().gtp_teid[0]);
  }

  parent->s1ap->ue_erab_setup_complete(rnti, res);
}

void rrc::ue::send_connection_reest_rej()
{
  dl_ccch_msg_s dl_ccch_msg;

  dl_ccch_msg.msg.set_c1().set_rrc_conn_reest_reject().crit_exts.set_rrc_conn_reest_reject_r8();

  send_dl_ccch(&dl_ccch_msg);
}

void rrc::ue::send_connection_reject()
{
  dl_ccch_msg_s dl_ccch_msg;

  dl_ccch_msg.msg.set_c1().set_rrc_conn_reject().crit_exts.set_c1().set_rrc_conn_reject_r8().wait_time = 10;

  send_dl_ccch(&dl_ccch_msg);
}

void rrc::ue::send_connection_setup(bool is_setup)
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1();

  rr_cfg_ded_s* rr_cfg = nullptr;
  if (is_setup) {
    dl_ccch_msg.msg.c1().set_rrc_conn_setup();
    dl_ccch_msg.msg.c1().rrc_conn_setup().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
    dl_ccch_msg.msg.c1().rrc_conn_setup().crit_exts.set_c1().set_rrc_conn_setup_r8();
    rr_cfg = &dl_ccch_msg.msg.c1().rrc_conn_setup().crit_exts.c1().rrc_conn_setup_r8().rr_cfg_ded;
  } else {
    dl_ccch_msg.msg.c1().set_rrc_conn_reest();
    dl_ccch_msg.msg.c1().rrc_conn_reest().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
    dl_ccch_msg.msg.c1().rrc_conn_reest().crit_exts.set_c1().set_rrc_conn_reest_r8();
    rr_cfg = &dl_ccch_msg.msg.c1().rrc_conn_reest().crit_exts.c1().rrc_conn_reest_r8().rr_cfg_ded;
  }

  // Add SRB1 to cfg
  rr_cfg->srb_to_add_mod_list_present = true;
  rr_cfg->srb_to_add_mod_list.resize(1);
  rr_cfg->srb_to_add_mod_list[0].srb_id            = 1;
  rr_cfg->srb_to_add_mod_list[0].lc_ch_cfg_present = true;
  rr_cfg->srb_to_add_mod_list[0].lc_ch_cfg.set(srb_to_add_mod_s::lc_ch_cfg_c_::types::default_value);
  rr_cfg->srb_to_add_mod_list[0].rlc_cfg_present = true;
  rr_cfg->srb_to_add_mod_list[0].rlc_cfg.set(srb_to_add_mod_s::rlc_cfg_c_::types::default_value);

  // mac-MainConfig
  rr_cfg->mac_main_cfg_present  = true;
  mac_main_cfg_s* mac_cfg       = &rr_cfg->mac_main_cfg.set_explicit_value();
  mac_cfg->ul_sch_cfg_present   = true;
  mac_cfg->ul_sch_cfg           = parent->cfg.mac_cnfg.ul_sch_cfg;
  mac_cfg->phr_cfg_present      = true;
  mac_cfg->phr_cfg              = parent->cfg.mac_cnfg.phr_cfg;
  mac_cfg->time_align_timer_ded = parent->cfg.mac_cnfg.time_align_timer_ded;

  // physicalConfigDedicated
  rr_cfg->phys_cfg_ded_present       = true;
  phys_cfg_ded_s* phy_cfg            = &rr_cfg->phys_cfg_ded;
  phy_cfg->pusch_cfg_ded_present     = true;
  phy_cfg->pusch_cfg_ded             = parent->cfg.pusch_cfg;
  phy_cfg->sched_request_cfg_present = true;
  phy_cfg->sched_request_cfg.set_setup();
  phy_cfg->sched_request_cfg.setup().dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  // set default antenna config
  phy_cfg->ant_info_present = true;
  phy_cfg->ant_info.set_explicit_value();
  if (parent->cfg.cell.nof_ports == 1) {
    phy_cfg->ant_info.explicit_value().tx_mode.value = ant_info_ded_s::tx_mode_e_::tm1;
  } else {
    phy_cfg->ant_info.explicit_value().tx_mode.value = ant_info_ded_s::tx_mode_e_::tm2;
  }
  phy_cfg->ant_info.explicit_value().ue_tx_ant_sel.set(setup_e::release);

  phy_cfg->sched_request_cfg.setup().sr_cfg_idx       = (uint8_t)cell_ded_list.get_sr_res()->sr_I;
  phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx = (uint16_t)cell_ded_list.get_sr_res()->sr_N_pucch;

  // Power control
  phy_cfg->ul_pwr_ctrl_ded_present              = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pusch          = 0;
  phy_cfg->ul_pwr_ctrl_ded.delta_mcs_enabled    = ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
  phy_cfg->ul_pwr_ctrl_ded.accumulation_enabled = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pucch = 0, phy_cfg->ul_pwr_ctrl_ded.psrs_offset = 3;

  // PDSCH
  phy_cfg->pdsch_cfg_ded_present = true;
  phy_cfg->pdsch_cfg_ded.p_a     = parent->cfg.pdsch_cfg;

  // PUCCH
  phy_cfg->pucch_cfg_ded_present = true;
  phy_cfg->pucch_cfg_ded.ack_nack_repeat.set(pucch_cfg_ded_s::ack_nack_repeat_c_::types::release);

  phy_cfg->cqi_report_cfg_present = true;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic         = cqi_report_mode_aperiodic_e::rm30;
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (is_setup) {
      if (get_cqi(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
                  &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
                  UE_PCELL_CC_IDX)) {
        parent->rrc_log->error("Allocating CQI resources for rnti=%d\n", rnti);
        return;
      }
    } else {
      get_cqi(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
              &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
              UE_PCELL_CC_IDX);
    }
  }
  phy_cfg->cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;

  // Add SRB1 to Scheduler
  current_sched_ue_cfg.maxharq_tx              = parent->cfg.mac_cnfg.ul_sch_cfg.max_harq_tx.to_number();
  current_sched_ue_cfg.continuous_pusch        = false;
  current_sched_ue_cfg.ue_bearers[0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  current_sched_ue_cfg.ue_bearers[1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    current_sched_ue_cfg.aperiodic_cqi_period                   = parent->cfg.cqi_cfg.period;
    current_sched_ue_cfg.dl_cfg.cqi_report.aperiodic_configured = true;
  } else {
    get_cqi(&current_sched_ue_cfg.dl_cfg.cqi_report.pmi_idx, &current_sched_ue_cfg.pucch_cfg.n_pucch, UE_PCELL_CC_IDX);
    current_sched_ue_cfg.dl_cfg.cqi_report.periodic_configured = true;
  }
  current_sched_ue_cfg.dl_cfg.tm                   = SRSLTE_TM1;
  current_sched_ue_cfg.pucch_cfg.I_sr              = cell_ded_list.get_sr_res()->sr_I;
  current_sched_ue_cfg.pucch_cfg.n_pucch_sr        = cell_ded_list.get_sr_res()->sr_N_pucch;
  current_sched_ue_cfg.pucch_cfg.sr_configured     = true;
  const sib_type2_s& sib2                          = get_ue_cc_cfg(UE_PCELL_CC_IDX)->sib2;
  current_sched_ue_cfg.pucch_cfg.delta_pucch_shift = sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();
  current_sched_ue_cfg.pucch_cfg.N_cs              = sib2.rr_cfg_common.pucch_cfg_common.ncs_an;
  current_sched_ue_cfg.pucch_cfg.n_rb_2            = sib2.rr_cfg_common.pucch_cfg_common.nrb_cqi;
  current_sched_ue_cfg.pucch_cfg.N_pucch_1         = sib2.rr_cfg_common.pucch_cfg_common.n1_pucch_an;
  current_sched_ue_cfg.dl_ant_info                 = srslte::make_ant_info_ded(phy_cfg->ant_info.explicit_value());

  // Configure MAC
  if (is_setup) {
    // In case of RRC Connection Setup message (Msg4), we need to resolve the contention by sending a ConRes CE
    parent->mac->ue_set_crnti(rnti, rnti, &current_sched_ue_cfg);
  } else {
    parent->mac->ue_cfg(rnti, &current_sched_ue_cfg);
  }

  // Configure SRB1 in RLC
  parent->rlc->add_bearer(rnti, 1, srslte::rlc_config_t::srb_config(1));

  // Configure SRB1 in PDCP
  parent->pdcp->add_bearer(rnti, 1, srslte::make_srb_pdcp_config_t(1, false));

  // Configure PHY layer
  apply_setup_phy_config_dedicated(*phy_cfg); // It assumes SCell has not been set before
  parent->mac->phy_config_enabled(rnti, false);

  rr_cfg->drb_to_add_mod_list_present = false;
  rr_cfg->drb_to_release_list_present = false;
  rr_cfg->rlf_timers_and_consts_r9.set_present(false);
  rr_cfg->sps_cfg_present = false;
  //  rr_cfg->rlf_timers_and_constants_present = false;

  send_dl_ccch(&dl_ccch_msg);
}

void rrc::ue::send_connection_reest()
{
  send_connection_setup(false);
}

void rrc::ue::send_connection_release()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_release();
  dl_dcch_msg.msg.c1().rrc_conn_release().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  dl_dcch_msg.msg.c1().rrc_conn_release().crit_exts.set_c1().set_rrc_conn_release_r8();
  dl_dcch_msg.msg.c1().rrc_conn_release().crit_exts.c1().rrc_conn_release_r8().release_cause = release_cause_e::other;
  if (is_csfb) {
    rrc_conn_release_r8_ies_s& rel_ies = dl_dcch_msg.msg.c1().rrc_conn_release().crit_exts.c1().rrc_conn_release_r8();
    rel_ies.redirected_carrier_info_present = true;
    rel_ies.redirected_carrier_info.set_geran();
    rel_ies.redirected_carrier_info.geran() = parent->sib7.carrier_freqs_info_list[0].carrier_freqs;
  }

  send_dl_dcch(&dl_dcch_msg);
}

int rrc::ue::get_drbid_config(drb_to_add_mod_s* drb, int drb_id)
{
  uint32_t lc_id   = (uint32_t)(drb_id + 2);
  uint32_t erab_id = lc_id + 2;
  uint32_t qci     = erabs[erab_id].qos_params.qci;

  if (qci >= MAX_NOF_QCI) {
    parent->rrc_log->error("Invalid QCI=%d for ERAB_id=%d, DRB_id=%d\n", qci, erab_id, drb_id);
    return SRSLTE_ERROR;
  }

  if (!parent->cfg.qci_cfg[qci].configured) {
    parent->rrc_log->error("QCI=%d not configured\n", qci);
    return SRSLTE_ERROR;
  }

  // Add DRB1 to the message
  drb->drb_id                = (uint8_t)drb_id;
  drb->lc_ch_id_present      = true;
  drb->lc_ch_id              = (uint8_t)lc_id;
  drb->eps_bearer_id         = (uint8_t)erab_id;
  drb->eps_bearer_id_present = true;

  drb->lc_ch_cfg_present                                = true;
  drb->lc_ch_cfg.ul_specific_params_present             = true;
  drb->lc_ch_cfg.ul_specific_params.lc_ch_group_present = true;
  drb->lc_ch_cfg.ul_specific_params                     = parent->cfg.qci_cfg[qci].lc_cfg;

  drb->pdcp_cfg_present = true;
  drb->pdcp_cfg         = parent->cfg.qci_cfg[qci].pdcp_cfg;

  drb->rlc_cfg_present = true;
  drb->rlc_cfg         = parent->cfg.qci_cfg[qci].rlc_cfg;

  return SRSLTE_SUCCESS;
}

void rrc::ue::send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu)
{
  dl_dcch_msg_s     dl_dcch_msg;
  rrc_conn_recfg_s* rrc_conn_recfg   = &dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg();
  rrc_conn_recfg->rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg->crit_exts.set_c1().set_rrc_conn_recfg_r8();

  rrc_conn_recfg->crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded_present = true;
  auto&         reconfig_r8 = rrc_conn_recfg->crit_exts.c1().rrc_conn_recfg_r8();
  rr_cfg_ded_s* rr_cfg      = &reconfig_r8.rr_cfg_ded;

  rr_cfg->phys_cfg_ded_present       = true;
  phys_cfg_ded_s* phy_cfg            = &rr_cfg->phys_cfg_ded;
  phy_cfg->sched_request_cfg_present = true;
  phy_cfg->sched_request_cfg.set_setup();
  phy_cfg->sched_request_cfg.setup().dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  phy_cfg->cqi_report_cfg_present = true;
  if (cell_ded_list.nof_cells() > 0) {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    get_cqi(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
            &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx,
            UE_PCELL_CC_IDX);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm3 ||
        parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = true;
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx = 483; /* TODO: HARDCODED! Add to UL scheduler */
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = false;
    }
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    if (phy_cfg->ant_info_present && parent->cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm31;
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm30;
    }
  }
  apply_reconf_phy_config(reconfig_r8);

  phy_cfg->sched_request_cfg.setup().sr_cfg_idx = cell_ded_list.get_sr_res()->sr_I;
  phy_cfg->sched_request_cfg.setup().sr_cfg_idx = cell_ded_list.get_sr_res()->sr_N_pucch;

  pdu->clear();

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reconf(srslte::unique_byte_buffer_t pdu)
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
  dl_dcch_msg.msg.c1().rrc_conn_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  rrc_conn_recfg_r8_ies_s* conn_reconf = &dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();
  conn_reconf->rr_cfg_ded_present      = true;

  conn_reconf->rr_cfg_ded.phys_cfg_ded_present = true;
  phys_cfg_ded_s* phy_cfg                      = &conn_reconf->rr_cfg_ded.phys_cfg_ded;

  // Configure PHY layer
  phy_cfg->ant_info_present              = true;
  phy_cfg->ant_info.set_explicit_value() = parent->cfg.antenna_info;
  phy_cfg->cqi_report_cfg_present        = true;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    if (phy_cfg->ant_info_present and
        phy_cfg->ant_info.explicit_value().tx_mode.value == ant_info_ded_s::tx_mode_e_::tm4) {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm31;
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm30;
    }
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    auto& cqi_rep                                       = phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    get_cqi(&cqi_rep.cqi_pmi_cfg_idx, &cqi_rep.cqi_pucch_res_idx, UE_PCELL_CC_IDX);
    cqi_rep.cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    cqi_rep.simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (phy_cfg->ant_info_present and
        ((phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm3) ||
         (phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm4))) {
      uint16_t ri_idx = 0;
      if (get_ri(parent->cfg.cqi_cfg.m_ri, &ri_idx) == SRSLTE_SUCCESS) {
        phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
        phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = true;
        phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx         = ri_idx;
      } else {
        parent->rrc_log->console("\nWarning: Configured wrong M_ri parameter.\n\n");
      }
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present = false;
    }
  }
  phy_cfg->cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;
  // PDSCH
  phy_cfg->pdsch_cfg_ded_present = true;
  phy_cfg->pdsch_cfg_ded.p_a     = parent->cfg.pdsch_cfg;

  // Configure 256QAM
  if (ue_capabilities.category_dl >= 11 && ue_capabilities.support_dl_256qam) {
    phy_cfg->cqi_report_cfg_pcell_v1250.set_present(true);
    cqi_report_cfg_v1250_s* cqi_report_cfg    = conn_reconf->rr_cfg_ded.phys_cfg_ded.cqi_report_cfg_pcell_v1250.get();
    cqi_report_cfg->alt_cqi_table_r12_present = true;
    cqi_report_cfg->alt_cqi_table_r12         = asn1::rrc::cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_::all_sfs;
    current_sched_ue_cfg.use_tbs_index_alt    = true;
  }

  // Add SCells
  if (fill_scell_to_addmod_list(conn_reconf) != SRSLTE_SUCCESS) {
    parent->rrc_log->warning("Could not create configuration for Scell\n");
    return;
  }

  apply_reconf_phy_config(*conn_reconf);
  current_sched_ue_cfg.dl_ant_info = srslte::make_ant_info_ded(phy_cfg->ant_info.explicit_value());
  parent->mac->ue_cfg(rnti, &current_sched_ue_cfg);
  parent->mac->phy_config_enabled(rnti, false);

  // Add SRB2 to the message
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list.resize(1);
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].srb_id            = 2;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].lc_ch_cfg_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].lc_ch_cfg.set(srb_to_add_mod_s::lc_ch_cfg_c_::types::default_value);
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].rlc_cfg_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].rlc_cfg.set(srb_to_add_mod_s::rlc_cfg_c_::types::default_value);

  // Configure SRB2 in RLC and PDCP
  parent->rlc->add_bearer(rnti, 2, srslte::rlc_config_t::srb_config(2));

  // Configure SRB2 in PDCP
  parent->pdcp->add_bearer(rnti, 2, srslte::make_srb_pdcp_config_t(2, false));
  parent->pdcp->config_security(rnti, 2, sec_cfg);
  parent->pdcp->enable_integrity(rnti, 2);
  parent->pdcp->enable_encryption(rnti, 2);

  // Add DRB Add/Mod list
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list_present = true;
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list.resize(erabs.size());

  // Add space for NAS messages
  uint8_t n_nas = erab_info_list.size();
  if (n_nas > 0) {
    conn_reconf->ded_info_nas_list_present = true;
    conn_reconf->ded_info_nas_list.resize(n_nas);
  }

  // Configure all DRBs
  uint8_t vec_idx = 0;
  for (const std::pair<const uint8_t, erab_t>& erab_id_pair : erabs) {
    const erab_t& erab   = erab_id_pair.second;
    uint8_t       drb_id = erab.id - 4;
    uint8_t       lcid   = erab.id - 2;

    // Get DRB1 configuration
    if (get_drbid_config(&conn_reconf->rr_cfg_ded.drb_to_add_mod_list[drb_id - 1], drb_id)) {
      parent->rrc_log->error("Getting DRB1 configuration\n");
      parent->rrc_log->console("The QCI %d for DRB1 is invalid or not configured.\n", erab.qos_params.qci);
      return;
    }

    // Configure DRBs in RLC
    parent->rlc->add_bearer(
        rnti, lcid, srslte::make_rlc_config_t(conn_reconf->rr_cfg_ded.drb_to_add_mod_list[vec_idx].rlc_cfg));

    // Configure DRB1 in PDCP
    srslte::pdcp_config_t pdcp_cnfg_drb =
        srslte::make_drb_pdcp_config_t(drb_id, false, conn_reconf->rr_cfg_ded.drb_to_add_mod_list[vec_idx].pdcp_cfg);
    parent->pdcp->add_bearer(rnti, lcid, pdcp_cnfg_drb);
    parent->pdcp->config_security(rnti, lcid, sec_cfg);
    parent->pdcp->enable_integrity(rnti, lcid);
    parent->pdcp->enable_encryption(rnti, lcid);

    // DRBs have already been configured in GTPU through bearer setup
    // Add E-RAB info message for the E-RABs
    std::map<uint8_t, srslte::unique_byte_buffer_t>::const_iterator it = erab_info_list.find(erab.id);
    if (it != erab_info_list.end()) {
      const srslte::unique_byte_buffer_t& erab_info = it->second;
      parent->rrc_log->info_hex(
          erab_info->msg, erab_info->N_bytes, "connection_reconf erab_info -> nas_info rnti 0x%x\n", rnti);
      conn_reconf->ded_info_nas_list[vec_idx].resize(erab_info->N_bytes);
      memcpy(conn_reconf->ded_info_nas_list[vec_idx].data(), erab_info->msg, erab_info->N_bytes);
      erab_info_list.erase(it);
    } else {
      parent->rrc_log->debug("Not adding NAS message to connection reconfiguration. E-RAB id %d\n", erab.id);
    }
    vec_idx++;
  }

  if (mobility_handler != nullptr) {
    mobility_handler->fill_conn_recfg_msg(conn_reconf);
  }
  last_rrc_conn_recfg = dl_dcch_msg.msg.c1().rrc_conn_recfg();

  // Reuse same PDU
  pdu->clear();

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

//! Helper method to access Cell configuration based on UE Carrier Index
cell_info_common* rrc::ue::get_ue_cc_cfg(uint32_t ue_cc_idx)
{
  if (ue_cc_idx >= current_sched_ue_cfg.supported_cc_list.size()) {
    return nullptr;
  }
  uint32_t enb_cc_idx = current_sched_ue_cfg.supported_cc_list[ue_cc_idx].enb_cc_idx;
  return parent->cell_common_list->get_cc_idx(enb_cc_idx);
}

//! Method to fill SCellToAddModList for SCell info
int rrc::ue::fill_scell_to_addmod_list(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_reconf)
{
  const cell_info_common* pcell_cfg = get_ue_cc_cfg(UE_PCELL_CC_IDX);
  if (pcell_cfg->cell_cfg.scell_list.empty()) {
    return SRSLTE_SUCCESS;
  }

  // Allocate CQI + PUCCH for SCells.
  for (auto scell_idx : pcell_cfg->cell_cfg.scell_list) {
    uint32_t cell_id = scell_idx.cell_id;
    cell_ded_list.add_cell(parent->cell_common_list->get_cell_id(cell_id)->enb_cc_idx);
  }
  if (cell_ded_list.nof_cells() == 1) {
    // No SCell could be allocated. Fallback to single cell mode.
    return SRSLTE_SUCCESS;
  }

  conn_reconf->non_crit_ext_present                                                     = true;
  conn_reconf->non_crit_ext.non_crit_ext_present                                        = true;
  conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext_present                           = true;
  conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext.scell_to_add_mod_list_r10_present = true;
  auto& list = conn_reconf->non_crit_ext.non_crit_ext.non_crit_ext.scell_to_add_mod_list_r10;

  // Add all SCells configured+allocated for the current PCell
  for (auto& p : cell_ded_list) {
    if (p.ue_cc_idx == UE_PCELL_CC_IDX) {
      continue;
    }
    uint32_t                scell_idx = p.ue_cc_idx;
    const cell_info_common* cc_cfg    = &p.cell_common;
    const sib_type1_s&      cell_sib1 = cc_cfg->sib1;
    const sib_type2_s&      cell_sib2 = cc_cfg->sib2;

    scell_to_add_mod_r10_s cell;
    cell.scell_idx_r10                        = scell_idx;
    cell.cell_identif_r10_present             = true;
    cell.cell_identif_r10.pci_r10             = cc_cfg->cell_cfg.pci;
    cell.cell_identif_r10.dl_carrier_freq_r10 = cc_cfg->cell_cfg.dl_earfcn;
    cell.rr_cfg_common_scell_r10_present      = true;
    // RadioResourceConfigCommon
    const rr_cfg_common_sib_s& cc_cfg_sib = cell_sib2.rr_cfg_common;
    auto&                      nonul_cfg  = cell.rr_cfg_common_scell_r10.non_ul_cfg_r10;
    asn1::number_to_enum(nonul_cfg.dl_bw_r10, parent->cfg.cell.nof_prb);
    nonul_cfg.ant_info_common_r10.ant_ports_count.value = ant_info_common_s::ant_ports_count_opts::an1;
    nonul_cfg.phich_cfg_r10                             = cc_cfg->mib.phich_cfg;
    nonul_cfg.pdsch_cfg_common_r10                      = cc_cfg_sib.pdsch_cfg_common;
    // RadioResourceConfigCommonSCell-r10::ul-Configuration-r10
    cell.rr_cfg_common_scell_r10.ul_cfg_r10_present          = true;
    auto& ul_cfg                                             = cell.rr_cfg_common_scell_r10.ul_cfg_r10;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10_present      = true;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10              = cc_cfg->cell_cfg.ul_earfcn;
    ul_cfg.p_max_r10_present                                 = cell_sib1.p_max_present;
    ul_cfg.p_max_r10                                         = cell_sib1.p_max;
    ul_cfg.ul_freq_info_r10.add_spec_emission_scell_r10      = 1;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.p0_nominal_pusch_r10 = cc_cfg_sib.ul_pwr_ctrl_common.p0_nominal_pusch;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.alpha_r10.value      = cc_cfg_sib.ul_pwr_ctrl_common.alpha;
    ul_cfg.srs_ul_cfg_common_r10                             = cc_cfg_sib.srs_ul_cfg_common;
    ul_cfg.ul_cp_len_r10.value                               = cc_cfg_sib.ul_cp_len.value;
    ul_cfg.pusch_cfg_common_r10                              = cc_cfg_sib.pusch_cfg_common;
    // RadioResourceConfigDedicatedSCell-r10
    cell.rr_cfg_ded_scell_r10_present                                       = true;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10_present                = true;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10_present = true;
    auto& nonul_cfg_ded                = cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10;
    nonul_cfg_ded.ant_info_r10_present = true;
    asn1::number_to_enum(nonul_cfg_ded.ant_info_r10.tx_mode_r10, parent->cfg.cell.nof_ports);
    nonul_cfg_ded.ant_info_r10.ue_tx_ant_sel.set(setup_opts::release);
    nonul_cfg_ded.cross_carrier_sched_cfg_r10_present                                            = true;
    nonul_cfg_ded.cross_carrier_sched_cfg_r10.sched_cell_info_r10.set_own_r10().cif_presence_r10 = false;
    nonul_cfg_ded.pdsch_cfg_ded_r10_present                                                      = true;
    nonul_cfg_ded.pdsch_cfg_ded_r10.p_a.value                           = parent->cfg.pdsch_cfg.value;
    cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10_present = true;
    auto& ul_cfg_ded                                  = cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10;
    ul_cfg_ded.ant_info_ul_r10_present                = true;
    ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10_present = true;
    asn1::number_to_enum(ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10, parent->cfg.cell.nof_ports);
    ul_cfg_ded.pusch_cfg_ded_scell_r10_present           = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10_present         = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.p0_ue_pusch_r10 = 0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.delta_mcs_enabled_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::en0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.accumulation_enabled_r10   = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10_present = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10         = 3;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.pathloss_ref_linking_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_opts::scell;
    ul_cfg_ded.cqi_report_cfg_scell_r10_present                               = true;
    ul_cfg_ded.cqi_report_cfg_scell_r10.nom_pdsch_rs_epre_offset_r10          = 0;
    ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10_present = true;

    // Add 256QAM
    if (ue_capabilities.category_dl >= 11 && ue_capabilities.support_dl_256qam) {
      cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250.set_present(true);
      auto cqi_report_cfg_scell = cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250.get();
      cqi_report_cfg_scell->alt_cqi_table_r12_present = true;
      cqi_report_cfg_scell->alt_cqi_table_r12 = asn1::rrc::cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_::all_sfs;
    }

    // Get CQI allocation for secondary cell
    auto& cqi_setup = ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10.set_setup();
    get_cqi(&cqi_setup.cqi_pmi_cfg_idx, &cqi_setup.cqi_pucch_res_idx_r10, scell_idx);

    cqi_setup.cqi_format_ind_periodic_r10.set_wideband_cqi_r10();
    cqi_setup.simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
#if SRS_ENABLED
    ul_cfg_ded.srs_ul_cfg_ded_r10_present                  = true;
    auto& srs_setup                                        = ul_cfg_ded.srs_ul_cfg_ded_r10.set_setup();
    srs_setup.srs_bw.value                                 = srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::bw0;
    srs_setup.srs_hop_bw.value                             = srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::hbw0;
    srs_setup.freq_domain_position                         = 0;
    srs_setup.dur                                          = true;
    srs_setup.srs_cfg_idx                                  = 167;
    srs_setup.tx_comb                                      = 0;
    srs_setup.cyclic_shift.value                           = srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::cs0;
    ul_cfg_ded.srs_ul_cfg_ded_v1020_present                = true;
    ul_cfg_ded.srs_ul_cfg_ded_v1020.srs_ant_port_r10.value = srs_ant_port_opts::an1;
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10_present        = true;
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10.set(setup_opts::release);
#endif // SRS_ENABLED
    list.push_back(cell);

    // Create new PHY configuration structure for this SCell
    phy_interface_rrc_lte::phy_rrc_dedicated_t scell_phy_rrc_ded = {};
    srslte::set_phy_cfg_t_scell_config(&scell_phy_rrc_ded.phy_cfg, cell);
    scell_phy_rrc_ded.configured = true;

    // Get corresponding eNB CC index
    scell_phy_rrc_ded.enb_cc_idx = cc_cfg->enb_cc_idx;

    // Append to PHY RRC config dedicated which will be applied further down
    phy_rrc_dedicated_list.push_back(scell_phy_rrc_ded);
  }

  // Set DL HARQ Feedback mode
  conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.set_present(true);
  conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020->pucch_format_r10_present = true;
  conn_reconf->rr_cfg_ded.phys_cfg_ded.ext                                           = true;
  auto pucch_format_r10                      = conn_reconf->rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.get();
  pucch_format_r10->pucch_format_r10_present = true;
  auto& ch_sel_r10                           = pucch_format_r10->pucch_format_r10.set_ch_sel_r10();
  ch_sel_r10.n1_pucch_an_cs_r10_present      = true;
  ch_sel_r10.n1_pucch_an_cs_r10.set_setup();
  n1_pucch_an_cs_r10_l item0(4);
  // TODO: should we use a different n1PUCCH-AN-CS-List configuration?
  for (auto& it : item0) {
    it = cell_ded_list.is_pucch_cs_allocated() ? *cell_ded_list.get_n_pucch_cs() : 0;
  }
  ch_sel_r10.n1_pucch_an_cs_r10.setup().n1_pucch_an_cs_list_r10.push_back(item0);

  return SRSLTE_SUCCESS;
}

void rrc::ue::send_connection_reconf_new_bearer(const asn1::s1ap::erab_to_be_setup_list_bearer_su_req_l& e)
{
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
  dl_dcch_msg.msg.c1().rrc_conn_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg_r8_ies_s* conn_reconf = &dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  for (const auto& item : e) {
    auto&   erab = item.value.erab_to_be_setup_item_bearer_su_req();
    uint8_t id   = erab.erab_id;
    uint8_t lcid = id - 2; // Map e.g. E-RAB 5 to LCID 3 (==DRB1)

    // Get DRB configuration
    drb_to_add_mod_s drb_item;
    if (get_drbid_config(&drb_item, lcid - 2)) {
      parent->rrc_log->error("Getting DRB configuration\n");
      parent->rrc_log->console("ERROR: The QCI %d is invalid or not configured.\n", erabs[id].qos_params.qci);
      // TODO: send S1AP response indicating error?
      return;
    }

    // Add DRB to the scheduler
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
    bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    parent->mac->bearer_ue_cfg(rnti, lcid, &bearer_cfg);
    current_sched_ue_cfg.ue_bearers[lcid] = bearer_cfg;

    // Configure DRB in RLC
    parent->rlc->add_bearer(rnti, lcid, srslte::make_rlc_config_t(drb_item.rlc_cfg));

    // Configure DRB in PDCP
    // TODO: Review all ID mapping LCID DRB ERAB EPSBID Mapping
    if (drb_item.pdcp_cfg_present) {
      parent->pdcp->add_bearer(
          rnti, lcid, srslte::make_drb_pdcp_config_t(drb_item.drb_id - 1, false, drb_item.pdcp_cfg));
    } else {
      // use default config
      parent->pdcp->add_bearer(rnti, lcid, srslte::make_drb_pdcp_config_t(drb_item.drb_id - 1, false));
    }

    // DRB has already been configured in GTPU through bearer setup
    conn_reconf->rr_cfg_ded.drb_to_add_mod_list.push_back(drb_item);

    // Add NAS message
    std::map<uint8_t, srslte::unique_byte_buffer_t>::const_iterator it = erab_info_list.find(id);
    if (it != erab_info_list.end()) {
      const srslte::unique_byte_buffer_t& erab_info = erab_info_list[id];
      parent->rrc_log->info_hex(
          erab_info->msg, erab_info->N_bytes, "reconf_new_bearer erab_info -> nas_info rnti 0x%x\n", rnti);
      asn1::dyn_octstring octstr(erab_info->N_bytes);
      memcpy(octstr.data(), erab_info->msg, erab_info->N_bytes);
      conn_reconf->ded_info_nas_list.push_back(octstr);
      conn_reconf->ded_info_nas_list_present = true;
      erab_info_list.erase(it);
    }
  }
  conn_reconf->rr_cfg_ded_present                     = true;
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list_present = conn_reconf->rr_cfg_ded.drb_to_add_mod_list.size() > 0;
  conn_reconf->ded_info_nas_list_present              = conn_reconf->ded_info_nas_list.size() > 0;

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));
}

void rrc::ue::send_security_mode_command()
{
  dl_dcch_msg_s        dl_dcch_msg;
  security_mode_cmd_s* comm = &dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  comm->rrc_transaction_id  = (uint8_t)((transaction_id++) % 4);

  // TODO: select these based on UE capabilities and preference order
  comm->crit_exts.set_c1().set_security_mode_cmd_r8();
  comm->crit_exts.c1().security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg.ciphering_algorithm =
      (ciphering_algorithm_r12_e::options)sec_cfg.cipher_algo;
  comm->crit_exts.c1().security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg.integrity_prot_algorithm =
      (security_algorithm_cfg_s::integrity_prot_algorithm_e_::options)sec_cfg.integ_algo;
  last_security_mode_cmd = comm->crit_exts.c1().security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg;

  send_dl_dcch(&dl_dcch_msg);
}

void rrc::ue::send_ue_cap_enquiry()
{
  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_ue_cap_enquiry().crit_exts.set_c1().set_ue_cap_enquiry_r8();

  ue_cap_enquiry_s* enq   = &dl_dcch_msg.msg.c1().ue_cap_enquiry();
  enq->rrc_transaction_id = (uint8_t)((transaction_id++) % 4);

  enq->crit_exts.c1().ue_cap_enquiry_r8().ue_cap_request.resize(1);
  enq->crit_exts.c1().ue_cap_enquiry_r8().ue_cap_request[0].value = rat_type_e::eutra;

  send_dl_dcch(&dl_dcch_msg);
}

/********************** Handover **************************/

void rrc::ue::handle_ho_preparation_complete(bool is_success, srslte::unique_byte_buffer_t container)
{
  mobility_handler->handle_ho_preparation_complete(is_success, std::move(container));
}

/********************** HELPERS ***************************/

bool rrc::ue::select_security_algorithms()
{
  // Each position in the bitmap represents an encryption algorithm:
  // “all bits equal to 0” – UE supports no other algorithm than EEA0,
  // “first bit” – 128-EEA1,
  // “second bit” – 128-EEA2,
  // “third bit” – 128-EEA3,
  // other bits reserved for future use. Value ‘1’ indicates support and value
  // ‘0’ indicates no support of the algorithm.
  // Algorithms are defined in TS 33.401 [15].
  // Note: information missing

  bool enc_algo_found   = false;
  bool integ_algo_found = false;

  for (auto& cipher_item : parent->cfg.eea_preference_list) {
    auto& v = security_capabilities.encryption_algorithms;
    switch (cipher_item) {
      case srslte::CIPHERING_ALGORITHM_ID_EEA0:
        // “all bits equal to 0” – UE supports no other algorithm than EEA0,
        // specification does not cover the case in which EEA0 is supported with other algorithms
        // just assume that EEA0 is always supported even this can not be explicity signaled by S1AP
        sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_EEA0;
        enc_algo_found      = true;
        parent->rrc_log->info("Selected EEA0 as RRC encryption algorithm\n");
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA1:
        // “first bit” – 128-EEA1,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA1)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
          enc_algo_found      = true;
          parent->rrc_log->info("Selected EEA1 as RRC encryption algorithm\n");
          break;
        } else {
          parent->rrc_log->info("Failed to selected EEA1 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA2:
        // “second bit” – 128-EEA2,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA2)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA2;
          enc_algo_found      = true;
          parent->rrc_log->info("Selected EEA2 as RRC encryption algorithm\n");
          break;
        } else {
          parent->rrc_log->info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA3:
        // “third bit” – 128-EEA3,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA3)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA3;
          enc_algo_found      = true;
          parent->rrc_log->info("Selected EEA3 as RRC encryption algorithm\n");
          break;
        } else {
          parent->rrc_log->info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      default:
        enc_algo_found = false;
        break;
    }
    if (enc_algo_found) {
      break;
    }
  }

  for (auto& eia_enum : parent->cfg.eia_preference_list) {
    auto& v = security_capabilities.integrity_protection_algorithms;
    switch (eia_enum) {
      case srslte::INTEGRITY_ALGORITHM_ID_EIA0:
        // Null integrity is not supported
        parent->rrc_log->info("Skipping EIA0 as RRC integrity algorithm. Null integrity is not supported.\n");
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA1:
        // “first bit” – 128-EIA1,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA1)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;
          integ_algo_found   = true;
          parent->rrc_log->info("Selected EIA1 as RRC integrity algorithm.\n");
        } else {
          parent->rrc_log->info("Failed to selected EIA1 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA2:
        // “second bit” – 128-EIA2,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA2)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA2;
          integ_algo_found   = true;
          parent->rrc_log->info("Selected EIA2 as RRC integrity algorithm.\n");
        } else {
          parent->rrc_log->info("Failed to selected EIA2 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA3:
        // “third bit” – 128-EIA3,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA3)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA3;
          integ_algo_found   = true;
          parent->rrc_log->info("Selected EIA3 as RRC integrity algorithm.\n");
        } else {
          parent->rrc_log->info("Failed to selected EIA3 as RRC encryption algorithm, due to unsupported algorithm\n");
        }
        break;
      default:
        integ_algo_found = false;
        break;
    }

    if (integ_algo_found) {
      break;
    }
  }

  if (not integ_algo_found || not enc_algo_found) {
    // TODO: if no security algorithm found abort radio connection and issue
    // encryption-and-or-integrity-protection-algorithms-not-supported message
    parent->rrc_log->error("Did not find a matching integrity or encryption algorithm with the UE\n");
    return false;
  }
  return true;
}
void rrc::ue::send_dl_ccch(dl_ccch_msg_s* dl_ccch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_ccch_msg->pack(bref) != asn1::SRSASN_SUCCESS) {
      parent->rrc_log->error_hex(pdu->msg, pdu->N_bytes, "Failed to pack DL-CCCH-Msg:\n");
      return;
    }
    pdu->N_bytes = 1u + (uint32_t)bref.distance_bytes(pdu->msg);

    char buf[32] = {};
    sprintf(buf, "SRB0 - rnti=0x%x", rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg, dl_ccch_msg->msg.c1().type().to_string());
    parent->rlc->write_sdu(rnti, RB_ID_SRB0, std::move(pdu));
  } else {
    parent->rrc_log->error("Allocating pdu\n");
  }
}

void rrc::ue::send_dl_dcch(dl_dcch_msg_s* dl_dcch_msg, srslte::unique_byte_buffer_t pdu)
{
  if (!pdu) {
    pdu = srslte::allocate_unique_buffer(*pool);
  }
  if (pdu) {
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    if (dl_dcch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      parent->rrc_log->error("Failed to encode DL-DCCH-Msg\n");
      return;
    }
    pdu->N_bytes = 1u + (uint32_t)bref.distance_bytes(pdu->msg);

    // send on SRB2 if user is fully registered (after RRC reconfig complete)
    uint32_t lcid =
        parent->rlc->has_bearer(rnti, RB_ID_SRB2) && state == RRC_STATE_REGISTERED ? RB_ID_SRB2 : RB_ID_SRB1;

    char buf[32] = {};
    sprintf(buf, "SRB%d - rnti=0x%x", lcid, rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_dcch_msg, dl_dcch_msg->msg.c1().type().to_string());

    parent->pdcp->write_sdu(rnti, lcid, std::move(pdu));
  } else {
    parent->rrc_log->error("Allocating pdu\n");
  }
}

void rrc::ue::apply_setup_phy_common(const asn1::rrc::rr_cfg_common_sib_s& config)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Flatten common configuration
  auto& current_phy_cfg = phy_rrc_dedicated_list[0].phy_cfg;
  set_phy_cfg_t_common_prach(&current_phy_cfg, &config.prach_cfg.prach_cfg_info, config.prach_cfg.root_seq_idx);
  set_phy_cfg_t_common_pdsch(&current_phy_cfg, config.pdsch_cfg_common);
  set_phy_cfg_t_common_pusch(&current_phy_cfg, config.pusch_cfg_common);
  set_phy_cfg_t_common_pucch(&current_phy_cfg, config.pucch_cfg_common);
  set_phy_cfg_t_common_srs(&current_phy_cfg, config.srs_ul_cfg_common);
  set_phy_cfg_t_common_pwr_ctrl(&current_phy_cfg, config.ul_pwr_ctrl_common);

  // Set PCell index
  phy_rrc_dedicated_list[0].configured = true;
  phy_rrc_dedicated_list[0].enb_cc_idx = current_sched_ue_cfg.supported_cc_list[0].enb_cc_idx;

  // Send configuration to physical layer
  if (parent->phy != nullptr) {
    parent->phy->set_config_dedicated(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_setup_phy_config_dedicated(const asn1::rrc::phys_cfg_ded_s& phys_cfg_ded)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Load PCell dedicated configuration
  srslte::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);

  // Deactivates eNb/Cells for this UE
  for (uint32_t cc = 1; cc < phy_rrc_dedicated_list.size(); cc++) {
    phy_rrc_dedicated_list[cc].configured = false;
  }

  // Send configuration to physical layer
  if (parent->phy != nullptr) {
    parent->phy->set_config_dedicated(rnti, phy_rrc_dedicated_list);
  }
}

void rrc::ue::apply_reconf_phy_config(const asn1::rrc::rrc_conn_recfg_r8_ies_s& reconfig_r8)
{
  // Return if no cell is supported
  if (phy_rrc_dedicated_list.empty()) {
    return;
  }

  // Configure PCell if available configuration
  if (reconfig_r8.rr_cfg_ded_present) {
    auto& rr_cfg_ded = reconfig_r8.rr_cfg_ded;
    if (rr_cfg_ded.phys_cfg_ded_present) {
      auto& phys_cfg_ded = rr_cfg_ded.phys_cfg_ded;
      srslte::set_phy_cfg_t_dedicated_cfg(&phy_rrc_dedicated_list[0].phy_cfg, phys_cfg_ded);
    }
  }

  // Parse extensions
  if (reconfig_r8.non_crit_ext_present) {
    auto& reconfig_r890 = reconfig_r8.non_crit_ext;
    if (reconfig_r890.non_crit_ext_present) {
      auto& reconfig_r920 = reconfig_r890.non_crit_ext;
      if (reconfig_r920.non_crit_ext_present) {
        auto& reconfig_r1020 = reconfig_r920.non_crit_ext;

        // Handle Add/Modify SCell list
        if (reconfig_r1020.scell_to_add_mod_list_r10_present) {
          // This is already applied when packing the SCell list
        }
      }
    }
  }

  // Send configuration to physical layer
  if (parent->phy != nullptr) {
    parent->phy->set_config_dedicated(rnti, phy_rrc_dedicated_list);
  }
}

int rrc::ue::get_cqi(uint16_t* pmi_idx, uint16_t* n_pucch, uint32_t ue_cc_idx)
{
  cell_ctxt_dedicated* c = cell_ded_list.get_ue_cc_idx(ue_cc_idx);
  if (c != nullptr and c->cqi_res_present) {
    *pmi_idx = c->cqi_res.pmi_idx;
    *n_pucch = c->cqi_res.pucch_res;
    return SRSLTE_SUCCESS;
  } else {
    parent->rrc_log->error("CQI resources for ue_cc_idx=%d have not been allocated\n", ue_cc_idx);
    return SRSLTE_ERROR;
  }
}

bool rrc::ue::is_allocated() const
{
  return cell_ded_list.is_allocated();
}

int rrc::ue::get_ri(uint32_t m_ri, uint16_t* ri_idx)
{
  int32_t ret = SRSLTE_SUCCESS;

  uint32_t I_ri        = 0;
  int32_t  N_offset_ri = 0; // Naivest approach: overlap RI with PMI
  switch (m_ri) {
    case 0:
      // Disabled
      break;
    case 1:
      I_ri = -N_offset_ri;
      break;
    case 2:
      I_ri = 161 - N_offset_ri;
      break;
    case 4:
      I_ri = 322 - N_offset_ri;
      break;
    case 8:
      I_ri = 483 - N_offset_ri;
      break;
    case 16:
      I_ri = 644 - N_offset_ri;
      break;
    case 32:
      I_ri = 805 - N_offset_ri;
      break;
    default:
      parent->rrc_log->error("Allocating RI: invalid m_ri=%d\n", m_ri);
  }

  // If ri_dix is available, copy
  if (ri_idx) {
    *ri_idx = I_ri;
  }

  return ret;
}

} // namespace srsenb
