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


#include <stdint.h>
#include <string>

/******************************************************************************
 *  File:         log.h
 *
 *  Description:  Abstract logging service
 *
 *  Reference:
 *****************************************************************************/

#ifndef LOG_H
#define LOG_H

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srslte {
  
class log
{
public:

  log(std::string service_name_) { service_name = service_name_; tti = 0; level = LOG_LEVEL_NONE; }
  
  // This function shall be called at the start of every tti for printing tti 
  void step(uint32_t tti_) {
    tti = tti_; 
  }
  
  typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_INFO, 
    LOG_LEVEL_DEBUG
  } log_level_t; 
  
  void set_level_info() {
    level = LOG_LEVEL_INFO; 
  }
  void set_level_debug() {
    level = LOG_LEVEL_DEBUG; 
  }
  
  // Pure virtual methods for logging
  virtual void error(std::string message, ...)   = 0;
  virtual void warning(std::string message, ...) = 0;
  virtual void info(std::string message, ...)    = 0;
  virtual void debug(std::string message, ...)   = 0;
  
  // Same with line and file info
//  virtual void error(std::string file, int line, std::string message, ...)   = 0;
//  virtual void warning(std::string file, int line, std::string message, ...) = 0;
//  virtual void info(std::string file, int line, std::string message, ...)    = 0;
//  virtual void debug(std::string file, int line, std::string message, ...)   = 0;
  
protected: 
  std::string get_service_name() { return service_name; }
  uint32_t tti; 
  log_level_t level; 
private:
  std::string service_name;
};

}

#endif
  
