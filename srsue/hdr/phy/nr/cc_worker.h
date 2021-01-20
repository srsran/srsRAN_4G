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

#ifndef SRSLTE_NR_CC_WORKER_H
#define SRSLTE_NR_CC_WORKER_H

#include "srslte/common/log.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy_common.h"
#include <array>
#include <vector>

namespace srsue {
namespace nr {

typedef struct {
  uint32_t               nof_carriers;
  srslte_ue_dl_nr_args_t dl;
} phy_nr_args_t;

typedef struct {
  srslte_sch_cfg_nr_t pdsch;
} phy_nr_cfg_t;

class phy_nr_state
{
public:
  phy_nr_args_t args = {};
  phy_nr_cfg_t  cfg  = {};

  phy_nr_state()
  {
    args.nof_carriers              = 1;
    args.dl.nof_rx_antennas        = 1;
    args.dl.nof_max_prb            = 100;
    args.dl.pdsch.measure_evm      = true;
    args.dl.pdsch.measure_time     = true;
    args.dl.pdsch.sch.disable_simd = false;
  }
};

class cc_worker
{
public:
  cc_worker(uint32_t cc_idx, srslte::log* log, phy_nr_state* phy_state_);
  ~cc_worker();

  bool set_carrier(const srslte_carrier_nr_t* carrier);
  void set_tti(uint32_t tti);

  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  bool work_dl();

private:
  srslte_dl_slot_cfg_t                dl_slot_cfg = {};
  uint32_t                            cc_idx      = 0;
  std::array<cf_t*, SRSLTE_MAX_PORTS> rx_buffer   = {};
  std::array<cf_t*, SRSLTE_MAX_PORTS> tx_buffer   = {};
  uint32_t                            buffer_sz   = 0;
  phy_nr_state*                       phy_state   = nullptr;
  srslte_ue_dl_nr_t                   ue_dl       = {};
  srslte::log*                        log_h       = nullptr;

  // Temporal attributes
  srslte_softbuffer_rx_t softbuffer_rx = {};
  std::vector<uint8_t>   data;

  // Current rnti
  uint16_t rnti = 0;

  // Current coreset and search space
  srslte_coreset_t      coreset      = {};
  srslte_search_space_t search_space = {};
};

} // namespace nr
} // namespace srsue

#endif // SRSLTE_NR_CC_WORKER_H
