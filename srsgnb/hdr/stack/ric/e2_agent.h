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

#ifndef E2_AGENT_H
#define E2_AGENT_H

#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/e2_metrics_interface.h"
#include "srsran/srsran.h"

static const int e2ap_ppid = 70;

enum e2_msg_type_t {
  E2_SETUP_REQUEST,
  E2_SUB_RESPONSE,
  E2_SUB_DEL_RESPONSE,
  E2_INDICATION,
  E2_RESET,
  E2_RESET_RESPONSE
};

struct e2_agent_args_t {
  bool        enable;
  std::string ric_ip;
  uint32_t    ric_port;
  std::string ric_bind_ip;
  uint32_t    ric_bind_port;
  int32_t     max_ric_setup_retries;
  uint32_t    ric_connect_timer;
};

namespace srsenb {
class e2_agent : public srsran::thread
{
public:
  e2_agent(srslog::basic_logger& logger, srsenb::e2_interface_metrics* _gnb_metrics);
  ~e2_agent() = default;

  // Initiate and Stop
  bool init(e2_agent_args_t args);
  void stop();
  void run_thread();
  void tic();
  bool is_ric_connected();

  // Send messages to RIC
  bool send_sctp(srsran::unique_byte_buffer_t& buf);
  bool send_e2_msg(e2_msg_type_t msg_type);
  bool queue_send_e2ap_pdu(e2_ap_pdu_c send_pdu);
  bool send_e2ap_pdu(e2_ap_pdu_c send_pdu);
  bool send_reset_response();

  // Handle messages received from RIC
  bool
  handle_ric_rx_msg(srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags);
  bool handle_e2_rx_pdu(srsran::byte_buffer_t* pdu);
  bool handle_e2_init_msg(asn1::e2ap::init_msg_s& init_msg);
  bool handle_e2_successful_outcome(asn1::e2ap::successful_outcome_s& successful_outcome);
  bool handle_e2_unsuccessful_outcome(asn1::e2ap::unsuccessful_outcome_s& unsuccessful_outcome);
  bool handle_e2_setup_response(e2setup_resp_s setup_response);
  bool handle_ric_subscription_request(ricsubscription_request_s ric_subscription_request);
  bool handle_ric_subscription_delete_request(ricsubscription_delete_request_s ricsubscription_delete_request);
  bool handle_subscription_modification_request(uint32_t ric_subscription_modification_request);
  bool handle_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm);
  bool handle_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse);

  bool handle_reset_response(reset_resp_s& reset_response);
  bool handle_reset_request(reset_request_s& reset_request);

private:
  bool connect_ric();
  bool setup_e2();

  e2_agent_args_t           _args;
  srsran::task_scheduler    task_sched;
  srsran::task_queue_handle ric_rece_task_queue;
  srsran::unique_socket     ric_socket;
  srsran::socket_manager    rx_sockets;
  srslog::basic_logger&     logger;
  struct sockaddr_in        ric_addr      = {}; // RIC address
  bool                      running       = false;
  bool                      ric_connected = false;
  srsran::unique_timer      ric_connect_timer;
  srsran::unique_timer      e2_setup_timeout;

  srsenb::e2_interface_metrics* gnb_metrics = nullptr;
  e2ap                          e2ap_;

  // procedures
  class e2_setup_proc_t
  {
  public:
    struct e2setupresult {
      bool success = false;
      enum class cause_t { timeout, failure } cause;
    };
    struct e2connectresult {
      bool success = false;
    };

    explicit e2_setup_proc_t(e2_agent* e2_agent_) : e2_agent_ptr(e2_agent_) {}
    srsran::proc_outcome_t init();
    srsran::proc_outcome_t step() { return srsran::proc_outcome_t::yield; }
    srsran::proc_outcome_t react(const e2connectresult& event);
    srsran::proc_outcome_t react(const e2setupresult& event);
    void                   then(const srsran::proc_state_t& result);
    const char*            name() const { return "RIC Connection"; }
    uint16_t               connect_count = 0;

  private:
    srsran::proc_outcome_t start_ric_connection();

    e2_agent* e2_agent_ptr = nullptr;
  };

  srsran::proc_t<e2_setup_proc_t> e2_setup_proc;
};
} // namespace srsenb

#endif /* E2_AGENT_H */
