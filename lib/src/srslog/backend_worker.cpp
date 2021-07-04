/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "backend_worker.h"
#include "srsran/srslog/sink.h"

using namespace srslog;

void backend_worker::stop()
{
  // Signal the worker thread to stop.
  running_flag = false;
  if (worker_thread.joinable()) {
    worker_thread.join();
  }
}

void backend_worker::set_thread_priority(backend_priority priority) const
{
  switch (priority) {
    case backend_priority::normal:
      break;
    case backend_priority::high: {
      int min = ::sched_get_priority_min(SCHED_FIFO);
      if (min == -1) {
        err_handler("Unable to set the backend thread priority to high, falling back to normal priority.");
        return;
      }
      ::sched_param sch{min};
      if (::pthread_setschedparam(::pthread_self(), SCHED_FIFO, &sch)) {
        err_handler("Unable to set the backend thread priority to high, falling back to normal priority.");
        return;
      }
      break;
    }
    case backend_priority::very_high: {
      int max = ::sched_get_priority_max(SCHED_FIFO);
      int min = ::sched_get_priority_min(SCHED_FIFO);
      if (max == -1 || min == -1) {
        err_handler("Unable to set the backend thread priority to real time, falling back to normal priority.");
        return;
      }
      ::sched_param sch{min + ((max - min) / 2)};
      if (::pthread_setschedparam(::pthread_self(), SCHED_FIFO, &sch)) {
        err_handler("Unable to set the backend thread priority to real time, falling back to normal priority.");
        return;
      }
      break;
    }
  }
}

void backend_worker::create_worker(backend_priority priority)
{
  assert(!running_flag && "Only one worker thread should be created");

  std::thread t([this, priority]() {
    running_flag = true;
    set_thread_priority(priority);
    do_work();
  });

  worker_thread = std::move(t);

  // Block the caller thread until we are signaled that the worker is running.
  while (!running_flag) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void backend_worker::start(backend_priority priority)
{
  // Ensure we only create the worker thread once.
  std::call_once(start_once_flag, [this, priority]() { create_worker(priority); });
}

void backend_worker::do_work()
{
  /// This period defines the time the worker will sleep while waiting for new entries. This is required to check the
  /// termination variable periodically.
  constexpr std::chrono::microseconds sleep_period{100};

  while (running_flag) {
    auto item = queue.try_pop();

    // Spin while there are no new entries to process.
    if (!item.first) {
      std::this_thread::sleep_for(sleep_period);
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

  assert(entry.format_func && "Invalid format function");
  fmt_buffer.clear();

  // Save the pointer before moving the entry.
  auto* arg_store = entry.metadata.store;

  entry.format_func(std::move(entry.metadata), fmt_buffer);

  arg_pool.dealloc(arg_store);

  if (auto err_str = entry.s->write({fmt_buffer.data(), fmt_buffer.size()})) {
    err_handler(err_str.get_error());
  }
}

void backend_worker::process_outstanding_entries()
{
  assert(!running_flag && "Cannot process outstanding entries while thread is running");

  while (true) {
    auto item = queue.try_pop();

    // Check if the queue is empty.
    if (!item.first) {
      break;
    }

    process_log_entry(std::move(item.second));
  }
}
