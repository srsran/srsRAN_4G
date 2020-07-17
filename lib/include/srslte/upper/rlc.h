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

#ifndef SRSLTE_RLC_H
#define SRSLTE_RLC_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc_common.h"
#include "srslte/upper/rlc_metrics.h"

namespace srslte {

/****************************************************************************
 * RLC Layer
 * Ref: 3GPP TS 36.322 v10.0.0
 * Single interface for RLC layer - contains separate RLC entities for
 * each bearer.
 ***************************************************************************/
class rlc : public srsue::rlc_interface_mac, public srsue::rlc_interface_pdcp, public srsue::rlc_interface_rrc
{
public:
  rlc(const char* logname);
  virtual ~rlc();
  void init(srsue::pdcp_interface_rlc* pdcp_,
            srsue::rrc_interface_rlc*  rrc_,
            srslte::timer_handler*     timers_,
            uint32_t                   lcid_);
  void init(srsue::pdcp_interface_rlc* pdcp_,
            srsue::rrc_interface_rlc*  rrc_,
            srslte::timer_handler*     timers_,
            uint32_t                   lcid_,
            bsr_callback_t             bsr_callback_);
  void stop();

  void get_metrics(rlc_metrics_t& m);

  // PDCP interface
  void write_sdu(uint32_t lcid, unique_byte_buffer_t sdu);
  void write_sdu_mch(uint32_t lcid, unique_byte_buffer_t sdu);
  bool rb_is_um(uint32_t lcid);
  void discard_sdu(uint32_t lcid, uint32_t discard_sn);
  bool sdu_queue_is_full(uint32_t lcid);

  // MAC interface
  bool     has_data_locked(const uint32_t lcid);
  uint32_t get_buffer_state(const uint32_t lcid);
  uint32_t get_total_mch_buffer_state(uint32_t lcid);
  int      read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  int      read_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  int      get_increment_sequence_num();
  void     write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void     write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu);
  void     write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes);
  void     write_pdu_pcch(srslte::unique_byte_buffer_t pdu);
  void     write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);

  // RRC interface
  bool is_suspended(const uint32_t lcid);
  bool has_data(const uint32_t lcid);
  void reestablish();
  void reestablish(uint32_t lcid);
  void reset();
  void empty_queue();
  void add_bearer(uint32_t lcid, const rlc_config_t& cnfg);
  void add_bearer_mrb(uint32_t lcid);
  void del_bearer(uint32_t lcid);
  void del_bearer_mrb(uint32_t lcid);
  void suspend_bearer(uint32_t lcid);
  void resume_bearer(uint32_t lcid);
  void change_lcid(uint32_t old_lcid, uint32_t new_lcid);
  bool has_bearer(uint32_t lcid);

private:
  void reset_metrics();

  byte_buffer_pool*          pool = nullptr;
  srslte::log_ref            rlc_log;
  srsue::pdcp_interface_rlc* pdcp   = nullptr;
  srsue::rrc_interface_rlc*  rrc    = nullptr;
  srslte::timer_handler*     timers = nullptr;

  typedef std::map<uint16_t, rlc_common*>  rlc_map_t;
  typedef std::pair<uint16_t, rlc_common*> rlc_map_pair_t;

  rlc_map_t        rlc_array, rlc_array_mrb;
  pthread_rwlock_t rwlock;

  uint32_t default_lcid = 0;

  bsr_callback_t bsr_callback = nullptr;

  // Timer needed for metrics calculation
  struct timeval metrics_time[3] = {};

  bool valid_lcid(uint32_t lcid);
  bool valid_lcid_mrb(uint32_t lcid);

  void update_bsr(uint32_t lcid);
  void update_bsr_mch(uint32_t lcid);
};

void rlc_bearer_metrics_print(const rlc_bearer_metrics_t& metrics);

} // namespace srslte

#endif // SRSLTE_RLC_H
