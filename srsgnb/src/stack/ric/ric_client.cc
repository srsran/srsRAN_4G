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
ric_client::ric_client(srslog::basic_logger& logger) :
  task_sched(), logger(logger), rx_sockets(), thread("RIC_CLIENT_THREAD")
{
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
    send_e2_setup_request();
    printf("e2 setup request sent\n");
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

bool ric_client::send_e2_setup_request()
{
  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  if (buf == nullptr) {
    // logger.error("Fatal Error: Couldn't allocate buffer for %s.", procedure_name);
    return false;
  }

  e2_ap_pdu_c setup_req_pdu = e2ap_.generate_setup_request();

  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (setup_req_pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();
  printf("try to send %d bytes to addr %s \n", buf->N_bytes, inet_ntoa(ric_addr.sin_addr));
  if (!send_sctp(buf)) {
    printf("failed to send e2 setup request\n");
    return false;
  }
  return true;
}
