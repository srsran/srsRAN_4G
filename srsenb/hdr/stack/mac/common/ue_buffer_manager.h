/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_UE_BUFFER_MANAGER_H
#define SRSRAN_UE_BUFFER_MANAGER_H

#include "sched_config.h"
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
  const static uint32_t     MAX_LC_ID     = isNR ? srsran::MAX_NR_NOF_BEARERS : srsran::MAX_LTE_LCID;
  const static uint32_t     MAX_LCG_ID    = isNR ? 7 : 3;
  const static uint32_t     MAX_SRB_LC_ID = isNR ? srsran::MAX_NR_SRB_ID : srsran::MAX_LTE_SRB_ID;
  const static uint32_t     MAX_NOF_LCIDS = MAX_LC_ID + 1;
  const static uint32_t     MAX_NOF_LCGS  = MAX_LCG_ID + 1;
  constexpr static uint32_t pbr_infinity  = -1;

public:
  explicit ue_buffer_manager(srslog::basic_logger& logger_) : logger(logger_) {}

  // Bearer configuration
  void config_lcid(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg);

  // Buffer Status update
  void ul_bsr(uint32_t lcg_id, uint32_t val);
  void dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue);

  // Configuration getters
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

  /// DL retx buffer status for given LCID (no RLC overhead included)
  int get_dl_retx(uint32_t lcid) const { return is_bearer_dl(lcid) ? channels[lcid].buf_retx : 0; }

  /// Sum of DL RLC newtx and retx buffer status for given LCID (no RLC overhead included)
  int get_dl_tx_total(uint32_t lcid) const { return get_dl_tx(lcid) + get_dl_retx(lcid); }

  /// Sum of DL RLC newtx and retx buffer status for all LCIDS
  int get_dl_tx_total() const;

  // UL BSR methods
  bool                                 is_lcg_active(uint32_t lcg) const;
  int                                  get_bsr(uint32_t lcg) const;
  int                                  get_bsr() const;
  const std::array<int, MAX_NOF_LCGS>& get_bsr_state() const { return lcg_bsr; }

  static bool is_lcid_valid(uint32_t lcid) { return lcid <= MAX_LC_ID; }
  static bool is_lcg_valid(uint32_t lcg) { return lcg <= MAX_LCG_ID; }

protected:
  srslog::basic_logger& logger;

  struct logical_channel {
    mac_lc_ch_cfg_t cfg;
    int             buf_tx      = 0;
    int             buf_retx    = 0;
    int             Bj          = 0;
    int             bucket_size = 0;
  };
  std::array<logical_channel, MAX_NOF_LCIDS> channels;

  std::array<int, MAX_NOF_LCGS> lcg_bsr{0};
};

} // namespace srsenb

#endif // SRSRAN_UE_BUFFER_MANAGER_H
