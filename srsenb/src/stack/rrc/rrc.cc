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

#include "srsenb/hdr/stack/rrc/rrc.h"
#include "srslte/asn1/asn1_utils.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/int_helpers.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/srslte.h"

using srslte::byte_buffer_t;
using srslte::bit_buffer_t;
using srslte::uint32_to_uint8;
using srslte::uint8_to_uint32;

using namespace asn1::rrc;

namespace srsenb {

void rrc::init(rrc_cfg_t*               cfg_,
               phy_interface_stack_lte* phy_,
               mac_interface_rrc*       mac_,
               rlc_interface_rrc*       rlc_,
               pdcp_interface_rrc*      pdcp_,
               s1ap_interface_rrc*      s1ap_,
               gtpu_interface_rrc*      gtpu_,
               srslte::log*             log_rrc)
{
  phy     = phy_; 
  mac     = mac_; 
  rlc     = rlc_; 
  pdcp    = pdcp_; 
  gtpu    = gtpu_;
  s1ap    = s1ap_; 
  rrc_log = log_rrc;
  cnotifier = NULL; 

  running = false;
  pool    = srslte::byte_buffer_pool::get_instance();

  cfg = *cfg_;

  if (cfg.sibs[12].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib13_v920 &&
      cfg_->enable_mbsfn) {
    configure_mbsfn_sibs(&cfg.sibs[1].sib2(), &cfg.sibs[12].sib13_v920());
  }
  
  nof_si_messages = generate_sibs();  
  config_mac();
 
  pthread_mutex_init(&user_mutex, NULL);
  pthread_mutex_init(&paging_mutex, NULL);

  act_monitor.start(RRC_THREAD_PRIO);
  bzero(&sr_sched, sizeof(sr_sched_t));
  
  start(RRC_THREAD_PRIO);
}

void rrc::set_connect_notifer(connect_notifier *cnotifier)
{
  this->cnotifier = cnotifier; 
}

void rrc::stop()
{
  if(running) {
    running = false;
    rrc_pdu p = {0, LCID_EXIT, NULL};
    rx_pdu_queue.push(std::move(p));
    wait_thread_finish();
  }
  act_monitor.stop();
  pthread_mutex_lock(&user_mutex);
  users.clear();
  pthread_mutex_unlock(&user_mutex);
  pthread_mutex_destroy(&user_mutex);
  pthread_mutex_destroy(&paging_mutex);
}


/*******************************************************************************
  Public functions

  All public functions must be mutexed. 
*******************************************************************************/

void rrc::get_metrics(rrc_metrics_t &m)
{
  if (running) {
    pthread_mutex_lock(&user_mutex);
    m.n_ues = 0;
    for(std::map<uint16_t, ue>::iterator iter=users.begin(); m.n_ues < ENB_METRICS_MAX_USERS &&iter!=users.end(); ++iter) {
      ue *u = (ue*) &iter->second;
      if(iter->first != SRSLTE_MRNTI){
        m.ues[m.n_ues++].state = u->get_state();
      }
    }
    pthread_mutex_unlock(&user_mutex);
  }
}

/*******************************************************************************
  MAC interface

  Those functions that shall be called from a phch_worker should push the command
  to the queue and process later
*******************************************************************************/

void rrc::read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t* payload)
{
  if (sib_index < ASN1_RRC_MAX_SIB) {
    memcpy(payload, sib_buffer[sib_index]->msg, sib_buffer[sib_index]->N_bytes);
  } 
}

void rrc::rl_failure(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_RLF_USER, NULL};
  rx_pdu_queue.push(std::move(p));
}

void rrc::set_activity_user(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_ACT_USER, NULL};
  rx_pdu_queue.push(std::move(p));
}

void rrc::rem_user_thread(uint16_t rnti)
{
  rrc_pdu p = {rnti, LCID_REM_USER, NULL};
  rx_pdu_queue.push(std::move(p));
}

uint32_t rrc::get_nof_users() {
  return users.size();
}

template <class T>
void rrc::log_rrc_message(const std::string& source, const direction_t dir, const srslte::byte_buffer_t* pdu,
                          const T& msg)
{
  const char* msg_type = msg.msg.c1().type().to_string().c_str();
  if (rrc_log->get_level() == srslte::LOG_LEVEL_INFO) {
    rrc_log->info("%s - %s %s (%d B)\n", source.c_str(), dir == Tx ? "Tx" : "Rx", msg_type, pdu->N_bytes);
  } else if (rrc_log->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    rrc_log->debug_hex(
        pdu->msg, pdu->N_bytes, "%s - %s %s (%d B)\n", source.c_str(), dir == Tx ? "Tx" : "Rx", msg_type, pdu->N_bytes);
    rrc_log->debug_long("Content:\n%s\n", json_writer.to_string().c_str());
  }
}

void rrc::max_retx_attempted(uint16_t rnti)
{

}

// This function is called from PRACH worker (can wait)
void rrc::add_user(uint16_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  if (users.count(rnti) == 0) {

    users[rnti].parent = this;
    users[rnti].rnti   = rnti;
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    rrc_log->info("Added new user rnti=0x%x\n", rnti);
  } else {
    rrc_log->error("Adding user rnti=0x%x (already exists)\n", rnti);
  }

  if (rnti == SRSLTE_MRNTI) {
    srslte::pdcp_config_t cfg = {
        .bearer_id    = 1,
        .rb_type      = srslte::PDCP_RB_IS_DRB,
        .tx_direction = srslte::SECURITY_DIRECTION_DOWNLINK,
        .rx_direction = srslte::SECURITY_DIRECTION_UPLINK,
        .sn_len       = srslte::PDCP_SN_LEN_12,
    };

    uint32_t teid_in = 1;

    for (uint32_t i = 0; i < mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9.size();
         i++) {
      uint32_t lcid = mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9[i].lc_ch_id_r9;
      rlc->add_bearer_mrb(SRSLTE_MRNTI,lcid);
      pdcp->add_bearer(SRSLTE_MRNTI,lcid,cfg);
      gtpu->add_bearer(SRSLTE_MRNTI,lcid, 1, 1, &teid_in);
    }
  }

  pthread_mutex_unlock(&user_mutex);
}

/* Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a
 * valid RNTI.
 * Called by MAC reader thread (can wait to process)
 */
void rrc::upd_user(uint16_t new_rnti, uint16_t old_rnti)
{
  // Remove new_rnti
  rem_user_thread(new_rnti);

  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  pthread_mutex_lock(&user_mutex);
  if (users.count(old_rnti) == 1) {
    if (users[old_rnti].is_connected()) {
      users[old_rnti].send_connection_reconf_upd(srslte::allocate_unique_buffer(*pool));
    } else {
      users[old_rnti].send_connection_release();
    }
  }
  pthread_mutex_unlock(&user_mutex);
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

  pthread_mutex_lock(&user_mutex);

  if (users.count(rnti) == 1) {
    dl_info_transfer_r8_ies_s* dl_info_r8 =
        &msg_c1->set_dl_info_transfer().crit_exts.set_c1().set_dl_info_transfer_r8();
    //    msg_c1->dl_info_transfer().rrc_transaction_id = ;
    dl_info_r8->non_crit_ext_present      = false;
    dl_info_r8->ded_info_type.set_ded_info_nas();
    dl_info_r8->ded_info_type.ded_info_nas().resize(sdu->N_bytes);
    memcpy(msg_c1->dl_info_transfer().crit_exts.c1().dl_info_transfer_r8().ded_info_type.ded_info_nas().data(),
           sdu->msg, sdu->N_bytes);

    sdu->clear();

    users[rnti].send_dl_dcch(&dl_dcch_msg, std::move(sdu));
  } else {
    rrc_log->error("Rx SDU for unknown rnti=0x%x\n", rnti);
  }

  pthread_mutex_unlock(&user_mutex);
}

void rrc::release_complete(uint16_t rnti) {
  rrc_pdu p = {rnti, LCID_REL_USER, NULL};
  rx_pdu_queue.push(std::move(p));
}

bool rrc::setup_ue_ctxt(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT *msg)
{
  pthread_mutex_lock(&user_mutex);

  rrc_log->info("Adding initial context for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    pthread_mutex_unlock(&user_mutex);
    return false;
  }

  if(msg->AdditionalCSFallbackIndicator_present) {
    rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
  }
  if(msg->CSGMembershipStatus_present) {
    rrc_log->warning("Not handling CSGMembershipStatus\n");
  }
  if(msg->GUMMEI_ID_present) {
    rrc_log->warning("Not handling GUMMEI_ID\n");
  }
  if(msg->HandoverRestrictionList_present) {
    rrc_log->warning("Not handling HandoverRestrictionList\n");
  }
  if(msg->ManagementBasedMDTAllowed_present) {
    rrc_log->warning("Not handling ManagementBasedMDTAllowed\n");
  }
  if(msg->ManagementBasedMDTPLMNList_present) {
    rrc_log->warning("Not handling ManagementBasedMDTPLMNList\n");
  }
  if(msg->MME_UE_S1AP_ID_2_present) {
    rrc_log->warning("Not handling MME_UE_S1AP_ID_2\n");
  }
  if(msg->RegisteredLAI_present) {
    rrc_log->warning("Not handling RegisteredLAI\n");
  }
  if(msg->SRVCCOperationPossible_present) {
    rrc_log->warning("Not handling SRVCCOperationPossible\n");
  }
  if(msg->SubscriberProfileIDforRFP_present) {
    rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
  }
  if(msg->TraceActivation_present) {
    rrc_log->warning("Not handling TraceActivation\n");
  }
  if(msg->UERadioCapability_present) {
    rrc_log->warning("Not handling UERadioCapability\n");
  }

  // UEAggregateMaximumBitrate
  users[rnti].set_bitrates(&msg->uEaggregateMaximumBitrate);

  // UESecurityCapabilities
  users[rnti].set_security_capabilities(&msg->UESecurityCapabilities);

  // SecurityKey
  uint8_t key[32];
  liblte_pack(msg->SecurityKey.buffer, LIBLTE_S1AP_SECURITYKEY_BIT_STRING_LEN, key);
  users[rnti].set_security_key(key, LIBLTE_S1AP_SECURITYKEY_BIT_STRING_LEN/8);

  // CSFB
  if (msg->CSFallbackIndicator_present) {
    if (msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_REQUIRED ||
        msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_HIGH_PRIORITY) {
      users[rnti].is_csfb = true;
    }
  }

  // Send RRC security mode command
  users[rnti].send_security_mode_command();

  // Setup E-RABs
  users[rnti].setup_erabs(&msg->E_RABToBeSetupListCtxtSUReq);

  pthread_mutex_unlock(&user_mutex);

  return true;
}

bool rrc::modify_ue_ctxt(uint16_t rnti, LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT *msg)
{
  bool err = false;
  pthread_mutex_lock(&user_mutex);

  rrc_log->info("Modifying context for 0x%x\n", rnti);

  if (users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    pthread_mutex_unlock(&user_mutex);
    return false;
  }

  if (msg->CSFallbackIndicator_present) {
    if (msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_REQUIRED ||
        msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_HIGH_PRIORITY) {
      /* Remember that we are in a CSFB right now */
      users[rnti].is_csfb = true;
    }
  }

  if (msg->AdditionalCSFallbackIndicator_present) {
    rrc_log->warning("Not handling AdditionalCSFallbackIndicator\n");
    err = true;
  }
  if (msg->CSGMembershipStatus_present) {
    rrc_log->warning("Not handling CSGMembershipStatus\n");
    err = true;
  }
  if (msg->RegisteredLAI_present) {
    rrc_log->warning("Not handling RegisteredLAI\n");
    err = true;
  }
  if (msg->SubscriberProfileIDforRFP_present) {
    rrc_log->warning("Not handling SubscriberProfileIDforRFP\n");
    err = true;
  }

  if (err) {
    // maybe pass a cause value?
    pthread_mutex_unlock(&user_mutex);
    return false;
  }

  // UEAggregateMaximumBitrate
  if (msg->uEaggregateMaximumBitrate_present) {
    users[rnti].set_bitrates(&msg->uEaggregateMaximumBitrate);
  }

  // UESecurityCapabilities
  if (msg->UESecurityCapabilities_present) {
    users[rnti].set_security_capabilities(&msg->UESecurityCapabilities);
  }

  // SecurityKey
  if (msg->SecurityKey_present) {
    uint8_t key[32];
    liblte_pack(msg->SecurityKey.buffer, LIBLTE_S1AP_SECURITYKEY_BIT_STRING_LEN, key);
    users[rnti].set_security_key(key, LIBLTE_S1AP_SECURITYKEY_BIT_STRING_LEN / 8);

    // Send RRC security mode command ??
    users[rnti].send_security_mode_command();
  }

  pthread_mutex_unlock(&user_mutex);

  return true;
}


bool rrc::setup_ue_erabs(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT *msg)
{
  pthread_mutex_lock(&user_mutex);

  rrc_log->info("Setting up erab(s) for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    pthread_mutex_unlock(&user_mutex);
    return false;
  }

  if(msg->uEaggregateMaximumBitrate_present) {
    // UEAggregateMaximumBitrate
    users[rnti].set_bitrates(&msg->uEaggregateMaximumBitrate);
  }

  // Setup E-RABs
  users[rnti].setup_erabs(&msg->E_RABToBeSetupListBearerSUReq);

  pthread_mutex_unlock(&user_mutex);

  return true;
}

bool rrc::release_erabs(uint32_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  rrc_log->info("Releasing E-RABs for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    pthread_mutex_unlock(&user_mutex);
    return false;
  }

  bool ret = users[rnti].release_erabs();
  pthread_mutex_unlock(&user_mutex);
  return ret;
}



/*******************************************************************************
  Paging functions
  These functions use a different mutex because access different shared variables
  than user map
*******************************************************************************/

void rrc::add_paging_id(uint32_t ueid, LIBLTE_S1AP_UEPAGINGID_STRUCT UEPagingID) 
{
  pthread_mutex_lock(&paging_mutex);
  if (pending_paging.count(ueid) == 0) {
    pending_paging[ueid] = UEPagingID;
  } else {
    rrc_log->warning("Received Paging for UEID=%d but not yet transmitted\n", ueid);
  }
  pthread_mutex_unlock(&paging_mutex);
}

// Described in Section 7 of 36.304
bool rrc::is_paging_opportunity(uint32_t tti, uint32_t *payload_len)
{
  int sf_pattern[4][4] = {{9, 4, -1, 0}, {-1, 9, -1, 4}, {-1, -1, -1, 5}, {-1, -1, -1, 9}};
  
  if (pending_paging.empty()) {
    return false; 
  }

  pthread_mutex_lock(&paging_mutex);

  asn1::rrc::pcch_msg_s pcch_msg;
  pcch_msg.msg.set_c1();
  paging_s* paging_rec = &pcch_msg.msg.c1().paging();

  // Default paging cycle, should get DRX from user
  uint32_t T  = cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.default_paging_cycle.to_number();
  uint32_t Nb = T * cfg.sibs[1].sib2().rr_cfg_common.pcch_cfg.nb;

  uint32_t N  = T<Nb?T:Nb;
  uint32_t Ns = Nb/T>1?Nb/T:1; 
  uint32_t sfn = tti/10;   
  
  std::vector<uint32_t> ue_to_remove;

  int n = 0;
  for (std::map<uint32_t, LIBLTE_S1AP_UEPAGINGID_STRUCT>::iterator iter = pending_paging.begin();
       n < ASN1_RRC_MAX_PAGE_REC && iter != pending_paging.end(); ++iter) {
    LIBLTE_S1AP_UEPAGINGID_STRUCT u    = (LIBLTE_S1AP_UEPAGINGID_STRUCT)iter->second;
    uint32_t                      ueid = ((uint32_t)iter->first) % 1024;
    uint32_t                      i_s  = (ueid / N) % Ns;

    if ((sfn % T) == (T / N) * (ueid % N)) {

      int sf_idx = sf_pattern[i_s % 4][(Ns - 1) % 4];
      if (sf_idx < 0) {
        rrc_log->error("SF pattern is N/A for Ns=%d, i_s=%d, imsi_decimal=%d\n", Ns, i_s, ueid);
      } else if ((uint32_t)sf_idx == (tti % 10)) {

        paging_rec->paging_record_list_present = true;
        paging_record_s paging_elem;
        if (u.choice_type == LIBLTE_S1AP_UEPAGINGID_CHOICE_IMSI) {
          paging_elem.ue_id.set_imsi();
          paging_elem.ue_id.imsi().resize(u.choice.iMSI.n_octets);
          memcpy(paging_elem.ue_id.imsi().data(), u.choice.iMSI.buffer, u.choice.iMSI.n_octets);
          rrc_log->console("Warning IMSI paging not tested\n");
        } else {
          paging_elem.ue_id.set_s_tmsi();
          paging_elem.ue_id.s_tmsi().mmec.from_number(u.choice.s_TMSI.mMEC.buffer[0]);
          uint32_t m_tmsi = 0;
          for (int i = 0; i < LIBLTE_S1AP_M_TMSI_OCTET_STRING_LEN; i++) {
            m_tmsi |= u.choice.s_TMSI.m_TMSI.buffer[i] << (8 * (LIBLTE_S1AP_M_TMSI_OCTET_STRING_LEN - i - 1));
          }
          paging_elem.ue_id.s_tmsi().m_tmsi.from_number(m_tmsi);
        }
        paging_elem.cn_domain = paging_record_s::cn_domain_e_::ps;
        paging_rec->paging_record_list.push_back(paging_elem);
        ue_to_remove.push_back(ueid);
        n++;
        rrc_log->info("Assembled paging for ue_id=%d, tti=%d\n", ueid, tti);
      }
    }
  }

  for (uint32_t i = 0; i < ue_to_remove.size(); i++) {
    pending_paging.erase(ue_to_remove[i]);
  }

  pthread_mutex_unlock(&paging_mutex);

  if (paging_rec->paging_record_list.size() > 0) {
    byte_buf_paging.clear();
    asn1::bit_ref bref(byte_buf_paging.msg, byte_buf_paging.get_tailroom());
    pcch_msg.pack(bref);
    byte_buf_paging.N_bytes = (uint32_t)bref.distance_bytes();
    uint32_t N_bits         = (uint32_t)bref.distance();

    if (payload_len) {
      *payload_len = byte_buf_paging.N_bytes;
    }
    rrc_log->info("Assembling PCCH payload with %d UE identities, payload_len=%d bytes, nbits=%d\n",
                  paging_rec->paging_record_list.size(), byte_buf_paging.N_bytes, N_bits);
    log_rrc_message("PCCH-Message", Tx, &byte_buf_paging, pcch_msg);

    return true;
  }

  return false;
}

void rrc::read_pdu_pcch(uint8_t* payload, uint32_t buffer_size)
{
  pthread_mutex_lock(&paging_mutex);
  if (byte_buf_paging.N_bytes <= buffer_size) {
    memcpy(payload, byte_buf_paging.msg, byte_buf_paging.N_bytes);
  }
  pthread_mutex_unlock(&paging_mutex);
}

/*******************************************************************************
  Private functions
  All private functions are not mutexed and must be called from a mutexed enviornment
  from either a public function or the internal thread
*******************************************************************************/

void rrc::parse_ul_ccch(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  uint16_t old_rnti = 0; 

  if (pdu) {
    ul_ccch_msg_s ul_ccch_msg;
    asn1::bit_ref bref(pdu->msg, pdu->N_bytes);
    if (ul_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_ccch_msg.msg.type().value != ul_ccch_msg_type_c::types_opts::c1) {
      rrc_log->error("Failed to unpack UL-CCCH message\n");
      return;
    }

    log_rrc_message("SRB0", Rx, pdu.get(), ul_ccch_msg);

    switch (ul_ccch_msg.msg.c1().type()) {
      case ul_ccch_msg_type_c::c1_c_::types::rrc_conn_request:
        if (users.count(rnti)) {
          users[rnti].handle_rrc_con_req(&ul_ccch_msg.msg.c1().rrc_conn_request());
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
        if (users[rnti].is_idle()) {
          old_rnti = (uint16_t)ul_ccch_msg.msg.c1()
                         .rrc_conn_reest_request()
                         .crit_exts.rrc_conn_reest_request_r8()
                         .ue_id.c_rnti.to_number();
          if (users.count(old_rnti)) {
            rrc_log->error("Not supported: ConnectionReestablishment for rnti=0x%x. Sending Connection Reject\n", old_rnti);
            users[rnti].send_connection_reest_rej();
            s1ap->user_release(old_rnti, LIBLTE_S1AP_CAUSERADIONETWORK_RELEASE_DUE_TO_EUTRAN_GENERATED_REASON);
          } else {
            rrc_log->error("Received ConnectionReestablishment for rnti=0x%x without context\n", old_rnti);
            users[rnti].send_connection_reest_rej();
          }
          // remove temporal rnti
          rrc_log->warning("Received ConnectionReestablishment for rnti=0x%x. Removing temporal rnti=0x%x\n", old_rnti, rnti);
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

void rrc::parse_ul_dcch(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  if (pdu) {
    if (users.count(rnti)) {
      users[rnti].parse_ul_dcch(lcid, std::move(pdu));
    } else {
      rrc_log->error("Processing %s: Unknown rnti=0x%x\n", rb_id_text[lcid], rnti);
    }
  }
}

void rrc::process_rl_failure(uint16_t rnti)
{
  if (users.count(rnti) == 1) {
    uint32_t n_rfl = users[rnti].rl_failure();
    if (n_rfl == 1) {
      rrc_log->info("Radio-Link failure detected rnti=0x%x\n", rnti);
      if (s1ap->user_exists(rnti)) {
        if (!s1ap->user_release(rnti, LIBLTE_S1AP_CAUSERADIONETWORK_RADIO_CONNECTION_WITH_UE_LOST)) {
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

void rrc::process_release_complete(uint16_t rnti)
{
  rrc_log->info("Received Release Complete rnti=0x%x\n", rnti);
  if (users.count(rnti) == 1) {
    if (!users[rnti].is_idle()) {
      rlc->clear_buffer(rnti);
      users[rnti].send_connection_release();
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
  pthread_mutex_lock(&user_mutex);
  if (users.count(rnti) == 1) {
    rrc_log->console("Disconnecting rnti=0x%x.\n", rnti);
    rrc_log->info("Disconnecting rnti=0x%x.\n", rnti);

    /* First remove MAC and GTPU to stop processing DL/UL traffic for this user
     */
    mac->ue_rem(rnti);  // MAC handles PHY
    gtpu->rem_user(rnti);

    // Now remove RLC and PDCP
    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);

    // And deallocate resources from RRC
    users[rnti].sr_free();
    users[rnti].cqi_free();

    users.erase(rnti);
    rrc_log->info("Removed user rnti=0x%x\n", rnti);
  } else {
    rrc_log->error("Removing user rnti=0x%x (does not exist)\n", rnti);
  }
  pthread_mutex_unlock(&user_mutex);
}

void rrc::config_mac()
{
  // Fill MAC scheduler configuration for SIBs
  sched_interface::cell_cfg_t sched_cfg;
  bzero(&sched_cfg, sizeof(sched_interface::cell_cfg_t));
  for (uint32_t i=0;i<nof_si_messages;i++) {
    sched_cfg.sibs[i].len = sib_buffer[i]->N_bytes;
    if (i == 0) {
      sched_cfg.sibs[i].period_rf = 8; // SIB1 is always 8 rf
    } else {
      sched_cfg.sibs[i].period_rf = cfg.sib1.sched_info_list[i - 1].si_periodicity.to_number();
    }
  }
  sched_cfg.si_window_ms = cfg.sib1.si_win_len.to_number();
  sched_cfg.prach_rar_window =
      cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.ra_supervision_info.ra_resp_win_size.to_number();
  sched_cfg.prach_freq_offset = cfg.sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;
  sched_cfg.maxharq_msg3tx    = cfg.sibs[1].sib2().rr_cfg_common.rach_cfg_common.max_harq_msg3_tx;

  sched_cfg.nrb_pucch = SRSLTE_MAX(cfg.sr_cfg.nof_prb, cfg.cqi_cfg.nof_prb);
  rrc_log->info("Allocating %d PRBs for PUCCH\n", sched_cfg.nrb_pucch);

  // Copy Cell configuration
  memcpy(&sched_cfg.cell, &cfg.cell, sizeof(srslte_cell_t));

  // Configure MAC scheduler
  mac->cell_cfg(&sched_cfg);
}

uint32_t rrc::generate_sibs()
{
  // nof_messages includes SIB2 by default, plus all configured SIBs
  uint32_t           nof_messages = 1 + cfg.sib1.sched_info_list.size();
  sched_info_list_l& sched_info   = cfg.sib1.sched_info_list;

  // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
  // all SIBs in a SI message msg[i] share the same periodicity
  asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

  // Copy SIB1 to first SI message
  msg[0].msg.set_c1().set_sib_type1() = cfg.sib1;

  // Copy rest of SIBs
  for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages - 1; sched_info_elem++) {
    uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

    msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info_r8();
    sys_info_r8_ies_s::sib_type_and_info_l_& sib_list =
        msg[msg_index].msg.c1().sys_info().crit_exts.sys_info_r8().sib_type_and_info;

    // SIB2 always in second SI message
    if (msg_index == 1) {
      sib_list.push_back(cfg.sibs[1]);
      // Save SIB2
      sib2 = cfg.sibs[1].sib2();
    }

    // Add other SIBs to this message, if any
    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].sib_map_info.size(); mapping++) {
      sib_list.push_back(cfg.sibs[(int)sched_info[sched_info_elem].sib_map_info[mapping] + 2]);
    }
  }

  // Pack payload for all messages
  for (uint32_t msg_index = 0; msg_index < nof_messages; msg_index++) {
    srslte::unique_byte_buffer_t sib = srslte::allocate_unique_buffer(*pool);
    asn1::bit_ref          bref(sib->msg, sib->get_tailroom());
    asn1::bit_ref          bref0 = bref;
    msg[msg_index].pack(bref);
    sib->N_bytes = static_cast<uint32_t>((bref.distance(bref0) - 1) / 8 + 1);
    sib_buffer.push_back(std::move(sib));

    // Log SIBs in JSON format
    log_rrc_message("SIB payload", Tx, sib_buffer[msg_index].get(), msg[msg_index]);
  }

  if (cfg.sibs[6].type() == asn1::rrc::sys_info_r8_ies_s::sib_type_and_info_item_c_::types::sib7) {
    sib7 = cfg.sibs[6].sib7();
  }

  return nof_messages;
}

void rrc::configure_mbsfn_sibs(sib_type2_s* sib2, sib_type13_r9_s* sib13)
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
    memcpy(&pmch_item->mbms_session_info_list_r9[1].tmgi_r9.service_id_r9[0], &byte[0],
           3); // FIXME: Check if service is set to 1
  }
  pmch_item->pmch_cfg_r9.data_mcs_r9         = 20;
  pmch_item->pmch_cfg_r9.mch_sched_period_r9 = pmch_cfg_r9_s::mch_sched_period_r9_e_::rf64;
  pmch_item->pmch_cfg_r9.sf_alloc_end_r9     = 64 * 6;

  phy->configure_mbsfn(sib2, sib13, mcch);
  mac->write_mcch(sib2, sib13, &mcch);
}

void rrc::configure_security(uint16_t rnti,
                             uint32_t lcid,
                             uint8_t *k_rrc_enc,
                             uint8_t *k_rrc_int,
                             uint8_t *k_up_enc,
                             uint8_t *k_up_int,
                             srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                             srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  // TODO: add k_up_enc, k_up_int support to PDCP
  pdcp->config_security(rnti, lcid, k_rrc_enc, k_rrc_int, k_up_enc, cipher_algo, integ_algo);
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
  RRC thread
*******************************************************************************/

void rrc::run_thread()
{
  rrc_pdu p;
  running = true;

  while(running) {
    p = rx_pdu_queue.wait_pop();
    if (p.pdu) {
      rrc_log->info_hex(p.pdu->msg, p.pdu->N_bytes, "Rx %s PDU", rb_id_text[p.lcid]);
    }

    // Mutex these calls even though it's a private function
    if (users.count(p.rnti) == 1) {
      switch(p.lcid)
      {
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
          if (users.count(p.rnti) == 1) {
            users[p.rnti].set_activity();
          }
          break;
        case LCID_EXIT:
          rrc_log->info("Exiting thread\n");
          break;
        default:
          rrc_log->error("Rx PDU with invalid bearer id: %d", p.lcid);
          break;
      }
    } else {
      rrc_log->warning("Discarding PDU for removed rnti=0x%x\n", p.rnti);
    }
  }
}



/*******************************************************************************
  Activity monitor class
*******************************************************************************/

rrc::activity_monitor::activity_monitor(rrc* parent_) : thread("RRC_ACTIVITY_MONITOR")
{
  running = true;
  parent = parent_;
}

void rrc::activity_monitor::stop()
{
  if (running) {
    running = false;
    thread_cancel();
    wait_thread_finish();
  }
}

void rrc::activity_monitor::run_thread()
{
  while(running)
  {
    usleep(10000);
    pthread_mutex_lock(&parent->user_mutex);
    uint16_t rem_rnti = 0;
    for(std::map<uint16_t, ue>::iterator iter=parent->users.begin(); rem_rnti == 0 && iter!=parent->users.end(); ++iter) {
      if(iter->first != SRSLTE_MRNTI){
        ue *u = (ue*) &iter->second;
        uint16_t rnti = (uint16_t) iter->first;

        if (parent->cnotifier && u->is_connected() && !u->connect_notified) {
          parent->cnotifier->user_connected(rnti);
          u->connect_notified = true;
        }

        if (u->is_timeout()) {
          parent->rrc_log->info("User rnti=0x%x timed out. Exists in s1ap=%s\n", rnti, parent->s1ap->user_exists(rnti)?"yes":"no");
          rem_rnti = rnti;
        }
      }
    }
    if (rem_rnti) {
      if (parent->s1ap->user_exists(rem_rnti)) {
        parent->s1ap->user_release(rem_rnti, LIBLTE_S1AP_CAUSERADIONETWORK_USER_INACTIVITY);
      } else {
        if(rem_rnti != SRSLTE_MRNTI)
          parent->rem_user_thread(rem_rnti);
      }
    }
    pthread_mutex_unlock(&parent->user_mutex);
  }
}





/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/
rrc::ue::ue()
{
  parent            = NULL;
  set_activity();
  has_tmsi          = false;
  connect_notified  = false;
  transaction_id    = 0;
  sr_allocated      = false;
  sr_sched_sf_idx   = 0;
  sr_sched_prb_idx  = 0;
  sr_N_pucch        = 0;
  sr_I              = 0;
  cqi_allocated     = false;
  cqi_pucch         = 0;
  cqi_idx           = 0;
  cqi_sched_sf_idx  = 0;
  cqi_sched_prb_idx = 0;
  rlf_cnt           = 0;
  integ_algo        = srslte::INTEGRITY_ALGORITHM_ID_EIA0;
  cipher_algo       = srslte::CIPHERING_ALGORITHM_ID_EEA0;
  nas_pending       = false;
  is_csfb           = false;
  state             = RRC_STATE_IDLE;
  pool              = srslte::byte_buffer_pool::get_instance();
}

rrc_state_t rrc::ue::get_state()
{
  return state;
}

uint32_t rrc::ue::rl_failure() {
  rlf_cnt++;
  return rlf_cnt;
}

void rrc::ue::set_activity() 
{
  gettimeofday(&t_last_activity, NULL);  
  if (parent) {
    if (parent->rrc_log) {
      parent->rrc_log->debug("Activity registered rnti=0x%x\n", rnti);
    }
  }
}

bool rrc::ue::is_connected() {
  return state == RRC_STATE_REGISTERED;
}

bool rrc::ue::is_idle() {
  return state == RRC_STATE_IDLE;
}

bool rrc::ue::is_timeout() 
{
  if (!parent) {
    return false; 
  }
  
  struct timeval t[3]; 
  uint32_t deadline_s  = 0; 
  uint32_t deadline_us = 0; 
  const char *deadline_str = NULL; 
  memcpy(&t[1], &t_last_activity, sizeof(struct timeval));
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  switch(state) {
    case RRC_STATE_IDLE:  
      deadline_s   = 0;
      deadline_us =
          static_cast<uint32_t>((parent->sib2.rr_cfg_common.rach_cfg_common.max_harq_msg3_tx + 1) * 16 * 1000);
      deadline_str = "RRCConnectionSetup";
      break;
    case RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE:
      deadline_s   = 1;
      deadline_us  = 0;
      deadline_str = "RRCConnectionSetupComplete";
      break;
    case RRC_STATE_RELEASE_REQUEST:
      deadline_s   = 4;
      deadline_us  = 0;
      deadline_str = "RRCReleaseRequest";
      break;
    default:
      deadline_s   = parent->cfg.inactivity_timeout_ms/1000;
      deadline_us  = (parent->cfg.inactivity_timeout_ms%1000)*1000;
      deadline_str = "Activity";
      break;    
  }
  
  if (deadline_str) {
    int64_t deadline = deadline_s*1e6  + deadline_us;
    int64_t elapsed  = t[0].tv_sec*1e6 + t[0].tv_usec;
    if (elapsed > deadline && elapsed > 0) {
      parent->rrc_log->warning("User rnti=0x%x expired %s deadline: %ld:%ld>%d:%d us\n", 
                                rnti, deadline_str, 
                                t[0].tv_sec, t[0].tv_usec, 
                               deadline_s, deadline_us);
      memcpy(&t_last_activity, &t[2], sizeof(struct timeval));
      state = RRC_STATE_RELEASE_REQUEST;
      return true; 
    }
  }
  return false;       
}

void rrc::ue::parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  set_activity();

  ul_dcch_msg_s ul_dcch_msg;
  asn1::bit_ref bref(pdu->msg, pdu->N_bytes);
  if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
    parent->rrc_log->error("Failed to unpack UL-DCCH message\n");
    return;
  }

  parent->log_rrc_message(rb_id_text[lcid], Rx, pdu.get(), ul_dcch_msg);

  // reuse PDU
  pdu->clear(); // FIXME: name collision with byte_buffer reset

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
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_complete:
      handle_security_mode_complete(&ul_dcch_msg.msg.c1().security_mode_complete());
      // Skipping send_ue_cap_enquiry() procedure for now
      // state = RRC_STATE_WAIT_FOR_UE_CAP_INFO;
      notify_s1ap_ue_ctxt_setup_complete();
      send_connection_reconf(std::move(pdu));
      state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      break;
    case ul_dcch_msg_type_c::c1_c_::types::security_mode_fail:
      handle_security_mode_failure(&ul_dcch_msg.msg.c1().security_mode_fail());
      break;
    case ul_dcch_msg_type_c::c1_c_::types::ue_cap_info:
      if (handle_ue_cap_info(&ul_dcch_msg.msg.c1().ue_cap_info())) {
        send_connection_reconf(std::move(pdu));
        state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      } else {
        send_connection_reject();
        state = RRC_STATE_IDLE;
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
  }

  set_activity();
  rrc_conn_request_r8_ies_s* msg_r8 = &msg->crit_exts.rrc_conn_request_r8();

  if (msg_r8->ue_id.type() == init_ue_id_c::types::s_tmsi) {
    mmec     = (uint8_t)msg_r8->ue_id.s_tmsi().mmec.to_number();
    m_tmsi   = (uint32_t)msg_r8->ue_id.s_tmsi().m_tmsi.to_number();
    has_tmsi = true;
  }
  establishment_cause = msg_r8->establishment_cause;
  send_connection_setup();
  state = RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE;
}

void rrc::ue::handle_rrc_con_reest_req(rrc_conn_reest_request_r8_ies_s* msg)
{
  //TODO: Check Short-MAC-I value 
  parent->rrc_log->error("Not Supported: ConnectionReestablishment. \n");
  
}

void rrc::ue::handle_rrc_con_setup_complete(rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  parent->rrc_log->info("RRCConnectionSetupComplete transaction ID: %d\n", msg->rrc_transaction_id);
  rrc_conn_setup_complete_r8_ies_s* msg_r8 = &msg->crit_exts.c1().rrc_conn_setup_complete_r8();

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  pdu->N_bytes = msg_r8->ded_info_nas.size();
  memcpy(pdu->msg, msg_r8->ded_info_nas.data(), pdu->N_bytes);

  // Acknowledge Dedicated Configuration
  parent->mac->phy_config_enabled(rnti, true);

  if(has_tmsi) {
    parent->s1ap->initial_ue(
        rnti, (LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM)establishment_cause.value, std::move(pdu), m_tmsi, mmec);
  } else {
    parent->s1ap->initial_ue(rnti, (LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM)establishment_cause.value, std::move(pdu));
  }
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::handle_rrc_reconf_complete(rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu)
{
  parent->rrc_log->info("RRCReconfigurationComplete transaction ID: %d\n", msg->rrc_transaction_id);

  // Acknowledge Dedicated Configuration
  parent->mac->phy_config_enabled(rnti, true);
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
      asn1::bit_ref bref(msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.data(),
                         msg_r8->ue_cap_rat_container_list[0].ue_cap_rat_container.size());
      if (eutra_capabilities.unpack(bref) != asn1::SRSASN_SUCCESS) {
        parent->rrc_log->error("Failed to unpack EUTRA capabilities message\n");
        return false;
      }
      parent->rrc_log->info("UE rnti: 0x%x category: %d\n", rnti, eutra_capabilities.ue_category);
    }
  }

  return true;

  // TODO: Add liblte_rrc support for unpacking UE cap info and repacking into
  //       inter-node UERadioAccessCapabilityInformation (36.331 v10.0.0 Section 10.2.2).
  //       This is then passed to S1AP for transfer to EPC.
  // parent->s1ap->ue_capabilities(rnti, &eutra_capabilities);
}

void rrc::ue::set_bitrates(LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT *rates)
{
  memcpy(&bitrates, rates, sizeof(LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT));
}

void rrc::ue::set_security_capabilities(LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT *caps)
{
  memcpy(&security_capabilities, caps, sizeof(LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT));
}

void rrc::ue::set_security_key(uint8_t* key, uint32_t length)
{
  memcpy(k_enb, key, length);
  parent->rrc_log->info_hex(k_enb, 32, "Key eNodeB (k_enb)");
  // Selects security algorithms (cipher_algo and integ_algo) based on capabilities and config preferences 
  select_security_algorithms();

  parent->rrc_log->info("Selected security algorithms EEA: EEA%d EIA: EIA%d\n", cipher_algo, integ_algo);
  
  // Generate K_rrc_enc and K_rrc_int
  srslte::security_generate_k_rrc(k_enb, cipher_algo, integ_algo, k_rrc_enc, k_rrc_int);

  // Generate K_up_enc and K_up_int
  security_generate_k_up( k_enb,
                          cipher_algo,
                          integ_algo,
                          k_up_enc,
                          k_up_int);

  parent->configure_security(rnti, RB_ID_SRB1,
                             k_rrc_enc, k_rrc_int,
                             k_up_enc,  k_up_int,
                             cipher_algo, integ_algo);

  parent->enable_integrity(rnti, RB_ID_SRB1);

  parent->rrc_log->info_hex(k_rrc_enc, 32, "RRC Encryption Key (k_rrc_enc)");
  parent->rrc_log->info_hex(k_rrc_int, 32, "RRC Integrity Key (k_rrc_int)");
  parent->rrc_log->info_hex(k_up_enc, 32, "UP Encryption Key (k_up_enc)");
}

bool rrc::ue::setup_erabs(LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT *e)
{
  for(uint32_t i=0; i<e->len; i++) {
    LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT *erab = &e->buffer[i];
    if(erab->ext) {
      parent->rrc_log->warning("Not handling LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT extensions\n");
    }
    if(erab->iE_Extensions_present) {
      parent->rrc_log->warning("Not handling LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT extensions\n");
    }
    if(erab->transportLayerAddress.n_bits > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    uint8_to_uint32(erab->gTP_TEID.buffer, &teid_out);
    LIBLTE_S1AP_NAS_PDU_STRUCT *nas_pdu = erab->nAS_PDU_present ? &erab->nAS_PDU : NULL;
    setup_erab(erab->e_RAB_ID.E_RAB_ID, &erab->e_RABlevelQoSParameters,
               &erab->transportLayerAddress, teid_out, nas_pdu);
  }
  return true;
}

bool rrc::ue::setup_erabs(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e)
{
  for(uint32_t i=0; i<e->len; i++) {
    LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT *erab = &e->buffer[i];
    if(erab->ext) {
      parent->rrc_log->warning("Not handling LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT extensions\n");
    }
    if(erab->iE_Extensions_present) {
      parent->rrc_log->warning("Not handling LIBLTE_S1AP_E_RABTOBESETUPITEMCTXTSUREQ_STRUCT extensions\n");
    }
    if(erab->transportLayerAddress.n_bits > 32) {
      parent->rrc_log->error("IPv6 addresses not currently supported\n");
      return false;
    }

    uint32_t teid_out;
    uint8_to_uint32(erab->gTP_TEID.buffer, &teid_out);
    setup_erab(erab->e_RAB_ID.E_RAB_ID, &erab->e_RABlevelQoSParameters,
               &erab->transportLayerAddress, teid_out, &erab->nAS_PDU);
  }

  // Work in progress
  notify_s1ap_ue_erab_setup_response(e);
  send_connection_reconf_new_bearer(e);
  return true;
}

void rrc::ue::setup_erab(uint8_t id, LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT *qos,
                         LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT *addr, uint32_t teid_out,
                         LIBLTE_S1AP_NAS_PDU_STRUCT *nas_pdu)
{
  erabs[id].id = id;
  memcpy(&erabs[id].qos_params, qos, sizeof(LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT));
  memcpy(&erabs[id].address, addr, sizeof(LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT));
  erabs[id].teid_out = teid_out;

  uint8_t* bit_ptr = addr->buffer;
  uint32_t addr_ = liblte_bits_2_value(&bit_ptr, addr->n_bits);
  uint8_t lcid  = id - 2;  // Map e.g. E-RAB 5 to LCID 3 (==DRB1)
  parent->gtpu->add_bearer(rnti, lcid, addr_, erabs[id].teid_out, &(erabs[id].teid_in));

  if(nas_pdu) {
    nas_pending = true;
    memcpy(erab_info.buffer, nas_pdu->buffer, nas_pdu->n_octets);
    erab_info.N_bytes = nas_pdu->n_octets;
    parent->rrc_log->info_hex(erab_info.buffer, erab_info.N_bytes, "setup_erab nas_pdu -> erab_info rnti 0x%x", rnti);
  } else {
    nas_pending = false;
  }
}

bool rrc::ue::release_erabs()
{
  typedef std::map<uint8_t, erab_t>::iterator it_t;
  for(it_t it=erabs.begin(); it!=erabs.end(); ++it) {
    // TODO: notify GTPU layer
  }
  erabs.clear();
  return true; 
}

void rrc::ue::notify_s1ap_ue_ctxt_setup_complete()
{
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT res;
  res.ext = false;
  res.E_RABFailedToSetupListCtxtSURes_present = false;
  res.CriticalityDiagnostics_present = false;

  res.E_RABSetupListCtxtSURes.len = 0;
  res.E_RABFailedToSetupListCtxtSURes.len = 0;

  typedef std::map<uint8_t, erab_t>::iterator it_t;
  for(it_t it=erabs.begin(); it!=erabs.end(); ++it) {
    uint32_t j = res.E_RABSetupListCtxtSURes.len++;
    res.E_RABSetupListCtxtSURes.buffer[j].ext = false;
    res.E_RABSetupListCtxtSURes.buffer[j].iE_Extensions_present = false;
    res.E_RABSetupListCtxtSURes.buffer[j].e_RAB_ID.ext = false;
    res.E_RABSetupListCtxtSURes.buffer[j].e_RAB_ID.E_RAB_ID = it->second.id;
    uint32_to_uint8(it->second.teid_in, res.E_RABSetupListCtxtSURes.buffer[j].gTP_TEID.buffer);
  }

  parent->s1ap->ue_ctxt_setup_complete(rnti, &res);
}

void rrc::ue::notify_s1ap_ue_erab_setup_response(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e)
{
  LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT res;
  res.ext=false;
  res.E_RABSetupListBearerSURes.len = 0;
  res.E_RABFailedToSetupListBearerSURes.len = 0;

  res.CriticalityDiagnostics_present = false;
  res.E_RABFailedToSetupListBearerSURes_present = false;

  for(uint32_t i=0; i<e->len; i++) {
    res.E_RABSetupListBearerSURes_present = true;
    LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT *erab = &e->buffer[i];
    uint8_t id = erab->e_RAB_ID.E_RAB_ID;
    uint32_t j = res.E_RABSetupListBearerSURes.len++;
    res.E_RABSetupListBearerSURes.buffer[j].ext = false;
    res.E_RABSetupListBearerSURes.buffer[j].iE_Extensions_present = false;
    res.E_RABSetupListBearerSURes.buffer[j].e_RAB_ID.ext = false;
    res.E_RABSetupListBearerSURes.buffer[j].e_RAB_ID.E_RAB_ID = id;
    uint32_to_uint8(erabs[id].teid_in, res.E_RABSetupListBearerSURes.buffer[j].gTP_TEID.buffer);
  }

  parent->s1ap->ue_erab_setup_complete(rnti, &res);
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

  rr_cfg_ded_s* rr_cfg = NULL;
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
  rr_cfg->mac_main_cfg_present = true;
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

  if (is_setup) {
    if (sr_allocate(parent->cfg.sr_cfg.period, &phy_cfg->sched_request_cfg.setup().sr_cfg_idx,
                    &phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx)) {
      parent->rrc_log->error("Allocating SR resources for rnti=%d\n", rnti);
      return; 
    }
  } else {
    phy_cfg->sched_request_cfg.setup().sr_cfg_idx       = (uint8_t)sr_I;
    phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx = (uint16_t)sr_N_pucch;
  }
  // Power control 
  phy_cfg->ul_pwr_ctrl_ded_present = true; 
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pusch = 0;
  phy_cfg->ul_pwr_ctrl_ded.delta_mcs_enabled    = ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
  phy_cfg->ul_pwr_ctrl_ded.accumulation_enabled = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pucch = 0, phy_cfg->ul_pwr_ctrl_ded.p_srs_offset = 3;

  // PDSCH
  phy_cfg->pdsch_cfg_ded_present = true;
  phy_cfg->pdsch_cfg_ded.p_a     = parent->cfg.pdsch_cfg;

  // PUCCH
  phy_cfg->pucch_cfg_ded_present = true;
  phy_cfg->pucch_cfg_ded.ack_nack_repeat.set(pucch_cfg_ded_s::ack_nack_repeat_c_::types::release);

  phy_cfg->cqi_report_cfg_present = true;
  if(parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_mode_aperiodic         = cqi_report_mode_aperiodic_e::rm30;
  } else {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = false;
    if (is_setup) {
      if (cqi_allocate(parent->cfg.cqi_cfg.period, &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
                       &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx)) {
        parent->rrc_log->error("Allocating CQI resources for rnti=%d\n", rnti);
        return; 
      }
    } else {
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx = (uint16_t)cqi_pucch;
      phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx   = (uint16_t)cqi_idx;
    }
  }
  phy_cfg->cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;

  // Add SRB1 to Scheduler 
  srsenb::sched_interface::ue_cfg_t sched_cfg; 
  bzero(&sched_cfg, sizeof(srsenb::sched_interface::ue_cfg_t));
  sched_cfg.maxharq_tx              = parent->cfg.mac_cnfg.ul_sch_cfg.max_harq_tx.to_number();
  sched_cfg.continuous_pusch = false;   
  sched_cfg.aperiodic_cqi_period = parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC?parent->cfg.cqi_cfg.period:0; 
  sched_cfg.ue_bearers[0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  sched_cfg.ue_bearers[1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  if (parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    sched_cfg.aperiodic_cqi_period                   = parent->cfg.cqi_cfg.mode == parent->cfg.cqi_cfg.period;
    sched_cfg.dl_cfg.cqi_report.aperiodic_configured = true;
  } else {
    sched_cfg.dl_cfg.cqi_report.pmi_idx             = cqi_idx;
    sched_cfg.dl_cfg.cqi_report.periodic_configured = true;
  }
  sched_cfg.pucch_cfg.I_sr              = sr_I;
  sched_cfg.pucch_cfg.n_pucch_sr        = sr_N_pucch;
  sched_cfg.pucch_cfg.sr_configured     = true;
  sched_cfg.pucch_cfg.n_pucch           = cqi_pucch;
  sched_cfg.pucch_cfg.delta_pucch_shift = parent->sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();
  sched_cfg.pucch_cfg.N_cs              = parent->sib2.rr_cfg_common.pucch_cfg_common.n_cs_an;
  sched_cfg.pucch_cfg.n_rb_2            = parent->sib2.rr_cfg_common.pucch_cfg_common.n_rb_cqi;
  sched_cfg.pucch_cfg.N_pucch_1         = parent->sib2.rr_cfg_common.pucch_cfg_common.n1_pucch_an;

  // Configure MAC 
  parent->mac->ue_cfg(rnti, &sched_cfg);
    
  // Configure SRB1 in RLC
  parent->rlc->add_bearer(rnti, 1, srslte::rlc_config_t::srb_config(1));

  // Configure SRB1 in PDCP
  srslte::pdcp_config_t pdcp_cnfg{.bearer_id    = 1,
                                  .rb_type      = srslte::PDCP_RB_IS_SRB,
                                  .tx_direction = srslte::SECURITY_DIRECTION_DOWNLINK,
                                  .rx_direction = srslte::SECURITY_DIRECTION_UPLINK,
                                  .sn_len       = srslte::PDCP_SN_LEN_5};
  parent->pdcp->add_bearer(rnti, 1, pdcp_cnfg);

  // Configure PHY layer
  parent->phy->set_config_dedicated(rnti, phy_cfg);
  parent->mac->set_dl_ant_info(rnti, &phy_cfg->ant_info);
  parent->mac->phy_config_enabled(rnti, false);

  rr_cfg->drb_to_add_mod_list_present      = false;
  rr_cfg->drb_to_release_list_present      = false;
  rr_cfg->rlf_timers_and_consts_r9.set_present(false);
  rr_cfg->sps_cfg_present                  = false;
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
  uint32_t qci     = erabs[erab_id].qos_params.qCI.QCI;

  if (qci >= MAX_NOF_QCI) {
    parent->rrc_log->error("Invalid QCI=%d for ERAB_id=%d, DRB_id=%d\n", qci, erab_id, drb_id);
    return -1; 
  }
  
  if (!parent->cfg.qci_cfg[qci].configured) {
    parent->rrc_log->error("QCI=%d not configured\n", qci);
    return -1; 
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

  return 0; 
}

void rrc::ue::send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu)
{
  dl_dcch_msg_s dl_dcch_msg;
  rrc_conn_recfg_s* rrc_conn_recfg   = &dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg();
  rrc_conn_recfg->rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg->crit_exts.set_c1().set_rrc_conn_recfg_r8();

  rrc_conn_recfg->crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded_present = true;
  rr_cfg_ded_s* rr_cfg = &rrc_conn_recfg->crit_exts.c1().rrc_conn_recfg_r8().rr_cfg_ded;

  rr_cfg->phys_cfg_ded_present       = true;
  phys_cfg_ded_s* phy_cfg            = &rr_cfg->phys_cfg_ded;
  phy_cfg->sched_request_cfg_present = true;
  phy_cfg->sched_request_cfg.set_setup();
  phy_cfg->sched_request_cfg.setup().dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  phy_cfg->cqi_report_cfg_present = true;
  if (cqi_allocated) {
    phy_cfg->cqi_report_cfg.cqi_report_periodic_present = true;
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    cqi_get(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
            &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx);
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
  parent->phy->set_config_dedicated(rnti, phy_cfg);

  sr_get(&phy_cfg->sched_request_cfg.setup().sr_cfg_idx, &phy_cfg->sched_request_cfg.setup().sr_pucch_res_idx);

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

  phy_cfg->ant_info_present = true;
  phy_cfg->ant_info.set(phys_cfg_ded_s::ant_info_c_::types::explicit_value);
  phy_cfg->ant_info.explicit_value() = parent->cfg.antenna_info;

  // Configure PHY layer
  phy_cfg->cqi_report_cfg_present = true;
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
    phy_cfg->cqi_report_cfg.cqi_report_periodic.set_setup();
    cqi_get(&phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx,
            &phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    phy_cfg->cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (phy_cfg->ant_info_present and
        ((phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm3) ||
         (phy_cfg->ant_info.explicit_value().tx_mode == ant_info_ded_s::tx_mode_e_::tm4))) {
      uint16_t ri_idx = 0;
      if (ri_get(parent->cfg.cqi_cfg.m_ri, &ri_idx) == SRSLTE_SUCCESS) {
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

  parent->phy->set_config_dedicated(rnti, phy_cfg);
  parent->mac->set_dl_ant_info(rnti, &phy_cfg->ant_info);
  parent->mac->phy_config_enabled(rnti, false);

  // Add SRB2 to the message
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list.resize(1);
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].srb_id            = 2;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].lc_ch_cfg_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].lc_ch_cfg.set(srb_to_add_mod_s::lc_ch_cfg_c_::types::default_value);
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].rlc_cfg_present = true;
  conn_reconf->rr_cfg_ded.srb_to_add_mod_list[0].rlc_cfg.set(srb_to_add_mod_s::rlc_cfg_c_::types::default_value);

  // Get DRB1 configuration
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list_present = true;
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list.resize(1);
  if (get_drbid_config(&conn_reconf->rr_cfg_ded.drb_to_add_mod_list[0], 1)) {
    parent->rrc_log->error("Getting DRB1 configuration\n");
    parent->rrc_log->console("The QCI %d for DRB1 is invalid or not configured.\n", erabs[5].qos_params.qCI.QCI);
    return;
  }
  
  // Add SRB2 and DRB1 to the scheduler
  srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
  bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  bearer_cfg.group = 0;
  parent->mac->bearer_ue_cfg(rnti, 2, &bearer_cfg);
  bearer_cfg.group = conn_reconf->rr_cfg_ded.drb_to_add_mod_list[0].lc_ch_cfg.ul_specific_params.lc_ch_group;
  parent->mac->bearer_ue_cfg(rnti, 3, &bearer_cfg);
  
  // Configure SRB2 in RLC and PDCP
  parent->rlc->add_bearer(rnti, 2, srslte::rlc_config_t::srb_config(2));

  // Configure SRB2 in PDCP
  srslte::pdcp_config_t pdcp_cnfg_srb = {.bearer_id    = 2,
                                         .rb_type      = srslte::PDCP_RB_IS_SRB,
                                         .tx_direction = srslte::SECURITY_DIRECTION_DOWNLINK,
                                         .rx_direction = srslte::SECURITY_DIRECTION_UPLINK,
                                         .sn_len       = srslte::PDCP_SN_LEN_5};
  parent->pdcp->add_bearer(rnti, 2, pdcp_cnfg_srb);
  parent->pdcp->config_security(rnti, 2, k_rrc_enc, k_rrc_int, k_up_enc, cipher_algo, integ_algo);
  parent->pdcp->enable_integrity(rnti, 2);
  parent->pdcp->enable_encryption(rnti, 2);

  // Configure DRB1 in RLC
  parent->rlc->add_bearer(rnti, 3, srslte::make_rlc_config_t(conn_reconf->rr_cfg_ded.drb_to_add_mod_list[0].rlc_cfg));

  // Configure DRB1 in PDCP
  srslte::pdcp_config_t pdcp_cnfg_drb = {.bearer_id    = 1,
                                         .rb_type      = srslte::PDCP_RB_IS_DRB,
                                         .tx_direction = srslte::SECURITY_DIRECTION_DOWNLINK,
                                         .rx_direction = srslte::SECURITY_DIRECTION_UPLINK,
                                         .sn_len       = srslte::PDCP_SN_LEN_12};
  if (conn_reconf->rr_cfg_ded.drb_to_add_mod_list[0].pdcp_cfg.rlc_um_present) {
    if (conn_reconf->rr_cfg_ded.drb_to_add_mod_list[0].pdcp_cfg.rlc_um.pdcp_sn_size.value ==
        pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_::len7bits) {
      pdcp_cnfg_drb.sn_len = srslte::PDCP_SN_LEN_7;
    }
  }
  parent->pdcp->add_bearer(rnti, 3, pdcp_cnfg_drb);
  parent->pdcp->config_security(rnti, 3, k_rrc_enc, k_rrc_int, k_up_enc, cipher_algo, integ_algo);
  parent->pdcp->enable_integrity(rnti, 3);
  parent->pdcp->enable_encryption(rnti, 3);
  // DRB1 has already been configured in GTPU through bearer setup

  // Add NAS Attach accept
  if(nas_pending){
    parent->rrc_log->info_hex(erab_info.buffer, erab_info.N_bytes, "connection_reconf erab_info -> nas_info rnti 0x%x\n", rnti);
    conn_reconf->ded_info_nas_list_present = true;
    conn_reconf->ded_info_nas_list.resize(1);
    conn_reconf->ded_info_nas_list[0].resize(erab_info.N_bytes);
    memcpy(conn_reconf->ded_info_nas_list[0].data(), erab_info.buffer, erab_info.N_bytes);
  } else {
    parent->rrc_log->debug("Not adding NAS message to connection reconfiguration\n");
    conn_reconf->ded_info_nas_list.resize(0);
  }

  // Reuse same PDU
  pdu->clear();

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));

  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reconf_new_bearer(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e)
{
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);

  dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
  dl_dcch_msg.msg.c1().rrc_conn_recfg().rrc_transaction_id = (uint8_t)((transaction_id++) % 4);
  rrc_conn_recfg_r8_ies_s* conn_reconf = &dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  for(uint32_t i=0; i<e->len; i++) {
    LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT *erab = &e->buffer[i];
    uint8_t id    = erab->e_RAB_ID.E_RAB_ID;
    uint8_t lcid  = id - 2; // Map e.g. E-RAB 5 to LCID 3 (==DRB1)

    // Get DRB configuration
    drb_to_add_mod_s drb_item;
    if (get_drbid_config(&drb_item, lcid - 2)) {
      parent->rrc_log->error("Getting DRB configuration\n");
      parent->rrc_log->console("ERROR: The QCI %d is invalid or not configured.\n", erabs[lcid + 4].qos_params.qCI.QCI);
      return;
    }

    // Add DRB to the scheduler
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
    bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    parent->mac->bearer_ue_cfg(rnti, lcid, &bearer_cfg);

    // Configure DRB in RLC
    parent->rlc->add_bearer(rnti, lcid, srslte::make_rlc_config_t(drb_item.rlc_cfg));

    // Configure DRB in PDCP
    srslte::pdcp_config_t pdcp_config = {
        .bearer_id    = (uint8_t)(drb_item.drb_id - 1), // TODO: Review all ID mapping LCID DRB ERAB EPSBID Mapping
        .rb_type      = srslte::PDCP_RB_IS_DRB,
        .tx_direction = srslte::SECURITY_DIRECTION_DOWNLINK,
        .rx_direction = srslte::SECURITY_DIRECTION_UPLINK,
        .sn_len       = srslte::PDCP_SN_LEN_12};
    parent->pdcp->add_bearer(rnti, lcid, pdcp_config);

    // DRB has already been configured in GTPU through bearer setup

    conn_reconf->rr_cfg_ded.drb_to_add_mod_list.push_back(drb_item);

    // Add NAS message
    parent->rrc_log->info_hex(erab_info.buffer, erab_info.N_bytes, "reconf_new_bearer erab_info -> nas_info rnti 0x%x\n", rnti);
    asn1::dyn_octstring octstr(erab_info.N_bytes);
    memcpy(octstr.data(), erab_info.msg, erab_info.N_bytes);
    conn_reconf->ded_info_nas_list.push_back(octstr);
  }
  conn_reconf->rr_cfg_ded.drb_to_add_mod_list_present = conn_reconf->rr_cfg_ded.drb_to_add_mod_list.size() > 0;
  conn_reconf->ded_info_nas_list_present              = conn_reconf->ded_info_nas_list.size() > 0;

  send_dl_dcch(&dl_dcch_msg, std::move(pdu));
}

void rrc::ue::send_security_mode_command()
{
  dl_dcch_msg_s dl_dcch_msg;
  security_mode_cmd_s* comm = &dl_dcch_msg.msg.set_c1().set_security_mode_cmd();
  comm->rrc_transaction_id  = (uint8_t)((transaction_id++) % 4);

  // TODO: select these based on UE capabilities and preference order
  comm->crit_exts.set_c1().set_security_mode_cmd_r8();
  comm->crit_exts.c1().security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg.ciphering_algorithm =
      (ciphering_algorithm_r12_e::options)cipher_algo;
  comm->crit_exts.c1().security_mode_cmd_r8().security_cfg_smc.security_algorithm_cfg.integrity_prot_algorithm =
      (security_algorithm_cfg_s::integrity_prot_algorithm_e_::options)integ_algo;

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

  bool enc_algo_found = false;
  bool integ_algo_found = false;

  for (int i = 0; i < srslte::CIPHERING_ALGORITHM_ID_N_ITEMS; i++) {
    switch (parent->cfg.eea_preference_list[i]) {
    case srslte::CIPHERING_ALGORITHM_ID_EEA0:
      // “all bits equal to 0” – UE supports no other algorithm than EEA0,
      // specification does not cover the case in which EEA0 is supported with other algorithms
      // just assume that EEA0 is always supported even this can not be explicity signaled by S1AP
      cipher_algo    = srslte::CIPHERING_ALGORITHM_ID_EEA0;
      enc_algo_found = true;
      parent->rrc_log->info("Selected EEA0 as RRC encryption algorithm\n");
      break;
    case srslte::CIPHERING_ALGORITHM_ID_128_EEA1:
      // “first bit” – 128-EEA1,
      if (security_capabilities.encryptionAlgorithms
              .buffer[srslte::CIPHERING_ALGORITHM_ID_128_EEA1 - 1]) {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
        enc_algo_found = true;
        parent->rrc_log->info("Selected EEA1 as RRC encryption algorithm\n");
        break;
      } else {
        parent->rrc_log->info("Failed to selected EEA1 as RRC encryption algorithm, due to unsupported algorithm\n");
      }
      break;
    case srslte::CIPHERING_ALGORITHM_ID_128_EEA2:
      // “second bit” – 128-EEA2,
      if (security_capabilities.encryptionAlgorithms
              .buffer[srslte::CIPHERING_ALGORITHM_ID_128_EEA2 - 1]) {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA2;
        enc_algo_found = true;
        parent->rrc_log->info("Selected EEA2 as RRC encryption algorithm\n");
        break;
      } else {
        parent->rrc_log->info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm\n");
      }
      break;
    case srslte::CIPHERING_ALGORITHM_ID_128_EEA3:
      // “third bit” – 128-EEA3,
      if (security_capabilities.encryptionAlgorithms.buffer[srslte::CIPHERING_ALGORITHM_ID_128_EEA3 - 1]) {
        cipher_algo    = srslte::CIPHERING_ALGORITHM_ID_128_EEA3;
        enc_algo_found = true;
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

  for (int i = 0; i < srslte::INTEGRITY_ALGORITHM_ID_N_ITEMS; i++) {
    switch (parent->cfg.eia_preference_list[i]) {
    case srslte::INTEGRITY_ALGORITHM_ID_EIA0:
      // Null integrity is not supported
      parent->rrc_log->info("Skipping EIA0 as RRC integrity algorithm. Null integrity is not supported.\n");
      break;
    case srslte::INTEGRITY_ALGORITHM_ID_128_EIA1:
      // “first bit” – 128-EIA1,
      if (security_capabilities.integrityProtectionAlgorithms.buffer[srslte::INTEGRITY_ALGORITHM_ID_128_EIA1 - 1]) {
        integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;
        integ_algo_found = true;
        parent->rrc_log->info("Selected EIA1 as RRC integrity algorithm.\n");
      } else {
        parent->rrc_log->info("Failed to selected EIA1 as RRC encryption algorithm, due to unsupported algorithm\n");
      }
      break;
    case srslte::INTEGRITY_ALGORITHM_ID_128_EIA2:
      // “second bit” – 128-EIA2,
      if (security_capabilities.integrityProtectionAlgorithms.buffer[srslte::INTEGRITY_ALGORITHM_ID_128_EIA2 - 1]) {
        integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA2;
        integ_algo_found = true;
        parent->rrc_log->info("Selected EIA2 as RRC integrity algorithm.\n");
      } else {
        parent->rrc_log->info("Failed to selected EIA2 as RRC encryption algorithm, due to unsupported algorithm\n");
      }
      break;
    case srslte::INTEGRITY_ALGORITHM_ID_128_EIA3:
      // “third bit” – 128-EIA3,
      if (security_capabilities.integrityProtectionAlgorithms.buffer[srslte::INTEGRITY_ALGORITHM_ID_128_EIA3 - 1]) {
        integ_algo       = srslte::INTEGRITY_ALGORITHM_ID_128_EIA3;
        integ_algo_found = true;
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
    dl_ccch_msg->pack(bref);
    pdu->N_bytes = 1u + (uint32_t)bref.distance_bytes(pdu->msg);

    char buf[32] = {};
    sprintf(buf, "SRB0 - rnti=0x%x", rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg);
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
    dl_dcch_msg->pack(bref);
    pdu->N_bytes = 1u + (uint32_t)bref.distance_bytes(pdu->msg);

    // send on SRB2 if user is fully registered (after RRC reconfig complete)
    uint32_t lcid =
        parent->rlc->has_bearer(rnti, RB_ID_SRB2) && state == RRC_STATE_REGISTERED ? RB_ID_SRB2 : RB_ID_SRB1;

    char buf[32] = {};
    sprintf(buf, "SRB%d - rnti=0x%x", lcid, rnti);
    parent->log_rrc_message(buf, Tx, pdu.get(), *dl_dcch_msg);

    parent->pdcp->write_sdu(rnti, lcid, std::move(pdu));
  } else {
    parent->rrc_log->error("Allocating pdu\n");
  }
}

int rrc::ue::sr_free()
{
  if (sr_allocated) {
    if (parent->sr_sched.nof_users[sr_sched_prb_idx][sr_sched_sf_idx] > 0) {
      parent->sr_sched.nof_users[sr_sched_prb_idx][sr_sched_sf_idx]--;
    } else {
      parent->rrc_log->warning("Removing SR resources: no users in time-frequency slot (%d, %d)\n", sr_sched_prb_idx, sr_sched_sf_idx);
    }
    parent->rrc_log->info("Deallocated SR resources for time-frequency slot (%d, %d)\n", sr_sched_prb_idx, sr_sched_sf_idx);
  }
  return 0; 
}

void rrc::ue::sr_get(uint8_t* I_sr, uint16_t* N_pucch_sr)
{
  *I_sr       = sr_I; 
  *N_pucch_sr = sr_N_pucch; 
}

int rrc::ue::sr_allocate(uint32_t period, uint8_t* I_sr, uint16_t* N_pucch_sr)
{
  uint32_t c = SRSLTE_CP_ISNORM(parent->cfg.cell.cp)?3:2;
  uint32_t delta_pucch_shift = parent->sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();

  uint32_t max_users = 12*c/delta_pucch_shift;

  // Find freq-time resources with least number of users 
  int i_min=0, j_min=0; 
  uint32_t min_users = 1e6;
  for (uint32_t i=0;i<parent->cfg.sr_cfg.nof_prb;i++) {
    for (uint32_t j=0;j<parent->cfg.sr_cfg.nof_subframes;j++) {
      if (parent->sr_sched.nof_users[i][j] < min_users) {
        i_min = i; 
        j_min = j; 
        min_users = parent->sr_sched.nof_users[i][j];
      }
    }
  }
  
  if (parent->sr_sched.nof_users[i_min][j_min] > max_users) {
    parent->rrc_log->error("Not enough PUCCH resources to allocate Scheduling Request\n");
    return -1; 
  }
  
  // Compute I_sr   
  if (period != 5 && period != 10 && period != 20 && period != 40 && period != 80) {
    parent->rrc_log->error("Invalid SchedulingRequest period %d ms\n", period);
    return -1; 
  }  
  if (parent->cfg.sr_cfg.sf_mapping[j_min] < period) {
    *I_sr = period - 5 + parent->cfg.sr_cfg.sf_mapping[j_min];
  } else {
    parent->rrc_log->error("Allocating SR: invalid sf_idx=%d for period=%d\n", parent->cfg.sr_cfg.sf_mapping[j_min], period);
    return -1; 
  }

  // Compute N_pucch_sr 
  *N_pucch_sr = i_min*max_users + parent->sr_sched.nof_users[i_min][j_min];
  if (parent->sib2.rr_cfg_common.pucch_cfg_common.n_cs_an) {
    *N_pucch_sr += parent->sib2.rr_cfg_common.pucch_cfg_common.n_cs_an;
  }
    
  // Allocate user 
  parent->sr_sched.nof_users[i_min][j_min]++; 
  sr_sched_prb_idx = i_min; 
  sr_sched_sf_idx  = j_min; 
  sr_allocated     = true; 
  sr_I             = *I_sr; 
  sr_N_pucch       = *N_pucch_sr; 
 
  parent->rrc_log->info("Allocated SR resources for time-frequency slot (%d, %d), N_pucch_sr=%d, I_sr=%d\n", 
                        sr_sched_prb_idx, sr_sched_sf_idx, *N_pucch_sr, *I_sr);

  return 0; 
}

int rrc::ue::cqi_free()
{
  if (cqi_allocated) {
    if (parent->cqi_sched.nof_users[cqi_sched_prb_idx][cqi_sched_sf_idx] > 0) {
      parent->cqi_sched.nof_users[cqi_sched_prb_idx][cqi_sched_sf_idx]--;
    } else {
      parent->rrc_log->warning("Removing CQI resources: no users in time-frequency slot (%d, %d)\n", cqi_sched_prb_idx, cqi_sched_sf_idx);
    }
    parent->rrc_log->info("Deallocated CQI resources for time-frequency slot (%d, %d)\n", cqi_sched_prb_idx, cqi_sched_sf_idx);
  }
  return 0; 
}

void rrc::ue::cqi_get(uint16_t* pmi_idx, uint16_t* n_pucch)
{
  *pmi_idx = cqi_idx; 
  *n_pucch = cqi_pucch; 
}

int rrc::ue::cqi_allocate(uint32_t period, uint16_t* pmi_idx, uint16_t* n_pucch)
{
  uint32_t c = SRSLTE_CP_ISNORM(parent->cfg.cell.cp)?3:2;
  uint32_t delta_pucch_shift = parent->sib2.rr_cfg_common.pucch_cfg_common.delta_pucch_shift.to_number();

  uint32_t max_users = 12*c/delta_pucch_shift;

  // Find freq-time resources with least number of users 
  int i_min=0, j_min=0; 
  uint32_t min_users = 1e6;
  for (uint32_t i=0;i<parent->cfg.cqi_cfg.nof_prb;i++) {
    for (uint32_t j=0;j<parent->cfg.cqi_cfg.nof_subframes;j++) {
      if (parent->cqi_sched.nof_users[i][j] < min_users) {
        i_min = i; 
        j_min = j; 
        min_users = parent->cqi_sched.nof_users[i][j];
      }
    }
  }
  
  if (parent->cqi_sched.nof_users[i_min][j_min] > max_users) {
    parent->rrc_log->error("Not enough PUCCH resources to allocate Scheduling Request\n");
    return -1; 
  }
  
  // Compute I_sr   
  if (period != 2 && period != 5 && period != 10 && period != 20 && period != 40 && period != 80 && 
      period != 160 && period != 32 && period != 64 && period != 128) {
    parent->rrc_log->error("Invalid CQI Report period %d ms\n", period);
    return -1; 
  }  
  if (parent->cfg.cqi_cfg.sf_mapping[j_min] < period) {
    if (period != 32 && period != 64 && period != 128) {
      if (period > 2) {
        *pmi_idx = period - 3 + parent->cfg.cqi_cfg.sf_mapping[j_min]; 
      } else {
        *pmi_idx = parent->cfg.cqi_cfg.sf_mapping[j_min]; 
      }      
    } else {
      if (period == 32) {
        *pmi_idx = 318 + parent->cfg.cqi_cfg.sf_mapping[j_min]; 
      } else if (period == 64) {
        *pmi_idx = 350 + parent->cfg.cqi_cfg.sf_mapping[j_min]; 
      } else if (period == 128) {
        *pmi_idx = 414 + parent->cfg.cqi_cfg.sf_mapping[j_min]; 
      }
    }
  } else {
    parent->rrc_log->error("Allocating SR: invalid sf_idx=%d for period=%d\n", parent->cfg.cqi_cfg.sf_mapping[j_min], period);
    return -1; 
  }

  // Compute n_pucch_2
  *n_pucch = i_min*max_users + parent->cqi_sched.nof_users[i_min][j_min];
  if (parent->sib2.rr_cfg_common.pucch_cfg_common.n_cs_an) {
    *n_pucch += parent->sib2.rr_cfg_common.pucch_cfg_common.n_cs_an;
  }
    
  // Allocate user 
  parent->cqi_sched.nof_users[i_min][j_min]++; 
  cqi_sched_prb_idx = i_min; 
  cqi_sched_sf_idx  = j_min; 
  cqi_allocated     = true;
  cqi_idx           = *pmi_idx;
  cqi_pucch         = *n_pucch;

  parent->rrc_log->info("Allocated CQI resources for time-frequency slot (%d, %d), n_pucch_2=%d, pmi_cfg_idx=%d\n",
                        cqi_sched_prb_idx,
                        cqi_sched_sf_idx,
                        *n_pucch,
                        *pmi_idx);

  return 0; 
}

int rrc::ue::ri_get(uint32_t m_ri, uint16_t* ri_idx)
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
}
