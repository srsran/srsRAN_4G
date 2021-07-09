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
