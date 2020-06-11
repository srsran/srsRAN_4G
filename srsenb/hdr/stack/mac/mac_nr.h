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

#ifndef SRSENB_MAC_NR_H
#define SRSENB_MAC_NR_H

#include "srslte/common/block_queue.h"
#include "srslte/common/logmap.h"
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/mac/mac_nr_pdu.h"

#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/gnb_interfaces.h"

namespace srsenb {

struct mac_nr_args_t {
  srsenb::pcap_args_t pcap;

  // params for the dummy user
  srsenb::sched_interface::sched_args_t sched;
  uint16_t                              rnti;
  uint32_t                              drb_lcid;

  // Add args
  std::string log_level;
  uint32_t    log_hex_limit;
  uint32_t    tb_size = 64;
};

class mac_nr final : public mac_interface_phy_nr, public mac_interface_rrc_nr, public mac_interface_rlc_nr
{
public:
  mac_nr();
  ~mac_nr();

  int  init(const mac_nr_args_t&    args_,
            phy_interface_stack_nr* phy,
            stack_interface_mac*    stack_,
            rlc_interface_mac_nr*   rlc_,
            rrc_interface_mac_nr*   rrc_);
  void stop();

  void get_metrics(srsenb::mac_metrics_t* metrics);

  // MAC interface for RRC
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg);
  int read_pdu_bcch_bch(uint8_t* payload);

  // MAC interface for RLC
  // TODO:
  int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) { return 0; }

  // Interface for PHY
  int sf_indication(const uint32_t tti);
  int rx_data_indication(stack_interface_phy_nr::rx_data_ind_t& grant);

  void process_pdus();

private:
  void get_dl_config(const uint32_t                               tti,
                     phy_interface_stack_nr::dl_config_request_t& config_request,
                     phy_interface_stack_nr::tx_request_t&        tx_request);

  // PDU processing
  int handle_pdu(srslte::unique_byte_buffer_t pdu);

  // Interaction with other components
  phy_interface_stack_nr* phy_h   = nullptr;
  stack_interface_mac*    stack_h = nullptr;
  rlc_interface_mac_nr*   rlc_h   = nullptr;
  rrc_interface_mac_nr*   rrc_h   = nullptr;

  std::unique_ptr<srslte::mac_nr_pcap> pcap = nullptr;
  srslte::log_ref                      log_h;
  srslte::byte_buffer_pool*            pool = nullptr;
  mac_nr_args_t                        args = {};

  bool started = false;

  srsenb::sched_interface::cell_cfg_t cfg = {};

  // BCH buffers
  struct sib_info_t {
    uint32_t                     index;
    uint32_t                     periodicity;
    srslte::unique_byte_buffer_t payload;
  };
  std::vector<sib_info_t>      bcch_dlsch_payload;
  srslte::unique_byte_buffer_t bcch_bch_payload = nullptr;

  // UE-specific buffer
  srslte::mac_nr_sch_pdu                    ue_tx_pdu;
  std::vector<srslte::unique_byte_buffer_t> ue_tx_buffer;
  srslte::block_queue<srslte::unique_byte_buffer_t>
      ue_rx_pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)

  srslte::unique_byte_buffer_t ue_rlc_buffer;

  srslte::mac_nr_sch_pdu ue_rx_pdu;
};

} // namespace srsenb

#endif // SRSENB_MAC_NR_H
