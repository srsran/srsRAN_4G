// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSLOG_DETAIL_SUPPORT_ERROR_STRING_H
#define SRSLOG_DETAIL_SUPPORT_ERROR_STRING_H

#include <string>

namespace srslog {

namespace detail {

/// This is a lightweight error class that encapsulates a string for error
/// reporting.
class error_string
{
  std::string error;

public:
  error_string() = default;

  /*implicit*/ error_string(std::string error) : error(std::move(error)) {}
  /*implicit*/ error_string(const char* error) : error(error) {}

  /// Returns the error string.
  const std::string& get_error() const { return error; }

  explicit operator bool() const { return !error.empty(); }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_ERROR_STRING_H
