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

#include "srslte/common/bounded_bitset.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>

namespace srsenb {

class harq_proc 
{
public:
  void     config(uint32_t id, uint32_t max_retx, srslte::log* log_h);
  void     set_max_retx(uint32_t max_retx); 
  void     reset(uint32_t tb_idx);
  uint32_t get_id() const;
  bool     is_empty() const;
  bool     is_empty(uint32_t tb_idx) const;

  bool     get_ack(uint32_t tb_idx) const;
  void     set_ack(uint32_t tb_idx, bool ack);

  uint32_t nof_tx(uint32_t tb_idx) const;
  uint32_t nof_retx(uint32_t tb_idx) const;
  uint32_t get_tti() const;
  bool     get_ndi(uint32_t tb_idx) const;

protected:

  void     new_tx_common(uint32_t tb_idx, uint32_t tti, int mcs, int tbs);
  void     new_retx_common(uint32_t tb_idx, uint32_t tti, int* mcs, int* tbs);
  bool     has_pending_retx_common(uint32_t tb_idx) const;

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

typedef srslte::bounded_bitset<25, true> rbgmask_t;

class dl_harq_proc : public harq_proc
{
public:
  dl_harq_proc();
  void      new_tx(uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce);
  void      new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs);
  rbgmask_t get_rbgmask();
  void      set_rbgmask(rbgmask_t new_mask);
  bool      has_pending_retx(uint32_t tb_idx, uint32_t tti) const;
  int       get_tbs(uint32_t tb_idx) const;
  uint32_t get_n_cce();
private:
  rbgmask_t rbgmask;
  uint32_t  n_cce;
};

class ul_harq_proc : public harq_proc
{
public:
  
  struct ul_alloc_t {
    uint32_t RB_start;
    uint32_t L;
    void     set(uint32_t start, uint32_t len)
    {
      RB_start = start;
      L        = len;
    }
  };

  void       new_tx(uint32_t tti, int mcs, int tbs);
  void       new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs);

  ul_alloc_t get_alloc();
  void       set_alloc(ul_alloc_t alloc);
  void       set_realloc(ul_alloc_t alloc);
  bool       has_pending_retx();
  bool       is_adaptive_retx();
  bool       is_rar_tx();
  bool       is_new_tx();

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
  bool       is_rar;
};

class ul_mask_t : public srslte::bounded_bitset<100>
{
  typedef srslte::bounded_bitset<100> base_type;

public:
  using srslte::bounded_bitset<100>::any;
  using srslte::bounded_bitset<100>::fill;
  bool any(ul_harq_proc::ul_alloc_t alloc) const noexcept;
  void fill(ul_harq_proc::ul_alloc_t alloc) noexcept;
};
} // namespace srsenb

#endif // SRSENB_SCHEDULER_HARQ_H
