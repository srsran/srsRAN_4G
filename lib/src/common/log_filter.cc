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

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <sys/time.h>

#include "srslte/common/log_filter.h"

namespace srslte{

log_filter::log_filter()
{
  do_tti      = false;
  time_src    = NULL;
  time_format = TIME;
  logger_h    = NULL;
}

log_filter::log_filter(std::string layer)
{
  do_tti      = false;
  time_src    = NULL;
  time_format = TIME;
  init(layer, &def_logger_stdout, tti);
}

log_filter::log_filter(std::string layer, logger *logger_, bool tti)
{
  do_tti      = false;
  time_src    = NULL;
  time_format = TIME;
  init(layer, logger_, tti);
}

void log_filter::init(std::string layer, logger *logger_, bool tti)
{
  service_name  = layer;
  logger_h      = logger_;
  do_tti        = tti;
}

void log_filter::all_log(srslte::LOG_LEVEL_ENUM level,
                         uint32_t               tti,
                         const char             *msg)
{
  if(logger_h) {
    std::stringstream ss;

    ss << now_time() << " ";
    if (show_layer_en) {
      ss << "[" <<get_service_name() << "] ";
    }
    if (level_text_short) {
      ss << log_level_text_short[level] << " ";
    } else {
      ss << log_level_text[level] << " ";
    }
    if(do_tti) {
      ss << "[" << std::setfill('0') << std::setw(5) << tti << "] ";
    }
    if (add_string_en) {
      ss << add_string_val << " ";
    }
    ss << msg;

    str_ptr s_ptr(new std::string(ss.str()));
    logger_h->log(s_ptr);
  }
}

void log_filter::all_log(srslte::LOG_LEVEL_ENUM level,
                         uint32_t               tti,
                         const char            *msg,
                         const uint8_t         *hex,
                         int                    size)
{
  if(logger_h) {
    std::stringstream ss;

    ss << now_time() << " ";
    if (show_layer_en) {
      ss << "[" <<get_service_name() << "] ";
    }
    if (level_text_short) {
      ss << log_level_text_short[level] << " ";
    } else {
      ss << log_level_text[level] << " ";
    }

    if(do_tti) {
      ss << "[" << std::setfill('0') << std::setw(5) << tti << "] ";
    }

    if (add_string_en) {
      ss << add_string_val << " ";
    }
    ss << msg;
    
    if (msg[strlen(msg)-1] != '\n') {
      ss << std::endl; 
    }
    
    if (hex_limit > 0 && hex && size > 0) {
      ss << hex_string(hex, size);
    } 
    str_ptr s_ptr(new std::string(ss.str()));
    logger_h->log(s_ptr);
  }
}

void log_filter::console(const char * message, ...) {
  char     *args_msg = NULL;
  va_list   args;
  va_start(args, message);
  if(vasprintf(&args_msg, message, args) > 0)
    printf("%s",args_msg); // Print directly to stdout
  va_end(args);
  free(args_msg);
}

void log_filter::error(const char * message, ...) {
  if (level >= LOG_LEVEL_ERROR) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_ERROR, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::warning(const char * message, ...) {
  if (level >= LOG_LEVEL_WARNING) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_WARNING, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::info(const char * message, ...) {
  if (level >= LOG_LEVEL_INFO) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_INFO, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::debug(const char * message, ...) {
  if (level >= LOG_LEVEL_DEBUG) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_DEBUG, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::error_hex(const uint8_t *hex, int size, const char * message, ...) {
  if (level >= LOG_LEVEL_ERROR) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_ERROR, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::warning_hex(const uint8_t *hex, int size, const char * message, ...) {
  if (level >= LOG_LEVEL_WARNING) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_WARNING, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::info_hex(const uint8_t *hex, int size, const char * message, ...) {
  if (level >= LOG_LEVEL_INFO) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_INFO, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::debug_hex(const uint8_t *hex, int size, const char * message, ...) {
  if (level >= LOG_LEVEL_DEBUG) {
    char     *args_msg = NULL;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message, args) > 0)
      all_log(LOG_LEVEL_DEBUG, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::set_time_src(time_itf *source, time_format_t format) {
  this->time_src    = source;
  this->time_format = format;
}

std::string log_filter::now_time()
{
  struct timeval rawtime;
  struct tm * timeinfo;
  char buffer[64];
  char us[16];

  srslte_timestamp_t now;
  uint64_t usec_epoch;

  if (!time_src) {
    gettimeofday(&rawtime, NULL);
    timeinfo = localtime(&rawtime.tv_sec);

    if (time_format == TIME) {
      strftime(buffer, 64, "%H:%M:%S", timeinfo);
      strcat(buffer, ".");
      snprintf(us, 16, "%06ld", rawtime.tv_usec);
      strcat(buffer, us);
    } else {
      usec_epoch = rawtime.tv_sec * 1000000 + rawtime.tv_usec;
      snprintf(buffer, 64, "%ld", usec_epoch);
    }
  } else {
    now = time_src->get_time();

    if (time_format == TIME) {
      snprintf(buffer, 64, "%ld:%06u", now.full_secs, (uint32_t) (now.frac_secs * 1e6));
    } else {
      usec_epoch = now.full_secs * 1000000 + (uint32_t) (now.frac_secs * 1e6);
      snprintf(buffer, 64, "%ld", usec_epoch);
    }
  }

  return std::string(buffer);
}

std::string log_filter::hex_string(const uint8_t *hex, int size)
{
  std::stringstream ss;
  int c = 0;

  ss << std::hex << std::setfill('0');
  if(hex_limit >= 0) {
    size = (size > hex_limit) ? hex_limit : size;
  }
  while(c < size) {
    ss << "             " << std::setw(4) << static_cast<unsigned>(c) << ": ";
    int tmp = (size-c < 16) ? size-c : 16;
    for(int i=0;i<tmp;i++) {
      ss << std::setw(2) << static_cast<unsigned>(hex[c++]) << " ";
    }
    ss << "\n";
  }
  return ss.str();
}

} // namespace srsue
