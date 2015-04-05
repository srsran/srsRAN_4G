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
#include "srslte/srslte.h"

#include "srslte/ue_itf/phy.h"
#include "srslte/ue_itf/prach.h"
#include "srslte/ue_itf/ul_buffer.h"
#include "srslte/ue_itf/dl_buffer.h"

namespace srslte {
namespace ue {
  
#if SYNC_MODE==SYNC_MODE_CALLBACK
phy::phy(ue_phy_callback_t tti_clock_callback_, ue_phy_callback_t status_change_)
{
  tti_clock_callback = tti_clock_callback_;
  status_change      = status_change_;
  ul_buffer_queue = new queue(6, sizeof(ul_buffer));
  dl_buffer_queue = new queue(6, sizeof(dl_buffer));
  
  started = true; 
  
  pthread_create(&radio_thread, NULL, radio_thread_fnc, this);
}

#else
phy()
{
  
}

#endif

phy::~phy()
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


// FIXME: Add PRACH power control
void phy::send_prach(uint32_t preamble_idx)
{
  if (phy_state == RXTX) {
    prach_buffer.ready_to_send(preamble_idx);
  }
}

// Do fast measurement on RSSI and/or PSS autocorrelation energy or PSR
void phy::measure()
{
  if (phy_state == IDLE) {
    // capture and do measurement 
    status_change();  
  }
}

void phy::dl_bch()
{
  if (phy_state == IDLE) {
    phy_state = RX_BCH; 
    status_change();  
   }
}

void phy::start_rxtx()
{
  if (phy_state == MIB_DECODED) {
    // Set sampling freq to MIB PRB 
    // start radio streaming
    phy_state = RXTX;
    status_change();  
   }
}

void phy::stop_rxtx()
{
  // stop radio 
  phy_state = IDLE; 
  status_change();  
 }

bool phy::status_is_idle() {
  return phy_state == IDLE;
}

bool phy::status_is_rxtx() {
  return phy_state == RXTX;  
}

bool phy::status_is_bch_decoded(uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]) 
{
  if (phy_state == MIB_DECODED) {
    memcpy(payload, bch_payload, SRSLTE_BCH_PAYLOAD_LEN*sizeof(uint8_t));
  }
}


void* phy::radio_thread_fnc(void *arg) {
  phy* phy = static_cast<srslte::ue::phy*>(arg);
  phy->main_radio_loop();
  return NULL; 
}

int radio_recv_wrapper_cs(void*,void*,uint32_t,srslte_timestamp_t*)
{
  
}


void phy::run_rx_bch_state() {
  phy_state = IDLE; 
  if (rx_bch()) {
    for(uint32_t i=0;i<6;i++) {
      get_ul_buffer(i)->init_cell(cell, &params_db);
      get_dl_buffer(i)->init_cell(cell, &params_db);      
    }    
    if (srslte_ue_mib_init(&ue_mib, cell)) {
      fprintf(stderr, "Error initiating UE mib\n"); 
    } else {
      if (srslte_ue_sync_init(&ue_sync, cell, radio_recv_wrapper_cs, radio_handler)) {
        fprintf(stderr, "Error initiating ue_sync");      
      } else {
        phy_state = MIB_DECODED;        
      }
    }
  }
  status_change();  
}

void phy::main_radio_loop() {
  while(started) {
    switch(phy_state) {
      case IDLE:
      case MIB_DECODED: 
        break;
      case RX_BCH:
        run_rx_bch_state();
        break;
      case RXTX:
        run_rx_tx_state();
        break;
    }
  }
}

ul_buffer* phy::get_ul_buffer(uint32_t tti)
{
  return (ul_buffer*) ul_buffer_queue->get(tti);  
}

dl_buffer* phy::get_dl_buffer(uint32_t tti)
{
  return (dl_buffer*) dl_buffer_queue->get(tti);  
}
  
bool phy::rx_bch() 
{
  srslte_ue_cellsearch_result_t found_cells[3];
  srslte_ue_cellsearch_t        cs; 
  
  bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));

  if (srslte_ue_cellsearch_init(&cs, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  srslte_ue_cellsearch_set_nof_frames_to_scan(&cs, params_db.get_param(params::CELLSEARCH_TIMEOUT_PSS_NFRAMES));
  srslte_ue_cellsearch_set_threshold(&cs, (float) params_db.get_param(params::CELLSEARCH_CORRELATION_THRESHOLD)/10);
      
  // set sampling freq 1.92 MHz
  // set frequency, gain etc 
  // start radio streaming
  
  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int ret = SRSLTE_ERROR; 
  uint32_t force_N_id_2 = params_db.get_param(params::CELLSEARCH_FORCE_N_ID_2); 
  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell); 
  }
  // Stop radio 
  
  srslte_ue_cellsearch_free(&cs);
  
  if (ret < 0) {
    fprintf(stderr, "Error searching cell");
    return false;
  } else if (ret == 0) {
    fprintf(stderr, "Could not find any cell in this frequency");
    return false;
  }
  
  // Save result 
  cell.id = found_cells[max_peak_cell].cell_id;
  cell.cp = found_cells[max_peak_cell].cp; 
  
  printf("Found CELL PHY_ID: %d, CP: %s PSR: %.1f AbsPower: %.1f dBm",
          cell.id, srslte_cp_string(cell.cp), 
          found_cells[max_peak_cell].psr, 30+10*log10(found_cells[max_peak_cell].peak));        
  
  srslte_ue_mib_sync_t ue_mib_sync; 

  if (srslte_ue_mib_sync_init(&ue_mib_sync, cell.id, cell.cp, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  uint32_t sfn, sfn_offset; 

  /* Find and decode MIB */
  
  // Start RX stream again
  
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, params_db.get_param(params::CELLSEARCH_TIMEOUT_MIB_NFRAMES), 
                                  bch_payload, &cell.nof_ports, &sfn_offset); 
  // Stop RX stream again
  srslte_ue_mib_sync_free(&ue_mib_sync);

  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);
    sfn = (sfn + sfn_offset)%1024;         
    current_tti = sfn*10+1;
    printf("MIB decoded: %d ports, SFN: %d, TTI: %d", cell.nof_ports, sfn, current_tti);          
    return true;     
  } else {
    printf("Error decoding MIB");      
    return false;
  }
}


int phy::sync_sfn(void) {
  
  cf_t *sf_buffer = NULL; 
  int ret = SRSLTE_ERROR; 

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
        current_tti = sfn*10;
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
    // Receive alligned buffer for the current tti 
    srslte_timestamp_t rx_time; 
    get_dl_buffer(current_tti)->recv_ue_sync(&ue_sync, &rx_time);

    // send prach if we have to 
    if (prach_buffer.is_ready_to_send(current_tti)) {
      prach_buffer.send(radio_handler, rx_time);
    }
    // send ul buffer if we have to 
    if (get_ul_buffer(current_tti)->is_ready_to_send()) {
      get_ul_buffer(current_tti)->send_packet(radio_handler, rx_time);      
    }
  }
  current_tti++; 
  tti_clock_callback();
}
  
}
}