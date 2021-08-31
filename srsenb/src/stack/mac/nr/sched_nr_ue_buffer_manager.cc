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

#include "srsenb/hdr/stack/mac/nr/sched_nr_ue_buffer_manager.h"
#include "srsran/common/string_helpers.h"
#include "srsran/srslog/bundled/fmt/format.h"
#include "srsran/srslog/bundled/fmt/ranges.h"

namespace srsenb {
namespace sched_nr_impl {

ue_buffer_manager::ue_buffer_manager(srslog::basic_logger& logger_) : logger(logger_) {}

bool ue_buffer_manager::is_lcg_active(uint32_t lcg) const
{
  if (lcg == 0) {
    return true;
  }
  for (uint32_t lcid = 0; lcid < MAX_LC_ID; ++lcid) {
    if (is_bearer_ul(lcid) and channels[lcid].cfg.group == (int)lcg) {
      return true;
    }
  }
  return false;
}

int ue_buffer_manager::get_bsr(uint32_t lcg) const
{
  return is_lcg_active(lcg) ? lcg_bsr[lcg] : 0;
}

void ue_buffer_manager::ul_bsr(uint32_t lcg_id, uint32_t val)
{
  srsran_assert(lcg_id < MAX_LCG_ID, "Provided LCG_ID=%d is above its limit=%d", lcg_id, MAX_LCG_ID);
  lcg_bsr[lcg_id] = val;

  if (logger.debug.enabled()) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", lcg_bsr);
    logger.debug("SCHED: lcg_id=%d, bsr=%s. Current state=%s", lcg_id, val, srsran::to_c_str(str_buffer));
  }
}

void ue_buffer_manager::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue)
{
  if (lcid >= MAX_LC_ID) {
    logger.warning("The provided lcid=%d is not valid", lcid);
    return;
  }
  if (lcid <= MAX_SRB_LC_ID and
      (channels[lcid].buf_tx != (int)tx_queue or channels[lcid].buf_retx != (int)retx_queue)) {
    logger.info("SCHED: DL lcid=%d buffer_state=%d,%d", lcid, tx_queue, retx_queue);
  } else {
    logger.debug("SCHED: DL lcid=%d buffer_state=%d,%d", lcid, tx_queue, retx_queue);
  }
  channels[lcid].buf_retx = retx_queue;
  channels[lcid].buf_tx   = tx_queue;
}

} // namespace sched_nr_impl
} // namespace srsenb
