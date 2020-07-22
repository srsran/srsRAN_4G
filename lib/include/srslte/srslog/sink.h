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

#ifndef SRSLOG_SINK_H
#define SRSLOG_SINK_H

#include "srslte/srslog/detail/support/error_string.h"
#include "srslte/srslog/detail/support/memory_buffer.h"

namespace srslog {

/// This interface provides the way to write incoming memory buffers to any kind
/// of backing store.
class sink
{
public:
  virtual ~sink() = default;

  /// Writes the provided memory buffer into the sink.
  virtual detail::error_string write(detail::memory_buffer buffer) = 0;

  /// Flushes any buffered contents to the backing store.
  virtual detail::error_string flush() = 0;
};

} // namespace srslog

#endif // SRSLOG_SINK_H
