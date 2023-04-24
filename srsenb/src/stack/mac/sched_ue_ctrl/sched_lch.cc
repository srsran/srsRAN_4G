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
  config_lcids(cfg.ue_bearers);
}

void lch_ue_manager::new_tti()
{
  prio_idx++;
  for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; ++lcid) {
    if (is_bearer_active(lcid)) {
      if (channels[lcid].cfg.pbr != pbr_infinity) {
        channels[lcid].Bj =
            std::min(channels[lcid].Bj + (int)(channels[lcid].cfg.pbr * tti_duration_ms), channels[lcid].bucket_size);
      }
    }
  }
}

void lch_ue_manager::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t prio_tx_queue)
{
  if (base_type::dl_buffer_state(lcid, tx_queue, prio_tx_queue) == SRSRAN_SUCCESS) {
    logger.debug("SCHED: rnti=0x%x DL lcid=%d buffer_state=%d,%d", rnti, lcid, tx_queue, prio_tx_queue);
  }
}

void lch_ue_manager::ul_bsr(uint32_t lcg_id, uint32_t val)
{
  if (base_type::ul_bsr(lcg_id, val) == SRSRAN_SUCCESS) {
    if (logger.debug.enabled()) {
      fmt::memory_buffer str_buffer;
      fmt::format_to(str_buffer, "{}", lcg_bsr);
      logger.debug(
          "SCHED: rnti=0x%x, lcg_id=%d, bsr=%d. Current state=%s", rnti, lcg_id, val, srsran::to_c_str(str_buffer));
    }
  }
}

void lch_ue_manager::ul_buffer_add(uint8_t lcid, uint32_t bytes)
{
  if (lcid >= sched_interface::MAX_LC) {
    logger.warning("SCHED: The provided lcid=%d for rnti=0x%x is not valid", lcid, rnti);
    return;
  }
  lcg_bsr[channels[lcid].cfg.group] += bytes;
  if (logger.debug.enabled()) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", get_bsr_state());
    logger.debug("SCHED: rnti=0x%x UL buffer update=%d, lcg_id=%d, bsr=%s",
                 rnti,
                 bytes,
                 channels[lcid].cfg.group,
                 srsran::to_c_str(str_buffer));
  }
}

int lch_ue_manager::get_max_prio_lcid() const
{
  int min_prio_val = std::numeric_limits<int>::max(), prio_lcid = -1;

  // Prioritized Txs first (e.g. Retxs, status PDUs)
  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (get_dl_prio_tx(lcid) > 0 and channels[lcid].cfg.priority < min_prio_val) {
      min_prio_val = channels[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Select lcid with new txs using Bj
  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (get_dl_tx(lcid) > 0 and channels[lcid].Bj > 0 and channels[lcid].cfg.priority < min_prio_val) {
      min_prio_val = channels[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Disregard Bj
  size_t                              nof_lcids    = 0;
  std::array<uint32_t, MAX_NOF_LCIDS> chosen_lcids = {};
  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (get_dl_tx_total(lcid) > 0) {
      if (channels[lcid].cfg.priority < min_prio_val) {
        min_prio_val    = channels[lcid].cfg.priority;
        chosen_lcids[0] = lcid;
        nof_lcids       = 1;
      } else if (channels[lcid].cfg.priority == min_prio_val) {
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

  // try first to allocate high priority txs (e.g. retxs, status pdus)
  alloc_bytes = alloc_prio_tx_bytes(lcid, rem_bytes);

  // if no prio tx alloc, try newtx
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

int lch_ue_manager::alloc_prio_tx_bytes(uint8_t lcid, int rem_bytes)
{
  const int rlc_overhead = (lcid == 0) ? 0 : RLC_MAX_HEADER_SIZE_NO_LI;
  if (rem_bytes <= rlc_overhead) {
    return 0;
  }
  int rem_bytes_no_header = rem_bytes - rlc_overhead;
  int alloc               = std::min(rem_bytes_no_header, get_dl_prio_tx(lcid));
  channels[lcid].buf_prio_tx -= alloc;
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
  channels[lcid].buf_tx -= alloc;
  if (alloc > 0 and channels[lcid].cfg.pbr != pbr_infinity) {
    // Update Bj
    channels[lcid].Bj -= alloc;
  }
  return alloc + (alloc > 0 ? rlc_overhead : 0);
}

bool lch_ue_manager::has_pending_dl_txs() const
{
  if (not pending_ces.empty()) {
    return true;
  }
  for (uint32_t lcid = 0; is_lcid_valid(lcid); ++lcid) {
    if (get_dl_tx_total(lcid) > 0) {
      return true;
    }
  }
  return false;
}

int lch_ue_manager::get_dl_tx_total_with_overhead(uint32_t lcid) const
{
  return get_dl_prio_tx_with_overhead(lcid) + get_dl_tx_with_overhead(lcid);
}

int lch_ue_manager::get_dl_tx_with_overhead(uint32_t lcid) const
{
  return get_dl_mac_sdu_size_with_overhead(lcid, get_dl_tx(lcid));
}

int lch_ue_manager::get_dl_prio_tx_with_overhead(uint32_t lcid) const
{
  return get_dl_mac_sdu_size_with_overhead(lcid, get_dl_prio_tx(lcid));
}

int lch_ue_manager::get_bsr_with_overhead(uint32_t lcg) const
{
  return get_ul_mac_sdu_size_with_overhead(get_bsr(lcg));
}

uint32_t allocate_mac_sdus(sched_interface::dl_sched_data_t* data,
                           lch_ue_manager&                   lch_handler,
                           uint32_t                          total_tbs,
                           uint32_t                          tbidx)
{
  uint32_t  rem_tbs       = total_tbs;
  auto&     pdu           = data->pdu[tbidx];
  uint32_t& nof_pdu_elems = data->nof_pdu_elems[tbidx];

  // if we do not have enough bytes to fit MAC subheader, skip MAC SDU allocation
  // NOTE: we do not account RLC header because some LCIDs (e.g. CCCH) do not need them
  uint32_t first_pdu_idx = nof_pdu_elems;
  while (rem_tbs > MAC_MAX_HEADER_SIZE and data->nof_pdu_elems[tbidx] < sched_interface::MAX_RLC_PDU_LIST) {
    uint32_t max_sdu_bytes   = rem_tbs - get_mac_subheader_size(rem_tbs - MAC_MIN_HEADER_SIZE);
    uint32_t alloc_sdu_bytes = lch_handler.alloc_rlc_pdu(&pdu[nof_pdu_elems], max_sdu_bytes);
    if (alloc_sdu_bytes == 0) {
      break;
    }
    rem_tbs -= get_mac_sdu_and_subheader_size(alloc_sdu_bytes); // account for MAC sub-header

    // In case the same LCID got reallocated (e.g. retx and newtx), merge with previous SDU.
    // Otherwise, increment number of scheduled SDUs
    uint32_t prev_same_lcid_idx = first_pdu_idx;
    for (; prev_same_lcid_idx < nof_pdu_elems; ++prev_same_lcid_idx) {
      if (pdu[prev_same_lcid_idx].lcid == pdu[nof_pdu_elems].lcid) {
        pdu[prev_same_lcid_idx].nbytes += pdu[nof_pdu_elems].nbytes;
        pdu[nof_pdu_elems].nbytes = 0;
        break;
      }
    }
    if (prev_same_lcid_idx == nof_pdu_elems) {
      nof_pdu_elems++;
    }
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
