/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "src/srslog/sinks/syslog_sink.h"
#include "srsran/srslog/srslog.h"
#include "test_dummies.h"
#include "testing_helpers.h"

#include <fstream>
#include <sstream>

using namespace srslog;

/// Syslog sink name.
static constexpr char sink_name[] = "srslog_syslog_sink";

static bool find_string_infile(std::string filename, std::string pattern)
{
  std::ifstream file(filename);
  std::string   line;
  bool          found = false;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.find(pattern) != std::string::npos) { // WILL SEARCH 2015-1113 in file
        found = true;
      }
    }
  } else {
    printf("WARNING: Could not open file %s", filename.c_str());
  }
  return found;
}

static bool syslog_basic_test()
{
  syslog_sink syslog_sink(get_default_log_formatter());

  // Build a 1000 byte entry.
  std::string entry(1000, 'a');

  syslog_sink.write(detail::memory_buffer(entry));

  syslog_sink.flush();

  ASSERT_EQ(find_string_infile("/var/log/syslog", entry), true);
  return true;
}

int main()
{
  TEST_FUNCTION(syslog_basic_test);
  return 0;
}