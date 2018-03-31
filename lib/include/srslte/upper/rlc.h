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

#ifndef SRSLTE_RLC_H
#define SRSLTE_RLC_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/msg_queue.h"
#include "srslte/upper/rlc_entity.h"
#include "srslte/upper/rlc_metrics.h"
#include "srslte/upper/rlc_common.h"

namespace srslte {

/****************************************************************************
 * RLC Layer
 * Ref: 3GPP TS 36.322 v10.0.0
 * Single interface for RLC layer - contains separate RLC entities for
 * each bearer.
 ***************************************************************************/
class rlc
    :public srsue::rlc_interface_mac
    ,public srsue::rlc_interface_pdcp
    ,public srsue::rlc_interface_rrc
{
public:
  rlc();
  virtual ~rlc() {}
  void init(srsue::pdcp_interface_rlc *pdcp_,
            srsue::rrc_interface_rlc  *rrc_,
            srsue::ue_interface       *ue_,
            log                       *rlc_log_,
            mac_interface_timers      *mac_timers_,
            uint32_t                   lcid_);
  void stop();

  void get_metrics(rlc_metrics_t &m);

  // PDCP interface
  void write_sdu(uint32_t lcid, byte_buffer_t *sdu);

  bool rb_is_um(uint32_t lcid);

  // MAC interface
  uint32_t get_buffer_state(uint32_t lcid);
  uint32_t get_total_buffer_state(uint32_t lcid);
  int      read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes);

  // RRC interface
  void reestablish();
  void reset();
  void empty_queue();
  void add_bearer(uint32_t lcid);
  void add_bearer(uint32_t lcid, srslte_rlc_config_t cnfg);

private:
  void reset_metrics(); 
  
  byte_buffer_pool            *pool;
  srslte::log                 *rlc_log;
  srsue::pdcp_interface_rlc   *pdcp;
  srsue::rrc_interface_rlc    *rrc;
  srslte::mac_interface_timers *mac_timers; 
  srsue::ue_interface         *ue;
  srslte::rlc_entity           rlc_array[SRSLTE_N_RADIO_BEARERS];
  uint32_t                     default_lcid;

  long                ul_tput_bytes[SRSLTE_N_RADIO_BEARERS];
  long                dl_tput_bytes[SRSLTE_N_RADIO_BEARERS];
  struct timeval      metrics_time[3];

  bool valid_lcid(uint32_t lcid);
};

} // namespace srsue


#endif // SRSLTE_RLC_H
