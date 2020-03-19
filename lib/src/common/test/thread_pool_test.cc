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
#include <srslte/common/log_filter.h>
#include <srslte/common/thread_pool.h>
#include <srslte/common/tti_sempahore.h>
#include <srslte/phy/utils/random.h>

class dummy_radio
{
private:
  static const int radio_delay_us = 200;

  srslte::log_filter* log_h = nullptr;
  std::mutex          mutex;

  uint32_t last_tti = 0;
  bool     first    = true;
  bool     late     = false;

public:
  dummy_radio(srslte::log_filter& log_h_) : log_h(&log_h_) { log_h->info("Dummy radio created\n"); }

  void tx(uint32_t tti)
  {
    std::lock_guard<std::mutex> lock(mutex);

    log_h->info("Transmitting TTI %d\n", tti);

    // Exit if TTI was advanced
    if (!first && tti <= last_tti) {
      late = true;
    }

    // Save TTI
    last_tti = tti;
    first    = false;

    // Simulate radio delay
    usleep(radio_delay_us);
  }

  bool is_late() { return late; }
};

class dummy_worker : public srslte::thread_pool::worker
{
private:
  static const int sleep_time_min_us = 50;
  static const int sleep_time_max_us = 2000;

  srslte::tti_semaphore<uint32_t>* tti_semaphore = nullptr;
  srslte::log_filter*              log_h         = nullptr;
  dummy_radio*                     radio         = nullptr;
  srslte_random_t                  random_gen    = nullptr;
  uint32_t                         tti           = 0;

public:
  dummy_worker(uint32_t                         id,
               srslte::tti_semaphore<uint32_t>* tti_semaphore_,
               srslte::log_filter*              log_h_,
               dummy_radio*                     radio_)
  {
    tti_semaphore = tti_semaphore_;
    log_h         = log_h_;
    radio         = radio_;
    random_gen    = srslte_random_init(id);
    log_h->info("Dummy worker created\n");
  }

  ~dummy_worker() { srslte_random_free(random_gen); }

  void set_tti(uint32_t tti_) { tti = tti_; }

protected:
  void work_imp() override
  {
    // Choose a random time to work
    int sleep_time_us = srslte_random_uniform_int_dist(random_gen, sleep_time_min_us, sleep_time_max_us);

    // Inform

    // Actual work ;)
    log_h->info("Start working for %d us.\n", sleep_time_us);
    usleep(sleep_time_us);
    log_h->info("Stopped working\n");

    // Wait for green light
    tti_semaphore->wait(tti);

    // Simulate radio delay
    radio->tx(tti);

    // Release semaphore
    tti_semaphore->release();
  }
};

int main(int argc, char** argv)
{
  int ret = SRSLTE_SUCCESS;

  // Simulation Constants
  const uint32_t  nof_workers        = FDD_HARQ_DELAY_UL_MS;
  const uint32_t  nof_tti            = 10240;
  const float     enable_probability = 0.9f;
  srslte_random_t random_gen         = srslte_random_init(1234);

  // Pools and workers
  srslte::thread_pool                         pool(nof_workers);
  std::vector<std::unique_ptr<dummy_worker> > workers;
  srslte::tti_semaphore<uint32_t>             tti_semaphore;

  // Loggers
  srslte::logger_stdout                             logger;
  srslte::log_filter                                radio_log("radio", &logger);
  std::vector<std::unique_ptr<srslte::log_filter> > worker_logs;

  radio_log.set_level("none");

  // Radio
  dummy_radio radio(radio_log);

  // Create workers
  for (uint32_t i = 0; i < nof_workers; i++) {
    // Create logging name
    char log_name[32] = {};
    snprintf(log_name, sizeof(log_name), "PHY%d", i);

    // Create log filter
    srslte::log_filter* log_filter = new srslte::log_filter(log_name, &logger);
    log_filter->set_level("none");

    // Create worker
    auto* worker = new dummy_worker(i, &tti_semaphore, log_filter, &radio);

    // Push back objects
    worker_logs.push_back(std::unique_ptr<srslte::log_filter>(log_filter));
    workers.push_back(std::unique_ptr<dummy_worker>(worker));

    // Init worker in pool
    pool.init_worker(i, worker);
  }

  for (uint32_t tti = 0; tti < nof_tti && !radio.is_late(); tti++) {
    if (enable_probability > srslte_random_uniform_real_dist(random_gen, 0.0f, 1.0f)) {
      // Wait worker
      auto worker = (dummy_worker*)pool.wait_worker(tti);

      // Set tti
      worker->set_tti(tti);

      // Launch
      tti_semaphore.push(tti);
      pool.start_worker(worker);
    }
  }

  if (radio.is_late()) {
    ret = SRSLTE_ERROR;
  }

  tti_semaphore.wait_all();
  pool.stop();
  srslte_random_free(random_gen);

  return ret;
}
