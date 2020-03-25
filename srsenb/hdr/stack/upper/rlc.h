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

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc.h"
#include <map>

#ifndef SRSENB_RLC_H
#define SRSENB_RLC_H

typedef struct {
  uint32_t lcid;
  uint32_t plmn;
  uint16_t mtch_stop;
  uint8_t* payload;
} mch_service_t;

namespace srsenb {

class rlc : public rlc_interface_mac, public rlc_interface_rrc, public rlc_interface_pdcp
{
public:
  void init(pdcp_interface_rlc*    pdcp_,
            rrc_interface_rlc*     rrc_,
            mac_interface_rlc*     mac_,
            srslte::timer_handler* timers_,
            srslte::log_ref        log_h);
  void stop();

  // rlc_interface_rrc
  void clear_buffer(uint16_t rnti);
  void add_user(uint16_t rnti);
  void rem_user(uint16_t rnti);
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg);
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid);
  bool has_bearer(uint16_t rnti, uint32_t lcid);
  bool suspend_bearer(uint16_t rnti, uint32_t lcid);
  bool resume_bearer(uint16_t rnti, uint32_t lcid);

  // rlc_interface_pdcp
  void        write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu);
  void        discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t discard_sn);
  bool        rb_is_um(uint16_t rnti, uint32_t lcid);
  std::string get_rb_name(uint32_t lcid);

  // rlc_interface_mac
  int  read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size);

private:
  class user_interface : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
  {
  public:
    void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_pcch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}
    void        max_retx_attempted();
    std::string get_rb_name(uint32_t lcid);
    uint16_t    rnti;

    srsenb::pdcp_interface_rlc*  pdcp;
    srsenb::rrc_interface_rlc*   rrc;
    std::unique_ptr<srslte::rlc> rlc;
    srsenb::rlc*                 parent;
  };

  pthread_rwlock_t rwlock;

  std::map<uint32_t, user_interface> users;
  std::vector<mch_service_t>         mch_services;

  mac_interface_rlc*        mac;
  pdcp_interface_rlc*       pdcp;
  rrc_interface_rlc*        rrc;
  srslte::log_ref           log_h;
  srslte::byte_buffer_pool* pool;
  srslte::timer_handler*    timers;
};

} // namespace srsenb

#endif // SRSENB_RLC_H
