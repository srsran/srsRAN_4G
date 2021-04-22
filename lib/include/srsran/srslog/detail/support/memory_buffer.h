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

#ifndef SRSLOG_DETAIL_SUPPORT_MEMORY_BUFFER_H
#define SRSLOG_DETAIL_SUPPORT_MEMORY_BUFFER_H

#include <string>

namespace srslog {

namespace detail {

/// This class wraps a read-only and non owning memory block, providing simple
/// methods to access its contents.
class memory_buffer
{
  const char* const buffer;
  const size_t      length;

public:
  memory_buffer(const char* buffer, size_t length) : buffer(buffer), length(length) {}

  explicit memory_buffer(const std::string& s) : buffer(s.data()), length(s.size()) {}

  /// Returns a pointer to the start of the memory block.
  const char* data() const { return buffer; }

  /// Returns an iterator to the beginning of the buffer.
  const char* begin() const { return buffer; }

  /// Returns an iterator to the end of the buffer.
  const char* end() const { return buffer + length; }

  /// Returns the size of the memory block.
  size_t size() const { return length; }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_MEMORY_BUFFER_H
