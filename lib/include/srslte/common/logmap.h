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

#include "log_filter.h"
#include "logger_stdout.h"
#include "singleton.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/logger_stdout.h"
#include <mutex>
#include <unordered_map>

#ifndef SRSLTE_LOGMAP_H
#define SRSLTE_LOGMAP_H

namespace srslte {

class log_filter;

class log_ref
{
  using ptr_type = std::unique_ptr<log>*;

public:
  log_ref() = default;
  explicit log_ref(ptr_type p) : ptr_(p) {}

  // works like a log*
  log* operator->() { return ptr_->get(); }
  log* operator->() const { return ptr_->get(); }

  // in case we want to obtain log*
  log* operator*() { return ptr_->get(); }
  log* get() { return ptr_->get(); }

  // identity defined by ref address
  bool operator==(const log_ref& l) { return ptr_ == l.ptr_; }

  // to do checks like if(log_ref)
  operator bool() { return ptr_ != nullptr; }

private:
  ptr_type ptr_ = nullptr;
};

class logmap : public singleton_t<logmap>
{
  static const size_t servicename_minimum_length = 4;
  using it_t                                     = std::unordered_map<std::string, std::unique_ptr<log> >::iterator;

public:
  // Access to log map by servicename. If servicename does not exist, create a new log_filter with default cfg
  // Access to the map is protected by a mutex
  static log_ref get(std::string servicename)
  {
    logmap* pool = get_instance();
    if (servicename.size() < servicename_minimum_length) {
      // pad right of string with spaces
      servicename.insert(servicename.end(), servicename_minimum_length - servicename.length(), ' ');
    }
    return pool->get_impl(std::move(servicename));
  }

  // register manually created log
  static void register_log(std::unique_ptr<log> log_ptr)
  {
    logmap*                     pool = get_instance();
    std::lock_guard<std::mutex> lock(pool->mutex);
    if (log_ptr != nullptr) {
      pool->log_map[log_ptr->get_service_name()] = std::move(log_ptr);
    }
  }

  static std::unique_ptr<srslte::log> deregister_log(const std::string& servicename)
  {
    logmap*                      pool = get_instance();
    std::unique_ptr<srslte::log> ret;
    std::lock_guard<std::mutex>  lock(pool->mutex);
    auto                         it = pool->log_map.find(servicename);
    if (it != pool->log_map.end()) {
      ret = std::move(it->second);
      pool->log_map.erase(it);
    }
    return ret;
  }

  // set default logger
  static void set_default_logger(logger* logger_)
  {
    logmap*                     pool = get_instance();
    std::lock_guard<std::mutex> lock(pool->mutex);
    pool->default_logger = logger_;
  }

  // set default log level
  static void set_default_log_level(LOG_LEVEL_ENUM l)
  {
    logmap*                     pool = get_instance();
    std::lock_guard<std::mutex> lock(pool->mutex);
    pool->default_log_level = l;
  }

  // set default hex limit
  static void set_default_hex_limit(int hex_limit)
  {
    logmap*                     pool = get_instance();
    std::lock_guard<std::mutex> lock(pool->mutex);
    pool->default_hex_limit = hex_limit;
  }

protected:
  logmap() : default_logger(&logger_stdout_val) {}

private:
  log_ref get_impl(std::string servicename)
  {
    std::lock_guard<std::mutex> lock(mutex);
    auto                        it = log_map.find(servicename);
    if (it == log_map.end()) {
      // create a new logger with default cfg
      std::unique_ptr<log_filter> filter(new log_filter{std::move(servicename), default_logger});
      filter->set_level(default_log_level);
      filter->set_hex_limit(default_hex_limit);
      auto ret = log_map.insert(std::make_pair(filter->get_service_name(), std::move(filter)));
      return log_ref{&ret.first->second};
    }
    return log_ref{&it->second};
  }

  // consts
  logger_stdout logger_stdout_val;

  // default cfg
  logger*                default_logger    = nullptr;
  srslte::LOG_LEVEL_ENUM default_log_level = LOG_LEVEL_WARNING;
  int                    default_hex_limit = 1024;

  // state
  std::mutex                                             mutex;
  std::unordered_map<std::string, std::unique_ptr<log> > log_map;
};

} // namespace srslte

#endif // SRSLTE_LOGMAP_H
