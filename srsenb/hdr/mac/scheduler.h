/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_SCHEDULER_H
#define SRSENB_SCHEDULER_H

#include <map>
#include "srslte/common/log.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include "scheduler_ue.h"
#include "scheduler_harq.h"
#include <pthread.h>

namespace srsenb {
  
 
class sched : public sched_interface
{  
  
  
public: 

  
  /*************************************************************
   * 
   * Scheduling metric interface definition
   * 
   ************************************************************/
  
  class metric_dl
  {
  public: 

    /* Virtual methods for user metric calculation */
    virtual void            new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t start_rb, uint32_t nof_rb, uint32_t nof_ctrl_symbols, uint32_t tti) = 0;
    virtual dl_harq_proc*   get_user_allocation(sched_ue *user) = 0;
  };

  
  class metric_ul
  {
  public: 

    /* Virtual methods for user metric calculation */
    virtual void           new_tti(std::map<uint16_t,sched_ue> &ue_db, uint32_t nof_rb, uint32_t tti) = 0;
    virtual ul_harq_proc*  get_user_allocation(sched_ue *user) = 0; 
    virtual void           update_allocation(ul_harq_proc::ul_alloc_t alloc) = 0;
  };

  
 
  /*************************************************************
   * 
   * FAPI-like Interface 
   * 
   ************************************************************/
  
  sched(); 
  ~sched();

  void init(rrc_interface_mac *rrc, srslte::log *log);
  void set_metric(metric_dl *dl_metric, metric_ul *ul_metric);
  int cell_cfg(cell_cfg_t *cell_cfg); 
  void set_sched_cfg(sched_args_t *sched_cfg);
  int reset();

  int ue_cfg(uint16_t rnti, ue_cfg_t *ue_cfg); 
  int ue_rem(uint16_t rnti);
  bool ue_exists(uint16_t rnti); 
  
  void phy_config_enabled(uint16_t rnti, bool enabled); 
  
  int bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, ue_bearer_cfg_t *cfg); 
  int bearer_ue_rem(uint16_t rnti, uint32_t lc_id); 

  uint32_t get_ul_buffer(uint16_t rnti); 
  uint32_t get_dl_buffer(uint16_t rnti); 

  int dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue);
  int dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code);
    
  int dl_ant_info(uint16_t rnti, LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT *dedicated);
  int dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack);
  int dl_rach_info(uint32_t tti, uint32_t ra_id, uint16_t rnti, uint32_t estimated_size); 
  int dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t ri_value);
  int dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value);
  int dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value); 
  
  int ul_crc_info(uint32_t tti, uint16_t rnti, bool crc);
  int ul_sr_info(uint32_t tti, uint16_t rnti); 
  int ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value = true);
  int ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len);
  int ul_phr(uint16_t rnti, int phr); 
  int ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code); 
    
  int dl_sched(uint32_t tti, dl_sched_res_t *sched_result);  
  int ul_sched(uint32_t tti, ul_sched_res_t *sched_result); 


  /* Custom TPC functions 
   */
  void tpc_inc(uint16_t rnti); 
  void tpc_dec(uint16_t rnti);


  
  static uint32_t get_rvidx(uint32_t retx_idx) {
    const static int rv_idx[4] = {0, 2, 3, 1}; 
    return rv_idx[retx_idx%4]; 
  }


  
  static void generate_cce_location(srslte_regs_t *regs, sched_ue::sched_dci_cce_t *location, 
                                    uint32_t cfi, uint32_t sf_idx = 0, uint16_t rnti = 0);     
  
private:
  
  metric_dl *dl_metric;
  metric_ul *ul_metric; 
  srslte::log *log_h; 
  rrc_interface_mac *rrc;
  
  cell_cfg_t cfg; 
  sched_args_t sched_cfg; 

  const static int MAX_PRB = 100; 
  const static int MAX_RBG = 25; 
  const static int MAX_CCE = 128; 

  // This is for computing DCI locations
  srslte_regs_t regs; 
  bool used_cce[MAX_CCE]; 
    
  typedef struct {
    int buf_rar; 
    uint16_t rnti; 
    uint32_t ra_id; 
    uint32_t rar_tti;    
  } sched_rar_t; 
  
  typedef struct {
    bool is_in_window;
    uint32_t window_start;
    uint32_t n_tx;
  } sched_sib_t;


  int  dl_sched_bc(dl_sched_bc_t bc[MAX_BC_LIST]); 
  int  dl_sched_rar(dl_sched_rar_t rar[MAX_RAR_LIST]); 
  int  dl_sched_data(dl_sched_data_t data[MAX_DATA_LIST]); 
    
  
  int  generate_format1a(uint32_t rb_start, uint32_t l_crb, uint32_t tbs, uint32_t rv, srslte_ra_dl_dci_t *dci);
  bool generate_dci(srslte_dci_location_t *sched_location, sched_ue::sched_dci_cce_t *locations, uint32_t aggr_level, sched_ue *user = NULL); 
 

  std::map<uint16_t, sched_ue>   ue_db;

  sched_sib_t pending_sibs[MAX_SIBS];
  
    
  typedef struct {
    bool enabled; 
    uint16_t rnti; 
    uint32_t L; 
    uint32_t n_prb; 
    uint32_t mcs; 
  } pending_msg3_t; 
 
  const static int SCHED_MAX_PENDING_RAR = 8; 
  sched_rar_t pending_rar[SCHED_MAX_PENDING_RAR];
  pending_msg3_t pending_msg3[10]; 
    
  // Allowed DCI locations for SIB and RAR per CFI
  sched_ue::sched_dci_cce_t common_locations[3];   
  sched_ue::sched_dci_cce_t rar_locations[3][10];   

  uint32_t bc_aggr_level; 
  uint32_t rar_aggr_level; 
  
  uint32_t pdsch_re[10];
  uint32_t avail_rbg;
  uint32_t P; 
  uint32_t start_rbg;
  uint32_t si_n_rbg;
  uint32_t rar_n_rb; 
  uint32_t nof_rbg; 
  uint32_t sf_idx;
  uint32_t sfn;
  uint32_t current_tti;
  uint32_t current_cfi;
  
  bool configured;
  
  pthread_mutex_t mutex, mutex2;
  
  
};


      


}

#endif // SRSENB_SCHEDULER_H
