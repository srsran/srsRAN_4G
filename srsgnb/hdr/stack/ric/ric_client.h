/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 *
 */

#ifndef RIC_CLIENT_H
#define RIC_CLIENT_H

#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/e2_metrics_interface.h"
#include "srsran/srsran.h"
static const int e2ap_ppid = 70;
static const int e2ap_port = 36422;

enum e2_msg_type_t { E2_SETUP_REQUEST, E2_SUB_RESPONSE, E2_INDICATION, E2_RESET, E2_RESET_RESPONSE };

namespace srsenb {
class ric_client : public srsran::thread
{
public:
  ric_client(srslog::basic_logger& logger, srsenb::e2_interface_metrics* _gnb_metrics);

  // Initiate and Stop
  bool init();
  void stop();
  void run_thread();

  // Send messages to RIC
  bool send_sctp(srsran::unique_byte_buffer_t& buf);
  bool send_e2_msg(e2_msg_type_t msg_type);
  bool send_reset_response();

  // Handle messages received from RIC
  bool
       handle_e2_rx_msg(srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags);
  bool handle_e2_init_msg(asn1::e2ap::init_msg_s& init_msg);
  bool handle_e2_successful_outcome(asn1::e2ap::successful_outcome_s& successful_outcome);
  bool handle_e2_unsuccessful_outcome(asn1::e2ap::unsuccessful_outcome_s& unsuccessful_outcome);
  bool handle_e2_setup_response(e2setup_resp_s setup_response);
  bool handle_ric_subscription_request(ricsubscription_request_s ric_subscription_request);
  bool handle_reset_response(reset_resp_s& reset_response);
  bool handle_reset_request(reset_request_s& reset_request);

private:
  e2ap                      e2ap_;
  srsran::unique_socket     ric_socket;
  srsran::task_queue_handle ric_rece_task_queue;
  srsran::task_scheduler    task_sched;
  srsran::socket_manager    rx_sockets;
  srslog::basic_logger&     logger;
  struct sockaddr_in        ric_addr = {}; // RIC address
  bool                      running  = false;
  srsenb::e2_interface_metrics* gnb_metrics = nullptr;
};
} // namespace srsenb

#endif /* RIC_CLIENT_H */
