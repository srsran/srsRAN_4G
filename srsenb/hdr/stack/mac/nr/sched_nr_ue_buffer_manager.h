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

#ifndef SRSRAN_SCHED_NR_UE_BUFFER_MANAGER_H
#define SRSRAN_SCHED_NR_UE_BUFFER_MANAGER_H

#include "srsenb/hdr/stack/mac/common/sched_config.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"

namespace srsenb {
namespace sched_nr_impl {

class ue_buffer_manager
{
  const static uint32_t MAX_LCG_ID    = 7;
  const static uint32_t MAX_LC_ID     = 32;
  const static uint32_t MAX_SRB_LC_ID = 3;

public:
  explicit ue_buffer_manager(srslog::basic_logger& logger_);

  // Configuration getters
  bool                         is_channel_active(uint32_t lcid) const { return get_cfg(lcid).is_active(); }
  bool                         is_bearer_ul(uint32_t lcid) const { return get_cfg(lcid).is_ul(); }
  bool                         is_bearer_dl(uint32_t lcid) const { return get_cfg(lcid).is_dl(); }
  const logical_channel_cfg_t& get_cfg(uint32_t lcid) const
  {
    srsran_assert(lcid < MAX_LC_ID, "Provided LCID=%d is above limit=%d", lcid, MAX_LC_ID);
    return channels[lcid].cfg;
  }

  // Buffer Status update
  void ul_bsr(uint32_t lcg_id, uint32_t val);
  void dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue);

  // UL BSR methods
  bool is_lcg_active(uint32_t lcg) const;
  int  get_bsr(uint32_t lcg) const;

private:
  srslog::basic_logger& logger;

  struct logical_channel {
    logical_channel_cfg_t cfg;
    int                   buf_tx   = 0;
    int                   buf_retx = 0;
  };
  std::array<logical_channel, MAX_LC_ID> channels;

  std::array<int, MAX_LCG_ID> lcg_bsr{0};
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_UE_BUFFER_MANAGER_H
