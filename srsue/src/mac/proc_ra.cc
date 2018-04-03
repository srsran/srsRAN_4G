/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
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

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#include "srsue/hdr/mac/proc_ra.h"
#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/mac/mux.h"

/* Random access procedure as specified in Section 5.1 of 36.321 */


namespace srsue {

// Table 7.2-1. Backoff Parameter values
uint32_t backoff_table[16] = {0, 10, 20, 30, 40, 60, 80, 120, 160, 240, 320, 480, 960, 960, 960, 960};

// Table 7.6-1: DELTA_PREAMBLE values.
int delta_preamble_db_table[5] = {0, 0, -3, -3, 8};

void ra_proc::init(phy_interface_mac* phy_h_, 
                   rrc_interface_mac *rrc_, 
                   srslte::log* log_h_, 
                   mac_interface_rrc::ue_rnti_t *rntis_, 
                   mac_interface_rrc::mac_cfg_t *mac_cfg_, 
                   srslte::timers::timer* time_alignment_timer_,
                   srslte::timers::timer* contention_resolution_timer_,
                   mux* mux_unit_, 
                   demux* demux_unit_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  mac_cfg   = mac_cfg_;
  rntis     = rntis_;
  mux_unit  = mux_unit_; 
  demux_unit= demux_unit_; 
  rrc       = rrc_;

  time_alignment_timer        = time_alignment_timer_;
  contention_resolution_timer = contention_resolution_timer_;
  
  srslte_softbuffer_rx_init(&softbuffer_rar, 10);
  
  // Tell demux to call us when a UE CRID is received
  demux_unit->set_uecrid_callback(uecrid_callback, this);

  reset();
}

ra_proc::~ra_proc() {
  srslte_softbuffer_rx_free(&softbuffer_rar);
}

void ra_proc::reset() {
  state = IDLE;
  msg3_transmitted = false;
  started_by_pdcch = false; 
}

void ra_proc::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_; 
}

void ra_proc::read_params() {
  
  // Read initialization parameters   
  configIndex               = mac_cfg->prach_config_index;
  if (noncontention_enabled) {
    preambleIndex             = next_preamble_idx;
    maskIndex                 = next_prach_mask;
    noncontention_enabled     = false;
  } else {
    preambleIndex             = 0; // pass when called from higher layers for non-contention based RA
    maskIndex                 = 0; // same
  }
  nof_preambles             = liblte_rrc_number_of_ra_preambles_num[mac_cfg->rach.num_ra_preambles];
  if (mac_cfg->rach.preambles_group_a_cnfg.present) {
    nof_groupA_preambles    = liblte_rrc_size_of_ra_preambles_group_a_num[mac_cfg->rach.preambles_group_a_cnfg.size_of_ra];
  } else {
    nof_groupA_preambles    = nof_preambles;
  }

  if (nof_groupA_preambles > nof_preambles) {
    nof_groupA_preambles    = nof_preambles;
  }

  nof_groupB_preambles      = nof_preambles - nof_groupA_preambles;
  if (nof_groupB_preambles) {
    messagePowerOffsetGroupB= liblte_rrc_message_power_offset_group_b_num[mac_cfg->rach.preambles_group_a_cnfg.msg_pwr_offset_group_b];
    messageSizeGroupA       = liblte_rrc_message_size_group_a_num[mac_cfg->rach.preambles_group_a_cnfg.msg_size];
  }
  responseWindowSize        = liblte_rrc_ra_response_window_size_num[mac_cfg->rach.ra_resp_win_size];
  powerRampingStep          = liblte_rrc_power_ramping_step_num[mac_cfg->rach.pwr_ramping_step];
  preambleTransMax          = liblte_rrc_preamble_trans_max_num[mac_cfg->rach.preamble_trans_max];
  iniReceivedTargetPower    = liblte_rrc_preamble_initial_received_target_power_num[mac_cfg->rach.preamble_init_rx_target_pwr];
  contentionResolutionTimer = liblte_rrc_mac_contention_resolution_timer_num[mac_cfg->rach.mac_con_res_timer]; 

  delta_preamble_db         = delta_preamble_db_table[configIndex%5]; 
  
  if (contentionResolutionTimer > 0) {
    contention_resolution_timer->set(this, contentionResolutionTimer);
  }

}

bool ra_proc::in_progress()
{
  return (state > IDLE && state != COMPLETION_DONE);
}

bool ra_proc::is_successful() {
  return state == COMPLETION_DONE;
}

bool ra_proc::is_response_error() {
  return state == RESPONSE_ERROR;
}

bool ra_proc::is_contention_resolution() {
  return state == CONTENTION_RESOLUTION;
}

bool ra_proc::is_error() {
  return state == RA_PROBLEM;
}

const char* state_str[12] = {"Idle",
                            "RA:    INIT:   ",
                            "RA:    Select: ",
                            "RA:    TX:     ",
                            "RA:    PDCCH:  ",
                            "RA:    Rx:     ",
                            "RA:    RxErr:  ",
                            "RA:    Backof: ",
                            "RA:    ConRes: ",
                            "RA:    Done:   ",
                            "RA:    Done:   ",
                            "RA:    Error:  "};

                           
#define rError(fmt, ...) Error("%s" fmt, state_str[state], ##__VA_ARGS__)                         
#define rInfo(fmt, ...)  Info("%s" fmt, state_str[state], ##__VA_ARGS__)                         
#define rDebug(fmt, ...) Debug("%s" fmt, state_str[state], ##__VA_ARGS__)

                            
// Process Timing Advance Command as defined in Section 5.2
void ra_proc::process_timeadv_cmd(uint32_t ta) {
  if (preambleIndex == 0) {
    // Preamble not selected by UE MAC 
    phy_h->set_timeadv_rar(ta);
    time_alignment_timer->reset();
    time_alignment_timer->run();
    Debug("Applying RAR TA CMD %d\n", ta);
  } else {
    // Preamble selected by UE MAC 
    if (!time_alignment_timer->is_running()) {
      phy_h->set_timeadv_rar(ta);
      time_alignment_timer->run();
      Debug("Applying RAR TA CMD %d\n", ta);
    } else {
      // Ignore TA CMD
      Warning("Ignoring RAR TA CMD because timeAlignmentTimer still running\n");
    }
  }
}

void ra_proc::step_initialization() {
  read_params();
  pdcch_to_crnti_received = PDCCH_CRNTI_NOT_RECEIVED; 
  transmitted_contention_id = 0; 
  preambleTransmissionCounter = 1; 
  first_rar_received = true; 
  mux_unit->msg3_flush();
  msg3_flushed = false; 
  backoff_param_ms = 0; 

  // FIXME: This is because RA in Connected state not working in amarisoft
  transmitted_crnti = rntis->crnti;
  if(transmitted_crnti) {
    state = RESPONSE_ERROR;
  }
  
  // Instruct phy to configure PRACH
  phy_h->configure_prach_params();
  state = RESOURCE_SELECTION; 
}

void ra_proc::step_resource_selection() {
  ra_group_t sel_group; 

  if (preambleIndex > 0) {
    // Preamble is chosen by Higher layers (ie Network)
    sel_maskIndex = maskIndex;
    sel_preamble = (uint32_t) preambleIndex;
  } else {
    // Preamble is chosen by MAC UE
    if (!msg3_transmitted) {
      if (nof_groupB_preambles > 0 && new_ra_msg_len > messageSizeGroupA) { // Check also pathloss (Pcmax,deltaPreamble and powerOffset)
        sel_group = RA_GROUP_B; 
      } else {
        sel_group = RA_GROUP_A; 
      }
      last_msg3_group = sel_group;
    } else {
      sel_group = last_msg3_group; 
    }
    if (sel_group == RA_GROUP_A) {
      if (nof_groupA_preambles) {
        sel_preamble = preambleTransmissionCounter%nof_groupA_preambles;
      } else {
        rError("Selected group preamble A but nof_groupA_preambles=0\n");
        state = RA_PROBLEM;
        return; 
      }
    } else {
      if (nof_groupB_preambles) {
        sel_preamble = nof_groupA_preambles + rand()%nof_groupB_preambles;
      } else {
        rError("Selected group preamble B but nof_groupA_preambles=0\n");
        state = RA_PROBLEM;
        return; 
      }
    }
    sel_maskIndex = 0;           
  }
  
  rDebug("Selected preambleIndex=%d maskIndex=%d GroupA=%d, GroupB=%d\n", 
        sel_preamble, sel_maskIndex,nof_groupA_preambles, nof_groupB_preambles);
  state = PREAMBLE_TRANSMISSION;
}

void ra_proc::step_preamble_transmission() {
  received_target_power_dbm = iniReceivedTargetPower + 
      delta_preamble_db + 
      (preambleTransmissionCounter-1)*powerRampingStep;
      
  rar_received = false; 
  phy_h->prach_send(sel_preamble, sel_maskIndex - 1, received_target_power_dbm); 
  state = PDCCH_SETUP;
}

void ra_proc::step_pdcch_setup() {
  int ra_tti = phy_h->prach_tx_tti();
  if (ra_tti > 0) {    
    ra_rnti = 1+ra_tti%10;
    rInfo("seq=%d, ra-rnti=0x%x, ra-tti=%d\n", sel_preamble, ra_rnti, ra_tti);
    log_h->console("Random Access Transmission: seq=%d, ra-rnti=0x%x\n", sel_preamble, ra_rnti);
    phy_h->pdcch_dl_search(SRSLTE_RNTI_RAR, ra_rnti, ra_tti+3, ra_tti+3+responseWindowSize);
    state = RESPONSE_RECEPTION;
  }
}

void ra_proc::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action) 
{
  if (grant.n_bytes[0] < MAX_RAR_PDU_LEN) {
    rDebug("DL grant found RA-RNTI=%d\n", ra_rnti);        
    action->decode_enabled[0] = true;
    action->decode_enabled[1] = false;
    action->default_ack[0] = false;
    action->generate_ack = false; 
    action->payload_ptr[0] = rar_pdu_buffer;
    action->rnti = grant.rnti; 
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    action->rv[0] = grant.rv[0];
    action->softbuffers[0] = &softbuffer_rar;
    rar_grant_nbytes = grant.n_bytes[0];
    rar_grant_tti    = grant.tti; 
    if (action->rv[0] == 0) {
      srslte_softbuffer_rx_reset(&softbuffer_rar);
    }
  } else {
    rError("Received RAR grant exceeds buffer length (%d>%d)\n", grant.n_bytes[0], MAX_RAR_PDU_LEN);
    action->decode_enabled[0] = false;
    action->decode_enabled[1] = false;
    state = RESPONSE_ERROR;
  }
}

void ra_proc::tb_decoded_ok() {
  if (pcap) {
    pcap->write_dl_ranti(rar_pdu_buffer, rar_grant_nbytes, ra_rnti, true, rar_grant_tti);            
  }
  
  rDebug("RAR decoded successfully TBS=%d\n", rar_grant_nbytes);
  
  rar_pdu_msg.init_rx(rar_grant_nbytes);
  rar_pdu_msg.parse_packet(rar_pdu_buffer);
  // Set Backoff parameter
  if (rar_pdu_msg.has_backoff()) {
    backoff_param_ms = backoff_table[rar_pdu_msg.get_backoff()%16];
  } else {
    backoff_param_ms = 0; 
  }
  
  current_ta = 0; 
  
  while(rar_pdu_msg.next()) {
    if (rar_pdu_msg.get()->get_rapid() == sel_preamble) {

      rar_received = true; 
      process_timeadv_cmd(rar_pdu_msg.get()->get_ta_cmd());
      
      // FIXME: Indicate received target power
      //phy_h->set_target_power_rar(iniReceivedTargetPower, (preambleTransmissionCounter-1)*powerRampingStep);

      uint8_t grant[srslte::rar_subh::RAR_GRANT_LEN];
      rar_pdu_msg.get()->get_sched_grant(grant);

      phy_h->pdcch_dl_search_reset();
      
      phy_h->set_rar_grant(rar_grant_tti, grant);          
      
      current_ta = rar_pdu_msg.get()->get_ta_cmd();
      
      rInfo("RAPID=%d, TA=%d\n", sel_preamble, rar_pdu_msg.get()->get_ta_cmd()); 
      
      if (preambleIndex > 0) {
        // Preamble selected by Network
        state = COMPLETION; 
      } else {
        // Preamble selected by UE MAC
        mux_unit->msg3_prepare();
        rntis->temp_rnti = rar_pdu_msg.get()->get_temp_crnti();
        phy_h->pdcch_dl_search(SRSLTE_RNTI_TEMP, rar_pdu_msg.get()->get_temp_crnti());
        
        if (first_rar_received) {
          first_rar_received = false; 
          
          // Save transmitted C-RNTI (if any) 
          transmitted_crnti = rntis->crnti;
          
          // If we have a C-RNTI, tell Mux unit to append C-RNTI CE if no CCCH SDU transmission
          if (transmitted_crnti) {
            rInfo("Appending C-RNTI MAC CE 0x%x in next transmission\n", transmitted_crnti);
            mux_unit->append_crnti_ce_next_tx(transmitted_crnti);
            phy_h->pdcch_ul_search(SRSLTE_RNTI_USER, transmitted_crnti);
            phy_h->pdcch_dl_search(SRSLTE_RNTI_USER, transmitted_crnti);
          }          
        }                  
        rDebug("Going to Contention Resolution state\n");
        state = CONTENTION_RESOLUTION;
        
        // Start contention resolution timer 
        contention_resolution_timer->reset();
        contention_resolution_timer->run();
      }  
    } else {
      rInfo("Found RAR for preamble %d\n", rar_pdu_msg.get()->get_rapid());
    }
  }
}

void ra_proc::step_response_reception() {
  // do nothing. Processing done in tb_decoded_ok()
  int ra_tti = phy_h->prach_tx_tti();
  if (ra_tti >= 0 && !rar_received) {
    uint32_t interval = srslte_tti_interval(phy_h->get_current_tti(), ra_tti+3+responseWindowSize); 
    if (interval > 1 && interval < 100) {
      rDebug("RA response not received within the response window\n");
      state = RESPONSE_ERROR;
    }
  }
}

void ra_proc::step_response_error()
{
  preambleTransmissionCounter++;
  if (preambleTransmissionCounter >= preambleTransMax + 1) {
    rError("Maximum number of transmissions reached (%d)\n", preambleTransMax);
    rrc->ra_problem();
    state = RA_PROBLEM;
    if (ra_is_ho) {
      rrc->ho_ra_completed(false);
    }
  } else {
    backoff_interval_start = phy_h->get_current_tti(); 
    if (backoff_param_ms) {
      backoff_inteval = rand()%backoff_param_ms;          
    } else {
      backoff_inteval = 0; 
    }
    if (backoff_inteval) {
      rDebug("Backoff wait interval %d\n", backoff_inteval);
      state = BACKOFF_WAIT; 
    } else {
      rDebug("Transmitting inmediatly (%d/%d)\n", preambleTransmissionCounter, preambleTransMax);
      state = RESOURCE_SELECTION;
    }
  }
}

void ra_proc::step_backoff_wait() {
  if (srslte_tti_interval(phy_h->get_current_tti(), backoff_interval_start) >= backoff_inteval) {
    state = RESOURCE_SELECTION; 
  }
}

bool ra_proc::uecrid_callback(void *arg, uint64_t uecri) {
  return ((ra_proc*) arg)->contention_resolution_id_received(uecri);
}

// Random Access initiated by RRC by the transmission of CCCH SDU      
bool ra_proc::contention_resolution_id_received(uint64_t rx_contention_id) {
  bool uecri_successful = false; 
  
  rDebug("MAC PDU Contains Contention Resolution ID CE\n");
  
  // MAC PDU successfully decoded and contains MAC CE contention Id
  contention_resolution_timer->stop();
  
  if (transmitted_contention_id == rx_contention_id) 
  {    
    // UE Contention Resolution ID included in MAC CE matches the CCCH SDU transmitted in Msg3
    rntis->crnti = rntis->temp_rnti;
    // finish the disassembly and demultiplexing of the MAC PDU
    uecri_successful = true;
    state = COMPLETION;                           
  } else {
    rInfo("Transmitted UE Contention Id differs from received Contention ID (0x%lx != 0x%lx)\n", 
          transmitted_contention_id, rx_contention_id);
    // Discard MAC PDU 
    uecri_successful = false;

    // Contention Resolution not successfully is like RAR not successful 
    // FIXME: Need to flush Msg3 HARQ buffer. Why? 
    state = RESPONSE_ERROR; 
  }  
  rntis->temp_rnti = 0; 
  
  return uecri_successful;
}

void ra_proc::step_contention_resolution() {
  // If Msg3 has been sent
  if (mux_unit->msg3_is_transmitted()) 
  {    
    msg3_transmitted = true; 
    if (transmitted_crnti) 
    {
      // Random Access with transmission of MAC C-RNTI CE
      if ((!started_by_pdcch && pdcch_to_crnti_received == PDCCH_CRNTI_UL_GRANT) || 
            (started_by_pdcch && pdcch_to_crnti_received != PDCCH_CRNTI_NOT_RECEIVED))
      {
        rDebug("PDCCH for C-RNTI received\n");
        contention_resolution_timer->stop();
        rntis->temp_rnti = 0; 
        state = COMPLETION;           
      }            
      pdcch_to_crnti_received = PDCCH_CRNTI_NOT_RECEIVED;      
    } else {
      // RA with transmission of CCCH SDU is resolved in contention_resolution_id_received() callback function
      if (!transmitted_contention_id) {
        // Save transmitted UE contention id, as defined by higher layers 
        transmitted_contention_id = rntis->contention_id;
        rntis->contention_id      = 0; 
      }
    }
  } else {
    rDebug("Msg3 not yet transmitted\n");
  }
  
}

void ra_proc::step_completition() {
  log_h->console("Random Access Complete.     c-rnti=0x%x, ta=%d\n", rntis->crnti, current_ta);
  rInfo("Random Access Complete.     c-rnti=0x%x, ta=%d\n",          rntis->crnti, current_ta);
  if (!msg3_flushed) {
    mux_unit->msg3_flush();
    msg3_flushed = true; 
  }
  // Configure PHY to look for UL C-RNTI grants
  phy_h->pdcch_ul_search(SRSLTE_RNTI_USER, rntis->crnti);
  phy_h->pdcch_dl_search(SRSLTE_RNTI_USER, rntis->crnti);

  phy_h->set_crnti(rntis->crnti);

  msg3_transmitted = false;
  state = COMPLETION_DONE;
  if (ra_is_ho) {
    rrc->ho_ra_completed(true);
  }
}

void ra_proc::step(uint32_t tti_)
{
  switch(state) {
    case IDLE: 
      break;
    case INITIALIZATION:
      step_initialization();
      break;
    case RESOURCE_SELECTION:
      step_resource_selection();
    break;
    case PREAMBLE_TRANSMISSION:
      step_preamble_transmission();
    break;
    case PDCCH_SETUP:      
      step_pdcch_setup();
    break;
    case RESPONSE_RECEPTION:      
      step_response_reception();
    break;
    case RESPONSE_ERROR:
      step_response_error();
    break;
    case BACKOFF_WAIT:
      step_backoff_wait();
    break;
    case CONTENTION_RESOLUTION:
      step_contention_resolution();
    break;
    case COMPLETION:
      step_completition();
    case COMPLETION_DONE:
    case RA_PROBLEM:
    break;
  }
}

void ra_proc::start_noncont(uint32_t preamble_index, uint32_t prach_mask) {
  next_preamble_idx = preamble_index;
  next_prach_mask   = prach_mask;
  noncontention_enabled = true;
  start_mac_order(56, true);
}

void ra_proc::start_mac_order(uint32_t msg_len_bits, bool is_ho)
{
  if (state == IDLE || state == COMPLETION_DONE || state == RA_PROBLEM) {
    ra_is_ho = is_ho;
    started_by_pdcch = false;
    new_ra_msg_len = msg_len_bits; 
    state = INITIALIZATION;    
    rInfo("Starting PRACH by MAC order\n");
  }
}

void ra_proc::start_pdcch_order()
{
  if (state == IDLE || state == COMPLETION_DONE || state == RA_PROBLEM) {
    started_by_pdcch = true;
    state = INITIALIZATION;    
    rInfo("Starting PRACH by PDCCH order\n");
  }
}

// Contention Resolution Timer is expired (Section 5.1.5)
void ra_proc::timer_expired(uint32_t timer_id)
{
  rInfo("Contention Resolution Timer expired. Stopping PDCCH Search and going to Response Error\n");
  rntis->temp_rnti = 0; 
  state = RESPONSE_ERROR; 
  phy_h->pdcch_dl_search_reset();
}

void ra_proc::pdcch_to_crnti(bool contains_uplink_grant) {
  rDebug("PDCCH to C-RNTI received %s UL grant\n", contains_uplink_grant?"with":"without");
  if (contains_uplink_grant) {
    pdcch_to_crnti_received = PDCCH_CRNTI_UL_GRANT;     
  } else if (pdcch_to_crnti_received == PDCCH_CRNTI_NOT_RECEIVED) {
    pdcch_to_crnti_received = PDCCH_CRNTI_DL_GRANT;         
  }
}

void ra_proc::harq_retx()
{
  contention_resolution_timer->reset();
}

}

