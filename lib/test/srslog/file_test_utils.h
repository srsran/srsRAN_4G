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

#ifndef SRSLOG_FILE_TEST_UTILS_H
#define SRSLOG_FILE_TEST_UTILS_H

#include <cstdio>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

namespace file_test_utils {

/// Removes a list of files from the file system on object destruction.
class scoped_file_deleter
{
  std::vector<std::string> paths;

public:
  explicit scoped_file_deleter(const std::string& path) : paths({path}) {}
  scoped_file_deleter(std::initializer_list<std::string> init) : paths(init) {}

  ~scoped_file_deleter()
  {
    for (const auto& path : paths) {
      ::remove(path.c_str());
    }
  }
};

/// Returns true if a file exists in the specified path, otherwise returns
/// false.
inline bool file_exists(const std::string& path)
{
  return ::access(path.c_str(), F_OK) == 0;
}

/// Reads the contents of the file specified in path and returns true if they
/// match with the data in entries, otherwise returns false.
inline bool compare_file_contents(const std::string& path, const std::vector<std::string>& entries)
{
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    return false;
  }

  std::vector<std::string> data;
  for (std::string line; std::getline(file, line);) {
    data.push_back(line + '\n');
  }

  return (data == entries);
}

} // namespace file_test_utils

#endif // SRSLOG_FILE_TEST_UTILS_H
