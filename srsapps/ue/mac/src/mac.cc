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

#define Error(fmt, ...)   log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

#include "srsapps/common/threads.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/mac/mac.h"
#include "srsapps/ue/mac/mac_params.h"
#include "srsapps/ue/mac/pcap.h"

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
  is_first_tx = true; 
  
  bsr_procedure.init(log_h, &timers_db, &params_db, &mac_io_lch);
  mux_unit.init(log_h, &mac_io_lch, &bsr_procedure);
  demux_unit.init(phy_h, log_h, &mac_io_lch, &timers_db);
  ra_procedure.init(&params_db, phy_h, log_h, &timers_db, &mux_unit, &demux_unit);
  sr_procedure.init(log_h, &params_db, phy_h);
  is_first_of_burst = true; 
  reset();

  for (int i=0;i<NOF_TTI_THREADS;i++) {
    pthread_mutex_init(&tti_threads_sync_tx[i], NULL); 
    tti_threads_sync[i].set_increment(NOF_TTI_THREADS);    
    if (!tti_threads[i].init(this, &tti_threads_sync[i])) {
      return false; 
    }
  }

  if (threads_new_rt_prio(&mac_thread, mac_thread_fnc, this, 1)) {
    started = true; 
  }
  
  return started; 
}

void mac::stop()
{
  started = false;   
  pthread_join(mac_thread, NULL);
  for (int i=0;i<NOF_TTI_THREADS;i++) {
    tti_threads[i].stop();
    pthread_mutex_destroy(&tti_threads_sync_tx[i]);
  }
}

int mac::get_tti()
{
  if (is_synchronized) {
    return (int) tti; 
  } else {
    return -1;
  }
}

void mac::start_pcap(mac_pcap* pcap_)
{
  pcap = pcap_; 
  dl_harq.start_pcap(pcap);
  ul_harq.start_pcap(pcap);
  ra_procedure.start_pcap(pcap);
}

void mac::start_trace()
{
  tr_enabled = true; 
}

void mac::write_trace(std::string filename)
{
  tr_exec_total.writeToBinary(filename + ".total");
  tr_exec_dl.writeToBinary(filename + ".dl");
  tr_exec_ul.writeToBinary(filename + ".ul");
}

void mac::tr_log_start(uint32_t tti)
{
  if (tr_enabled) {
    gettimeofday(&tr_time_total[1], NULL);
  }
}

void mac::tr_log_end(uint32_t tti)
{
  if (tr_enabled) {
    /* compute total execution time */
    gettimeofday(&tr_time_total[2], NULL);
    get_time_interval(tr_time_total);
    tr_exec_total.push(tti, tr_time_total[0].tv_usec);
    
    /* ul execution time is from the call to tr_log_ul */ 
    memcpy(&tr_time_ul[2], &tr_time_total[2], sizeof(struct timeval));
    get_time_interval(tr_time_ul);
    tr_exec_ul.push(tti, tr_time_ul[0].tv_usec);
  }
}

void mac::tr_log_ul(uint32_t tti)
{
  if (tr_enabled) {
    /* DL execution time is from the call to tr_log_dl to the call to tr_log_ul */
    gettimeofday(&tr_time_dl[2], NULL);
    get_time_interval(tr_time_dl);
    tr_exec_dl.push(tti, tr_time_dl[0].tv_usec);
    
    memcpy(&tr_time_ul[1], &tr_time_dl[2], sizeof(struct timeval));
  }
}

void mac::tr_log_dl(uint32_t tti)
{
  if (tr_enabled) {
    gettimeofday(&tr_time_dl[1], NULL);    
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
  while(started) {
    if (!is_synchronized) {      
      srslte_cell_t cell; 
      uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
      
      if (phy_h->decode_mib_best(&cell, bch_payload)) {
        
        // Print MIB 
        srslte_cell_fprint(stdout, &cell, phy_h->get_current_tti()/10);

        if (pcap) {
          pcap->write_dl_bch(bch_payload, SRSLTE_BCH_PAYLOAD_LEN/8, true, phy_h->get_current_tti());
        }
        
        // Init HARQ for this cell 
        Info("Init UL/DL HARQ\n");
        ul_harq.init(cell, &params_db, log_h, &timers_db, &mux_unit);
        dl_harq.init(cell, 1024*1024, log_h, &timers_db, &demux_unit);

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
            for (int i=0;i<NOF_TTI_THREADS;i++) {
              tti_threads_sync[i].set_producer_cntr(tti+i);
              tti_threads_sync[i].resync();
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
      tti = ttisync->wait();
      tr_log_start(tti);
      log_h->step(tti);
      
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
      
      ra_procedure.step(tti);
      //phr_procedure.step(tti);
      
      timers_db.step_all();
    
      // Trigger execution of corresponding TTI processor thread
      //printf("triggering tti=%d\n", tti);
      tti_threads_sync[tti%NOF_TTI_THREADS].increase();
      //tti_threads[0].run_tti(tti);
      
    }
  }  
}


struct phy_crnti {
  phy *phy_ptr; 
  log *log_h; 
  uint16_t crnti;   
};

void *pregen_phy_thread(void *arg) {
  struct phy_crnti *a = (struct phy_crnti*) arg;
  a->log_h->info("Setting PHY RNTI=%d\n", a->crnti);
  a->phy_ptr->set_crnti(a->crnti);
  a->phy_ptr->pregen_signals();
  a->log_h->info("Done Setting PHY RNTI\n");
  free(a);
  return NULL; 
}

void mac::pregen_phy(uint16_t phy_rnti)
{
  pthread_t rnti_thread; 
  struct phy_crnti *arg = (struct phy_crnti*) malloc(sizeof(struct phy_crnti));
  arg->crnti = phy_rnti;
  arg->phy_ptr = phy_h; 
  arg->log_h = log_h; 
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  if (pthread_create(&rnti_thread, &attr, pregen_phy_thread, arg)) {
    perror("pthread_create");
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
            Info("Set NDI=1 for Temp-RNTI DL grant\n");
            last_temporal_crnti = params_db.get_param(i);
          }
          if (i == mac_params::RNTI_C && dl_harq.is_sps(harq_pid)) {
            ue_grant.set_ndi(true);
            Info("Set NDI=1 for C-RNTI DL grant\n");
          }
          dl_harq.set_harq_info(harq_pid, &ue_grant);
          dl_harq.receive_data(tti, harq_pid, dl_buffer, phy_h);
        } else {
          /* This is for SPS scheduling */
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
      Debug("Searching for DL grant for SI-RNTI window_st=%d, window_len=%d\n", si_window_start, si_window_length);
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
  Info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSd=%d\n", lcid, lcg, priority, PBR_x_tti, BSD);
  mux_unit.set_priority(lcid, priority, PBR_x_tti, BSD);
  bsr_procedure.setup_lcg(lcid, lcg);
  bsr_procedure.set_priority(lcid, priority);
}


void* tti_thread_runner(void *arg) {
  mac::tti_thread* x = (mac::tti_thread*) arg;
  x->run();
  return NULL; 
}

bool mac::tti_thread::init(mac* parent_, tti_sync_cv *sync_)
{
  parent = parent_; 
  log_h  = parent->log_h;
  sync  = sync_; 
  
  started = threads_new_rt(&thread, tti_thread_runner, this);
  return started; 
}

void mac::tti_thread::stop()
{
  started = false; 
  pthread_join(thread, NULL);
}

void mac::tti_thread::run()
{
  while(started) {
    uint32_t tti = sync->wait();

    if (parent->is_synchronized) {
      run_tti((tti+1)%10240); 
    }
  }
}

void mac::tti_thread::run_tti(uint32_t tti) {
  //printf("running tti=%d\n", tti);
  // Receive PCH, if requested
  parent->receive_pch(tti);
  
  // Process DL grants always 
  parent->process_dl_grants(tti);

  // Send pending HARQ ACK, if any, and contention resolution is resolved
  if (parent->dl_harq.is_ack_pending_resolution()) {
    parent->ra_procedure.step(tti);
    if (parent->ra_procedure.is_successful() || parent->ra_procedure.is_response_error()) {
      Info("Sending pending ACK for contention resolution PHY TTI: %d\n", parent->phy_h->get_current_tti());
      parent->dl_harq.send_pending_ack_contention_resolution();
    }
  }

  // Process UL grants if RA procedure is done or in contention resolution 
  if (parent->ra_procedure.is_contention_resolution() || parent->ra_procedure.is_successful()) {
    parent->process_ul_grants(tti);
  }

  // If ACK/SR was pending but there was no PUSCH transmission, transmit now through PUCCH
  ul_buffer *ul_buffer = parent->phy_h->get_ul_buffer(tti+4);
  
  // Generate scheduling request if we have to 
  if (parent->phy_h->sr_is_ready_to_send(tti+4)) {
    ul_buffer->generate_sr();
  }

  // If the packet was not generated by an UL grant, means it's PUCCH or SRS. Generate now the signal
  if (!ul_buffer->is_released() && (ul_buffer->uci_ready() || ul_buffer->srs_is_ready_to_send())) {
    ul_buffer->generate_data();
  }
  
  // Wait for previous TTI to be transmitted
  if (!parent->is_first_tx) {
    pthread_mutex_lock(&parent->tti_threads_sync_tx[tti%parent->NOF_TTI_THREADS]);
  }
  parent->is_first_tx = false; 
  
  // Send now to the radio
  if (ul_buffer->is_released()) {
    ul_buffer->send();
    ul_buffer->ready();
    parent->is_first_of_burst = false; 
  } else {
    if (!parent->is_first_of_burst) {
      ul_buffer->send_end_of_burst();
      parent->is_first_of_burst = true; 
    }
  }
  
  // Allow next TTI to be transmitted
  pthread_mutex_unlock(&parent->tti_threads_sync_tx[(tti+1)%parent->NOF_TTI_THREADS]);
  
  // Check if there is pending CCCH SDU in Multiplexing Unit
  if (parent->mux_unit.is_pending_ccch_sdu()) {
    // Start RA procedure 
    if (!parent->ra_procedure.in_progress() && !parent->ra_procedure.is_successful()) {
      parent->ra_procedure.start_rlc_order();        
    }
  }
  if (parent->ra_procedure.is_successful()                      && 
    parent->phy_rnti != parent->params_db.get_param(mac_params::RNTI_C) && 
    parent->params_db.get_param(mac_params::RNTI_C) > 0         && 
    parent->phy_h->get_param(srslte::ue::phy_params::SRS_IS_CONFIGURED) == 1) 
  {
    parent->phy_rnti = parent->params_db.get_param(mac_params::RNTI_C);
    parent->pregen_phy(parent->phy_rnti);
  }
}
}
}
