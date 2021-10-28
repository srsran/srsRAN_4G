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

#include "srsenb/hdr/stack/ngap/ngap.h"
#include "srsenb/hdr/stack/ngap/ngap_ue.h"
#include "srsran/common/int_helpers.h"

using srsran::s1ap_mccmnc_to_plmn;
using srsran::uint32_to_uint8;

#define procError(fmt, ...) ngap_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) ngap_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procInfo(fmt, ...) ngap_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

#define WarnUnsupportFeature(cond, featurename)                                                                        \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      logger.warning("Not handling feature - %s", featurename);                                                        \
    }                                                                                                                  \
  } while (0)

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
    srsran::console("NG connection successful\n");
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

ngap::~ngap() {}

int ngap::init(const ngap_args_t& args_, rrc_interface_ngap_nr* rrc_, gtpu_interface_rrc* gtpu_)
{
  rrc  = rrc_;
  args = args_;
  gtpu = gtpu_;

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

void ngap::get_metrics(ngap_metrics_t& m)
{
  if (!running) {
    m.status = ngap_error;
    return;
  }
  if (amf_connected) {
    m.status = ngap_connected;
  } else {
    m.status = ngap_attaching;
  }
}

void ngap::get_args(ngap_args_t& args_)
{
  args_ = args;
}

bool ngap::is_amf_connected()
{
  return amf_connected;
}

// Generate common NGAP protocol IEs from config args
int ngap::build_tai_cgi()
{
  uint32_t plmn;
  uint8_t  shift;

  // TAI
  srsran::s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tai.plmn_id.from_number(plmn);
  tai.tac.from_number(args.tac);

  // NR CGI
  nr_cgi.plmn_id.from_number(plmn);

  // NR CELL ID (36 bits) = gnb_id (22...32 bits) + cell_id (4...14 bits)
  if (((uint8_t)log2(args.gnb_id) + (uint8_t)log2(args.cell_id) + 2) > 36) {
    logger.error("gNB ID and Cell ID combination greater than 36 bits");
    return SRSRAN_ERROR;
  }
  // Consider moving sanity checks into the parsing function of the configs.
  if (((uint8_t)log2(args.gnb_id) + 1) < 22) {
    shift = 14;
  } else {
    shift = 36 - ((uint8_t)log2(args.gnb_id) + 1);
  }

  nr_cgi.nrcell_id.from_number((uint64_t)args.gnb_id << shift | args.cell_id);
  return SRSRAN_SUCCESS;
}

/*******************************************************************************
/* RRC interface
********************************************************************************/
void ngap::initial_ue(uint16_t                                rnti,
                      uint32_t                                gnb_cc_idx,
                      asn1::ngap_nr::rrcestablishment_cause_e cause,
                      srsran::unique_byte_buffer_t            pdu)
{
  std::unique_ptr<ue> ue_ptr{new ue{this, rrc, gtpu, logger}};
  ue_ptr->ctxt.rnti       = rnti;
  ue_ptr->ctxt.gnb_cc_idx = gnb_cc_idx;
  ue* u                   = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    logger.error("Failed to add rnti=0x%x", rnti);
    return;
  }
  u->send_initial_ue_message(cause, std::move(pdu), false);
}

void ngap::initial_ue(uint16_t                                rnti,
                      uint32_t                                gnb_cc_idx,
                      asn1::ngap_nr::rrcestablishment_cause_e cause,
                      srsran::unique_byte_buffer_t            pdu,
                      uint32_t                                s_tmsi)
{
  std::unique_ptr<ue> ue_ptr{new ue{this, rrc, gtpu, logger}};
  ue_ptr->ctxt.rnti       = rnti;
  ue_ptr->ctxt.gnb_cc_idx = gnb_cc_idx;
  ue* u                   = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    logger.error("Failed to add rnti=0x%x", rnti);
    return;
  }
  u->send_initial_ue_message(cause, std::move(pdu), true, s_tmsi);
}

void ngap::ue_notify_rrc_reconf_complete(uint16_t rnti, bool outcome)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return;
  }
  u->notify_rrc_reconf_complete(outcome);
}

void ngap::write_pdu(uint16_t rnti, srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Received RRC SDU");

  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    logger.info("The rnti=0x%x does not exist", rnti);
    return;
  }
  u->send_ul_nas_transport(std::move(pdu));
}

/*********************************************************
 *              ngap::user_list class
 *********************************************************/

ngap::ue* ngap::user_list::find_ue_rnti(uint16_t rnti)
{
  if (rnti == SRSRAN_INVALID_RNTI) {
    return nullptr;
  }
  auto it = std::find_if(
      users.begin(), users.end(), [rnti](const user_list::pair_type& v) { return v.second->ctxt.rnti == rnti; });
  return it != users.end() ? it->second.get() : nullptr;
}

ngap::ue* ngap::user_list::find_ue_gnbid(uint32_t gnbid)
{
  auto it = users.find(gnbid);
  return (it != users.end()) ? it->second.get() : nullptr;
}

ngap::ue* ngap::user_list::find_ue_amfid(uint32_t amfid)
{
  auto it = std::find_if(users.begin(), users.end(), [amfid](const user_list::pair_type& v) {
    return v.second->ctxt.amf_ue_ngap_id == amfid;
  });
  return it != users.end() ? it->second.get() : nullptr;
}

ngap::ue* ngap::user_list::add_user(std::unique_ptr<ngap::ue> user)
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("NGAP");
  // Check for ID repetitions
  if (find_ue_rnti(user->ctxt.rnti) != nullptr) {
    logger.error("The user to be added with rnti=0x%x already exists", user->ctxt.rnti);
    return nullptr;
  }
  if (find_ue_gnbid(user->ctxt.ran_ue_ngap_id) != nullptr) {
    logger.error("The user to be added with ran ue ngap id=%d already exists", user->ctxt.ran_ue_ngap_id);
    return nullptr;
  }
  if (user->ctxt.amf_ue_ngap_id.has_value() and find_ue_amfid(user->ctxt.amf_ue_ngap_id.value()) != nullptr) {
    logger.error("The user to be added with amf id=%d already exists", user->ctxt.amf_ue_ngap_id.value());
    return nullptr;
  }
  auto p = users.insert(std::make_pair(user->ctxt.ran_ue_ngap_id, std::move(user)));
  return p.second ? p.first->second.get() : nullptr;
}

void ngap::user_list::erase(ue* ue_ptr)
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("NGAP");
  auto                         it     = users.find(ue_ptr->ctxt.ran_ue_ngap_id);
  if (it == users.end()) {
    logger.error("User to be erased does not exist");
    return;
  }
  users.erase(it);
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

  // Restart AMF connection procedure if we lost connection
  if (not amf_socket.is_open()) {
    amf_connected = false;
    if (ngsetup_proc.is_busy()) {
      logger.error("Failed to initiate AMF connection procedure, as it is already running.");
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
  // TODO:
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
    send_error_indication(cause);
    return false;
  }

  switch (rx_pdu.type().value) {
    case ngap_pdu_c::types_opts::init_msg:
      return handle_initiating_message(rx_pdu.init_msg());
    case ngap_pdu_c::types_opts::successful_outcome:
      return handle_successful_outcome(rx_pdu.successful_outcome());
    case ngap_pdu_c::types_opts::unsuccessful_outcome:
      return handle_unsuccessful_outcome(rx_pdu.unsuccessful_outcome());
    default:
      logger.error("Unhandled PDU type %d", rx_pdu.type().value);
      return false;
  }

  return true;
}

bool ngap::handle_initiating_message(const asn1::ngap_nr::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport:
      return handle_dl_nas_transport(msg.value.dl_nas_transport());
    case ngap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request:
      return handle_initial_ctxt_setup_request(msg.value.init_context_setup_request());
    case ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      return handle_ue_ctxt_release_cmd(msg.value.ue_context_release_cmd());
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_setup_request:
      return handle_ue_pdu_session_res_setup_request(msg.value.pdu_session_res_setup_request());
    default:
      logger.error("Unhandled initiating message: %s", msg.value.type().to_string());
  }
  return true;
}

bool ngap::handle_successful_outcome(const successful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp:
      return handle_ng_setup_response(msg.value.ng_setup_resp());
    default:
      logger.error("Unhandled successful outcome message: %s", msg.value.type().to_string());
  }
  return true;
}

bool ngap::handle_unsuccessful_outcome(const asn1::ngap_nr::unsuccessful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail:
      return handle_ng_setup_failure(msg.value.ng_setup_fail());
    default:
      logger.error("Unhandled unsuccessful outcome message: %s", msg.value.type().to_string());
  }
  return true;
}

bool ngap::handle_ng_setup_response(const asn1::ngap_nr::ng_setup_resp_s& msg)
{
  ngsetupresponse = msg;
  amf_connected   = true;
  ng_setup_proc_t::ngsetupresult res;
  res.success = true;
  logger.info("AMF name: %s", ngsetupresponse.protocol_ies.amf_name.value.to_string());
  ngsetup_proc.trigger(res);

  return true;
}

bool ngap::handle_ng_setup_failure(const asn1::ngap_nr::ng_setup_fail_s& msg)
{
  std::string cause = get_cause(msg.protocol_ies.cause.value);
  logger.error("NG Setup Failure. Cause: %s", cause.c_str());
  srsran::console("NG Setup Failure. Cause: %s\n", cause.c_str());
  return true;
}

bool ngap::handle_dl_nas_transport(const asn1::ngap_nr::dl_nas_transport_s& msg)
{
  if (msg.ext) {
    logger.warning("Not handling NGAP message extension");
  }
  ue* u =
      handle_ngapmsg_ue_id(msg.protocol_ies.ran_ue_ngap_id.value.value, msg.protocol_ies.amf_ue_ngap_id.value.value);

  if (u == nullptr) {
    logger.warning("Couldn't find user with ran_ue_ngap_id %d and %d",
                   msg.protocol_ies.ran_ue_ngap_id.value.value,
                   msg.protocol_ies.amf_ue_ngap_id.value.value);
    return false;
  }

  if (msg.protocol_ies.old_amf_present) {
    logger.warning("Not handling OldAMF");
  }

  if (msg.protocol_ies.ran_paging_prio_present) {
    logger.warning("Not handling RANPagingPriority");
  }

  if (msg.protocol_ies.mob_restrict_list_present) {
    logger.warning("Not handling MobilityRestrictionList");
  }

  if (msg.protocol_ies.idx_to_rfsp_present) {
    logger.warning("Not handling IndexToRFSP");
  }

  if (msg.protocol_ies.ue_aggregate_maximum_bit_rate_present) {
    logger.warning("Not handling UEAggregateMaximumBitRate");
  }

  if (msg.protocol_ies.allowed_nssai_present) {
    logger.warning("Not handling AllowedNSSAI");
  }

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Fatal Error: Couldn't allocate buffer in ngap::run_thread().");
    return false;
  }
  memcpy(pdu->msg, msg.protocol_ies.nas_pdu.value.data(), msg.protocol_ies.nas_pdu.value.size());
  pdu->N_bytes = msg.protocol_ies.nas_pdu.value.size();
  rrc->write_dl_info(u->ctxt.rnti, std::move(pdu));
  return true;
}

bool ngap::handle_initial_ctxt_setup_request(const asn1::ngap_nr::init_context_setup_request_s& msg)
{
  ue* u =
      handle_ngapmsg_ue_id(msg.protocol_ies.ran_ue_ngap_id.value.value, msg.protocol_ies.amf_ue_ngap_id.value.value);
  if (u == nullptr) {
    logger.warning("Can not find UE");
    return false;
  }

  u->handle_initial_ctxt_setup_request(msg);

  return true;
}

bool ngap::handle_ue_ctxt_release_cmd(const asn1::ngap_nr::ue_context_release_cmd_s& msg)
{
  const asn1::ngap_nr::ue_ngap_id_pair_s& ue_ngap_id_pair = msg.protocol_ies.ue_ngap_ids.value.ue_ngap_id_pair();

  ue* u = handle_ngapmsg_ue_id(ue_ngap_id_pair.ran_ue_ngap_id, ue_ngap_id_pair.amf_ue_ngap_id);
  if (u == nullptr) {
    logger.warning("Can not find UE");
    return false;
  }

  u->handle_ue_ctxt_release_cmd(msg);

  return true;
}

bool ngap::handle_ue_pdu_session_res_setup_request(const asn1::ngap_nr::pdu_session_res_setup_request_s& msg)
{
  ue* u =
      handle_ngapmsg_ue_id(msg.protocol_ies.ran_ue_ngap_id.value.value, msg.protocol_ies.amf_ue_ngap_id.value.value);
  if (u == nullptr) {
    logger.warning("Can not find UE");
    return false;
  }

  if (msg.protocol_ies.ue_aggregate_maximum_bit_rate_present) {
    rrc->set_aggregate_max_bitrate(u->ctxt.rnti, msg.protocol_ies.ue_aggregate_maximum_bit_rate.value);
  }
  u->handle_pdu_session_res_setup_request(msg);

  return true;
}

/*******************************************************************************
/* NGAP message senders
********************************************************************************/

bool ngap::send_error_indication(const asn1::ngap_nr::cause_c& cause,
                                 srsran::optional<uint32_t>    ran_ue_ngap_id,
                                 srsran::optional<uint32_t>    amf_ue_ngap_id)
{
  if (amf_connected == false) {
    logger.warning("AMF not connected.");
    return false;
  }

  ngap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_NGAP_NR_ID_ERROR_IND);
  auto& container = tx_pdu.init_msg().value.error_ind().protocol_ies;

  uint16_t rnti                    = SRSRAN_INVALID_RNTI;
  container.ran_ue_ngap_id_present = ran_ue_ngap_id.has_value();
  if (ran_ue_ngap_id.has_value()) {
    container.ran_ue_ngap_id.value = ran_ue_ngap_id.value();
    ue* user_ptr                   = users.find_ue_gnbid(ran_ue_ngap_id.value());
    rnti                           = user_ptr != nullptr ? user_ptr->ctxt.rnti : SRSRAN_INVALID_RNTI;
  }
  container.amf_ue_ngap_id_present = amf_ue_ngap_id.has_value();
  if (amf_ue_ngap_id.has_value()) {
    container.amf_ue_ngap_id.value = amf_ue_ngap_id.value();
  }

  container.cause_present = true;
  container.cause.value   = cause;

  return sctp_send_ngap_pdu(tx_pdu, rnti, "Error Indication");
}
/*******************************************************************************
/* NGAP connection helpers
********************************************************************************/

bool ngap::connect_amf()
{
  using namespace srsran::net_utils;
  logger.info("Connecting to AMF %s:%d", args.amf_addr.c_str(), int(AMF_PORT));

  // Init SCTP socket and bind it
  if (not sctp_init_socket(&amf_socket, socket_type::seqpacket, args.ngc_bind_addr.c_str(), 0)) {
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

  container.ran_node_name_present = true;
  if (args.gnb_name.length() >= 150) {
    args.gnb_name.resize(150);
  }

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

  if (rnti != SRSRAN_INVALID_RNTI) {
    logger.info(buf->msg, buf->N_bytes, "Tx NGAP SDU, %s, rnti=0x%x", procedure_name, rnti);
  } else {
    logger.info(buf->msg, buf->N_bytes, "Tx NGAP SDU, %s", procedure_name);
  }

  uint16_t streamid = rnti == SRSRAN_INVALID_RNTI ? NONUE_STREAM_ID : users.find_ue_rnti(rnti)->stream_id;

  ssize_t n_sent = sctp_sendmsg(amf_socket.fd(),
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
      logger.error("Error: Failure at Tx NGAP SDU, %s, rnti=0x%x", procedure_name, rnti);
    } else {
      logger.error("Error: Failure at Tx NGAP SDU, %s", procedure_name);
    }
    return false;
  }
  return true;
}

/**
 * Helper method to find user based on the ran_ue_ngap_id stored in an S1AP Msg, and update amf_ue_ngap_id
 * @param gnb_id ran_ue_ngap_id value stored in NGAP message
 * @param amf_id amf_ue_ngap_id value stored in NGAP message
 * @return pointer to user if it has been found
 */
ngap::ue* ngap::handle_ngapmsg_ue_id(uint32_t gnb_id, uint32_t amf_id)
{
  ue*     user_ptr     = users.find_ue_gnbid(gnb_id);
  ue*     user_amf_ptr = nullptr;
  cause_c cause;
  // TODO: Introduce proper error handling for faulty ids
  if (user_ptr != nullptr) {
    if (user_ptr->ctxt.amf_ue_ngap_id == amf_id) {
      return user_ptr;
    }

    user_amf_ptr = users.find_ue_amfid(amf_id);
    if (not user_ptr->ctxt.amf_ue_ngap_id.has_value() and user_amf_ptr == nullptr) {
      user_ptr->ctxt.amf_ue_ngap_id = amf_id;
      return user_ptr;
    }

    logger.warning("AMF UE NGAP ID=%d not found - discarding message", amf_id);
    if (user_amf_ptr != nullptr) {
      cause.set_radio_network().value = cause_radio_network_opts::unknown_target_id;
    }

  } else {
    logger.warning("User associated with gNB ID %d not found", gnb_id);
    user_amf_ptr = users.find_ue_amfid(amf_id);
    logger.warning("RAN UE NGAP ID=%d not found - discarding message", gnb_id);
    if (user_amf_ptr != nullptr) {
      cause.set_radio_network().value = cause_radio_network_opts::unknown_local_ue_ngap_id;
    }
  }

  send_error_indication(cause, gnb_id, amf_id);

  return nullptr;
}

std::string ngap::get_cause(const cause_c& c)
{
  std::string cause = c.type().to_string();
  cause += " - ";
  switch (c.type().value) {
    case cause_c::types_opts::radio_network:
      cause += c.radio_network().to_string();
      break;
    case cause_c::types_opts::transport:
      cause += c.transport().to_string();
      break;
    case cause_c::types_opts::nas:
      cause += c.nas().to_string();
      break;
    case cause_c::types_opts::protocol:
      cause += c.protocol().to_string();
      break;
    case cause_c::types_opts::misc:
      cause += c.misc().to_string();
      break;
    default:
      cause += "unknown";
      break;
  }
  return cause;
}

} // namespace srsenb
