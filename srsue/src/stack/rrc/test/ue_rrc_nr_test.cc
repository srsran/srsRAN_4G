/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
