/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
