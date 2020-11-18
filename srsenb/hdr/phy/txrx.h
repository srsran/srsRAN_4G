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

#ifndef SRSENB_TXRX_H
#define SRSENB_TXRX_H

#include "phy_common.h"
#include "prach_worker.h"
#include "srsenb/hdr/phy/lte/worker_pool.h"
#include "srslte/common/log.h"
#include "srslte/config.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/radio/radio.h"

namespace srsenb {

class txrx final : public srslte::thread
{
public:
  txrx();
  bool init(stack_interface_phy_lte*     stack_,
            srslte::radio_interface_phy* radio_handler,
            lte::worker_pool*            _workers_pool,
            phy_common*                  worker_com,
            prach_worker_pool*           prach_,
            srslte::log*                 log_h,
            uint32_t                     prio);
  void stop();

private:
  void run_thread() override;

  stack_interface_phy_lte*     stack        = nullptr;
  srslte::radio_interface_phy* radio_h      = nullptr;
  srslte::log*                 log_h        = nullptr;
  lte::worker_pool*            workers_pool = nullptr;
  prach_worker_pool*           prach        = nullptr;
  phy_common*                  worker_com   = nullptr;
  srslte::channel_ptr          ul_channel   = nullptr;

  // Main system TTI counter
  uint32_t tti = 0;

  uint32_t tx_worker_cnt = 0;
  uint32_t nof_workers   = 0;
  bool     running       = false;
};

} // namespace srsenb

#endif // SRSENB_TXRX_H
