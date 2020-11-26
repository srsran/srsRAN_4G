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

#ifndef SRSLTE_LOGGER_SRSLOG_WRAPPER_H
#define SRSLTE_LOGGER_SRSLOG_WRAPPER_H

#include "srslte/common/logger.h"

namespace srslog {

class log_channel;

} // namespace srslog

namespace srslte {

/// This logger implementation uses the srsLog framework to write log entries.
class srslog_wrapper : public logger
{
public:
  explicit srslog_wrapper(srslog::log_channel& chan) : chan(chan) {}

  void log(unique_log_str_t msg) override;

private:
  srslog::log_channel& chan;
};

} // namespace srslte

#endif // SRSLTE_LOGGER_SRSLOG_WRAPPER_H
