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
 */

#include "srsgnb/hdr/stack/ric/ric_client.h"
#include "srsran/asn1/e2ap.h"
#include "stdint.h"

using namespace srsenb;
ric_client::ric_client(srslog::basic_logger& logger, e2_interface_metrics* _gnb_metrics) :
  task_sched(), logger(logger), rx_sockets(), thread("RIC_CLIENT_THREAD"), e2ap_(logger, _gnb_metrics)
{
  gnb_metrics = _gnb_metrics;
}
bool ric_client::init()
{
  printf("RIC_CLIENT: Init\n");
  using namespace srsran::net_utils;
  // Open SCTP socket
  if (not ric_socket.open_socket(addr_family::ipv4, socket_type::seqpacket, protocol_type::SCTP)) {
    return false;
  }
  printf("RIC SCTP socket opened. fd=%d\n", ric_socket.fd());
  if (not ric_socket.sctp_subscribe_to_events()) {
    ric_socket.close();
    return false;
  }

  // Bind socket

  if (not ric_socket.bind_addr("172.17.0.3", 36422)) {
    ric_socket.close();
    return false;
  }

  // Connect to the AMF address
  if (not ric_socket.connect_to("10.104.149.217", e2ap_port, &ric_addr)) {
    return false;
  }
  // Assign a handler to rx RIC packets
  ric_rece_task_queue = task_sched.make_task_queue();
  auto rx_callback =
      [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        handle_e2_rx_msg(std::move(pdu), from, sri, flags);
      };
  rx_sockets.add_socket_handler(ric_socket.fd(),
                                srsran::make_sctp_sdu_handler(logger, ric_rece_task_queue, rx_callback));

  printf("SCTP socket connected with RIC. fd=%d", ric_socket.fd());
  running = true;
  start(0);
  return SRSRAN_SUCCESS;
}
void ric_client::stop()
{
  running = false;
  wait_thread_finish();
}
void ric_client::run_thread()
{
  while (running) {
    if (!e2ap_.has_setup_response()) {
      send_e2_msg(E2_SETUP_REQUEST);
      printf("e2 setup request sent\n");
    }
    sleep(1);
    task_sched.run_next_task();
    sleep(5);
  }
}
bool ric_client::send_sctp(srsran::unique_byte_buffer_t& buf)
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

bool ric_client::send_e2_msg(e2_msg_type_t msg_type)
{
  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  if (buf == nullptr) {
    // logger.error("Fatal Error: Couldn't allocate buffer for %s.", procedure_name);
    return false;
  }
  e2_ap_pdu_c send_pdu;
  switch (msg_type) {
    case e2_msg_type_t::E2_SETUP_REQUEST:
      send_pdu = e2ap_.generate_setup_request();
      break;
    case e2_msg_type_t::E2_SUB_RESPONSE:
      send_pdu = e2ap_.generate_subscription_response();
      break;
    case e2_msg_type_t::E2_INDICATION:
      // TODO create E2 INDICATION generation
      break;
    default:
      printf("Unknown E2AP message type\n");
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
    logger.error("failed to send e2 setup request");
    return false;
  }
  return true;
}

bool ric_client::handle_e2_rx_msg(srsran::unique_byte_buffer_t pdu,
                                  const sockaddr_in&           from,
                                  const sctp_sndrcvinfo&       sri,
                                  int                          flags)
{
  printf("RIC_CLIENT: Received %d bytes from %s\n", pdu->N_bytes, inet_ntoa(from.sin_addr));
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
    // handle_e2_unsuccessful_outcome(pdu_c.unsuccessful_outcome());
  } else {
    logger.warning("Received E2AP Unknown Message ");
  }
  return true;
}

bool ric_client::handle_e2_init_msg(asn1::e2ap::init_msg_s& init_msg)
{
  using namespace asn1::e2ap;
  if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ricsubscription_request) {
    logger.info("Received E2AP RIC Subscription Request");
    handle_ric_subscription_request(init_msg.value.ricsubscription_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ricsubscription_delete_request) {
    logger.info("Received E2AP RIC Subscription Delete Request");
    // handle_ric_subscription_delete_request(init_msg.value.ricsubscription_delete_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::ri_cctrl_request) {
    logger.info("Received E2AP RIC Control Request");
    // handle_ri_cctrl_request(init_msg.value.ri_cctrl_request());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::e2conn_upd) {
    logger.info("Received E2AP E2 Connection Update");
    //handle_e2conn_upd(init_msg.value.e2conn_upd());
  } else if (init_msg.value.type() == e2_ap_elem_procs_o::init_msg_c::types_opts::reset_request) {
    logger.info("Received E2AP E2 Reset Request");
    //handle_reset_request(init_msg.value.reset_request());
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

bool ric_client::handle_e2_successful_outcome(asn1::e2ap::successful_outcome_s& successful_outcome)
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
  } else if (successful_outcome.value.type() == e2_ap_elem_procs_o::successful_outcome_c::types_opts::e2_removal_resp) {
    logger.info("Received E2AP RIC Service Status Successful Outcome \n");
    // handle_e2_removal_response(successful_outcome.value.e2_removal());
  } else {
    logger.info("Received E2AP Unknown Successful Outcome \n");
  }
  return true;
}

bool ric_client::handle_e2_setup_response(e2setup_resp_s setup_response)
{
  if (e2ap_.process_setup_response(setup_response)) {
    logger.error("Failed to process E2 Setup Response");
    return false;
  }
  return true;
}

bool ric_client::handle_e2_unsuccessful_outcome(asn1::e2ap::unsuccessful_outcome_s& unsuccessful_outcome)
{
  using namespace asn1::e2ap;
  // TODO check for different type of RIC generated unsuccessful outcomes
  // eg. RIC subscription failure, RIC Reset failure, RIC control failure, RIC subscription delete failure
  return true;
}

bool ric_client::handle_ric_subscription_request(ricsubscription_request_s ric_subscription_request)
{
    auto send_sub_resp =
      [this]() {
        send_e2_msg(E2_SUB_RESPONSE);
      };
    ric_rece_task_queue.push(send_sub_resp);
  // TODO handle RIC subscription request
  return true;
}
