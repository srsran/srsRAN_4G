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

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

#include "srslte/common/log.h"
#include "srsue/hdr/mac/mac.h"
#include "srslte/common/pcap.h"


namespace srsue {

mac::mac() : ttisync(10240), 
             timers(64),
             mux_unit(MAC_NOF_HARQ_PROC),
             pdu_process_thread(&demux_unit)
{
  started = false;
  pcap    = NULL;
  bzero(&metrics, sizeof(mac_metrics_t));
}

bool mac::init(phy_interface_mac *phy, rlc_interface_mac *rlc, rrc_interface_mac *rrc, srslte::log *log_h_)
{
  started = false;
  phy_h = phy;
  rlc_h = rlc;
  rrc_h = rrc;
  log_h = log_h_;
  tti = 0;
  is_synchronized = false;
  last_temporal_crnti = 0;
  phy_rnti = 0;

  srslte_softbuffer_rx_init(&pch_softbuffer, 100);

  timer_alignment             = timers.get_unique_id();
  contention_resolution_timer = timers.get_unique_id();

  bsr_procedure.init(       rlc_h, log_h,          &config,                &timers);
  phr_procedure.init(phy_h,        log_h,          &config,                &timers);
  mux_unit.init     (       rlc_h, log_h,                                              &bsr_procedure, &phr_procedure);
  demux_unit.init   (phy_h, rlc_h, log_h,                                  timers.get(timer_alignment));
  ra_procedure.init (phy_h, rrc,   log_h, &uernti, &config,                timers.get(timer_alignment), timers.get(contention_resolution_timer), &mux_unit, &demux_unit);
  sr_procedure.init (phy_h, rrc,   log_h,          &config);
  ul_harq.init      (              log_h, &uernti, &config.ul_harq_params, timers.get(contention_resolution_timer), &mux_unit);
  dl_harq.init      (              log_h,                                  timers.get(timer_alignment),             &demux_unit);

  reset();

  started = true;
  start(MAC_MAIN_THREAD_PRIO);

  mactimers.init(&timers, log_h);

  return started;
}

void mac::stop()
{
  srslte_softbuffer_rx_free(&pch_softbuffer);

  started = false;
  ttisync.increase();
  pdu_process_thread.stop();
  wait_thread_finish();
  mactimers.stop();
}

void mac::start_pcap(srslte::mac_pcap* pcap_)
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

void mac::wait_uplink() {
  int cnt=0;
  Info("Waiting to uplink...\n");
  while(mux_unit.is_pending_any_sdu() && cnt<20) {
    usleep(1000);
    cnt++;
  }
}

// Implement Section 5.9
void mac::reset()
{
  bzero(&metrics, sizeof(mac_metrics_t));

  Info("Resetting MAC\n");

  timers.get(timer_alignment)->stop();
  timers.get(contention_resolution_timer)->stop();

  ul_harq.reset_ndi();

  mux_unit.msg3_flush();
  mux_unit.reset();

  ra_procedure.reset();
  sr_procedure.reset();
  bsr_procedure.reset();
  phr_procedure.reset();

  dl_harq.reset();
  phy_h->pdcch_dl_search_reset();
  phy_h->pdcch_ul_search_reset();

  is_first_ul_grant = true;

  bzero(&uernti, sizeof(ue_rnti_t));
}

void mac::mac_timers::init(srslte::timers *timers, srslte::log *log_h) {
  this->timers = timers;
  running = true;
  this->log_h = log_h;
  start_periodic(1000);
}

void mac::mac_timers::run_period() {
  timers->step_all();
}

void mac::run_thread() {
  int cnt=0;

  while (!phy_h->sync_status() && started) {
    usleep(5000);
    if (phy_h->sync_status()) {
      Debug("Setting ttysync to %d\n", phy_h->get_current_tti());
      ttisync.set_producer_cntr(phy_h->get_current_tti());
    }
  }

  while(started) {

    /* Warning: Here order of invocation of procedures is important!! */
    tti = ttisync.wait();

    log_h->step(tti);

    // Step all procedures
    bsr_procedure.step(tti);
    phr_procedure.step(tti);

    // Check if BSR procedure need to start SR

    if (bsr_procedure.need_to_send_sr(tti)) {
      Debug("Starting SR procedure by BSR request, PHY TTI=%d\n", tti);
      sr_procedure.start();
    }
    if (bsr_procedure.need_to_reset_sr()) {
      Debug("Resetting SR procedure by BSR request\n");
      sr_procedure.reset();
    }
    sr_procedure.step(tti);

    // Check SR if we need to start RA
    if (sr_procedure.need_random_access()) {
      ra_procedure.start_mac_order();
    }
    ra_procedure.step(tti);

    rrc_h->run_tti(tti);
  }
}

void mac::bcch_start_rx()
{
  bcch_start_rx(tti, -1);
}

void mac::bcch_start_rx(int si_window_start, int si_window_length)
{
  if (si_window_length >= 0 && si_window_start >= 0) {
    dl_harq.set_si_window_start(si_window_start);
    phy_h->pdcch_dl_search(SRSLTE_RNTI_SI, SRSLTE_SIRNTI, si_window_start, si_window_start+si_window_length);
  } else {
    phy_h->pdcch_dl_search(SRSLTE_RNTI_SI, SRSLTE_SIRNTI, si_window_start);
  }
  Info("SCHED: Searching for DL grant for SI-RNTI window_st=%d, window_len=%d\n", si_window_start, si_window_length);
}

void mac::bcch_stop_rx()
{
  phy_h->pdcch_dl_search_reset();
}

void mac::pcch_start_rx()
{
  phy_h->pdcch_dl_search(SRSLTE_RNTI_PCH, SRSLTE_PRNTI);
  Info("SCHED: Searching for DL grant for P-RNTI\n");
}

void mac::pcch_stop_rx()
{
  phy_h->pdcch_dl_search_reset();
}


void mac::tti_clock(uint32_t tti)
{
  ttisync.increase(tti);
}

void mac::bch_decoded_ok(uint8_t* payload, uint32_t len)
{
  // Send MIB to RLC
  rlc_h->write_pdu_bcch_bch(payload, len);

  if (pcap) {
    pcap->write_dl_bch(payload, len, true, phy_h->get_current_tti());
  }
}

void mac::pch_decoded_ok(uint32_t len)
{
  // Send PCH payload to RLC
  rlc_h->write_pdu_pcch(pch_payload_buffer, len);

  if (pcap) {
    pcap->write_dl_pch(pch_payload_buffer, len, true, phy_h->get_current_tti());
  }
}

void mac::tb_decoded(bool ack, uint32_t tb_idx, srslte_rnti_type_t rnti_type, uint32_t harq_pid)
{
  if (rnti_type == SRSLTE_RNTI_RAR) {
    if (ack) {
      ra_procedure.tb_decoded_ok();
    }
  } else {
    dl_harq.tb_decoded(ack, tb_idx, rnti_type, harq_pid);
    if (ack) {
      pdu_process_thread.notify();
      metrics.rx_brate += dl_harq.get_current_tbs(harq_pid, tb_idx);
    } else {
      metrics.rx_errors++;
    }
    metrics.rx_pkts++;
  }
}

void mac::new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action)
{
  if (grant.rnti_type == SRSLTE_RNTI_RAR) {
    ra_procedure.new_grant_dl(grant, action);
  } else if (grant.rnti_type == SRSLTE_RNTI_PCH) {

    memcpy(&action->phy_grant, &grant.phy_grant, sizeof(srslte_phy_grant_t));
    action->generate_ack = false;
    action->decode_enabled[0] = true;
    action->decode_enabled[1] = false;
    srslte_softbuffer_rx_reset_cb(&pch_softbuffer, 1);
    action->payload_ptr[0] = pch_payload_buffer;
    action->softbuffers[0]  = &pch_softbuffer;
    action->rnti = grant.rnti;
    action->rv[0]   = grant.rv[0];
    if (grant.n_bytes[0] > pch_payload_buffer_sz) {
      Error("Received grant for PCH (%d bytes) exceeds buffer (%d bytes)\n", grant.n_bytes[0], pch_payload_buffer_sz);
      action->decode_enabled[0] = false;
    }
  } else {
    // If PDCCH for C-RNTI and RA procedure in Contention Resolution, notify it
    if (grant.rnti_type == SRSLTE_RNTI_USER && ra_procedure.is_contention_resolution()) {
      ra_procedure.pdcch_to_crnti(false);
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
  /* Start PHR Periodic timer on first UL grant */
  if (is_first_ul_grant) {
    is_first_ul_grant = false;
    phr_procedure.start_timer();
  }
  if (grant.rnti_type == SRSLTE_RNTI_USER && ra_procedure.is_contention_resolution()) {
    ra_procedure.pdcch_to_crnti(true);
  }
  ul_harq.new_grant_ul(grant, action);
  metrics.tx_pkts++;
}

void mac::new_grant_ul_ack(mac_interface_phy::mac_grant_t grant, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  int tbs = ul_harq.get_current_tbs(tti);
  ul_harq.new_grant_ul_ack(grant, &ack, action);
  if (!ack) {
    metrics.tx_errors++;
  } else {
    metrics.tx_brate += tbs;
  }
  metrics.tx_pkts++;
  if (!ack && ra_procedure.is_contention_resolution()) {
    ra_procedure.harq_retx();
  }
  if (grant.rnti_type == SRSLTE_RNTI_USER && ra_procedure.is_contention_resolution()) {
    ra_procedure.pdcch_to_crnti(true);
  }
}

void mac::harq_recv(uint32_t tti, bool ack, mac_interface_phy::tb_action_ul_t* action)
{
  int tbs = ul_harq.get_current_tbs(tti);
  ul_harq.harq_recv(tti, ack, action);
  if (!ack) {
    metrics.tx_errors++;
    metrics.tx_pkts++;
  } else {
    metrics.tx_brate += tbs;
  }
  if (!ack && ra_procedure.is_contention_resolution()) {
    ra_procedure.harq_retx();
  }
}

void mac::setup_timers()
{
  int value = liblte_rrc_time_alignment_timer_num[config.main.time_alignment_timer];
  if (value > 0) {
    timers.get(timer_alignment)->set(this, value);
  }
}

void mac::timer_expired(uint32_t timer_id)
{
  if(timer_id == timer_alignment) {
    timer_alignment_expire();
  } else {
    Warning("Received callback from unknown timer_id=%d\n", timer_id);
  }
}

/* Function called on expiry of TimeAlignmentTimer */
void mac::timer_alignment_expire()
{
  printf("TimeAlignment timer has expired value=%d ms\n", timers.get(timer_alignment)->get_timeout());
  rrc_h->release_pucch_srs();
  dl_harq.reset();
  ul_harq.reset();
}

void mac::get_rntis(ue_rnti_t* rntis)
{
  memcpy(rntis, &uernti, sizeof(ue_rnti_t));
}

void mac::set_ho_rnti(uint16_t crnti, uint16_t target_pci) {
  phy_h->pdcch_dl_search_reset();
  phy_h->pdcch_ul_search_reset();
  uernti.crnti = crnti;
  if (pcap) {
    pcap->set_ue_id(target_pci);
  }
}

void mac::set_contention_id(uint64_t uecri)
{
  uernti.contention_id = uecri;
}

void mac::start_noncont_ho(uint32_t preamble_index, uint32_t prach_mask)
{
  ra_procedure.start_noncont(preamble_index, prach_mask);
}

void mac::start_cont_ho()
{
  ra_procedure.start_mac_order(56, true);
}

void mac::get_config(mac_cfg_t* mac_cfg)
{
  memcpy(mac_cfg, &config, sizeof(mac_cfg_t));
}

void mac::set_config(mac_cfg_t* mac_cfg)
{
  memcpy(&config, mac_cfg, sizeof(mac_cfg_t));
  setup_timers();
}

void mac::set_config_main(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT* main_cfg)
{
  memcpy(&config.main, main_cfg, sizeof(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT));
  setup_timers();
}

void mac::set_config_rach(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT* rach_cfg, uint32_t prach_config_index)
{
  memcpy(&config.rach, rach_cfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
  config.prach_config_index = prach_config_index;
}

void mac::set_config_sr(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT* sr_cfg)
{
  memcpy(&config.sr, sr_cfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
}

void mac::setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD)
{
  Info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSd=%d\n",
       lcid, lcg, priority, PBR_x_tti, BSD);
  mux_unit.set_priority(lcid, priority, PBR_x_tti, BSD);
  bsr_procedure.setup_lcg(lcid, lcg);
  bsr_procedure.set_priority(lcid, priority);
}

void mac::get_metrics(mac_metrics_t &m)
{
  Info("DL retx: %.2f \%%, perpkt: %.2f, UL retx: %.2f \%% perpkt: %.2f\n", 
       metrics.rx_pkts?((float) 100*metrics.rx_errors/metrics.rx_pkts):0.0, 
       dl_harq.get_average_retx(),
       metrics.tx_pkts?((float) 100*metrics.tx_errors/metrics.tx_pkts):0.0, 
       ul_harq.get_average_retx());
  
  metrics.ul_buffer = (int) bsr_procedure.get_buffer_state();
  m = metrics;  
  bzero(&metrics, sizeof(mac_metrics_t));  
}




/********************************************************
 *
 * Interface for timers used by upper layers
 *
 *******************************************************/
srslte::timers::timer* mac::timer_get(uint32_t timer_id)
{
  return timers.get(timer_id);
}

void mac::timer_release_id(uint32_t timer_id)
{
  timers.release_id(timer_id);
}

uint32_t mac::timer_get_unique_id()
{
  return timers.get_unique_id();
}


/********************************************************
 *
 * Class that runs a thread to process DL MAC PDUs from
 * DEMUX unit
 *
 *******************************************************/
mac::pdu_process::pdu_process(demux *demux_unit_)
{
  demux_unit = demux_unit_;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cvar, NULL);
  have_data = false; 
  start(MAC_PDU_THREAD_PRIO);  
}

void mac::pdu_process::stop()
{
  pthread_mutex_lock(&mutex);
  running = false; 
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
  
  wait_thread_finish();
}

void mac::pdu_process::notify()
{
  pthread_mutex_lock(&mutex);
  have_data = true; 
  pthread_cond_signal(&cvar);
  pthread_mutex_unlock(&mutex);
}

void mac::pdu_process::run_thread()
{
  running = true; 
  while(running) {
    have_data = demux_unit->process_pdus();
    if (!have_data) {
      pthread_mutex_lock(&mutex);
      while(!have_data && running) {
        pthread_cond_wait(&cvar, &mutex);
      }
      pthread_mutex_unlock(&mutex);
    }
  }
}







}



