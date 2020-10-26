/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsue/hdr/stack/mac/proc_bsr.h"
#include "srslte/common/log_helper.h"
#include "srsue/hdr/stack/mac/mux.h"

namespace srsue {

bsr_proc::bsr_proc() {}

void bsr_proc::init(sr_proc*                       sr_,
                    rlc_interface_mac*             rlc_,
                    srslte::log_ref                log_h_,
                    srslte::ext_task_sched_handle* task_sched_)
{
  log_h      = log_h_;
  rlc        = rlc_;
  sr         = sr_;
  task_sched = task_sched_;

  timer_periodic           = task_sched->get_unique_timer();
  timer_retx               = task_sched->get_unique_timer();
  timer_queue_status_print = task_sched->get_unique_timer();

  reset();

  // Print periodically the LCID queue status
  auto queue_status_print_task = [this](uint32_t tid) {
    print_state();
    timer_queue_status_print.run();
  };
  timer_queue_status_print.set(QUEUE_STATUS_PERIOD_MS, queue_status_print_task);
  timer_queue_status_print.run();

  initiated = true;
}

void bsr_proc::print_state()
{
  char str[128];
  str[0] = '\0';
  int n  = 0;
  for (auto& lcg : lcgs) {
    for (auto& iter : lcg) {
      n = srslte_print_check(str, 128, n, "%d: %d ", iter.first, iter.second.old_buffer);
    }
  }
  Info("BSR:   triggered_bsr_type=%s, LCID QUEUE status: %s\n", bsr_type_tostring(triggered_bsr_type), str);
}

void bsr_proc::set_trigger(srsue::bsr_proc::triggered_bsr_type_t new_trigger)
{
  triggered_bsr_type = new_trigger;

  // Trigger SR always when Regular BSR is triggered in the current TTI. Will be cancelled if a grant is received
  if (triggered_bsr_type == REGULAR) {
    Debug("BSR:   Triggering SR procedure\n");
    sr->start();
  }
}

void bsr_proc::reset()
{
  timer_periodic.stop();
  timer_retx.stop();

  triggered_bsr_type = NONE;
}

void bsr_proc::set_config(srslte::bsr_cfg_t& bsr_cfg_)
{
  std::lock_guard<std::mutex> lock(mutex);

  bsr_cfg = bsr_cfg_;

  if (bsr_cfg_.periodic_timer > 0) {
    timer_periodic.set(bsr_cfg_.periodic_timer, [this](uint32_t tid) { timer_expired(tid); });
    Info("BSR:   Configured timer periodic %d ms\n", bsr_cfg_.periodic_timer);
  }
  if (bsr_cfg_.retx_timer > 0) {
    timer_retx.set(bsr_cfg_.retx_timer, [this](uint32_t tid) { timer_expired(tid); });
    Info("BSR:   Configured timer reTX %d ms\n", bsr_cfg_.retx_timer);
  }
}

/* Process Periodic BSR */
void bsr_proc::timer_expired(uint32_t timer_id)
{
  std::lock_guard<std::mutex> lock(mutex);

  // periodicBSR-Timer
  if (timer_id == timer_periodic.id()) {
    if (triggered_bsr_type == NONE) {
      set_trigger(PERIODIC);
      Debug("BSR:   Triggering Periodic BSR\n");
    }
    // retxBSR-Timer
  } else if (timer_id == timer_retx.id()) {
    // Enable reTx of SR only if periodic timer is not infinity
    Debug("BSR:   Timer BSR reTX expired, periodic=%d, channel=%d\n", bsr_cfg.periodic_timer, check_any_channel());
    // Triger Regular BSR if UE has available data for transmission on any channel
    if (check_any_channel()) {
      set_trigger(REGULAR);
      Debug("BSR:   Triggering BSR reTX\n");
    }
  }
}

uint32_t bsr_proc::get_buffer_state()
{
  uint32_t buffer = 0;
  for (int i = 0; i < NOF_LCG; i++) {
    buffer += get_buffer_state_lcg(i);
  }
  return buffer;
}

// Checks if data is available for a channel with higher priority than others
bool bsr_proc::check_highest_channel()
{
  for (int i = 0; i < NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      // If new data available
      if (iter->second.new_buffer > iter->second.old_buffer) {
        // Check if this LCID has higher priority than any other LCID ("belong to any LCG") for which data is already
        // available for transmission
        bool is_max_priority = true;
        for (int j = 0; j < NOF_LCG; j++) {
          for (std::map<uint32_t, lcid_t>::iterator iter2 = lcgs[j].begin(); iter2 != lcgs[j].end(); ++iter2) {
            // No max prio LCG if prio isn't higher or LCID already had buffered data
            if (iter2->second.priority <= iter->second.priority && (iter2->second.old_buffer > 0)) {
              is_max_priority = false;
            }
          }
        }
        if (is_max_priority) {
          Debug("BSR:   New data for lcid=%d with maximum priority in lcg=%d\n", iter->first, i);
          return true;
        }
      }
    }
  }
  return false;
}

bool bsr_proc::check_any_channel()
{
  for (int i = 0; i < NOF_LCG; i++) {
    if (get_buffer_state_lcg(i)) {
      return true;
    }
  }
  return false;
}

// Checks if only one logical channel has data avaiable for Tx
bool bsr_proc::check_new_data()
{
  for (int i = 0; i < NOF_LCG; i++) {
    // If there was no data available in any LCID belonging to this LCG
    if (get_buffer_state_lcg(i) == 0) {
      for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
        if (iter->second.new_buffer > 0) {
          Debug("BSR:   New data available for lcid=%d\n", iter->first);
          return true;
        }
      }
    }
  }
  return false;
}

void bsr_proc::update_new_data()
{
  for (int i = 0; i < NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      iter->second.new_buffer = rlc->get_buffer_state(iter->first);
    }
  }
}

void bsr_proc::update_old_buffer()
{
  for (int i = 0; i < NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      iter->second.old_buffer = iter->second.new_buffer;
    }
  }
}

uint32_t bsr_proc::get_buffer_state_lcg(uint32_t lcg)
{
  uint32_t n = 0;
  for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[lcg].begin(); iter != lcgs[lcg].end(); ++iter) {
    n += iter->second.old_buffer;
  }
  return n;
}

// Checks if a BSR needs to be generated and, if so, configures the BSR format
// It does not update the BSR values of the LCGs
bool bsr_proc::generate_bsr(bsr_t* bsr, uint32_t pdu_space)
{
  bool     send_bsr = false;
  uint32_t nof_lcg  = 0;

  // Check if more than one LCG has data to sned
  for (int i = 0; i < NOF_LCG; i++) {
    if (bsr->buff_size[i] > 0) {
      nof_lcg++;
    }
  }

  if (pdu_space >= CE_SUBHEADER_LEN + ce_size(srslte::ul_sch_lcid::LONG_BSR)) {
    // we could fit a long BSR
    if (triggered_bsr_type != PADDING && nof_lcg <= 1) {
      // for Regular and periodic BSR we still send a short BSR if only one LCG has data to send
      bsr->format = SHORT_BSR;
    } else {
      bsr->format = LONG_BSR;
    }
    send_bsr = true;
  } else if (pdu_space >= CE_SUBHEADER_LEN + ce_size(srslte::ul_sch_lcid::SHORT_BSR)) {
    // we can only fit a short or truncated BSR
    if (nof_lcg > 1) {
      // send truncated BSR
      bsr->format           = TRUNC_BSR;
      uint32_t max_prio_lcg = find_max_priority_lcg_with_data();
      for (uint32_t i = 0; i < NOF_LCG; i++) {
        if (max_prio_lcg != i) {
          bsr->buff_size[i] = 0;
        }
      }
    } else {
      bsr->format = SHORT_BSR;
    }
    send_bsr = true;
  }

  if (send_bsr) {
    // Restart or Start Periodic timer every time a BSR is generated and transmitted in an UL grant
    if (timer_periodic.duration() && bsr->format != TRUNC_BSR) {
      timer_periodic.run();
      Debug("BSR:   Started periodicBSR-Timer\n");
    }
    // reset trigger to avoid another BSR in the next UL grant
    triggered_bsr_type = NONE;
  }

  return send_bsr;
}

/* After packing all UL PDUs for this TTI, the internal buffer state of the BSR procedure needs to be updated with what
 * has actually been transmitted in each LCG. We don't ask RLC again as new SDUs could have queued up again. Currently
 * we just get the updates per LCG. Since we are only interested when zero outstanding data has been reported, we
 * currently just reset the buffer for each LCID of the LCG.
 */
void bsr_proc::update_bsr_tti_end(const bsr_t* bsr)
{
  std::lock_guard<std::mutex> lock(mutex);
  for (uint32_t i = 0; i < NOF_LCG; i++) {
    if (bsr->buff_size[i] == 0) {
      for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
        // Reset buffer state for all LCIDs of that the LCG for which we reported no further data to transmit
        iter->second.old_buffer = 0;
      }
    }
  }
}

// Checks if Regular BSR must be assembled, as defined in 5.4.5
// Padding BSR is assembled when called by mux_unit when UL dci is received
// Periodic BSR is triggered by the expiration of the timers
void bsr_proc::step(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (!initiated) {
    return;
  }

  update_new_data();

  // Regular BSR triggered if new data arrives or channel with high priority has new data
  if (check_new_data() || check_highest_channel()) {
    Debug("BSR:   Triggering Regular BSR tti=%d\n", tti);
    set_trigger(REGULAR);
  }

  update_old_buffer();
}

char* bsr_proc::bsr_type_tostring(triggered_bsr_type_t type)
{
  switch (type) {
    case bsr_proc::NONE:
      return (char*)"none";
    case bsr_proc::REGULAR:
      return (char*)"Regular";
    case bsr_proc::PADDING:
      return (char*)"Padding";
    case bsr_proc::PERIODIC:
      return (char*)"Periodic";
  }
  return (char*)"unknown";
}

char* bsr_proc::bsr_format_tostring(bsr_format_t format)
{
  switch (format) {
    case bsr_proc::LONG_BSR:
      return (char*)"Long";
    case bsr_proc::SHORT_BSR:
      return (char*)"Short";
    case bsr_proc::TRUNC_BSR:
      return (char*)"Truncated";
    default:
      return (char*)"Short";
  }
}

bool bsr_proc::need_to_send_bsr_on_ul_grant(uint32_t grant_size, uint32_t total_data, bsr_t* bsr)
{
  std::lock_guard<std::mutex> lock(mutex);

  bool send_bsr = false;
  if (triggered_bsr_type == PERIODIC || triggered_bsr_type == REGULAR) {
    // All triggered BSRs shall be cancelled in case the UL grant can accommodate all pending data
    if (grant_size >= total_data) {
      triggered_bsr_type = NONE;
    } else {
      send_bsr = generate_bsr(bsr, grant_size);
    }
  }

  // Cancel SR if an Uplink grant is received
  Debug("BSR:   Cancelling SR procedure due to uplink grant\n");
  sr->reset();

  // Restart or Start ReTX timer upon indication of a grant
  if (timer_retx.duration()) {
    timer_retx.run();
    Debug("BSR:   Started retxBSR-Timer\n");
  }
  return send_bsr;
}

// This function is called by MUX only if Regular BSR has not been triggered before
bool bsr_proc::generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (nof_padding_bytes >= CE_SUBHEADER_LEN + ce_size(srslte::ul_sch_lcid::SHORT_BSR)) {
    // generate padding BSR
    set_trigger(PADDING);
    generate_bsr(bsr, nof_padding_bytes);
    set_trigger(NONE);
    return true;
  }

  return false;
}

void bsr_proc::setup_lcid(uint32_t lcid, uint32_t new_lcg, uint32_t priority)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (new_lcg < NOF_LCG) {
    // First see if it already exists and eliminate it
    for (int i = 0; i < NOF_LCG; i++) {
      if (lcgs[i].count(lcid)) {
        lcgs[i].erase(lcid);
      }
    }
    // Now add it
    lcgs[new_lcg][lcid].priority   = priority;
    lcgs[new_lcg][lcid].old_buffer = 0;
  } else {
    Error("BSR:   Invalid lcg=%d for lcid=%d\n", new_lcg, lcid);
  }
}

uint32_t bsr_proc::find_max_priority_lcg_with_data()
{
  int32_t  max_prio = 99;
  uint32_t max_idx  = 0;
  for (int i = 0; i < NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      if (iter->second.priority < max_prio && iter->second.old_buffer > 0) {
        max_prio = iter->second.priority;
        max_idx  = i;
      }
    }
  }
  return max_idx;
}

} // namespace srsue
