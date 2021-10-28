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

#include "srsue/hdr/stack/mac_nr/proc_bsr_nr.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsue {

using namespace srsran;

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
    logger.debug("BSR:   %s", buffer_state.to_string());
    timer_queue_status_print.run();
  };
  timer_queue_status_print.set(QUEUE_STATUS_PERIOD_MS, queue_status_print_task);
  timer_queue_status_print.run();

  initiated = true;

  return SRSRAN_SUCCESS;
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
  std::lock_guard<std::mutex> lock(mutex);

  timer_periodic.stop();
  timer_retx.stop();

  lcg_priorities.clear();

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

// Checks if data is available for a channel with higher priority than others
bool proc_bsr_nr::check_highest_channel()
{
  // TODO: move 4G implementation to base class or rewrite
  return false;
}

bool proc_bsr_nr::check_any_channel()
{
  for (const auto& lcg : buffer_state.lcg_buffer_size) {
    if (lcg.second > 0) {
      return true;
    }
  }
  return false;
}

// Checks if only one logical channel has data avaiable for Tx
bool proc_bsr_nr::check_new_data(const mac_buffer_states_t& new_buffer_state)
{
  for (const auto& lcg : buffer_state.lcg_buffer_size) {
    if (lcg.second == 0 and new_buffer_state.lcg_buffer_size.at(lcg.first) > 0) {
      logger.debug("BSR:   New data available for LCG=%d", lcg.first);
      return true;
    }
  }
  return false;
}

/**
 * @brief Called by Mux through PHY worker whenever a new SBSR CE
 * needs to be included in a MAC PDU.
 *
 * @return SBSR CE for subPDU containing the buffer state of the highest priority LCG
 */
srsran::mac_sch_subpdu_nr::lcg_bsr_t proc_bsr_nr::generate_sbsr()
{
  std::lock_guard<std::mutex>          lock(mutex);
  srsran::mac_sch_subpdu_nr::lcg_bsr_t sbsr = {};
  if (buffer_state.nof_lcgs_with_data > 0) {
    sbsr.lcg_id      = buffer_state.last_non_zero_lcg;
    sbsr.buffer_size = buff_size_bytes_to_field(buffer_state.lcg_buffer_size.at(sbsr.lcg_id), SHORT_BSR);
  }
  triggered_bsr_type = NONE;
  return sbsr;
}

/**
 * @brief Called by MAC every TTI with the current state of each LCID/LCGs
 *
 * Checks if Regular BSR must be assembled, as defined in 5.4.5.
 * Padding BSR is assembled when explicitly called by MUX when UL DCI is received
 * Periodic BSR is triggered by the expiration of the timers
 *
 * @param tti              The current TTI
 * @param new_buffer_state Buffer state of all LCID/LCGs at the start of the TTI
 *
 */
void proc_bsr_nr::step(uint32_t tti, const mac_buffer_states_t& new_buffer_state_)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (not initiated) {
    return;
  }

  // Regular BSR triggered if new data arrives or channel with high priority has new data
  if (check_new_data(new_buffer_state_) || check_highest_channel()) {
    logger.debug("BSR:   Triggering Regular BSR tti=%d", tti);
    set_trigger(REGULAR);
  }

  // store buffer state for comparision in next TTI
  buffer_state = new_buffer_state_;
}

void proc_bsr_nr::new_grant_ul(uint32_t grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (triggered_bsr_type != NONE) {
    // Decide BSR type to be transmitted, state for all LCG/LCIDs has already been updated by step()
    if (buffer_state.nof_lcgs_with_data > 1) {
      // report Long BSR if more than one LCG has data to send
      mux->generate_bsr_mac_ce(LONG_BSR);
    } else {
      // report Short BSR otherwise
      mux->generate_bsr_mac_ce(SHORT_BSR);
    }

    // 3> start or restart periodicBSR-Timer, except when all the generated BSRs are long or short Truncated BSRs
    // TODO: add check if only truncated version can be included
    timer_periodic.run();

    // 3> start or restart retxBSR-Timer.
    timer_retx.run();
  }

  // Cancel SR if an UL grant is received
  logger.debug("BSR:   Cancelling SR procedure due to UL grant");
  sr->reset();
}

// This function is called by MUX only if no BSR has been triggered before
void proc_bsr_nr::set_padding_bytes(uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  // TODO: get correct values from mac_sch_pdu_nr
  const uint32_t SBSR_CE_SUBHEADER_LEN = 1;
  const uint32_t LBSR_CE_SUBHEADER_LEN = 1;
  // if the number of padding bits is equal to or larger than the size of the Short BSR plus its subheader but smaller
  // than the size of the Long BSR plus its subheader
  if (nof_bytes >= (SBSR_CE_SUBHEADER_LEN +
                    srsran::mac_sch_subpdu_nr::sizeof_ce(srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::SHORT_BSR, true)) &&
      nof_bytes < (LBSR_CE_SUBHEADER_LEN +
                   srsran::mac_sch_subpdu_nr::sizeof_ce(srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::LONG_BSR, true))) {
    // generate short padding BSR
    mux->generate_bsr_mac_ce(SHORT_BSR);
  } else if (nof_bytes >= (LBSR_CE_SUBHEADER_LEN + srsran::mac_sch_subpdu_nr::sizeof_ce(
                                                       srsran::mac_sch_subpdu_nr::nr_lcid_sch_t::LONG_BSR, true))) {
    // report Long BSR if more than one LCG has data to send
    mux->generate_bsr_mac_ce(LONG_BSR);
  }
}

int proc_bsr_nr::setup_lcid(uint32_t lcid, uint32_t new_lcg, uint32_t priority)
{
  // TODO: move 4G implementation to base class
  if (new_lcg > srsran::mac_sch_subpdu_nr::max_num_lcg_lbsr) {
    logger.error("BSR:   Invalid lcg=%d for lcid=%d", new_lcg, lcid);
    return SRSRAN_ERROR;
  }

  std::lock_guard<std::mutex> lock(mutex);

  // Check that the new priority doesn't not already exist
  if (lcg_priorities.find(priority) != lcg_priorities.end()) {
    logger.error(
        "BSR:   Invalid config. Priority=%d already configured for lcg=%d", priority, lcg_priorities.at(priority));
    return SRSRAN_ERROR;
  }

  lcg_priorities[priority] = new_lcg;

  return SRSRAN_SUCCESS;
}

uint32_t proc_bsr_nr::find_max_priority_lcg_with_data()
{
  // iterate over LCGs in order of their priorities and check if there is one with data to transmit
  for (const auto& lcg_prio : lcg_priorities) {
    if (buffer_state.lcg_buffer_size.at(lcg_prio.second) > 0) {
      return lcg_prio.second;
    }
  }
  return 0;
}

/** Converts the buffer size levels (in Bytes) to the 5 or 8-bit Buffer Size field
 * @param buffer_size The actual buffer size level in Bytes
 * @param format      The BSR format that determines the buffer size field length
 * @return uint8_t    The buffer size field that will be used for the MAC PDU
 */
uint8_t proc_bsr_nr::buff_size_bytes_to_field(uint32_t buffer_size, bsr_format_nr_t format)
{
  // early exit
  if (buffer_size == 0) {
    return 0;
  }

  switch (format) {
    case SHORT_BSR:
    case SHORT_TRUNC_BSR:
      if (buffer_size > buffer_size_levels_5bit[buffer_size_levels_5bit_max_idx]) {
        return buffer_size_levels_5bit_max_idx;
      } else {
        for (uint32_t i = 1; i < buffer_size_levels_5bit_max_idx; i++) {
          if (buffer_size <= buffer_size_levels_5bit[i]) {
            return i;
          }
        }
        return buffer_size_levels_5bit_max_idx - 1;
      }
      break;
    case LONG_BSR:
    case LONG_TRUNC_BSR:
      if (buffer_size > buffer_size_levels_8bit[buffer_size_levels_8bit_max_idx]) {
        return buffer_size_levels_8bit_max_idx;
      } else {
        for (uint32_t i = 1; i < buffer_size_levels_8bit_max_idx; i++) {
          if (buffer_size <= buffer_size_levels_8bit[i + 1]) {
            return i + 1;
          }
        }
        return buffer_size_levels_8bit_max_idx - 1;
      }
      break;
  }

  return 0;
}

} // namespace srsue
