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

#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_lch.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/common/string_helpers.h"
#include "srsran/srslog/bundled/fmt/ranges.h"

namespace srsenb {

/******************************************************
 *                 Helper Functions                   *
 ******************************************************/

#define RLC_MAX_HEADER_SIZE_NO_LI 3
#define MAC_MAX_HEADER_SIZE 3
#define MAC_MIN_HEADER_SIZE 2
#define MAC_MIN_ALLOC_SIZE RLC_MAX_HEADER_SIZE_NO_LI + MAC_MIN_HEADER_SIZE

/// TS 36.321 sec 7.1.2 - MAC PDU subheader is 2 bytes if L<=128 and 3 otherwise
uint32_t get_mac_subheader_size(uint32_t sdu_bytes)
{
  return sdu_bytes == 0 ? 0 : (sdu_bytes > 128 ? 3 : 2);
}
uint32_t get_mac_sdu_and_subheader_size(uint32_t sdu_bytes)
{
  return sdu_bytes + get_mac_subheader_size(sdu_bytes);
}
uint32_t get_dl_mac_sdu_size_with_overhead(uint32_t lcid, uint32_t rlc_pdu_bytes)
{
  uint32_t overhead = (lcid == 0 or rlc_pdu_bytes == 0) ? 0 : RLC_MAX_HEADER_SIZE_NO_LI;
  return get_mac_sdu_and_subheader_size(overhead + rlc_pdu_bytes);
}
uint32_t get_ul_mac_sdu_size_with_overhead(uint32_t rlc_pdu_bytes)
{
  if (rlc_pdu_bytes == 0) {
    return 0;
  }
  uint32_t overhead = get_mac_subheader_size(rlc_pdu_bytes + RLC_MAX_HEADER_SIZE_NO_LI);
  return overhead + rlc_pdu_bytes;
}

/*******************************************************
 *
 *         Logical Channel Management
 *
 *******************************************************/

void lch_ue_manager::set_cfg(const sched_interface::ue_cfg_t& cfg)
{
  for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; lcid++) {
    config_lcid(lcid, cfg.ue_bearers[lcid]);
  }
}

void lch_ue_manager::new_tti()
{
  prio_idx++;
  for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; ++lcid) {
    if (is_bearer_active(lcid)) {
      if (lch[lcid].cfg.pbr != pbr_infinity) {
        lch[lcid].Bj = std::min(lch[lcid].Bj + (int)(lch[lcid].cfg.pbr * tti_duration_ms), lch[lcid].bucket_size);
      }
    }
  }
}

void lch_ue_manager::config_lcid(uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& bearer_cfg)
{
  if (lc_id >= sched_interface::MAX_LC) {
    logger.warning("Adding bearer with invalid logical channel id=%d", lc_id);
    return;
  }
  if (bearer_cfg.group >= sched_interface::MAX_LC_GROUP) {
    logger.warning("Adding bearer with invalid logical channel group id=%d", bearer_cfg.group);
    return;
  }

  // update bearer config
  bool is_equal = memcmp(&bearer_cfg, &lch[lc_id].cfg, sizeof(bearer_cfg)) == 0;

  if (not is_equal) {
    lch[lc_id].cfg = bearer_cfg;
    if (lch[lc_id].cfg.pbr == pbr_infinity) {
      lch[lc_id].bucket_size = std::numeric_limits<int>::max();
      lch[lc_id].Bj          = std::numeric_limits<int>::max();
    } else {
      lch[lc_id].bucket_size = lch[lc_id].cfg.bsd * lch[lc_id].cfg.pbr;
      lch[lc_id].Bj          = 0;
    }
    logger.info("SCHED: bearer configured: lc_id=%d, mode=%s, prio=%d",
                lc_id,
                to_string(lch[lc_id].cfg.direction),
                lch[lc_id].cfg.priority);
  }
}

void lch_ue_manager::ul_bsr(uint8_t lcg_id, uint32_t bsr)
{
  if (lcg_id >= sched_interface::MAX_LC_GROUP) {
    logger.warning("The provided logical channel group id=%d is not valid", lcg_id);
    return;
  }
  lcg_bsr[lcg_id] = bsr;
  if (logger.debug.enabled()) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", get_bsr_state());
    logger.debug("SCHED: bsr=%d, lcg_id=%d, bsr=%s", bsr, lcg_id, srsran::to_c_str(str_buffer));
  }
}

void lch_ue_manager::ul_buffer_add(uint8_t lcid, uint32_t bytes)
{
  if (lcid >= sched_interface::MAX_LC) {
    logger.warning("The provided lcid=%d is not valid", lcid);
    return;
  }
  lcg_bsr[lch[lcid].cfg.group] += bytes;
  if (logger.debug.enabled()) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", get_bsr_state());
    logger.debug(
        "SCHED: UL buffer update=%d, lcg_id=%d, bsr=%s", bytes, lch[lcid].cfg.group, srsran::to_c_str(str_buffer));
  }
}

void lch_ue_manager::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue)
{
  if (lcid >= sched_interface::MAX_LC) {
    logger.warning("The provided lcid=%d is not valid", lcid);
    return;
  }
  if (lcid < 3 and (lch[lcid].buf_tx != (int)tx_queue or lch[lcid].buf_retx != (int)retx_queue)) {
    logger.info("SCHED: DL lcid=%d buffer_state=%d,%d", lcid, tx_queue, retx_queue);
  } else {
    logger.debug("SCHED: DL lcid=%d buffer_state=%d,%d", lcid, tx_queue, retx_queue);
  }
  lch[lcid].buf_retx = retx_queue;
  lch[lcid].buf_tx   = tx_queue;
}

int lch_ue_manager::get_max_prio_lcid() const
{
  int min_prio_val = std::numeric_limits<int>::max(), prio_lcid = -1;

  // Prioritize retxs
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_retx(lcid) > 0 and lch[lcid].cfg.priority < min_prio_val) {
      min_prio_val = lch[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Select lcid with new txs using Bj
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_tx(lcid) > 0 and lch[lcid].Bj > 0 and lch[lcid].cfg.priority < min_prio_val) {
      min_prio_val = lch[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Disregard Bj
  size_t                       nof_lcids    = 0;
  std::array<uint32_t, MAX_LC> chosen_lcids = {};
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_tx_total(lcid) > 0) {
      if (lch[lcid].cfg.priority < min_prio_val) {
        min_prio_val    = lch[lcid].cfg.priority;
        chosen_lcids[0] = lcid;
        nof_lcids       = 1;
      } else if (lch[lcid].cfg.priority == min_prio_val) {
        chosen_lcids[nof_lcids++] = lcid;
      }
    }
  }
  // logical chanels with equal priority should be served equally
  if (nof_lcids > 0) {
    prio_lcid = chosen_lcids[prio_idx % nof_lcids];
  }

  return prio_lcid;
}

/// Allocates first available RLC PDU
int lch_ue_manager::alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* rlc_pdu, int rem_bytes)
{
  int alloc_bytes = 0;
  int lcid        = get_max_prio_lcid();
  if (lcid < 0) {
    return alloc_bytes;
  }

  // try first to allocate retxs
  alloc_bytes = alloc_retx_bytes(lcid, rem_bytes);

  // if no retx alloc, try newtx
  if (alloc_bytes == 0) {
    alloc_bytes = alloc_tx_bytes(lcid, rem_bytes);
  }

  // If it is last PDU of the TBS, allocate all leftover bytes
  int leftover_bytes = rem_bytes - alloc_bytes;
  if (leftover_bytes > 0 and (leftover_bytes <= MAC_MIN_ALLOC_SIZE or get_dl_tx_total() == 0)) {
    alloc_bytes += leftover_bytes;
  }

  if (alloc_bytes > 0) {
    rlc_pdu->nbytes = alloc_bytes;
    rlc_pdu->lcid   = lcid;
  }
  return alloc_bytes;
}

int lch_ue_manager::alloc_retx_bytes(uint8_t lcid, int rem_bytes)
{
  const int rlc_overhead = (lcid == 0) ? 0 : RLC_MAX_HEADER_SIZE_NO_LI;
  if (rem_bytes <= rlc_overhead) {
    return 0;
  }
  int rem_bytes_no_header = rem_bytes - rlc_overhead;
  int alloc               = std::min(rem_bytes_no_header, get_dl_retx(lcid));
  lch[lcid].buf_retx -= alloc;
  return alloc + (alloc > 0 ? rlc_overhead : 0);
}

int lch_ue_manager::alloc_tx_bytes(uint8_t lcid, int rem_bytes)
{
  const int rlc_overhead = (lcid == 0) ? 0 : RLC_MAX_HEADER_SIZE_NO_LI;
  if (rem_bytes <= rlc_overhead) {
    return 0;
  }
  int rem_bytes_no_header = rem_bytes - rlc_overhead;
  int alloc               = std::min(rem_bytes_no_header, get_dl_tx(lcid));
  lch[lcid].buf_tx -= alloc;
  if (alloc > 0 and lch[lcid].cfg.pbr != pbr_infinity) {
    // Update Bj
    lch[lcid].Bj -= alloc;
  }
  return alloc + (alloc > 0 ? rlc_overhead : 0);
}

bool lch_ue_manager::is_bearer_active(uint32_t lcid) const
{
  return lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::IDLE;
}

bool lch_ue_manager::is_bearer_ul(uint32_t lcid) const
{
  return is_bearer_active(lcid) and lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::DL;
}

bool lch_ue_manager::is_bearer_dl(uint32_t lcid) const
{
  return is_bearer_active(lcid) and lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::UL;
}

bool lch_ue_manager::has_pending_dl_txs() const
{
  if (not pending_ces.empty()) {
    return true;
  }
  for (uint32_t lcid = 0; lcid < lch.size(); ++lcid) {
    if (get_dl_tx_total(lcid) > 0) {
      return true;
    }
  }
  return false;
}

int lch_ue_manager::get_dl_tx_total() const
{
  int sum = 0;
  for (size_t lcid = 0; lcid < lch.size(); ++lcid) {
    sum += get_dl_tx_total(lcid);
  }
  return sum;
}

int lch_ue_manager::get_dl_tx_total_with_overhead(uint32_t lcid) const
{
  return get_dl_retx_with_overhead(lcid) + get_dl_tx_with_overhead(lcid);
}

int lch_ue_manager::get_dl_tx(uint32_t lcid) const
{
  return is_bearer_dl(lcid) ? lch[lcid].buf_tx : 0;
}
int lch_ue_manager::get_dl_tx_with_overhead(uint32_t lcid) const
{
  return get_dl_mac_sdu_size_with_overhead(lcid, get_dl_tx(lcid));
}

int lch_ue_manager::get_dl_retx(uint32_t lcid) const
{
  return is_bearer_dl(lcid) ? lch[lcid].buf_retx : 0;
}
int lch_ue_manager::get_dl_retx_with_overhead(uint32_t lcid) const
{
  return get_dl_mac_sdu_size_with_overhead(lcid, get_dl_retx(lcid));
}

bool lch_ue_manager::is_lcg_active(uint32_t lcg) const
{
  if (lcg == 0) {
    return true;
  }
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (is_bearer_ul(lcid) and lch[lcid].cfg.group == (int)lcg) {
      return true;
    }
  }
  return false;
}
int lch_ue_manager::get_bsr(uint32_t lcg) const
{
  return is_lcg_active(lcg) ? lcg_bsr[lcg] : 0;
}
int lch_ue_manager::get_bsr_with_overhead(uint32_t lcg) const
{
  return get_ul_mac_sdu_size_with_overhead(get_bsr(lcg));
}

const std::array<int, 4>& lch_ue_manager::get_bsr_state() const
{
  return lcg_bsr;
}

uint32_t allocate_mac_sdus(sched_interface::dl_sched_data_t* data,
                           lch_ue_manager&                   lch_handler,
                           uint32_t                          total_tbs,
                           uint32_t                          tbidx)
{
  uint32_t rem_tbs = total_tbs;

  // if we do not have enough bytes to fit MAC subheader, skip MAC SDU allocation
  // NOTE: we do not account RLC header because some LCIDs (e.g. CCCH) do not need them
  while (rem_tbs > MAC_MAX_HEADER_SIZE and data->nof_pdu_elems[tbidx] < sched_interface::MAX_RLC_PDU_LIST) {
    uint32_t max_sdu_bytes   = rem_tbs - get_mac_subheader_size(rem_tbs - MAC_MIN_HEADER_SIZE);
    uint32_t alloc_sdu_bytes = lch_handler.alloc_rlc_pdu(&data->pdu[tbidx][data->nof_pdu_elems[tbidx]], max_sdu_bytes);
    if (alloc_sdu_bytes == 0) {
      break;
    }
    rem_tbs -= get_mac_sdu_and_subheader_size(alloc_sdu_bytes); // account for MAC sub-header
    data->nof_pdu_elems[tbidx]++;
  }

  return total_tbs - rem_tbs;
}

uint32_t allocate_mac_ces(sched_interface::dl_sched_data_t* data, lch_ue_manager& lch_handler, uint32_t total_tbs)
{
  int rem_tbs = total_tbs;
  while (not lch_handler.pending_ces.empty() and data->nof_pdu_elems[0] < sched_interface::MAX_RLC_PDU_LIST) {
    int toalloc = srsran::ce_total_size(lch_handler.pending_ces.front());
    if (rem_tbs < toalloc) {
      break;
    }
    data->pdu[0][data->nof_pdu_elems[0]].lcid = (uint32_t)lch_handler.pending_ces.front();
    data->nof_pdu_elems[0]++;
    rem_tbs -= toalloc;
    lch_handler.pending_ces.pop_front();
  }
  return total_tbs - rem_tbs;
}

} // namespace srsenb
