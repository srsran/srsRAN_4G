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

#ifndef SRSLOG_FILE_UTILS_H
#define SRSLOG_FILE_UTILS_H

#include "srslte/srslog/bundled/fmt/format.h"
#include "srslte/srslog/detail/support/error_string.h"
#include "srslte/srslog/detail/support/memory_buffer.h"

namespace srslog {

namespace file_utils {

/// Helper function that formats errnos nicely.
inline std::string format_error(const std::string& error, int error_code)
{
  fmt::memory_buffer result;
  fmt::format_system_error(result, error_code, error);

  return fmt::to_string(result);
}

/// Splits the specified path into a filename and its extension (if present).
inline std::pair<std::string, std::string>
split_filename_extension(const std::string& filename)
{
  // Search for the last dot.
  auto dot_pos = filename.find_last_of('.');

  // Check for the following corner cases (returns {filename, ""}):
  //   a) No dot found: my_file
  //   b) Dot found at the beginning: .my_file
  //   c) Dot found at the end: my_file.
  if (dot_pos == std::string::npos || dot_pos == 0 ||
      dot_pos == filename.size() - 1) {
    return {filename, ""};
  }

  // Handle directories that contain dots, search for the last separator
  // character.
  auto separator_pos = filename.find_last_of('/');

  // Check for the following corner cases (returns {filename, ""}):
  //   a) /my_folder.1/my_file
  //   b) /my_folder.1/.my_file
  if (separator_pos != std::string::npos && separator_pos >= dot_pos - 1) {
    return {filename, ""};
  }

  return {filename.substr(0, dot_pos), filename.substr(dot_pos)};
}

/// Builds a file name formatting the input base name and file index.
inline std::string build_filename_with_index(const std::string& basename,
                                             size_t index)
{
  if (index == 0) {
    return basename;
  }

  auto result = split_filename_extension(basename);
  return fmt::format("{}.{}{}", result.first, index, result.second);
}

/// This class provides basic file operations and disables itself when it
/// encounters an error.
class file
{
  std::string path;
  std::FILE* handle = nullptr;

public:
  ~file() { close(); }

  explicit operator bool() const { return handle; }

  /// Returns the handle of the underlying file.
  std::FILE* get_handle() { return handle; }

  /// Returns the path of the file.
  const std::string& get_path() const { return path; }

  /// Creates a new file in the specified path by previously closing any opened
  /// file.
  detail::error_string create(const std::string& new_path)
  {
    close();

    if ((handle = std::fopen(new_path.c_str(), "wb"))) {
      path = new_path;
      return {};
    }

    return format_error(fmt::format("Unable to create log file \"{}\"", path),
                        errno);
  }

  /// Writes the provided memory buffer into an open file, otherwise does
  /// nothing.
  detail::error_string write(detail::memory_buffer buffer)
  {
    if (handle &&
        std::fwrite(buffer.data(), sizeof(char), buffer.size(), handle) !=
            buffer.size()) {
      close();
      return format_error(fmt::format("Unable to write log file \"{}\"", path),
                          errno);
    }

    return {};
  }

  /// Flushes the contents of an open file, otherwise does nothing.
  detail::error_string flush()
  {
    if (handle && ::fflush(handle) == EOF) {
      close();
      return format_error(
          fmt::format("Error encountered while flushing log file \"{}\"", path),
          errno);
    }

    return {};
  }

  /// Closes an open file, otherwise does nothing.
  void close()
  {
    if (handle) {
      ::fclose(handle);
      handle = nullptr;
      path.clear();
    }
  }
};

} // namespace file_utils

} // namespace srslog

#endif // SRSLOG_FILE_UTILS_H
