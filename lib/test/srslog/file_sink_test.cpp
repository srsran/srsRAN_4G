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

#include "file_test_utils.h"
#include "src/srslog/sinks/file_sink.h"
#include "testing_helpers.h"

using namespace srslog;

static constexpr char log_filename[] = "file_sink_test.log";

static bool when_data_is_written_to_file_then_contents_are_valid()
{
  file_test_utils::scoped_file_deleter deleter(log_filename);
  file_sink file(log_filename, 0);

  std::vector<std::string> entries;
  for (unsigned i = 0; i != 10; ++i) {
    std::string entry = "Test log entry - " + std::to_string(i) + '\n';
    file.write(detail::memory_buffer(entry));
    entries.push_back(entry);
  }

  file.flush();

  ASSERT_EQ(file_test_utils::file_exists(log_filename), true);
  ASSERT_EQ(file_test_utils::compare_file_contents(log_filename, entries),
            true);

  return true;
}

/// A Test-Specific Subclass of file_sink. This subclass provides public access
/// to the data members of the parent class.
class file_sink_subclass : public file_sink
{
public:
  file_sink_subclass(std::string name, size_t max_size) :
    file_sink(std::move(name), max_size)
  {}

  uint32_t get_num_of_files() const { return get_file_index(); }
};

static bool when_data_written_exceeds_size_threshold_then_new_file_is_created()
{
  std::string filename0 =
      file_utils::build_filename_with_index(log_filename, 0);
  std::string filename1 =
      file_utils::build_filename_with_index(log_filename, 1);
  std::string filename2 =
      file_utils::build_filename_with_index(log_filename, 2);
  file_test_utils::scoped_file_deleter deleter = {
      filename0, filename1, filename2};

  file_sink_subclass file(log_filename, 5001);

  // Build a 1000 byte entry.
  std::string entry(1000, 'a');

  // Fill in the file with 5000 bytes, one byte less than the threshold.
  for (unsigned i = 0; i != 5; ++i) {
    file.write(detail::memory_buffer(entry));
  }
  file.flush();

  // Only one file should exist.
  ASSERT_EQ(file.get_num_of_files(), 1);

  // Trigger a file rotation.
  file.write(detail::memory_buffer(entry));
  file.flush();

  // A second file should be created.
  ASSERT_EQ(file.get_num_of_files(), 2);

  // Fill in the second file with 4000 bytes, one byte less than the threshold.
  for (unsigned i = 0; i != 4; ++i) {
    file.write(detail::memory_buffer(entry));
  }
  file.flush();

  // Two files should exist, third should not be created yet.
  ASSERT_EQ(file.get_num_of_files(), 2);

  // Trigger a file rotation.
  file.write(detail::memory_buffer(entry));
  file.flush();

  // Three files should exist.
  ASSERT_EQ(file.get_num_of_files(), 3);

  return true;
}

int main()
{
  TEST_FUNCTION(when_data_is_written_to_file_then_contents_are_valid);
  TEST_FUNCTION(
      when_data_written_exceeds_size_threshold_then_new_file_is_created);

  return 0;
}
