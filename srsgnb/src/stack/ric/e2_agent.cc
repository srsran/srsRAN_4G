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

#include "srsgnb/hdr/stack/ric/e2_agent.h"
#include "srsran/asn1/e2ap.h"
#include "srsran/common/standard_streams.h"

using namespace srsenb;

/*********************************************************
 *                     RIC Connection
 *********************************************************/

srsran::proc_outcome_t e2_agent::e2_setup_proc_t::init()
{
  e2_agent_ptr->logger.info("Starting new RIC connection.");
  connect_count++;
  return start_ric_connection();
}

srsran::proc_outcome_t e2_agent::e2_setup_proc_t::start_ric_connection()
{
  if (not e2_agent_ptr->running) {
    e2_agent_ptr->logger.info("E2 Agent is not running anymore.");
    return srsran::proc_outcome_t::error;
  }
  if (e2_agent_ptr->ric_connected) {
    e2_agent_ptr->logger.info("E2 Agent is already connected to RIC");
    return srsran::proc_outcome_t::success;
  }

  auto connect_callback = [this]() {
    bool connected = e2_agent_ptr->connect_ric();

    auto notify_result = [this, connected]() {
      e2_setup_proc_t::e2connectresult res;
      res.success = connected;
      e2_agent_ptr->e2_setup_proc.trigger(res);
    };
    e2_agent_ptr->task_sched.notify_background_task_result(notify_result);
  };
  srsran::get_background_workers().push_task(connect_callback);
  e2_agent_ptr->logger.debug("Connection to RIC requested.");

  return srsran::proc_outcome_t::yield;
}

srsran::proc_outcome_t e2_agent::e2_setup_proc_t::react(const srsenb::e2_agent::e2_setup_proc_t::e2connectresult& event)
{
  if (event.success) {
    e2_agent_ptr->logger.info("Connected to RIC. Sending setup request.");
    e2_agent_ptr->e2_setup_timeout.run();
    if (not e2_agent_ptr->setup_e2()) {
      e2_agent_ptr->logger.error("E2 setup failed. Exiting...");
      srsran::console("E2 setup failed\n");
      e2_agent_ptr->running = false;
      return srsran::proc_outcome_t::error;
    }
    e2_agent_ptr->logger.info("E2 setup request sent. Waiting for response.");
    return srsran::proc_outcome_t::yield;
  }

  e2_agent_ptr->logger.info("Could not connected to RIC. Aborting");
  return srsran::proc_outcome_t::error;
}

srsran::proc_outcome_t e2_agent::e2_setup_proc_t::react(const srsenb::e2_agent::e2_setup_proc_t::e2setupresult& event)
{
  if (e2_agent_ptr->e2_setup_timeout.is_running()) {
    e2_agent_ptr->e2_setup_timeout.stop();
  }
  if (event.success) {
    e2_agent_ptr->logger.info("E2 Setup procedure completed successfully");
    return srsran::proc_outcome_t::success;
  }
  e2_agent_ptr->logger.error("E2 Setup failed.");
  srsran::console("E2 setup failed\n");
  return srsran::proc_outcome_t::error;
}

void e2_agent::e2_setup_proc_t::then(const srsran::proc_state_t& result)
{
  if (result.is_error()) {
    e2_agent_ptr->logger.info("Failed to initiate RIC connection. Attempting reconnection in %d seconds",
                              e2_agent_ptr->ric_connect_timer.duration() / 1000);
    srsran::console("Failed to initiate RIC connection. Attempting reconnection in %d seconds\n",
                    e2_agent_ptr->ric_connect_timer.duration() / 1000);
    e2_agent_ptr->rx_sockets.remove_socket(e2_agent_ptr->ric_socket.get_socket());
    e2_agent_ptr->ric_socket.close();
    e2_agent_ptr->logger.info("R2 Agent socket closed.");
    e2_agent_ptr->ric_connect_timer.run();
    if (e2_agent_ptr->_args.max_ric_setup_retries > 0 && connect_count > e2_agent_ptr->_args.max_ric_setup_retries) {
      srsran_terminate("Error connecting to RIC");
    }
    // Try again with in 10 seconds
  } else {
    connect_count = 0;
  }
}

/*********************************************************
 *                     E2 Agent class
 *********************************************************/

e2_agent::e2_agent(srslog::basic_logger& logger, e2_interface_metrics* _gnb_metrics) :
  task_sched(),
  logger(logger),
  rx_sockets(),
  thread("E2_AGENT_THREAD"),
  e2ap_(logger, this, _gnb_metrics, &task_sched),
  e2_setup_proc(this)
{
  gnb_metrics = _gnb_metrics;
  ric_rece_task_queue = task_sched.make_task_queue();
}

bool e2_agent::init(e2_agent_args_t args)
{
  _args = args;

  // Setup RIC reconnection timer
  ric_connect_timer    = task_sched.get_unique_timer();
  auto ric_connect_run = [this](uint32_t tid) {
    if (e2_setup_proc.is_busy()) {
      logger.error("Failed to initiate RIC Setup procedure: procedure is busy.");
    }
    e2_setup_proc.launch();
  };
  ric_connect_timer.set(_args.ric_connect_timer * 1000, ric_connect_run);
  // Setup timeout
  e2_setup_timeout               = task_sched.get_unique_timer();
  uint32_t ric_setup_timeout_val = 5000;
  e2_setup_timeout.set(ric_setup_timeout_val, [this](uint32_t tid) {
    e2_setup_proc_t::e2setupresult res;
    res.success = false;
    res.cause   = e2_setup_proc_t::e2setupresult::cause_t::timeout;
    e2_setup_proc.trigger(res);
  });

  start(0);
  running = true;
  // starting RIC connection
  if (not e2_setup_proc.launch()) {
    logger.error("Failed to initiate RIC Setup procedure: error launching procedure.");
  }

  return SRSRAN_SUCCESS;
}

void e2_agent::stop()
{
  running = false;
  wait_thread_finish();
}

void e2_agent::tic()
{
  // get tick every 1ms to advance timers
  task_sched.tic();
}

bool e2_agent::is_ric_connected()
{
  return ric_connected;
}

bool e2_agent::connect_ric()
{
  using namespace srsran::net_utils;
  logger.info("Connecting to RIC %s:%d", _args.ric_ip.c_str(), _args.ric_port);
  // Open SCTP socket
  if (not ric_socket.open_socket(addr_family::ipv4, socket_type::seqpacket, protocol_type::SCTP)) {
    return false;
  }

  // Subscribe to shutdown events
  if (not ric_socket.sctp_subscribe_to_events()) {
    ric_socket.close();
    return false;
  }

  // Set SRTO_MAX
  if (not ric_socket.sctp_set_rto_opts(6000)) {
    return false;
  }

  // Set SCTP init options
  if (not ric_socket.sctp_set_init_msg_opts(3, 5000)) {
    return false;
  }

  // Bind socket
  if (not ric_socket.bind_addr(_args.ric_bind_ip.c_str(), _args.ric_bind_port)) {
    ric_socket.close();
    return false;
  }
  logger.info("SCTP socket opened. fd=%d", ric_socket.fd());

  // Connect to the AMF address
  if (not ric_socket.connect_to(_args.ric_ip.c_str(), _args.ric_port, &ric_addr)) {
    ric_socket.close();
    return false;
  }
  logger.info("SCTP socket connected with RIC. fd=%d", ric_socket.fd());

  // Assign a handler to rx RIC packets
  auto rx_callback =
      [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        handle_ric_rx_msg(std::move(pdu), from, sri, flags);
      };
  rx_sockets.add_socket_handler(ric_socket.fd(),
                                srsran::make_sctp_sdu_handler(logger, ric_rece_task_queue, rx_callback));

  logger.info("SCTP socket connected established with RIC");
  return true;
}

bool e2_agent::setup_e2()
{
  return send_e2_msg(E2_SETUP_REQUEST);
}

void e2_agent::run_thread()
{
  while (running) {
    task_sched.run_next_task();
  }
}

bool e2_agent::send_sctp(srsran::unique_byte_buffer_t& buf)
{
  ssize_t ret;
  ret = sctp_sendmsg(ric_socket.fd(),
                     buf->msg,
                     buf->N_bytes,
                     (struct sockaddr*)&ric_addr,
                     sizeof(ric_addr),
                     htonl(e2ap_ppid),
                     0,
                     0,
                     0,
                     0);
  if (ret == -1) {
    printf("failed to send %d bytes\n", buf->N_bytes);
    return false;
  }
  return true;
}

bool e2_agent::send_e2_msg(e2_msg_type_t msg_type)
{
  std::string message_name;
  e2_ap_pdu_c send_pdu;

  switch (msg_type) {
    case e2_msg_type_t::E2_SETUP_REQUEST:
      send_pdu     = e2ap_.generate_setup_request();
      message_name = "E2 SETUP REQUEST";
      break;
    case e2_msg_type_t::E2_RESET:
      send_pdu     = e2ap_.generate_reset_request();
      message_name = "E2 RESET REQUEST";
      break;
    case e2_msg_type_t::E2_RESET_RESPONSE:
      send_pdu     = e2ap_.generate_reset_response();
      message_name = "E2 RESET RESPONSE";
      break;
    default:
      printf("Unknown E2AP message type\n");
      return false;
  }

  return send_e2ap_pdu(send_pdu);
}

bool e2_agent::queue_send_e2ap_pdu(e2_ap_pdu_c e2ap_pdu)
{
  if (not ric_connected) {
    logger.error("Aborting sending msg. Cause: RIC is not connected.");
    return false;
  }

  auto send_e2ap_pdu_task = [this, e2ap_pdu]() { send_e2ap_pdu(e2ap_pdu); };
  ric_rece_task_queue.push(send_e2ap_pdu_task);
  return true;
}

bool e2_agent::send_e2ap_pdu(e2_ap_pdu_c send_pdu)
{
  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  if (buf == nullptr) {
    // logger.error("Fatal Error: Couldn't allocate buffer for %s.", procedure_name);
    return false;
  }
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (send_pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack TX E2 PDU");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();
  printf("try to send %d bytes to addr %s \n", buf->N_bytes, inet_ntoa(ric_addr.sin_addr));
  if (!send_sctp(buf)) {
    logger.error("failed to send");
    return false;
  }
  return true;
}

bool e2_agent::handle_ric_rx_msg(srsran::unique_byte_buffer_t pdu,
                                 const sockaddr_in&           from,
                                 const sctp_sndrcvinfo&       sri,
                                 int                          flags)
{
  // Handle Notification Case
  if (flags & MSG_NOTIFICATION) {
    // Received notification
    union sctp_notification* notification = (union sctp_notification*)pdu->msg;
    logger.info("SCTP Notification %04x", notification->sn_header.sn_type);
    bool restart_e2 = false;
    if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT) {
      logger.info("SCTP Association Shutdown. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
      restart_e2 = true;
    } else if (notification->sn_header.sn_type == SCTP_PEER_ADDR_CHANGE &&
               notification->sn_paddr_change.spc_state == SCTP_ADDR_UNREACHABLE) {
      logger.info("SCTP peer addres unreachable. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP peer address unreachable. Association: %d\n", sri.sinfo_assoc_id);
      restart_e2 = true;
    } else if (notification->sn_header.sn_type == SCTP_REMOTE_ERROR) {
      logger.info("SCTP remote error. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP remote error. Association: %d\n", sri.sinfo_assoc_id);
      restart_e2 = true;
    } else if (notification->sn_header.sn_type == SCTP_ASSOC_CHANGE) {
      logger.info("SCTP association changed. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP association changed. Association: %d\n", sri.sinfo_assoc_id);
    }
    if (restart_e2) {
      logger.info("Restarting E2 connection");
      srsran::console("Restarting E2 connection\n");
      rx_sockets.remove_socket(ric_socket.get_socket());
      ric_socket.close();
    }
  } else if (pdu->N_bytes == 0) {
    logger.error("SCTP return 0 bytes. Closing socket");
    ric_socket.close();
  }

  // Restart RIC connection procedure if we lost connection
  if (not ric_socket.is_open()) {
    ric_connected = false;
    if (e2_setup_proc.is_busy()) {
      logger.error("Failed to initiate RIC connection procedure, as it is already running.");
      return false;
    }
    e2_setup_proc.launch();
    return false;
  }

  if ((flags & MSG_NOTIFICATION) == 0 && pdu->N_bytes != 0) {
    handle_e2_rx_pdu(pdu.get());
  }

  return true;
}

bool e2_agent::handle_e2_rx_pdu(srsran::byte_buffer_t* pdu)
{
  printf("E2_AGENT: Received %d bytes from RIC\n", pdu->N_bytes);
  e2_ap_pdu_c    pdu_c;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (pdu_c.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to unpack RX E2 PDU");
    return false;
  }
  if (pdu_c.type().value == e2_ap_pdu_c::types_opts::init_msg) {
    logger.info("Received E2AP Init Message");
    handle_e2_init_msg(pdu_c.init_msg());
  } else if (pdu_c.type().value == e2_ap_pdu_c::types_opts::successful_outcome) {
    logger.info("Received E2AP Successful Outcome");
    handle_e2_successful_outcome(pdu_c.successful_outcome());
  } else if (pdu_c.type().value == e2_ap_pdu_c::types_opts::unsuccessful_outcome) {
    logger.info("Received E2AP Unsuccessful Outcome ");
    handle_e2_unsuccessful_outcome(pdu_c.unsuccessful_outcome());
  } else {
    logger.warning("Received E2AP Unknown Message ");
  }
  return true;
}

bool e2_agent::handle_e2_init_msg(asn1::e2ap::init_msg_s& init_msg)
{
  using namespace asn1::e2ap;
  if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ricsubscription_request) {
    logger.info("Received E2AP RIC Subscription Request");
    handle_ric_subscription_request(init_msg.value.ricsubscription_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ricsubscription_delete_request) {
    logger.info("Received E2AP RIC Subscription Delete Request");
    handle_ric_subscription_delete_request(init_msg.value.ricsubscription_delete_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ri_cctrl_request) {
    logger.info("Received E2AP RIC Control Request");
    // handle_ri_cctrl_request(init_msg.value.ri_cctrl_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::e2conn_upd) {
    logger.info("Received E2AP E2 Connection Update");
    //handle_e2conn_upd(init_msg.value.e2conn_upd());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::reset_request) {
    logger.info("Received E2AP E2 Reset Request");
    handle_reset_request(init_msg.value.reset_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::e2_removal_request) {
    logger.info("Received E2AP E2 Removal Request");
    //handle_e2_removal_request(init_msg.value.e2_removal_request());
  } else {
    logger.warning("Received E2AP Unknown Init Message ");
  }
  // TODO check for different type of RIC generated init messages
  // eg. RIC subscription request, RIC Reset request, RIC control request, RIC subscription delete request
  return true;
}

bool e2_agent::handle_e2_successful_outcome(asn1::e2ap::successful_outcome_s& successful_outcome)
{
  using namespace asn1::e2ap;
  if (successful_outcome.value.type() == e2_ap_elem_procs_o::successful_outcome_c::types_opts::e2setup_resp) {
    logger.info("Received E2AP E2 Setup Response");
    handle_e2_setup_response(successful_outcome.value.e2setup_resp());
  } else if (successful_outcome.value.type() ==
             e2_ap_elem_procs_o::successful_outcome_c::types_opts::ricsubscription_resp) {
    logger.info("Received E2AP RIC Subscription Response");
    // handle_ric_subscription_response(successful_outcome.value.ric_subscription());
  } else if (successful_outcome.value.type() == e2_ap_elem_procs_o::successful_outcome_c::types_opts::ri_cctrl_ack) {
    logger.info("Received E2AP RIC Control acknowlegement  \n");
    // handle_ric_control_response(successful_outcome.value.ric_control());
  } else if (successful_outcome.value.type() ==
             e2_ap_elem_procs_o::successful_outcome_c::types_opts::ricservice_upd_ack) {
    logger.info("Received E2AP RIC Service Update acknowlegement \n");
    // handle_ric_service_update_ack(successful_outcome.value.ric_service_update());
  } else if (successful_outcome.value.type() ==
             e2_ap_elem_procs_o::successful_outcome_c::types_opts::ricsubscription_delete_resp) {
    logger.info("Received E2AP RIC Subscription Delete Response \n");
    // handle_ric_subscription_delete_response(successful_outcome.value.ric_subscription_delete());
  } else if (successful_outcome.value.type() == e2_ap_elem_procs_o::successful_outcome_c::types_opts::reset_resp) {
    logger.info("Received E2AP RIC Reset Response \n");
    handle_reset_response(successful_outcome.value.reset_resp());
  } else {
    logger.info("Received E2AP Unknown Successful Outcome \n");
  }
  return true;
}

bool e2_agent::handle_e2_setup_response(e2setup_resp_s setup_response)
{
  if (e2ap_.process_setup_response(setup_response)) {
    logger.error("Failed to process E2 Setup Response \n");
    ric_connected = false;
    e2_setup_proc_t::e2setupresult res;
    res.success = false;
    e2_setup_proc.trigger(res);
    return false;
  }

  ric_connected = true;
  e2_setup_proc_t::e2setupresult res;
  res.success = true;
  e2_setup_proc.trigger(res);
  return true;
}

bool e2_agent::handle_e2_unsuccessful_outcome(asn1::e2ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::e2ap;
  if (unsuccessful_outcome.value.type() == e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2setup_fail) {
    logger.info("Received E2AP E2 Setup Failure");
    if (e2ap_.process_e2_setup_failure(unsuccessful_outcome.value.e2setup_fail())) {
      logger.error("Failed to process E2 Setup Failure \n");
      return false;
    }
  } else if (unsuccessful_outcome.value.type() ==
             e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2node_cfg_upd_fail) {
    logger.info("Received E2node configuration update Failure");
    if (e2ap_.process_e2_node_config_update_failure(unsuccessful_outcome.value.e2node_cfg_upd_fail())) {
      logger.error("Failed to process E2node configuration update Failure \n");
      return false;
    }
  } else if (unsuccessful_outcome.value.type() ==
             e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ricservice_upd_fail) {
    logger.info("Received E2AP RIC Service Update Failure \n");
    if (e2ap_.process_ric_service_update_failure(unsuccessful_outcome.value.ricservice_upd_fail())) {
      logger.error("Failed to process RIC service update failure \n");
      return false;
    }
  } else if (unsuccessful_outcome.value.type() ==
             e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2_removal_fail) {
    logger.info("Received E2AP removal Unsuccessful Outcome \n");
    if (e2ap_.process_e2_removal_failure(unsuccessful_outcome.value.e2_removal_fail())) {
      logger.error("Failed to process RIC service status failure \n");
      return false;
    }
  } else {
    logger.info("Received E2AP Unknown Unsuccessful Outcome \n");
  }

  return true;
}

bool e2_agent::handle_ric_subscription_request(ricsubscription_request_s ric_subscription_request)
{
  logger.info("Received RIC Subscription Request from RIC ID: %i (instance id %i) to RAN Function ID: %i",
              ric_subscription_request->ri_crequest_id->ric_requestor_id,
              ric_subscription_request->ri_crequest_id->ric_instance_id,
              ric_subscription_request->ra_nfunction_id->value);

  if (e2ap_.process_subscription_request(ric_subscription_request)) {
    logger.error("Failed to process RIC subscription request \n");
    return false;
  }

  return true;
}

bool e2_agent::handle_ric_subscription_delete_request(ricsubscription_delete_request_s ricsubscription_delete_request)
{
  logger.info("Received RIC Subscription Delete request from RIC ID: %i (instance id %i) to RAN Function ID: %i",
              ricsubscription_delete_request->ri_crequest_id->ric_requestor_id,
              ricsubscription_delete_request->ri_crequest_id->ric_instance_id,
              ricsubscription_delete_request->ra_nfunction_id->value);

  if (e2ap_.process_subscription_delete_request(ricsubscription_delete_request)) {
    logger.error("Failed to process RIC subscription delete request \n");
    return false;
  }

  return true;
}

bool e2_agent::handle_subscription_modification_request(uint32_t ric_subscription_modification_request)
{
  if (e2ap_.process_subscription_modification_request(ric_subscription_modification_request)) {
    logger.error("Failed to process RIC subscription delete request \n");
    return false;
  }
  return true;
}
bool e2_agent::handle_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm)
{
  if (e2ap_.process_subscription_modification_confirm(ric_subscription_modification_confirm)) {
    logger.error("Failed to process RIC subscription delete request \n");
    return false;
  }
  return true;
}
bool e2_agent::handle_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse)
{
  if (e2ap_.process_subscription_modification_refuse(ric_subscription_modification_refuse)) {
    logger.error("Failed to process RIC subscription delete request \n");
    return false;
  }
  return true;
}

bool e2_agent::handle_reset_request(reset_request_s& reset_request)
{
  printf("Received E2AP E2 Reset Request \n");
  // call process to handle reset request, if it fails log error and return false, else return true - success
  if (e2ap_.process_reset_request(reset_request)) {
    logger.error("Failed to process E2 Reset Request \n");
    return false;
  }

  logger.info("Reset transaction with ID = {}", e2ap_.get_reset_id());

  // send reset reset response
  auto send_reset_resp = [this]() { send_e2_msg(E2_RESET_RESPONSE); };
  ric_rece_task_queue.push(send_reset_resp);

  return true;
}

bool e2_agent::handle_reset_response(reset_resp_s& reset_response)
{
  printf("Received E2AP E2 Reset Response \n");
  // call process to handle reset reponse, if it fails log error, else return true - success
  // all processing of message will be done in process_reset_response (e2ap.cc)
  if (e2ap_.process_reset_response(reset_response)) {
    logger.error("Failed to process E2 Reset Response \n");
    return false;
  }

  logger.info("Reset Response successfully processed \n");

  return true;
}
