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

#ifndef SRSENB_UE_H
#define SRSENB_UE_H

#include "mac_metrics.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/log.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/sched_interface.h"
#include "srslte/mac/pdu.h"
#include "srslte/mac/pdu_queue.h"
#include "ta.h"
#include <pthread.h>
#include <vector>

namespace srsenb {

class ue : public srslte::read_pdu_interface, public srslte::pdu_queue::process_callback, public mac_ta_ue_interface
{
public:
  ue(uint16_t                 rnti,
     uint32_t                 nof_prb,
     sched_interface*         sched,
     rrc_interface_mac*       rrc_,
     rlc_interface_mac*       rlc,
     phy_interface_stack_lte* phy_,
     srslte::log_ref          log_,
     uint32_t                 nof_cells_,
     uint32_t                 nof_rx_harq_proc = SRSLTE_FDD_NOF_HARQ,
     uint32_t                 nof_tx_harq_proc = SRSLTE_FDD_NOF_HARQ * SRSLTE_MAX_TB);
  virtual ~ue();

  void reset();

  void start_pcap(srslte::mac_pcap* pcap_);

  void set_tti(uint32_t tti);

  uint32_t set_ta(int ta) override;

  void     start_ta() { ta_fsm.start(); };
  uint32_t set_ta_us(float ta_us) { return ta_fsm.push_value(ta_us); };
  uint32_t tick_ta_fsm() { return ta_fsm.tick(); };

  uint8_t* generate_pdu(uint32_t                        ue_cc_idx,
                        uint32_t                        harq_pid,
                        uint32_t                        tb_idx,
                        sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                        uint32_t                        nof_pdu_elems,
                        uint32_t                        grant_size);
  uint8_t*
  generate_mch_pdu(uint32_t harq_pid, sched_interface::dl_pdu_mch_t sched, uint32_t nof_pdu_elems, uint32_t grant_size);

  srslte_softbuffer_tx_t*
                          get_tx_softbuffer(const uint32_t ue_cc_idx, const uint32_t harq_process, const uint32_t tb_idx);
  srslte_softbuffer_rx_t* get_rx_softbuffer(const uint32_t ue_cc_idx, const uint32_t tti);

  bool     process_pdus();
  uint8_t* request_buffer(const uint32_t ue_cc_idx, const uint32_t tti, const uint32_t len);
  void     process_pdu(uint8_t* pdu, uint32_t nof_bytes, srslte::pdu_queue::channel_t channel) override;
  void     push_pdu(const uint32_t ue_cc_idx, const uint32_t tti, uint32_t len);
  void     deallocate_pdu(const uint32_t ue_cc_idx, const uint32_t tti);

  uint32_t rl_failure();
  void     rl_failure_reset();

  void set_lcg(uint32_t lcid, uint32_t lcg);

  void metrics_read(srsenb::mac_metrics_t* metrics);
  void metrics_rx(bool crc, uint32_t tbs);
  void metrics_tx(bool crc, uint32_t tbs);
  void metrics_phr(float phr);
  void metrics_dl_ri(uint32_t dl_cqi);
  void metrics_dl_pmi(uint32_t dl_cqi);
  void metrics_dl_cqi(uint32_t dl_cqi);
  void metrics_cnt();

  bool is_phy_added = false;
  int  read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes) final;

private:
  uint32_t allocate_cc_buffers(const uint32_t num_cc = 1); ///< Add and initialize softbuffers for CC

  void allocate_sdu(srslte::sch_pdu* pdu, uint32_t lcid, uint32_t sdu_len);
  bool process_ce(srslte::sch_subh* subh);
  void allocate_ce(srslte::sch_pdu* pdu, uint32_t lcid);

  std::vector<uint32_t> lc_groups[4];

  uint32_t      phr_counter    = 0;
  uint32_t      dl_cqi_counter = 0;
  uint32_t      dl_ri_counter  = 0;
  uint32_t      dl_pmi_counter = 0;
  mac_metrics_t metrics        = {};

  srslte::mac_pcap* pcap             = nullptr;
  uint64_t          conres_id        = 0;
  uint16_t          rnti             = 0;
  uint32_t          nof_prb          = 0;
  uint32_t          last_tti         = 0;
  uint32_t          nof_failures     = 0;
  int               nof_rx_harq_proc = 0;
  int               nof_tx_harq_proc = 0;

  typedef std::vector<srslte_softbuffer_tx_t>
                                       cc_softbuffer_tx_list_t; ///< List of Tx softbuffers for all HARQ processes of one carrier
  std::vector<cc_softbuffer_tx_list_t> softbuffer_tx;           ///< List of softbuffer lists for Tx

  typedef std::vector<srslte_softbuffer_rx_t>
                                       cc_softbuffer_rx_list_t; ///< List of Rx softbuffers for all HARQ processes of one carrier
  std::vector<cc_softbuffer_rx_list_t> softbuffer_rx;           ///< List of softbuffer lists for Rx

  typedef std::vector<uint8_t*> cc_buffer_ptr_t; ///< List of buffer pointers for RX HARQ processes of one carrier
  std::vector<cc_buffer_ptr_t>  pending_buffers; ///< List of buffer pointer list for Rx

  // One buffer per TB per HARQ process and per carrier is needed for each UE.
  std::vector<std::array<std::array<srslte::unique_byte_buffer_t, SRSLTE_MAX_TB>, SRSLTE_FDD_NOF_HARQ> >
      tx_payload_buffer;

  srslte::block_queue<uint32_t> pending_ta_commands;
  ta                            ta_fsm;

  // For UL there are multiple buffers per PID and are managed by pdu_queue
  srslte::pdu_queue pdus;
  srslte::sch_pdu   mac_msg_dl, mac_msg_ul;
  srslte::mch_pdu   mch_mac_msg_dl;

  rlc_interface_mac*       rlc = nullptr;
  rrc_interface_mac*       rrc = nullptr;
  phy_interface_stack_lte* phy = nullptr;
  srslte::log_ref          log_h;
  sched_interface*         sched = nullptr;

  // Mutexes
  std::mutex mutex;

  const uint8_t UL_CC_IDX = 0; ///< Passed to write CC index in PCAP (TODO: use actual CC idx)
};

} // namespace srsenb

#endif // SRSENB_UE_H
