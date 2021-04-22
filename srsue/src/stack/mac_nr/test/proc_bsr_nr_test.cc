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
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/test_common.h"
#include "srsue/hdr/stack/mac_nr/proc_bsr_nr.h"

using namespace srsue;

int sbsr_tests()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::debug);
  mac_logger.set_hex_dump_max_size(-1);

  srsran::task_scheduler        task_sched{5, 2};
  srsran::ext_task_sched_handle ext_task_sched_h(&task_sched);

  proc_bsr_nr proc(mac_logger);
  proc.init(nullptr, nullptr, nullptr, &ext_task_sched_h);

  srsran::bsr_cfg_nr_t bsr_cfg = {};
  bsr_cfg.periodic_timer       = 20;
  bsr_cfg.retx_timer           = 320;
  TESTASSERT(proc.set_config(bsr_cfg) == SRSRAN_SUCCESS);

  uint32_t            tti = 0;
  mac_buffer_states_t buffer_state;
  buffer_state.nof_lcgs_with_data = 1;
  buffer_state.last_non_zero_lcg  = 1;
  buffer_state.lcg_buffer_size[1] = 10;
  proc.step(tti++, buffer_state);

  // Buffer size == 10 should result in index 1 (<= 10)
  srsran::mac_sch_subpdu_nr::lcg_bsr_t sbsr = proc.generate_sbsr();
  TESTASSERT(sbsr.lcg_id == 1);
  TESTASSERT(sbsr.buffer_size == 1);

  buffer_state.last_non_zero_lcg  = 1;
  buffer_state.lcg_buffer_size[1] = 11;
  proc.step(tti++, buffer_state);

  // Buffer size == 11 should result in index 1
  sbsr = proc.generate_sbsr();
  TESTASSERT(sbsr.lcg_id == 1);
  TESTASSERT(sbsr.buffer_size == 2);

  buffer_state.last_non_zero_lcg  = 1;
  buffer_state.lcg_buffer_size[1] = 77285; // 77284 + 1
  proc.step(tti++, buffer_state);

  // Buffer size 77285 should result in index 29 (first value of that index)
  sbsr = proc.generate_sbsr();
  TESTASSERT(sbsr.lcg_id == 1);
  TESTASSERT(sbsr.buffer_size == 29);

  buffer_state.last_non_zero_lcg  = 1;
  buffer_state.lcg_buffer_size[1] = 150000;
  proc.step(tti++, buffer_state);

  // Buffer size 150000 should result in index 30
  sbsr = proc.generate_sbsr();
  TESTASSERT(sbsr.lcg_id == 1);
  TESTASSERT(sbsr.buffer_size == 30);

  buffer_state.last_non_zero_lcg  = 1;
  buffer_state.lcg_buffer_size[1] = 150001;
  proc.step(tti++, buffer_state);

  // Buffer size 150001 should result in index 31
  sbsr = proc.generate_sbsr();
  TESTASSERT(sbsr.lcg_id == 1);
  TESTASSERT(sbsr.buffer_size == 31);

  return SRSRAN_SUCCESS;
}

int main()
{
  srslog::init();

  TESTASSERT(sbsr_tests() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
