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

#ifndef SRSENB_PHCH_WORKER_H
#define SRSENB_PHCH_WORKER_H

#include <mutex>
#include <string.h>

#include "cc_worker.h"
#include "phy_common.h"
#include "srslte/srslte.h"

namespace srsenb {

class sf_worker : public srslte::thread_pool::worker
{
public:
  sf_worker() = default;
  ~sf_worker();
  void init(phy_common* phy, srslte::log* log_h);

  cf_t* get_buffer_rx(uint32_t cc_idx, uint32_t antenna_idx);
  void  set_time(uint32_t tti, uint32_t tx_worker_cnt, srslte_timestamp_t tx_time);

  int      add_rnti(uint16_t rnti, uint32_t cc_idx, bool is_pcell, bool is_temporal);
  void     rem_rnti(uint16_t rnti);
  uint32_t get_nof_rnti();

  /* These are used by the GUI plotting tools */
  int  read_ce_abs(float* ce_abs);
  int  read_ce_arg(float* ce_abs);
  int  read_pusch_d(cf_t* pusch_d);
  int  read_pucch_d(cf_t* pusch_d);
  void start_plot();

  uint32_t get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);

private:
  void work_imp() final;

  /* Common objects */
  srslte::log* log_h     = nullptr;
  phy_common*  phy       = nullptr;
  bool         initiated = false;
  bool         running   = false;
  std::mutex   work_mutex;

  uint32_t           tti_rx = 0, tti_tx_dl = 0, tti_tx_ul = 0;
  uint32_t           t_rx = 0, t_tx_dl = 0, t_tx_ul = 0;
  uint32_t           tx_worker_cnt = 0;
  srslte_timestamp_t tx_time       = {};

  std::vector<std::unique_ptr<cc_worker> > cc_workers;

  srslte_softbuffer_tx_t temp_mbsfn_softbuffer = {};
};

} // namespace srsenb

#endif // SRSENB_PHCH_WORKER_H
