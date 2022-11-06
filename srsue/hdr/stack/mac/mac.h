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

#ifndef SRSUE_MAC_H
#define SRSUE_MAC_H

#include "demux.h"
#include "dl_harq.h"
#include "mac_metrics.h"
#include "mux.h"
#include "proc_bsr.h"
#include "proc_phr.h"
#include "proc_ra.h"
#include "proc_sr.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/threads.h"
#include "srsran/common/timers.h"
#include "srsran/common/tti_sync_cv.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "ul_harq.h"
#include <condition_variable>
#include <mutex>

namespace srsue {

class mac : public mac_interface_phy_lte,
            public mac_interface_rrc,
            public srsran::timer_callback,
            public mac_interface_demux
{
public:
  mac(const char* logname, ext_task_sched_handle task_sched_);
  ~mac();
  bool init(phy_interface_mac_lte* phy, rlc_interface_mac* rlc, rrc_interface_mac* rrc);
  void stop();

  void get_metrics(mac_metrics_t m[SRSRAN_MAX_CARRIERS]);

  /******** Interface from PHY (PHY -> MAC) ****************/
  /* see mac_interface.h for comments */
  void     new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action);
  void     new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action);
  void     tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSRAN_MAX_CODEWORDS]);
  void     bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len);
  uint16_t get_dl_sched_rnti(uint32_t tti);
  uint16_t get_ul_sched_rnti(uint32_t tti);

  void mch_decoded(uint32_t len, bool crc, uint8_t* payload);
  void process_mch_pdu(uint32_t len);

  void set_mbsfn_config(uint32_t nof_mbsfn_services);

  void run_tti(const uint32_t tti);

  /******** Interface from RRC (RRC -> MAC) ****************/
  void bcch_start_rx(int si_window_start, int si_window_length);
  void bcch_stop_rx();
  void pcch_start_rx();
  void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD);
  void setup_lcid(const logical_channel_config_t& config);
  void mch_start_rx(uint32_t lcid);
  void reconfiguration(const uint32_t& cc_idx, const bool& enable);
  void reset();

  /******** set/get MAC configuration  ****************/
  void set_config(mac_cfg_t& mac_cfg);
  void set_config(sr_cfg_t& sr_cfg);
  void set_contention_id(uint64_t uecri);

  /******* interface from demux object ****************/
  void reset_harq(uint32_t cc_idx);
  bool contention_resolution_id_rcv(uint64_t id);

  void set_rach_ded_cfg(uint32_t preamble_index, uint32_t prach_mask);

  uint16_t get_crnti();
  void     set_ho_rnti(uint16_t crnti, uint16_t target_pci);

  /*********** interface for stack ******************/
  void process_pdus();

  void start_pcap(srsran::mac_pcap* pcap);

  // Timer callback interface
  void timer_expired(uint32_t timer_id);

  uint32_t get_current_tti();

private:
  void clear_rntis();

  // Interaction with PHY
  phy_interface_mac_lte*                     phy_h = nullptr;
  rlc_interface_mac*                         rlc_h = nullptr;
  rrc_interface_mac*                         rrc_h = nullptr;
  srsran::ext_task_sched_handle              task_sched;
  srslog::basic_logger&                      logger;
  mac_interface_phy_lte::mac_phy_cfg_mbsfn_t phy_mbsfn_cfg = {};

  // Control scheduling for SI/RA/P RNTIs
  rnti_window_safe si_window, ra_window, p_window;

  // UE-specific RNTIs
  ue_rnti uernti;

  /* Multiplexing/Demultiplexing Units */
  mux   mux_unit;
  demux demux_unit;

  /* DL/UL HARQ */
  dl_harq_entity_vector dl_harq = {};
  ul_harq_entity_vector ul_harq = {};
  ul_harq_cfg_t         ul_harq_cfg;

  /* MAC Uplink-related Procedures */
  ra_proc  ra_procedure;
  sr_proc  sr_procedure;
  bsr_proc bsr_procedure;
  phr_proc phr_procedure;

  /* Buffers for PCH reception (not included in DL HARQ) */
  const static uint32_t  pch_payload_buffer_sz = 8 * 1024;
  srsran_softbuffer_rx_t pch_softbuffer        = {};
  uint8_t                pch_payload_buffer[pch_payload_buffer_sz];

  /* Buffers for MCH reception (not included in DL HARQ) */
  const static uint32_t  mch_payload_buffer_sz = SRSRAN_MAX_BUFFER_SIZE_BYTES;
  srsran_softbuffer_rx_t mch_softbuffer        = {};
  uint8_t                mch_payload_buffer[mch_payload_buffer_sz];
  srsran::mch_pdu        mch_msg;

  /* Functions for MAC Timers */
  srsran::timer_handler::unique_timer timer_alignment;
  void                                setup_timers(int time_alignment_timer);
  void                                timer_alignment_expire();

  /* Queue to dispatch stack tasks */
  srsran::task_multiqueue::queue_handle stack_task_dispatch_queue;

  // pointer to MAC PCAP object
  srsran::mac_pcap* pcap = nullptr;
  std::atomic<bool> is_first_ul_grant{false};

  std::mutex    metrics_mutex                = {};
  mac_metrics_t metrics[SRSRAN_MAX_CARRIERS] = {};

  std::atomic<bool> initialized = {false};

  const uint8_t PCELL_CC_IDX = 0;
};

} // namespace srsue

#endif // SRSUE_MAC_H
