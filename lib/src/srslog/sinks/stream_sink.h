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

#ifndef SRSLOG_STREAM_SINK_H
#define SRSLOG_STREAM_SINK_H

#include "srsran/srslog/sink.h"

namespace srslog {

/// Standard stream types.
enum class sink_stream_type { stdout, stderr };

/// This sink implementation writes to either stdout or stderr streams.
class stream_sink : public sink
{
public:
  stream_sink(sink_stream_type s, std::unique_ptr<log_formatter> f) :
    sink(std::move(f)), handle((s == sink_stream_type::stdout) ? stdout : stderr)
  {}

  stream_sink(const stream_sink& other) = delete;
  stream_sink& operator=(const stream_sink& other) = delete;

  detail::error_string write(detail::memory_buffer buffer) override
  {
    assert(handle && "Invalid stream handle");
    std::fwrite(buffer.data(), sizeof(char), buffer.size(), handle);
    // We want to see the output instantly.
    std::fflush(handle);
    return {};
  }

  detail::error_string flush() override
  {
    std::fflush(handle);
    return {};
  }

private:
  std::FILE* handle;
};

} // namespace srslog

#endif // SRSLOG_STREAM_SINK_H
