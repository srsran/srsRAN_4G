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

#ifndef SRSRAN_UE_BUFFER_MANAGER_H
#define SRSRAN_UE_BUFFER_MANAGER_H

#include "sched_config.h"
#include "srsran/adt/span.h"
#include "srsran/common/common_lte.h"
#include "srsran/common/common_nr.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"

namespace srsenb {

/**
 * Class to handle UE DL+UL RLC and MAC buffers state
 */
template <bool isNR>
class ue_buffer_manager
{
protected:
  const static uint32_t     MAX_LC_ID     = isNR ? (srsran::MAX_NR_NOF_BEARERS - 1) : srsran::MAX_LTE_LCID;
  const static uint32_t     MAX_LCG_ID    = isNR ? 7 : 3; // Should import from sched_interface and sched_nr_interface
  const static uint32_t     MAX_SRB_LC_ID = isNR ? srsran::MAX_NR_SRB_ID : srsran::MAX_LTE_SRB_ID;
  const static uint32_t     MAX_NOF_LCIDS = MAX_LC_ID + 1;
  const static uint32_t     MAX_NOF_LCGS  = MAX_LCG_ID + 1;
  constexpr static uint32_t pbr_infinity  = -1;

public:
  explicit ue_buffer_manager(uint16_t rnti, srslog::basic_logger& logger_);

  // Bearer configuration
  void config_lcids(srsran::const_span<mac_lc_ch_cfg_t> bearer_cfg_list);
  void config_lcid(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg);

  // Buffer Status update
  void ul_bsr(uint32_t lcg_id, uint32_t val);
  void dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t prio_tx_queue);

  // Configuration getters
  uint16_t               get_rnti() const { return rnti; }
  bool                   is_bearer_active(uint32_t lcid) const { return get_cfg(lcid).is_active(); }
  bool                   is_bearer_ul(uint32_t lcid) const { return get_cfg(lcid).is_ul(); }
  bool                   is_bearer_dl(uint32_t lcid) const { return get_cfg(lcid).is_dl(); }
  const mac_lc_ch_cfg_t& get_cfg(uint32_t lcid) const
  {
    srsran_assert(is_lcid_valid(lcid), "Provided LCID=%d is above limit=%d", lcid, MAX_LC_ID);
    return channels[lcid].cfg;
  }

  /// DL newtx buffer status for given LCID (no RLC overhead included)
  int get_dl_tx(uint32_t lcid) const { return is_bearer_dl(lcid) ? channels[lcid].buf_tx : 0; }

  /// DL high prio tx buffer status for given LCID (no RLC overhead included)
  int get_dl_prio_tx(uint32_t lcid) const { return is_bearer_dl(lcid) ? channels[lcid].buf_prio_tx : 0; }

  /// Sum of DL RLC newtx and high prio tx buffer status for given LCID (no RLC overhead included)
  int get_dl_tx_total(uint32_t lcid) const { return get_dl_tx(lcid) + get_dl_prio_tx(lcid); }

  /// Sum of DL RLC newtx and high prio buffer status for all LCIDS
  int get_dl_tx_total() const;

  // UL BSR methods
  bool                                 is_lcg_active(uint32_t lcg) const;
  int                                  get_bsr(uint32_t lcg) const;
  int                                  get_bsr() const;
  const std::array<int, MAX_NOF_LCGS>& get_bsr_state() const { return lcg_bsr; }

  static bool is_lcid_valid(uint32_t lcid) { return lcid <= MAX_LC_ID; }
  static bool is_lcg_valid(uint32_t lcg) { return lcg <= MAX_LCG_ID; }

protected:
  bool config_lcid_internal(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg);

  srslog::basic_logger& logger;
  uint16_t              rnti;

  struct logical_channel {
    mac_lc_ch_cfg_t cfg;
    int             buf_tx      = 0;
    int             buf_prio_tx = 0;
    int             Bj          = 0;
    int             bucket_size = 0;
  };
  std::array<logical_channel, MAX_NOF_LCIDS> channels;

  std::array<int, MAX_NOF_LCGS> lcg_bsr;
};

} // namespace srsenb

#endif // SRSRAN_UE_BUFFER_MANAGER_H
