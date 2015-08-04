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

#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/pcap.h"

namespace srslte {
namespace ue {

mac::mac() : ttisync(10240), timers_db((uint32_t) NOF_MAC_TIMERS) 
{
  started = false;  
  pcap    = NULL;   
  si_search_in_progress = false; 
  si_window_length = -1; 
  si_window_start  = -1; 
  signals_pregenerated = false; 
}
  
bool mac::init(phy_interface *phy, rlc_interface_mac *rlc, log *log_h_)
{
  started = false; 
  phy_h = phy;
  rlc_h = rlc; 
  log_h = log_h_; 
  tti = 0; 
  is_synchronized = false;   
  last_temporal_crnti = 0; 
  phy_rnti = 0; 
  
  bsr_procedure.init(       rlc_h, log_h, &params_db, &timers_db);
  mux_unit.init     (       rlc_h, log_h,                          &bsr_procedure);
  demux_unit.init   (phy_h, rlc_h, log_h,             &timers_db);
  ra_procedure.init (phy_h,        log_h, &params_db, &timers_db, &mux_unit, &demux_unit);
  sr_procedure.init (phy_h,        log_h, &params_db);
  ul_harq.init      (              log_h, &params_db, &timers_db, &mux_unit);
  dl_harq.init      (              log_h, &params_db, &timers_db, &demux_unit);

  reset();

  started = true; 
  start(MAC_THREAD_PRIO);
  
  return started; 
}

void mac::stop()
{
  started = false;   
  wait_thread_finish();
}

void mac::start_pcap(mac_pcap* pcap_)
{
  pcap = pcap_; 
  dl_harq.start_pcap(pcap);
  ul_harq.start_pcap(pcap);
  ra_procedure.start_pcap(pcap);
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
  phy_h->pdcch_dl_search_reset();
  phy_h->pdcch_ul_search_reset();
  
  signals_pregenerated = false; 
  
  params_db.set_param(mac_interface_params::BCCH_SI_WINDOW_ST, -1);
  params_db.set_param(mac_interface_params::BCCH_SI_WINDOW_LEN, -1);
}

void mac::run_thread() {
  setup_timers();

  Info("Waiting PHY to synchronize with cell\n");  
  phy_h->sync_start();
  while(!phy_h->get_current_tti()) {
    usleep(50000);
  }
  Debug("Setting ttysync to %d\n", phy_h->get_current_tti());
  ttisync.set_producer_cntr(phy_h->get_current_tti());
     
  while(started) {

    /* Warning: Here order of invocation of procedures is important!! */
    tti = (ttisync.wait() + 1)%10240;
    
    log_h->step(tti);
      
    search_si_rnti();
    
    // Step all procedures 
    bsr_procedure.step(tti);
    
    // Check if BSR procedure need to start SR 
    
    if (bsr_procedure.need_to_send_sr()) {
      Debug("Starting SR procedure by BSR request, PHY TTI=%d\n", phy_h->get_current_tti());
      sr_procedure.start();
    }
    if (bsr_procedure.need_to_reset_sr()) {
      Debug("Resetting SR procedure by BSR request\n");
      sr_procedure.reset();
    }
    sr_procedure.step(tti);

    // Check SR if we need to start RA 
    if (sr_procedure.need_random_access()) {
      Warning("Starting RA procedure by MAC order is DISABLED\n");
      //ra_procedure.start_mac_order();
    }
    
    // Check if there is pending CCCH SDU in Mux unit 
    if (mux_unit.is_pending_ccch_sdu()) {
      if (!ra_procedure.in_progress() && !ra_procedure.is_successful()) {
        ra_procedure.start_rlc_order();
      }
    }
    
    ra_procedure.step(tti);
    //phr_procedure.step(tti);

    // FIXME: Do here DTX and look for UL grants only when needed
    if (ra_procedure.is_successful() && !signals_pregenerated) {
      // Configure PHY to look for UL C-RNTI grants
      uint16_t crnti = params_db.get_param(mac_interface_params::RNTI_C);
      phy_h->pdcch_ul_search(SRSLTE_RNTI_USER, crnti);
      phy_h->pdcch_dl_search(SRSLTE_RNTI_USER, crnti);
      
      // Pregenerate UL signals and C-RNTI scrambling sequences
      Info("Pre-generating UL signals and C-RNTI scrambling sequences\n");
      ((phy*) phy_h)->enable_pregen_signals(true);
      ((phy*) phy_h)->set_crnti(crnti);
      phy_h->configure_ul_params();
      Info("Done\n");
      signals_pregenerated = true; 
    }
    
    timers_db.step_all();
    
    demux_unit.process_pdus();
  }  
}

void mac::search_si_rnti() 
{

  // Setup PDCCH search
  int _si_window_start  = params_db.get_param(mac_interface_params::BCCH_SI_WINDOW_ST); 
  int _si_window_length = params_db.get_param(mac_interface_params::BCCH_SI_WINDOW_LEN); 
  
  if ((tti >= si_window_start + si_window_length) && si_search_in_progress) {
    si_search_in_progress = false; 
    phy_h->pdcch_dl_search_reset();
    Debug("SI search window expired (%d >= %d+%d)\n", tti, si_window_start, si_window_length);
    params_db.set_param(mac_interface_params::BCCH_SI_WINDOW_ST, -1);
    params_db.set_param(mac_interface_params::BCCH_SI_WINDOW_LEN, -1);
  }

  if (_si_window_length > 0 && _si_window_start >= 0 && !si_search_in_progress) {     
    si_window_length = _si_window_length;
    si_window_start  = _si_window_start;
    Debug("Searching for DL grant for SI-RNTI window_st=%d, window_len=%d\n", si_window_start, si_window_length);
    phy_h->pdcch_dl_search(SRSLTE_RNTI_SI, 0xffff, si_window_start, si_window_start+si_window_length);
    si_search_in_progress = true; 
  } 
  if ((_si_window_length < 0 || _si_window_start < 0) && si_search_in_progress) {
    phy_h->pdcch_dl_search_reset();
    si_search_in_progress = false; 
    Debug("SI search interrupted by higher layers\n");
  }
}

void mac::tti_clock(uint32_t tti)
{
  ttisync.increase();
}

void mac::bch_decoded_ok(uint8_t* payload, uint32_t len)
{
  // Send MIB to RRC 
  rlc_h->write_pdu_bcch_bch(payload, len);
  
  if (pcap) {
    pcap->write_dl_bch(payload, len, true, phy_h->get_current_tti());
  }
}

void mac::harq_recv(uint32_t tti, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  ul_harq.harq_recv(tti, ack, action);
}

void mac::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action)
{
  if (grant.rnti_type == SRSLTE_RNTI_RAR) {
    ra_procedure.new_grant_dl(grant, action);
  } else {
    // If PDCCH for C-RNTI and RA procedure in Contention Resolution, notify it
    if (grant.rnti_type == SRSLTE_RNTI_USER) {
      if (ra_procedure.is_contention_resolution()) {
        ra_procedure.pdcch_to_crnti(false);
      }
    }
    dl_harq.new_grant_dl(grant, action);
  }
}

uint32_t mac::get_current_tti()
{
  return phy_h->get_current_tti();
}

void mac::new_grant_ul(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_ul_t* action)
{
  if (grant.rnti_type == SRSLTE_RNTI_USER) {
    if (ra_procedure.is_contention_resolution()) {
      ra_procedure.pdcch_to_crnti(true);
    }
  }
  ul_harq.new_grant_ul(grant, action);
}

void mac::new_grant_ul_ack(mac_interface_phy::mac_grant_t grant, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  ul_harq.new_grant_ul_ack(grant, ack, action);
}

void mac::tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid)
{
  if (rnti_type == SRSLTE_RNTI_RAR) {
    if (ack) {
      ra_procedure.tb_decoded_ok();
    }
  } else {
    dl_harq.tb_decoded(ack, rnti_type, harq_pid);
  }
}


void mac::setup_timers()
{
  if (params_db.get_param(mac_interface_params::TIMER_TIMEALIGN) > 0) {
    timers_db.get(TIME_ALIGNMENT)->set(this, params_db.get_param(mac_interface_params::TIMER_TIMEALIGN));
  }
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
void mac::timeAlignmentTimerExpire() 
{
  dl_harq.reset();
  ul_harq.reset();
}

void mac::set_param(mac_interface_params::mac_param_t param, int64_t value)
{
  params_db.set_param((uint32_t) param, value);
}

int64_t mac::get_param(mac_interface_params::mac_param_t param)
{
  return params_db.get_param((uint32_t) param);
}

void mac::setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD)
{
  Info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSd=%d\n", 
       lcid, lcg, priority, PBR_x_tti, BSD);
  mux_unit.set_priority(lcid, priority, PBR_x_tti, BSD);
  bsr_procedure.setup_lcg(lcid, lcg);
  bsr_procedure.set_priority(lcid, priority);
}

}
}


