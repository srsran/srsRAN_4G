/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSUE_DEMUX_H
#define SRSUE_DEMUX_H

#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/pdu_queue.h"
#include "srslte/common/log.h"
#include "srslte/common/timers.h"
#include "srslte/common/pdu.h"

/* Logical Channel Demultiplexing and MAC CE dissassemble */   


namespace srsue {

class mac_interface_demux
{
public:
  virtual void reset_harq(uint32_t cc_idx)               = 0;
  virtual bool contention_resolution_id_rcv(uint64_t id) = 0;
};

class demux : public srslte::pdu_queue::process_callback
{
public:
  demux(srslte::log* log_);
  void init(phy_interface_mac_common* phy_h_,
            rlc_interface_mac*        rlc,
            mac_interface_demux*      mac,
            srslte::timers::timer*    time_alignment_timer);

  bool     process_pdus();
  uint8_t* request_buffer(uint32_t len);
  uint8_t* request_buffer_bcch(uint32_t len);
  void     deallocate(uint8_t* payload_buffer_ptr);

  void push_pdu(uint8_t* buff, uint32_t nof_bytes);
  void push_pdu_bcch(uint8_t* buff, uint32_t nof_bytes);
  void push_pdu_mch(uint8_t* buff, uint32_t nof_bytes);
  void push_pdu_temp_crnti(uint8_t* buff, uint32_t nof_bytes);

  bool     get_uecrid_successful();

  void     process_pdu(uint8_t* pdu, uint32_t nof_bytes, srslte::pdu_queue::channel_t channel);
  void     mch_start_rx(uint32_t lcid);

private:
  const static int MAX_PDU_LEN      = 150 * 1024 / 8; // ~ 150 Mbps
  const static int MAX_BCCH_PDU_LEN = 1024;
  uint8_t bcch_buffer[MAX_BCCH_PDU_LEN]; // BCCH PID has a dedicated buffer

  srslte::sch_pdu mac_msg;
  srslte::mch_pdu mch_mac_msg;
  srslte::sch_pdu pending_mac_msg;
  uint8_t         mch_lcids[SRSLTE_N_MCH_LCIDS];
  void            process_sch_pdu_rt(uint8_t* buff, uint32_t nof_bytes);
  void            process_sch_pdu(srslte::sch_pdu* pdu);
  void            process_mch_pdu(srslte::mch_pdu* pdu);

  bool process_ce(srslte::sch_subh *subheader);

  bool is_uecrid_successful;

  phy_interface_mac_common* phy_h                = nullptr;
  srslte::log*              log_h                = nullptr;
  srslte::timers::timer*    time_alignment_timer = nullptr;
  rlc_interface_mac*        rlc                  = nullptr;
  mac_interface_demux*      mac                  = nullptr;

  // Buffer of PDUs
  srslte::pdu_queue pdus; 
};

} // namespace srsue

#endif // SRSUE_DEMUX_H



