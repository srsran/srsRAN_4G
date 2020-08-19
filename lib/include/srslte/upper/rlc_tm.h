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

#ifndef SRSLTE_RLC_TM_H
#define SRSLTE_RLC_TM_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/byte_buffer_queue.h"
#include "srslte/upper/rlc_common.h"

namespace srslte {

class rlc_tm final : public rlc_common
{
public:
  rlc_tm(srslte::log_ref log_, uint32_t lcid_, srsue::pdcp_interface_rlc* pdcp_, srsue::rrc_interface_rlc* rrc_);
  ~rlc_tm() override;
  bool configure(const rlc_config_t& cnfg) override;
  void stop() override;
  void reestablish() override;
  void empty_queue() override;

  rlc_mode_t get_mode() override;
  uint32_t   get_bearer() override;

  rlc_bearer_metrics_t get_metrics() override;
  void                 reset_metrics() override;

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu) override;
  void discard_sdu(uint32_t discard_sn) override;
  bool sdu_queue_is_full() override;

  // MAC interface
  bool     has_data() override;
  uint32_t get_buffer_state() override;
  int      read_pdu(uint8_t* payload, uint32_t nof_bytes) override;
  void     write_pdu(uint8_t* payload, uint32_t nof_bytes) override;

  void set_bsr_callback(bsr_callback_t callback) override {}

private:
  byte_buffer_pool*          pool = nullptr;
  srslte::log_ref            log;
  uint32_t                   lcid = 0;
  srsue::pdcp_interface_rlc* pdcp = nullptr;
  srsue::rrc_interface_rlc*  rrc  = nullptr;

  bool tx_enabled = true;

  rlc_bearer_metrics_t metrics = {};

  // Thread-safe queues for MAC messages
  byte_buffer_queue ul_queue;
};

} // namespace srslte

#endif // SRSLTE_RLC_TM_H
