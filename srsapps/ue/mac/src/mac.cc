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

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

#include "srsapps/common/threads.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/mac_params.h"

namespace srslte {
namespace ue {
    
bool mac::init(phy *phy_h_, tti_sync* ttisync_, log* log_h_)
{
  started = false; 
  ttisync = ttisync_;
  phy_h = phy_h_;
  log_h = log_h_; 
  tti = 0; 
  is_synchronized = false;   
  last_temporal_crnti = 0; 
  phy_rnti = 0; 
  
  bsr_procedure.init(log_h, &timers_db, &params_db, &mac_io_lch);
  mux_unit.init(log_h, &mac_io_lch, &bsr_procedure);
  demux_unit.init(phy_h, log_h, &mac_io_lch, &timers_db);
  ra_procedure.init(&params_db, phy_h, log_h, &timers_db, &mux_unit, &demux_unit);
  sr_procedure.init(log_h, &params_db, phy_h);
  reset();

  if (threads_new_rt(&mac_thread, mac_thread_fnc, this)) {
    started = true; 
  }

  return started; 
}

void mac::stop()
{
  started = false;   
  pthread_join(mac_thread, NULL);
}

int mac::get_tti()
{
  if (is_synchronized) {
    return (int) tti; 
  } else {
    return -1;
  }
}

void mac::start_trace()
{
  tr_enabled = true; 
}

void mac::write_trace(std::string filename)
{
  tr_start_time.writeToBinary(filename + ".start");
  tr_end_time.writeToBinary(filename + ".end");
}

void mac::tr_log_start(uint32_t tti)
{
  if (tr_enabled) {
    tr_start_time.push_cur_time_us(tti);
  }
}

void mac::tr_log_end(uint32_t tti)
{
  if (tr_enabled) {
    tr_end_time.push_cur_time_us(tti);
  }
}


// Implement Section 5.8
void mac::reconfiguration()
{

}

// Implement Section 5.9
void mac::reset()
{
  timers_db.stop_all();
  timeAlignmentTimerExpire(); 
  ul_harq.reset_ndi();
  
  mux_unit.msg3_flush();
  mux_unit.reset();
  
  ra_procedure.stop();
  ra_procedure.reset();    
  sr_procedure.stop();
  sr_procedure.reset();
  bsr_procedure.stop();
  bsr_procedure.reset();
  phr_procedure.stop();
  phr_procedure.reset();
  
  dl_harq.reset();
  params_db.set_param(mac_params::RNTI_TEMP, 0); 
}

void* mac::mac_thread_fnc(void *arg) {
  srslte::ue::mac* mac = static_cast<srslte::ue::mac*>(arg);  
  mac->main_radio_loop();
  return NULL; 
}

void mac::main_radio_loop() {
  setup_timers();
  while(1) {
    if (!is_synchronized) {      
      srslte_cell_t cell; 
      uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
      
      if (phy_h->decode_mib_best(&cell, bch_payload)) {
        
        // Print MIB 
        srslte_cell_fprint(stdout, &cell, phy_h->get_current_tti()/10);

        // Init HARQ for this cell 
        Info("Init UL/DL HARQ\n");
        ul_harq.init(cell, &params_db, log_h, &timers_db, &mux_unit);
        dl_harq.init(cell, 8*1024, log_h, &timers_db, &demux_unit);

        // Set the current PHY cell to the detected cell
        Info("Setting up PHY for cell_id=%d\n", cell.id);
        if (phy_h->set_cell(cell)) {
          Info("Starting RX streaming\n");
          if (phy_h->start_rxtx()) {
            log_h->step(ttisync->wait());
    
            Info("Receiver synchronized\n");

            // Send MIB to RRC 
            mac_io_lch.get(mac_io::MAC_LCH_BCCH_DL)->send(bch_payload, SRSLTE_BCH_PAYLOAD_LEN);
        
            ttisync->resync();
            Info("Wait for AGC, CFO estimation, etc. \n");
            for (int i=0;i<1000;i++) {
              tti = ttisync->wait();
            }
            is_synchronized = true;               
          } else {
            Error("Starting PHY receiver\n");
            exit(-1);
          }
        } else {
          Error("Setting PHY cell\n");
          exit(-1);
        }
      } else {
        Warning("Cell not found\n");
        sleep(1);
      }
    }
    if (is_synchronized) {
      /* Warning: Here order of invocation of procedures is important!! */
      tr_log_end(tti);
      tti = ttisync->wait();
      tr_log_start(tti);
      log_h->step(tti);
      
      // Step all procedures 
      bsr_procedure.step(tti);
      
      // Check if BSR procedure need to start SR 
      
      if (bsr_procedure.need_to_send_sr()) {
        Info("Starting SR procedure by BSR request, PHY TTI=%d\n", phy_h->get_current_tti());
        sr_procedure.start();
      } else if (bsr_procedure.need_to_reset_sr()) {
        Info("Resetting SR procedure by BSR request\n");
        sr_procedure.reset();
      }
      sr_procedure.step(tti);

      // Check SR if we need to start RA 
      if (sr_procedure.need_random_access()) {
        Info("Starting RA procedure by MAC order\n");
        //ra_procedure.start_mac_order();
      }
      
      ra_procedure.step(tti);
      //phr_procedure.step(tti);
      
      // Receive PCH, if requested
      receive_pch(tti);
      
      // Process DL grants always 
      process_dl_grants(tti);

      // Send pending HARQ ACK, if any, and contention resolution is resolved
      if (dl_harq.is_ack_pending_resolution()) {
        ra_procedure.step(tti);
        if (ra_procedure.is_successful() || ra_procedure.is_response_error()) {
          Info("Sending pending ACK for contention resolution PHY TTI: %d\n", phy_h->get_current_tti());
          dl_harq.send_pending_ack_contention_resolution();
        }
      }

      // Process UL grants if RA procedure is done and we have pending data or in contention resolution 
      if (ra_procedure.is_contention_resolution() || ra_procedure.is_successful()) {
        process_ul_grants(tti);
      }
      
      timers_db.step_all();
      
      // Check if there is pending CCCH SDU in Multiplexing Unit
      if (mux_unit.is_pending_ccch_sdu()) {
        // Start RA procedure 
        if (!ra_procedure.in_progress() && !ra_procedure.is_successful()) {
          Info("Starting RA procedure by RLC order\n");
          ra_procedure.start_rlc_order();        
        }
      }
      if (ra_procedure.is_successful() && phy_rnti != params_db.get_param(mac_params::RNTI_C) && params_db.get_param(mac_params::RNTI_C) > 0) {
        phy_rnti = params_db.get_param(mac_params::RNTI_C);
                // This operation takes a while, do nothing for the rest 100 slots to re-align with PHY 
        for (int i=0;i<10;i++) {
          tti = ttisync->wait();
        }
        Info("Setting PHY RNTI=%d\n", phy_rnti);
        phy_h->set_crnti(phy_rnti);          
        for (int i=0;i<100;i++) {
          tti = ttisync->wait();
        }
      }
    }
  }  
}

void mac::add_sdu_handler(sdu_handler *handler) {
  demux_unit.add_sdu_handler(handler);
}

void mac::setup_timers()
{
  if (params_db.get_param(mac_params::TIMER_TIMEALIGN) > 0) {
    timers_db.get(TIME_ALIGNMENT)->set(this, params_db.get_param(mac_params::TIMER_TIMEALIGN));
  }
}
static sched_grant::rnti_type_t rnti_type(int rnti_param_id) {
  switch(rnti_param_id) {
    case mac_params::RNTI_C: 
      return sched_grant::RNTI_TYPE_CRNTI;
    case mac_params::RNTI_TEMP: 
      return sched_grant::RNTI_TYPE_TEMP;
    case mac_params::RNTI_SPS: 
      return sched_grant::RNTI_TYPE_SPS;
    case mac_params::RNTI_RA: 
      return sched_grant::RNTI_TYPE_RA;
  }
}

uint32_t mac::get_harq_sps_pid(uint32_t tti) {
  uint32_t nof_proc = ((uint32_t) params_db.get_param(mac_params::SPS_DL_NOF_PROC));
  return tti/params_db.get_param(mac_params::SPS_DL_SCHED_INTERVAL)%nof_proc;

}

void mac::timer_expired(uint32_t timer_id)
{
  switch(timer_id) {
    case TIME_ALIGNMENT:
      timeAlignmentTimerExpire();
      break;
    default: 
      break;
  }
}

/* Function called on expiry of TimeAlignmentTimer */
void mac::timeAlignmentTimerExpire() {
  dl_harq.reset();
  ul_harq.reset();
  dl_sps_assig.clear();
  ul_sps_assig.clear();
}

// Receive PCH when requested as defined in Section 5.5
void mac::receive_pch(uint32_t tti) {
  if (params_db.get_param(mac_params::PCCH_RECEIVE)) {
    dl_buffer *dl_buffer = phy_h->get_dl_buffer(tti); 
    dl_sched_grant pch_grant(sched_grant::RNTI_TYPE_PRNTI, SRSLTE_PRNTI); 
    if (dl_buffer->get_dl_grant(&pch_grant)) {
      qbuff *pcch_buff = mac_io_lch.get(mac_io::MAC_LCH_PCCH_DL); 
      uint8_t *ptr = (uint8_t*) pcch_buff->request(); 
      if (ptr && pch_grant.get_tbs() <= mac_io_lch.DEFAULT_MSG_SZ) {
        if (dl_buffer->decode_data(&pch_grant, ptr)) {
          pcch_buff->release();
        } else {
          Warning("Error decoding PCH\n");
        }
      } else {
        Error("Error getting pointer from PCCH buffer\n");
      }
    } else {
      Debug("No P-RNTI grant found while looking for PCH messages\n");
    }
  }
}

/* This function controls DL Grant Assignment as specified in Section 5.3.1 in 36.321 
 * and issues commands to DL harq operation 
 */
void mac::process_dl_grants(uint32_t tti) {
  // Get DL buffer for this TTI
  dl_buffer *dl_buffer = phy_h->get_dl_buffer(tti); 
  
  // 5.3.1 DL Assignment reception
  for (int i = mac_params::RNTI_C;i<=mac_params::RNTI_RA;i++) {
    // Check C-RNTI, SPS-RNTI and Temporal RNTI
    if (params_db.get_param(i) != 0) {
      dl_sched_grant ue_grant(rnti_type(i), params_db.get_param(i)); 
      if (dl_buffer->get_dl_grant(&ue_grant)) {
        // If PDCCH for C-RNTI and RA procedure in Contention Resolution, notify it
        if (ra_procedure.is_contention_resolution() && i == mac_params::RNTI_C) {
          ra_procedure.pdcch_to_crnti(false);
        }
        if (i != mac_params::RNTI_SPS) {
          uint32_t harq_pid = ue_grant.get_harq_process(); 
          if (i == mac_params::RNTI_TEMP && last_temporal_crnti != params_db.get_param(i)) {
            ue_grant.set_ndi(true);
            last_temporal_crnti = params_db.get_param(i);
          }
          if (i == mac_params::RNTI_C && dl_harq.is_sps(harq_pid)) {
            ue_grant.set_ndi(true);
          }
          dl_harq.set_harq_info(harq_pid, &ue_grant);
          dl_harq.receive_data(tti, harq_pid, dl_buffer, phy_h);
        } else {
          uint32_t harq_pid = get_harq_sps_pid(tti); 
          if (ue_grant.get_ndi()) {
            ue_grant.set_ndi(false);
            dl_harq.set_harq_info(harq_pid, &ue_grant);
            dl_harq.receive_data(tti, harq_pid, dl_buffer, phy_h);
          } else {
            if (ue_grant.is_sps_release()) {
              dl_sps_assig.clear();
              if (timers_db.get(TIME_ALIGNMENT)->is_running()) {
                //phy_h->send_sps_ack();
                Warning("PHY Send SPS ACK not implemented\n");
              }
            } else {
              dl_sps_assig.reset(tti, &ue_grant);
              ue_grant.set_ndi(true);
              dl_harq.set_harq_info(harq_pid, &ue_grant);              
            }
          }
        }
      }      
    }
  }
  /* Process configured DL assingments (SPS) */
  dl_sched_grant *sps_grant = dl_sps_assig.get_pending_grant(tti);
  if (sps_grant != NULL) {
    Info("Processing SPS grant\n");
    uint32_t harq_pid = get_harq_sps_pid(tti);
    sps_grant->set_ndi(true);
    dl_harq.set_harq_info(harq_pid, sps_grant);
    dl_harq.receive_data(tti, harq_pid, dl_buffer, phy_h);
  }
  
  /* Process SI-RNTI */
  uint32_t si_window_start  = params_db.get_param(mac_params::BCCH_SI_WINDOW_ST); 
  uint32_t si_window_length = params_db.get_param(mac_params::BCCH_SI_WINDOW_LEN); 
  
  if (tti >= si_window_start && tti < (si_window_start + si_window_length)) {    
    // Exclude subf 5 and sfn%2==0 unless it's a SIB1 message (window_length=1) (This is defined in 36.331 Sec 5.2.3)
    if (!(phy_h->tti_to_subf(si_window_length) != 1 && 
          phy_h->tti_to_subf(si_window_start) == 5 && (phy_h->tti_to_SFN(tti)%2) == 0)) 
    {
      Info("Searching for DL grant for SI-RNTI window_st=%d, window_len=%d\n", si_window_start, si_window_length);
      dl_sched_grant si_grant(sched_grant::RNTI_TYPE_SIRNTI, SRSLTE_SIRNTI); 
      if (dl_buffer->get_dl_grant(&si_grant)) {
        uint32_t k; 
        if (phy_h->tti_to_subf(si_window_start) == 5) { // This is SIB1, k is different
          k = (phy_h->tti_to_SFN(tti)/2)%4; 
        } else {      
          k = phy_h->tti_to_subf(tti)%4; 
        }
        si_grant.set_rv(((uint32_t) ceilf((float)1.5*k))%4);
        Info("DL grant found, sending to HARQ with RV: %d\n", si_grant.get_rv());
        dl_harq.set_harq_info(dl_harq_entity::HARQ_BCCH_PID, &si_grant);
        dl_harq.receive_data(tti, dl_harq_entity::HARQ_BCCH_PID, dl_buffer, phy_h);
        params_db.set_param(mac_params::BCCH_SI_WINDOW_ST, 0);
        params_db.set_param(mac_params::BCCH_SI_WINDOW_LEN, 0);
      } else {
        Warning("DL grant not found\n");
      }
    } 
      
  }  
}

/* UL Grant reception and processin as defined in Section 5.4.1 in 36.321 */
bool mac::process_ul_grants(uint32_t tti) {
  // Get DL buffer for this TTI to look for DCI grants
  dl_buffer *dl_buffer = phy_h->get_dl_buffer(tti); 

  //if (timers_db.get(TIME_ALIGNMENT)->is_running()) {
  if (1) {
    for (int i = mac_params::RNTI_C;i<=mac_params::RNTI_TEMP;i++) {
      // Check C-RNTI, SPS-RNTI and Temporal C-RNTI
      if (params_db.get_param(i) != 0) {
        ul_sched_grant ul_grant(rnti_type(i), params_db.get_param(i)); 
        if (dl_buffer->get_ul_grant(&ul_grant)) {
          if (ul_grant.is_from_rar()) {
            dl_buffer->discard_pending_rar_grant();
          }
          if (ra_procedure.is_contention_resolution() && i == mac_params::RNTI_C) {
            ra_procedure.pdcch_to_crnti(true);
          }
          if (i == mac_params::RNTI_C || i == mac_params::RNTI_TEMP || ra_procedure.is_running()) {
            if (i == mac_params::RNTI_C && ul_harq.is_sps(tti)) {
              ul_grant.set_ndi(true);
            }
            ul_harq.run_tti(tti, &ul_grant, phy_h); 
            return true;
          }
          else if (i == mac_params::RNTI_SPS) {
            if (ul_grant.get_ndi()) {
              ul_grant.set_ndi(false);
              ul_harq.run_tti(tti, &ul_grant, phy_h);
            } else {
              if (ul_grant.is_sps_release()) {
                ul_sps_assig.clear();              
              } else {
                ul_sps_assig.reset(tti, &ul_grant);
                ul_grant.set_ndi(true);
                ul_harq.run_tti(tti, &ul_grant, phy_h);              
                return true;
              }
            }
          }
        }
      }
    }
    /* Process configured UL assingments (SPS) */
    ul_sched_grant *sps_grant = ul_sps_assig.get_pending_grant(tti);
    if (sps_grant != NULL) {
      sps_grant->set_ndi(true);
      ul_harq.run_tti(tti, sps_grant, phy_h);
      return true;
    }
  }
  ul_harq.run_tti(tti, phy_h);      
  return false; 
}


int mac::recv_sdu(uint32_t lcid, uint8_t* sdu_payload, uint32_t nbytes)
{
  if (lcid <= mac_io::MAC_LCH_PCCH_DL) {
    return mac_io_lch.get(lcid)->recv(sdu_payload, nbytes);
  } else {
    Error("Receiving SDU: Invalid lcid=%d\n", lcid);
    return -1; 
  }
}

int mac::recv_bcch_sdu(uint8_t* sdu_payload, uint32_t buffer_len_nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_BCCH_DL)->recv(sdu_payload, buffer_len_nbytes);   
}

int mac::recv_ccch_sdu(uint8_t* sdu_payload, uint32_t buffer_len_nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_CCCH_DL)->recv(sdu_payload, buffer_len_nbytes);   
}

int mac::recv_dtch0_sdu(uint8_t* sdu_payload, uint32_t buffer_len_nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_DTCH0_DL)->recv(sdu_payload, buffer_len_nbytes);   
}

int mac::recv_dcch0_sdu(uint8_t* sdu_payload, uint32_t buffer_len_nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_DCCH0_DL)->recv(sdu_payload, buffer_len_nbytes);   
}


bool mac::send_sdu(uint32_t lcid, uint8_t* sdu_payload, uint32_t nbytes)
{
  lcid += mac_io::MAC_LCH_CCCH_UL;
  if (lcid <= mac_io::MAC_LCH_DTCH2_UL) {
    return mac_io_lch.get(lcid)->send(sdu_payload, nbytes);
  } else {
    Error("Receiving SDU: Invalid lcid=%d\n", lcid);
    return -1; 
  }
}

bool mac::send_ccch_sdu(uint8_t* sdu_payload, uint32_t nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_CCCH_UL)->send(sdu_payload, nbytes);   
}

bool mac::send_dtch0_sdu(uint8_t* sdu_payload, uint32_t nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_DTCH0_UL)->send(sdu_payload, nbytes);   
}

bool mac::send_dcch0_sdu(uint8_t* sdu_payload, uint32_t nbytes)
{
  return mac_io_lch.get(mac_io::MAC_LCH_DCCH0_UL)->send(sdu_payload, nbytes);   
}

void mac::set_param(mac_params::mac_param_t param, int64_t value)
{
  params_db.set_param((uint32_t) param, value);
}

int64_t mac::get_param(mac_params::mac_param_t param)
{
  return params_db.get_param((uint32_t) param);
}

void mac::setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD)
{
  mux_unit.set_priority(mac_io::MAC_LCH_CCCH_UL+lcid, priority, PBR_x_tti, BSD);
  bsr_procedure.setup_lcg(lcid, lcg);
  bsr_procedure.set_priority(lcid, priority);
}


}
}
