/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSENB_PRACH_WORKER_H
#define SRSENB_PRACH_WORKER_H

#include "srsran/common/block_queue.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include "srsran/srslog/srslog.h"
#include <atomic>

// Setting ENABLE_PRACH_GUI to non zero enables a GUI showing signal received in the PRACH window.
#define ENABLE_PRACH_GUI 0

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
#include <srsgui/srsgui.h>
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI

namespace srsenb {

class stack_interface_phy_lte;

class prach_worker : srsran::thread
{
public:
  prach_worker(uint32_t cc_idx_, srslog::basic_logger& logger) :
    buffer_pool(8), thread("PRACH_WORKER"), logger(logger), running(false)
  {
    cc_idx = cc_idx_;
  }

  int  init(const srsran_cell_t&      cell_,
            const srsran_prach_cfg_t& prach_cfg_,
            stack_interface_phy_lte*  mac,
            int                       priority,
            uint32_t                  nof_workers);
  int  new_tti(uint32_t tti, cf_t* buffer);
  void set_max_prach_offset_us(float delay_us);
  void stop();

private:
  uint32_t cc_idx = 0;

  uint32_t prach_indices[165] = {};
  float    prach_offsets[165] = {};
  float    prach_p2avg[165]   = {};

  srsran_cell_t      cell      = {};
  srsran_prach_cfg_t prach_cfg = {};
  srsran_prach_t     prach     = {};

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
  plot_real_t                              plot_real;
  std::array<float, 3 * SRSRAN_SF_LEN_MAX> plot_buffer;
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI

  const static int sf_buffer_sz = 128 * 1024;
  class sf_buffer
  {
  public:
    sf_buffer() = default;
    void reset()
    {
      nof_samples = 0;
      tti         = 0;
    }
    cf_t     samples[sf_buffer_sz] = {};
    uint32_t nof_samples           = 0;
    uint32_t tti                   = 0;
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
    char debug_name[SRSRAN_BUFFER_POOL_LOG_NAME_LEN];
#endif /* SRSRAN_BUFFER_POOL_LOG_ENABLED */
  };
  srsran::buffer_pool<sf_buffer>  buffer_pool;
  srsran::block_queue<sf_buffer*> pending_buffers;

  srslog::basic_logger&    logger;
  sf_buffer*               current_buffer      = nullptr;
  stack_interface_phy_lte* stack               = nullptr;
  float                    max_prach_offset_us = 0.0f;
  bool                     initiated           = false;
  std::atomic<bool>        running;
  uint32_t                 nof_sf      = 0;
  uint32_t                 sf_cnt      = 0;
  uint32_t                 nof_workers = 0;

  void run_thread() final;
  int  run_tti(sf_buffer* b);
};

class prach_worker_pool
{
private:
  std::vector<std::unique_ptr<prach_worker> > prach_vec;

public:
  prach_worker_pool()  = default;
  ~prach_worker_pool() = default;

  void init(uint32_t                  cc_idx,
            const srsran_cell_t&      cell_,
            const srsran_prach_cfg_t& prach_cfg_,
            stack_interface_phy_lte*  mac,
            srslog::basic_logger&     logger,
            int                       priority,
            uint32_t                  nof_workers_x_cc)
  {
    // Create PRACH worker if required
    while (cc_idx >= prach_vec.size()) {
      prach_vec.push_back(std::unique_ptr<prach_worker>(new prach_worker(prach_vec.size(), logger)));
    }

    prach_vec[cc_idx]->init(cell_, prach_cfg_, mac, priority, nof_workers_x_cc);
  }

  void set_max_prach_offset_us(float delay_us)
  {
    for (auto& prach : prach_vec) {
      prach->set_max_prach_offset_us(delay_us);
    }
  }

  void stop()
  {
    for (auto& prach : prach_vec) {
      prach->stop();
    }
  }

  int new_tti(uint32_t cc_idx, uint32_t tti, cf_t* buffer)
  {
    int ret = SRSRAN_ERROR;
    if (cc_idx < prach_vec.size()) {
      ret = prach_vec[cc_idx]->new_tti(tti, buffer);
    }
    return ret;
  }
};
} // namespace srsenb
#endif // SRSENB_PRACH_WORKER_H
