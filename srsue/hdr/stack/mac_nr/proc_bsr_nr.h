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

#ifndef SRSUE_PROC_BSR_NR_H
#define SRSUE_PROC_BSR_NR_H

#include <map>
#include <stdint.h>

#include "proc_sr_nr.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/mac/bsr_nr.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_common/mac_common.h"

/* Buffer status report procedure */

namespace srsue {

class rlc_interface_mac;

// BSR interface for MUX
class bsr_interface_mux_nr
{
public:
  /// MUX calls BSR to receive the buffer state of a single LCG.
  virtual srsran::mac_sch_subpdu_nr::lcg_bsr_t generate_sbsr() = 0;
};

class mux_interface_bsr_nr
{
public:
  /// Inform MUX unit to that a BSR needs to be generated in the next UL transmission.
  virtual void generate_bsr_mac_ce(const srsran::bsr_format_nr_t& format) = 0;
};

/**
 * @brief  BSR procedure for NR according to 3GPP TS 38.321 version 15.3.0
 *
 * @remark: So far only class scelleton.
 */
class proc_bsr_nr : public srsran::timer_callback, public bsr_interface_mux_nr
{
public:
  explicit proc_bsr_nr(srslog::basic_logger& logger_) : logger(logger_) {}
  int  init(proc_sr_nr*                    sr_proc,
            mux_interface_bsr_nr*          mux_,
            rlc_interface_mac*             rlc,
            srsran::ext_task_sched_handle* task_sched_);
  void step(uint32_t tti, const mac_buffer_states_t& new_buffer_state);
  void reset();
  int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg);

  int      setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority);
  void     timer_expired(uint32_t timer_id);

  /// Called by MAC when an UL grant is received
  void new_grant_ul(uint32_t grant_size);

  /// MUX interface for BSR generation
  srsran::mac_sch_subpdu_nr::lcg_bsr_t generate_sbsr();
  void                                 set_padding_bytes(uint32_t nof_bytes);

private:
  const static int QUEUE_STATUS_PERIOD_MS = 1000;

  std::mutex mutex;

  srsran::ext_task_sched_handle* task_sched = nullptr;
  srslog::basic_logger&          logger;
  rlc_interface_mac*             rlc = nullptr;
  mux_interface_bsr_nr*          mux = nullptr;
  proc_sr_nr*                    sr  = nullptr;

  srsran::bsr_cfg_nr_t bsr_cfg = {};

  bool initiated = false;

  mac_buffer_states_t buffer_state;

  // map of LCGs and their priorities, key is the priority (sorted) and the value the LCG
  std::map<uint32_t, uint32_t> lcg_priorities;

  bsr_trigger_type_t triggered_bsr_type = NONE;

  void     print_state();
  void     set_trigger(bsr_trigger_type_t new_trigger);
  bool     check_highest_channel();
  bool     check_new_data(const mac_buffer_states_t& new_buffer_state);
  bool     check_any_channel();

  uint8_t buff_size_bytes_to_field(uint32_t buffer_size, srsran::bsr_format_nr_t format);

  uint32_t find_max_priority_lcg_with_data();

  srsran::timer_handler::unique_timer timer_periodic;
  srsran::timer_handler::unique_timer timer_retx;
  srsran::timer_handler::unique_timer timer_queue_status_print;
};

} // namespace srsue

#endif // SRSUE_PROC_BSR_NR_H
