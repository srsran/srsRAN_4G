/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"

namespace srsenb {

class prach_worker : thread
{
public:
  prach_worker() :
    initiated(false),
    prach_nof_det(0),
    max_prach_offset_us(0),
    buffer_pool(8),
    running(false),
    nof_sf(0),
    sf_cnt(0),
    thread("PRACH_WORKER")
  {
    log_h = NULL;
    stack = NULL;
    bzero(&prach, sizeof(srslte_prach_t));
    bzero(&prach_indices, sizeof(prach_indices));
    bzero(&prach_offsets, sizeof(prach_offsets));
    bzero(&prach_p2avg, sizeof(prach_p2avg));
    bzero(&cell, sizeof(cell));
    bzero(&prach_cfg, sizeof(prach_cfg));
  }

  int  init(const srslte_cell_t&      cell_,
            const srslte_prach_cfg_t& prach_cfg_,
            stack_interface_phy_lte*  mac,
            srslte::log*              log_h,
            int                       priority);
  int  new_tti(uint32_t tti, cf_t *buffer);
  void set_max_prach_offset_us(float delay_us);
  void stop();
  
private:
  uint32_t prach_nof_det;
  uint32_t prach_indices[165]; 
  float    prach_offsets[165]; 
  float    prach_p2avg[165];
 
  srslte_cell_t cell; 
  srslte_prach_cfg_t prach_cfg;
  srslte_prach_t  prach;

  const static int sf_buffer_sz = 128*1024;
  class sf_buffer {
  public:
    sf_buffer()
    {
      nof_samples = 0;
      tti         = 0;
    }
    void reset()
    {
      nof_samples = 0;
      tti         = 0;
    }
    cf_t     samples[sf_buffer_sz];
    uint32_t nof_samples;
    uint32_t tti;
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    char debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN];
#endif /* SRSLTE_BUFFER_POOL_LOG_ENABLED */
  };
  srslte::buffer_pool<sf_buffer>  buffer_pool;
  srslte::block_queue<sf_buffer*> pending_buffers;
  sf_buffer* current_buffer;

  srslte::log* log_h;
  stack_interface_phy_lte* stack;
  float max_prach_offset_us;
  bool initiated;
  bool running;
  uint32_t nof_sf;
  uint32_t sf_cnt;

  void run_thread();
  int run_tti(sf_buffer *b);


};
}
#endif // SRSENB_PRACH_WORKER_H
