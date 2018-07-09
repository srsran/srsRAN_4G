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

#ifndef SRSENB_SCHEDULER_HARQ_H
#define SRSENB_SCHEDULER_HARQ_H

#include <map>
#include "srslte/common/log.h"
#include "srslte/interfaces/sched_interface.h"

namespace srsenb {

class harq_proc 
{
public:
  void     config(uint32_t id, uint32_t max_retx, srslte::log* log_h);    
  void     set_max_retx(uint32_t max_retx); 
  void     reset(uint32_t tb_idx);
  uint32_t get_id(); 
  bool     is_empty(uint32_t tb_idx);
  
  void     new_retx(uint32_t tb_idx, uint32_t tti, int *mcs, int *tbs);
  
  bool     get_ack(uint32_t tb_idx);
  void     set_ack(uint32_t tb_idx, bool ack);
  
  uint32_t nof_tx(uint32_t tb_idx);
  uint32_t nof_retx(uint32_t tb_idx);
  uint32_t get_tti();
  bool     get_ndi(uint32_t tb_idx);
  
protected:

  void     new_tx_common(uint32_t tb_idx, uint32_t tti, int mcs, int tbs);
  bool     has_pending_retx_common(uint32_t tb_idx);
  
  bool     ack[SRSLTE_MAX_TB];
  bool     active[SRSLTE_MAX_TB];
  bool     ndi[SRSLTE_MAX_TB];
  uint32_t id;  
  uint32_t max_retx; 
  uint32_t n_rtx[SRSLTE_MAX_TB];
  uint32_t tx_cnt[SRSLTE_MAX_TB];
  int      tti;
  int      last_mcs[SRSLTE_MAX_TB];
  int      last_tbs[SRSLTE_MAX_TB];
  
  srslte::log* log_h;

  private:
    bool ack_received[SRSLTE_MAX_TB];
}; 

class dl_harq_proc : public harq_proc
{
public:
  void     new_tx(uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce);
  uint32_t get_rbgmask();
  void     set_rbgmask(uint32_t new_mask); 
  bool     has_pending_retx(uint32_t tb_idx, uint32_t tti);
  int      get_tbs(uint32_t tb_idx);
  uint32_t get_n_cce();
private:
  uint32_t rbgmask;     
  uint32_t nof_rbg; 
  uint32_t n_cce;
};

class ul_harq_proc : public harq_proc
{
public:
  
  struct ul_alloc_t {
    uint32_t RB_start;
    uint32_t L;
    inline void set(uint32_t start, uint32_t len) {RB_start = start; L = len;}
  };
  
  void       new_tx(uint32_t tti, int mcs, int tbs);
  
  ul_alloc_t get_alloc();
  void       set_alloc(ul_alloc_t alloc);
  void       re_alloc(ul_alloc_t alloc);
  bool       is_adaptive_retx();

  void       reset_pending_data();
  bool       has_pending_ack();
  uint32_t   get_pending_data(); 
  
  void       set_rar_mcs(uint32_t mcs);
  bool       get_rar_mcs(int* mcs);
  
private:
  ul_alloc_t allocation;       
  bool need_ack;
  int  pending_data;
  uint32_t rar_mcs;
  bool has_rar_mcs;
  bool is_adaptive;
  bool is_msg3;
};
  
}      


#endif // SRSENB_SCHEDULER_HARQ_H
