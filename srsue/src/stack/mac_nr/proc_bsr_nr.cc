/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/mac_nr/proc_bsr_nr.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsue {

int32_t proc_bsr_nr::init(proc_sr_nr*                    sr_,
                          mux_interface_bsr_nr*          mux_,
                          rlc_interface_mac*             rlc_,
                          srsran::ext_task_sched_handle* task_sched_)
{
  rlc        = rlc_;
  mux        = mux_;
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

  return SRSRAN_SUCCESS;
}

void proc_bsr_nr::print_state()
{
  char str[128];
  str[0] = '\0';
  int n  = 0;
  for (auto& lcg : lcgs) {
    for (auto& iter : lcg) {
      n = srsran_print_check(str, 128, n, "%d: %d ", iter.first, iter.second.old_buffer);
    }
  }
  logger.info(
      "BSR:   triggered_bsr_type=%s, LCID QUEUE status: %s", bsr_trigger_type_tostring(triggered_bsr_type), str);
}

void proc_bsr_nr::set_trigger(bsr_trigger_type_t new_trigger)
{
  triggered_bsr_type = new_trigger;

  // Trigger SR always when Regular BSR is triggered in the current TTI. Will be cancelled if a grant is received
  if (triggered_bsr_type == REGULAR) {
    logger.debug("BSR:   Triggering SR procedure");
    sr->start();
  }
}

void proc_bsr_nr::reset()
{
  timer_periodic.stop();
  timer_retx.stop();

  triggered_bsr_type = NONE;
}

int proc_bsr_nr::set_config(const srsran::bsr_cfg_nr_t& bsr_cfg_)
{
  std::lock_guard<std::mutex> lock(mutex);

  bsr_cfg = bsr_cfg_;

  if (bsr_cfg_.periodic_timer > 0) {
    timer_periodic.set(bsr_cfg_.periodic_timer, [this](uint32_t tid) { timer_expired(tid); });
    logger.info("BSR:   Configured timer periodic %d ms", bsr_cfg_.periodic_timer);
  }
  if (bsr_cfg_.retx_timer > 0) {
    timer_retx.set(bsr_cfg_.retx_timer, [this](uint32_t tid) { timer_expired(tid); });
    logger.info("BSR:   Configured timer reTX %d ms", bsr_cfg_.retx_timer);
  }

  return SRSRAN_SUCCESS;
}

/* Process Periodic BSR */
void proc_bsr_nr::timer_expired(uint32_t timer_id)
{
  std::lock_guard<std::mutex> lock(mutex);

  // periodicBSR-Timer
  if (timer_id == timer_periodic.id()) {
    if (triggered_bsr_type == NONE) {
      set_trigger(PERIODIC);
      logger.debug("BSR:   Triggering Periodic BSR");
    }
    // retxBSR-Timer
  } else if (timer_id == timer_retx.id()) {
    // Enable reTx of SR only if periodic timer is not infinity
    logger.debug("BSR:   Timer BSR reTX expired, periodic=%d, channel=%d", bsr_cfg.periodic_timer, check_any_channel());
    // Triger Regular BSR if UE has available data for transmission on any channel
    if (check_any_channel()) {
      set_trigger(REGULAR);
      logger.debug("BSR:   Triggering BSR reTX");
    }
  }
}

uint32_t proc_bsr_nr::get_buffer_state()
{
  uint32_t buffer = 0;
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    buffer += get_buffer_state_lcg(i);
  }
  return buffer;
}

// Checks if data is available for a channel with higher priority than others
bool proc_bsr_nr::check_highest_channel()
{
  // TODO: move 4G implementation to base class or rewrite
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      // If new data available
      if (iter->second.new_buffer > iter->second.old_buffer) {
        // Check if this LCID has higher priority than any other LCID ("belong to any LCG") for which data is already
        // available for transmission
        bool is_max_priority = true;
        for (int j = 0; j < MAX_NOF_LCG; j++) {
          for (std::map<uint32_t, lcid_t>::iterator iter2 = lcgs[j].begin(); iter2 != lcgs[j].end(); ++iter2) {
            // No max prio LCG if prio isn't higher or LCID already had buffered data
            if (iter2->second.priority <= iter->second.priority && (iter2->second.old_buffer > 0)) {
              is_max_priority = false;
            }
          }
        }
        if (is_max_priority) {
          logger.debug("BSR:   New data for lcid=%d with maximum priority in lcg=%d", iter->first, i);
          return true;
        }
      }
    }
  }
  return false;
}

bool proc_bsr_nr::check_any_channel()
{
  // TODO: move 4G implementation to base class or rewrite
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    if (get_buffer_state_lcg(i)) {
      return true;
    }
  }
  return false;
}

// Checks if only one logical channel has data avaiable for Tx
bool proc_bsr_nr::check_new_data()
{
  // TODO: move 4G implementation to base class or rewrite
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    // If there was no data available in any LCID belonging to this LCG
    if (get_buffer_state_lcg(i) == 0) {
      for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
        if (iter->second.new_buffer > 0) {
          logger.debug("BSR:   New data available for lcid=%d", iter->first);
          return true;
        }
      }
    }
  }
  return false;
}

void proc_bsr_nr::update_new_data()
{
  // TODO: move 4G implementation to base class or rewrite
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      iter->second.new_buffer = rlc->get_buffer_state(iter->first);
    }
  }
}

void proc_bsr_nr::update_old_buffer()
{
  // TODO: move 4G implementation to base class or rewrite
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      iter->second.old_buffer = iter->second.new_buffer;
    }
  }
}

uint32_t proc_bsr_nr::get_buffer_state_lcg(uint32_t lcg)
{
  // TODO: move 4G implementation to base class or rewrite
  uint32_t n = 0;
  for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[lcg].begin(); iter != lcgs[lcg].end(); ++iter) {
    n += iter->second.old_buffer;
  }
  return n;
}

// Generate BSR
bool proc_bsr_nr::generate_bsr(bsr_t* bsr, uint32_t pdu_space)
{
  // TODO: add BSR generation
  bool     send_bsr = false;
  return send_bsr;
}

// Called by MAC every TTI
// Checks if Regular BSR must be assembled, as defined in 5.4.5
// Padding BSR is assembled when called by mux_unit when UL dci is received
// Periodic BSR is triggered by the expiration of the timers
void proc_bsr_nr::step(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (not initiated) {
    return;
  }

  update_new_data();

  // Regular BSR triggered if new data arrives or channel with high priority has new data
  if (check_new_data() || check_highest_channel()) {
    logger.debug("BSR:   Triggering Regular BSR tti=%d", tti);
    set_trigger(REGULAR);
  }

  update_old_buffer();
}

void proc_bsr_nr::new_grant_ul(uint32_t grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (triggered_bsr_type != NONE) {
    // inform MUX we need to generate a BSR
    mux->generate_bsr_mac_ce();
  }

  // TODO: restart retxBSR-Timer
}

// This function is called by MUX only if Regular BSR has not been triggered before
bool proc_bsr_nr::generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr)
{
  std::lock_guard<std::mutex> lock(mutex);

  // TODO: get correct values from mac_sch_pdu_nr
  const uint32_t SBSR_CE_SUBHEADER_LEN = 1;
  const uint32_t LBSR_CE_SUBHEADER_LEN = 1;
  // if the number of padding bits is equal to or larger than the size of the Short BSR plus its subheader but smaller
  // than the size of the Long BSR plus its subheader
  if (nof_padding_bytes >= SBSR_CE_SUBHEADER_LEN + srsran::mac_sch_subpdu_nr::sizeof_ce(SHORT_BSR, true) &&
      nof_padding_bytes <= LBSR_CE_SUBHEADER_LEN + srsran::mac_sch_subpdu_nr::sizeof_ce(LONG_BSR, true)) {
    // generate padding BSR
    set_trigger(PADDING);
    generate_bsr(bsr, nof_padding_bytes);
    set_trigger(NONE);
    return true;
  }

  return false;
}

int proc_bsr_nr::setup_lcid(uint32_t lcid, uint32_t new_lcg, uint32_t priority)
{
  // TODO: move 4G implementation to base class
  if (new_lcg > MAX_NOF_LCG) {
    logger.error("BSR:   Invalid lcg=%d for lcid=%d", new_lcg, lcid);
    return SRSRAN_ERROR;
  }

  std::lock_guard<std::mutex> lock(mutex);

  // First see if it already exists and eliminate it
  for (int i = 0; i < MAX_NOF_LCG; i++) {
    if (lcgs[i].count(lcid)) {
      lcgs[i].erase(lcid);
    }
  }
  // Now add it
  lcgs[new_lcg][lcid].priority   = priority;
  lcgs[new_lcg][lcid].old_buffer = 0;

  return SRSRAN_SUCCESS;
}

uint32_t proc_bsr_nr::find_max_priority_lcg_with_data()
{
  // TODO: move 4G implementation to base class or rewrite
  int32_t  max_prio = 99;
  uint32_t max_idx  = 0;
  for (int i = 0; i < MAX_NOF_LCG; i++) {
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
