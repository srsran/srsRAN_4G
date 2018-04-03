/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/interfaces/sched_interface.h"
#include "srslte/asn1/liblte_rrc.h"
#include "srsenb/hdr/upper/rrc.h"
#include "srslte/srslte.h"
#include "srslte/asn1/liblte_mme.h"

using srslte::byte_buffer_t;
using srslte::bit_buffer_t;

namespace srsenb {
  
void rrc::init(rrc_cfg_t *cfg_,
               phy_interface_rrc* phy_, 
               mac_interface_rrc* mac_, 
               rlc_interface_rrc* rlc_, 
               pdcp_interface_rrc* pdcp_, 
               s1ap_interface_rrc *s1ap_,
               gtpu_interface_rrc* gtpu_,
               srslte::log* log_rrc)
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

  memcpy(&cfg, cfg_, sizeof(rrc_cfg_t));
  nof_si_messages = generate_sibs();  
  config_mac();
 
  pthread_mutex_init(&user_mutex, NULL);
  pthread_mutex_init(&paging_mutex, NULL);

  act_monitor.start(RRC_THREAD_PRIO);
  bzero(&sr_sched, sizeof(sr_sched_t));
  
  start(RRC_THREAD_PRIO);
}

rrc::activity_monitor::activity_monitor(rrc* parent_) 
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

void rrc::set_connect_notifer(connect_notifier *cnotifier) 
{
  this->cnotifier = cnotifier; 
}

void rrc::stop()
{
  if(running) {
    running = false;
    thread_cancel();
    wait_thread_finish();
  }
  act_monitor.stop();
  users.clear();
  pthread_mutex_destroy(&user_mutex);
  pthread_mutex_destroy(&paging_mutex);
}

void rrc::get_metrics(rrc_metrics_t &m)
{
  pthread_mutex_lock(&user_mutex);
  m.n_ues = 0;
  for(std::map<uint16_t, ue>::iterator iter=users.begin(); m.n_ues < ENB_METRICS_MAX_USERS &&iter!=users.end(); ++iter) {
    ue *u = (ue*) &iter->second;
    m.ues[m.n_ues++].state = u->get_state();
  }
  pthread_mutex_unlock(&user_mutex);
}

uint32_t rrc::generate_sibs()
{
  // nof_messages includes SIB2 by default, plus all configured SIBs
  uint32_t nof_messages = 1+cfg.sibs[0].sib.sib1.N_sched_info;
  LIBLTE_RRC_SCHEDULING_INFO_STRUCT *sched_info = cfg.sibs[0].sib.sib1.sched_info; 
  
  // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
  // all SIBs in a SI message msg[i] share the same periodicity
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT *msg = (LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT*)calloc(nof_messages, sizeof(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT));

  // Copy SIB1 to first SI message
  msg[0].N_sibs = 1;
  memcpy(&msg[0].sibs[0], &cfg.sibs[0], sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_STRUCT));

  // Copy rest of SIBs
  for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages; sched_info_elem++) {
    uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second
    uint32_t current_msg_element_offset = 0;

    msg[msg_index].N_sibs = 0;

    // SIB2 always in second SI message
    if (msg_index == 1) {
      msg[msg_index].N_sibs++;
      memcpy(&msg[msg_index].sibs[0], &cfg.sibs[1], sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_STRUCT));
      current_msg_element_offset = 1; // make sure "other SIBs" do not overwrite this SIB2
      // Save SIB2
      memcpy(&sib2, &cfg.sibs[1].sib.sib2, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT));
    } else {
      current_msg_element_offset = 0; // no SIB2, no offset
    }

    // Add other SIBs to this message, if any
    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].N_sib_mapping_info; mapping++) {
      msg[msg_index].N_sibs++;
      // current_msg_element_offset skips SIB2 if necessary
      memcpy(&msg[msg_index].sibs[mapping + current_msg_element_offset],
              &cfg.sibs[(int) sched_info[sched_info_elem].sib_mapping_info[mapping].sib_type+2],
              sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_STRUCT));
    }
  }

  // Pack payload for all messages 
  for (uint32_t msg_index = 0; msg_index < nof_messages; msg_index++) {
    LIBLTE_BIT_MSG_STRUCT bitbuffer;
    liblte_rrc_pack_bcch_dlsch_msg(&msg[msg_index], &bitbuffer);
    srslte_bit_pack_vector(bitbuffer.msg, sib_buffer[msg_index].msg, bitbuffer.N_bits);
    sib_buffer[msg_index].N_bytes = (bitbuffer.N_bits-1)/8+1;
  }

  free(msg);
  return nof_messages;
}

void rrc::config_mac()
{ 
  
  // Fill MAC scheduler configuration for SIBs 
  sched_interface::cell_cfg_t sched_cfg; 
  bzero(&sched_cfg, sizeof(sched_interface::cell_cfg_t));
  for (uint32_t i=0;i<nof_si_messages;i++) {
    sched_cfg.sibs[i].len = sib_buffer[i].N_bytes;
    if (i == 0) {
      sched_cfg.sibs[i].period_rf = 8; // SIB1 is always 8 rf
    } else {
      sched_cfg.sibs[i].period_rf = liblte_rrc_si_periodicity_num[cfg.sibs[0].sib.sib1.sched_info[i-1].si_periodicity];          
    }
  }
  sched_cfg.si_window_ms = liblte_rrc_si_window_length_num[cfg.sibs[0].sib.sib1.si_window_length];  
  sched_cfg.prach_rar_window = liblte_rrc_ra_response_window_size_num[cfg.sibs[1].sib.sib2.rr_config_common_sib.rach_cnfg.ra_resp_win_size];
  sched_cfg.maxharq_msg3tx = cfg.sibs[1].sib.sib2.rr_config_common_sib.rach_cnfg.max_harq_msg3_tx; 

  // Copy Cell configuration 
  memcpy(&sched_cfg.cell, &cfg.cell, sizeof(srslte_cell_t));
  
  // Configure MAC scheduler 
  mac->cell_cfg(&sched_cfg);
}


void rrc::read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t* payload)
{
  if (sib_index < LIBLTE_RRC_MAX_SIB) {
    memcpy(payload, sib_buffer[sib_index].msg, sib_buffer[sib_index].N_bytes);
  } 
}

void rrc::rl_failure(uint16_t rnti)
{ 
  rrc_log->info("Radio-Link failure detected rnti=0x%x\n", rnti);
  if (s1ap->user_exists(rnti)) {
    if (!s1ap->user_link_lost(rnti)) {
      rrc_log->info("Removing rnti=0x%x\n", rnti);
      rem_user_thread(rnti);
    }
  } else {
    rrc_log->warning("User rnti=0x%x context not existing in S1AP. Removing user\n", rnti);
    rem_user_thread(rnti);
  }
}

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
  pthread_mutex_unlock(&user_mutex);
}

void rrc::rem_user(uint16_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  if (users.count(rnti) == 1) {
    rrc_log->console("Disconnecting rnti=0x%x.\n", rnti);
    rrc_log->info("Disconnecting rnti=0x%x.\n", rnti);
    /* **Caution** order of removal here is important: from bottom to top */
    mac->ue_rem(rnti);  // MAC handles PHY

    pthread_mutex_unlock(&user_mutex);
    usleep(50000);
    pthread_mutex_lock(&user_mutex);

    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);
    gtpu->rem_user(rnti);
    users[rnti].sr_free();
    users[rnti].cqi_free();
    users.erase(rnti);
    rrc_log->info("Removed user rnti=0x%x\n", rnti);
  } else {
    rrc_log->error("Removing user rnti=0x%x (does not exist)\n", rnti);
  }
  pthread_mutex_unlock(&user_mutex);
}

// Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a 
// valid RNTI
void rrc::upd_user(uint16_t new_rnti, uint16_t old_rnti) 
{
  // Remove new_rnti
  rem_user_thread(new_rnti);
  
  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  if (users.count(old_rnti) == 1) {
    if (users[old_rnti].is_connected()) {
      users[old_rnti].send_connection_reconf_upd(pool_allocate);
    } else {
      users[old_rnti].send_connection_release();
    }
  }  
}

void rrc::set_activity_user(uint16_t rnti) 
{
  if (users.count(rnti) == 1) {
    users[rnti].set_activity();
  }
}

void rrc::rem_user_thread(uint16_t rnti)
{
  if (users.count(rnti) == 1) {
    rrc_pdu p = {rnti, LCID_REM_USER, NULL};
    rx_pdu_queue.push(p);
  }
}

uint32_t rrc::get_nof_users() {
  return users.size();
}

void rrc::max_retx_attempted(uint16_t rnti)
{

}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc::write_pdu(uint16_t rnti, uint32_t lcid, byte_buffer_t* pdu)
{
  rrc_pdu p = {rnti, lcid, pdu};
  rx_pdu_queue.push(p);
}

/*******************************************************************************
  S1AP interface
*******************************************************************************/
void rrc::write_dl_info(uint16_t rnti, byte_buffer_t* sdu)
{
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;
  bzero(&dl_dcch_msg, sizeof(LIBLTE_RRC_DL_DCCH_MSG_STRUCT));

  if (users.count(rnti) == 1) {
    dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER; 
    memcpy(dl_dcch_msg.msg.dl_info_transfer.dedicated_info.msg, sdu->msg, sdu->N_bytes);
    dl_dcch_msg.msg.dl_info_transfer.dedicated_info.N_bytes = sdu->N_bytes;
    
    sdu->reset();
    
    users[rnti].send_dl_dcch(&dl_dcch_msg, sdu);
        
  } else {
    rrc_log->error("Rx SDU for unknown rnti=0x%x\n", rnti);
  }
}

void rrc::release_complete(uint16_t rnti)
{
  rrc_log->info("Received Release Complete rnti=0x%x\n", rnti);
  if (users.count(rnti) == 1) {
    if (!users[rnti].is_idle()) {
      rlc->clear_buffer(rnti); 
      users[rnti].send_connection_release();
      // There is no RRCReleaseComplete message from UE thus wait ~100 subframes for tx
      usleep(100000);
    }
    rem_user(rnti);
  } else {
    rrc_log->error("Received ReleaseComplete for unknown rnti=0x%x\n", rnti);
  }
}

bool rrc::setup_ue_ctxt(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT *msg)
{
  rrc_log->info("Adding initial context for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  if(msg->CSFallbackIndicator_present) {
    rrc_log->warning("Not handling CSFallbackIndicator\n");
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

  // Send RRC security mode command
  users[rnti].send_security_mode_command();

  // Setup E-RABs
  users[rnti].setup_erabs(&msg->E_RABToBeSetupListCtxtSUReq);

  return true;
}

bool rrc::setup_ue_erabs(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT *msg)
{
  rrc_log->info("Setting up erab(s) for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  if(msg->uEaggregateMaximumBitrate_present) {
    // UEAggregateMaximumBitrate
    users[rnti].set_bitrates(&msg->uEaggregateMaximumBitrate);
  }

  // Setup E-RABs
  users[rnti].setup_erabs(&msg->E_RABToBeSetupListBearerSUReq);

  return true;
}

bool rrc::release_erabs(uint32_t rnti)
{
  rrc_log->info("Releasing E-RABs for 0x%x\n", rnti);

  if(users.count(rnti) == 0) {
    rrc_log->warning("Unrecognised rnti: 0x%x\n", rnti);
    return false;
  }

  return users[rnti].release_erabs();
}

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
  
  LIBLTE_RRC_PCCH_MSG_STRUCT pcch_msg;
  bzero(&pcch_msg, sizeof(LIBLTE_RRC_PCCH_MSG_STRUCT));
  
  // Default paging cycle, should get DRX from user
  uint32_t T  = liblte_rrc_default_paging_cycle_num[cfg.sibs[1].sib.sib2.rr_config_common_sib.pcch_cnfg.default_paging_cycle]; 
  uint32_t Nb = T*liblte_rrc_nb_num[cfg.sibs[1].sib.sib2.rr_config_common_sib.pcch_cnfg.nB]; 

  uint32_t N  = T<Nb?T:Nb;
  uint32_t Ns = Nb/T>1?Nb/T:1; 
  uint32_t sfn = tti/10;   
  
  std::vector<uint32_t> ue_to_remove;
  
  int n=0;
  for(std::map<uint32_t, LIBLTE_S1AP_UEPAGINGID_STRUCT>::iterator iter=pending_paging.begin(); n <  LIBLTE_RRC_MAX_PAGE_REC && iter!=pending_paging.end(); ++iter) {
    LIBLTE_S1AP_UEPAGINGID_STRUCT u = (LIBLTE_S1AP_UEPAGINGID_STRUCT) iter->second; 
    uint32_t ueid = ((uint32_t) iter->first)%1024; 
    uint32_t i_s = (ueid/N) % Ns; 
    
    if ((sfn % T) == (T/N) * (ueid % N)) {
            
      int sf_idx = sf_pattern[i_s%4][(Ns-1)%4];
      if (sf_idx < 0) {
        rrc_log->error("SF pattern is N/A for Ns=%d, i_s=%d, imsi_decimal=%d\n", Ns, i_s, ueid);
      } else if ((uint32_t) sf_idx == (tti%10)) {

        if (u.choice_type == LIBLTE_S1AP_UEPAGINGID_CHOICE_IMSI) {
          pcch_msg.paging_record_list[n].ue_identity.ue_identity_type = LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_IMSI; 
          memcpy(pcch_msg.paging_record_list[n].ue_identity.imsi, u.choice.iMSI.buffer, u.choice.iMSI.n_octets);
          pcch_msg.paging_record_list[n].ue_identity.imsi_size = u.choice.iMSI.n_octets;
          printf("Warning IMSI paging not tested\n");
        } else {
          pcch_msg.paging_record_list[n].ue_identity.ue_identity_type = LIBLTE_RRC_PAGING_UE_IDENTITY_TYPE_S_TMSI; 
          pcch_msg.paging_record_list[n].ue_identity.s_tmsi.mmec   = u.choice.s_TMSI.mMEC.buffer[0];
          uint32_t m_tmsi = 0; 
          for (int i=0;i<LIBLTE_S1AP_M_TMSI_OCTET_STRING_LEN;i++) {
            m_tmsi |= u.choice.s_TMSI.m_TMSI.buffer[i]<<(8*(LIBLTE_S1AP_M_TMSI_OCTET_STRING_LEN-i-1));
          }
          pcch_msg.paging_record_list[n].ue_identity.s_tmsi.m_tmsi = m_tmsi; 
        }
        pcch_msg.paging_record_list[n].cn_domain = LIBLTE_RRC_CN_DOMAIN_PS;        
        ue_to_remove.push_back(ueid);
        n++;
        rrc_log->info("Assembled paging for ue_id=%d, tti=%d\n", ueid, tti);
      }
    }
  }  
  
  for (uint32_t i=0;i<ue_to_remove.size();i++) {
    pending_paging.erase(ue_to_remove[i]);
  }

  pthread_mutex_unlock(&paging_mutex);
  
  if (n > 0) {
    pcch_msg.paging_record_list_size = n;
    liblte_rrc_pack_pcch_msg(&pcch_msg, (LIBLTE_BIT_MSG_STRUCT*)&bit_buf_paging);
    uint32_t N_bytes = (bit_buf_paging.N_bits-1)/8+1;

    if (payload_len) {
      *payload_len = N_bytes;
    }
    rrc_log->info("Assembling PCCH payload with %d UE identities, payload_len=%d bytes, nbits=%d\n", 
                  pcch_msg.paging_record_list_size, N_bytes, bit_buf_paging.N_bits);  
    return true; 
  } 
  
  return false;     
}


void rrc::read_pdu_pcch(uint8_t *payload, uint32_t buffer_size)
{
  uint32_t N_bytes = (bit_buf_paging.N_bits-1)/8+1;
  if (N_bytes <= buffer_size) {
    srslte_bit_pack_vector(bit_buf_paging.msg, payload, bit_buf_paging.N_bits);    
  }    
}

/*******************************************************************************
  Parsers
*******************************************************************************/

void rrc::parse_ul_ccch(uint16_t rnti, byte_buffer_t *pdu)
{
  uint16_t old_rnti = 0; 

  if (pdu) {
    LIBLTE_RRC_UL_CCCH_MSG_STRUCT ul_ccch_msg;
    bzero(&ul_ccch_msg, sizeof(LIBLTE_RRC_UL_CCCH_MSG_STRUCT));

    srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
    bit_buf.N_bits = pdu->N_bytes * 8;
    liblte_rrc_unpack_ul_ccch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &ul_ccch_msg);

    rrc_log->info_hex(pdu->msg, pdu->N_bytes,
                      "SRB0 - Rx: %s",
                      liblte_rrc_ul_ccch_msg_type_text[ul_ccch_msg.msg_type]);

    switch (ul_ccch_msg.msg_type) {
      case LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ:
        if (users.count(rnti)) {
          users[rnti].handle_rrc_con_req(&ul_ccch_msg.msg.rrc_con_req);
        } else {
          rrc_log->error("Received ConnectionSetup for rnti=0x%x without context\n", rnti);
        }
        break;
      case LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ:
        rrc_log->debug("rnti=0x%x, phyid=0x%x, smac=0x%x, cause=%s\n",
                       ul_ccch_msg.msg.rrc_con_reest_req.ue_id.c_rnti,
                       ul_ccch_msg.msg.rrc_con_reest_req.ue_id.phys_cell_id,
                       ul_ccch_msg.msg.rrc_con_reest_req.ue_id.short_mac_i,
                       liblte_rrc_con_reest_req_cause_text[ul_ccch_msg.msg.rrc_con_reest_req.cause]
        );
        if (users[rnti].is_idle()) {
          old_rnti = ul_ccch_msg.msg.rrc_con_reest_req.ue_id.c_rnti;
          if (users.count(old_rnti)) {
            rrc_log->error("Not supported: ConnectionReestablishment for rnti=0x%x. Sending Connection Reject\n", old_rnti);
            users[rnti].send_connection_reest_rej();
            rem_user_thread(old_rnti);
          } else {
            rrc_log->error("Received ConnectionReestablishment for rnti=0x%x without context\n", old_rnti);
            users[rnti].send_connection_reest_rej();
          }
          // remove temporal rnti
          rem_user_thread(rnti);
        } else {
          rrc_log->error("Received ReestablishmentRequest from an rnti=0x%x not in IDLE\n", rnti);
        }
        break;
      default:
        rrc_log->error("UL CCCH message not recognised\n");
        break;
    }

    pool->deallocate(pdu);
  }
}

void rrc::parse_ul_dcch(uint16_t rnti, uint32_t lcid, byte_buffer_t *pdu)
{
  if (pdu) {
    if (users.count(rnti)) {
      users[rnti].parse_ul_dcch(lcid, pdu);
    } else {
      rrc_log->error("Processing %s: Unkown rnti=0x%x\n", rb_id_text[lcid], rnti);
    }
  }
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
    pthread_mutex_lock(&user_mutex);
    if (users.count(p.rnti) == 1) {
      switch(p.lcid)
      {
        case RB_ID_SRB0:
          parse_ul_ccch(p.rnti, p.pdu);
          break;
        case RB_ID_SRB1:
        case RB_ID_SRB2:
          parse_ul_dcch(p.rnti, p.lcid, p.pdu);
          break;
        case LCID_REM_USER:
          pthread_mutex_unlock(&user_mutex);
          usleep(10000);
          rem_user(p.rnti);
          pthread_mutex_lock(&user_mutex);
          break;
        default:
          rrc_log->error("Rx PDU with invalid bearer id: %d", p.lcid);
          break;
      }
    } else {
      printf("Discarting rnti=0x%xn", p.rnti);
      rrc_log->warning("Discarting PDU for removed rnti=0x%x\n", p.rnti);
    }
    pthread_mutex_unlock(&user_mutex);
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
    pthread_mutex_unlock(&parent->user_mutex);
    if (rem_rnti) {
      if (parent->s1ap->user_exists(rem_rnti)) {
        parent->s1ap->user_inactivity(rem_rnti);
      } else {
        parent->rem_user(rem_rnti);          
      }
    }
  }
}

/*******************************************************************************
  RRC::UE Helpers
*******************************************************************************/

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
  pdcp->config_security(rnti, lcid, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
}
  
  
  
  
/*******************************************************************************
  UE class
*******************************************************************************/
rrc::ue::ue()
{
  parent           = NULL; 
  set_activity();
  has_tmsi         = false;
  connect_notified = false; 
  transaction_id   = 0;
  sr_allocated     = false;
  sr_sched_sf_idx  = 0;
  sr_sched_prb_idx = 0;
  sr_N_pucch       = 0;
  sr_I             = 0;
  cqi_allocated    = false;
  cqi_pucch        = 0;
  cqi_idx          = 0;
  cqi_sched_sf_idx = 0;
  cqi_sched_prb_idx = 0;
  state            = RRC_STATE_IDLE;
}

rrc_state_t rrc::ue::get_state()
{
  return state;
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
      deadline_us  = (parent->sib2.rr_config_common_sib.rach_cnfg.max_harq_msg3_tx + 1)* 8 * 1000;
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

void rrc::ue::parse_ul_dcch(uint32_t lcid, byte_buffer_t *pdu)
{
  
  set_activity();

  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;
  bzero(&ul_dcch_msg, sizeof(LIBLTE_RRC_UL_DCCH_MSG_STRUCT));
  
  srslte_bit_unpack_vector(pdu->msg, parent->bit_buf.msg, pdu->N_bytes*8);
  parent->bit_buf.N_bits = pdu->N_bytes*8;
  liblte_rrc_unpack_ul_dcch_msg((LIBLTE_BIT_MSG_STRUCT*)&parent->bit_buf, &ul_dcch_msg);

  parent->rrc_log->info_hex(pdu->msg, pdu->N_bytes, 
                      "%s - Rx %s\n", 
                      rb_id_text[lcid], liblte_rrc_ul_dcch_msg_type_text[ul_dcch_msg.msg_type]);
  transaction_id = 0;
  pdu->reset();
  
  switch(ul_dcch_msg.msg_type) {
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE:
      handle_rrc_con_setup_complete(&ul_dcch_msg.msg.rrc_con_setup_complete, pdu);
      break;      
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER:
      memcpy(pdu->msg, ul_dcch_msg.msg.ul_info_transfer.dedicated_info.msg, ul_dcch_msg.msg.ul_info_transfer.dedicated_info.N_bytes);
      pdu->N_bytes = ul_dcch_msg.msg.ul_info_transfer.dedicated_info.N_bytes;
      parent->s1ap->write_pdu(rnti, pdu);
      break;
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE:
      handle_rrc_reconf_complete(&ul_dcch_msg.msg.rrc_con_reconfig_complete, pdu);
      parent->rrc_log->console("User 0x%x connected\n", rnti);
      state = RRC_STATE_REGISTERED; 
      break;
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE:
      handle_security_mode_complete(&ul_dcch_msg.msg.security_mode_complete);
      // Skipping send_ue_cap_enquiry() procedure for now
      // state = RRC_STATE_WAIT_FOR_UE_CAP_INFO;
      notify_s1ap_ue_ctxt_setup_complete();
      send_connection_reconf(pdu);
      state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      break;
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_FAILURE:
      handle_security_mode_failure(&ul_dcch_msg.msg.security_mode_failure);
      break;
    case LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO:
      handle_ue_cap_info(&ul_dcch_msg.msg.ue_capability_info);
      send_connection_reconf(pdu);
      state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
      break;
    default:
      parent->rrc_log->error("Msg: %s not supported\n", liblte_rrc_ul_dcch_msg_type_text[ul_dcch_msg.msg_type]); 
      break;
  }
}

void rrc::ue::handle_rrc_con_req(LIBLTE_RRC_CONNECTION_REQUEST_STRUCT *msg)
{
  set_activity();
  
  if(msg->ue_id_type == LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI) {
    mmec      = msg->ue_id.s_tmsi.mmec;
    m_tmsi    = msg->ue_id.s_tmsi.m_tmsi;
    has_tmsi = true;
  }
  send_connection_setup();
  state = RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE;
}

void rrc::ue::handle_rrc_con_reest_req(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_REQUEST_STRUCT *msg)
{
  //TODO: Check Short-MAC-I value 
  parent->rrc_log->error("Not Supported: ConnectionReestablishment. \n");
  
}

void rrc::ue::handle_rrc_con_setup_complete(LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT *msg, srslte::byte_buffer_t *pdu)
{
  parent->rrc_log->info("RRCConnectionSetupComplete transaction ID: %d\n", msg->rrc_transaction_id);

  // TODO: msg->selected_plmn_id - used to select PLMN from SIB1 list
  // TODO: if(msg->registered_mme_present) - the indicated MME should be used from a pool

  memcpy(pdu->msg, msg->dedicated_info_nas.msg, msg->dedicated_info_nas.N_bytes);
  pdu->N_bytes = msg->dedicated_info_nas.N_bytes;

  // Acknowledge Dedicated Configuration
  parent->phy->set_conf_dedicated_ack(rnti, true);
  parent->mac->phy_config_enabled(rnti, true);

  if(has_tmsi) {
    parent->s1ap->initial_ue(rnti, pdu, m_tmsi, mmec);
  } else {
    parent->s1ap->initial_ue(rnti, pdu);
  }
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::handle_rrc_reconf_complete(LIBLTE_RRC_CONNECTION_RECONFIGURATION_COMPLETE_STRUCT *msg, srslte::byte_buffer_t *pdu)
{
  parent->rrc_log->info("RRCReconfigurationComplete transaction ID: %d\n", msg->rrc_transaction_id);


  // Acknowledge Dedicated Configuration
  parent->phy->set_conf_dedicated_ack(rnti, true);
  parent->mac->phy_config_enabled(rnti, true);
}

void rrc::ue::handle_security_mode_complete(LIBLTE_RRC_SECURITY_MODE_COMPLETE_STRUCT *msg)
{
  parent->rrc_log->info("SecurityModeComplete transaction ID: %d\n", msg->rrc_transaction_id);
}

void rrc::ue::handle_security_mode_failure(LIBLTE_RRC_SECURITY_MODE_FAILURE_STRUCT *msg)
{
  parent->rrc_log->info("SecurityModeFailure transaction ID: %d\n", msg->rrc_transaction_id);
}

void rrc::ue::handle_ue_cap_info(LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *msg)
{
  parent->rrc_log->info("UECapabilityInformation transaction ID: %d\n", msg->rrc_transaction_id);
  for(uint32_t i=0; i<msg->N_ue_caps; i++) {
    if(msg->ue_capability_rat[i].rat_type != LIBLTE_RRC_RAT_TYPE_EUTRA) {
      parent->rrc_log->warning("Not handling UE capability information for RAT type %s\n",
                               liblte_rrc_rat_type_text[msg->ue_capability_rat[i].rat_type]);
    } else {
      memcpy(&eutra_capabilities, &msg->ue_capability_rat[0], sizeof(LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT));
      parent->rrc_log->info("UE rnti: 0x%x category: %d\n", rnti, msg->ue_capability_rat[0].eutra_capability.ue_category);
    }
  }

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

  // Select algos (TODO: use security capabilities and config preferences)
  cipher_algo = srslte::CIPHERING_ALGORITHM_ID_EEA0;
  integ_algo  = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc( k_enb,
                           cipher_algo,
                           integ_algo,
                           k_rrc_enc,
                           k_rrc_int);

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
    memcpy(parent->erab_info.msg, nas_pdu->buffer, nas_pdu->n_octets);
    parent->erab_info.N_bytes = nas_pdu->n_octets;
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
  res.E_RABSetupListBearerSURes.len = 0;
  res.E_RABFailedToSetupListBearerSURes.len = 0;

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
  LIBLTE_RRC_DL_CCCH_MSG_STRUCT dl_ccch_msg; 
  bzero(&dl_ccch_msg, sizeof(LIBLTE_RRC_DL_CCCH_MSG_STRUCT));
  
  dl_ccch_msg.msg_type = LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ;
  
  send_dl_ccch(&dl_ccch_msg);
  
}

void rrc::ue::send_connection_setup(bool is_setup)
{
  LIBLTE_RRC_DL_CCCH_MSG_STRUCT dl_ccch_msg; 
  bzero(&dl_ccch_msg, sizeof(LIBLTE_RRC_DL_CCCH_MSG_STRUCT));
  
  LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT* rr_cfg = NULL;
  if (is_setup) {
    dl_ccch_msg.msg_type = LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP;
    dl_ccch_msg.msg.rrc_con_setup.rrc_transaction_id = (transaction_id++)%4; 
    rr_cfg = &dl_ccch_msg.msg.rrc_con_setup.rr_cnfg; 
  } else {
    dl_ccch_msg.msg_type = LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST;
    dl_ccch_msg.msg.rrc_con_reest.rrc_transaction_id = (transaction_id++)%4; 
    rr_cfg = &dl_ccch_msg.msg.rrc_con_reest.rr_cnfg; 
  }

  
  // Add SRB1 to cfg 
  rr_cfg->srb_to_add_mod_list_size = 1; 
  rr_cfg->srb_to_add_mod_list[0].srb_id = 1; 
  rr_cfg->srb_to_add_mod_list[0].lc_cnfg_present = true;
  rr_cfg->srb_to_add_mod_list[0].lc_default_cnfg_present  = true; 
  rr_cfg->srb_to_add_mod_list[0].rlc_cnfg_present = true;
  rr_cfg->srb_to_add_mod_list[0].rlc_default_cnfg_present = true;

  // mac-MainConfig
  rr_cfg->mac_main_cnfg_present = true; 
  LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *mac_cfg = &rr_cfg->mac_main_cnfg.explicit_value; 
  mac_cfg->ulsch_cnfg_present = true; 
  memcpy(&mac_cfg->ulsch_cnfg, &parent->cfg.mac_cnfg.ulsch_cnfg, sizeof(LIBLTE_RRC_ULSCH_CONFIG_STRUCT));
  mac_cfg->drx_cnfg_present = false; 
  mac_cfg->phr_cnfg_present = true;
  memcpy(&mac_cfg->phr_cnfg, &parent->cfg.mac_cnfg.phr_cnfg, sizeof(LIBLTE_RRC_PHR_CONFIG_STRUCT));  
  mac_cfg->time_alignment_timer = parent->cfg.mac_cnfg.time_alignment_timer;
  
  // physicalConfigDedicated
  rr_cfg->phy_cnfg_ded_present = true;
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cfg = &rr_cfg->phy_cnfg_ded; 
  bzero(phy_cfg, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
  phy_cfg->pusch_cnfg_ded_present = true; 
  memcpy(&phy_cfg->pusch_cnfg_ded, &parent->cfg.pusch_cfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT));
  phy_cfg->sched_request_cnfg_present = true;
  phy_cfg->sched_request_cnfg.setup_present = true; 
  phy_cfg->sched_request_cnfg.dsr_trans_max = parent->cfg.sr_cfg.dsr_max;

  phy_cfg->antenna_info_default_value = true;
  phy_cfg->antenna_info_present = false;

  if (is_setup) {
    if (sr_allocate(parent->cfg.sr_cfg.period, &phy_cfg->sched_request_cnfg.sr_cnfg_idx, &phy_cfg->sched_request_cnfg.sr_pucch_resource_idx)) {
      parent->rrc_log->error("Allocating SR resources for rnti=%d\n", rnti);
      return; 
    }
  } else {
    phy_cfg->sched_request_cnfg.sr_cnfg_idx           = sr_I;
    phy_cfg->sched_request_cnfg.sr_pucch_resource_idx = sr_N_pucch;
  }
  // Power control 
  phy_cfg->ul_pwr_ctrl_ded_present = true; 
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pusch = 0;
  phy_cfg->ul_pwr_ctrl_ded.delta_mcs_en = LIBLTE_RRC_DELTA_MCS_ENABLED_EN0;
  phy_cfg->ul_pwr_ctrl_ded.accumulation_en = true;
  phy_cfg->ul_pwr_ctrl_ded.p0_ue_pucch = 0, 
  phy_cfg->ul_pwr_ctrl_ded.p_srs_offset = 3; 

  // PDSCH
  phy_cfg->pdsch_cnfg_ded_present = true;
  phy_cfg->pdsch_cnfg_ded = parent->cfg.pdsch_cfg;

  // PUCCH
  phy_cfg->pucch_cnfg_ded_present = true;
  phy_cfg->pucch_cnfg_ded.ack_nack_repetition_n1_pucch_an = 0;

  phy_cfg->cqi_report_cnfg_present = true; 
  if(parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cnfg.report_mode_aperiodic_present = true; 
    phy_cfg->cqi_report_cnfg.report_mode_aperiodic = LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM30;
  } else {
    phy_cfg->cqi_report_cnfg.report_periodic_present = true; 
    phy_cfg->cqi_report_cnfg.report_periodic_setup_present = true; 
    phy_cfg->cqi_report_cnfg.report_periodic.format_ind_periodic = LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND_CQI; 
    phy_cfg->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi = false;
    if (is_setup) {
      if (cqi_allocate(parent->cfg.cqi_cfg.period, 
                       &phy_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx, 
                       &phy_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx)) 
      {
        parent->rrc_log->error("Allocating CQI resources for rnti=%d\n", rnti);
        return; 
      }
    } else {
      phy_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx = cqi_pucch; 
      phy_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx       = cqi_idx;
    }
  }
  phy_cfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset = 0; 
  
  
  // Add SRB1 to Scheduler 
  srsenb::sched_interface::ue_cfg_t sched_cfg; 
  bzero(&sched_cfg, sizeof(srsenb::sched_interface::ue_cfg_t));
  sched_cfg.maxharq_tx = liblte_rrc_max_harq_tx_num[parent->cfg.mac_cnfg.ulsch_cnfg.max_harq_tx]; 
  sched_cfg.continuous_pusch = false;   
  sched_cfg.aperiodic_cqi_period = parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC?parent->cfg.cqi_cfg.period:0; 
  sched_cfg.ue_bearers[0].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 
  sched_cfg.ue_bearers[1].direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH; 
  sched_cfg.sr_I       = sr_I; 
  sched_cfg.sr_N_pucch = sr_N_pucch; 
  sched_cfg.sr_enabled = true;
  sched_cfg.cqi_pucch  = cqi_pucch; 
  sched_cfg.cqi_idx    = cqi_idx; 
  sched_cfg.cqi_enabled = parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_PERIODIC;
  sched_cfg.pucch_cfg.delta_pucch_shift  = liblte_rrc_delta_pucch_shift_num[parent->sib2.rr_config_common_sib.pucch_cnfg.delta_pucch_shift%LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS];
  sched_cfg.pucch_cfg.N_cs               = parent->sib2.rr_config_common_sib.pucch_cnfg.n_cs_an;
  sched_cfg.pucch_cfg.n_rb_2             = parent->sib2.rr_config_common_sib.pucch_cnfg.n_rb_cqi;
  sched_cfg.pucch_cfg.n1_pucch_an        = parent->sib2.rr_config_common_sib.pucch_cnfg.n1_pucch_an;

  // Configure MAC 
  parent->mac->ue_cfg(rnti, &sched_cfg);
    
  // Configure SRB1 in RLC
  parent->rlc->add_bearer(rnti, 1);

  // Configure SRB1 in PDCP
  srslte::srslte_pdcp_config_t pdcp_cnfg;
  pdcp_cnfg.is_control = true;
  pdcp_cnfg.direction = SECURITY_DIRECTION_DOWNLINK;
  parent->pdcp->add_bearer(rnti, 1, pdcp_cnfg);

  // Configure PHY layer
  parent->phy->set_config_dedicated(rnti, phy_cfg);
  parent->phy->set_conf_dedicated_ack(rnti, false);
  parent->mac->set_dl_ant_info(rnti, &phy_cfg->antenna_info_explicit_value);
  parent->mac->phy_config_enabled(rnti, false);
  
  rr_cfg->drb_to_add_mod_list_size = 0; 
  rr_cfg->drb_to_release_list_size = 0; 
  rr_cfg->rlf_timers_and_constants_present = false; 
  rr_cfg->sps_cnfg_present = false; 
  
  send_dl_ccch(&dl_ccch_msg);
}


void rrc::ue::send_connection_reest()
{
  send_connection_setup(false);
}


void rrc::ue::send_connection_release()
{
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg; 
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE; 
  dl_dcch_msg.msg.rrc_con_release.rrc_transaction_id = (transaction_id++)%4; 
  dl_dcch_msg.msg.rrc_con_release.release_cause = LIBLTE_RRC_RELEASE_CAUSE_OTHER; 
 
  send_dl_dcch(&dl_dcch_msg);
}

int rrc::ue::get_drbid_config(LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT *drb, int drb_id)
{
  uint32_t lc_id    = drb_id + 2; 
  uint32_t erab_id  = lc_id + 2; 
  uint32_t qci = erabs[erab_id].qos_params.qCI.QCI;
  
  if (qci >= MAX_NOF_QCI) {
    parent->rrc_log->error("Invalid QCI=%d for ERAB_id=%d, DRB_id=%d\n", qci, erab_id, drb_id);
    return -1; 
  }
  
  if (!parent->cfg.qci_cfg[qci].configured) {
    parent->rrc_log->error("QCI=%d not configured\n", qci);
    return -1; 
  }
  
  // Add DRB1 to the message 
  drb->drb_id = drb_id; 
  drb->lc_id = lc_id; 
  drb->lc_id_present = true; 
  drb->eps_bearer_id = erab_id; 
  drb->eps_bearer_id_present = true; 
  
  drb->lc_cnfg_present = true; 
  drb->lc_cnfg.ul_specific_params_present = true; 
  drb->lc_cnfg.log_chan_sr_mask_present = false; 
  drb->lc_cnfg.ul_specific_params.log_chan_group_present = true; 
  memcpy(&drb->lc_cnfg.ul_specific_params, &parent->cfg.qci_cfg[qci].lc_cfg, sizeof(LIBLTE_RRC_UL_SPECIFIC_PARAMETERS_STRUCT));
  
  drb->pdcp_cnfg_present = true; 
  memcpy(&drb->pdcp_cnfg, &parent->cfg.qci_cfg[qci].pdcp_cfg, sizeof(LIBLTE_RRC_PDCP_CONFIG_STRUCT));

  drb->rlc_cnfg_present = true; 
  memcpy(&drb->rlc_cnfg, &parent->cfg.qci_cfg[qci].rlc_cfg, sizeof(LIBLTE_RRC_RLC_CONFIG_STRUCT));
  
  return 0; 
}

void rrc::ue::send_connection_reconf_upd(srslte::byte_buffer_t *pdu)
{
  
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg; 
  bzero(&dl_dcch_msg, sizeof(LIBLTE_RRC_DL_DCCH_MSG_STRUCT));
  
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG; 
  dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id = (transaction_id++)%4; 
 
  LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT* rr_cfg = &dl_dcch_msg.msg.rrc_con_reconfig.rr_cnfg_ded;
  
  dl_dcch_msg.msg.rrc_con_reconfig.rr_cnfg_ded_present = true; 
  
  rr_cfg->phy_cnfg_ded_present = true; 
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cfg = &rr_cfg->phy_cnfg_ded; 
  bzero(phy_cfg, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
  phy_cfg->sched_request_cnfg_present = true;
  phy_cfg->sched_request_cnfg.setup_present = true; 
  phy_cfg->sched_request_cnfg.dsr_trans_max = parent->cfg.sr_cfg.dsr_max; 

  phy_cfg->cqi_report_cnfg_present = true;
  if (cqi_allocated) {
    cqi_get(&phy_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx,
            &phy_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx);
    phy_cfg->cqi_report_cnfg.report_periodic_present = true;
    phy_cfg->cqi_report_cnfg.report_periodic_setup_present = true;
    phy_cfg->cqi_report_cnfg.report_periodic.format_ind_periodic =
        LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND_CQI;
    phy_cfg->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (parent->cfg.antenna_info.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3 ||
        parent->cfg.antenna_info.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present = true;
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx = 483; /* TODO: HARDCODED! Add to UL scheduler */
    } else {
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present = false;
    }
  } else {
    phy_cfg->cqi_report_cnfg.report_mode_aperiodic_present = true;
    if (phy_cfg->antenna_info_present &&
        parent->cfg.antenna_info.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
      phy_cfg->cqi_report_cnfg.report_mode_aperiodic = LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM31;
    } else {
      phy_cfg->cqi_report_cnfg.report_mode_aperiodic = LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM30;
    }
  }
  parent->phy->set_config_dedicated(rnti, phy_cfg);

  sr_get(&phy_cfg->sched_request_cnfg.sr_cnfg_idx, &phy_cfg->sched_request_cnfg.sr_pucch_resource_idx);
  
  pdu->reset();
  
  send_dl_dcch(&dl_dcch_msg, pdu);
  
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;

}

void rrc::ue::send_connection_reconf(srslte::byte_buffer_t *pdu)
{
  
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg; 
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG; 
  dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id = (transaction_id++)%4; 

  LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT* conn_reconf = &dl_dcch_msg.msg.rrc_con_reconfig;
  conn_reconf->rr_cnfg_ded_present   = true; 
  conn_reconf->rr_cnfg_ded.mac_main_cnfg_present = false; 
  conn_reconf->rr_cnfg_ded.phy_cnfg_ded_present  = false; 
  conn_reconf->rr_cnfg_ded.rlf_timers_and_constants_present = false; 
  conn_reconf->rr_cnfg_ded.sps_cnfg_present = false; 
  conn_reconf->rr_cnfg_ded.drb_to_release_list_size = 0; 
  conn_reconf->meas_cnfg_present     = false; 
  conn_reconf->mob_ctrl_info_present = false; 
  conn_reconf->sec_cnfg_ho_present   = false; 
  
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cfg = &conn_reconf->rr_cnfg_ded.phy_cnfg_ded;
  bzero(phy_cfg, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
  conn_reconf->rr_cnfg_ded.phy_cnfg_ded_present  = true;

  if (parent->cfg.antenna_info.tx_mode > LIBLTE_RRC_TRANSMISSION_MODE_1) {
    memcpy(&phy_cfg->antenna_info_explicit_value, &parent->cfg.antenna_info,
           sizeof(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT));
    phy_cfg->antenna_info_present = true;
    phy_cfg->antenna_info_default_value = false;
  }

  // Configure PHY layer
  phy_cfg->cqi_report_cnfg_present = true;
  if(parent->cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    phy_cfg->cqi_report_cnfg.report_mode_aperiodic_present = true;
    if (phy_cfg->antenna_info_present &&
        phy_cfg->antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4) {
      phy_cfg->cqi_report_cnfg.report_mode_aperiodic = LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM31;
    } else {
      phy_cfg->cqi_report_cnfg.report_mode_aperiodic = LIBLTE_RRC_CQI_REPORT_MODE_APERIODIC_RM30;
    }
  } else {
    cqi_get(&phy_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx,
            &phy_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx);
    phy_cfg->cqi_report_cnfg.report_periodic_present = true;
    phy_cfg->cqi_report_cnfg.report_periodic_setup_present = true;
    phy_cfg->cqi_report_cnfg.report_periodic.format_ind_periodic = LIBLTE_RRC_CQI_FORMAT_INDICATOR_PERIODIC_WIDEBAND_CQI;
    phy_cfg->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi = parent->cfg.cqi_cfg.simultaneousAckCQI;
    if (phy_cfg->antenna_info_present &&
        (phy_cfg->antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_3 ||
            phy_cfg->antenna_info_explicit_value.tx_mode == LIBLTE_RRC_TRANSMISSION_MODE_4)) {
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present = true;
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx = 483;
    } else {
      phy_cfg->cqi_report_cnfg.report_periodic.ri_cnfg_idx_present = false;
    }
  }
  phy_cfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset = 0;

  parent->phy->set_config_dedicated(rnti, phy_cfg);
  parent->phy->set_conf_dedicated_ack(rnti, false);
  parent->mac->set_dl_ant_info(rnti, &phy_cfg->antenna_info_explicit_value);
  parent->mac->phy_config_enabled(rnti, false);

  // Add SRB2 to the message 
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list_size = 1; 
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list[0].srb_id = 2; 
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list[0].lc_cnfg_present = true;
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list[0].lc_default_cnfg_present  = true; 
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list[0].rlc_cnfg_present = true;
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list[0].rlc_default_cnfg_present = true; 

  // Get DRB1 configuration 
  if (get_drbid_config(&conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[0], 1)) {
    parent->rrc_log->error("Getting DRB1 configuration\n");
  } else {
    conn_reconf->rr_cnfg_ded.drb_to_add_mod_list_size = 1; 
  }
  
  // Add SRB2 and DRB1 to the scheduler
  srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
  bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
  bearer_cfg.group = 0;
  parent->mac->bearer_ue_cfg(rnti, 2, &bearer_cfg);
  bearer_cfg.group = conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[0].lc_cnfg.ul_specific_params.log_chan_group;
  parent->mac->bearer_ue_cfg(rnti, 3, &bearer_cfg);
  
  // Configure SRB2 in RLC and PDCP
  parent->rlc->add_bearer(rnti, 2);

  // Configure SRB2 in PDCP
  srslte::srslte_pdcp_config_t pdcp_cnfg;
  pdcp_cnfg.direction = SECURITY_DIRECTION_DOWNLINK;
  pdcp_cnfg.is_control = true;
  pdcp_cnfg.is_data = false;
  parent->pdcp->add_bearer(rnti, 2, pdcp_cnfg);

  // Configure DRB1 in RLC
  parent->rlc->add_bearer(rnti, 3, &conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[0].rlc_cnfg);

  // Configure DRB1 in PDCP
  pdcp_cnfg.is_control = false;
  pdcp_cnfg.is_data = true;
  if (conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[0].pdcp_cnfg.rlc_um_pdcp_sn_size_present) {
    if(LIBLTE_RRC_PDCP_SN_SIZE_7_BITS == conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[0].pdcp_cnfg.rlc_um_pdcp_sn_size) {
      pdcp_cnfg.sn_len = 7;
    }
  }
  parent->pdcp->add_bearer(rnti, 3, pdcp_cnfg);

  // DRB1 has already been configured in GTPU through bearer setup

  // Add NAS Attach accept 
  conn_reconf->N_ded_info_nas = 1; 
  conn_reconf->ded_info_nas_list[0].N_bytes = parent->erab_info.N_bytes;
  memcpy(conn_reconf->ded_info_nas_list[0].msg, parent->erab_info.msg, parent->erab_info.N_bytes);
  
  // Reuse same PDU
  pdu->reset();
  
  send_dl_dcch(&dl_dcch_msg, pdu);
  
  state = RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE;
}

void rrc::ue::send_connection_reconf_new_bearer(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e)
{
  srslte::byte_buffer_t *pdu = parent->pool->allocate(__FUNCTION__);

  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG;
  dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id = (transaction_id++)%4;

  LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT* conn_reconf = &dl_dcch_msg.msg.rrc_con_reconfig;
  conn_reconf->rr_cnfg_ded_present   = true;
  conn_reconf->rr_cnfg_ded.mac_main_cnfg_present = false;
  conn_reconf->rr_cnfg_ded.phy_cnfg_ded_present  = false;
  conn_reconf->rr_cnfg_ded.rlf_timers_and_constants_present = false;
  conn_reconf->rr_cnfg_ded.sps_cnfg_present = false;
  conn_reconf->rr_cnfg_ded.drb_to_release_list_size = 0;
  conn_reconf->rr_cnfg_ded.srb_to_add_mod_list_size = 0;
  conn_reconf->rr_cnfg_ded.drb_to_add_mod_list_size = 0;
  conn_reconf->meas_cnfg_present     = false;
  conn_reconf->mob_ctrl_info_present = false;
  conn_reconf->sec_cnfg_ho_present   = false;

  for(uint32_t i=0; i<e->len; i++) {
    LIBLTE_S1AP_E_RABTOBESETUPITEMBEARERSUREQ_STRUCT *erab = &e->buffer[i];
    uint8_t id    = erab->e_RAB_ID.E_RAB_ID;
    uint8_t lcid  = id - 2; // Map e.g. E-RAB 5 to LCID 3 (==DRB1)

    // Get DRB configuration
    if (get_drbid_config(&conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[i], lcid)) {
      parent->rrc_log->error("Getting DRB configuration\n");
    } else {
      conn_reconf->rr_cnfg_ded.drb_to_add_mod_list_size++;
    }

    // Add DRB to the scheduler
    srsenb::sched_interface::ue_bearer_cfg_t bearer_cfg;
    bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;
    parent->mac->bearer_ue_cfg(rnti, lcid, &bearer_cfg);

    // Configure DRB in RLC
    parent->rlc->add_bearer(rnti, lcid, &conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[i].rlc_cnfg);
    // Configure DRB in PDCP
    parent->pdcp->add_bearer(rnti, lcid, &conn_reconf->rr_cnfg_ded.drb_to_add_mod_list[i].pdcp_cnfg);
    // DRB has already been configured in GTPU through bearer setup

    // Add NAS message
    conn_reconf->ded_info_nas_list[conn_reconf->N_ded_info_nas].N_bytes = parent->erab_info.N_bytes;
    memcpy(conn_reconf->ded_info_nas_list[conn_reconf->N_ded_info_nas].msg, parent->erab_info.msg, parent->erab_info.N_bytes);
    conn_reconf->N_ded_info_nas++;
  }

  send_dl_dcch(&dl_dcch_msg, pdu);
}

void rrc::ue::send_security_mode_command()
{
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND;

  LIBLTE_RRC_SECURITY_MODE_COMMAND_STRUCT* comm = &dl_dcch_msg.msg.security_mode_cmd;
  comm->rrc_transaction_id = (transaction_id++)%4;

  // TODO: select these based on UE capabilities and preference order
  comm->sec_algs.cipher_alg = (LIBLTE_RRC_CIPHERING_ALGORITHM_ENUM)cipher_algo;
  comm->sec_algs.int_alg    = (LIBLTE_RRC_INTEGRITY_PROT_ALGORITHM_ENUM)integ_algo;

  send_dl_dcch(&dl_dcch_msg);
}

void rrc::ue::send_ue_cap_enquiry()
{
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;
  dl_dcch_msg.msg_type = LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY;

  LIBLTE_RRC_UE_CAPABILITY_ENQUIRY_STRUCT* enq = &dl_dcch_msg.msg.ue_cap_enquiry;
  enq->rrc_transaction_id = (transaction_id++)%4;

  enq->N_ue_cap_reqs = 1;
  enq->ue_capability_request[0] = LIBLTE_RRC_RAT_TYPE_EUTRA;

  send_dl_dcch(&dl_dcch_msg);
}

/********************** HELPERS ***************************/ 

void rrc::ue::send_dl_ccch(LIBLTE_RRC_DL_CCCH_MSG_STRUCT *dl_ccch_msg) 
{
  // Allocate a new PDU buffer, pack the message and send to PDCP 
  byte_buffer_t *pdu = parent->pool->allocate(__FUNCTION__);
  if (pdu) {
    liblte_rrc_pack_dl_ccch_msg(dl_ccch_msg, (LIBLTE_BIT_MSG_STRUCT*) &parent->bit_buf);
    srslte_bit_pack_vector(parent->bit_buf.msg, pdu->msg, parent->bit_buf.N_bits);
    pdu->N_bytes = 1+(parent->bit_buf.N_bits-1)/8;
    parent->rrc_log->info_hex(pdu->msg, pdu->N_bytes, 
                          "SRB0 - rnti=0x%x, Sending: %s\n",
                          rnti,
                          liblte_rrc_dl_ccch_msg_type_text[dl_ccch_msg->msg_type]);
    
    parent->pdcp->write_sdu(rnti, RB_ID_SRB0, pdu);
    
  } else {
    parent->rrc_log->error("Allocating pdu\n");
  }
}

void rrc::ue::send_dl_dcch(LIBLTE_RRC_DL_DCCH_MSG_STRUCT *dl_dcch_msg, byte_buffer_t *pdu) 
{  
  if (!pdu) {
    pdu = parent->pool->allocate(__FUNCTION__);
  }
  if (pdu) {
    liblte_rrc_pack_dl_dcch_msg(dl_dcch_msg, (LIBLTE_BIT_MSG_STRUCT*) &parent->bit_buf);
    srslte_bit_pack_vector(parent->bit_buf.msg, pdu->msg, parent->bit_buf.N_bits);
    pdu->N_bytes = 1+(parent->bit_buf.N_bits-1)/8;
    parent->rrc_log->info_hex(pdu->msg, pdu->N_bytes, 
                          "SRB1 - rnti=0x%x, Sending: %s\n",
                          rnti,
                          liblte_rrc_dl_dcch_msg_type_text[dl_dcch_msg->msg_type]);
    
    parent->pdcp->write_sdu(rnti, RB_ID_SRB1, pdu);
    
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

void rrc::ue::sr_get(uint32_t *I_sr, uint32_t *N_pucch_sr)
{
  *I_sr       = sr_I; 
  *N_pucch_sr = sr_N_pucch; 
}

int rrc::ue::sr_allocate(uint32_t period, uint32_t *I_sr, uint32_t *N_pucch_sr) 
{
  uint32_t c = SRSLTE_CP_ISNORM(parent->cfg.cell.cp)?3:2;
  uint32_t delta_pucch_shift = liblte_rrc_delta_pucch_shift_num[parent->sib2.rr_config_common_sib.pucch_cnfg.delta_pucch_shift];
  
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
    if (period > 5) {
      *I_sr = period - 5 + parent->cfg.sr_cfg.sf_mapping[j_min]; 
    } else {
      *I_sr = period + parent->cfg.sr_cfg.sf_mapping[j_min]; 
    }
  } else {
    parent->rrc_log->error("Allocating SR: invalid sf_idx=%d for period=%d\n", parent->cfg.sr_cfg.sf_mapping[j_min], period);
    return -1; 
  }

  // Compute N_pucch_sr 
  *N_pucch_sr = i_min*max_users + parent->sr_sched.nof_users[i_min][j_min]; 
  if (parent->sib2.rr_config_common_sib.pucch_cnfg.n_cs_an) {
    *N_pucch_sr += parent->sib2.rr_config_common_sib.pucch_cnfg.n_cs_an;
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

void rrc::ue::cqi_get(uint32_t *pmi_idx, uint32_t *n_pucch)
{
  *pmi_idx = cqi_idx; 
  *n_pucch = cqi_pucch; 
}

int rrc::ue::cqi_allocate(uint32_t period, uint32_t *pmi_idx, uint32_t *n_pucch) 
{
  uint32_t c = SRSLTE_CP_ISNORM(parent->cfg.cell.cp)?3:2;
  uint32_t delta_pucch_shift = liblte_rrc_delta_pucch_shift_num[parent->sib2.rr_config_common_sib.pucch_cnfg.delta_pucch_shift];
  
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
  if (parent->sib2.rr_config_common_sib.pucch_cnfg.n_cs_an) {
    *n_pucch += parent->sib2.rr_config_common_sib.pucch_cnfg.n_cs_an;
  }
    
  // Allocate user 
  parent->cqi_sched.nof_users[i_min][j_min]++; 
  cqi_sched_prb_idx = i_min; 
  cqi_sched_sf_idx  = j_min; 
  cqi_allocated     = true; 
  cqi_idx           = *pmi_idx; 
  cqi_pucch         = *n_pucch; 
 
  parent->rrc_log->info("Allocated CQI resources for time-frequency slot (%d, %d), n_pucch_2=%d, pmi_cfg_idx=%d\n", 
                        cqi_sched_prb_idx, cqi_sched_sf_idx, *n_pucch, *pmi_idx);

  return 0; 
}




}
