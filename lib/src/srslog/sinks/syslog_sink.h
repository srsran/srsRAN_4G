/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSLOG_SYSLOG_SINK_H
#define SRSLOG_SYSLOG_SINK_H

#include "srsran/srslog/shared_types.h"
#include "srsran/srslog/sink.h"
#include <syslog.h>

namespace srslog {

/// This sink implementation writes to syslog.
class syslog_sink : public sink
{
public:
  syslog_sink(std::unique_ptr<log_formatter> f,
              std::string                    preamble_  = "",
              syslog_local_type              log_local_ = syslog_local_type::local0) :
    sink(std::move(f))
  {
    create_syslog(preamble_, syslog_translate(log_local_));
  }

  syslog_sink(const syslog_sink& other) = delete;
  syslog_sink& operator=(const syslog_sink& other) = delete;

  detail::error_string write(detail::memory_buffer buffer) override
  {
    std::string entry(buffer.data(), buffer.size());
    if (entry.find("[E]") != std::string::npos) {
      syslog(LOG_ERR, "%s", buffer.data());
    } else if (entry.find("[W]") != std::string::npos) {
      syslog(LOG_WARNING, "%s", buffer.data());
    } else if (entry.find("[I]") != std::string::npos) {
      syslog(LOG_INFO, "%s", buffer.data());
    } else if (entry.find("[D]") != std::string::npos) {
      syslog(LOG_DEBUG, "%s", buffer.data());
    } else {
      syslog(LOG_ERR, "%s", buffer.data());
    }
    // openlog syslog does not return any value.
    return {};
  }

  detail::error_string flush() override { return {}; }

private:
  /// Creates a new syslog
  detail::error_string create_syslog(std::string preamble, int log_local)
  {
    if (preamble == "") {
      openlog(NULL, LOG_CONS | LOG_PID | LOG_NDELAY, log_local);
    } else {
      openlog(preamble.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY, log_local);
    }
    return {};
  }

  static int syslog_translate(syslog_local_type log_local)
  {
    switch (log_local) {
      case syslog_local_type::local0:
        return LOG_LOCAL0;
      case syslog_local_type::local1:
        return LOG_LOCAL1;
      case syslog_local_type::local2:
        return LOG_LOCAL2;
      case syslog_local_type::local3:
        return LOG_LOCAL3;
      case syslog_local_type::local4:
        return LOG_LOCAL4;
      case syslog_local_type::local5:
        return LOG_LOCAL5;
      case syslog_local_type::local6:
        return LOG_LOCAL6;
      case syslog_local_type::local7:
        return LOG_LOCAL7;
      default:
        return LOG_LOCAL0;
        break;
    }
  };
};

} // namespace srslog

#endif // SRSLOG_SYSLOG_SINK_H
