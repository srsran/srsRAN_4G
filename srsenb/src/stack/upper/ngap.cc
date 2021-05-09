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

#include "srsenb/hdr/stack/upper/ngap.h"

#define procError(fmt, ...) ngap_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) ngap_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procInfo(fmt, ...) ngap_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::ngap_nr;

namespace srsenb {
/*********************************************************
 *                     AMF Connection
 *********************************************************/

srsran::proc_outcome_t ngap::ng_setup_proc_t::init()
{
  procInfo("Starting new AMF connection.");
  return start_amf_connection();
}

srsran::proc_outcome_t ngap::ng_setup_proc_t::start_amf_connection()
{
  if (not ngap_ptr->running) {
    procInfo("NGAP is not running anymore.");
    return srsran::proc_outcome_t::error;
  }
  if (ngap_ptr->amf_connected) {
    procInfo("gNB NGAP is already connected to AMF");
    return srsran::proc_outcome_t::success;
  }

  if (not ngap_ptr->connect_amf()) {
    procInfo("Could not connect to AMF");
    return srsran::proc_outcome_t::error;
  }

  if (not ngap_ptr->setup_ng()) {
    procError("NG setup failed. Exiting...");
    srsran::console("NG setup failed\n");
    ngap_ptr->running = false;
    return srsran::proc_outcome_t::error;
  }

  ngap_ptr->ngsetup_timeout.run();
  procInfo("NGSetupRequest sent. Waiting for response...");
  return srsran::proc_outcome_t::yield;
}

srsran::proc_outcome_t ngap::ng_setup_proc_t::react(const srsenb::ngap::ng_setup_proc_t::ngsetupresult& event)
{
  if (ngap_ptr->ngsetup_timeout.is_running()) {
    ngap_ptr->ngsetup_timeout.stop();
  }
  if (event.success) {
    procInfo("NGSetup procedure completed successfully");
    return srsran::proc_outcome_t::success;
  }
  procError("NGSetup failed.");
  srsran::console("NGsetup failed\n");
  return srsran::proc_outcome_t::error;
}

void ngap::ng_setup_proc_t::then(const srsran::proc_state_t& result) const
{
  if (result.is_error()) {
    procInfo("Failed to initiate NG connection. Attempting reconnection in %d seconds",
             ngap_ptr->amf_connect_timer.duration() / 1000);
    srsran::console("Failed to initiate NG connection. Attempting reconnection in %d seconds\n",
                    ngap_ptr->amf_connect_timer.duration() / 1000);
    ngap_ptr->rx_socket_handler->remove_socket(ngap_ptr->amf_socket.get_socket());
    ngap_ptr->amf_socket.close();
    procInfo("NGAP socket closed.");
    ngap_ptr->amf_connect_timer.run();
    // Try again with in 10 seconds
  }
}

/*********************************************************
 *                     NGAP class
 *********************************************************/

ngap::ngap(srsran::task_sched_handle   task_sched_,
           srslog::basic_logger&       logger,
           srsran::socket_manager_itf* rx_socket_handler_) :
  ngsetup_proc(this), logger(logger), task_sched(task_sched_), rx_socket_handler(rx_socket_handler_)
{
  amf_task_queue = task_sched.make_task_queue();
}

int ngap::init(ngap_args_t args_, rrc_interface_ngap_nr* rrc_)
{
  rrc  = rrc_;
  args = args_;

  build_tai_cgi();

  // Setup AMF reconnection timer
  amf_connect_timer    = task_sched.get_unique_timer();
  auto amf_connect_run = [this](uint32_t tid) {
    if (ngsetup_proc.is_busy()) {
      logger.error("Failed to initiate NGSetup procedure.");
    }
    ngsetup_proc.launch();
  };
  amf_connect_timer.set(10000, amf_connect_run);
  // Setup NGSetup timeout
  ngsetup_timeout              = task_sched.get_unique_timer();
  uint32_t ngsetup_timeout_val = 5000;
  ngsetup_timeout.set(ngsetup_timeout_val, [this](uint32_t tid) {
    ng_setup_proc_t::ngsetupresult res;
    res.success = false;
    res.cause   = ng_setup_proc_t::ngsetupresult::cause_t::timeout;
    ngsetup_proc.trigger(res);
  });

  running = true;
  // starting AMF connection
  if (not ngsetup_proc.launch()) {
    logger.error("Failed to initiate NGSetup procedure.");
  }

  return SRSRAN_SUCCESS;
}

void ngap::stop()
{
  running = false;
  amf_socket.close();
}

bool ngap::is_amf_connected()
{
  return amf_connected;
}

// Generate common NGAP protocol IEs from config args
void ngap::build_tai_cgi()
{
  uint32_t plmn;

  // TAI
  srsran::s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tai.plmn_id.from_number(plmn);

  tai.tac.from_number(args.tac);

  // nr_cgi
  nr_cgi.plmn_id.from_number(plmn);
  // TODO Check how to build nr cell id
  nr_cgi.nrcell_id.from_number((uint32_t)(args.gnb_id << 8) | args.cell_id);
}

/*******************************************************************************
/* NGAP message handlers
********************************************************************************/
bool ngap::handle_amf_rx_msg(srsran::unique_byte_buffer_t pdu,
                             const sockaddr_in&           from,
                             const sctp_sndrcvinfo&       sri,
                             int                          flags)
{
  // Handle Notification Case
  if (flags & MSG_NOTIFICATION) {
    // Received notification
    union sctp_notification* notification = (union sctp_notification*)pdu->msg;
    logger.debug("SCTP Notification %d", notification->sn_header.sn_type);
    if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT) {
      logger.info("SCTP Association Shutdown. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
      rx_socket_handler->remove_socket(amf_socket.get_socket());
      amf_socket.close();
    } else if (notification->sn_header.sn_type == SCTP_PEER_ADDR_CHANGE &&
               notification->sn_paddr_change.spc_state == SCTP_ADDR_UNREACHABLE) {
      logger.info("SCTP peer addres unreachable. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP peer address unreachable. Association: %d\n", sri.sinfo_assoc_id);
      rx_socket_handler->remove_socket(amf_socket.get_socket());
      amf_socket.close();
    }
  } else if (pdu->N_bytes == 0) {
    logger.error("SCTP return 0 bytes. Closing socket");
    amf_socket.close();
  }

  // Restart MME connection procedure if we lost connection
  if (not amf_socket.is_open()) {
    amf_connected = false;
    if (ngsetup_proc.is_busy()) {
      logger.error("Failed to initiate MME connection procedure, as it is already running.");
      return false;
    }
    ngsetup_proc.launch();
    return false;
  }

  handle_ngap_rx_pdu(pdu.get());
  return true;
}

bool ngap::handle_ngap_rx_pdu(srsran::byte_buffer_t* pdu)
{
  // Save message to PCAP
  // if (pcap != nullptr) {
  //   pcap->write_ngap(pdu->msg, pdu->N_bytes);
  // }

  ngap_pdu_c     rx_pdu;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);

  if (rx_pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error(pdu->msg, pdu->N_bytes, "Failed to unpack received PDU");
    cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::transfer_syntax_error;
    // send_error_indication(cause);
    return false;
  }
  // log_ngap_msg(rx_pdu, srsran::make_span(*pdu), true);

  switch (rx_pdu.type().value) {
    // case ngap_pdu_c::types_opts::init_msg:
    // return handle_initiatingmessage(rx_pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return handle_successfuloutcome(rx_pdu.successful_outcome());
    // case ngap_pdu_c::types_opts::unsuccessful_outcome:
    // return handle_unsuccessfuloutcome(rx_pdu.unsuccessful_outcome());
    default:
      logger.error("Unhandled PDU type %d", rx_pdu.type().value);
      return false;
  }

  return true;
}

bool ngap::handle_successfuloutcome(const successful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp:
      return handle_ngsetupresponse(msg.value.ng_setup_resp());
    default:
      logger.error("Unhandled successful outcome message: %s", msg.value.type().to_string());
  }
  return true;
}

bool ngap::handle_ngsetupresponse(const asn1::ngap_nr::ng_setup_resp_s& msg)
{
  ngsetupresponse = msg;
  amf_connected   = true;
  ng_setup_proc_t::ngsetupresult res;
  res.success = true;
  ngsetup_proc.trigger(res);
  return true;
}

/*******************************************************************************
/* NGAP connection helpers
********************************************************************************/

bool ngap::connect_amf()
{
  using namespace srsran::net_utils;
  logger.info("Connecting to AMF %s:%d", args.amf_addr.c_str(), int(AMF_PORT));

  // Init SCTP socket and bind it
  if (not sctp_init_client(&amf_socket, socket_type::seqpacket, args.ngc_bind_addr.c_str())) {
    return false;
  }
  logger.info("SCTP socket opened. fd=%d", amf_socket.fd());

  // Connect to the AMF address
  if (not amf_socket.connect_to(args.amf_addr.c_str(), AMF_PORT, &amf_addr)) {
    return false;
  }
  logger.info("SCTP socket connected with AMF. fd=%d", amf_socket.fd());

  // Assign a handler to rx AMF packets
  auto rx_callback =
      [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        // Defer the handling of AMF packet to eNB stack main thread
        handle_amf_rx_msg(std::move(pdu), from, sri, flags);
      };
  rx_socket_handler->add_socket_handler(amf_socket.fd(),
                                        srsran::make_sctp_sdu_handler(logger, amf_task_queue, rx_callback));

  logger.info("SCTP socket established with AMF");
  return true;
}

bool ngap::setup_ng()
{
  uint32_t tmp32;
  uint16_t tmp16;

  uint32_t plmn;
  srsran::s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  plmn = htonl(plmn);

  ngap_pdu_c pdu;
  pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_NG_SETUP);
  ng_setup_request_ies_container& container     = pdu.init_msg().value.ng_setup_request().protocol_ies;
  global_gnb_id_s&                global_gnb_id = container.global_ran_node_id.value.set_global_gnb_id();
  global_gnb_id.plmn_id                         = tai.plmn_id;
  // TODO: when ASN1 is fixed
  // global_gnb_id.gnb_id.set_gnb_id().from_number(args.gnb_id);

  // container.ran_node_name_present = true;
  // container.ran_node_name.value.from_string(args.gnb_name);

  asn1::bounded_bitstring<22, 32, false, true>& gnb_str = global_gnb_id.gnb_id.set_gnb_id();
  gnb_str.resize(32);
  uint8_t       buffer[4];
  asn1::bit_ref bref(&buffer[0], sizeof(buffer));
  bref.pack(args.gnb_id, 8);
  memcpy(gnb_str.data(), &buffer[0], bref.distance_bytes());

  //  .from_number(args.gnb_id);

  container.ran_node_name_present = true;
  if (args.gnb_name.length() >= 150) {
    args.gnb_name.resize(150);
  }
  //  container.ran_node_name.value.from_string(args.enb_name);
  container.ran_node_name.value.resize(args.gnb_name.size());
  memcpy(&container.ran_node_name.value[0], &args.gnb_name[0], args.gnb_name.size());

  container.supported_ta_list.value.resize(1);
  container.supported_ta_list.value[0].tac = tai.tac;
  container.supported_ta_list.value[0].broadcast_plmn_list.resize(1);
  container.supported_ta_list.value[0].broadcast_plmn_list[0].plmn_id = tai.plmn_id;
  container.supported_ta_list.value[0].broadcast_plmn_list[0].tai_slice_support_list.resize(1);
  container.supported_ta_list.value[0].broadcast_plmn_list[0].tai_slice_support_list[0].s_nssai.sst.from_number(1);

  container.default_paging_drx.value.value = asn1::ngap_nr::paging_drx_opts::v256; // Todo: add to args, config file

  return sctp_send_ngap_pdu(pdu, 0, "ngSetupRequest");
}

/*******************************************************************************
/* General helpers
********************************************************************************/

bool ngap::sctp_send_ngap_pdu(const asn1::ngap_nr::ngap_pdu_c& tx_pdu, uint32_t rnti, const char* procedure_name)
{
  if (not amf_connected and rnti != SRSRAN_INVALID_RNTI) {
    logger.error("Aborting %s for rnti=0x%x. Cause: AMF is not connected.", procedure_name, rnti);
    return false;
  }

  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  if (buf == nullptr) {
    logger.error("Fatal Error: Couldn't allocate buffer for %s.", procedure_name);
    return false;
  }
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (tx_pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack TX PDU %s", procedure_name);
    return false;
  }
  buf->N_bytes = bref.distance_bytes();

  // TODO: when we got pcap support
  // Save message to PCAP
  // if (pcap != nullptr) {
  //   pcap->write_s1ap(buf->msg, buf->N_bytes);
  // }

  if (rnti != SRSRAN_INVALID_RNTI) {
    logger.info(buf->msg, buf->N_bytes, "Tx S1AP SDU, %s, rnti=0x%x", procedure_name, rnti);
  } else {
    logger.info(buf->msg, buf->N_bytes, "Tx S1AP SDU, %s", procedure_name);
  }
  // TODO: when user list is ready
  // uint16_t streamid = rnti == SRSRAN_INVALID_RNTI ? NONUE_STREAM_ID : users.find_ue_rnti(rnti)->stream_id;
  uint16_t streamid = 0;
  ssize_t  n_sent   = sctp_sendmsg(amf_socket.fd(),
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&amf_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                streamid,
                                0,
                                0);
  if (n_sent == -1) {
    if (rnti != SRSRAN_INVALID_RNTI) {
      logger.error("Error: Failure at Tx S1AP SDU, %s, rnti=0x%x", procedure_name, rnti);
    } else {
      logger.error("Error: Failure at Tx S1AP SDU, %s", procedure_name);
    }
    return false;
  }
  return true;
}

} // namespace srsenb
