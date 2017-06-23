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
  do_tti = false; 
}

log_filter::log_filter(std::string layer)
{
  init(layer, &def_logger_stdout, tti);
}

log_filter::log_filter(std::string layer, logger *logger_, bool tti)
{
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
                         char                  *msg)
{
  if(logger_h) {
    std::stringstream ss;

    ss << now_time() << " ";
    ss << "[" <<get_service_name() << "] ";
    ss << log_level_text[level] << " ";
    if(do_tti)
      ss << "[" << std::setfill('0') << std::setw(5) << tti << "] ";
    ss << msg;

    str_ptr s_ptr(new std::string(ss.str()));
    logger_h->log(s_ptr);
  }
}

void log_filter::all_log(srslte::LOG_LEVEL_ENUM level,
                         uint32_t               tti,
                         char                  *msg,
                         uint8_t               *hex,
                         int                    size)
{
  if(logger_h) {
    std::stringstream ss;

    ss << now_time() << " ";
    ss << "[" <<get_service_name() << "] ";
    ss << log_level_text[level] << " ";
    if(do_tti)
      ss << "[" << std::setfill('0') << std::setw(5) << tti << "] ";

    ss << msg;
    
    if (msg[strlen(msg)-1] != '\n') {
      ss << std::endl; 
    }
    
    if (hex_limit > 0) {
      ss << hex_string(hex, size);
    } 
    str_ptr s_ptr(new std::string(ss.str()));
    logger_h->log(s_ptr);
  }
}

void log_filter::all_log_line(srslte::LOG_LEVEL_ENUM level,
                              uint32_t               tti,
                              std::string            file,
                              int                    line,
                              char                  *msg)
{
  if(logger_h) {
    std::stringstream ss;

    ss << now_time() << " ";
    ss << "[" <<get_service_name() << "] ";
    ss << log_level_text[level] << " ";
    if(do_tti)
      ss << "[" << std::setfill('0') << std::setw(5) << tti << "] ";
    ss << msg;

    str_ptr s_ptr(new std::string(ss.str()));
    logger_h->log(s_ptr);
  }
}

void log_filter::console(std::string message, ...) {
  char     *args_msg;
  va_list   args;
  va_start(args, message);
  if(vasprintf(&args_msg, message.c_str(), args) > 0)
    printf("%s",args_msg); // Print directly to stdout
  va_end(args);
  free(args_msg);
}

void log_filter::error(std::string message, ...) {
  if (level >= LOG_LEVEL_ERROR) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_ERROR, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::warning(std::string message, ...) {
  if (level >= LOG_LEVEL_WARNING) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_WARNING, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::info(std::string message, ...) {
  if (level >= LOG_LEVEL_INFO) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_INFO, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::debug(std::string message, ...) {
  if (level >= LOG_LEVEL_DEBUG) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_DEBUG, tti, args_msg);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::error_hex(uint8_t *hex, int size, std::string message, ...) {
  if (level >= LOG_LEVEL_ERROR) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_ERROR, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::warning_hex(uint8_t *hex, int size, std::string message, ...) {
  if (level >= LOG_LEVEL_WARNING) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_WARNING, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::info_hex(uint8_t *hex, int size, std::string message, ...) {
  if (level >= LOG_LEVEL_INFO) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_INFO, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}
void log_filter::debug_hex(uint8_t *hex, int size, std::string message, ...) {
  if (level >= LOG_LEVEL_DEBUG) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log(LOG_LEVEL_DEBUG, tti, args_msg, hex, size);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::error_line(std::string file, int line, std::string message, ...)
{
  if (level >= LOG_LEVEL_ERROR) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log_line(LOG_LEVEL_ERROR, tti, file, line, args_msg);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::warning_line(std::string file, int line, std::string message, ...)
{
  if (level >= LOG_LEVEL_WARNING) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log_line(LOG_LEVEL_WARNING, tti, file, line, args_msg);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::info_line(std::string file, int line, std::string message, ...)
{
  if (level >= LOG_LEVEL_INFO) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log_line(LOG_LEVEL_INFO, tti, file, line, args_msg);
    va_end(args);
    free(args_msg);
  }
}

void log_filter::debug_line(std::string file, int line, std::string message, ...)
{
  if (level >= LOG_LEVEL_DEBUG) {
    char     *args_msg;
    va_list   args;
    va_start(args, message);
    if(vasprintf(&args_msg, message.c_str(), args) > 0)
      all_log_line(LOG_LEVEL_DEBUG, tti, file, line, args_msg);
    va_end(args);
    free(args_msg);
  }
}



std::string log_filter::now_time()
{
  struct timeval rawtime;
  struct tm * timeinfo;
  char buffer[64];
  char us[16];
  
  gettimeofday(&rawtime, NULL);
  timeinfo = localtime(&rawtime.tv_sec);
  
  strftime(buffer,64,"%H:%M:%S",timeinfo);
  strcat(buffer,".");
  snprintf(us,16,"%06ld",rawtime.tv_usec);
  strcat(buffer,us);
  
  return std::string(buffer);
}

std::string log_filter::hex_string(uint8_t *hex, int size)
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
