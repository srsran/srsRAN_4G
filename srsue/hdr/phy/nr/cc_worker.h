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

#ifndef SRSLTE_NR_CC_WORKER_H
#define SRSLTE_NR_CC_WORKER_H

#include "srslte/common/log.h"
#include "srsue/hdr/phy/phy_common.h"
#include "state.h"

namespace srsue {
namespace nr {

class cc_worker
{
public:
  cc_worker(uint32_t cc_idx, srslog::basic_logger& log, state* phy_state_);
  ~cc_worker();

  bool set_carrier(const srslte_carrier_nr_t* carrier);
  void set_tti(uint32_t tti);

  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  cf_t*    get_tx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  bool work_dl();
  bool work_ul();

  int read_pdsch_d(cf_t* pdsch_d);

private:
  srslte_slot_cfg_t                   dl_slot_cfg = {};
  srslte_slot_cfg_t                   ul_slot_cfg = {};
  uint32_t                            cc_idx      = 0;
  std::array<cf_t*, SRSLTE_MAX_PORTS> rx_buffer   = {};
  std::array<cf_t*, SRSLTE_MAX_PORTS> tx_buffer   = {};
  uint32_t                            buffer_sz   = 0;
  state*                              phy         = nullptr;
  srslte_ue_dl_nr_t                   ue_dl       = {};
  srslte_ue_ul_nr_t                   ue_ul       = {};
  srslog::basic_logger&               logger;

  // Temporal attributes
  srslte_softbuffer_tx_t softbuffer_tx = {};
  srslte_softbuffer_rx_t softbuffer_rx = {};
  std::vector<uint8_t>   tx_data;
};

} // namespace nr
} // namespace srsue

#endif // SRSLTE_NR_CC_WORKER_H
