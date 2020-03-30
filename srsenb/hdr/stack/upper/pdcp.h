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

#include "srslte/common/timers.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/pdcp.h"
#include <map>

#ifndef SRSENB_PDCP_H
#define SRSENB_PDCP_H

namespace srsenb {

class pdcp : public pdcp_interface_rlc, public pdcp_interface_gtpu, public pdcp_interface_rrc
{
public:
  pdcp(srslte::task_handler_interface* task_executor_, const char* logname);
  virtual ~pdcp() {}
  void init(rlc_interface_pdcp* rlc_, rrc_interface_pdcp* rrc_, gtpu_interface_pdcp* gtpu_);
  void stop();

  // pdcp_interface_rlc
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) override;
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}

  // pdcp_interface_rrc
  void reset(uint16_t rnti) override;
  void add_user(uint16_t rnti) override;
  void rem_user(uint16_t rnti) override;
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) override;
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cnfg) override;
  void config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t cfg_sec) override;
  void enable_integrity(uint16_t rnti, uint32_t lcid) override;
  void enable_encryption(uint16_t rnti, uint32_t lcid) override;
  bool get_bearer_status(uint16_t rnti, uint32_t lcid, uint16_t* dlsn, uint16_t* dlhfn, uint16_t* ulsn, uint16_t* ulhfn)
      override;

private:
  class user_interface_rlc : public srsue::rlc_interface_pdcp
  {
  public:
    uint16_t                    rnti;
    srsenb::rlc_interface_pdcp* rlc;
    // rlc_interface_pdcp
    void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking);
    void discard_sdu(uint32_t lcid, uint32_t discard_sn);
    bool rb_is_um(uint32_t lcid);
  };

  class user_interface_gtpu : public srsue::gw_interface_pdcp
  {
  public:
    uint16_t                     rnti;
    srsenb::gtpu_interface_pdcp* gtpu;
    // gw_interface_pdcp
    void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
    void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}
  };

  class user_interface_rrc : public srsue::rrc_interface_pdcp
  {
  public:
    uint16_t                    rnti;
    srsenb::rrc_interface_pdcp* rrc;
    // rrc_interface_pdcp
    void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
    void        write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu);
    void        write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu);
    void        write_pdu_pcch(srslte::unique_byte_buffer_t pdu);
    void        write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
    std::string get_rb_name(uint32_t lcid);
  };

  class user_interface
  {
  public:
    user_interface_rlc  rlc_itf;
    user_interface_gtpu gtpu_itf;
    user_interface_rrc  rrc_itf;
    srslte::pdcp*       pdcp;
  };

  void clear_user(user_interface* ue);

  std::map<uint32_t, user_interface> users;

  rlc_interface_pdcp*             rlc;
  rrc_interface_pdcp*             rrc;
  gtpu_interface_pdcp*            gtpu;
  srslte::task_handler_interface* task_executor;
  srslte::log_ref                 log_h;
  srslte::byte_buffer_pool*       pool;
};

} // namespace srsenb
#endif // SRSENB_PDCP_H
