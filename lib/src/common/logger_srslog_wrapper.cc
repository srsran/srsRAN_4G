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

#include "srslte/common/logger_srslog_wrapper.h"
#include "srslte/srslog/log_channel.h"

using namespace srslte;

void srslog_wrapper::log(unique_log_str_t msg)
{
  chan("%s", msg->str());
}
