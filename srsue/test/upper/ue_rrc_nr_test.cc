/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/common/test_common.h"
#include "srsue/hdr/stack/rrc/rrc_nr.h"

using namespace srsue;

int rrc_nr_cap_request_test()
{
  srslte::log_ref rrc_log("RRC");
  rrc_log->set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log->set_hex_limit(-1);
  srslte::task_scheduler    task_sched{512, 100};
  srslte::task_sched_handle task_sched_handle(&task_sched);
  rrc_nr                    rrc_nr(task_sched_handle);
  srslte::byte_buffer_t     caps;
  rrc_nr.get_eutra_nr_capabilities(&caps);
  rrc_nr.get_nr_capabilities(&caps);
  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_nr_cap_request_test() == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}
