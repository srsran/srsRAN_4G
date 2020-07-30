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

#include "srslte/common/logmap.h"
#include "srslte/common/log_filter.h"
#include "srslte/srslog/srslog.h"

using namespace srslte;

log_ref::log_ref(const char* name)
{
  ptr_ = srslte::logmap::get(name).ptr_;
}

/// Creates a log channel that writes to stdout.
static srslog::log_channel* create_or_get_default_logger()
{
  srslog::sink* s = srslog::create_stdout_sink();
  if (!s) {
    s = srslog::find_sink("stdout");
  }
  srslog::log_channel* log = srslog::create_log_channel("logmap_default", *s);
  if (!log) {
    log = srslog::find_log_channel("logmap_default");
  }

  srslog::init();

  return log;
}

logmap::logmap()
{
  stdout_channel = std::unique_ptr<srslog_wrapper>(new srslog_wrapper(*create_or_get_default_logger()));
  default_logger = stdout_channel.get();
}

// Access to log map by servicename. If servicename does not exist, create a new log_filter with default cfg
// Access to the map is protected by a mutex
log_ref SRSLTE_EXPORT logmap::get(std::string servicename)
{
  logmap* pool = get_instance();
  // strip trailing white spaces
  size_t last_char_pos = servicename.find_last_not_of(' ');
  if (last_char_pos != servicename.size() - 1) {
    servicename.erase(last_char_pos + 1, servicename.size());
  }
  return pool->get_impl(std::move(servicename));
}

// register manually created log
void SRSLTE_EXPORT logmap::register_log(std::unique_ptr<log> log_ptr)
{
  logmap*                     pool = get_instance();
  std::lock_guard<std::mutex> lock(pool->mutex);
  if (log_ptr != nullptr) {
    pool->log_map[log_ptr->get_service_name()] = std::move(log_ptr);
  }
}

std::unique_ptr<srslte::log> SRSLTE_EXPORT logmap::deregister_log(const std::string& servicename)
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
void SRSLTE_EXPORT logmap::set_default_logger(logger* logger_)
{
  logmap*                     pool = get_instance();
  std::lock_guard<std::mutex> lock(pool->mutex);
  pool->default_logger = logger_;
}

// set default log level
void SRSLTE_EXPORT logmap::set_default_log_level(LOG_LEVEL_ENUM l)
{
  logmap*                     pool = get_instance();
  std::lock_guard<std::mutex> lock(pool->mutex);
  pool->default_log_level = l;
}

// set default hex limit
void SRSLTE_EXPORT logmap::set_default_hex_limit(int hex_limit)
{
  logmap*                     pool = get_instance();
  std::lock_guard<std::mutex> lock(pool->mutex);
  pool->default_hex_limit = hex_limit;
}

log_ref logmap::get_impl(std::string servicename)
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
