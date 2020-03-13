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
 * File:        logger_file.h
 * Description: Common log object. Maintains a queue of log messages
 *              and runs a thread to read messages and write to file.
 *              Multiple producers, single consumer. If full, producers
 *              increase queue size. If empty, consumer blocks.
 *****************************************************************************/

#ifndef SRSLTE_LOGGER_FILE_H
#define SRSLTE_LOGGER_FILE_H

#include "srslte/common/logger.h"
#include "srslte/common/threads.h"
#include <deque>
#include <stdio.h>
#include <string>

namespace srslte {

typedef std::string* str_ptr;

class logger_file : public thread, public logger
{
public:
  logger_file();
  logger_file(std::string file);
  ~logger_file();
  void init(std::string file, int max_length = -1);
  void stop();
  // Implementation of log_out
  void log(unique_log_str_t msg);

private:
  void run_thread();
  void flush();

  uint32_t        name_idx;
  int64_t         max_length;
  int64_t         cur_length;
  FILE*           logfile;
  bool            is_running;
  std::string     filename;
  pthread_cond_t  not_empty;
  pthread_mutex_t mutex;

  std::deque<unique_log_str_t> buffer;
};

} // namespace srslte

#endif // SRSLTE_LOGGER_FILE_H
