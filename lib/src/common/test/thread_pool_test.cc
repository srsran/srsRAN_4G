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
#include "srsran/common/common.h"
#include "srsran/common/thread_pool.h"
#include "srsran/common/tti_sempahore.h"
#include "srsran/phy/utils/random.h"
#include "srsran/srslog/srslog.h"

class dummy_radio
{
private:
  static const int radio_delay_us = 200;

  srslog::basic_logger& logger;
  std::mutex            mutex;

  uint32_t last_tti = 0;
  bool     first    = true;
  bool     late     = false;

public:
  explicit dummy_radio(srslog::basic_logger& logger) : logger(logger) { logger.info("Dummy radio created"); }

  void tx(uint32_t tti)
  {
    std::lock_guard<std::mutex> lock(mutex);

    logger.info("Transmitting TTI %d", tti);

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

class dummy_worker : public srsran::thread_pool::worker
{
private:
  static const int sleep_time_min_us = 50;
  static const int sleep_time_max_us = 2000;

  srslog::basic_logger&            logger;
  srsran::tti_semaphore<uint32_t>* tti_semaphore = nullptr;
  dummy_radio*                     radio         = nullptr;
  srsran_random_t                  random_gen    = nullptr;
  uint32_t                         tti           = 0;

public:
  dummy_worker(uint32_t                         id,
               srsran::tti_semaphore<uint32_t>* tti_semaphore_,
               srslog::basic_logger&            logger,
               dummy_radio*                     radio_) :
    logger(logger)
  {
    tti_semaphore = tti_semaphore_;
    radio         = radio_;
    random_gen    = srsran_random_init(id);
    logger.info("Dummy worker created");
  }

  ~dummy_worker() { srsran_random_free(random_gen); }

  void set_tti(uint32_t tti_) { tti = tti_; }

protected:
  void work_imp() override
  {
    // Choose a random time to work
    int sleep_time_us = srsran_random_uniform_int_dist(random_gen, sleep_time_min_us, sleep_time_max_us);

    // Inform

    // Actual work ;)
    logger.info("Start working for %d us.", sleep_time_us);
    usleep(sleep_time_us);
    logger.info("Stopped working");

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
  int ret = SRSRAN_SUCCESS;

  // Simulation Constants
  const uint32_t  nof_workers        = FDD_HARQ_DELAY_UL_MS;
  const uint32_t  nof_tti            = 10240;
  const float     enable_probability = 0.9f;
  srsran_random_t random_gen         = srsran_random_init(1234);

  // Pools and workers
  srsran::thread_pool                         pool(nof_workers);
  std::vector<std::unique_ptr<dummy_worker> > workers;
  srsran::tti_semaphore<uint32_t>             tti_semaphore;

  // Loggers.
  auto& radio_logger = srslog::fetch_basic_logger("radio", false);
  radio_logger.set_level(srslog::basic_levels::none);

  // Start the log backend.
  srslog::init();

  // Radio
  dummy_radio radio(radio_logger);

  // Create workers
  for (uint32_t i = 0; i < nof_workers; i++) {
    // Create logging name
    char log_name[32] = {};
    snprintf(log_name, sizeof(log_name), "PHY%d", i);

    auto& logger = srslog::fetch_basic_logger(log_name, false);
    logger.set_level(srslog::basic_levels::none);

    // Create worker
    auto* worker = new dummy_worker(i, &tti_semaphore, logger, &radio);

    // Push back objects
    workers.push_back(std::unique_ptr<dummy_worker>(worker));

    // Init worker in pool
    pool.init_worker(i, worker);
  }

  for (uint32_t tti = 0; tti < nof_tti && !radio.is_late(); tti++) {
    if (enable_probability > srsran_random_uniform_real_dist(random_gen, 0.0f, 1.0f)) {
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
    ret = SRSRAN_ERROR;
  }

  tti_semaphore.wait_all();
  pool.stop();
  srsran_random_free(random_gen);

  return ret;
}
