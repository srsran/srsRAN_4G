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

#ifndef SRSUE_PROC_BSR_H
#define SRSUE_PROC_BSR_H

#include <map>
#include <stdint.h>

#include "proc_sr.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_common/mac_common.h"

/* Buffer status report procedure */

namespace srsue {

class rlc_interface_mac;

// BSR interface for MUX
class bsr_interface_mux
{
public:
  typedef enum { LONG_BSR, SHORT_BSR, TRUNC_BSR } bsr_format_t;

  typedef struct {
    bsr_format_t format;
    uint32_t     buff_size[4];
  } bsr_t;

  /* MUX calls BSR to check if it should send (and can fit) a BSR into PDU */
  virtual bool need_to_send_bsr_on_ul_grant(uint32_t grant_size, uint32_t total_data, bsr_t* bsr) = 0;

  /* MUX calls BSR to let it generate a padding BSR if there is space in PDU */
  virtual bool generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr) = 0;

  /* MUX calls BSR to update buffer state of each LCG after all PDUs for this TTI have been packed */
  virtual void update_bsr_tti_end(const bsr_t* bsr) = 0;
};

class bsr_proc : public srsran::timer_callback, public bsr_interface_mux
{
public:
  explicit bsr_proc(srslog::basic_logger& logger) : logger(logger) {}
  void init(sr_proc* sr_proc, rlc_interface_mac* rlc, srsran::ext_task_sched_handle* task_sched_);
  void step(uint32_t tti);
  void reset();
  void set_config(srsran::bsr_cfg_t& bsr_cfg);

  void     setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority);
  void     timer_expired(uint32_t timer_id);
  uint32_t get_buffer_state();
  bool     need_to_send_bsr_on_ul_grant(uint32_t grant_size, uint32_t total_data, bsr_t* bsr);
  bool     generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr);
  void     update_bsr_tti_end(const bsr_t* bsr);

private:
  const static int QUEUE_STATUS_PERIOD_MS = 1000;

  std::mutex mutex;

  srsran::ext_task_sched_handle* task_sched = nullptr;
  srslog::basic_logger&          logger;
  rlc_interface_mac*             rlc = nullptr;
  sr_proc*                       sr  = nullptr;

  srsran::bsr_cfg_t bsr_cfg;

  bool initiated = false;

  const static int NOF_LCG = 4;

  typedef struct {
    int      priority;
    uint32_t old_buffer;
    uint32_t new_buffer;
  } lcid_t;

  std::map<uint32_t, lcid_t> lcgs[NOF_LCG]; // groups LCID in LCG

  mac_buffer_states_t old_buffer_state;

  uint32_t find_max_priority_lcg_with_data();

  bsr_trigger_type_t triggered_bsr_type = NONE;

  void     print_state();
  void     set_trigger(bsr_trigger_type_t new_trigger);
  void     update_new_data();
  void     update_old_buffer();
  bool     check_highest_channel();
  bool     check_new_data();
  bool     check_any_channel();
  uint32_t get_buffer_state_lcg(uint32_t lcg);
  bool     generate_bsr(bsr_t* bsr, uint32_t nof_padding_bytes);
  char*    bsr_type_tostring(bsr_trigger_type_t type);
  char*    bsr_format_tostring(bsr_format_t format);

  srsran::timer_handler::unique_timer timer_periodic;
  srsran::timer_handler::unique_timer timer_retx;
  srsran::timer_handler::unique_timer timer_queue_status_print;
};

} // namespace srsue

#endif // SRSUE_PROC_BSR_H
