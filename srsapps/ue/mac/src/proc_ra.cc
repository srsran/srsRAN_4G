/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#include "srsapps/ue/mac/proc_ra.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/mux.h"

/* Random access procedure as specified in Section 5.1 of 36.321 */

namespace srslte {
namespace ue {

// Table 7.2-1. Backoff Parameter values
uint32_t backoff_table[16] = {0, 10, 20, 30, 40, 60, 80, 120, 160, 240, 320, 480, 960, 960, 960, 960};

// Table 7.6-1: DELTA_PREAMBLE values.
int delta_preamble_db_table[5] = {0, 0, -3, -3, 8};

bool ra_proc::init(phy_interface* phy_h_, log* log_h_, mac_params* params_db_, timers* timers_db_, 
                   mux* mux_unit_, demux* demux_unit_)
{
  phy_h     = phy_h_; 
  log_h     = log_h_; 
  params_db = params_db_;
  timers_db = timers_db_;
  mux_unit  = mux_unit_; 
  demux_unit= demux_unit_; 
  srslte_softbuffer_rx_init(&softbuffer_rar, 10);
  
  reset();
}

void ra_proc::reset() {
  state = IDLE;   
}

void ra_proc::start_pcap(mac_pcap* pcap_)
{
  pcap = pcap_; 
}

void ra_proc::read_params() {
  
  // Read initialization parameters   
  configIndex               = params_db->get_param(mac_interface_params::RA_CONFIGINDEX);
  preambleIndex             = params_db->get_param(mac_interface_params::RA_PREAMBLEINDEX);
  maskIndex                 = params_db->get_param(mac_interface_params::RA_MASKINDEX); 
  nof_preambles             = params_db->get_param(mac_interface_params::RA_NOFPREAMBLES); 
  if (!nof_preambles || nof_preambles > 64) {
    nof_preambles = 64; 
  }
  nof_groupA_preambles      = params_db->get_param(mac_interface_params::RA_NOFGROUPAPREAMBLES);
  if (!nof_groupA_preambles) {
    nof_groupA_preambles    = nof_preambles; 
  }
  if (nof_groupA_preambles > nof_preambles) {
    nof_groupA_preambles    = nof_preambles;
  }
  nof_groupB_preambles      = nof_preambles - nof_groupA_preambles;
  if (nof_groupB_preambles) {
    messagePowerOffsetGroupB= params_db->get_param(mac_interface_params::RA_MESSAGEPOWEROFFSETB);
    messageSizeGroupA       = params_db->get_param(mac_interface_params::RA_MESSAGESIZEA);
    Pcmax                   = params_db->get_param(mac_interface_params::RA_PCMAX);
    deltaPreambleMsg3       = params_db->get_param(mac_interface_params::RA_DELTAPREAMBLEMSG3);      
  }
  responseWindowSize        = params_db->get_param(mac_interface_params::RA_RESPONSEWINDOW);
  powerRampingStep          = params_db->get_param(mac_interface_params::RA_POWERRAMPINGSTEP);
  preambleTransMax          = params_db->get_param(mac_interface_params::RA_PREAMBLETRANSMAX);
  iniReceivedTargetPower    = params_db->get_param(mac_interface_params::RA_INITRECEIVEDPOWER);
  contentionResolutionTimer = params_db->get_param(mac_interface_params::RA_CONTENTIONTIMER); 

  delta_preamble_db         = delta_preamble_db_table[configIndex%5]; 
  
  if (contentionResolutionTimer > 0) {
    timers_db->get(mac::CONTENTION_TIMER)->set(this, contentionResolutionTimer);
  }

}

bool ra_proc::in_progress()
{
  return (state > IDLE && state != COMPLETION);
}

bool ra_proc::is_successful() {
  return state == COMPLETION;
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

const char* state_str[11] = {"Idle",
                            "RA Initializat.: ",
                            "RA ResSelection: ",
                            "RA PreambleTx  : ",
                            "RA PDCCH setup : ",
                            "RA PreambleRx  : ",
                            "RA ResponseErr : ",
                            "RA BackoffWait : ",
                            "RA ContentResol: ",
                            "RA Completed   : ",
                            "RA Problem     : "};
                           
#define rError(fmt, ...) Error("%s" fmt, state_str[state], ##__VA_ARGS__)                         
#define rInfo(fmt, ...)  Info("%s" fmt, state_str[state], ##__VA_ARGS__)                         
#define rDebug(fmt, ...) Debug("%s" fmt, state_str[state], ##__VA_ARGS__)

                            
// Process Timing Advance Command as defined in Section 5.2
void ra_proc::process_timeadv_cmd(uint32_t ta) {
  if (preambleIndex > 0) {
    // Preamble not selected by UE MAC 
    phy_h->set_timeadv_rar(ta);
    timers_db->get(mac::TIME_ALIGNMENT)->reset();
    timers_db->get(mac::TIME_ALIGNMENT)->run();
    Info("Applying RAR TA CMD %d\n", ta);
  } else {
    // Preamble selected by UE MAC 
    if (!timers_db->get(mac::TIME_ALIGNMENT)->is_running()) {
      phy_h->set_timeadv_rar(ta);
      timers_db->get(mac::TIME_ALIGNMENT)->run();
      Info("Applying RAR TA CMD %d\n", ta);
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
  
  // Instruct phy to configure PRACH
  phy_h->configure_prach_params();
  state = RESOURCE_SELECTION; 
}

void ra_proc::step_resource_selection() {
  ra_group_t sel_group; 
  if (preambleIndex > 0) {
    // Preamble is chosen by Higher layers (ie Network)
    sel_maskIndex = maskIndex;
    sel_preamble = (uint32_t) preambleIndex%nof_preambles;
  } else {
    // Preamble is chosen by MAC UE
    if (!msg3_transmitted) {
      if (nof_groupB_preambles > 0) { // Check also messageSizeGroupA and pathloss (Pcmax,deltaPreamble and powerOffset)
        sel_group = RA_GROUP_B; 
      } else {
        sel_group = RA_GROUP_A; 
      }
      last_msg3_group = sel_group;
    } else {
      sel_group = last_msg3_group; 
    }
    if (sel_group == RA_GROUP_A) {
      sel_preamble = rand()%(nof_groupA_preambles-1);
    } else {
      sel_preamble = nof_groupA_preambles + rand()%(nof_groupB_preambles-1);
    }
    sel_maskIndex = 0;           
  }
  
  rInfo("Selected preambleIndex=%d maskIndex=%d nof_GroupApreambles=%d\n", 
        sel_preamble, sel_maskIndex,nof_groupA_preambles);
  state = PREAMBLE_TRANSMISSION;
}

void ra_proc::step_preamble_transmission() {
  received_target_power_dbm = iniReceivedTargetPower + 
      delta_preamble_db + 
      (preambleTransmissionCounter-1)*powerRampingStep;
      
  phy_h->prach_send(sel_preamble, sel_maskIndex - 1, received_target_power_dbm); 
  rInfo("Selected received_target_power_dbm=%d dBm\n", received_target_power_dbm);
  state = PDCCH_SETUP;
}

void ra_proc::step_pdcch_setup() {
  int ra_tti = phy_h->prach_tx_tti();
  if (ra_tti > 0) {    
    ra_rnti = 1+ra_tti%10;
    phy_h->pdcch_dl_search(SRSLTE_RNTI_RAR, ra_rnti, ra_tti+3, ra_tti+3+responseWindowSize);
    state = RESPONSE_RECEPTION;
  }
}

void ra_proc::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action) 
{
  if (grant.n_bytes < MAX_RAR_PDU_LEN) {
    rInfo("DL grant found RA-RNTI=%d\n", ra_rnti);        
    action->decode_enabled = true; 
    action->default_ack = false; 
    action->generate_ack = false; 
    action->payload_ptr = rar_pdu_buffer; 
    action->rnti = grant.rnti; 
    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    action->rv = grant.rv;
    action->softbuffer = &softbuffer_rar;
    rar_grant_nbytes = grant.n_bytes;
    rar_grant_tti    = grant.tti; 
    if (action->rv == 0) {
      srslte_softbuffer_rx_reset(&softbuffer_rar);
    }
  } else {
    rError("Received RAR grant exceeds buffer length (%d>%d)\n", grant.n_bytes, MAX_RAR_PDU_LEN);
    action->decode_enabled = false; 
    state = RESPONSE_ERROR;
  }
}

void ra_proc::tb_decoded_ok() {
  if (pcap) {
    pcap->write_dl_crnti(rar_pdu_buffer, rar_grant_nbytes, ra_rnti, true, rar_grant_tti);            
  }
  
  rDebug("RAR decoded successfully TBS=%d\n", rar_grant_nbytes);
  
  rar_pdu_msg.init_rx(rar_pdu_buffer, rar_grant_nbytes);

  // Set Backoff parameter
  if (rar_pdu_msg.has_backoff()) {
    backoff_param_ms = backoff_table[rar_pdu_msg.get_backoff()%16];
  } else {
    backoff_param_ms = 0; 
  }
  
  while(rar_pdu_msg.next()) {
    if (rar_pdu_msg.get()->get_rapid() == sel_preamble) {
      rInfo("Received RAPID=%d\n", sel_preamble);

      process_timeadv_cmd(rar_pdu_msg.get()->get_ta_cmd());
      
      // FIXME: Indicate received target power
      //phy_h->set_target_power_rar(iniReceivedTargetPower, (preambleTransmissionCounter-1)*powerRampingStep);

      uint8_t grant[rar_subh::RAR_GRANT_LEN];
      rar_pdu_msg.get()->get_sched_grant(grant);

      phy_h->pdcch_dl_search_reset();
      
      phy_h->set_rar_grant(rar_grant_tti, grant);          
      
      if (preambleIndex > 0) {
        // Preamble selected by Network
        state = COMPLETION; 
      } else {
        // Preamble selected by UE MAC 
        params_db->set_param(mac_interface_params::RNTI_TEMP, rar_pdu_msg.get()->get_temp_crnti());
        phy_h->pdcch_dl_search(SRSLTE_RNTI_TEMP, rar_pdu_msg.get()->get_temp_crnti());
        
        if (first_rar_received) {
          first_rar_received = false; 
          
          // Save transmitted C-RNTI (if any) 
          transmitted_crnti = params_db->get_param(mac_interface_params::RNTI_C);
          
          // If we have a C-RNTI, tell Mux unit to append C-RNTI CE if no CCCH SDU transmission
          if (transmitted_crnti) {
            mux_unit->append_crnti_ce_next_tx(transmitted_crnti);
          }           
          
          // Tell demux to call us when a UE CRID is received
          demux_unit->set_uecrid_callback(uecrid_callback, this);
        }                  
        rDebug("Going to Contention Resolution state\n");
        state = CONTENTION_RESOLUTION;
        
        // Start contention resolution timer 
        timers_db->get(mac::CONTENTION_TIMER)->reset();
        timers_db->get(mac::CONTENTION_TIMER)->run();                      
      }  
    } else {
      rDebug("Found RAR for preamble %d\n", rar_pdu_msg.get()->get_rapid());
    }
  }
}

void ra_proc::step_response_reception() {
  // do nothing. Processing done in tb_decoded_ok()
}

void ra_proc::step_response_error() {
  
  preambleTransmissionCounter++;
  if (preambleTransmissionCounter >= preambleTransMax + 1) {
    rError("Maximum number of transmissions reached (%d)\n", preambleTransMax);
    state = RA_PROBLEM;
  } else {
    backoff_interval_start = phy_h->get_current_tti(); 
    if (backoff_param_ms) {
      backoff_inteval = rand()%backoff_param_ms;          
    } else {
      backoff_inteval = 0; 
    }
    if (backoff_inteval) {
      rInfo("Backoff wait interval %d\n", backoff_inteval);
      state = BACKOFF_WAIT; 
    } else {
      rInfo("Transmitting inmediatly (%d/%d)\n", preambleTransmissionCounter, preambleTransMax);
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
  timers_db->get(mac::CONTENTION_TIMER)->stop();
  
  if (transmitted_contention_id == rx_contention_id) 
  {
    rDebug("MAC PDU Contention Resolution ID matches the one transmitted in CCCH SDU\n");
    // UE Contention Resolution ID included in MAC CE matches the CCCH SDU transmitted in Msg3
    params_db->set_param(mac_interface_params::RNTI_C, params_db->get_param(mac_interface_params::RNTI_TEMP));
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
  params_db->set_param(mac_interface_params::RNTI_TEMP, 0);                
  
  return uecri_successful;
}

void ra_proc::step_contention_resolution() {
  // If Msg3 has been sent
  if (mux_unit->msg3_is_transmitted()) 
  {
    // Save transmitted UE contention id, as defined by higher layers 
    if (!transmitted_contention_id) {
      transmitted_contention_id = params_db->get_param(mac_interface_params::CONTENTION_ID);
      params_db->set_param(mac_interface_params::CONTENTION_ID, 0);                        
    }
    
    msg3_transmitted = true; 
    if (pdcch_to_crnti_received != PDCCH_CRNTI_NOT_RECEIVED) 
    {
      rInfo("PDCCH for C-RNTI received\n");
      // Random Access initiated by MAC itself or PDCCH order (transmission of MAC C-RNTI CE)
      if (start_mode == MAC_ORDER && pdcch_to_crnti_received == PDCCH_CRNTI_UL_GRANT ||
          start_mode == PDCCH_ORDER) 
      {
        timers_db->get(mac::CONTENTION_TIMER)->stop();
        params_db->set_param(mac_interface_params::RNTI_TEMP, 0);
        state = COMPLETION;           
      }            
      pdcch_to_crnti_received = PDCCH_CRNTI_NOT_RECEIVED;      
    }
    // RA initiated by RLC order is resolved in contention_resolution_id_received() callback function
  } else {
    rDebug("Msg3 not yet transmitted\n");
  }
  
}

void ra_proc::step_completition() {
  params_db->set_param(mac_interface_params::RA_PREAMBLEINDEX, 0);
  params_db->set_param(mac_interface_params::RA_MASKINDEX, 0);
  if (!msg3_flushed) {
    mux_unit->msg3_flush();
    msg3_flushed = true; 
  }
  msg3_transmitted = false;  
}

void ra_proc::step(uint32_t tti_)
{
  if (is_running()) {
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
      break;
    }
  }  
}

void ra_proc::start_mac_order()
{
  if (state == IDLE || state == COMPLETION || state == RA_PROBLEM) {
    start_mode = MAC_ORDER;
    state = INITIALIZATION;    
    Info("Starting PRACH by MAC order\n");
    run();
  }
}

void ra_proc::start_pdcch_order()
{
  if (state == IDLE || state == COMPLETION || state == RA_PROBLEM) {
    start_mode = PDCCH_ORDER;
    state = INITIALIZATION;    
    Info("Starting PRACH by PDCCH order\n");
    run();
  }
}

void ra_proc::start_rlc_order()
{
  if (state == IDLE || state == COMPLETION || state == RA_PROBLEM) {
    start_mode = RLC_ORDER;
    state = INITIALIZATION;    
    Info("Starting PRACH by RLC CCCH SDU order\n");
    run();
  }
}

// Contention Resolution Timer is expired (Section 5.1.5)
void ra_proc::timer_expired(uint32_t timer_id)
{
  rInfo("Contention Resolution Timer expired. Going to Response Error\n");
  params_db->set_param(mac_interface_params::RNTI_TEMP, 0);
  state = RESPONSE_ERROR; 
}

void ra_proc::pdcch_to_crnti(bool is_uplink_grant) {
  if (is_uplink_grant) {
    pdcch_to_crnti_received = PDCCH_CRNTI_UL_GRANT;     
  } else {
    pdcch_to_crnti_received = PDCCH_CRNTI_DL_GRANT;         
  }
}

}
}

