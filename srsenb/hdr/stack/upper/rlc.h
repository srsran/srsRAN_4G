/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/rlc/rlc.h"
#include "srsran/srslog/srslog.h"
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

class rrc_interface_rlc;
class pdcp_interface_rlc;
class mac_interface_rlc;

class rlc : public rlc_interface_mac, public rlc_interface_rrc, public rlc_interface_pdcp
{
public:
  explicit rlc(srslog::basic_logger& logger) : logger(logger) {}
  void
  init(pdcp_interface_rlc* pdcp_, rrc_interface_rlc* rrc_, mac_interface_rlc* mac_, srsran::timer_handler* timers_);
  void stop();
  void get_metrics(rlc_metrics_t& m, const uint32_t nof_tti);

  // rlc_interface_rrc
  void clear_buffer(uint16_t rnti);
  void add_user(uint16_t rnti);
  void rem_user(uint16_t rnti);
  void add_bearer(uint16_t rnti, uint32_t lcid, const srsran::rlc_config_t& cnfg);
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid);
  void del_bearer(uint16_t rnti, uint32_t lcid);
  bool has_bearer(uint16_t rnti, uint32_t lcid);
  bool suspend_bearer(uint16_t rnti, uint32_t lcid);
  bool resume_bearer(uint16_t rnti, uint32_t lcid);
  bool is_suspended(uint16_t rnti, uint32_t lcid);
  void reestablish(uint16_t rnti) final;

  // rlc_interface_pdcp
  void        write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu);
  void        discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t discard_sn);
  bool        rb_is_um(uint16_t rnti, uint32_t lcid);
  const char* get_rb_name(uint32_t lcid);
  bool        sdu_queue_is_full(uint16_t rnti, uint32_t lcid);

  // rlc_interface_mac
  int  read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);

private:
  class user_interface : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
  {
  public:
    void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu);
    void        notify_delivery(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn);
    void        notify_failure(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn);
    void        write_pdu_bcch_bch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_pcch(srsran::unique_byte_buffer_t sdu);
    void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
    void        max_retx_attempted();
    void        protocol_failure();
    const char* get_rb_name(uint32_t lcid);
    uint16_t    rnti;

    srsenb::pdcp_interface_rlc*  pdcp;
    srsenb::rrc_interface_rlc*   rrc;
    unique_rnti_ptr<srsran::rlc> rlc;
    srsenb::rlc*                 parent;
  };

  void update_bsr(uint32_t rnti, uint32_t lcid, uint32_t tx_queue, uint32_t retx_queue);

  pthread_rwlock_t rwlock;

  std::map<uint32_t, user_interface> users;
  std::vector<mch_service_t>         mch_services;

  mac_interface_rlc*     mac  = nullptr;
  pdcp_interface_rlc*    pdcp = nullptr;
  rrc_interface_rlc*     rrc  = nullptr;
  srslog::basic_logger&  logger;
  srsran::timer_handler* timers = nullptr;
};

} // namespace srsenb

#endif // SRSENB_RLC_H
