/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/common/base_ue_buffer_manager.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/common/string_helpers.h"
#include "srsran/srslog/bundled/fmt/format.h"
#include "srsran/srslog/bundled/fmt/ranges.h"
extern "C" {
#include "srsran/config.h"
}

namespace srsenb {

template <bool isNR>
base_ue_buffer_manager<isNR>::base_ue_buffer_manager(uint16_t rnti_, srslog::basic_logger& logger_) :
  logger(logger_), rnti(rnti_)
{
  std::fill(lcg_bsr.begin(), lcg_bsr.end(), 0);
}

template <bool isNR>
void base_ue_buffer_manager<isNR>::config_lcids(srsran::const_span<mac_lc_ch_cfg_t> bearer_cfg_list)
{
  bool                                            log_enabled = logger.info.enabled();
  srsran::bounded_vector<uint32_t, MAX_NOF_LCIDS> changed_list;

  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (config_lcid_internal(lcid, bearer_cfg_list[lcid]) and log_enabled) {
      // add to the changed_list the lcids that have been updated with new parameters
      changed_list.push_back(lcid);
    }
  }

  // Log configurations of the LCIDs for which there were param updates
  if (not changed_list.empty()) {
    fmt::memory_buffer fmtbuf;
    for (uint32_t i = 0; i < changed_list.size(); ++i) {
      uint32_t lcid = changed_list[i];
      fmt::format_to(fmtbuf,
                     "{}{{lcid={}, mode={}, prio={}, lcg={}}}",
                     i > 0 ? ", " : "",
                     lcid,
                     to_string(channels[lcid].cfg.direction),
                     channels[lcid].cfg.priority,
                     channels[lcid].cfg.group);
    }
    logger.info("SCHED: rnti=0x%x, new lcid configuration: [%s]", rnti, srsran::to_c_str(fmtbuf));
  }
}

template <bool isNR>
void base_ue_buffer_manager<isNR>::config_lcid(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg)
{
  bool cfg_changed = config_lcid_internal(lcid, bearer_cfg);
  if (cfg_changed) {
    logger.info("SCHED: rnti=0x%x, lcid=%d configured: mode=%s, prio=%d, lcg=%d",
                rnti,
                lcid,
                to_string(channels[lcid].cfg.direction),
                channels[lcid].cfg.priority,
                channels[lcid].cfg.group);
  }
}

/**
 * @brief configure MAC logical channel. The function checks if the configuration is valid
 *        and whether there was any change compared to previous value
 * @return true if the lcid was updated with new parameters. False in case of case of error or no update.
 */
template <bool isNR>
bool base_ue_buffer_manager<isNR>::config_lcid_internal(uint32_t lcid, const mac_lc_ch_cfg_t& bearer_cfg)
{
  if (not is_lcid_valid(lcid)) {
    logger.warning("SCHED: Configuring rnti=0x%x bearer with invalid lcid=%d", rnti, lcid);
    return false;
  }
  if (not is_lcg_valid(bearer_cfg.group)) {
    logger.warning(
        "SCHED: Configuring rnti=0x%x bearer with invalid logical channel group id=%d", rnti, bearer_cfg.group);
    return false;
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
    return true;
  }
  return false;
}

template <bool isNR>
int base_ue_buffer_manager<isNR>::get_dl_tx_total() const
{
  int sum = 0;
  for (size_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    sum += get_dl_tx_total(lcid);
  }
  return sum;
}

template <bool isNR>
bool base_ue_buffer_manager<isNR>::is_lcg_active(uint32_t lcg) const
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
int base_ue_buffer_manager<isNR>::get_bsr(uint32_t lcg) const
{
  return is_lcg_active(lcg) ? lcg_bsr[lcg] : 0;
}

template <bool isNR>
int base_ue_buffer_manager<isNR>::get_bsr() const
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
int base_ue_buffer_manager<isNR>::ul_bsr(uint32_t lcg_id, uint32_t val)
{
  if (not is_lcg_valid(lcg_id)) {
    logger.warning("SCHED: The provided lcg_id=%d for rnti=0x%x is not valid", lcg_id, rnti);
    return SRSRAN_ERROR;
  }
  lcg_bsr[lcg_id] = val;
  return SRSRAN_SUCCESS;
}

template <bool isNR>
int base_ue_buffer_manager<isNR>::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t prio_tx_queue)
{
  if (not is_lcid_valid(lcid)) {
    logger.warning("The provided lcid=%d is not valid", lcid);
    return SRSRAN_ERROR;
  }
  channels[lcid].buf_prio_tx = prio_tx_queue;
  channels[lcid].buf_tx      = tx_queue;
  return SRSRAN_SUCCESS;
}

// Explicit instantiation
template class base_ue_buffer_manager<true>;
template class base_ue_buffer_manager<false>;

} // namespace srsenb
