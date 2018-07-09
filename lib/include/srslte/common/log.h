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
 *  File:         log.h
 *
 *  Description:  Abstract logging service
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_LOG_H
#define SRSLTE_LOG_H

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
static const char log_level_text[LOG_LEVEL_N_ITEMS][16] = {"None   ",
                                                           "Error  ",
                                                           "Warning",
                                                           "Info   ",
                                                           "Debug  "};

static const char log_level_text_short[LOG_LEVEL_N_ITEMS][16] = {"[-]",
                                                                 "[E]",
                                                                 "[W]",
                                                                 "[I]",
                                                                 "[D]"};

class log
{
public:

  log() {
    service_name = "";
    tti = 0;
    level = LOG_LEVEL_NONE;
    hex_limit = 0;
    show_layer_en = true;
    add_string_en = false;
    level_text_short = true;
  }

  log(std::string service_name_) {
    service_name = service_name_;
    tti = 0;
    level = LOG_LEVEL_NONE;
    hex_limit = 0;
    show_layer_en = true;
    add_string_en = false;
    level_text_short = true;
  }

  virtual ~log() {};

  // This function shall be called at the start of every tti for printing tti
  void step(uint32_t tti_) {
    tti = tti_;
    add_string_en  = false;
  }

  void prepend_string(std::string s) {
    add_string_en  = true;
    add_string_val = s;
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
  void set_log_level_short(bool enable) {
    level_text_short = enable;
  }
  void show_layer(bool enable) {
    show_layer_en = enable;
  }

  // Pure virtual methods for logging
  virtual void console(const char * message, ...) __attribute__ ((format (printf, 2, 3))) = 0;
  virtual void error(const char * message, ...)   __attribute__ ((format (printf, 2, 3))) = 0;
  virtual void warning(const char * message, ...) __attribute__ ((format (printf, 2, 3))) = 0;
  virtual void info(const char * message, ...)    __attribute__ ((format (printf, 2, 3))) = 0;
  virtual void debug(const char * message, ...)   __attribute__ ((format (printf, 2, 3))) = 0;

  // Same with hex dump
  virtual void error_hex(const uint8_t *, int, const char *, ...)   __attribute__((format (printf, 4, 5)))
    {error("error_hex not implemented.\n");}
  virtual void warning_hex(const uint8_t *, int, const char *, ...) __attribute__((format (printf, 4, 5)))
    {error("warning_hex not implemented.\n");}
  virtual void info_hex(const uint8_t *, int, const char *, ...)    __attribute__((format (printf, 4, 5)))
    {error("info_hex not implemented.\n");}
  virtual void debug_hex(const uint8_t *, int, const char *, ...)   __attribute__((format (printf, 4, 5)))
    {error("debug_hex not implemented.\n");}

protected:
  std::string get_service_name() { return service_name; }
  uint32_t        tti;
  LOG_LEVEL_ENUM  level;
  int             hex_limit;
  std::string     service_name;

  bool        show_layer_en;
  bool        level_text_short;
  bool        add_string_en;
  std::string add_string_val;
};

} // namespace srslte

#endif // SRSLTE_LOG_H

