/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/common/base_ue_buffer_manager.h"
#include "srsenb/hdr/stack/mac/sched_interface.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/mac/pdu.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

class lch_ue_manager : private base_ue_buffer_manager<false>
{
  using base_type = base_ue_buffer_manager<false>;

public:
  explicit lch_ue_manager(uint16_t rnti) : base_ue_buffer_manager(rnti, srslog::fetch_basic_logger("MAC")) {}
  void set_cfg(const sched_interface::ue_cfg_t& cfg_);
  void new_tti();

  // Inherited methods from ue_buffer_manager base class
  using base_type::config_lcid;
  using base_type::get_bsr;
  using base_type::get_bsr_state;
  using base_type::get_dl_prio_tx;
  using base_type::get_dl_tx;
  using base_type::get_dl_tx_total;
  using base_type::is_bearer_active;
  using base_type::is_bearer_dl;
  using base_type::is_bearer_ul;
  using base_type::is_lcg_active;

  void dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t prio_tx_queue);
  void ul_bsr(uint32_t lcg_id, uint32_t val);
  void ul_buffer_add(uint8_t lcid, uint32_t bytes);

  int alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* lcid, int rem_bytes);

  bool has_pending_dl_txs() const;
  int  get_dl_tx_total_with_overhead(uint32_t lcid) const;
  int  get_dl_tx_with_overhead(uint32_t lcid) const;
  int  get_dl_prio_tx_with_overhead(uint32_t lcid) const;

  int get_bsr_with_overhead(uint32_t lcid) const;
  int get_max_prio_lcid() const;

  // Control Element Command queue
  using ce_cmd = srsran::dl_sch_lcid;
  srsran::deque<ce_cmd> pending_ces;

private:
  int alloc_prio_tx_bytes(uint8_t lcid, int rem_bytes);
  int alloc_tx_bytes(uint8_t lcid, int rem_bytes);

  size_t prio_idx = 0;
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
