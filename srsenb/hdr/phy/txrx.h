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

#ifndef SRSENB_TXRX_H
#define SRSENB_TXRX_H

#include "phy_common.h"
#include "prach_worker.h"
#include "srslte/common/log.h"
#include "srslte/common/thread_pool.h"
#include "srslte/common/threads.h"
#include "srslte/radio/radio.h"

namespace srsenb {
    
typedef _Complex float cf_t; 

class txrx : public thread
{
public:
  txrx();
  bool init(srslte::radio_interface_phy* radio_handler,
            srslte::thread_pool*         _workers_pool,
            phy_common*                  worker_com,
            prach_worker*                prach,
            srslte::log*                 log_h,
            uint32_t                     prio);
  void stop();
    
private:
  void run_thread();

  srslte::radio_interface_phy* radio_h;
  srslte::log          *log_h;
  srslte::thread_pool  *workers_pool;
  prach_worker*         prach;
  phy_common*           worker_com;

  // Main system TTI counter   
  uint32_t tti;

  uint32_t tx_worker_cnt;
  uint32_t nof_workers;
  
  bool running; 
};

} // namespace srsenb

#endif // SRSENB_TXRX_H
