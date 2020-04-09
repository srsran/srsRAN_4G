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

#ifndef SRSUE_PHCH_WORKER_H
#define SRSUE_PHCH_WORKER_H

#include "cc_worker.h"
#include "phy_common.h"
#include "srslte/common/thread_pool.h"
#include "srslte/srslte.h"
#include <string.h>

namespace srsue {

/**
 * The sf_worker class handles the PHY processing, UL and DL procedures associated with 1 subframe.
 * It contains multiple cc_worker objects, one for each component carrier which may be executed in
 * one or multiple threads.
 *
 * A sf_worker object is executed by a thread within the thread_pool.
 */

class sf_worker : public srslte::thread_pool::worker
{
public:
  sf_worker(uint32_t            max_prb,
            phy_common*         phy,
            srslte::log*        log,
            srslte::log*        log_phy_lib_h,
            chest_feedback_itf* chest_loop);
  virtual ~sf_worker();
  void reset();

  bool set_cell(uint32_t cc_idx, srslte_cell_t cell);

  /* Functions used by main PHY thread */
  cf_t*    get_buffer(uint32_t cc_idx, uint32_t antenna_idx);
  uint32_t get_buffer_len();
  void     set_tti(uint32_t tti);
  void     set_tx_time(srslte_timestamp_t tx_time);
  void     set_prach(cf_t* prach_ptr, float prach_power);
  void     set_cfo(const uint32_t& cc_idx, float cfo);

  void set_tdd_config(srslte_tdd_config_t config);
  void set_config(uint32_t cc_idx, srslte::phy_cfg_t& phy_cfg);
  void set_crnti(uint16_t rnti);
  void enable_pregen_signals(bool enabled);

  ///< Methods for plotting called from GUI thread
  int      read_ce_abs(float* ce_abs, uint32_t tx_antenna, uint32_t rx_antenna);
  uint32_t get_cell_nof_ports()
  {
    // wait until cell is initialized
    std::unique_lock<std::mutex> lock(mutex);
    while (!cell_initiated) {
      cell_init_cond.wait(lock);
    }
    return cell.nof_ports;
  }
  uint32_t get_rx_nof_antennas() { return phy->args->nof_rx_ant; }
  int      read_pdsch_d(cf_t* pdsch_d);
  float    get_sync_error();
  float    get_cfo();
  void     start_plot();

private:
  /* Inherited from thread_pool::worker. Function called every subframe to run the DL/UL processing */
  void work_imp() final;

  void update_measurements();
  void reset_uci(srslte_uci_data_t* uci_data);

  std::vector<cc_worker*> cc_workers;

  phy_common* phy = nullptr;

  srslte::log* log_h = nullptr;

  srslte::log* log_phy_lib_h = nullptr;

  chest_feedback_itf* chest_loop = nullptr;

  std::mutex mutex;

  srslte_cell_t       cell       = {};
  srslte_tdd_config_t tdd_config = {};

  std::condition_variable cell_init_cond;
  bool cell_initiated = false;

  cf_t* prach_ptr   = nullptr;
  float prach_power = 0;

  uint32_t           tti         = 0;
  srslte_timestamp_t tx_time     = {};

  uint32_t rssi_read_cnt = 0;
};

} // namespace srsue

#endif // SRSUE_PHCH_WORKER_H
