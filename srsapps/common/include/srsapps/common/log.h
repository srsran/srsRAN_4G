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

#define Error(fmt, ...)   log_h->error(tti, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(tti, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(tti, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(tti, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

using namespace std; 

namespace srslte {
  
class log
{
public:

  log(string service_name_) { service_name = service_name_; }
  
  // Pure virtual methods for logging
  virtual void error(uint32_t tti, string message, ...)   = 0;
  virtual void warning(uint32_t tti, string message, ...) = 0;
  virtual void info(uint32_t tti, string message, ...)    = 0;
  virtual void debug(uint32_t tti, string message, ...)   = 0;
  
  // Same with line and file info
  virtual void error(uint32_t tti, string file, int line, string message, ...)   = 0;
  virtual void warning(uint32_t tti, string file, int line, string message, ...) = 0;
  virtual void info(uint32_t tti, string file, int line, string message, ...)    = 0;
  virtual void debug(uint32_t tti, string file, int line, string message, ...)   = 0;
  
protected: 
  string get_service_name() { return service_name; }

private:
  string service_name; 
};

}

#endif
  
