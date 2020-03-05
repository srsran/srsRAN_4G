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

#ifndef SRSENB_SCHEDULER_HARQ_H
#define SRSENB_SCHEDULER_HARQ_H

#include "srslte/common/bounded_bitset.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>

namespace srsenb {

// MASK used for CCE allocations
typedef srslte::bounded_bitset<sched_interface::max_cce, true> pdcch_mask_t;

// Range of RBGs
class prb_range_t;
struct rbg_range_t {
  uint32_t rbg_start = 0, rbg_end = 0;
  rbg_range_t() = default;
  rbg_range_t(uint32_t s, uint32_t e) : rbg_start(s), rbg_end(e) {}
  rbg_range_t(const prb_range_t& rbgs, uint32_t P);
  uint32_t length() const { return rbg_end - rbg_start; }
};

// Range of PRBs
class prb_range_t
{
public:
  uint32_t prb_start = 0, prb_end = 0;
  prb_range_t() = default;
  prb_range_t(uint32_t s, uint32_t e) : prb_start(s), prb_end(e) {}
  prb_range_t(const rbg_range_t& rbgs, uint32_t P);
  uint32_t           length() { return prb_end - prb_start; }
  static prb_range_t riv_to_prbs(uint32_t riv, uint32_t nof_prbs, int nof_vrbs = -1);
};

class harq_proc
{
public:
  void     init(uint32_t id);
  void     set_cfg(uint32_t max_retx);
  void     reset(uint32_t tb_idx);
  uint32_t get_id() const;
  bool     is_empty() const;
  bool     is_empty(uint32_t tb_idx) const;

  uint32_t nof_tx(uint32_t tb_idx) const;
  uint32_t nof_retx(uint32_t tb_idx) const;
  uint32_t get_tti() const;
  bool     get_ndi(uint32_t tb_idx) const;
  uint32_t max_nof_retx() const;

protected:
  void new_tx_common(uint32_t tb_idx, uint32_t tti, int mcs, int tbs);
  void new_retx_common(uint32_t tb_idx, uint32_t tti, int* mcs, int* tbs);
  bool has_pending_retx_common(uint32_t tb_idx) const;
  void set_ack_common(uint32_t tb_idx, bool ack);
  void reset_pending_data_common();

  enum ack_t { NULL_ACK, NACK, ACK };

  ack_t                           ack_state[SRSLTE_MAX_TB];
  bool                            active[SRSLTE_MAX_TB];
  std::array<bool, SRSLTE_MAX_TB> ndi = {};
  uint32_t                        id;
  uint32_t                        max_retx = 5;
  uint32_t                        n_rtx[SRSLTE_MAX_TB];
  uint32_t                        tx_cnt[SRSLTE_MAX_TB];
  int                             tti;
  int                             last_mcs[SRSLTE_MAX_TB];
  int                             last_tbs[SRSLTE_MAX_TB];

  srslte::log_ref log_h;
};

typedef srslte::bounded_bitset<25, true> rbgmask_t;

class dl_harq_proc : public harq_proc
{
public:
  dl_harq_proc();
  void      new_tx(const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce_);
  void      new_retx(const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, uint32_t n_cce_);
  void      set_ack(uint32_t tb_idx, bool ack);
  rbgmask_t get_rbgmask() const;
  bool      has_pending_retx(uint32_t tb_idx, uint32_t tti) const;
  int       get_tbs(uint32_t tb_idx) const;
  uint32_t  get_n_cce() const;
  void      reset_pending_data();

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
    uint32_t RB_end() const { return RB_start + L; }
  };

  void new_tx(uint32_t tti, int mcs, int tbs, ul_alloc_t alloc, uint32_t max_retx_);
  void new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, ul_alloc_t alloc);
  void set_ack(uint32_t tb_idx, bool ack);

  ul_alloc_t get_alloc() const;
  bool       has_pending_retx() const;
  bool       is_adaptive_retx() const;

  void     reset_pending_data();
  bool     has_pending_ack() const;
  bool     get_pending_ack() const;
  uint32_t get_pending_data() const;

private:
  ul_alloc_t allocation;
  int        pending_data;
  bool       is_adaptive;
  ack_t      pending_ack;
};

typedef srslte::bounded_bitset<100, true> prbmask_t;

class dl_harq_entity : private std::vector<dl_harq_proc>
{
  using base_t = std::vector<dl_harq_proc>;

public:
  static const bool is_async = ASYNC_DL_SCHED;

  using base_t::const_iterator;
  using base_t::iterator;
  using base_t::operator[];
  using base_t::begin;
  using base_t::data;
  using base_t::end;
  using base_t::size;

  explicit dl_harq_entity(size_t nof_harqs) : base_t(nof_harqs) {}

  /**
   * Get the DL harq proc based on tti_tx_dl
   * @param tti_tx_dl assumed to always be equal or ahead in time in comparison to current harqs
   * @return pointer to found dl_harq
   */
  dl_harq_proc* get_pending_harq(uint32_t tti_tx_dl);
  /**
   * Get empty DL Harq
   * @param tti_tx_dl only used in case of sync dl sched
   * @return pointer to found dl_harq
   */
  dl_harq_proc* get_empty_harq(uint32_t tti_tx_dl);

private:
  dl_harq_proc* get_oldest_harq(uint32_t tti_tx_dl);
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_HARQ_H
