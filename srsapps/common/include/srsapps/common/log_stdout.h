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
#include <stdarg.h>


#include "srsapps/common/log.h"

/******************************************************************************
 *  File:         log_stout.h
 *
 *  Description:  Logging service through standard output. Inherits log interface
 *
 *  Reference:
 *****************************************************************************/

#ifndef LOGSTDOUT_H
#define LOGSTDOUT_H

using namespace std; 

namespace srslte {
  
class log_stdout : public log
{
public:

  log_stdout(string service_name_) : log(service_name_) { }
  
  void error(string message, ...);
  void warning(string message, ...);
  void info(string message, ...);   
  void debug(string message, ...);  
  
  // Same with line and file info
  void error_line(string file, int line, string message, ...);  
  void warning_line(string file, int line, string message, ...);
  void info_line(string file, int line, string message, ...);   
  void debug_line(string file, int line, string message, ...);  

private:
  typedef enum {
    ERROR=0, WARNING, INFO, DEBUG, NOF_LEVELS
  } level_t;
  void printlog(level_t level, uint32_t tti, string file, int line, string message, va_list args);
  void printlog(level_t level, uint32_t tti, string message, va_list args);
};

}

#endif
  
