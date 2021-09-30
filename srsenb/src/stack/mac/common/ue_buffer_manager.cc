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

#include "srsenb/hdr/stack/mac/common/ue_buffer_manager.h"
#include "srsran/common/string_helpers.h"
#include "srsran/srslog/bundled/fmt/format.h"
#include "srsran/srslog/bundled/fmt/ranges.h"

namespace srsenb {

template <bool isNR>
ue_buffer_manager<isNR>::ue_buffer_manager(srslog::basic_logger& logger_) : logger(logger_)
{
  std::fill(lcg_bsr.begin(), lcg_bsr.end(), 0);
}

template <bool isNR>
void ue_buffer_manager<isNR>::config_lcid(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg)
{
  if (not is_lcid_valid(lcid)) {
    logger.warning("Configuring bearer with invalid logical channel id=%d", lcid);
    return;
  }
  if (not is_lcg_valid(bearer_cfg.group)) {
    logger.warning("Configuring bearer with invalid logical channel group id=%d", bearer_cfg.group);
    return;
  }

  // update bearer config
  if (bearer_cfg != channels[lcid].cfg) {
    channels[lcid].cfg = bearer_cfg;
    if (channels[lcid].cfg.pbr == pbr_infinity) {
      channels[lcid].bucket_size = std::numeric_limits<int>::max();
      channels[lcid].Bj          = std::numeric_limits<int>::max();
    } else {
      channels[lcid].bucket_size = channels[lcid].cfg.bsd * channels[lcid].cfg.pbr;
      channels[lcid].Bj          = 0;
    }
    logger.info("SCHED: bearer configured: lcid=%d, mode=%s, prio=%d, lcg=%d",
                lcid,
                to_string(channels[lcid].cfg.direction),
                channels[lcid].cfg.priority,
                channels[lcid].cfg.group);
  }
}

template <bool isNR>
int ue_buffer_manager<isNR>::get_dl_tx_total() const
{
  int sum = 0;
  for (size_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    sum += get_dl_tx_total(lcid);
  }
  return sum;
}

template <bool isNR>
bool ue_buffer_manager<isNR>::is_lcg_active(uint32_t lcg) const
{
  if (lcg == 0) {
    return true;
  }
  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (is_bearer_ul(lcid) and channels[lcid].cfg.group == (int)lcg) {
      return true;
    }
  }
  return false;
}

template <bool isNR>
int ue_buffer_manager<isNR>::get_bsr(uint32_t lcg) const
{
  return is_lcg_active(lcg) ? lcg_bsr[lcg] : 0;
}

template <bool isNR>
int ue_buffer_manager<isNR>::get_bsr() const
{
  uint32_t count = 0;
  for (uint32_t lcg = 0; is_lcg_valid(lcg); ++lcg) {
    if (is_lcg_active(lcg)) {
      count += lcg_bsr[lcg];
    }
  }
  return count;
}

template <bool isNR>
void ue_buffer_manager<isNR>::ul_bsr(uint32_t lcg_id, uint32_t val)
{
  if (not is_lcg_valid(lcg_id)) {
    logger.warning("The provided logical channel group id=%d is not valid", lcg_id);
    return;
  }
  lcg_bsr[lcg_id] = val;

  if (logger.debug.enabled()) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", lcg_bsr);
    logger.debug("SCHED: lcg_id=%d, bsr=%d. Current state=%s", lcg_id, val, srsran::to_c_str(str_buffer));
  }
}

template <bool isNR>
void ue_buffer_manager<isNR>::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue)
{
  if (not is_lcid_valid(lcid)) {
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

// Explicit instantiation
template class ue_buffer_manager<true>;
template class ue_buffer_manager<false>;

} // namespace srsenb
