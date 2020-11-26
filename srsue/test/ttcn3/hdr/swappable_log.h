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

#ifndef SRSUE_TTCN3_SWAPPABLE_LOG_H
#define SRSUE_TTCN3_SWAPPABLE_LOG_H

#include "srslte/common/logger_srslog_wrapper.h"

/// This is a log wrapper that allows hot swapping the underlying log instance.
class swappable_log : public srslte::logger
{
public:
  explicit swappable_log(std::unique_ptr<srslte::srslog_wrapper> log) : l(std::move(log)) {}

  void log(unique_log_str_t msg) override
  {
    assert(l && "Missing log instance");
    l->log(std::move(msg));
  }

  /// Swaps the underlying log wrapper.
  void swap_log(std::unique_ptr<srslte::srslog_wrapper> new_log) { l = std::move(new_log); }

private:
  std::unique_ptr<srslte::srslog_wrapper> l;
};

#endif // SRSUE_TTCN3_SWAPPABLE_LOG_H
