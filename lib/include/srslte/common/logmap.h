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

#include "logger.h"
#include <mutex>
#include <unordered_map>

#ifndef SRSLTE_LOGMAP_H
#define SRSLTE_LOGMAP_H

namespace srslte {

class log_filter;

class logmap
{
public:
  // Access to log map by servicename. If servicename does not exist, create a new log_filter with default cfg
  static log* get(const std::string& servicename)
  {
    logmap*                     pool = get_instance();
    std::lock_guard<std::mutex> lock(pool->mutex);
    auto                        it = pool->log_map.find(servicename);
    if (it == pool->log_map.end()) {
      // create a new logger with default cfg
      std::unique_ptr<log_filter> filter(new log_filter{servicename, pool->default_logger});
      filter->set_level(pool->default_log_level);
      filter->set_hex_limit(pool->default_hex_limit);
      auto                  ret      = pool->log_map.insert(std::make_pair(servicename, std::move(filter)));
      std::unique_ptr<log>& inserted = ret.first->second;
      return inserted.get();
    }
    return it->second.get();
  }

  static logmap* get_instance()
  {
    static logmap* instance = new logmap{};
    return instance;
  }

  // set default logger
  void set_default_logger(logger* logger_)
  {
    std::lock_guard<std::mutex> lock(mutex);
    default_logger = logger_;
  }

  // set default log level
  void set_default_log_level(LOG_LEVEL_ENUM l)
  {
    std::lock_guard<std::mutex> lock(mutex);
    default_log_level = l;
  }

  // set default hex limit
  void set_default_hex_limit(int hex_limit)
  {
    std::lock_guard<std::mutex> lock(mutex);
    default_hex_limit = hex_limit;
  }

  // register manually created log
  void register_log(std::unique_ptr<log> log_ptr)
  {
    std::lock_guard<std::mutex> lock(mutex);
    log_map[log_ptr->get_service_name()] = std::move(log_ptr);
  }

  bool deregister_log(const std::string& servicename)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return log_map.erase(servicename) > 0;
  }

private:
  logmap() : default_logger(&logger_stdout_val) {}

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
