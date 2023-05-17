/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSENB_UE_H
#define SRSENB_UE_H

#include "common/mac_metrics.h"
#include "sched_interface.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/pool/pool_interface.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/mac_pcap_net.h"
#include "srsran/common/tti_point.h"
#include "srsran/mac/pdu.h"
#include "srsran/mac/pdu_queue.h"
#include "srsran/srslog/srslog.h"

#include "ta.h"
#include <pthread.h>
#include <vector>

namespace srsenb {

class rrc_interface_mac;
class rlc_interface_mac;
class phy_interface_stack_lte;

/// Class to manage the allocation, deallocation & access to UE carrier DL + UL softbuffers
struct ue_cc_softbuffers {
  // List of Tx softbuffers for all HARQ processes of one carrier
  using cc_softbuffer_tx_list_t = std::vector<srsran_softbuffer_tx_t>;
  // List of Rx softbuffers for all HARQ processes of one carrier
  using cc_softbuffer_rx_list_t = std::vector<srsran_softbuffer_rx_t>;

  const uint32_t          nof_tx_harq_proc;
  const uint32_t          nof_rx_harq_proc;
  cc_softbuffer_tx_list_t softbuffer_tx_list;
  cc_softbuffer_rx_list_t softbuffer_rx_list;

  ue_cc_softbuffers(uint32_t nof_prb, uint32_t nof_tx_harq_proc_, uint32_t nof_rx_harq_proc_);
  ue_cc_softbuffers(ue_cc_softbuffers&&) noexcept = default;
  ~ue_cc_softbuffers();
  void clear();

  srsran_softbuffer_tx_t& get_tx(uint32_t pid, uint32_t tb_idx)
  {
    return softbuffer_tx_list.at(pid * SRSRAN_MAX_TB + tb_idx);
  }
  srsran_softbuffer_rx_t& get_rx(uint32_t tti) { return softbuffer_rx_list.at(tti % nof_rx_harq_proc); }
};

/// Class to manage the allocation, deallocation & access to pending UL HARQ buffers
class cc_used_buffers_map
{
public:
  explicit cc_used_buffers_map();
  ~cc_used_buffers_map();

  void clear() { pdu_map.clear(); }

  uint8_t* request_pdu(tti_point tti, uint32_t len);

  srsran::unique_byte_buffer_t release_pdu(tti_point tti);

  void clear_old_pdus(tti_point current_tti);

  uint8_t*& operator[](tti_point tti);

  bool has_tti(tti_point tti) const;

private:
  srslog::basic_logger* logger;
  std::mutex            mutex;

  srsran::static_circular_map<uint32_t, srsran::unique_byte_buffer_t, SRSRAN_FDD_NOF_HARQ * 8> pdu_map;
};

class cc_buffer_handler
{
public:
  explicit cc_buffer_handler();
  ~cc_buffer_handler();

  void reset();
  void allocate_cc(srsran::unique_pool_ptr<ue_cc_softbuffers> cc_softbuffers_);
  void deallocate_cc();

  bool                    empty() const { return cc_softbuffers == nullptr; }
  srsran_softbuffer_tx_t& get_tx_softbuffer(uint32_t pid, uint32_t tb_idx)
  {
    return cc_softbuffers->get_tx(pid, tb_idx);
  }
  srsran_softbuffer_rx_t& get_rx_softbuffer(uint32_t tti) { return cc_softbuffers->get_rx(tti); }
  srsran::byte_buffer_t*  get_tx_payload_buffer(size_t harq_pid, size_t tb)
  {
    return tx_payload_buffer[harq_pid][tb].get();
  }
  cc_used_buffers_map& get_rx_used_buffers() { return rx_used_buffers; }

private:
  // CC softbuffers
  srsran::unique_pool_ptr<ue_cc_softbuffers> cc_softbuffers;

  // buffers
  cc_used_buffers_map rx_used_buffers;

  // One buffer per TB per DL HARQ process and per carrier is needed for each UE.
  std::array<std::array<srsran::unique_byte_buffer_t, SRSRAN_MAX_TB>, SRSRAN_FDD_NOF_HARQ> tx_payload_buffer;
};

class ue : public srsran::read_pdu_interface, public mac_ta_ue_interface
{
public:
  ue(uint16_t                                 rnti,
     uint32_t                                 enb_cc_idx,
     sched_interface*                         sched,
     rrc_interface_mac*                       rrc_,
     rlc_interface_mac*                       rlc,
     phy_interface_stack_lte*                 phy_,
     srslog::basic_logger&                    logger,
     uint32_t                                 nof_cells_,
     srsran::obj_pool_itf<ue_cc_softbuffers>* softbuffer_pool);

  virtual ~ue();
  void reset();
  void start_pcap(srsran::mac_pcap* pcap_);
  void start_pcap_net(srsran::mac_pcap_net* pcap_net_);
  void ue_cfg(const sched_interface::ue_cfg_t& ue_cfg);

  void     set_tti(uint32_t tti);
  uint16_t get_rnti() const { return rnti; }
  uint32_t set_ta(int ta) override;
  void     start_ta() { ta_fsm.start(); };
  uint32_t set_ta_us(float ta_us) { return ta_fsm.push_value(ta_us); };
  void     tic();
  void     trigger_padding(int lcid);
  void     set_active(bool active) { active_state.store(active, std::memory_order_relaxed); }
  bool     is_active() const { return active_state.load(std::memory_order_relaxed); }

  uint8_t* generate_pdu(uint32_t                              enb_cc_idx,
                        uint32_t                              harq_pid,
                        uint32_t                              tb_idx,
                        const sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                        uint32_t                              nof_pdu_elems,
                        uint32_t                              grant_size);
  uint8_t* generate_mch_pdu(uint32_t                             harq_pid,
                            const sched_interface::dl_pdu_mch_t& sched,
                            uint32_t                             nof_pdu_elems,
                            uint32_t                             grant_size);

  srsran_softbuffer_tx_t* get_tx_softbuffer(uint32_t enb_cc_idx, uint32_t harq_process, uint32_t tb_idx);
  srsran_softbuffer_rx_t* get_rx_softbuffer(uint32_t enb_cc_idx, uint32_t tti);

  uint8_t* request_buffer(uint32_t tti, uint32_t enb_cc_idx, uint32_t len);
  void     process_pdu(srsran::unique_byte_buffer_t pdu, uint32_t ue_cc_idx, uint32_t grant_nof_prbs);
  srsran::unique_byte_buffer_t release_pdu(uint32_t tti, uint32_t enb_cc_idx);
  void                         clear_old_buffers(uint32_t tti);

  std::mutex metrics_mutex = {};
  void       metrics_read(mac_ue_metrics_t* metrics_);
  void       metrics_rx(bool crc, uint32_t tbs);
  void       metrics_tx(bool crc, uint32_t tbs);
  void       metrics_phr(float phr);
  void       metrics_dl_ri(uint32_t dl_cqi);
  void       metrics_dl_pmi(uint32_t dl_cqi);
  void       metrics_dl_cqi(uint32_t dl_cqi);
  void       metrics_cnt();

  uint32_t read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes) final;

private:
  void allocate_sdu(srsran::sch_pdu* pdu, uint32_t lcid, uint32_t sdu_len);
  bool process_ce(srsran::sch_subh* subh, uint32_t grant_nof_prbs);
  void allocate_ce(srsran::sch_pdu* pdu, uint32_t lcid);

  rlc_interface_mac*       rlc = nullptr;
  rrc_interface_mac*       rrc = nullptr;
  phy_interface_stack_lte* phy = nullptr;
  srslog::basic_logger&    logger;
  sched_interface*         sched = nullptr;

  srsran::mac_pcap*     pcap      = nullptr;
  srsran::mac_pcap_net* pcap_net  = nullptr;
  uint64_t              conres_id = 0;
  uint16_t              rnti      = 0;
  std::atomic<uint32_t> last_tti{0};
  uint32_t              nof_failures = 0;

  std::atomic<bool> active_state{true};

  uint32_t         phr_counter    = 0;
  uint32_t         dl_cqi_counter = 0;
  uint32_t         dl_ri_counter  = 0;
  uint32_t         dl_pmi_counter = 0;
  mac_ue_metrics_t ue_metrics     = {};

  srsran::obj_pool_itf<ue_cc_softbuffers>* softbuffer_pool = nullptr;

  srsran::block_queue<uint32_t> pending_ta_commands;
  ta                            ta_fsm;

  // For UL there are multiple buffers per PID and are managed by pdu_queue
  srsran::sch_pdu mac_msg_dl, mac_msg_ul;
  srsran::mch_pdu mch_mac_msg_dl;

  srsran::bounded_vector<cc_buffer_handler, SRSRAN_MAX_CARRIERS> cc_buffers;

  // Mutexes
  std::mutex mutex;
};

} // namespace srsenb

#endif // SRSENB_UE_H
