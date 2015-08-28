/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

/******************************************************************************
 *  File:         log.h
 *
 *  Description:  Abstract logging service
 *
 *  Reference:
 *****************************************************************************/

#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <string>

namespace srslte {

typedef enum {
  LOG_LEVEL_NONE = 0,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_N_ITEMS
} LOG_LEVEL_ENUM;
static const char log_level_text[LOG_LEVEL_N_ITEMS][16] = {"None",
                                                           "Error",
                                                           "Warning",
                                                           "Info",
                                                           "Debug"};

class log
{
public:

  log(std::string service_name_) {
    service_name = service_name_;
    tti = 0;
    level = LOG_LEVEL_NONE;
  }
  
  // This function shall be called at the start of every tti for printing tti 
  void step(uint32_t tti_) {
    tti = tti_; 
  }
  uint32_t get_tti() {
    return tti;
  }
  
  void set_level(LOG_LEVEL_ENUM l) {
    level = l;
  }
  LOG_LEVEL_ENUM get_level() {
    return level;
  }
  
  void set_hex_limit(int limit) {
    hex_limit = limit;
  }
  int get_hex_limit() {
    return hex_limit;
  }
  
  // Pure virtual methods for logging
  virtual void error(std::string message, ...)   = 0;
  virtual void warning(std::string message, ...) = 0;
  virtual void info(std::string message, ...)    = 0;
  virtual void debug(std::string message, ...)   = 0;

  // Same with hex dump
  virtual void error_hex(uint8_t *hex, int size, std::string message, ...){error("error_hex not implemented.");}
  virtual void warning_hex(uint8_t *hex, int size, std::string message, ...){error("warning_hex not implemented.");}
  virtual void info_hex(uint8_t *hex, int size, std::string message, ...){error("info_hex not implemented.");}
  virtual void debug_hex(uint8_t *hex, int size, std::string message, ...){error("debug_hex not implemented.");}
  
  // Same with line and file info
  virtual void error_line(std::string file, int line, std::string message, ...){error("error_line not implemented.");}
  virtual void warning_line(std::string file, int line, std::string message, ...){error("warning_line not implemented.");}
  virtual void info_line(std::string file, int line, std::string message, ...){error("info_line not implemented.");}
  virtual void debug_line(std::string file, int line, std::string message, ...){error("debug_line not implemented.");}
  
protected: 
  std::string get_service_name() { return service_name; }
  uint32_t        tti;
  LOG_LEVEL_ENUM  level;
  int             hex_limit;
private:
  std::string     service_name;
};

} // namespace srslte

#endif // LOG_H
  
