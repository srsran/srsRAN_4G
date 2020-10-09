/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srslte/adt/bounded_bitset.h"
#include "srslte/common/log.h"
#include "srslte/common/tti_point.h"
#include "srslte/interfaces/sched_interface.h"
#include <map>

namespace srsenb {

class harq_proc
{
public:
  void     init(uint32_t id);
  void     set_cfg(uint32_t max_retx);
  void     reset(uint32_t tb_idx);
  uint32_t get_id() const;
  bool     is_empty() const;
  bool     is_empty(uint32_t tb_idx) const;

  uint32_t          nof_tx(uint32_t tb_idx) const;
  uint32_t          nof_retx(uint32_t tb_idx) const;
  srslte::tti_point get_tti() const;
  bool              get_ndi(uint32_t tb_idx) const;
  uint32_t          max_nof_retx() const;

protected:
  void new_tx_common(uint32_t tb_idx, srslte::tti_point tti, int mcs, int tbs);
  void new_retx_common(uint32_t tb_idx, srslte::tti_point tti, int* mcs, int* tbs);
  bool has_pending_retx_common(uint32_t tb_idx) const;
  int  set_ack_common(uint32_t tb_idx, bool ack);
  void reset_pending_data_common();

  enum ack_t { NULL_ACK, NACK, ACK };

  ack_t                           ack_state[SRSLTE_MAX_TB];
  bool                            active[SRSLTE_MAX_TB];
  std::array<bool, SRSLTE_MAX_TB> ndi = {};
  uint32_t                        id;
  uint32_t                        max_retx = 5;
  uint32_t                        n_rtx[SRSLTE_MAX_TB];
  uint32_t                        tx_cnt[SRSLTE_MAX_TB];
  srslte::tti_point               tti;
  int                             last_mcs[SRSLTE_MAX_TB];
  int                             last_tbs[SRSLTE_MAX_TB];

  srslte::log_ref log_h;
};

class dl_harq_proc : public harq_proc
{
public:
  dl_harq_proc();
  void      new_tx(const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti, int mcs, int tbs, uint32_t n_cce_);
  void      new_retx(const rbgmask_t& new_mask, uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, uint32_t n_cce_);
  int       set_ack(uint32_t tb_idx, bool ack);
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
  void new_tx(uint32_t tti, int mcs, int tbs, prb_interval alloc, uint32_t max_retx_);
  void new_retx(uint32_t tb_idx, uint32_t tti_, int* mcs, int* tbs, prb_interval alloc);
  bool set_ack(uint32_t tb_idx, bool ack);

  prb_interval get_alloc() const;
  bool         has_pending_retx() const;
  bool         is_adaptive_retx() const;

  void     reset_pending_data();
  bool     has_pending_ack() const;
  bool     get_pending_ack() const;
  uint32_t get_pending_data() const;

private:
  prb_interval allocation;
  int          pending_data;
  bool         is_adaptive;
  ack_t        pending_ack;
};

class harq_entity
{
public:
  static const bool is_async = ASYNC_DL_SCHED;

  harq_entity(size_t nof_dl_harqs, size_t nof_ul_harqs);
  void reset();
  void set_cfg(uint32_t max_retx);

  size_t                           nof_dl_harqs() const { return dl_harqs.size(); }
  size_t                           nof_ul_harqs() const { return ul_harqs.size(); }
  std::vector<dl_harq_proc>&       dl_harq_procs() { return dl_harqs; }
  const std::vector<dl_harq_proc>& dl_harq_procs() const { return dl_harqs; }
  std::vector<ul_harq_proc>&       ul_harq_procs() { return ul_harqs; }

  /**
   * Get the DL harq proc based on tti_tx_dl
   * @param tti_tx_dl assumed to always be equal or ahead in time in comparison to current harqs
   * @return pointer to found dl_harq
   */
  dl_harq_proc* get_pending_dl_harq(uint32_t tti_tx_dl);
  /**
   * Get empty DL Harq
   * @param tti_tx_dl only used in case of sync dl sched
   * @return pointer to found dl_harq
   */
  dl_harq_proc* get_empty_dl_harq(uint32_t tti_tx_dl);

  /**
   * Set ACK state for DL Harq Proc
   * @param tti_rx tti the DL ACK was received
   * @param tb_idx TB index for the given ACK
   * @param ack true for ACK and false for NACK
   * @return pair with pid and size of TB of the DL harq that was ACKed
   */
  std::pair<uint32_t, int> set_ack_info(uint32_t tti_rx, uint32_t tb_idx, bool ack);

  //! Get UL Harq for a given tti_tx_ul
  ul_harq_proc* get_ul_harq(uint32_t tti_tx_ul);

  /**
   * Set ACK state for UL Harq Proc
   */
  std::pair<bool, uint32_t> set_ul_crc(srslte::tti_point tti_tx_ul, uint32_t tb_idx, bool ack_);

  //! Resets pending harq ACKs and cleans UL Harqs with maxretx == 0
  void reset_pending_data(srslte::tti_point tti_rx);

private:
  dl_harq_proc* get_oldest_dl_harq(uint32_t tti_tx_dl);

  srslte::log_ref log_h;

  std::vector<dl_harq_proc> dl_harqs;
  std::vector<ul_harq_proc> ul_harqs;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_HARQ_H
