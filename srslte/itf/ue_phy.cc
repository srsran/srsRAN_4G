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
#include "ue_phy.h"

    
/* 
 * CLASS SCHED_GRANT 
 * 
 */ 

/* Returns the RNTI associated with the UL/DL scheduling grant */
uint16_t srslte::ue_phy::sched_grant::get_rnti() {
  return rnti; 
}

uint32_t srslte::ue_phy::sched_grant::get_rv() {
  if (dir == UPLINK) {
    return ul_grant.rv_idx;
  } else {
    return dl_grant.rv_idx;    
  }
}

void srslte::ue_phy::sched_grant::set_rv(uint32_t rv) {
  if (dir == UPLINK) {
    ul_grant.rv_idx = rv;
  } else {
    dl_grant.rv_idx = rv;    
  }
}

bool srslte::ue_phy::sched_grant::get_ndi() {
  if (dir == UPLINK) {
    return ul_grant.ndi;
  } else {
    return dl_grant.ndi;    
  }
}

bool srslte::ue_phy::sched_grant::get_cqi_request() {
  if (dir == UPLINK) {
    return ul_grant.ndi;
  } else {
    return dl_grant.ndi;    
  }  
}

uint32_t srslte::ue_phy::sched_grant::get_current_tti() {
  return current_tti;
}

int srslte::ue_phy::sched_grant::get_harq_process() {
  if (dir == UPLINK) {
    return -1;
  } else {
    return dl_grant.harq_process;    
  }
}

bool srslte::ue_phy::sched_grant::is_uplink() {
  return dir == UPLINK;
}

bool srslte::ue_phy::sched_grant::is_downlink() {
  return dir == DOWNLINK;
}
void* srslte::ue_phy::sched_grant::get_grant_ptr() {
  if (is_uplink()) {
    return (void*) &ul_grant;
  } else {
    return (void*) &dl_grant;    
  }
}


/* 
 * CLASS UL_BUFFER 
 * 
 */ 
bool srslte::ue_phy::ul_buffer::init_cell(srslte_cell_t cell_, int64_t *params_db_) {
  cell = cell_; 
  params_db = params_db_; 
  signal_generated = false;
  current_tx_nb = 0;
  if (!srslte_ue_ul_init(&ue_ul, cell)) {  
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    return signal_buffer?true:false; 
  } else {
    return false; 
  }
}

void srslte::ue_phy::ul_buffer::free_cell() {
  if (signal_buffer) {
    free(signal_buffer);
  }
  srslte_ue_ul_free(&ue_ul);
}

bool srslte::ue_phy::ul_buffer::generate_pusch(sched_grant pusch_grant, 
                                               uint8_t *payload, 
                                               srslte_uci_data_t uci_data) 
{
  if (pusch_grant.is_uplink()) {
    fprintf(stderr, "Invalid scheduling grant. Grant is for Downlink\n");
    return false; 
  }
  
  srslte_refsignal_drms_pusch_cfg_t drms_cfg; 
  bzero(&drms_cfg, sizeof(srslte_refsignal_drms_pusch_cfg_t));
  
  drms_cfg.beta_pusch = params_db[PARAM_PUSCH_BETA]; 
  drms_cfg.group_hopping_en = params_db[PARAM_PUSCH_RS_GROUP_HOPPING_EN];
  drms_cfg.sequence_hopping_en = params_db[PARAM_PUSCH_RS_SEQUENCE_HOPPING_EN];
  drms_cfg.cyclic_shift = params_db[PARAM_PUSCH_RS_CYCLIC_SHIFT];
  drms_cfg.delta_ss = params_db[PARAM_PUSCH_RS_GROUP_ASSIGNMENT];
  
  srslte_pusch_hopping_cfg_t pusch_hopping; 
  pusch_hopping.n_sb = params_db[PARAM_PUSCH_HOPPING_N_SB];
  pusch_hopping.hop_mode = params_db[PARAM_PUSCH_HOPPING_INTRA_SF] ? 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                  pusch_hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF; 
  pusch_hopping.hopping_offset = params_db[PARAM_PUSCH_HOPPING_OFFSET];
  pusch_hopping.current_tx_nb = current_tx_nb; 
  
  srslte_ue_ul_set_pusch_cfg(&ue_ul, &drms_cfg, &pusch_hopping);

  int n = srslte_ue_ul_pusch_uci_encode_rnti(&ue_ul, (srslte_ra_pusch_t*) pusch_grant.get_grant_ptr(), 
                                             payload, uci_data, 
                                             tti%10, pusch_grant.get_rnti(), 
                                             signal_buffer);
  if (n < 0) {
    fprintf(stderr, "Error encoding PUSCH\n");
    return false; 
  }

  signal_generated = true; 

  /* This is done by the transmission thread
  srslte_vec_sc_prod_cfc(signal_buffer, beta_pusch, signal_buffer, SRSLTE_SF_LEN_PRB(cell.nof_prb));
  float time_adv_sec = TA_OFFSET + ((float) n_ta)*LTE_TS;
  srslte_timestamp_t next_tx_time; 
  srslte_timestamp_copy(&next_tx_time, &last_rx_time);
  srslte_timestamp_add(&next_tx_time, 0, 0.003 - time_adv_sec); 
  */
  // Send through radio  
  
  return true; 
}

bool srslte::ue_phy::ul_buffer::generate_pucch(srslte_uci_data_t uci_data)
{  
  return false; 
}

/* 
 * CLASS DL_BUFFER 
 * 
 */ 
bool srslte::ue_phy::dl_buffer::init_cell(srslte_cell_t cell_, int64_t *params_db_)
{
  params_db = params_db_; 
  cell = cell_; 
  sf_symbols_and_ce_done = false; 
  pdcch_llr_extracted = false; 
  tti = 0; 
  if (srslte_ue_dl_init(&ue_dl, cell)) {
    signal_buffer = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
    return signal_buffer?true:false;       
  } else {
    return false; 
  }
}

void srslte::ue_phy::dl_buffer::free_cell()
{
  if (signal_buffer) {
    free(signal_buffer);
  }
  srslte_ue_dl_free(&ue_dl);
}

bool srslte::ue_phy::dl_buffer::get_ul_grant(pdcch_ul_search_t mode, uint32_t rnti, ue_phy::sched_grant *grant)
{
  if (!sf_symbols_and_ce_done) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
      return false; 
    }
    sf_symbols_and_ce_done = true; 
  }
  if (!pdcch_llr_extracted) {
    if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
      return false; 
    }
    pdcch_llr_extracted = true; 
  }
  
  srslte_dci_msg_t dci_msg; 
  if (srslte_ue_dl_find_ul_dci(&ue_dl, &dci_msg, cfi, tti%10, rnti)) {
    return false; 
  }
  
  if (srslte_dci_msg_to_ra_ul(&dci_msg, cell.nof_prb, 
                          params_db[PARAM_PUSCH_HOPPING_OFFSET],  
                          (srslte_ra_pusch_t*) grant->get_grant_ptr())) 
  {
    return false; 
  }

  return true; 
}

bool srslte::ue_phy::dl_buffer::get_dl_grant(pdcch_dl_search_t mode, uint32_t rnti, ue_phy::sched_grant *grant)
{
  if (!sf_symbols_and_ce_done) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
      return false; 
    }
    sf_symbols_and_ce_done = true; 
  }
  if (!pdcch_llr_extracted) {
    if (srslte_pdcch_extract_llr(&ue_dl.pdcch, ue_dl.sf_symbols, ue_dl.ce, 0, tti%10, cfi)) {
      return false; 
    }
    pdcch_llr_extracted = true; 
  }
  
  srslte_dci_msg_t dci_msg; 
  if (srslte_ue_dl_find_dl_dci(&ue_dl, &dci_msg, cfi, tti%10, rnti)) {
    return false; 
  }
  
  if (srslte_dci_msg_to_ra_dl(&dci_msg, rnti, cell, cfi, 
                          (srslte_ra_pdsch_t*) grant->get_grant_ptr())) 
  {
    return false; 
  }

  return true; 
}

bool srslte::ue_phy::dl_buffer::decode_phich(srslte_phich_alloc_t assignment)
{
  if (!sf_symbols_and_ce_done) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
      return false; 
    }
    sf_symbols_and_ce_done = true; 
  }
  return false; 
}

bool srslte::ue_phy::dl_buffer::decode_pdsch(sched_grant pdsch_grant, uint8_t *payload)
{
  if (!sf_symbols_and_ce_done) {
    if (srslte_ue_dl_decode_fft_estimate(&ue_dl, signal_buffer, tti%10, &cfi) < 0) {
      return false; 
    }
    sf_symbols_and_ce_done = true; 
  }
  srslte_ra_pdsch_t *ra_dl = (srslte_ra_pdsch_t*) pdsch_grant.get_grant_ptr();
  if (srslte_harq_setup_dl(&ue_dl.harq_process[0], ra_dl->mcs, 
                           pdsch_grant.get_rv(), tti%10, &ra_dl->prb_alloc)) {
    fprintf(stderr, "Error configuring HARQ process\n");
    return SRSLTE_ERROR;
  }
  if (ue_dl.harq_process[0].mcs.mod > 0 && ue_dl.harq_process[0].mcs.tbs >= 0) {
    int ret = srslte_pdsch_decode_rnti(&ue_dl.pdsch, &ue_dl.harq_process[0], ue_dl.sf_symbols, 
                            ue_dl.ce, 0, pdsch_grant.get_rnti(), payload);
    if (ret == SRSLTE_SUCCESS) {
      return true; 
    } 
  }
  return false; 
}







/* 
 * CLASS UE_PHY 
 * 
 */ 
#if SYNC_MODE==SYNC_MODE_CALLBACK
srslte::ue_phy::ue_phy(ue_phy_callback_t tti_clock_callback_, ue_phy_callback_t status_change_)
{
  tti_clock_callback = tti_clock_callback_;
  status_change      = status_change_;
  ul_buffer_queue = new queue(6, sizeof(ul_buffer));
  dl_buffer_queue = new queue(6, sizeof(dl_buffer));
  
  started = true; 
  
  pthread_create(&radio_thread, NULL, radio_thread_fnc, this);
}

#else
srslte::ue_phy::ue_phy()
{
  
}

#endif

srslte::ue_phy::~ue_phy()
{
  started = false; 
  
  pthread_join(radio_thread, NULL);
  
  delete ul_buffer_queue;
  delete dl_buffer_queue;
  
  if (prach_initiated) {
    for (uint32_t i=0;i<64;i++) {
      if (prach_buffer[i]) {
        free(prach_buffer[i]);
      }
    }
    srslte_prach_free(&prach);
  }
}

bool srslte::ue_phy::init_prach()
{

  if (srslte_prach_init(&prach, srslte_symbol_sz(cell.nof_prb), 
                        srslte_prach_get_preamble_format(params_db[PARAMS_PRACH_CONFIG_INDEX]), 
                        params_db[PARAMS_PRACH_ROOT_SEQ_IDX], 
                        params_db[PARAMS_PRACH_HIGH_SPEED_FLAG]?true:false, 
                        params_db[PARAMS_PRACH_ZC_CONFIG])) {
    return false; 
  }
  
  prach_len = prach.N_seq + prach.N_cp;
  for (uint32_t i=0;i<64;i++) {
    prach_buffer[i] = (cf_t*) srslte_vec_malloc(prach_len*sizeof(cf_t));
    if(!prach_buffer[i]) {
      return false; 
    }
    if(srslte_prach_gen(&prach, i, params_db[PARAMS_PRACH_FREQ_OFFSET], prach_buffer[i])){
      return false;
    }
  }
  prach_initiated = true; 
  return true;  
}

// FIXME: Add PRACH power control
void srslte::ue_phy::send_prach(uint32_t preamble_idx)
{
  if (prach_initiated && phy_state == RXTX) {
    prach_ready_to_send = true; 
  }
}

// Do fast measurement on RSSI and/or PSS autocorrelation energy or PSR
void srslte::ue_phy::measure()
{
  if (phy_state == IDLE) {
    // capture and do measurement 
    status_change();  
  }
}

void srslte::ue_phy::dl_bch()
{
  if (phy_state == IDLE) {
    phy_state = RX_BCH; 
    status_change();  
   }
}

void srslte::ue_phy::start_rxtx()
{
  if (phy_state == MIB_DECODED) {
    // Set sampling freq to MIB PRB 
    // start radio streaming
    phy_state = RXTX;
    status_change();  
   }
}

void srslte::ue_phy::stop_rxtx()
{
  // stop radio 
  phy_state = IDLE; 
  status_change();  
 }

bool srslte::ue_phy::status_is_idle() {
  return phy_state == IDLE;
}

bool srslte::ue_phy::status_is_rxtx() {
  return phy_state == RXTX;  
}

bool srslte::ue_phy::status_bch_decoded(uint8_t payload[SRSLTE_BCH_PAYLOAD_LEN]) 
{
  if (phy_state == MIB_DECODED) {
    memcpy(payload, bch_payload, SRSLTE_BCH_PAYLOAD_LEN*sizeof(uint8_t));
  }
}


static void* radio_thread_fnc(void *arg) {
  srslte::ue_phy* ue_phy = (srslte::ue_phy*) arg;
  ue_phy->main_radio_loop();
  return NULL; 
}

int radio_recv_wrapper_cs(void*,void*,uint32_t,srslte_timestamp_t*);

bool srslte::ue_phy::rx_bch() 
{
  srslte_ue_cellsearch_result_t found_cells[3];
  srslte_ue_cellsearch_t        cs; 
  
  bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));

  if (srslte_ue_cellsearch_init(&cs, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  srslte_ue_cellsearch_set_nof_frames_to_scan(&cs, params_db[PARAM_CELLSEARCH_TIMEOUT_PSS_NFRAMES]);
  srslte_ue_cellsearch_set_threshold(&cs, (float) params_db[PARAM_CELLSEARCH_CORRELATION_THRESHOLD]/10);
      
  // set sampling freq 1.92 MHz
  // set frequency, gain etc 
  // start radio streaming
  
  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int ret = SRSLTE_ERROR; 
  uint32_t force_N_id_2 = params_db[PARAM_CELLSEARCH_FORCE_N_ID_2]; 
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
  
  srslte_ue_mib_sync_t ue_mib; 

  if (srslte_ue_mib_sync_init(&ue_mib, cell.id, cell.cp, radio_recv_wrapper_cs, radio_handler)) {
    return false; 
  }
  
  uint32_t sfn, sfn_offset; 

  /* Find and decode MIB */
  
  // Start RX stream again
  
  ret = srslte_ue_mib_sync_decode(&ue_mib, params_db[PARAM_CELLSEARCH_TIMEOUT_MIB_NFRAMES], 
                                  bch_payload, &cell.nof_ports, &sfn_offset); 
  // Stop RX stream again
  srslte_ue_mib_sync_free(&ue_mib);

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

void srslte::ue_phy::run_rx_bch_state() {
  if (rx_bch()) {
    for(uint32_t i=0;i<6;i++) {
      get_ul_buffer(i)->init_cell(cell, params_db);
      get_dl_buffer(i)->init_cell(cell, params_db);
    }
    // init also ue_mib for sfn synch
    phy_state = MIB_DECODED;
    status_change();  
  } else {
    phy_state = IDLE; 
  }
}

void srslte::ue_phy::run_rx_tx_state() 
{
  // if not synched -> go through mib 
  // else receive sync frame on dl_frame
  //      if have to send prach, send it
  //      if ul ready to send send 
}

void srslte::ue_phy::main_radio_loop() {
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

void srslte::ue_phy::set_param(param_t param, int64_t value)
{
  params_db[param] = value; 
}

srslte::ue_phy::ul_buffer* srslte::ue_phy::get_ul_buffer(uint32_t tti)
{
  return (srslte::ue_phy::ul_buffer*) ul_buffer_queue->get(tti);  
}

srslte::ue_phy::dl_buffer* srslte::ue_phy::get_dl_buffer(uint32_t tti)
{
  return (srslte::ue_phy::dl_buffer*) dl_buffer_queue->get(tti);  
}
  

