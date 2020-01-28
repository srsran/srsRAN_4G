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
#ifndef SRSUE_INTRA_MEASURE_H
#define SRSUE_INTRA_MEASURE_H

#include <srslte/common/log.h>
#include <srslte/common/threads.h>
#include <srslte/common/tti_sync_cv.h>
#include <srslte/srslte.h>

#include "scell_recv.h"

namespace srsue {
namespace scell {

// Class to perform intra-frequency measurements
class intra_measure : public thread
{
public:
  intra_measure();
  ~intra_measure();
  void     init(phy_common* common, rrc_interface_phy_lte* rrc, srslte::log* log_h);
  void     stop();
  void     set_primary_cell(uint32_t earfcn, srslte_cell_t cell);
  void     set_cells_to_meas(const std::set<uint32_t>& pci);
  void     meas_stop();
  void     write(uint32_t tti, cf_t* data, uint32_t nsamples);
  uint32_t get_earfcn() { return current_earfcn; };
  void     wait_meas()
  { // Only used by scell_search_test
    meas_sync.wait();
  }

private:
  void             run_thread() override;
  const static int INTRA_FREQ_MEAS_PRIO = DEFAULT_PRIORITY + 5;

  scell_recv             scell            = {};
  rrc_interface_phy_lte* rrc              = nullptr;
  srslte::log*           log_h            = nullptr;
  phy_common*            common           = nullptr;
  uint32_t               current_earfcn   = 0;
  uint32_t               current_sflen    = 0;
  srslte_cell_t          serving_cell     = {};
  std::set<uint32_t>     active_pci       = {};
  std::mutex             active_pci_mutex = {};

  srslte::tti_sync_cv tti_sync;
  srslte::tti_sync_cv meas_sync; // Only used by scell_search_test

  cf_t* search_buffer = nullptr;

  bool                running         = false;
  bool                receive_enabled = false;
  bool                receiving       = false;
  uint32_t            receive_cnt     = 0;
  srslte_ringbuffer_t ring_buffer     = {};

  srslte_refsignal_dl_sync_t refsignal_dl_sync = {};
};

} // namespace scell
} // namespace srsue

#endif // SRSUE_INTRA_MEASURE_H
