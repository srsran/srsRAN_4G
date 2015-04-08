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
#include "srslte/cuhd/cuhd.h"

#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/prach.h"
#include "srslte/ue_itf/ul_buffer.h"
#include "srslte/ue_itf/dl_buffer.h"

namespace srslte {
namespace ue {
  
  
bool phy::init_radio_handler(char *args) {
  printf("Opening UHD device...\n");
  if (cuhd_open(args, &radio_handler)) {
    fprintf(stderr, "Error opening uhd\n");
    return false;
  }
  return true;    
}

bool phy::init(ue_phy_callback_tti_t tti_clock_callback_, ue_phy_callback_status_t status_change_)
{
  started = false; 
  tti_clock_callback = tti_clock_callback_;
  status_change      = status_change_;
  ul_buffer_queue = new queue(6, sizeof(ul_buffer));
  dl_buffer_queue = new queue(6, sizeof(dl_buffer));
  
  // Set default params  
  params_db.set_param(params::CELLSEARCH_TIMEOUT_PSS_NFRAMES, 100);
  params_db.set_param(params::CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD, 160);
  params_db.set_param(params::CELLSEARCH_TIMEOUT_MIB_NFRAMES, 100);

  
  if (init_radio_handler("")) {
    pthread_create(&radio_thread, NULL, radio_thread_fnc, this);
    started = true;         
  }
  return started; 
}

void phy::stop()
{
  started = false; 
  
  pthread_join(radio_thread, NULL);

  for (int i=0;i<6;i++) {
    ((ul_buffer*) ul_buffer_queue->get(i))->free_cell();
    ((dl_buffer*) dl_buffer_queue->get(i))->free_cell();
  }
   
  delete ul_buffer_queue;
  delete dl_buffer_queue;
  
  prach_buffer.free_cell(); 
}

void phy::set_tx_gain(float gain) {
  float x = cuhd_set_tx_gain(radio_handler, gain);
  printf("Set TX gain to %.1f dB\n", x);
}

void phy::set_rx_gain(float gain) {
  float x = cuhd_set_rx_gain(radio_handler, gain);
  printf("Set RX gain to %.1f dB\n", x);
}

void phy::set_tx_freq(float freq) {
  float x = cuhd_set_tx_freq(radio_handler, freq);
  printf("Set TX freq to %.1f MHz\n", x/1000000);
}

void phy::set_rx_freq(float freq) {
  float x = cuhd_set_rx_freq(radio_handler, freq);
  printf("Set RX freq to %.1f MHz\n", x/1000000);
}

void phy::set_param(params::param_t param, int64_t value) {
  params_db.set_param(param, value);
}


// FIXME: Add PRACH power control
bool phy::send_prach(uint32_t preamble_idx)
{
  if (phy_state == RXTX) {
    prach_buffer.ready_to_send(preamble_idx);
  }
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
      cuhd_set_rx_srate(radio_handler, srslte_sampling_freq_hz(cell.nof_prb));
      cuhd_set_tx_srate(radio_handler, srslte_sampling_freq_hz(cell.nof_prb));
      
      // Start streaming
      cuhd_start_rx_stream(radio_handler);
      phy_state = RXTX;
      status_change();  
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
    cuhd_stop_rx_stream(radio_handler);
    phy_state = IDLE; 
    status_change();  
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
  return current_tti; 
}
uint32_t phy::tti_to_SFN(uint32_t tti) {
  return tti/10; 
}

uint32_t phy::tti_to_subf(uint32_t tti) {
  return tti%10; 
}

void* phy::radio_thread_fnc(void *arg) {
  phy* phy = static_cast<srslte::ue::phy*>(arg);  
  phy->main_radio_loop();
  return NULL; 
}

int radio_recv_wrapper_cs(void *h,void *data, uint32_t nsamples, srslte_timestamp_t*)
{
  return cuhd_recv(h, data, nsamples, 1);
}

bool phy::set_cell(srslte_cell_t cell_) {
  if (phy_state == IDLE) {
    cell_is_set = false;
    cell = cell_; 
    if (!srslte_ue_mib_init(&ue_mib, cell)) 
    {
      if (!srslte_ue_sync_init(&ue_sync, cell, radio_recv_wrapper_cs, radio_handler)) 
      {
        if (prach_buffer.init_cell(cell, &params_db)) {
          for(uint32_t i=0;i<6;i++) {
            get_ul_buffer(i)->init_cell(cell, &params_db);
            get_dl_buffer(i)->init_cell(cell, &params_db);      
            get_dl_buffer(i)->buffer_id = i; 
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
  
  srslte_ue_cellsearch_set_nof_frames_to_scan(&cs, params_db.get_param(params::CELLSEARCH_TIMEOUT_PSS_NFRAMES));
  srslte_ue_cellsearch_set_threshold(&cs, (float) params_db.get_param(params::CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD)/10);

  cuhd_set_rx_srate(radio_handler, 1920000.0);
  cuhd_start_rx_stream(radio_handler);
  
  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int ret = SRSLTE_ERROR; 
  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell); 
  }
  
  cuhd_stop_rx_stream(radio_handler);
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
  
  INFO("\nFound CELL ID: %d CP: %s\n", cell_ptr->id, srslte_cp_string(cell_ptr->cp));
  
  srslte_ue_mib_sync_t ue_mib_sync; 

  if (srslte_ue_mib_sync_init(&ue_mib_sync, cell_ptr->id, cell_ptr->cp, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  /* Find and decode MIB */
  uint32_t sfn, sfn_offset; 

  cuhd_start_rx_stream(radio_handler);
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, params_db.get_param(params::CELLSEARCH_TIMEOUT_MIB_NFRAMES), 
                                  bch_payload, &cell_ptr->nof_ports, &sfn_offset); 
  cuhd_stop_rx_stream(radio_handler);
  srslte_ue_mib_sync_free(&ue_mib_sync);

  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, cell_ptr, &sfn);
    sfn = (sfn + sfn_offset)%1024;         
    current_tti = sfn*10+1;
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
        current_tti = sfn*10 + 1;
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
    printf("Synchronising SFN...\n");
    ret = sync_sfn(); 
    switch(ret) {
      default:
        phy_state = IDLE; 
        break; 
      case 1:
        printf("SFN synched ok\n");
        is_sfn_synched = true; 
        break;        
      case 0:
        break;        
    } 
  } else {
    // Receive alligned buffer for the current tti 
    srslte_timestamp_t rx_time; 
    get_dl_buffer(current_tti)->recv_ue_sync(current_tti, &ue_sync, &rx_time);

    // send prach if we have to 
    if (prach_buffer.is_ready_to_send(current_tti)) {
      prach_buffer.send(radio_handler, rx_time);
    }
    // send ul buffer if we have to 
    if (get_ul_buffer(current_tti)->is_ready_to_send()) {
      get_ul_buffer(current_tti)->send_packet(radio_handler, rx_time);      
    }
    tti_clock_callback(current_tti);
    current_tti = (current_tti + 1)%10240; 
  }
}


void phy::main_radio_loop() {
  printf("PHY initiated\n");

  while(started) {
    switch(phy_state) {
      case IDLE:
        usleep(50000);
        break;
      case RXTX:
        printf("Run RX_TX\n");
        run_rx_tx_state();
        break;
    }
  }
}

  
}
}