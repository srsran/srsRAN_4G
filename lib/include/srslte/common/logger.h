/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  const static uint32_t preallocated_log_str_size = 1024;

  logger() : pool(16 * 1024) {}
  virtual ~logger() = default;

  class log_str
  {
  public:
    log_str(const char* msg_ = nullptr, uint32_t size_ = 0)
    {
      size = size_ ? size_ : preallocated_log_str_size;
      msg  = new char[size];
      if (msg_) {
        strncpy(msg, msg_, size);
      } else {
        msg[0] = '\0';
      }
    }
    log_str(const log_str&) = delete;
    log_str& operator=(const log_str&) = delete;
    ~log_str() { delete[] msg; }
    void     reset() { msg[0] = '\0'; }
    char*    str() { return msg; }
    uint32_t get_buffer_size() { return size; }
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN] = {};
#endif

  private:
    uint32_t size;
    char*    msg;
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
        } else {
          delete buf;
        }
      }
    }

  private:
    log_str_pool_t* pool;
  };
  typedef std::unique_ptr<log_str, log_str_deleter> unique_log_str_t;

  void log_char(const char* msg) { log(unique_log_str_t(new log_str(msg), log_str_deleter())); }

  virtual void log(unique_log_str_t msg) = 0;

  log_str_pool_t&  get_pool() { return pool; }
  unique_log_str_t allocate_unique_log_str()
  {
    return unique_log_str_t(pool.allocate(), logger::log_str_deleter(&pool));
  }

private:
  log_str_pool_t pool;
};

} // namespace srslte

#endif // SRSLTE_LOGGER_H
