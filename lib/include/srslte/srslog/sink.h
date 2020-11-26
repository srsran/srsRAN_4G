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
