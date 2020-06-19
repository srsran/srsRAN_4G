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

#ifndef SRSENB_PRACH_WORKER_H
#define SRSENB_PRACH_WORKER_H

#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/enb_interfaces.h"

// Setting ENABLE_PRACH_GUI to non zero enables a GUI showing signal received in the PRACH window.
#define ENABLE_PRACH_GUI 0

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
#include <srsgui/srsgui.h>
#endif // defined(ENABLE_GUI) and ENABLE_PRACH_GUI

namespace srsenb {

class prach_worker : srslte::thread
{
public:
  prach_worker(uint32_t cc_idx_) : buffer_pool(8), thread("PRACH_WORKER") { cc_idx = cc_idx_; }

  int  init(const srslte_cell_t&      cell_,
            const srslte_prach_cfg_t& prach_cfg_,
            stack_interface_phy_lte*  mac,
            srslte::log*              log_h,
            int                       priority);
  int  new_tti(uint32_t tti, cf_t* buffer);
  void set_max_prach_offset_us(float delay_us);
  void stop();

private:
  uint32_t cc_idx             = 0;
  uint32_t prach_nof_det      = 0;
  uint32_t prach_indices[165] = {};
  float    prach_offsets[165] = {};
  float    prach_p2avg[165]   = {};

  srslte_cell_t      cell      = {};
  srslte_prach_cfg_t prach_cfg = {};
  srslte_prach_t     prach     = {};

#if defined(ENABLE_GUI) and ENABLE_PRACH_GUI
  plot_real_t                              plot_real;
  std::array<float, 3 * SRSLTE_SF_LEN_MAX> plot_buffer;
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
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN];
#endif /* SRSLTE_BUFFER_POOL_LOG_ENABLED */
  };
  srslte::buffer_pool<sf_buffer>  buffer_pool;
  srslte::block_queue<sf_buffer*> pending_buffers;

  sf_buffer*               current_buffer      = nullptr;
  srslte::log*             log_h               = nullptr;
  stack_interface_phy_lte* stack               = nullptr;
  float                    max_prach_offset_us = 0.0f;
  bool                     initiated           = false;
  bool                     running             = false;
  uint32_t                 nof_sf              = 0;
  uint32_t                 sf_cnt              = 0;

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
            const srslte_cell_t&      cell_,
            const srslte_prach_cfg_t& prach_cfg_,
            stack_interface_phy_lte*  mac,
            srslte::log*              log_h,
            int                       priority)
  {
    // Create PRACH worker if required
    while (cc_idx >= prach_vec.size()) {
      prach_vec.push_back(std::unique_ptr<prach_worker>(new prach_worker(prach_vec.size())));
    }

    prach_vec[cc_idx]->init(cell_, prach_cfg_, mac, log_h, priority);
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
    int ret = SRSLTE_ERROR;
    if (cc_idx < prach_vec.size()) {
      ret = prach_vec[cc_idx]->new_tti(tti, buffer);
    }
    return ret;
  }
};
} // namespace srsenb
#endif // SRSENB_PRACH_WORKER_H
