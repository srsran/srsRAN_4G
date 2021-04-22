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

#ifndef SRSRAN_SCHED_LCH_H
#define SRSRAN_SCHED_LCH_H

#include "srsran/interfaces/sched_interface.h"
#include "srsran/mac/pdu.h"
#include "srsran/srslog/srslog.h"
#include <deque>

namespace srsenb {

class lch_ue_manager
{
  constexpr static uint32_t pbr_infinity = -1;
  constexpr static uint32_t MAX_LC       = sched_interface::MAX_LC;

public:
  lch_ue_manager() : logger(srslog::fetch_basic_logger("MAC")) {}
  void set_cfg(const sched_interface::ue_cfg_t& cfg_);
  void new_tti();

  void config_lcid(uint32_t lcg_id, const sched_interface::ue_bearer_cfg_t& bearer_cfg);
  void ul_bsr(uint8_t lcg_id, uint32_t bsr);
  void ul_buffer_add(uint8_t lcid, uint32_t bytes);
  void dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue);

  int alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* lcid, int rem_bytes);

  bool is_bearer_active(uint32_t lcid) const;
  bool is_bearer_ul(uint32_t lcid) const;
  bool is_bearer_dl(uint32_t lcid) const;

  bool has_pending_dl_txs() const;
  int  get_dl_tx_total() const;
  int  get_dl_tx_total(uint32_t lcid) const { return get_dl_tx(lcid) + get_dl_retx(lcid); }
  int  get_dl_tx_total_with_overhead(uint32_t lcid) const;
  int  get_dl_tx(uint32_t lcid) const;
  int  get_dl_tx_with_overhead(uint32_t lcid) const;
  int  get_dl_retx(uint32_t lcid) const;
  int  get_dl_retx_with_overhead(uint32_t lcid) const;

  bool is_lcg_active(uint32_t lcg) const;
  int  get_bsr(uint32_t lcid) const;
  int  get_bsr_with_overhead(uint32_t lcid) const;
  int  get_max_prio_lcid() const;

  const std::array<int, 4>& get_bsr_state() const;

  // Control Element Command queue
  using ce_cmd = srsran::dl_sch_lcid;
  std::deque<ce_cmd> pending_ces;

private:
  struct ue_bearer_t {
    sched_interface::ue_bearer_cfg_t cfg         = {};
    int                              bucket_size = 0;
    int                              buf_tx      = 0;
    int                              buf_retx    = 0;
    int                              Bj          = 0;
  };

  int alloc_retx_bytes(uint8_t lcid, int rem_bytes);
  int alloc_tx_bytes(uint8_t lcid, int rem_bytes);

  size_t                                           prio_idx = 0;
  srslog::basic_logger&                            logger;
  std::array<ue_bearer_t, sched_interface::MAX_LC> lch     = {};
  std::array<int, 4>                               lcg_bsr = {};
};

/**
 * Allocate space for multiple MAC SDUs (i.e. RLC PDUs) and corresponding MAC SDU subheaders
 * @param data struct where the rlc pdu allocations are stored
 * @param total_tbs available TB size for allocations for a single UE
 * @param tbidx index of TB
 * @return allocated bytes, which is always equal or lower than total_tbs
 */
uint32_t allocate_mac_sdus(sched_interface::dl_sched_data_t* data,
                           lch_ue_manager&                   lch_handler,
                           uint32_t                          total_tbs,
                           uint32_t                          tbidx);

/**
 * Allocate space for pending MAC CEs
 * @param data struct where the MAC CEs allocations are stored
 * @param total_tbs available space in bytes for allocations
 * @return number of bytes allocated
 */
uint32_t allocate_mac_ces(sched_interface::dl_sched_data_t* data, lch_ue_manager& lch_handler, uint32_t total_tbs);

} // namespace srsenb

#endif // SRSRAN_SCHED_LCH_H
