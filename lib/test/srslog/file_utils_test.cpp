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
#include "src/srslog/sinks/file_utils.h"
#include "testing_helpers.h"

using namespace srslog;

static constexpr char log_filename[] = "file_utils_test.log";
static constexpr char log_filename2[] = "file_utils_test2.log";

static bool filename_extension_split_test()
{
  auto checker = [](const std::string& base,
                    const std::string& file,
                    const std::string& ext) {
    auto result = file_utils::split_filename_extension(base);
    return (result.first == file && result.second == ext);
  };

  ASSERT_EQ(checker("filename", "filename", ""), true);
  ASSERT_EQ(checker(".filename", ".filename", ""), true);
  ASSERT_EQ(checker("filename.", "filename.", ""), true);
  ASSERT_EQ(checker(".filename.", ".filename.", ""), true);
  ASSERT_EQ(checker("filename.txt", "filename", ".txt"), true);

  ASSERT_EQ(checker("/a/b/filename", "/a/b/filename", ""), true);
  ASSERT_EQ(checker("/a/b/.filename", "/a/b/.filename", ""), true);
  ASSERT_EQ(checker("/a/b/filename.", "/a/b/filename.", ""), true);
  ASSERT_EQ(checker("/a/b/.filename.", "/a/b/.filename.", ""), true);
  ASSERT_EQ(checker("/a/b/filename.txt", "/a/b/filename", ".txt"), true);

  return true;
}

static bool filename_with_index_build_test()
{
  ASSERT_EQ(file_utils::build_filename_with_index("filename", 0), "filename");
  ASSERT_EQ(file_utils::build_filename_with_index("filename", 1), "filename.1");

  ASSERT_EQ(file_utils::build_filename_with_index("filename.txt", 0),
            "filename.txt");
  ASSERT_EQ(file_utils::build_filename_with_index("filename.txt", 1),
            "filename.1.txt");

  return true;
}

static bool when_default_constructing_file_then_no_file_is_created()
{
  file_utils::file f;

  ASSERT_EQ(f.get_handle(), nullptr);
  ASSERT_EQ(f.get_path().empty(), true);

  return true;
}

static bool when_created_method_is_called_then_file_is_created()
{
  file_test_utils::scoped_file_deleter deleter(log_filename);
  file_utils::file f;

  auto err_str = f.create(log_filename);
  ASSERT_EQ(err_str.get_error().empty(), true);

  ASSERT_NE(f.get_handle(), nullptr);
  ASSERT_EQ(f.get_path(), log_filename);

  ASSERT_EQ(file_test_utils::file_exists(log_filename), true);

  return true;
}

static bool
when_created_method_is_called_twice_then_file_is_closed_and_created()
{
  file_test_utils::scoped_file_deleter deleter = {log_filename, log_filename2};
  file_utils::file f;

  auto err_str = f.create(log_filename);
  ASSERT_EQ(err_str.get_error().empty(), true);
  ASSERT_EQ(file_test_utils::file_exists(log_filename), true);

  err_str = f.create(log_filename2);
  ASSERT_EQ(err_str.get_error().empty(), true);
  ASSERT_EQ(file_test_utils::file_exists(log_filename2), true);

  ASSERT_NE(f.get_handle(), nullptr);
  ASSERT_EQ(f.get_path(), log_filename2);

  return true;
}

static bool when_write_method_is_called_with_no_file_then_nothing_happens()
{
  file_utils::file f;

  auto err_str = f.write(detail::memory_buffer("test"));
  ASSERT_EQ(err_str.get_error().empty(), true);

  return true;
}

static bool when_flush_method_is_called_with_no_file_then_nothing_happens()
{
  file_utils::file f;

  auto err_str = f.flush();
  ASSERT_EQ(err_str.get_error().empty(), true);

  return true;
}

static bool when_data_is_written_to_file_then_contents_are_valid()
{
  file_test_utils::scoped_file_deleter deleter(log_filename);
  file_utils::file f;

  auto err_str = f.create(log_filename);
  ASSERT_EQ(err_str.get_error().empty(), true);
  ASSERT_EQ(file_test_utils::file_exists(log_filename), true);

  err_str = f.write(detail::memory_buffer("test\n"));
  ASSERT_EQ(err_str.get_error().empty(), true);

  err_str = f.flush();
  ASSERT_EQ(err_str.get_error().empty(), true);

  ASSERT_EQ(file_test_utils::compare_file_contents(log_filename, {"test\n"}),
            true);

  return true;
}

static bool when_file_is_opened_and_closed_then_members_are_reset()
{
  file_test_utils::scoped_file_deleter deleter(log_filename);
  file_utils::file f;

  auto err_str = f.create(log_filename);
  ASSERT_EQ(err_str.get_error().empty(), true);
  ASSERT_EQ(file_test_utils::file_exists(log_filename), true);

  f.close();

  ASSERT_EQ(f.get_handle(), nullptr);
  ASSERT_EQ(f.get_path().empty(), true);

  return true;
}

int main()
{
  TEST_FUNCTION(filename_extension_split_test);
  TEST_FUNCTION(filename_with_index_build_test);
  TEST_FUNCTION(when_default_constructing_file_then_no_file_is_created);
  TEST_FUNCTION(when_created_method_is_called_then_file_is_created);
  TEST_FUNCTION(
      when_created_method_is_called_twice_then_file_is_closed_and_created);
  TEST_FUNCTION(when_write_method_is_called_with_no_file_then_nothing_happens);
  TEST_FUNCTION(when_flush_method_is_called_with_no_file_then_nothing_happens);
  TEST_FUNCTION(when_data_is_written_to_file_then_contents_are_valid);
  TEST_FUNCTION(when_file_is_opened_and_closed_then_members_are_reset);

  return 0;
}
