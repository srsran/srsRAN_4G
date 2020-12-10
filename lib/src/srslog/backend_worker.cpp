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

#include "backend_worker.h"
#include "srslte/srslog/sink.h"

using namespace srslog;

void backend_worker::stop()
{
  // Signal the worker thread to stop.
  running_flag = false;
  if (worker_thread.joinable()) {
    worker_thread.join();
  }
}

void backend_worker::create_worker()
{
  assert(!running_flag && "Only one worker thread should be created");

  std::thread t([this]() {
    running_flag = true;
    do_work();
  });

  worker_thread = std::move(t);

  // Block the caller thread until we are signaled that the worker is running.
  while (!running_flag) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void backend_worker::start()
{
  // Ensure we only create the worker thread once.
  std::call_once(start_once_flag, [this]() { create_worker(); });
}

void backend_worker::do_work()
{
  assert(running_flag && "Thread entry function called without running thread");

  while (running_flag) {
    auto item = queue.timed_pop(sleep_period_ms);

    // Spin again when the timeout expires.
    if (!item.first) {
      continue;
    }

    report_queue_on_full_once();

    process_log_entry(std::move(item.second));
  }

  // When we reach here, the thread is about to terminate, last chance to
  // process the last log entries.
  process_outstanding_entries();
}

/// Executes the flush command over all registered sinks.
static void process_flush_command(const detail::flush_backend_cmd& cmd)
{
  for (const auto sink : cmd.sinks) {
    sink->flush();
  }

  // Notify caller thread we are done.
  cmd.completion_flag = true;
}

void backend_worker::process_log_entry(detail::log_entry&& entry)
{
  // Check first for flush commands.
  if (entry.flush_cmd) {
    process_flush_command(*entry.flush_cmd);
    return;
  }

  fmt::memory_buffer fmt_buffer;

  assert(entry.format_func && "Invalid format function");
  entry.format_func(std::move(entry.metadata), fmt_buffer);

  const auto str = fmt::to_string(fmt_buffer);
  detail::memory_buffer buffer(str);
  if (auto err_str = entry.s->write(buffer)) {
    err_handler(err_str.get_error());
  }
}

void backend_worker::process_outstanding_entries()
{
  assert(!running_flag &&
         "Cannot process outstanding entries while thread is running");

  while (true) {
    auto item = queue.timed_pop(1);

    // Check if the queue is empty.
    if (!item.first) {
      break;
    }

    process_log_entry(std::move(item.second));
  }
}
