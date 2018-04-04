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
 * File:        log_filter.h
 * Description: Log filter for a specific layer or element.
 *              Performs filtering based on log level, generates
 *              timestamped log strings and passes them to the
 *              common logger object.
 *****************************************************************************/

#ifndef SRSLTE_LOG_FILTER_H
#define SRSLTE_LOG_FILTER_H

#include <stdarg.h>
#include <string>

#include "srslte/phy/common/timestamp.h"
#include "srslte/common/log.h"
#include "srslte/common/logger.h"
#include "srslte/common/logger_stdout.h"

namespace srslte {

typedef std::string* str_ptr;

class log_filter : public srslte::log
{
public:

  log_filter();
  log_filter(std::string layer);
  log_filter(std::string layer, logger *logger_, bool tti=false);

  void init(std::string layer, logger *logger_, bool tti=false);

  void console(const char * message, ...) __attribute__ ((format (printf, 2, 3)));
  void error(const char * message, ...)   __attribute__ ((format (printf, 2, 3)));
  void warning(const char * message, ...) __attribute__ ((format (printf, 2, 3)));
  void info(const char * message, ...)    __attribute__ ((format (printf, 2, 3)));
  void debug(const char * message, ...)   __attribute__ ((format (printf, 2, 3)));

  void error_hex(const uint8_t *hex, int size, const char * message, ...)   __attribute__((format (printf, 4, 5)));
  void warning_hex(const uint8_t *hex, int size, const char * message, ...) __attribute__((format (printf, 4, 5)));
  void info_hex(const uint8_t *hex, int size, const char * message, ...)    __attribute__((format (printf, 4, 5)));
  void debug_hex(const uint8_t *hex, int size, const char * message, ...)   __attribute__((format (printf, 4, 5)));

  class time_itf {
  public:
    virtual srslte_timestamp_t get_time() = 0;
  };

  typedef enum {
    TIME,
    EPOCH
  } time_format_t;

  void set_time_src(time_itf *source, time_format_t format);

private:
  logger *logger_h;
  bool    do_tti;

  time_itf      *time_src;
  time_format_t time_format;

  logger_stdout def_logger_stdout;

  void all_log(srslte::LOG_LEVEL_ENUM level, uint32_t tti, const char *msg);
  void all_log(srslte::LOG_LEVEL_ENUM level, uint32_t tti, const char *msg, const uint8_t *hex, int size);
  void all_log_line(srslte::LOG_LEVEL_ENUM level, uint32_t tti, std::string file, int line, char *msg);
  std::string now_time();
  std::string hex_string(const uint8_t *hex, int size);
};

} // namespace srsue

#endif // SRSLTE_LOG_FILTER_H
