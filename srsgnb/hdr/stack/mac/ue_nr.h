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

#ifndef SRSENB_UE_NR_H
#define SRSENB_UE_NR_H

#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsgnb/hdr/stack/mac/sched_nr_interface.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/mac/bsr_nr.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
#include <mutex>
#include <vector>

namespace srsenb {

class rrc_interface_mac_nr;
class rlc_interface_mac_nr;
class phy_interface_stack_nr;

class ue_nr : public srsran::read_pdu_interface
{
public:
  ue_nr(uint16_t                rnti,
        uint32_t                enb_cc_idx,
        sched_nr_interface*     sched_,
        rrc_interface_mac_nr*   rrc_,
        rlc_interface_mac*      rlc,
        phy_interface_stack_nr* phy_,
        srslog::basic_logger&   logger);

  virtual ~ue_nr();
  void reset();
  void ue_cfg(const sched_nr_interface::ue_cfg_t& ue_cfg);

  void     set_tti(uint32_t tti);
  uint16_t get_rnti() const { return rnti; }
  void     set_active(bool active) { active_state.store(active, std::memory_order_relaxed); }
  bool     is_active() const { return active_state.load(std::memory_order_relaxed); }
  void     store_msg3(srsran::unique_byte_buffer_t pdu);

  int generate_pdu(srsran::byte_buffer_t* pdu, uint32_t grant_size, srsran::const_span<uint32_t> subpdu_lcids);

  std::mutex metrics_mutex = {};
  void       metrics_read(mac_ue_metrics_t* metrics_);
  void       metrics_rx(bool crc, uint32_t tbs);
  void       metrics_tx(bool crc, uint32_t tbs);
  void       metrics_phr(float phr);
  void       metrics_dl_ri(uint32_t dl_cqi);
  void       metrics_dl_pmi(uint32_t dl_cqi);
  void       metrics_dl_cqi(const srsran_uci_cfg_nr_t& cfg_, uint32_t dl_cqi);
  void       metrics_dl_mcs(uint32_t mcs);
  void       metrics_ul_mcs(uint32_t mcs);
  void       metrics_pucch_sinr(float sinr);
  void       metrics_pusch_sinr(float sinr);
  void       metrics_cnt();

  uint32_t read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes) final;

private:
  rlc_interface_mac*      rlc = nullptr;
  rrc_interface_mac_nr*   rrc = nullptr;
  phy_interface_stack_nr* phy = nullptr;
  srslog::basic_logger&   logger;
  sched_nr_interface*     sched = nullptr;

  uint64_t conres_id    = 0;
  uint16_t rnti         = 0;
  uint32_t last_tti     = 0;
  uint32_t nof_failures = 0;

  std::atomic<bool> active_state{true};

  // TODO: some counters are kept as members of class ue_nr, while some others (i.e., mcs) are kept in the ue_metrics
  // We should make these counters more uniform
  uint32_t         phr_counter          = 0;
  uint32_t         dl_cqi_valid_counter = 0;
  uint32_t         dl_ri_counter        = 0;
  uint32_t         dl_pmi_counter       = 0;
  uint32_t         pucch_sinr_counter   = 0;
  uint32_t         pusch_sinr_counter   = 0;
  mac_ue_metrics_t ue_metrics           = {};

  // UE-specific buffer for MAC PDU packing, unpacking and handling
  srsran::mac_sch_pdu_nr                    mac_pdu_dl, mac_pdu_ul;
  std::vector<srsran::unique_byte_buffer_t> ue_tx_buffer;
  srsran::block_queue<srsran::unique_byte_buffer_t>
                               ue_rx_pdu_queue; ///< currently only DCH PDUs supported (add BCH, PCH, etc)
  srsran::unique_byte_buffer_t ue_rlc_buffer;

  srsran::unique_byte_buffer_t last_msg3; ///< holds UE ID received in Msg3 for ConRes CE

  static constexpr int32_t MIN_RLC_PDU_LEN =
      5; ///< minimum bytes that need to be available in a MAC PDU for attempting to add another RLC SDU

  // Mutexes
  std::mutex mutex;
};

} // namespace srsenb

#endif // SRSENB_UE_NR_H
