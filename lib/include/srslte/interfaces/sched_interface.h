/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include "srslte/common/common.h"
#include "srslte/srslte.h"

#ifndef SRSLTE_SCHED_INTERFACE_H
#define SRSLTE_SCHED_INTERFACE_H

namespace srsenb {

class sched_interface
{
public:
  const static uint32_t max_cce = 128;
  const static uint32_t max_prb = 100;
  const static uint32_t max_rbg = 25;

  const static int MAX_SIB_PAYLOAD_LEN = 2048; 
  const static int MAX_SIBS            = 16;
  const static int MAX_LC              = 6; 
  const static int MAX_DATA_LIST       = 32;
  const static int MAX_RAR_LIST        = 8;
  const static int MAX_BC_LIST         = 8;
  const static int MAX_RLC_PDU_LIST    = 8;
  const static int MAX_PHICH_LIST      = 8; 
  
  typedef struct {
    uint32_t len; 
    uint32_t period_rf; 
  } cell_cfg_sib_t;
  
  
  typedef struct {
    int pdsch_mcs; 
    int pdsch_max_mcs; 
    int pusch_mcs; 
    int pusch_max_mcs; 
    int nof_ctrl_symbols; 
  } sched_args_t; 

    
  typedef struct {
    
    // Main cell configuration (used to calculate DCI locations in scheduler)
    srslte_cell_t cell;
    
    /* SIB configuration */
    cell_cfg_sib_t sibs[MAX_SIBS]; 
    uint32_t si_window_ms; 
    
    /* pusch configuration */
    srslte_pusch_hopping_cfg_t pusch_hopping_cfg; 
    
    /* prach configuration */
    uint32_t prach_config; 
    uint32_t prach_freq_offset; 
    uint32_t prach_rar_window;
    uint32_t prach_contention_resolution_timer; 
    
    uint32_t maxharq_msg3tx;
    uint32_t n1pucch_an;
    uint32_t delta_pucch_shift; 

    // If non-negative, statically allocate N prbs at the edges of the uplink for PUCCH
    int      nrb_pucch;

    uint32_t nrb_cqi; 
    uint32_t ncs_an;
    
    uint32_t srs_subframe_config;
    uint32_t srs_subframe_offset; 
    uint32_t srs_bw_config; 
        
  } cell_cfg_t; 
  
  
  typedef struct {
    int priority; 
    int bsd; 
    int pbr;
    int group;
    enum {IDLE = 0, UL, DL, BOTH} direction; 
  } ue_bearer_cfg_t; 
  
  typedef struct {

    /* ue capabilities, etc */

    uint32_t maxharq_tx;
    bool     continuous_pusch;

    srslte_uci_offset_cfg_t uci_offset;
    srslte_pucch_cfg_t      pucch_cfg;

    uint32_t aperiodic_cqi_period; // if 0 is periodic CQI
    srslte_dl_cfg_t dl_cfg;

    ue_bearer_cfg_t ue_bearers[MAX_LC]; 
    
  } ue_cfg_t; 
  
  typedef struct {
    uint32_t lcid; 
    uint32_t nbytes;
  } dl_sched_pdu_t; 
  
  
  typedef struct {
    uint32_t lcid;
    uint32_t lcid_buffer_size;
    uint32_t stop;
    uint8_t* mtch_payload;
  } dl_mtch_sched_t;
  
  typedef struct {
    dl_sched_pdu_t pdu[20];
    dl_mtch_sched_t mtch_sched[8];
    uint32_t num_mtch_sched;
    uint8_t *mcch_payload;
    uint32_t current_sf_allocation_num;
  } dl_pdu_mch_t; 
 
  typedef struct {
    srslte_dci_dl_t       dci;
    uint32_t              tbs[SRSLTE_MAX_TB];
    bool mac_ce_ta;
    bool mac_ce_rnti;
    uint32_t nof_pdu_elems[SRSLTE_MAX_TB];
    dl_sched_pdu_t pdu[SRSLTE_MAX_TB][MAX_RLC_PDU_LIST];
  } dl_sched_data_t;
  
  typedef struct {
    bool            needs_pdcch;
    uint32_t        current_tx_nb;
    uint32_t        tbs;
    srslte_dci_ul_t dci;
  } ul_sched_data_t;
  
  typedef struct {
    uint32_t               ra_id;
    srslte_dci_rar_grant_t grant; 
  } dl_sched_rar_grant_t;
  
  typedef struct {
    uint32_t             tbs;
    srslte_dci_dl_t      dci;
    uint32_t             nof_grants;
    dl_sched_rar_grant_t msg3_grant[MAX_RAR_LIST];
  } dl_sched_rar_t; 

  typedef struct {
    srslte_dci_dl_t dci;

    enum bc_type {
      BCCH, PCCH
    } type; 
    
    uint32_t index;
    
    uint32_t tbs; 
       
  } dl_sched_bc_t; 
  
  typedef struct {
    uint32_t cfi; 
    uint32_t nof_data_elems;
    uint32_t nof_rar_elems;
    uint32_t nof_bc_elems; 
    dl_sched_data_t data[MAX_DATA_LIST];
    dl_sched_rar_t  rar[MAX_RAR_LIST];
    dl_sched_bc_t   bc[MAX_BC_LIST];
  } dl_sched_res_t; 
  
  typedef struct {
    uint16_t rnti; 
    enum phich_elem {
      ACK, NACK
    } phich;
  } ul_sched_phich_t;

  typedef struct {
    uint32_t nof_dci_elems; 
    uint32_t nof_phich_elems; 
    ul_sched_data_t  pusch[MAX_DATA_LIST];
    ul_sched_phich_t phich[MAX_PHICH_LIST];
  } ul_sched_res_t; 
  
  /******************* Scheduler Control ****************************/

  /* Provides cell configuration including SIB periodicity, etc. */
  virtual int cell_cfg(cell_cfg_t *cell_cfg) = 0; 
  virtual int reset() = 0;

  /* Manages UE scheduling context */
  virtual int ue_cfg(uint16_t rnti, ue_cfg_t *cfg) = 0; 
  virtual int ue_rem(uint16_t rnti) = 0;
  virtual bool ue_exists(uint16_t rnti) = 0; 

  /* Manages UE bearers and associated configuration */
  virtual int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, ue_bearer_cfg_t *cfg) = 0; 
  virtual int bearer_ue_rem(uint16_t rnti, uint32_t lc_id) = 0; 
  
  virtual uint32_t get_ul_buffer(uint16_t rnti) = 0; 
  virtual uint32_t get_dl_buffer(uint16_t rnti) = 0; 

  /******************* Scheduling Interface ***********************/
  /* DL buffer status report */
  virtual int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) = 0; 
  virtual int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code) = 0; 
    
  /* DL information */
  virtual int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack) = 0;
  virtual int dl_rach_info(uint32_t tti, uint32_t ra_id, uint16_t rnti, uint32_t estimated_size) = 0; 
  virtual int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value) = 0; 
  virtual int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value) = 0;
  virtual int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value) = 0; 
  
  /* UL information */
  virtual int ul_crc_info(uint32_t tti, uint16_t rnti, bool crc) = 0; 
  virtual int ul_sr_info(uint32_t tti, uint16_t rnti) = 0; 
  virtual int ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value = true) = 0;
  virtual int ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len) = 0; 
  virtual int ul_phr(uint16_t rnti, int phr) = 0; 
  virtual int ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code) = 0; 
  
  /* Run Scheduler for this tti */
  virtual int dl_sched(uint32_t tti, dl_sched_res_t *sched_result) = 0; 
  virtual int ul_sched(uint32_t tti, ul_sched_res_t *sched_result) = 0;

  /* Custom */
  virtual void set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) = 0;
};

}

#endif // SRSLTE_SCHED_INTERFACE_H
