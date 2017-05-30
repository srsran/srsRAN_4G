/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef RLC_H
#define RLC_H

#include "common/buffer_pool.h"
#include "common/log.h"
#include "common/common.h"
#include "common/interfaces.h"
#include "common/msg_queue.h"
#include "upper/rlc_entity.h"
#include "upper/rlc_metrics.h"

namespace srsue {

/****************************************************************************
 * RLC Layer
 * Ref: 3GPP TS 36.322 v10.0.0
 * Single interface for RLC layer - contains separate RLC entities for
 * each bearer.
 ***************************************************************************/
class rlc
    :public rlc_interface_mac
    ,public rlc_interface_pdcp
    ,public rlc_interface_rrc
{
public:
  rlc();
  void init(pdcp_interface_rlc *pdcp_,
            rrc_interface_rlc  *rrc_,
            ue_interface       *ue_,
            srslte::log        *rlc_log_, 
            srslte::mac_interface_timers *mac_timers_);
  void stop();

  void get_metrics(rlc_metrics_t &m);

  // PDCP interface
  void write_sdu(uint32_t lcid, byte_buffer_t *sdu);

  // MAC interface
  uint32_t get_buffer_state(uint32_t lcid);
  uint32_t get_total_buffer_state(uint32_t lcid);
  int      read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes);

  // RRC interface
  void reset();
  void add_bearer(uint32_t lcid);
  void add_bearer(uint32_t lcid, LIBLTE_RRC_RLC_CONFIG_STRUCT *cnfg);

private:
  void reset_metrics(); 
  
  srslte::byte_buffer_pool        *pool;
  srslte::log        *rlc_log;
  pdcp_interface_rlc *pdcp;
  rrc_interface_rlc  *rrc;
  srslte::mac_interface_timers *mac_timers; 
  ue_interface       *ue;
  rlc_entity          rlc_array[SRSUE_N_RADIO_BEARERS];

  long                ul_tput_bytes[SRSUE_N_RADIO_BEARERS];
  long                dl_tput_bytes[SRSUE_N_RADIO_BEARERS];
  struct timeval      metrics_time[3];

  bool valid_lcid(uint32_t lcid);
};

} // namespace srsue


#endif // RLC_H
