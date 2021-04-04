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

#ifndef SRSUE_PROC_BSR_NR_H
#define SRSUE_PROC_BSR_NR_H

#include <map>
#include <stdint.h>

#include "proc_sr_nr.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_common/mac_common.h"

/* Buffer status report procedure */

namespace srsue {

class rlc_interface_mac;

// BSR interface for MUX
class bsr_interface_mux_nr
{
public:
  // TS 38.321 Sec 6.1.3.1
  typedef enum { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR } bsr_format_nr_t;

  // FIXME: this will be replaced
  typedef struct {
    bsr_format_nr_t format;
    uint32_t        buff_size[4];
  } bsr_t;

  /// MUX calls BSR to let it generate a padding BSR if there is space in PDU.
  virtual bool generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr) = 0;
};

class mux_interface_bsr_nr
{
public:
  /// Inform MUX unit to that a BSR needs to be generated in the next UL transmission.
  virtual void generate_bsr_mac_ce() = 0;
};

/**
 * @brief  BSR procedure for NR according to 3GPP TS 38.321 version 15.3.0
 *
 * @remark: So far only class scelleton.
 */
class proc_bsr_nr : public srsran::timer_callback, public bsr_interface_mux_nr
{
public:
  explicit proc_bsr_nr(srslog::basic_logger& logger) : logger(logger) {}
  int  init(proc_sr_nr*                    sr_proc,
            mux_interface_bsr_nr*          mux_,
            rlc_interface_mac*             rlc,
            srsran::ext_task_sched_handle* task_sched_);
  void step(uint32_t tti);
  void reset();
  int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg);

  int      setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority);
  void     timer_expired(uint32_t timer_id);
  uint32_t get_buffer_state();

  /// Called by MAC when an UL grant is received
  void new_grant_ul(uint32_t grant_size);

  // bool     need_to_send_bsr();
  bool generate_padding_bsr(uint32_t nof_padding_bytes, bsr_t* bsr);
  void update_bsr_tti_end(const bsr_t* bsr);

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

  const static int MAX_NOF_LCG = 8;

  typedef struct {
    int      priority;
    uint32_t old_buffer;
    uint32_t new_buffer;
  } lcid_t;

  std::map<uint32_t, lcid_t> lcgs[MAX_NOF_LCG]; // groups LCID in LCG

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

  uint32_t find_max_priority_lcg_with_data();

  srsran::timer_handler::unique_timer timer_periodic;
  srsran::timer_handler::unique_timer timer_retx;
  srsran::timer_handler::unique_timer timer_queue_status_print;
};

} // namespace srsue

#endif // SRSUE_PROC_BSR_NR_H
