/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

#include "srslte/srslte.h"

#include "srsapps/common/threads.h"
#include "srsapps/common/log.h"
#include "srsapps/ue/phy/phy.h"
#include "srsapps/ue/phy/prach.h"
#include "srsapps/ue/phy/ul_buffer.h"
#include "srsapps/ue/phy/dl_buffer.h"

namespace srslte {
namespace ue {

phy::phy() : tr_end_time(1024*10), tr_start_time(1024*10)
{
  started = false; 
  is_sfn_synched = false; 
  cell_is_set = false; 
  phy_state = IDLE; 
}

bool phy::init(srslte::radio* radio_handler_, srslte::ue::tti_sync* ttisync_, log *log_h) {
  return init_(radio_handler_, ttisync_, log_h, false);
}

bool phy::init_agc(srslte::radio* radio_handler_, srslte::ue::tti_sync* ttisync_, log *log_h) {
  return init_(radio_handler_, ttisync_, log_h, true);
}

void phy::start_trace()
{
  tr_enabled = true; 
}

void phy::write_trace(std::string filename)
{
  tr_start_time.writeToBinary(filename + ".start");
  tr_end_time.writeToBinary(filename + ".end");
}

void phy::tr_log_start()
{
  if (tr_enabled) {
    tr_start_time.push_cur_time_us(get_current_tti());
  }
}

void phy::tr_log_end()
{
  if (tr_enabled) {
    tr_end_time.push_cur_time_us(get_current_tti());
  }
}

bool phy::init_(srslte::radio* radio_handler_, srslte::ue::tti_sync* ttisync_, log *log_h_, bool do_agc_)
{

  mlockall(MCL_CURRENT | MCL_FUTURE);
  
  started = false; 
  radio_is_streaming = false; 
  ttisync = ttisync_;
  log_h = log_h_; 
  radio_handler = radio_handler_;
  ul_buffer_queue = new queue(NOF_ULDL_QUEUES, sizeof(ul_buffer));
  dl_buffer_queue = new queue(NOF_ULDL_QUEUES, sizeof(dl_buffer));
  do_agc = do_agc_;
  last_gain = 1e4; 
  time_adv_sec = 0;
  sr_tx_tti = 0;  
  
  // Set default params  
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_PSS_NFRAMES, 100);
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD, 160);
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_MIB_NFRAMES, 100);
  
  if (threads_new_rt_prio(&phy_thread, phy_thread_fnc, this, 2)) {
    started = true; 
  }
  return started; 
}

void phy::stop()
{
  started = false; 
  
  pthread_join(phy_thread, NULL);

  for (int i=0;i<NOF_ULDL_QUEUES;i++) {
    ((ul_buffer*) ul_buffer_queue->get(i))->free_cell();
    ((dl_buffer*) dl_buffer_queue->get(i))->free_cell();
  }
   
  delete ul_buffer_queue;
  delete dl_buffer_queue;
  
  prach_buffer.free_cell(); 
}

radio* phy::get_radio() {
  return radio_handler; 
}

void phy::set_timeadv_rar(uint32_t ta_cmd) {
  n_ta = srslte_N_ta_new_rar(ta_cmd);
  time_adv_sec = ((float) n_ta)*SRSLTE_LTE_TS;
  Info("Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, time_adv_sec*1e6);
}

void phy::set_timeadv(uint32_t ta_cmd) {
  n_ta = srslte_N_ta_new(n_ta, ta_cmd);
  time_adv_sec = ((float) n_ta)*SRSLTE_LTE_TS;  
  Info("Set TA: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, time_adv_sec*1e6);
}

void phy::rar_ul_grant(srslte_dci_rar_grant_t *rar, ul_sched_grant *grant)
{
  uint32_t n_ho = params_db.get_param(phy_params::PUSCH_HOPPING_OFFSET);
  grant->create_from_rar(rar, cell, params_db.get_param(phy_params::PUSCH_HOPPING_OFFSET)); 
}

void phy::set_param(phy_params::phy_param_t param, int64_t value) {
  params_db.set_param((uint32_t) param, value);
}

int64_t phy::get_param(phy_params::phy_param_t param) {
  return params_db.get_param((uint32_t) param);
}

// FIXME: Add PRACH power control
bool phy::send_prach(uint32_t preamble_idx) {
  return send_prach(preamble_idx, -1, 0);
}
bool phy::send_prach(uint32_t preamble_idx, int allowed_subframe) {
  return send_prach(preamble_idx, allowed_subframe, 0);
}
bool phy::send_prach(uint32_t preamble_idx, int allowed_subframe, int target_power_dbm)
{
  if (phy_state == RXTX) {
    srslte_agc_lock(&ue_sync.agc, true);
    old_gain = radio_handler->get_tx_gain();
    radio_handler->set_tx_gain(80);
    Info("Stopped AGC. Set TX gain to %.1f dB\n", radio_handler->get_tx_gain());
    return prach_buffer.prepare_to_send(preamble_idx, allowed_subframe, target_power_dbm);
  } 
  return false; 
}

/* Instruct the PHY to send a SR as soon as possible */
void phy::send_sr(bool enable)
{
  sr_enabled = enable;
  if (!enable) {
    sr_tx_tti = 0; 
  }
}

int phy::sr_last_tx_tti() {
  if (sr_enabled) {
    return -1; 
  } else {
    return (int) sr_tx_tti; 
  }
}

bool phy::cqi_is_ready_to_send(uint32_t tti)
{
  /*
  if (params_db.get_param(phy_params::CQI_PERIODIC_CONFIGURED)) {
    if (srslte_cqi_send(params_db.get_param(phy_params::CQI_PERIODIC_PMI_IDX), tti)) {
      Warning("Sending PUCCH CQI\n");
      return true; 
    }
  }
  */
  return false; 
}

bool phy::sr_is_ready_to_send(uint32_t tti_) {
  if (sr_enabled) {
    // Get I_sr parameter
    uint32_t I_sr = params_db.get_param(phy_params::SR_CONFIG_INDEX);
    if (srslte_ue_ul_sr_send_tti(I_sr, tti_)) {
      sr_enabled = false;
      sr_tx_tti = tti_; 
      Info("SR transmission at TTI=%d\n", tti_);
      return true; 
    }
  }
  return false; 
}

int phy::get_prach_transmitted_tti()
{
  return prach_buffer.get_transmitted_tti(); 
}

// Do fast measurement on RSSI and/or PSS autocorrelation energy or PSR
bool phy::measure()
{
  if (phy_state == IDLE) {
    // capture and do measurement 
  }
  return false; 
}

void phy::set_crnti(uint16_t rnti) {
  for(uint32_t i=0;i<NOF_ULDL_QUEUES;i++) {
    ((ul_buffer*) ul_buffer_queue->get(i))->set_crnti(rnti);
    ((dl_buffer*) dl_buffer_queue->get(i))->set_crnti(rnti);      
    
  }    
}

bool phy::start_rxtx()
{
  if (phy_state == IDLE) {
    if (cell_is_set) {
      // Set RX/TX sampling rate 
      radio_handler->set_rx_srate((float) srslte_sampling_freq_hz(cell.nof_prb));
      radio_handler->set_tx_srate((float) srslte_sampling_freq_hz(cell.nof_prb));
      
      phy_state = RXTX;
      return true; 
    } else {
      Error("Can not change state to RXTX: cell is not set\n");
    }    
  } else {
    Error("Can not change state to RXTX: invalid state %d\n", phy_state);
  }
  return false; 
}

bool phy::stop_rxtx()
{
  if (phy_state == RXTX) {
    // Stop streaming
    radio_handler->stop_rx();
    phy_state = IDLE; 
    return true; 
  } else {
    Error("Can not change state to RXTX: invalid state %d\n", phy_state);
  }
  return false; 
}

float phy::get_agc_gain()
{
  return 10*log10(srslte_agc_get_gain(&ue_sync.agc));
}

bool phy::status_is_idle() {
  return phy_state == IDLE;
}

bool phy::status_is_rxtx() {
  return phy_state == RXTX;  
}

uint32_t phy::get_current_tti() {
  return ttisync->get_producer_cntr(); 
}
uint32_t phy::tti_to_SFN(uint32_t tti) {
  return tti/10; 
}

uint32_t phy::tti_to_subf(uint32_t tti) {
  return tti%10; 
}

void* phy::phy_thread_fnc(void *arg) {
  phy* phy = static_cast<srslte::ue::phy*>(arg);  
  phy->main_radio_loop();
  return NULL; 
}

int radio_recv_wrapper_cs(void *h,void *data, uint32_t nsamples, srslte_timestamp_t *rx_time)
{
  radio *radio_handler = (radio*) h;
  int n = radio_handler->rx_now(data, nsamples, rx_time);
  return n; 
}

double callback_set_rx_gain(void *h, double gain) {
  radio *radio_handler = (radio*) h;
  return radio_handler->set_rx_gain_th(gain);
}

bool phy::set_cell(srslte_cell_t cell_) {
  if (phy_state == IDLE) {
    cell_is_set = false;
    cell = cell_; 
    if (!srslte_ue_mib_init(&ue_mib, cell)) 
    {
      if (!srslte_ue_sync_init(&ue_sync, cell, radio_recv_wrapper_cs, radio_handler)) 
      {

        if (do_agc) {
          srslte_ue_sync_start_agc(&ue_sync, callback_set_rx_gain, last_gain);    
        }

        srslte_ue_sync_set_cfo(&ue_sync, cellsearch_cfo);
        for(uint32_t i=0;i<NOF_ULDL_QUEUES;i++) {
          ((ul_buffer*) ul_buffer_queue->get(i))->init_cell(cell, &params_db, log_h, radio_handler);
          ((dl_buffer*) dl_buffer_queue->get(i))->init_cell(cell, &params_db, log_h);      
          ((dl_buffer*) dl_buffer_queue->get(i))->buffer_id = i; 
          ((ul_buffer*) ul_buffer_queue->get(i))->ready(); 
          ((dl_buffer*) dl_buffer_queue->get(i))->release(); 
        }    
        cell_is_set = true;                   
      } else {
        Error("Error setting cell: initiating ue_sync");      
      }
    } else {
      Error("Error setting cell: initiating ue_mib\n"); 
    }      
  } else {
    Error("Error setting cell: Invalid state %d\n", phy_state);
  }
  return cell_is_set; 
}

bool phy::init_prach() {
  return prach_buffer.init_cell(cell, &params_db, log_h);
}

ul_buffer* phy::get_ul_buffer(uint32_t tti)
{
  tti=tti%10240;
  if (tti + 1 < get_current_tti() && tti > NOF_ULDL_QUEUES) {
    Warning("Warning access to PHY UL buffer too late. Requested TTI=%d while PHY is in %d\n", tti, get_current_tti());
  }
  return (ul_buffer*) ul_buffer_queue->get(tti);        
}

ul_buffer* phy::get_ul_buffer_adv(uint32_t tti)
{
  return (ul_buffer*) ul_buffer_queue->get(tti + ul_buffer::tx_advance_sf);        
}

dl_buffer* phy::get_dl_buffer(uint32_t tti)
{
  tti=tti%10240;
  if (tti + 4 < get_current_tti()) {
    Warning("Warning access to PHY DL buffer too late. Requested TTI=%d while PHY is in %d\n", tti, get_current_tti());
   // return NULL; 
  }
  return (dl_buffer*) dl_buffer_queue->get(tti);  
}
    
bool phy::decode_mib(uint32_t N_id_2, srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]) {
  return decode_mib_N_id_2((int) N_id_2, cell, payload); 
}

bool phy::decode_mib_best(srslte_cell_t *cell, uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]) {
  return decode_mib_N_id_2(-1, cell, payload); 
}

bool phy::decode_mib_N_id_2(int force_N_id_2, srslte_cell_t *cell_ptr, uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN]) 
{
  srslte_ue_cellsearch_result_t found_cells[3];
  srslte_ue_cellsearch_t        cs; 

  bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));

  if (srslte_ue_cellsearch_init(&cs, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  if (do_agc) {
    srslte_ue_sync_start_agc(&cs.ue_sync, callback_set_rx_gain, last_gain);
  }

  
  srslte_ue_cellsearch_set_nof_frames_to_scan(&cs, params_db.get_param(phy_params::CELLSEARCH_TIMEOUT_PSS_NFRAMES));
  srslte_ue_cellsearch_set_threshold(&cs, (float) 
    params_db.get_param(phy_params::CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD)/10);

  radio_handler->set_rx_srate(1920000.0);
  radio_handler->start_rx();
  
  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int ret = SRSLTE_ERROR; 
  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell); 
  }

  last_gain = srslte_agc_get_gain(&cs.ue_sync.agc);

  radio_handler->stop_rx();
  srslte_ue_cellsearch_free(&cs);
  
  if (ret < 0) {
    Error("Error decoding MIB: Error searching PSS\n");
    return false;
  } else if (ret == 0) {
    Error("Error decoding MIB: Could not find any PSS in this frequency\n");
    return false;
  }
    
  // Save result 
  cell_ptr->id = found_cells[max_peak_cell].cell_id;
  cell_ptr->cp = found_cells[max_peak_cell].cp; 
  cellsearch_cfo = found_cells[max_peak_cell].cfo;
  
  Info("\nFound CELL ID: %d CP: %s, CFO: %f\n", cell_ptr->id, srslte_cp_string(cell_ptr->cp), cellsearch_cfo);
  
  srslte_ue_mib_sync_t ue_mib_sync; 

  if (srslte_ue_mib_sync_init(&ue_mib_sync, cell_ptr->id, cell_ptr->cp, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  if (do_agc) {
    srslte_ue_sync_start_agc(&ue_mib_sync.ue_sync, callback_set_rx_gain, last_gain);    
  }

  /* Find and decode MIB */
  uint32_t sfn, sfn_offset; 

  radio_handler->start_rx();
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, params_db.get_param(phy_params::CELLSEARCH_TIMEOUT_MIB_NFRAMES), 
                                  bch_payload, &cell_ptr->nof_ports, &sfn_offset); 
  radio_handler->stop_rx();
  last_gain = srslte_agc_get_gain(&ue_mib_sync.ue_sync.agc);
  srslte_ue_mib_sync_free(&ue_mib_sync);


  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, cell_ptr, NULL);
    return true;     
  } else {
    Warning("Error decoding MIB: Error decoding PBCH\n");      
    return false;
  }
}


int phy::sync_sfn(void) {
  
  cf_t *sf_buffer = NULL; 
  int ret = SRSLTE_ERROR; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];

  srslte_ue_sync_decode_sss_on_track(&ue_sync, true);
  ret = srslte_ue_sync_get_buffer(&ue_sync, &sf_buffer);
  if (ret < 0) {
    Error("Error calling ue_sync_get_buffer");      
    return -1;
  }
    
  if (ret == 1) {
    if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
      uint32_t sfn_offset=0;
      srslte_pbch_decode_reset(&ue_mib.pbch);
      int n = srslte_ue_mib_decode(&ue_mib, sf_buffer, bch_payload, NULL, &sfn_offset);
      if (n < 0) {
        Error("Error decoding MIB while synchronising SFN");      
        return -1; 
      } else if (n == SRSLTE_UE_MIB_FOUND) {  
        uint32_t sfn; 
        srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);

        sfn = (sfn + sfn_offset)%1024;         
        ttisync->set_producer_cntr(10*sfn+1);
        srslte_ue_sync_decode_sss_on_track(&ue_sync, false);
        return 1;
      }
    }    
  }
  return 0;
}

void phy::run_rx_tx_state() 
{
  int ret; 
  if (!is_sfn_synched) {
    if (!radio_is_streaming) {
      // Start streaming
      radio_handler->start_rx();
      radio_is_streaming = true; 
    }
      
    ret = sync_sfn(); 
    switch(ret) {
      default:
        phy_state = IDLE; 
        break; 
      case 1:
        is_sfn_synched = true;
        is_first_of_burst = true; 
        break;        
      case 0:
        break;        
    } 
  } else {
    uint32_t current_tti = ttisync->get_producer_cntr();
    log_h->step(current_tti);
    float cfo = srslte_ue_sync_get_cfo(&ue_sync)/15000; 

    srslte_timestamp_add(&last_rx_time, 0, 1e-3);

    /* Set CFO and next TX time for UL buffer for TTI+4 */    
    get_ul_buffer(current_tti+4)->set_tx_params(cfo, time_adv_sec, last_rx_time);
    
    // Every subframe, TX a PRACH or a PUSCH/PUCCH
    if (prach_buffer.is_ready_to_send(current_tti)) {
      // send prach if we have to 
      prach_buffer.send(radio_handler, cfo, last_rx_time);
      radio_handler->tx_end();
      radio_handler->set_tx_gain(old_gain);
      srslte_agc_lock(&ue_sync.agc, false);
      Info("Restoring AGC. Set TX gain to %.1f dB\n", old_gain);    
    } 
    
    // Receive alligned buffer for the current tti 
    tr_log_end();
    get_dl_buffer(current_tti)->recv_ue_sync(&ue_sync, &last_rx_time);
    tr_log_start();
    ttisync->increase();      
  }
}


void phy::main_radio_loop() {
  while(started) {
    switch(phy_state) {
      case IDLE:
        usleep(50000);
        break;
      case RXTX:
        run_rx_tx_state();
        break;
    }
  }
}

  
}
}
