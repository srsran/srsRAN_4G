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
