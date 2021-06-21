/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_NR_CC_WORKER_H
#define SRSENB_NR_CC_WORKER_H

#include "phy_nr_state.h"
#include "srsenb/hdr/phy/phy_interfaces.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/rrc_nr_interface_types.h"
#include "srsran/phy/enb/enb_dl_nr.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include <array>
#include <vector>

namespace srsenb {
namespace nr {

class cc_worker
{
public:
  struct args_t {
    uint32_t                cc_idx  = 0;
    srsran_carrier_nr_t     carrier = {};
    srsran_enb_dl_nr_args_t dl      = {};
  };

  cc_worker(const args_t& args, srslog::basic_logger& logger, phy_nr_state& phy_state_);
  ~cc_worker();

  void set_tti(uint32_t tti);

  cf_t*    get_tx_buffer(uint32_t antenna_idx);
  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  bool work_dl(stack_interface_phy_nr::dl_sched_t& dl_grants, stack_interface_phy_nr::ul_sched_t& ul_grants);
  bool work_ul();

private:
  int encode_pdsch(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants);
  int encode_pdcch_dl(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants);

  uint32_t                            nof_tx_antennas = 0;
  srsran_slot_cfg_t                   dl_slot_cfg     = {};
  srsran_slot_cfg_t                   ul_slot_cfg     = {};
  uint32_t                            cc_idx          = 0;
  std::array<cf_t*, SRSRAN_MAX_PORTS> tx_buffer       = {};
  std::array<cf_t*, SRSRAN_MAX_PORTS> rx_buffer       = {};
  uint32_t                            buffer_sz       = 0;
  phy_nr_state&                       phy_state;
  srsran_enb_dl_nr_t                  gnb_dl = {};
  srslog::basic_logger&               logger;
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_CC_WORKER_H
