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

#ifndef SRSLOG_STREAM_SINK_H
#define SRSLOG_STREAM_SINK_H

#include "srslte/srslog/sink.h"
#include <cassert>
#include <cstdio>

namespace srslog {

/// Standard stream types.
enum class sink_stream_type { stdout, stderr };

/// This sink implementation writes to either stdout or stderr streams.
class stream_sink : public sink
{
public:
  explicit stream_sink(sink_stream_type s) :
    handle((s == sink_stream_type::stdout) ? stdout : stderr)
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
