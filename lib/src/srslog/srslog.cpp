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
#include "srslog_instance.h"

using namespace srslog;

/// Returns a copy of the input string with any occurrences of the '#' character
/// removed.
static std::string remove_sharp_chars(const std::string& s)
{
  std::string result(s);
  result.erase(std::remove(result.begin(), result.end(), '#'), result.end());
  return result;
}

/// Generic argument function that fetches a log channel from the repository.
template <typename... Args>
static log_channel& fetch_log_channel_helper(const std::string& id, Args&&... args)
{
  return srslog_instance::get().get_channel_repo().emplace(
      std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id, std::forward<Args>(args)...));
}

///
/// Log channel management function implementations.
///

log_channel* srslog::find_log_channel(const std::string& id)
{
  return srslog_instance::get().get_channel_repo().find(id);
}

log_channel& srslog::fetch_log_channel(const std::string& id)
{
  assert(!id.empty() && "Empty id string");

  std::string      clean_id = remove_sharp_chars(id);
  srslog_instance& instance = srslog_instance::get();
  return fetch_log_channel_helper(clean_id, instance.get_default_sink(), instance.get_backend());
}

log_channel& srslog::fetch_log_channel(const std::string& id, sink& s, log_channel_config config)
{
  assert(!id.empty() && "Empty id string");

  std::string      clean_id = remove_sharp_chars(id);
  srslog_instance& instance = srslog_instance::get();
  return fetch_log_channel_helper(clean_id, s, instance.get_backend(), std::move(config));
}

///
/// Sink management function implementations.
///

void srslog::set_default_sink(sink& s)
{
  srslog_instance::get().set_default_sink(s);
}

sink& srslog::get_default_sink()
{
  return srslog_instance::get().get_default_sink();
}

sink* srslog::find_sink(const std::string& id)
{
  return srslog_instance::get().get_sink_repo().find(id);
}

sink& srslog::fetch_stdout_sink()
{
  return srslog_instance::get().get_sink_repo().get_stdout_sink();
}

sink& srslog::fetch_stderr_sink()
{
  return srslog_instance::get().get_sink_repo().get_stderr_sink();
}

sink& srslog::fetch_file_sink(const std::string& path, size_t max_size)
{
  assert(!path.empty() && "Empty path string");

  std::string clean_path = remove_sharp_chars(path);
  //:TODO: GCC5 or lower versions emits an error if we use the new() expression directly, use redundant
  // piecewise_construct instead.
  return srslog_instance::get().get_sink_repo().fetch_sink(std::piecewise_construct,
                                                           std::forward_as_tuple(clean_path),
                                                           std::forward_as_tuple(new file_sink(clean_path, max_size)));
}

///
/// Framework configuration and control function implementations.
///

void srslog::init()
{
  srslog_instance::get().get_backend().start();
}

void srslog::flush()
{
  srslog_instance& instance = srslog_instance::get();

  // Nothing to do when the backend is not running yet.
  if (!instance.get_backend().is_running()) {
    return;
  }

  // The backend will set this shared variable when done.
  detail::shared_variable<bool> completion_flag(false);

  detail::log_entry cmd;
  cmd.flush_cmd = std::unique_ptr<detail::flush_backend_cmd>(
      new detail::flush_backend_cmd{completion_flag, instance.get_sink_repo().contents()});

  instance.get_backend().push(std::move(cmd));

  // Block the caller thread until we are signaled that the flush is completed.
  while (!completion_flag) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }
}

void srslog::set_error_handler(error_handler handler)
{
  srslog_instance::get().set_error_handler(std::move(handler));
}

///
/// Logger management function implementations.
///

detail::any* srslog::detail::find_logger(const std::string& id)
{
  return srslog_instance::get().get_logger_repo().find(id);
}

detail::any* srslog::detail::fetch_logger(const std::string& id, detail::any&& logger)
{
  assert(!id.empty() && "Empty id string");
  return &srslog_instance::get().get_logger_repo().emplace(id, std::move(logger));
}

/// Builds a logger name out of the id and tag.
static std::string build_logger_name(const std::string& id, char tag)
{
  return fmt::format("{}#{}", id, tag);
}

/// Fetches a logger with all its log channels.
static basic_logger& fetch_basic_logger_helper(const std::string& id, sink& s, bool should_print_context)
{
  static constexpr char basic_logger_chan_tags[] = {'E', 'W', 'I', 'D'};

  srslog_instance& instance = srslog_instance::get();

  // User created log channels cannot have ids with a # character, encode the
  // ids here with a # to ensure all channels are unique.

  log_channel& error =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[0]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[0], should_print_context});
  log_channel& warning =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[1]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[1], should_print_context});
  log_channel& info = fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[2]),
                                               s,
                                               instance.get_backend(),
                                               log_channel_config{id, basic_logger_chan_tags[2], should_print_context});
  log_channel& debug =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[3]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[3], should_print_context});

  return fetch_logger<basic_logger>(id, error, warning, info, debug);
}

basic_logger& srslog::fetch_basic_logger(const std::string& id, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");
  return fetch_basic_logger_helper(id, srslog_instance::get().get_default_sink(), should_print_context);
}

basic_logger& srslog::fetch_basic_logger(const std::string& id, sink& s, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");
  return fetch_basic_logger_helper(id, s, should_print_context);
}

///
/// Deprecated functions to be removed.
///

/// Creates and registers a log channel. Returns a pointer to the newly created
/// channel on success, otherwise nullptr.
static log_channel* create_and_register_log_channel(const std::string& id, sink& s)
{
  assert(!id.empty() && "Empty id string");

  srslog_instance& instance = srslog_instance::get();

  auto& p = instance.get_channel_repo().emplace(
      std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id, s, instance.get_backend()));

  return &p;
}

static log_channel* create_and_register_log_channel(const std::string& id, log_channel_config config, sink& s)
{
  assert(!id.empty() && "Empty id string");

  srslog_instance& instance = srslog_instance::get();

  auto& p =
      instance.get_channel_repo().emplace(std::piecewise_construct,
                                          std::forward_as_tuple(id),
                                          std::forward_as_tuple(id, s, instance.get_backend(), std::move(config)));

  return &p;
}

/// Returns true if the input string contains a sharp character, otherwise
/// returns false.
static bool contains_sharp_char(const std::string& s)
{
  return s.find('#') != std::string::npos;
}

log_channel* srslog::create_log_channel(const std::string& id, sink& s)
{
  if (contains_sharp_char(id)) {
    return nullptr;
  }

  return create_and_register_log_channel(id, s);
}

sink* srslog::create_stdout_sink(const std::string& name)
{
  return &srslog_instance::get().get_sink_repo().get_stdout_sink();
}

sink* srslog::create_stderr_sink(const std::string& name)
{
  return &srslog_instance::get().get_sink_repo().get_stderr_sink();
}

sink* srslog::create_file_sink(const std::string& path, size_t max_size)
{
  //:TODO: GCC5 or lower versions emits an error if we use the new() expression directly, use redundant
  // piecewise_construct instead.
  return &srslog_instance::get().get_sink_repo().fetch_sink(
      std::piecewise_construct, std::forward_as_tuple(path), std::forward_as_tuple(new file_sink(path, max_size)));
}

basic_logger* srslog::create_basic_logger(const std::string& id, sink& s, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");

  static constexpr char basic_logger_chan_tags[] = {'E', 'W', 'I', 'D'};

  auto& logger_repo = srslog_instance::get().get_logger_repo();

  // Nothing to do when the logger already exists.
  if (logger_repo.find(id)) {
    return nullptr;
  }

  // User created log channels cannot have ids with a # character, encode the
  // ids here with a # to ensure all channel creations will be successful
  // without any id clashes.

  log_channel* error = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[0]), {id, basic_logger_chan_tags[0], should_print_context}, s);
  assert(error && "Could not create channel");
  log_channel* warning = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[1]), {id, basic_logger_chan_tags[1], should_print_context}, s);
  assert(warning && "Could not create channel");
  log_channel* info = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[2]), {id, basic_logger_chan_tags[2], should_print_context}, s);
  assert(info && "Could not create channel");
  log_channel* debug = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[3]), {id, basic_logger_chan_tags[3], should_print_context}, s);
  assert(debug && "Could not create channel");

  return create_logger<basic_logger>(id, *error, *warning, *info, *debug);
}
