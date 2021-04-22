/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSENB_NR_CC_WORKER_H
#define SRSENB_NR_CC_WORKER_H

#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/rrc_nr_interface_types.h"
#include "srsran/phy/enb/enb_dl_nr.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include <array>
#include <vector>

namespace srsenb {
namespace nr {

typedef struct {
  uint32_t                nof_carriers;
  srsran_enb_dl_nr_args_t dl;
} phy_nr_args_t;

class phy_nr_state
{
public:
  phy_nr_args_t        args = {};
  srsran::phy_cfg_nr_t cfg  = {};

  phy_nr_state()
  {
    args.nof_carriers              = 1;
    args.dl.nof_max_prb            = 100;
    args.dl.nof_tx_antennas        = 1;
    args.dl.pdsch.measure_evm      = true;
    args.dl.pdsch.measure_time     = true;
    args.dl.pdsch.sch.disable_simd = false;
  }
};

class cc_worker
{
public:
  cc_worker(uint32_t cc_idx, srslog::basic_logger& logger, phy_nr_state* phy_state_);
  ~cc_worker();

  bool set_carrier(const srsran_carrier_nr_t* carrier);
  void set_tti(uint32_t tti);

  cf_t*    get_tx_buffer(uint32_t antenna_idx);
  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  bool work_dl(const srsran_slot_cfg_t& dl_slot_cfg, stack_interface_phy_nr::dl_sched_t& dl_grants);

private:
  int encode_pdsch(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants);
  int encode_pdcch_dl(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants);

  srsran_slot_cfg_t                   dl_slot_cfg = {};
  uint32_t                            cc_idx      = 0;
  std::array<cf_t*, SRSRAN_MAX_PORTS> tx_buffer   = {};
  std::array<cf_t*, SRSRAN_MAX_PORTS> rx_buffer   = {};
  uint32_t                            buffer_sz   = 0;
  phy_nr_state*                       phy_state;
  srsran_enb_dl_nr_t                  enb_dl = {};
  srslog::basic_logger&               logger;
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_CC_WORKER_H
