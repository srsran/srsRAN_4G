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

/*! \brief Utility functions for the DUT.
 *
 */

#ifndef SRSUE_TTCN3_DUT_UTILS_H
#define SRSUE_TTCN3_DUT_UTILS_H

#include <set>
#include <sstream>
#include <vector>

std::string get_filename_with_tc_name(const std::string& str, const uint32_t run_id, const std::string tc_name)
{
  // split base path into components
  std::set<char> delims;
  delims.insert('/');
  std::vector<std::string> result;
  char const*              pch   = str.c_str();
  char const*              start = pch;
  for (; *pch; ++pch) {
    if (delims.find(*pch) != delims.end()) {
      if (start != pch) {
        std::string str(start, pch);
        result.push_back(str);
      } else {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  // prepend TC name to last element (the actual filename)
  std::stringstream filename_ss;
  filename_ss << tc_name << "_"
              << "run" << run_id << "_" << result.back();

  std::string                        final_path;
  std::vector<std::string>::iterator path_it;
  for (path_it = result.begin() + 1; path_it != --result.end(); ++path_it) {
    final_path += "/";
    final_path += *path_it;
  }
  final_path += "/";
  final_path += filename_ss.str();

  return final_path;
}

std::string get_tc_name(const std::string& str)
{
  // split after dot
  std::string::size_type pos = str.find('.');
  return (pos == str.npos ? str : str.substr(pos + 1, -1));
}

#endif // SRSUE_TTCN3_DUT_UTILS_H
