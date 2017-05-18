/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         log_stout.h
 *
 *  Description:  Logging service through standard output. Inherits log interface
 *
 *  Reference:
 *****************************************************************************/

#ifndef LOGSTDOUT_H
#define LOGSTDOUT_H

#include <stdarg.h>
#include <string>
#include <common/log.h>

namespace srslte {
  
class log_stdout : public log
{
public:

  log_stdout(std::string service_name_) : log(service_name_) { }
  
  void console(std::string message, ...);
  void error(std::string message, ...);
  void warning(std::string message, ...);
  void info(std::string message, ...);
  void debug(std::string message, ...);

  // Same with hex dump
  void error_hex(uint8_t *hex, int size, std::string message, ...);
  void warning_hex(uint8_t *hex, int size, std::string message, ...);
  void info_hex(uint8_t *hex, int size, std::string message, ...);
  void debug_hex(uint8_t *hex, int size, std::string message, ...);

  // Same with line and file info
  void error_line(std::string file, int line, std::string message, ...);
  void warning_line(std::string file, int line, std::string message, ...);
  void info_line(std::string file, int line, std::string message, ...);
  void debug_line(std::string file, int line, std::string message, ...);

private:
  void printlog(srslte::LOG_LEVEL_ENUM level, uint32_t tti, std::string file, int line, std::string message, va_list args);
  void printlog(srslte::LOG_LEVEL_ENUM level, uint32_t tti, std::string message, va_list args);

  void all_log(srslte::LOG_LEVEL_ENUM level, uint32_t tti, char *msg);
  void all_log(srslte::LOG_LEVEL_ENUM level, uint32_t tti, char *msg, uint8_t *hex, int size);
  void all_log_line(srslte::LOG_LEVEL_ENUM level, uint32_t tti, std::string file, int line, char *msg);
  std::string now_time();
  std::string hex_string(uint8_t *hex, int size);
};

}

#endif
  
