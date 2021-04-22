/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/common/tti_point.h"
#include "srsran/interfaces/sched_interface.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

class harq_proc
{
public:
  harq_proc();
  void init(uint32_t id);
  void reset(uint32_t tb_idx);

  uint32_t get_id() const { return id; }
  bool     is_empty() const;
  bool     is_empty(uint32_t tb_idx) const;

  uint32_t          nof_tx(uint32_t tb_idx) const;
  uint32_t          nof_retx(uint32_t tb_idx) const;
  srsran::tti_point get_tti() const;
  bool              get_ndi(uint32_t tb_idx) const;
  uint32_t          max_nof_retx() const;

protected:
  void new_tx_common(uint32_t tb_idx, srsran::tti_point tti, int mcs, int tbs, uint32_t max_retx_);
  void new_retx_common(uint32_t tb_idx, srsran::tti_point tti, int* mcs, int* tbs);
  bool has_pending_retx_common(uint32_t tb_idx) const;
  int  set_ack_common(uint32_t tb_idx, bool ack);
  void reset_pending_data_common();

  enum ack_t { NACK, ACK };

  srslog::basic_logger*           logger;
  bool                            ack_state[SRSRAN_MAX_TB];
  bool                            active[SRSRAN_MAX_TB];
  std::array<bool, SRSRAN_MAX_TB> ndi = {};
  uint32_t                        id;
  uint32_t                        max_retx = 5;
  uint32_t                        n_rtx[SRSRAN_MAX_TB];
  uint32_t                        tx_cnt[SRSRAN_MAX_TB];
  srsran::tti_point               tti;
  int                             last_mcs[SRSRAN_MAX_TB];
  int                             last_tbs[SRSRAN_MAX_TB];
};

class dl_harq_proc : public harq_proc
{
public:
  dl_harq_proc();

  void new_tti(tti_point tti_tx_dl);

  void new_tx(const rbgmask_t& new_mask,
              uint32_t         tb_idx,
              tti_point        tti_tx_dl,
              int              mcs,
              int              tbs,
              uint32_t         n_cce_,
              uint32_t         max_retx);
  void new_retx(const rbgmask_t& new_mask, uint32_t tb_idx, tti_point tti_tx_dl, int* mcs, int* tbs, uint32_t n_cce_);
  int  set_ack(uint32_t tb_idx, bool ack);
  rbgmask_t get_rbgmask() const;
  bool      has_pending_retx(uint32_t tb_idx, tti_point tti_tx_dl) const;
  bool      has_pending_retx(tti_point tti_tx_dl) const;
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
  void new_tti();

  void new_tx(srsran::tti_point tti, int mcs, int tbs, prb_interval alloc, uint32_t max_retx_, bool is_msg3);
  void new_retx(srsran::tti_point tti_, int* mcs, int* tbs, prb_interval alloc);
  bool set_ack(uint32_t tb_idx, bool ack);
  bool retx_requires_pdcch(srsran::tti_point tti_, prb_interval alloc) const;

  prb_interval get_alloc() const;
  bool         has_pending_retx() const;
  bool         is_msg3() const { return is_msg3_; }

  void     reset_pending_data();
  uint32_t get_pending_data() const;
  bool     has_pending_phich() const;
  bool     pop_pending_phich();

private:
  prb_interval allocation;
  int          pending_data;
  bool         pending_phich = false;
  bool         is_msg3_      = false;
};

class harq_entity
{
public:
  static const bool is_async = ASYNC_DL_SCHED;

  harq_entity(size_t nof_dl_harqs, size_t nof_ul_harqs);

  void reset();
  void new_tti(tti_point tti_rx);

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
  dl_harq_proc* get_pending_dl_harq(tti_point tti_tx_dl);
  /**
   * Get empty DL Harq
   * @param tti_tx_dl only used in case of sync dl sched
   * @return pointer to found dl_harq
   */
  dl_harq_proc* get_empty_dl_harq(tti_point tti_tx_dl);

  /**
   * Set ACK state for DL Harq Proc
   * @param tti_rx tti the DL ACK was received
   * @param tb_idx TB index for the given ACK
   * @param ack true for ACK and false for NACK
   * @return pair with pid and size of TB of the DL harq that was ACKed
   */
  std::pair<uint32_t, int> set_ack_info(tti_point tti_rx, uint32_t tb_idx, bool ack);

  //! Get UL Harq for a given tti_tx_ul
  ul_harq_proc* get_ul_harq(tti_point tti_tx_ul);

  /**
   * Set ACK state for UL Harq Proc
   */
  int set_ul_crc(srsran::tti_point tti_tx_ul, uint32_t tb_idx, bool ack_);

  //! Resets pending harq ACKs and cleans UL Harqs with maxretx == 0
  void reset_pending_data(srsran::tti_point tti_rx);

private:
  dl_harq_proc* get_oldest_dl_harq(tti_point tti_tx_dl);

  std::array<tti_point, SRSRAN_FDD_NOF_HARQ> last_ttis;

  std::vector<dl_harq_proc> dl_harqs;
  std::vector<ul_harq_proc> ul_harqs;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_HARQ_H
