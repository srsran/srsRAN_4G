/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
 * File:        logger.h
 * Description: Interface for logging output
 *****************************************************************************/

#ifndef SRSLTE_LOGGER_H
#define SRSLTE_LOGGER_H

#include "buffer_pool.h"
#include <memory>
#include <stdio.h>
#include <string>

namespace srslte {

class logger
{
public:
  logger() : pool(16 * 1024) {}

  class log_str
  {
  public:
    log_str(const char* msg_) { strncpy(msg, msg_, size); }
    log_str() { reset(); }
    void             reset() { msg[0] = '\0'; }
    const static int size = 512;
    char             msg[size];
  };

  typedef buffer_pool<log_str> log_str_pool_t;

  class log_str_deleter
  {
  public:
    explicit log_str_deleter(log_str_pool_t* pool_ = nullptr) : pool(pool_) {}
    void operator()(log_str* buf)
    {
      if (buf) {
        if (pool) {
          buf->reset();
          pool->deallocate(buf);
        } else
          delete buf;
      }
    }

  private:
    log_str_pool_t* pool;
  };
  typedef std::unique_ptr<log_str, log_str_deleter> unique_log_str_t;

  void log_char(const char* msg) { log(std::move(unique_log_str_t(new log_str(msg), log_str_deleter()))); }

  virtual void log(unique_log_str_t msg) = 0;

  log_str_pool_t& get_pool() { return pool; }

private:
  log_str_pool_t pool;
};

inline logger::unique_log_str_t allocate_unique_log_str(logger::log_str_pool_t& pool)
{
  return logger::unique_log_str_t(pool.allocate(), logger::log_str_deleter(&pool));
}

} // namespace srslte

#endif // SRSLTE_LOGGER_H
