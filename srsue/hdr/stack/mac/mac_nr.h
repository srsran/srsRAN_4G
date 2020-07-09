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

#ifndef SRSUE_MAC_NR_H
#define SRSUE_MAC_NR_H

#include "srslte/common/block_queue.h"
#include "srslte/common/logmap.h"
#include "srslte/common/mac_nr_pcap.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srslte/mac/mac_nr_pdu.h"
#include "srsue/hdr/stack/mac/mux.h"
#include "srsue/hdr/stack/ue_stack_base.h"

namespace srsue {

struct mac_nr_args_t {
  srsue::pcap_args_t pcap;
  // TODO: remove temp variable
  uint32_t drb_lcid;
};

class mac_nr final : public mac_interface_phy_nr, public mac_interface_rrc_nr
{
public:
  mac_nr(srslte::ext_task_sched_handle task_sched_);
  ~mac_nr();

  int  init(const mac_nr_args_t& args_, phy_interface_mac_nr* phy, rlc_interface_mac* rlc);
  void stop();

  void reset();

  void run_tti(const uint32_t tti);

  uint16_t get_dl_sched_rnti(uint32_t tti);
  uint16_t get_ul_sched_rnti(uint32_t tti);

  void bch_decoded_ok(uint32_t tti, srslte::unique_byte_buffer_t payload);

  int sf_indication(const uint32_t tti);

  void tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant);

  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant);

  void timer_expired(uint32_t timer_id);

  void get_metrics(mac_metrics_t* metrics);

  /// stack interface
  void process_pdus();

private:
  void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD);
  void setup_lcid(const logical_channel_config_t& config);

  void handle_pdu(srslte::unique_byte_buffer_t pdu);
  void get_ul_data(const mac_nr_grant_ul_t& grant, phy_interface_stack_nr::tx_request_t* tx_request);

  /// Interaction with rest of the stack
  phy_interface_mac_nr*         phy = nullptr;
  rlc_interface_mac*            rlc = nullptr;
  srslte::ext_task_sched_handle task_sched;

  std::unique_ptr<srslte::mac_nr_pcap> pcap = nullptr;
  srslte::log_ref                      log_h;
  srslte::byte_buffer_pool*            pool = nullptr;
  mac_nr_args_t                        args = {};

  bool started = false;

  uint16_t crnti = 0xdead;

  static constexpr uint32_t MIN_RLC_PDU_LEN =
      5; ///< minimum bytes that need to be available in a MAC PDU for attempting to add another RLC SDU

  srslte::block_queue<srslte::unique_byte_buffer_t>
      pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)

  mac_metrics_t metrics[SRSLTE_MAX_CARRIERS] = {};

  /// Rx buffer
  srslte::mac_nr_sch_pdu rx_pdu;

  /// Tx buffer
  srslte::mac_nr_sch_pdu       tx_pdu;
  srslte::unique_byte_buffer_t tx_buffer  = nullptr;
  srslte::unique_byte_buffer_t rlc_buffer = nullptr;

  srslte::task_multiqueue::queue_handle stack_task_dispatch_queue;
};

} // namespace srsue

#endif // SRSUE_MAC_NR_H
