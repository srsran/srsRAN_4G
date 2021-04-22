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

#include "srsran/common/test_common.h"
#include "srsue/hdr/stack/rrc/rrc_nr.h"

using namespace srsue;

int rrc_nr_cap_request_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  rrc_nr                    rrc_nr(task_sched_handle);
  srsran::byte_buffer_t     caps;
  rrc_nr.get_eutra_nr_capabilities(&caps);
  rrc_nr.get_nr_capabilities(&caps);
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_nr_cap_request_test() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
