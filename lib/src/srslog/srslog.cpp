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

#include "srslte/srslog/srslog.h"
#include "sinks/file_sink.h"
#include "sinks/stream_sink.h"
#include "srslog_instance.h"

using namespace srslog;

log_channel* srslog::create_log_channel(const std::string& id, sink& s)
{
  srslog_instance& instance = srslog_instance::get();

  auto channel = std::unique_ptr<log_channel>(
      new log_channel(id, s, instance.get_backend()));
  auto p = instance.get_channel_repo().insert(id, std::move(channel));

  return (p) ? p->get() : nullptr;
}

log_channel* srslog::find_log_channel(const std::string& id)
{
  auto p = srslog_instance::get().get_channel_repo().find(id);
  return (p) ? p->get() : nullptr;
}

sink* srslog::find_sink(const std::string& id)
{
  auto p = srslog_instance::get().get_sink_repo().find(id);
  return (p) ? p->get() : nullptr;
}

/// Helper for creating and registering sinks.
template <typename T, typename... Args>
static sink* create_sink(const std::string& id, Args&&... args)
{
  auto new_sink = std::unique_ptr<sink>(new T(std::forward<Args>(args)...));
  auto p =
      srslog_instance::get().get_sink_repo().insert(id, std::move(new_sink));

  return (p) ? p->get() : nullptr;
}

sink* srslog::create_stdout_sink(const std::string& name)
{
  return create_sink<stream_sink>(name, sink_stream_type::stdout);
}

sink* srslog::create_stderr_sink(const std::string& name)
{
  return create_sink<stream_sink>(name, sink_stream_type::stderr);
}

sink* srslog::create_file_sink(const std::string& path, size_t max_size)
{
  return create_sink<file_sink>(path, path, max_size);
}

void srslog::init()
{
  srslog_instance::get().get_backend().start();
}

detail::any* srslog::detail::create_logger(const std::string& id,
                                           detail::any&& logger)
{
  return srslog_instance::get().get_logger_repo().insert(id, std::move(logger));
}

detail::any* srslog::detail::find_logger(const std::string& id)
{
  return srslog_instance::get().get_logger_repo().find(id);
}

void srslog::set_error_handler(error_handler handler)
{
  srslog_instance::get().set_error_handler(std::move(handler));
}
