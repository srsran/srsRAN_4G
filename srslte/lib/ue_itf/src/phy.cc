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

#include "srslte/srslte.h"

#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/prach.h"
#include "srslte/ue_itf/ul_buffer.h"
#include "srslte/ue_itf/dl_buffer.h"

namespace srslte {
namespace ue {
    
bool phy::init(srslte::radio* radio_handler_, srslte::ue::tti_sync* ttisync_)
{
  started = false; 
  ttisync = ttisync_;
  radio_handler = radio_handler_;
  ul_buffer_queue = new queue(6, sizeof(ul_buffer));
  dl_buffer_queue = new queue(6, sizeof(dl_buffer));
  
  // Set default params  
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_PSS_NFRAMES, 100);
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD, 160);
  params_db.set_param(phy_params::CELLSEARCH_TIMEOUT_MIB_NFRAMES, 100);
  
  pthread_attr_t attr;
  struct sched_param param;
  param.sched_priority = 99;
  
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setschedparam(&attr, &param);
  if (!pthread_create(&phy_thread, &attr, phy_thread_fnc, this)) {
    started = true;             
  } else {
    perror("pthread_create");
  }
  pthread_attr_destroy(&attr);
  return started; 
}

void phy::stop()
{
  started = false; 
  
  pthread_join(phy_thread, NULL);

  for (int i=0;i<6;i++) {
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
  time_adv_sec = SRSLTE_TA_OFFSET+((float) n_ta)*SRSLTE_LTE_TS;
  INFO("Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, time_adv_sec*1e6);
}

void phy::set_timeadv(uint32_t ta_cmd) {
  n_ta = srslte_N_ta_new(n_ta, ta_cmd);
  time_adv_sec = SRSLTE_TA_OFFSET+((float) n_ta)*SRSLTE_LTE_TS;  
  INFO("Set TA: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, time_adv_sec*1e6);
}

void phy::rar_ul_grant(srslte_dci_rar_grant_t *rar, ul_sched_grant *grant)
{
  uint32_t n_ho = params_db.get_param(phy_params::PUSCH_HOPPING_OFFSET);
  grant->create_from_rar(rar, cell, 0, params_db.get_param(phy_params::PUSCH_HOPPING_OFFSET)); 
}

void phy::set_param(phy_params::phy_param_t param, int64_t value) {
  params_db.set_param((uint32_t) param, value);
}


// FIXME: Add PRACH power control
bool phy::send_prach(uint32_t preamble_idx)
{
  if (phy_state == RXTX) {
    return prach_buffer.prepare_to_send(preamble_idx);
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
      fprintf(stderr, "Can not change state to RXTX: cell is not set\n");
    }    
  } else {
    fprintf(stderr, "Can not change state to RXTX: invalid state %d\n", phy_state);
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
    fprintf(stderr, "Can not change state to RXTX: invalid state %d\n", phy_state);
  }
  return false; 
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
  return radio_handler->rx_now(data, nsamples, rx_time);
}

bool phy::set_cell(srslte_cell_t cell_) {
  if (phy_state == IDLE) {
    cell_is_set = false;
    cell = cell_; 
    if (!srslte_ue_mib_init(&ue_mib, cell)) 
    {
      if (!srslte_ue_sync_init(&ue_sync, cell, radio_recv_wrapper_cs, radio_handler)) 
      {

        srslte_ue_sync_set_cfo(&ue_sync, cellsearch_cfo);
        if (prach_buffer.init_cell(cell, &params_db)) {
          for(uint32_t i=0;i<6;i++) {
            ((ul_buffer*) ul_buffer_queue->get(i))->init_cell(cell, &params_db);
            ((dl_buffer*) dl_buffer_queue->get(i))->init_cell(cell, &params_db);      
            ((dl_buffer*) dl_buffer_queue->get(i))->buffer_id = i; 
            ((ul_buffer*) ul_buffer_queue->get(i))->ready(); 
            ((dl_buffer*) dl_buffer_queue->get(i))->release(); 
          }    
          cell_is_set = true;           
        }
      } else {
        fprintf(stderr, "Error setting cell: initiating ue_sync");      
      }
    } else {
      fprintf(stderr, "Error setting cell: initiating ue_mib\n"); 
    }      
  } else {
    fprintf(stderr, "Error setting cell: Invalid state %d\n", phy_state);
  }
  return cell_is_set; 
}

ul_buffer* phy::get_ul_buffer(uint32_t tti)
{
  return (ul_buffer*) ul_buffer_queue->get(tti);        
}

ul_buffer* phy::get_ul_buffer_adv(uint32_t tti)
{
  return (ul_buffer*) ul_buffer_queue->get(tti + ul_buffer::tx_advance_sf);        
}

dl_buffer* phy::get_dl_buffer(uint32_t tti)
{
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

  radio_handler->stop_rx();
  srslte_ue_cellsearch_free(&cs);
  
  if (ret < 0) {
    fprintf(stderr, "Error decoding MIB: Error searching PSS\n");
    return false;
  } else if (ret == 0) {
    fprintf(stderr, "Error decoding MIB: Could not find any PSS in this frequency\n");
    return false;
  }
  
  // Save result 
  cell_ptr->id = found_cells[max_peak_cell].cell_id;
  cell_ptr->cp = found_cells[max_peak_cell].cp; 
  cellsearch_cfo = found_cells[max_peak_cell].cfo;
  
  INFO("\nFound CELL ID: %d CP: %s, CFO: %f\n", cell_ptr->id, srslte_cp_string(cell_ptr->cp), cellsearch_cfo);
  
  srslte_ue_mib_sync_t ue_mib_sync; 

  if (srslte_ue_mib_sync_init(&ue_mib_sync, cell_ptr->id, cell_ptr->cp, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  /* Find and decode MIB */
  uint32_t sfn, sfn_offset; 

  radio_handler->start_rx();
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, params_db.get_param(phy_params::CELLSEARCH_TIMEOUT_MIB_NFRAMES), 
                                  bch_payload, &cell_ptr->nof_ports, &sfn_offset); 
  radio_handler->stop_rx();
  srslte_ue_mib_sync_free(&ue_mib_sync);

  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, cell_ptr, NULL);
    return true;     
  } else {
    printf("Error decoding MIB: Error decoding PBCH\n");      
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
    fprintf(stderr, "Error calling ue_sync_get_buffer");      
    return -1;
  }
    
  if (ret == 1) {
    if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
      uint32_t sfn_offset=0;
      srslte_pbch_decode_reset(&ue_mib.pbch);
      int n = srslte_ue_mib_decode(&ue_mib, sf_buffer, bch_payload, NULL, &sfn_offset);
      if (n < 0) {
        fprintf(stderr, "Error decoding MIB while synchronising SFN");      
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
        break;        
      case 0:
        break;        
    } 
  } else {
    uint32_t current_tti = ttisync->get_producer_cntr();
    float cfo = srslte_ue_sync_get_cfo(&ue_sync)/15000; 

    // Prepare transmission for the next tti 
    srslte_timestamp_add(&last_rx_time, 0, 1e-3);
    
    // send prach if we have to 
    if (prach_buffer.is_ready_to_send(current_tti)) {
      prach_buffer.send(radio_handler, cfo, last_rx_time);
    }
    // send ul buffer if we have to 
    if (get_ul_buffer_adv(current_tti)->is_released() || get_ul_buffer_adv(current_tti)->uci_ready()) {
      // Generate PUCCH if no UL grant
      if (!get_ul_buffer_adv(current_tti)->is_released()) {
        get_ul_buffer_adv(current_tti)->generate_data();
      }
      get_ul_buffer_adv(current_tti)->send(radio_handler, time_adv_sec, cfo, last_rx_time);      
    } 
    
    // Receive alligned buffer for the current tti 
    get_dl_buffer(current_tti)->recv_ue_sync(&ue_sync, &last_rx_time);

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