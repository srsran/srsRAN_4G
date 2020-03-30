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

bsr_proc::bsr_proc()
{
  initiated          = false;
  current_tti        = 0;
  trigger_tti        = 0;
  triggered_bsr_type = NONE;

  pthread_mutex_init(&mutex, NULL);
}

void bsr_proc::init(rlc_interface_mac* rlc_, srslte::log_ref log_h_, srslte::task_handler_interface* task_handler_)
{
  log_h        = log_h_;
  rlc          = rlc_;
  task_handler = task_handler_;

  timer_periodic           = task_handler->get_unique_timer();
  timer_retx               = task_handler->get_unique_timer();
  timer_queue_status_print = task_handler->get_unique_timer();

  reset();

  // Print periodically the LCID queue status
  auto queue_status_print_task = [this](uint32_t tid) {
    char str[128];
    str[0] = '\0';
    int n  = 0;
    for (auto& lcg : lcgs) {
      for (auto& iter : lcg) {
        n = srslte_print_check(str, 128, n, "%d: %d ", iter.first, iter.second.old_buffer);
      }
    }
    Info("BSR:   triggered_bsr_type=%d, LCID QUEUE status: %s\n", triggered_bsr_type, str);
    timer_queue_status_print.run();
  };
  timer_queue_status_print.set(QUEUE_STATUS_PERIOD_MS, queue_status_print_task);
  timer_queue_status_print.run();

  initiated = true;
}

void bsr_proc::set_trigger(srsue::bsr_proc::triggered_bsr_type_t new_trigger)
{
  triggered_bsr_type = new_trigger;
  trigger_tti        = current_tti;
}

void bsr_proc::reset()
{
  timer_periodic.stop();
  timer_retx.stop();

  reset_sr           = false;
  sr_is_sent         = false;
  triggered_bsr_type = NONE;

  trigger_tti = 0;
}

void bsr_proc::set_config(srslte::bsr_cfg_t& bsr_cfg_)
{
  pthread_mutex_lock(&mutex);

  bsr_cfg = bsr_cfg_;

  if (bsr_cfg_.periodic_timer > 0) {
    timer_periodic.set(bsr_cfg_.periodic_timer, [this](uint32_t tid) { timer_expired(tid); });
    Info("BSR:   Configured timer periodic %d ms\n", bsr_cfg_.periodic_timer);
  }
  if (bsr_cfg_.retx_timer > 0) {
    timer_retx.set(bsr_cfg_.retx_timer, [this](uint32_t tid) { timer_expired(tid); });
    Info("BSR:   Configured timer reTX %d ms\n", bsr_cfg_.retx_timer);
  }
  pthread_mutex_unlock(&mutex);
}

/* Process Periodic BSR */
void bsr_proc::timer_expired(uint32_t timer_id)
{
  pthread_mutex_lock(&mutex);
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
      sr_is_sent = false;
    }
  }
  pthread_mutex_unlock(&mutex);
}

uint32_t bsr_proc::get_buffer_state()
{
  uint32_t buffer = 0;
  for (int i = 0; i < NOF_LCG; i++) {
    buffer += get_buffer_state_lcg(i);
  }
  return buffer;
}

// Checks if data is available for a a channel with higher priority than others
bool bsr_proc::check_highest_channel()
{

  for (int i = 0; i < NOF_LCG; i++) {
    for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
      // If new data available
      if (iter->second.new_buffer > iter->second.old_buffer) {
        // Check if this lcid has higher priority than any other LCID in the group
        bool is_max_priority = true;
        for (int j = 0; j < NOF_LCG; j++) {
          for (std::map<uint32_t, lcid_t>::iterator iter2 = lcgs[j].begin(); iter2 != lcgs[j].end(); ++iter2) {
            if (iter2->second.priority > iter->second.priority && iter2->second.old_buffer) {
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
    if (!get_buffer_state_lcg(i)) {
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

void bsr_proc::update_buffer_state()
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

bool bsr_proc::generate_bsr(bsr_t* bsr, uint32_t nof_padding_bytes)
{
  bool     ret     = false;
  uint32_t nof_lcg = 0;
  bzero(bsr, sizeof(bsr_t));

  // Calculate buffer size for each LCG
  for (int i = 0; i < NOF_LCG; i++) {
    bsr->buff_size[i] = get_buffer_state_lcg(i);
    if (bsr->buff_size[i] > 0) {
      nof_lcg++;
      ret = true;
    }
  }
  if (triggered_bsr_type == PADDING) {
    if (nof_padding_bytes < 4) {
      // If space only for short
      if (nof_lcg > 1) {
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
    } else {
      // If space for long BSR
      bsr->format = LONG_BSR;
    }
  } else {
    bsr->format = SHORT_BSR;
    if (nof_lcg > 1) {
      bsr->format = LONG_BSR;
    }
  }
  Info("BSR:   Type %s, Format %s, Value=%d,%d,%d,%d\n",
       bsr_type_tostring(triggered_bsr_type),
       bsr_format_tostring(bsr->format),
       bsr->buff_size[0],
       bsr->buff_size[1],
       bsr->buff_size[2],
       bsr->buff_size[3]);

  // Restart or Start Periodic timer every time a BSR is generated and transmitted in an UL grant
  if (timer_periodic.duration() && bsr->format != TRUNC_BSR) {
    timer_periodic.run();
    Debug("BSR:   Started periodicBSR-Timer\n");
  }

  return ret;
}

// Checks if Regular BSR must be assembled, as defined in 5.4.5
// Padding BSR is assembled when called by mux_unit when UL dci is received
// Periodic BSR is triggered by the expiration of the timers
void bsr_proc::step(uint32_t tti)
{
  if (!initiated) {
    return;
  }

  pthread_mutex_lock(&mutex);

  current_tti = tti;

  update_new_data();

  // Regular BSR triggered if new data arrives or channel with high priority has new data
  if (check_new_data() || check_highest_channel()) {
    set_trigger(REGULAR);
  }

  update_buffer_state();

  pthread_mutex_unlock(&mutex);
}

char* bsr_proc::bsr_type_tostring(triggered_bsr_type_t type)
{
  switch (type) {
    case bsr_proc::REGULAR:
      return (char*)"Regular";
    case bsr_proc::PADDING:
      return (char*)"Padding";
    case bsr_proc::PERIODIC:
      return (char*)"Periodic";
    default:
      return (char*)"Regular";
  }
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

bool bsr_proc::need_to_send_bsr_on_ul_grant(uint32_t grant_size, bsr_t* bsr)
{
  bool bsr_included = false;

  pthread_mutex_lock(&mutex);

  uint32_t bsr_sz = 0;
  if (triggered_bsr_type == PERIODIC || triggered_bsr_type == REGULAR) {

    generate_bsr(bsr, 0);

    // Only include BSR if it can fit into the remaining grant
    bsr_sz = bsr->format == LONG_BSR ? 3 : 1;
    if (bsr_sz > grant_size) {
      Debug("Grant is not enough to accommodate the BSR MAC CE\n");
    } else {
      Debug("BSR:   Including Regular BSR: grant_size=%d, bsr_sz=%d\n", grant_size, bsr_sz);
      bsr_included = true;
    }
  }

  // All triggered BSRs shall be cancelled in case the UL grant can accommodate all pending data available for
  // transmission but is not sufficient to additionally accommodate the BSR MAC control element plus its subheader. All
  // triggered BSRs shall be cancelled when a BSR is included in a MAC PDU for transmission
  int total_data = 0;
  if (!bsr_included) {
    for (int i = 0; i < NOF_LCG; i++) {
      for (std::map<uint32_t, lcid_t>::iterator iter = lcgs[i].begin(); iter != lcgs[i].end(); ++iter) {
        total_data += srslte::sch_pdu::size_header_sdu(iter->second.old_buffer) + iter->second.old_buffer;
      }
    }
    total_data--; // Because last SDU has no size header
  }
  // If all data fits in the grant or BSR has been included, cancel the trigger
  if (total_data < (int)grant_size || bsr_included) {
    set_trigger(NONE);
  }

  // Cancel SR if an Uplink grant is received
  reset_sr = true;

  // Restart or Start ReTX timer upon indication of a grant
  if (timer_retx.duration()) {
    timer_retx.run();
    Debug("BSR:   Started retxBSR-Timer\n");
  }
  pthread_mutex_unlock(&mutex);
  return bsr_included;
}

bool bsr_proc::generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr)
{
  bool ret = false;

  pthread_mutex_lock(&mutex);

  // This function is called by MUX only if Regular BSR has not been triggered before
  if (nof_padding_bytes >= 2) {
    // generate padding BSR
    set_trigger(PADDING);
    generate_bsr(bsr, nof_padding_bytes);
    set_trigger(NONE);
    ret = true;
  }
  pthread_mutex_unlock(&mutex);

  return ret;
}

bool bsr_proc::need_to_reset_sr()
{
  bool ret = false;
  pthread_mutex_lock(&mutex);
  if (reset_sr) {
    reset_sr   = false;
    sr_is_sent = false;
    Debug("BSR:   SR reset. sr_is_sent and reset_rs false\n");
    ret = true;
  }
  pthread_mutex_unlock(&mutex);
  return ret;
}

bool bsr_proc::need_to_send_sr(uint32_t tti)
{
  bool ret = false;
  pthread_mutex_lock(&mutex);
  if (!sr_is_sent && triggered_bsr_type == REGULAR) {
    reset_sr   = false;
    sr_is_sent = true;
    Debug("BSR:   Need to send sr: sr_is_sent=true, reset_sr=false, tti=%d, trigger_tti=%d\n", tti, trigger_tti);
    ret = true;
  }
  pthread_mutex_unlock(&mutex);
  return ret;
}

void bsr_proc::setup_lcid(uint32_t lcid, uint32_t new_lcg, uint32_t priority)
{
  if (new_lcg < NOF_LCG) {
    pthread_mutex_lock(&mutex);
    // First see if it already exists and eliminate it
    for (int i = 0; i < NOF_LCG; i++) {
      if (lcgs[i].count(lcid)) {
        lcgs[i].erase(lcid);
      }
    }
    // Now add it
    lcgs[new_lcg][lcid].priority   = priority;
    lcgs[new_lcg][lcid].old_buffer = 0;
    pthread_mutex_unlock(&mutex);
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
