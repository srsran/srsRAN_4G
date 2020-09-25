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

/******************************************************************************
 *  File:         log.h
 *
 *  Description:  Abstract logging service
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_LOG_H
#define SRSLTE_LOG_H

#include "srslte/common/standard_streams.h"
#include <algorithm>
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
static const char log_level_text[LOG_LEVEL_N_ITEMS][16] = {"None   ", "Error  ", "Warning", "Info   ", "Debug  "};

static const char log_level_text_short[LOG_LEVEL_N_ITEMS][16] = {"[-]", "[E]", "[W]", "[I]", "[D]"};

class log
{
public:
  log()
  {
    service_name  = "";
    tti           = 0;
    level         = LOG_LEVEL_NONE;
    hex_limit     = 0;
    add_string_en = false;
  }

  explicit log(std::string service_name_)
  {
    service_name  = std::move(service_name_);
    tti           = 0;
    level         = LOG_LEVEL_NONE;
    hex_limit     = 0;
    add_string_en = false;
  }

  log(const log&) = delete;
  log& operator=(const log&) = delete;

  virtual ~log() = default;

  // This function shall be called at the start of every tti for printing tti
  void step(uint32_t tti_)
  {
    tti           = tti_;
    add_string_en = false;
  }

  void prepend_string(std::string s)
  {
    add_string_en  = true;
    add_string_val = std::move(s);
  }

  uint32_t get_tti() { return tti; }

  void set_level(LOG_LEVEL_ENUM l) { level = l; }

  void set_level(std::string l) { set_level(get_level_from_string(std::move(l))); }

  static srslte::LOG_LEVEL_ENUM get_level_from_string(std::string l)
  {
    std::transform(l.begin(), l.end(), l.begin(), ::toupper);
    if ("NONE" == l) {
      return srslte::LOG_LEVEL_NONE;
    } else if ("ERROR" == l) {
      return srslte::LOG_LEVEL_ERROR;
    } else if ("WARNING" == l) {
      return srslte::LOG_LEVEL_WARNING;
    } else if ("INFO" == l) {
      return srslte::LOG_LEVEL_INFO;
    } else if ("DEBUG" == l) {
      return srslte::LOG_LEVEL_DEBUG;
    } else {
      return srslte::LOG_LEVEL_NONE;
    }
  }

  LOG_LEVEL_ENUM     get_level() { return level; }
  const std::string& get_service_name() const { return service_name; }

  void set_hex_limit(int limit) { hex_limit = limit; }
  int  get_hex_limit() { return hex_limit; }

  // Pure virtual methods for logging
  virtual void error(const char* message, ...) __attribute__((format(printf, 2, 3)))      = 0;
  virtual void warning(const char* message, ...) __attribute__((format(printf, 2, 3)))    = 0;
  virtual void info(const char* message, ...) __attribute__((format(printf, 2, 3)))       = 0;
  virtual void info_long(const char* message, ...) __attribute__((format(printf, 2, 3)))  = 0;
  virtual void debug(const char* message, ...) __attribute__((format(printf, 2, 3)))      = 0;
  virtual void debug_long(const char* message, ...) __attribute__((format(printf, 2, 3))) = 0;

  // Same with hex dump
  virtual void error_hex(const uint8_t*, int, const char*, ...) __attribute__((format(printf, 4, 5)))
  {
    error("error_hex not implemented.\n");
  }
  virtual void warning_hex(const uint8_t*, int, const char*, ...) __attribute__((format(printf, 4, 5)))
  {
    error("warning_hex not implemented.\n");
  }
  virtual void info_hex(const uint8_t*, int, const char*, ...) __attribute__((format(printf, 4, 5)))
  {
    error("info_hex not implemented.\n");
  }
  virtual void debug_hex(const uint8_t*, int, const char*, ...) __attribute__((format(printf, 4, 5)))
  {
    error("debug_hex not implemented.\n");
  }

protected:
  uint32_t       tti;
  LOG_LEVEL_ENUM level;
  int            hex_limit;

  bool        add_string_en;
  std::string add_string_val;
  std::string service_name;
};

} // namespace srslte

#endif // SRSLTE_LOG_H
