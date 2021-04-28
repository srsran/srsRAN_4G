/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/common/rnti_pool.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/enb_pdcp_interfaces.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/upper/pdcp.h"
#include <map>

#ifndef SRSENB_PDCP_H
#define SRSENB_PDCP_H

namespace srsenb {

class rrc_interface_pdcp;
class rlc_interface_pdcp;
class gtpu_interface_pdcp;

class pdcp : public pdcp_interface_rlc, public pdcp_interface_gtpu, public pdcp_interface_rrc
{
public:
  pdcp(srsran::task_sched_handle task_sched_, srslog::basic_logger& logger);
  virtual ~pdcp() {}
  void init(rlc_interface_pdcp* rlc_, rrc_interface_pdcp* rrc_, gtpu_interface_pdcp* gtpu_);
  void stop();

  // pdcp_interface_rlc
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) override;
  void notify_delivery(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn) override;
  void notify_failure(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn) override;
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}

  // pdcp_interface_rrc
  void reset(uint16_t rnti) override;
  void add_user(uint16_t rnti) override;
  void rem_user(uint16_t rnti) override;
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1) override;
  void add_bearer(uint16_t rnti, uint32_t lcid, srsran::pdcp_config_t cnfg) override;
  void del_bearer(uint16_t rnti, uint32_t lcid) override;
  void config_security(uint16_t rnti, uint32_t lcid, srsran::as_security_config_t cfg_sec) override;
  void enable_integrity(uint16_t rnti, uint32_t lcid) override;
  void enable_encryption(uint16_t rnti, uint32_t lcid) override;
  bool get_bearer_state(uint16_t rnti, uint32_t lcid, srsran::pdcp_lte_state_t* state) override;
  bool set_bearer_state(uint16_t rnti, uint32_t lcid, const srsran::pdcp_lte_state_t& state) override;
  void send_status_report(uint16_t rnti) override;
  void send_status_report(uint16_t rnti, uint32_t lcid) override;
  void reestablish(uint16_t rnti) override;

  // pdcp_interface_gtpu
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) override;

  // Metrics
  void get_metrics(pdcp_metrics_t& m, const uint32_t nof_tti);

private:
  class user_interface_rlc : public srsue::rlc_interface_pdcp
  {
  public:
    uint16_t                    rnti;
    srsenb::rlc_interface_pdcp* rlc;
    // rlc_interface_pdcp
    void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu);
    void discard_sdu(uint32_t lcid, uint32_t discard_sn);
    bool rb_is_um(uint32_t lcid);
    bool sdu_queue_is_full(uint32_t lcid);
  };

  class user_interface_gtpu : public srsue::gw_interface_pdcp
  {
  public:
    uint16_t                     rnti;
    srsenb::gtpu_interface_pdcp* gtpu;
    // gw_interface_pdcp
    void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
    void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
  };

  class user_interface_rrc : public srsue::rrc_interface_pdcp
  {
  public:
    uint16_t                    rnti;
    srsenb::rrc_interface_pdcp* rrc;
    // rrc_interface_pdcp
    void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu);
    void        write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu);
    void        write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu);
    void        write_pdu_pcch(srsran::unique_byte_buffer_t pdu);
    void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}
    const char* get_rb_name(uint32_t lcid);
  };

  class user_interface
  {
  public:
    user_interface_rlc            rlc_itf;
    user_interface_gtpu           gtpu_itf;
    user_interface_rrc            rrc_itf;
    unique_rnti_ptr<srsran::pdcp> pdcp;
  };

  void clear_user(user_interface* ue);

  std::map<uint32_t, user_interface> users;

  rlc_interface_pdcp*       rlc  = nullptr;
  rrc_interface_pdcp*       rrc  = nullptr;
  gtpu_interface_pdcp*      gtpu = nullptr;
  srsran::task_sched_handle task_sched;
  srslog::basic_logger&     logger;
};

} // namespace srsenb
#endif // SRSENB_PDCP_H
