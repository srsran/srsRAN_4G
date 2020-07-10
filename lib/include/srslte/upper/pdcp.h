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

#ifndef SRSLTE_PDCP_H
#define SRSLTE_PDCP_H

#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/task_scheduler.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/pdcp_entity_lte.h"

namespace srslte {

class pdcp : public srsue::pdcp_interface_rlc, public srsue::pdcp_interface_rrc
{
public:
  pdcp(srslte::task_sched_handle task_sched_, const char* logname);
  virtual ~pdcp();
  void init(srsue::rlc_interface_pdcp* rlc_, srsue::rrc_interface_pdcp* rrc_, srsue::gw_interface_pdcp* gw_);
  void stop();

  // GW interface
  bool is_lcid_enabled(uint32_t lcid);

  // RRC interface
  void reestablish() override;
  void reestablish(uint32_t lcid) override;
  void reset() override;
  void write_sdu(uint32_t lcid, unique_byte_buffer_t sdu) override;
  void write_sdu_mch(uint32_t lcid, unique_byte_buffer_t sdu);
  void add_bearer(uint32_t lcid, pdcp_config_t cnfg) override;
  void add_bearer_mrb(uint32_t lcid, pdcp_config_t cnfg);
  void del_bearer(uint32_t lcid);
  void change_lcid(uint32_t old_lcid, uint32_t new_lcid) override;
  void config_security(uint32_t lcid, as_security_config_t sec_cfg) override;
  void config_security_all(as_security_config_t sec_cfg) override;
  void enable_integrity(uint32_t lcid, srslte_direction_t direction) override;
  void enable_encryption(uint32_t lcid, srslte_direction_t direction) override;
  void enable_security_timed(uint32_t lcid, srslte_direction_t direction, uint32_t sn);
  bool get_bearer_state(uint32_t lcid, srslte::pdcp_lte_state_t* state);
  bool set_bearer_state(uint32_t lcid, const srslte::pdcp_lte_state_t& state);

  // RLC interface
  void write_pdu(uint32_t lcid, unique_byte_buffer_t sdu) override;
  void write_pdu_mch(uint32_t lcid, unique_byte_buffer_t sdu) override;
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) override;
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) override;
  void write_pdu_pcch(unique_byte_buffer_t sdu) override;

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;
  srslte::task_sched_handle  task_sched;
  srslte::log_ref            pdcp_log;

  std::map<uint16_t, std::unique_ptr<pdcp_entity_base> > pdcp_array, pdcp_array_mrb;

  // cache valid lcids to be checked from separate thread
  std::mutex         cache_mutex;
  std::set<uint32_t> valid_lcids_cached;

  bool valid_lcid(uint32_t lcid);
  bool valid_mch_lcid(uint32_t lcid);
};

} // namespace srslte
#endif // SRSLTE_PDCP_H
