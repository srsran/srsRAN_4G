/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/srslog/srslog.h"
#include <atomic>
#include <sys/resource.h>
#include <thread>

using namespace srslog;

static constexpr unsigned num_iterations       = 4000;
static constexpr unsigned num_entries_per_iter = 40;

namespace {

/// This helper class checks if there has been context switches between its construction and destruction for the caller
/// thread.
class context_switch_checker
{
public:
  explicit context_switch_checker(std::atomic<unsigned>& counter) : counter(counter)
  {
    ::getrusage(RUSAGE_THREAD, &before);
  }

  ~context_switch_checker()
  {
    ::rusage after{};
    ::getrusage(RUSAGE_THREAD, &after);
    unsigned diff = (after.ru_nvcsw - before.ru_nvcsw) + (after.ru_nivcsw - before.ru_nivcsw);
    if (diff) {
      counter.fetch_add(diff, std::memory_order_relaxed);
    }
  }

private:
  ::rusage               before{};
  std::atomic<unsigned>& counter;
};

} // namespace

/// Busy waits in the calling thread for the specified amount of time.
static void busy_wait(std::chrono::milliseconds interval)
{
  auto begin = std::chrono::steady_clock::now();
  auto end   = begin + interval;

  while (std::chrono::steady_clock::now() < end) {
  }
}

/// Worker function used for each thread of the benchmark to generate and measure the time taken for each log entry.
static void run_thread(log_channel& c, std::vector<uint64_t>& results, std::atomic<unsigned>& ctx_counter)
{
  for (unsigned iter = 0; iter != num_iterations; ++iter) {
    context_switch_checker ctx_checker(ctx_counter);

    auto begin = std::chrono::steady_clock::now();
    for (unsigned entry_num = 0; entry_num != num_entries_per_iter; ++entry_num) {
      double d = entry_num;
      c("SRSLOG latency benchmark: int: %u, double: %f, string: %s", iter, d, "test");
    }
    auto end = std::chrono::steady_clock::now();

    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / num_entries_per_iter);

    busy_wait(std::chrono::milliseconds(4));
  }
}

/// This function runs the latency benchmark generating log entries using the specified number of threads.
static void benchmark(unsigned num_threads)
{
  std::vector<std::vector<uint64_t> > thread_results;
  thread_results.resize(num_threads);
  for (auto& v : thread_results) {
    v.reserve(num_iterations);
  }

  auto& s       = srslog::fetch_file_sink("srslog_latency_benchmark.txt");
  auto& channel = srslog::fetch_log_channel("bench", s, {});

  srslog::init();

  std::vector<std::thread> workers;
  workers.reserve(num_threads);

  std::atomic<unsigned> ctx_counter(0);
  for (unsigned i = 0; i != num_threads; ++i) {
    workers.emplace_back(run_thread, std::ref(channel), std::ref(thread_results[i]), std::ref(ctx_counter));
  }
  for (auto& w : workers) {
    w.join();
  }

  std::vector<uint64_t> results;
  results.reserve(num_threads * num_iterations);
  for (const auto& v : thread_results) {
    results.insert(results.end(), v.begin(), v.end());
  }
  std::sort(results.begin(), results.end());

  fmt::print("SRSLOG Frontend Latency Benchmark - logging with {} thread{}\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n"
             "Context switches: {} in {} of generated entries\n\n",
             num_threads,
             (num_threads > 1) ? "s" : "",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back(),
             ctx_counter,
             num_threads * num_iterations * num_entries_per_iter);
}

int main()
{
  for (auto n : {1, 2, 4}) {
    benchmark(n);
  }

  return 0;
}
