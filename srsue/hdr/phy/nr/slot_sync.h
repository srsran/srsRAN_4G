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

#ifndef SRSUE_SLOT_SYNC_H
#define SRSUE_SLOT_SYNC_H

#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/radio/rf_buffer.h"
#include "srsran/radio/rf_timestamp.h"
#include "srsran/srsran.h"

namespace srsue {
namespace nr {
class slot_sync
{
public:
  struct args_t {
    double                      max_srate_hz    = 1.92e6;
    uint32_t                    nof_rx_channels = 1;
    srsran_subcarrier_spacing_t ssb_min_scs     = srsran_subcarrier_spacing_15kHz;
    bool                        disable_cfo     = false;
    float                       pbch_dmrs_thr   = 0.0f; ///< PBCH DMRS correlation detection threshold (0 means auto)
    float                       cfo_alpha       = 0.0f; ///< CFO averaging alpha (0 means auto)
    int                         thread_priority = -1;
  };

  slot_sync(srslog::basic_logger& logger);
  ~slot_sync();

  bool init(const args_t& args, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_);

  int set_sync_cfg(const srsran_ue_sync_nr_cfg_t& cfg);

  int  recv_callback(srsran::rf_buffer_t& rf_buffer, srsran_timestamp_t* timestamp);
  bool run_sfn_sync();
  bool run_camping(srsran::rf_buffer_t& buffer, srsran::rf_timestamp_t& timestamp);
  void run_stack_tti();

  srsran_slot_cfg_t get_slot_cfg();

private:
  const static int             MIN_TTI_JUMP = 1;    ///< Time gap reported to stack after receiving subframe
  const static int             MAX_TTI_JUMP = 1000; ///< Maximum time gap tolerance in RF stream metadata
  srslog::basic_logger&        logger;
  stack_interface_phy_nr*      stack = nullptr;
  srsran::radio_interface_phy* radio = nullptr;
  srsran::rf_timestamp_t       last_rx_time;
  srsran_ue_sync_nr_t          ue_sync_nr          = {};
  srsran_timestamp_t           stack_tti_ts_new    = {};
  srsran_timestamp_t           stack_tti_ts        = {};
  bool                         forced_rx_time_init = true; // Rx time sync after first receive from radio
  srsran::rf_buffer_t          sfn_sync_buff       = {};
  srsran_slot_cfg_t            slot_cfg            = {};
};
} // namespace nr
} // namespace srsue

#endif // SRSUE_SLOT_SYNC_H
