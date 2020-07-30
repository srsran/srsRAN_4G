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

#ifndef SRSLTE_LOGMAP_H
#define SRSLTE_LOGMAP_H

#include "srslte/common/log.h"
#include "srslte/common/logger.h"
#include "srslte/common/logger_srslog_wrapper.h"
#include "srslte/common/singleton.h"

#include <memory>
#include <mutex>
#include <unordered_map>

namespace srslte {

class log_ref
{
  using ptr_type = std::unique_ptr<log>*;

public:
  log_ref() = default;
  explicit log_ref(ptr_type p) : ptr_(p) {}
  explicit log_ref(const char* name);

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
public:
  // Access to log map by servicename. If servicename does not exist, create a new log_filter with default cfg
  // Access to the map is protected by a mutex
  static log_ref get(std::string servicename);

  // register manually created log
  static void register_log(std::unique_ptr<log> log_ptr);

  static std::unique_ptr<srslte::log> deregister_log(const std::string& servicename);

  // set default logger
  static void set_default_logger(logger* logger_);

  // set default log level
  static void set_default_log_level(LOG_LEVEL_ENUM l);

  // set default hex limit
  static void set_default_hex_limit(int hex_limit);

protected:
  logmap();

private:
  log_ref get_impl(std::string servicename);

  // default cfg
  std::unique_ptr<srslog_wrapper> stdout_channel;
  logger*                         default_logger    = nullptr;
  srslte::LOG_LEVEL_ENUM          default_log_level = LOG_LEVEL_WARNING;
  int                             default_hex_limit = 1024;

  // state
  std::mutex                                             mutex;
  std::unordered_map<std::string, std::unique_ptr<log> > log_map;
};

} // namespace srslte

#endif // SRSLTE_LOGMAP_H
