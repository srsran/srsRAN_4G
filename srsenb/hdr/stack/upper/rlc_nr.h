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

#ifndef SRSENB_RLC_NR_H
#define SRSENB_RLC_NR_H

#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/upper/rlc.h"
#include <map>

namespace srsenb {

typedef struct {
  uint32_t lcid;
  uint32_t plmn;
  uint16_t mtch_stop;
  uint8_t* payload;
} mch_service_t;

class rlc_nr final : public rlc_interface_mac_nr, public rlc_interface_rrc_nr, public rlc_interface_pdcp_nr
{
public:
  explicit rlc_nr(const char* logname);
  void init(pdcp_interface_rlc_nr* pdcp_,
            rrc_interface_rlc_nr*  rrc_,
            mac_interface_rlc_nr*  mac_,
            srsran::timer_handler* timers_);
  void stop();

  // rlc_interface_rrc_nr
  void clear_buffer(uint16_t rnti);
  void add_user(uint16_t rnti);
  void rem_user(uint16_t rnti);
  void add_bearer(uint16_t rnti, uint32_t lcid, srsran::rlc_config_t cnfg);
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid);

  // rlc_interface_pdcp_nr
  void        write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu);
  bool        rb_is_um(uint16_t rnti, uint32_t lcid);
  bool        sdu_queue_is_full(uint16_t rnti, uint32_t lcid);
  const char* get_rb_name(uint32_t lcid);

  // rlc_interface_mac_nr
  int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  //  void read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t* payload);
  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size);

private:
  class user_interface : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
  {
  public:
    void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu);
    void        notify_delivery(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns);
    void        notify_failure(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns);
    void        write_pdu_bcch_bch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_pcch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
    void        max_retx_attempted() final;
    const char* get_rb_name(uint32_t lcid) final;
    uint16_t    rnti;

    srsenb::pdcp_interface_rlc_nr* m_pdcp = nullptr;
    srsenb::rrc_interface_rlc_nr*  m_rrc  = nullptr;
    std::unique_ptr<srsran::rlc>   m_rlc;
    rlc_nr*                        parent = nullptr;
  };

  // args
  srsran::timer_handler* timers = nullptr;
  mac_interface_rlc_nr*  m_mac  = nullptr;
  pdcp_interface_rlc_nr* m_pdcp = nullptr;
  rrc_interface_rlc_nr*  m_rrc  = nullptr;
  srslog::basic_logger&  logger;

  // state
  std::map<uint32_t, user_interface> users;
  std::vector<mch_service_t>         mch_services;
};

} // namespace srsenb

#endif // SRSENB_RLC_NR_H
