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

#include "srsenb/hdr/stack/upper/s1ap.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/adt/scope_exit.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/logmap.h"

#include <arpa/inet.h> //for inet_ntop()
#include <inttypes.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> //for close(), sleep()

using srslte::s1ap_mccmnc_to_plmn;
using srslte::uint32_to_uint8;

#define procError(fmt, ...) s1ap_ptr->s1ap_log->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) s1ap_ptr->s1ap_log->warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procInfo(fmt, ...) s1ap_ptr->s1ap_log->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::s1ap;

namespace srsenb {

asn1::bounded_bitstring<1, 160, true, true> addr_to_asn1(const char* addr_str)
{
  asn1::bounded_bitstring<1, 160, true, true> transport_layer_addr(32);
  uint8_t                                     addr[4];
  inet_pton(AF_INET, addr_str, addr);
  for (uint32_t j = 0; j < 4; ++j) {
    transport_layer_addr.data()[j] = addr[3 - j];
  }
  return transport_layer_addr;
}

/*********************************************************
 * TS 36.413 - Section 8.4.1 - "Handover Preparation"
 *********************************************************/
s1ap::ue::ho_prep_proc_t::ho_prep_proc_t(s1ap::ue* ue_) : ue_ptr(ue_), s1ap_ptr(ue_->s1ap_ptr) {}

srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::init(uint32_t                     target_eci_,
                                                      srslte::plmn_id_t            target_plmn_,
                                                      srslte::unique_byte_buffer_t rrc_container_)
{
  target_eci  = target_eci_;
  target_plmn = target_plmn_;

  procInfo("Sending HandoverRequired to MME id=%d\n", ue_ptr->ctxt.mme_ue_s1ap_id);
  if (not ue_ptr->send_ho_required(target_eci, target_plmn, std::move(rrc_container_))) {
    procError("Failed to send HORequired to cell 0x%x\n", target_eci);
    return srslte::proc_outcome_t::error;
  }

  // Start HO preparation timer
  ue_ptr->ts1_reloc_prep.run();

  return srslte::proc_outcome_t::yield;
}
srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(ts1_reloc_prep_expired e)
{
  // do nothing for now
  procError("timer TS1Relocprep has expired.\n");
  return srslte::proc_outcome_t::error;
}
srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(const ho_prep_fail_s& msg)
{
  ue_ptr->ts1_reloc_prep.stop();

  std::string cause = s1ap_ptr->get_cause(msg.protocol_ies.cause.value);
  procError("HO preparation Failure. Cause: %s\n", cause.c_str());
  srslte::console("HO preparation Failure. Cause: %s\n", cause.c_str());

  return srslte::proc_outcome_t::error;
}

/**
 * TS 36.413 - Section 8.4.1.2 - HandoverPreparation Successful Operation
 * Description: MME returns back an HandoverCommand to the SeNB
 */
srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(const asn1::s1ap::ho_cmd_s& msg)
{
  // update timers
  ue_ptr->ts1_reloc_prep.stop();
  ue_ptr->ts1_reloc_overall.run();

  // Check for unsupported S1AP fields
  if (msg.ext or msg.protocol_ies.target_to_source_transparent_container_secondary_present or
      msg.protocol_ies.handov_type.value.value != handov_type_opts::intralte or
      msg.protocol_ies.crit_diagnostics_present or msg.protocol_ies.nas_security_paramsfrom_e_utran_present) {
    procWarning("Not handling HandoverCommand extensions and non-intraLTE params\n");
  }

  // Check for E-RABs that could not be admitted in the target
  if (msg.protocol_ies.erab_to_release_list_ho_cmd_present) {
    procWarning("Not handling E-RABtoReleaseList\n");
    // TODO
  }

  // Check for E-RABs subject to being forwarded
  if (msg.protocol_ies.erab_subjectto_data_forwarding_list_present) {
    procWarning("Not handling E-RABSubjecttoDataForwardingList\n");
    // TODO
  }

  // In case of intra-system Handover, Target to Source Transparent Container IE shall be encoded as
  // Target eNB to Source eNB Transparent Container IE
  asn1::cbit_ref bref(msg.protocol_ies.target_to_source_transparent_container.value.data(),
                      msg.protocol_ies.target_to_source_transparent_container.value.size());
  asn1::s1ap::targetenb_to_sourceenb_transparent_container_s container;
  if (container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    procError("Failed to decode TargeteNBToSourceeNBTransparentContainer\n");
    return srslte::proc_outcome_t::error;
  }
  if (container.ie_exts_present or container.ext) {
    procWarning("Not handling extensions\n");
  }

  // Create a unique buffer out of transparent container to pass to RRC
  rrc_container = srslte::allocate_unique_buffer(*s1ap_ptr->pool, false);
  if (rrc_container == nullptr) {
    procError("Fatal Error: Couldn't allocate buffer.\n");
    return srslte::proc_outcome_t::error;
  }
  memcpy(rrc_container->msg, container.rrc_container.data(), container.rrc_container.size());
  rrc_container->N_bytes = container.rrc_container.size();

  return srslte::proc_outcome_t::success;
}

void s1ap::ue::ho_prep_proc_t::then(const srslte::proc_state_t& result)
{
  if (result.is_error()) {
    s1ap_ptr->rrc->ho_preparation_complete(ue_ptr->ctxt.rnti, false, {});
  } else {
    s1ap_ptr->rrc->ho_preparation_complete(ue_ptr->ctxt.rnti, true, std::move(rrc_container));
    procInfo("Completed with success\n");
  }
}

/*********************************************************
 *                     MME Connection
 *********************************************************/

srslte::proc_outcome_t s1ap::s1_setup_proc_t::init()
{
  procInfo("Starting new MME connection.\n");

  return start_mme_connection();
}

srslte::proc_outcome_t s1ap::s1_setup_proc_t::start_mme_connection()
{
  if (not s1ap_ptr->running) {
    procInfo("S1AP is not running anymore.\n");
    return srslte::proc_outcome_t::error;
  }
  if (s1ap_ptr->mme_connected) {
    procInfo("eNB S1AP is already connected to MME\n");
    return srslte::proc_outcome_t::success;
  }

  if (not s1ap_ptr->connect_mme()) {
    procInfo("Failed to initiate SCTP socket. Attempting reconnection in %d seconds\n",
             s1ap_ptr->mme_connect_timer.duration() / 1000);
    srslte::console("Failed to initiate SCTP socket. Attempting reconnection in %d seconds\n",
                       s1ap_ptr->mme_connect_timer.duration() / 1000);
    s1ap_ptr->mme_connect_timer.run();
    return srslte::proc_outcome_t::error;
  }

  if (not s1ap_ptr->setup_s1()) {
    procError("S1 setup failed. Exiting...\n");
    srslte::console("S1 setup failed\n");
    s1ap_ptr->running = false;
    return srslte::proc_outcome_t::error;
  }

  s1ap_ptr->s1setup_timeout.run();
  procInfo("S1SetupRequest sent. Waiting for response...\n");
  return srslte::proc_outcome_t::yield;
}

srslte::proc_outcome_t s1ap::s1_setup_proc_t::react(const srsenb::s1ap::s1_setup_proc_t::s1setupresult& event)
{
  if (s1ap_ptr->s1setup_timeout.is_running()) {
    s1ap_ptr->s1setup_timeout.stop();
  }
  if (event.success) {
    procInfo("S1Setup procedure completed successfully\n");
    return srslte::proc_outcome_t::success;
  }
  procError("S1Setup failed. Exiting...\n");
  srslte::console("S1setup failed\n");
  return srslte::proc_outcome_t::error;
}

void s1ap::s1_setup_proc_t::then(const srslte::proc_state_t& result) const
{
  if (result.is_error()) {
    s1ap_ptr->s1ap_socket.reset();
    procInfo("S1AP socket closed.\n");
  }
}

/*********************************************************
 *                     S1AP class
 *********************************************************/

s1ap::s1ap(srslte::task_sched_handle task_sched_) : s1setup_proc(this), task_sched(task_sched_) {}

int s1ap::init(s1ap_args_t args_, rrc_interface_s1ap* rrc_, srsenb::stack_interface_s1ap_lte* stack_)
{
  rrc      = rrc_;
  args     = args_;
  s1ap_log = srslte::logmap::get("S1AP");
  stack    = stack_;
  pool     = srslte::byte_buffer_pool::get_instance();

  build_tai_cgi();

  // Setup MME reconnection timer
  mme_connect_timer    = task_sched.get_unique_timer();
  auto mme_connect_run = [this](uint32_t tid) {
    if (not s1setup_proc.launch()) {
      s1ap_log->error("Failed to initiate S1Setup procedure.\n");
    }
  };
  mme_connect_timer.set(10000, mme_connect_run);
  // Setup S1Setup timeout
  s1setup_timeout              = task_sched.get_unique_timer();
  uint32_t s1setup_timeout_val = 5000;
  s1setup_timeout.set(s1setup_timeout_val, [this](uint32_t tid) {
    s1_setup_proc_t::s1setupresult res;
    res.success = false;
    res.cause   = s1_setup_proc_t::s1setupresult::cause_t::timeout;
    s1setup_proc.trigger(res);
  });

  running = true;
  // starting MME connection
  if (not s1setup_proc.launch()) {
    s1ap_log->error("Failed to initiate S1Setup procedure.\n");
  }

  return SRSLTE_SUCCESS;
}

void s1ap::stop()
{
  running = false;
  s1ap_socket.reset();
}

void s1ap::get_metrics(s1ap_metrics_t& m)
{
  if (!running) {
    m.status = S1AP_ERROR;
    return;
  }
  if (mme_connected) {
    m.status = S1AP_READY;
  } else {
    m.status = S1AP_ATTACHING;
  }
}

// Generate common S1AP protocol IEs from config args
void s1ap::build_tai_cgi()
{
  uint32_t plmn;

  // TAI
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tai.plm_nid.from_number(plmn);

  tai.tac.from_number(args.tac);

  // EUTRAN_CGI
  eutran_cgi.plm_nid.from_number(plmn);

  eutran_cgi.cell_id.from_number((uint32_t)(args.enb_id << 8) | args.cell_id);
}

/*******************************************************************************
/* RRC interface
********************************************************************************/
void s1ap::initial_ue(uint16_t rnti, asn1::s1ap::rrc_establishment_cause_e cause, srslte::unique_byte_buffer_t pdu)
{
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.rnti = rnti;
  ue* u             = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    s1ap_log->error("Failed to add rnti=0x%x\n", rnti);
    return;
  }
  u->send_initialuemessage(cause, std::move(pdu), false);
}

void s1ap::initial_ue(uint16_t                              rnti,
                      asn1::s1ap::rrc_establishment_cause_e cause,
                      srslte::unique_byte_buffer_t          pdu,
                      uint32_t                              m_tmsi,
                      uint8_t                               mmec)
{
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.rnti = rnti;
  ue* u             = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    s1ap_log->error("Failed to add rnti=0x%x\n", rnti);
    return;
  }
  u->send_initialuemessage(cause, std::move(pdu), true, m_tmsi, mmec);
}

void s1ap::write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  s1ap_log->info_hex(pdu->msg, pdu->N_bytes, "Received RRC SDU");

  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    s1ap_log->info("The rnti=0x%x does not exist\n", rnti);
    return;
  }
  u->send_ulnastransport(std::move(pdu));
}

bool s1ap::user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio)
{
  s1ap_log->info("User inactivity - RNTI:0x%x\n", rnti);

  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return false;
  }

  if (u->was_uectxtrelease_requested()) {
    s1ap_log->warning("UE context for RNTI:0x%x is in zombie state. Releasing...\n", rnti);
    users.erase(u);
    rrc->release_complete(rnti);
    return false;
  }

  cause_c cause;
  cause.set_radio_network().value = cause_radio.value;

  if (u->ctxt.mme_ue_s1ap_id_present) {
    return u->send_uectxtreleaserequest(cause);
  }
  return true;
}

bool s1ap::user_exists(uint16_t rnti)
{
  return users.find_ue_rnti(rnti) != nullptr;
}

void s1ap::user_mod(uint16_t old_rnti, uint16_t new_rnti)
{
  s1ap_log->info("Modifying user context. Old rnti: 0x%x, new rnti: 0x%x\n", old_rnti, new_rnti);
  if (not user_exists(old_rnti)) {
    s1ap_log->error("Old rnti does not exist, aborting.\n");
    return;
  }
  if (user_exists(new_rnti)) {
    s1ap_log->error("New rnti already exists, aborting.\n");
    return;
  }
  users.find_ue_rnti(old_rnti)->ctxt.rnti = new_rnti;
}

void s1ap::ue_ctxt_setup_complete(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return;
  }
  if (res.protocol_ies.erab_setup_list_ctxt_su_res.value.size() > 0) {
    u->send_initial_ctxt_setup_response(res);
  } else {
    u->send_initial_ctxt_setup_failure();
  }
}

void s1ap::ue_erab_setup_complete(uint16_t rnti, const asn1::s1ap::erab_setup_resp_s& res)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    s1ap_log->error("rnti 0x%x not found\n", rnti);
    return;
  }
  u->send_erab_setup_response(res);
}

// void ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps)
//{

//}

bool s1ap::is_mme_connected()
{
  return mme_connected;
}

/*******************************************************************************
/* S1AP connection helpers
********************************************************************************/

bool s1ap::connect_mme()
{
  s1ap_log->info("Connecting to MME %s:%d\n", args.mme_addr.c_str(), MME_PORT);

  // Init SCTP socket and bind it
  if (not srslte::net_utils::sctp_init_client(
          &s1ap_socket, srslte::net_utils::socket_type::seqpacket, args.s1c_bind_addr.c_str())) {
    return false;
  }

  // Connect to the MME address
  if (not s1ap_socket.connect_to(args.mme_addr.c_str(), MME_PORT, &mme_addr)) {
    return false;
  }

  // Assign a handler to rx MME packets (going to run in a different thread)
  stack->add_mme_socket(s1ap_socket.fd());

  s1ap_log->info("SCTP socket established with MME\n");
  return true;
}

bool s1ap::setup_s1()
{
  uint32_t tmp32;
  uint16_t tmp16;

  uint32_t plmn;
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  plmn = htonl(plmn);

  tmp32 = htonl(args.enb_id);

  s1ap_pdu_c pdu;
  pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_S1_SETUP);
  s1_setup_request_ies_container& container = pdu.init_msg().value.s1_setup_request().protocol_ies;
  container.global_enb_id.value.plm_nid[0]  = ((uint8_t*)&plmn)[1];
  container.global_enb_id.value.plm_nid[1]  = ((uint8_t*)&plmn)[2];
  container.global_enb_id.value.plm_nid[2]  = ((uint8_t*)&plmn)[3];

  container.global_enb_id.value.enb_id.set_macro_enb_id().from_number(args.enb_id);

  container.enbname_present = true;
  container.enbname.value.from_string(args.enb_name);

  container.supported_tas.value.resize(1);
  tmp16 = htons(args.tac);
  memcpy(container.supported_tas.value[0].tac.data(), (uint8_t*)&tmp16, 2);
  container.supported_tas.value[0].broadcast_plmns.resize(1);
  container.supported_tas.value[0].broadcast_plmns[0][0] = ((uint8_t*)&plmn)[1];
  container.supported_tas.value[0].broadcast_plmns[0][1] = ((uint8_t*)&plmn)[2];
  container.supported_tas.value[0].broadcast_plmns[0][2] = ((uint8_t*)&plmn)[3];

  container.default_paging_drx.value.value = asn1::s1ap::paging_drx_opts::v128; // Todo: add to args, config file

  return sctp_send_s1ap_pdu(pdu, 0, "s1SetupRequest");
}

/*******************************************************************************
/* S1AP message handlers
********************************************************************************/

bool s1ap::handle_mme_rx_msg(srslte::unique_byte_buffer_t pdu,
                             const sockaddr_in&           from,
                             const sctp_sndrcvinfo&       sri,
                             int                          flags)
{
  // Handle Notification Case
  if (flags & MSG_NOTIFICATION) {
    // Received notification
    union sctp_notification* notification = (union sctp_notification*)pdu->msg;
    s1ap_log->debug("SCTP Notification %d\n", notification->sn_header.sn_type);
    if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT) {
      s1ap_log->info("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
      srslte::console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
      s1ap_socket.reset();
    }
  } else if (pdu->N_bytes == 0) {
    s1ap_log->error("SCTP return 0 bytes. Closing socket\n");
    s1ap_socket.reset();
  }

  // Restart MME connection procedure if we lost connection
  if (not s1ap_socket.is_init()) {
    mme_connected = false;
    if (not s1setup_proc.launch()) {
      s1ap_log->error("Failed to initiate MME connection procedure.\n");
    }
    return false;
  }

  s1ap_log->info_hex(pdu->msg, pdu->N_bytes, "Received S1AP PDU");
  handle_s1ap_rx_pdu(pdu.get());
  return true;
}

bool s1ap::handle_s1ap_rx_pdu(srslte::byte_buffer_t* pdu)
{
  // Save message to PCAP
  if (pcap != nullptr) {
    pcap->write_s1ap(pdu->msg, pdu->N_bytes);
  }

  s1ap_pdu_c     rx_pdu;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);

  if (rx_pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    s1ap_log->error("Failed to unpack received PDU\n");
    return false;
  }

  switch (rx_pdu.type().value) {
    case s1ap_pdu_c::types_opts::init_msg:
      return handle_initiatingmessage(rx_pdu.init_msg());
    case s1ap_pdu_c::types_opts::successful_outcome:
      return handle_successfuloutcome(rx_pdu.successful_outcome());
    case s1ap_pdu_c::types_opts::unsuccessful_outcome:
      return handle_unsuccessfuloutcome(rx_pdu.unsuccessful_outcome());
    default:
      s1ap_log->error("Unhandled PDU type %d\n", rx_pdu.type().value);
      return false;
  }

  return true;
}

bool s1ap::handle_initiatingmessage(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport:
      return handle_dlnastransport(msg.value.dl_nas_transport());
    case s1ap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request:
      return handle_initialctxtsetuprequest(msg.value.init_context_setup_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      return handle_uectxtreleasecommand(msg.value.ue_context_release_cmd());
    case s1ap_elem_procs_o::init_msg_c::types_opts::paging:
      return handle_paging(msg.value.paging());
    case s1ap_elem_procs_o::init_msg_c::types_opts::erab_setup_request:
      return handle_erabsetuprequest(msg.value.erab_setup_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request:
      return handle_uecontextmodifyrequest(msg.value.ue_context_mod_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ho_request:
      return handle_ho_request(msg.value.ho_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::mme_status_transfer:
      return handle_mme_status_transfer(msg.value.mme_status_transfer());
    default:
      s1ap_log->error("Unhandled initiating message: %s\n", msg.value.type().to_string().c_str());
  }
  return true;
}

bool s1ap::handle_successfuloutcome(const successful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::s1_setup_resp:
      return handle_s1setupresponse(msg.value.s1_setup_resp());
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::ho_cmd:
      return handle_s1hocommand(msg.value.ho_cmd());
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::ho_cancel_ack:
      s1ap_log->info("Received %s\n", msg.value.type().to_string().c_str());
      return true;
    default:
      s1ap_log->error("Unhandled successful outcome message: %s\n", msg.value.type().to_string().c_str());
  }
  return true;
}

bool s1ap::handle_unsuccessfuloutcome(const unsuccessful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::s1_setup_fail:
      return handle_s1setupfailure(msg.value.s1_setup_fail());
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail:
      return handle_hopreparationfailure(msg.value.ho_prep_fail());
    default:
      s1ap_log->error("Unhandled unsuccessful outcome message: %s\n", msg.value.type().to_string().c_str());
  }
  return true;
}

bool s1ap::handle_s1setupresponse(const asn1::s1ap::s1_setup_resp_s& msg)
{
  s1ap_log->info("Received S1SetupResponse\n");
  s1setupresponse = msg;
  mme_connected   = true;
  s1_setup_proc_t::s1setupresult res;
  res.success = true;
  s1setup_proc.trigger(res);
  return true;
}

bool s1ap::handle_dlnastransport(const dl_nas_transport_s& msg)
{
  s1ap_log->info("Received DownlinkNASTransport\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (msg.protocol_ies.ho_restrict_list_present) {
    s1ap_log->warning("Not handling HandoverRestrictionList\n");
  }
  if (msg.protocol_ies.subscriber_profile_idfor_rfp_present) {
    s1ap_log->warning("Not handling SubscriberProfileIDforRFP\n");
  }

  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);
  if (pdu == nullptr) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::run_thread().\n");
    return false;
  }
  memcpy(pdu->msg, msg.protocol_ies.nas_pdu.value.data(), msg.protocol_ies.nas_pdu.value.size());
  pdu->N_bytes = msg.protocol_ies.nas_pdu.value.size();
  rrc->write_dl_info(u->ctxt.rnti, std::move(pdu));
  return true;
}

bool s1ap::handle_initialctxtsetuprequest(const init_context_setup_request_s& msg)
{
  s1ap_log->info("Received InitialContextSetupRequest\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  // Setup UE ctxt in RRC
  if (not rrc->setup_ue_ctxt(u->ctxt.rnti, msg)) {
    return false;
  }

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_required ||
        msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_high_prio) {
      // Send RRC Release (cs-fallback-triggered) to MME
      cause_c cause;
      cause.set_radio_network().value = cause_radio_network_opts::cs_fallback_triggered;

      /* TODO: This should normally probably only be sent after the SecurityMode procedure has completed! */
      u->send_uectxtreleaserequest(cause);
    }
  }

  return true;
}

bool s1ap::handle_paging(const asn1::s1ap::paging_s& msg)
{
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  uint32_t ueid = msg.protocol_ies.ue_id_idx_value.value.to_number();
  rrc->add_paging_id(ueid, msg.protocol_ies.ue_paging_id.value);
  return true;
}

bool s1ap::handle_erabsetuprequest(const erab_setup_request_s& msg)
{
  s1ap_log->info("Received ERABSetupRequest\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  // Setup UE ctxt in RRC
  return rrc->setup_ue_erabs(u->ctxt.rnti, msg);
}

bool s1ap::handle_uecontextmodifyrequest(const ue_context_mod_request_s& msg)
{
  s1ap_log->info("Received UeContextModificationRequest\n");
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (!rrc->modify_ue_ctxt(u->ctxt.rnti, msg)) {
    cause_c cause;
    cause.set_misc().value = cause_misc_opts::unspecified;
    u->send_uectxtmodifyfailure(cause);
    return true;
  }

  // Send UEContextModificationResponse
  u->send_uectxtmodifyresp();

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_required ||
        msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_high_prio) {
      // Send RRC Release (cs-fallback-triggered) to MME
      cause_c cause;
      cause.set_radio_network().value = cause_radio_network_opts::cs_fallback_triggered;

      u->send_uectxtreleaserequest(cause);
    }
  }

  return true;
}

bool s1ap::handle_uectxtreleasecommand(const ue_context_release_cmd_s& msg)
{
  s1ap_log->info("Received UEContextReleaseCommand\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }

  ue* u = nullptr;
  if (msg.protocol_ies.ue_s1ap_ids.value.type().value == ue_s1ap_ids_c::types_opts::ue_s1ap_id_pair) {
    auto& idpair = msg.protocol_ies.ue_s1ap_ids.value.ue_s1ap_id_pair();

    if (idpair.ext) {
      s1ap_log->warning("Not handling S1AP message extension\n");
    }
    if (idpair.ie_exts_present) {
      s1ap_log->warning("Not handling S1AP message iE_Extensions\n");
    }
    u = find_s1apmsg_user(idpair.enb_ue_s1ap_id, idpair.mme_ue_s1ap_id);
    if (u == nullptr) {
      return false;
    }
  } else {
    uint32_t mme_ue_id = msg.protocol_ies.ue_s1ap_ids.value.mme_ue_s1ap_id();
    u                  = users.find_ue_mmeid(mme_ue_id);
    if (u == nullptr) {
      s1ap_log->warning("UE for mme_ue_s1ap_id:%d not found - discarding message\n", mme_ue_id);
      return false;
    }
  }

  uint16_t rnti = u->ctxt.rnti;
  rrc->release_erabs(rnti);
  u->send_uectxtreleasecomplete();
  users.erase(u);
  s1ap_log->info("UE context for RNTI:0x%x released\n", rnti);
  rrc->release_complete(rnti);
  return true;
}

bool s1ap::handle_s1setupfailure(const asn1::s1ap::s1_setup_fail_s& msg)
{
  std::string cause = get_cause(msg.protocol_ies.cause.value);
  s1ap_log->error("S1 Setup Failure. Cause: %s\n", cause.c_str());
  srslte::console("S1 Setup Failure. Cause: %s\n", cause.c_str());
  return true;
}

bool s1ap::handle_hopreparationfailure(const ho_prep_fail_s& msg)
{
  s1ap_log->info("Received HO Preparation Failure\n");
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }
  u->ho_prep_proc.trigger(msg);
  return true;
}

bool s1ap::handle_s1hocommand(const asn1::s1ap::ho_cmd_s& msg)
{
  s1ap_log->info("Received S1 HO Command\n");
  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }
  u->ho_prep_proc.trigger(msg);
  return true;
}

/**************************************************************
 * TS 36.413 - Section 8.4.2 - "Handover Resource Allocation"
 *************************************************************/

bool s1ap::handle_ho_request(const asn1::s1ap::ho_request_s& msg)
{
  uint16_t rnti = SRSLTE_INVALID_RNTI;

  s1ap_log->info("Received S1 HO Request\n");
  srslte::console("Received S1 HO Request\n");

  auto on_scope_exit = srslte::make_scope_exit([this, &rnti, msg]() {
    // If rnti is not allocated successfully, remove from s1ap and send handover failure
    if (rnti == SRSLTE_INVALID_RNTI) {
      send_ho_failure(msg.protocol_ies.mme_ue_s1ap_id.value.value);
    }
  });

  if (msg.ext or msg.protocol_ies.ho_restrict_list_present or
      msg.protocol_ies.handov_type.value.value != handov_type_opts::intralte) {
    s1ap_log->error("Not handling S1AP non-intra LTE handovers and extensions\n");
    return false;
  }

  // Confirm the UE does not exist in TeNB
  if (users.find_ue_mmeid(msg.protocol_ies.mme_ue_s1ap_id.value.value) != nullptr) {
    s1ap_log->error("The provided MME_UE_S1AP_ID=%" PRIu64 " is already connected to the cell\n",
                    msg.protocol_ies.mme_ue_s1ap_id.value.value);
    return false;
  }

  // Create user ctxt object and associated MME context
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.mme_ue_s1ap_id_present = true;
  ue_ptr->ctxt.mme_ue_s1ap_id         = msg.protocol_ies.mme_ue_s1ap_id.value.value;
  if (users.add_user(std::move(ue_ptr)) == nullptr) {
    return false;
  }

  // Unpack Transparent Container
  sourceenb_to_targetenb_transparent_container_s container;
  asn1::cbit_ref bref{msg.protocol_ies.source_to_target_transparent_container.value.data(),
                      msg.protocol_ies.source_to_target_transparent_container.value.size()};
  if (container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    s1ap_log->error("Failed to unpack SourceToTargetTransparentContainer\n");
    return false;
  }

  // Handle Handover Resource Allocation
  rnti = rrc->start_ho_ue_resource_alloc(msg, container);
  return rnti != SRSLTE_INVALID_RNTI;
}

bool s1ap::send_ho_failure(uint32_t mme_ue_s1ap_id)
{
  // Remove created s1ap user
  ue* u = users.find_ue_mmeid(mme_ue_s1ap_id);
  if (u != nullptr) {
    users.erase(u);
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_HO_RES_ALLOC);
  ho_fail_ies_container& container = tx_pdu.unsuccessful_outcome().value.ho_fail().protocol_ies;

  container.mme_ue_s1ap_id.value = mme_ue_s1ap_id;
  // TODO: Setup cause
  container.cause.value.set_radio_network().value = cause_radio_network_opts::ho_target_not_allowed;

  return sctp_send_s1ap_pdu(tx_pdu, SRSLTE_INVALID_RNTI, "HandoverFailure");
}

bool s1ap::send_ho_req_ack(const asn1::s1ap::ho_request_s&               msg,
                           uint16_t                                      rnti,
                           srslte::unique_byte_buffer_t                  ho_cmd,
                           srslte::span<asn1::fixed_octstring<4, true> > admitted_bearers)
{
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_HO_RES_ALLOC);
  ho_request_ack_ies_container& container = tx_pdu.successful_outcome().value.ho_request_ack().protocol_ies;

  ue* ue_ptr        = users.find_ue_mmeid(msg.protocol_ies.mme_ue_s1ap_id.value.value);
  ue_ptr->ctxt.rnti = rnti;

  container.mme_ue_s1ap_id.value = msg.protocol_ies.mme_ue_s1ap_id.value.value;
  container.enb_ue_s1ap_id.value = ue_ptr->ctxt.enb_ue_s1ap_id;

  // TODO: Add admitted E-RABs
  for (uint32_t i = 0; i < msg.protocol_ies.erab_to_be_setup_list_ho_req.value.size(); ++i) {
    const auto&                           erab      = msg.protocol_ies.erab_to_be_setup_list_ho_req.value[i];
    const erab_to_be_setup_item_ho_req_s& erabsetup = erab.value.erab_to_be_setup_item_ho_req();
    container.erab_admitted_list.value.push_back({});
    container.erab_admitted_list.value.back().load_info_obj(ASN1_S1AP_ID_ERAB_ADMITTED_ITEM);
    auto& c                   = container.erab_admitted_list.value.back().value.erab_admitted_item();
    c.erab_id                 = erabsetup.erab_id;
    c.gtp_teid                = admitted_bearers[i];
    c.transport_layer_address = addr_to_asn1(args.gtp_bind_addr.c_str());
    //    c.dl_transport_layer_address_present = true;
    //    c.dl_transport_layer_address         = c.transport_layer_address;
    //    c.ul_transport_layer_address_present = true;
    //    c.ul_transport_layer_address         = c.transport_layer_address;
  }
  asn1::s1ap::targetenb_to_sourceenb_transparent_container_s transparent_container;
  transparent_container.rrc_container.resize(ho_cmd->N_bytes);
  memcpy(transparent_container.rrc_container.data(), ho_cmd->msg, ho_cmd->N_bytes);

  auto&         pdu = ho_cmd; // reuse pdu
  asn1::bit_ref bref{pdu->msg, pdu->get_tailroom()};
  if (transparent_container.pack(bref) != asn1::SRSASN_SUCCESS) {
    s1ap_log->error("Failed to pack TargeteNBToSourceeNBTransparentContainer\n");
    return false;
  }
  container.target_to_source_transparent_container.value.resize(bref.distance_bytes());
  memcpy(container.target_to_source_transparent_container.value.data(), pdu->msg, bref.distance_bytes());

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "HandoverRequestAcknowledge");
}

bool s1ap::handle_mme_status_transfer(const asn1::s1ap::mme_status_transfer_s& msg)
{
  s1ap_log->info("Received S1 MMEStatusTransfer\n");
  srslte::console("Received S1 MMEStatusTransfer\n");

  ue* u = find_s1apmsg_user(msg.protocol_ies.enb_ue_s1ap_id.value.value, msg.protocol_ies.mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  rrc->set_erab_status(
      u->ctxt.rnti,
      msg.protocol_ies.enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list);
  return true;
}

void s1ap::send_ho_notify(uint16_t rnti, uint64_t target_eci)
{
  ue* user_ptr = users.find_ue_rnti(rnti);
  if (user_ptr == nullptr) {
    return;
  }

  s1ap_pdu_c tx_pdu;

  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_HO_NOTIF);
  ho_notify_ies_container& container = tx_pdu.init_msg().value.ho_notify().protocol_ies;

  container.mme_ue_s1ap_id.value = user_ptr->ctxt.mme_ue_s1ap_id;
  container.enb_ue_s1ap_id.value = user_ptr->ctxt.enb_ue_s1ap_id;

  container.eutran_cgi.value = eutran_cgi;
  container.eutran_cgi.value.cell_id.from_number(target_eci);
  container.tai.value = tai;

  sctp_send_s1ap_pdu(tx_pdu, rnti, "HandoverNotify");
}

void s1ap::send_ho_cancel(uint16_t rnti)
{
  ue* user_ptr = users.find_ue_rnti(rnti);
  if (user_ptr == nullptr) {
    return;
  }

  s1ap_pdu_c tx_pdu;

  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_HO_CANCEL);
  ho_cancel_ies_container& container = tx_pdu.init_msg().value.ho_cancel().protocol_ies;

  container.mme_ue_s1ap_id.value                  = user_ptr->ctxt.mme_ue_s1ap_id;
  container.enb_ue_s1ap_id.value                  = user_ptr->ctxt.enb_ue_s1ap_id;
  container.cause.value.set_radio_network().value = cause_radio_network_opts::ho_cancelled;

  sctp_send_s1ap_pdu(tx_pdu, rnti, "HandoverCancel");
}

/*******************************************************************************
/* S1AP message senders
********************************************************************************/

bool s1ap::ue::send_initialuemessage(asn1::s1ap::rrc_establishment_cause_e cause,
                                     srslte::unique_byte_buffer_t          pdu,
                                     bool                                  has_tmsi,
                                     uint32_t                              m_tmsi,
                                     uint8_t                               mmec)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_INIT_UE_MSG);
  init_ue_msg_ies_container& container = tx_pdu.init_msg().value.init_ue_msg().protocol_ies;

  // S_TMSI
  if (has_tmsi) {
    container.s_tmsi_present = true;
    uint32_to_uint8(m_tmsi, container.s_tmsi.value.m_tmsi.data());
    container.s_tmsi.value.mmec[0] = mmec;
  }

  // ENB_UE_S1AP_ID
  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // NAS_PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // TAI
  container.tai.value = s1ap_ptr->tai;

  // EUTRAN_CGI
  container.eutran_cgi.value = s1ap_ptr->eutran_cgi;

  // RRC Establishment Cause
  container.rrc_establishment_cause.value = cause;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "InitialUEMessage");
}

bool s1ap::ue::send_ulnastransport(srslte::unique_byte_buffer_t pdu)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UL_NAS_TRANSPORT);
  asn1::s1ap::ul_nas_transport_ies_container& container = tx_pdu.init_msg().value.ul_nas_transport().protocol_ies;
  container.mme_ue_s1ap_id.value                        = ctxt.mme_ue_s1ap_id;
  container.enb_ue_s1ap_id.value                        = ctxt.enb_ue_s1ap_id;

  // NAS PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // EUTRAN CGI
  container.eutran_cgi.value = s1ap_ptr->eutran_cgi;

  // TAI
  container.tai.value = s1ap_ptr->tai;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UplinkNASTransport");
}

bool s1ap::ue::send_uectxtreleaserequest(const cause_c& cause)
{
  if (!s1ap_ptr->mme_connected) {
    return false;
  }

  if (!ctxt.mme_ue_s1ap_id_present) {
    s1ap_log->error("Cannot send UE context release request without a MME-UE-S1AP-Id allocated.\n");
    return false;
  }

  release_requested = true;
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE_REQUEST);
  ue_context_release_request_ies_container& container =
      tx_pdu.init_msg().value.ue_context_release_request().protocol_ies;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;
  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // Cause
  container.cause.value = cause;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseRequest");
}

bool s1ap::ue::send_uectxtreleasecomplete()
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE);
  auto& container                = tx_pdu.successful_outcome().value.ue_context_release_complete().protocol_ies;
  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseComplete");
}

bool s1ap::ue::send_initial_ctxt_setup_response(const asn1::s1ap::init_context_setup_resp_s& res_)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  // Copy in the provided response message
  tx_pdu.successful_outcome().value.init_context_setup_resp() = res_;

  // Fill in the MME and eNB IDs
  auto& container                = tx_pdu.successful_outcome().value.init_context_setup_resp().protocol_ies;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;
  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // Fill in the GTP bind address for all bearers
  for (uint32_t i = 0; i < container.erab_setup_list_ctxt_su_res.value.size(); ++i) {
    auto& item = container.erab_setup_list_ctxt_su_res.value[i].value.erab_setup_item_ctxt_su_res();
    item.transport_layer_address.resize(32);
    uint8_t addr[4];
    inet_pton(AF_INET, s1ap_ptr->args.gtp_bind_addr.c_str(), addr);
    for (uint32_t j = 0; j < 4; ++j) {
      item.transport_layer_address.data()[j] = addr[3 - j];
    }
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "InitialContextSetupResponse");
}

bool s1ap::ue::send_erab_setup_response(const erab_setup_resp_s& res_)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_ERAB_SETUP);
  erab_setup_resp_s& res = tx_pdu.successful_outcome().value.erab_setup_resp();

  res = res_;

  // Fill in the GTP bind address for all bearers
  if (res.protocol_ies.erab_setup_list_bearer_su_res_present) {
    for (uint32_t i = 0; i < res.protocol_ies.erab_setup_list_bearer_su_res.value.size(); ++i) {
      auto& item = res.protocol_ies.erab_setup_list_bearer_su_res.value[i].value.erab_setup_item_bearer_su_res();
      item.transport_layer_address.resize(32);
      uint8_t addr[4];
      inet_pton(AF_INET, s1ap_ptr->args.gtp_bind_addr.c_str(), addr);
      for (uint32_t j = 0; j < 4; ++j) {
        item.transport_layer_address.data()[j] = addr[3 - j];
      }
    }
  }

  // Fill in the MME and eNB IDs
  res.protocol_ies.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;
  res.protocol_ies.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "E_RABSetupResponse");
}

bool s1ap::ue::send_initial_ctxt_setup_failure()
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
  auto& container = tx_pdu.unsuccessful_outcome().value.init_context_setup_fail().protocol_ies;

  container.enb_ue_s1ap_id.value                  = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value                  = ctxt.mme_ue_s1ap_id;
  container.cause.value.set_radio_network().value = cause_radio_network_opts::unspecified;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "InitialContextSetupFailure");
}

bool s1ap::ue::send_uectxtmodifyresp()
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.successful_outcome().value.ue_context_mod_resp().protocol_ies;

  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextModificationResponse");
}

bool s1ap::ue::send_uectxtmodifyfailure(const cause_c& cause)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.unsuccessful_outcome().value.ue_context_mod_fail().protocol_ies;

  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;
  container.cause.value          = cause;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextModificationFailure");
}

/*********************
 * Handover Messages
 ********************/

bool s1ap::send_ho_required(uint16_t                     rnti,
                            uint32_t                     target_eci,
                            srslte::plmn_id_t            target_plmn,
                            srslte::unique_byte_buffer_t rrc_container)
{
  if (!mme_connected) {
    return false;
  }
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return false;
  }

  // launch procedure
  if (not u->ho_prep_proc.launch(target_eci, target_plmn, std::move(rrc_container))) {
    s1ap_log->error("Failed to initiate an HandoverPreparation procedure for user rnti=0x%x\n", u->ctxt.rnti);
    return false;
  }
  return true;
}

bool s1ap::send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list)
{
  if (not mme_connected) {
    return false;
  }
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return false;
  }

  return u->send_enb_status_transfer_proc(bearer_status_list);
}

// bool s1ap::send_ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps)
//{
//  srslte::byte_buffer_t msg;

//  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
//  tx_pdu.ext          = false;
//  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

//  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
//  init->procedureCode = LIBLTE_S1AP_PROC_ID_UPLINKNASTRANSPORT;
//  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECAPABILITYINFOINDICATION;

//  LIBLTE_S1AP_MESSAGE_UECAPABILITYINFOINDICATION_STRUCT *caps = &init->choice.UECapabilityInfoIndication;
//  caps->ext                           = false;
//  caps->mme_ue_s1ap_id.mme_ue_s1ap_id = ue_ctxt_map[rnti]->mme_ue_s1ap_id;
//  caps->enb_ue_s1ap_id.ENB_UE_S1AP_ID = ue_ctxt_map[rnti]->enb_ue_s1ap_id;
//  // TODO: caps->UERadioCapability.

//  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
//  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending UERadioCapabilityInfo for RNTI:0x%x", rnti);

//  ssize_t n_sent = sctp_sendmsg(socket_fd, msg.msg, msg.N_bytes,
//                                (struct sockaddr*)&mme_addr, sizeof(struct sockaddr_in),
//                                htonl(PPID), 0, ue_ctxt_map[rnti]->stream_id, 0, 0);
//  if(n_sent == -1) {
//    s1ap_log->error("Failed to send UplinkNASTransport for RNTI:0x%x\n", rnti);
//    return false;
//  }

//  return true;
//}

/*********************************************************
 *              s1ap::user_list class
 *********************************************************/

s1ap::ue* s1ap::user_list::find_ue_rnti(uint16_t rnti)
{
  if (rnti == SRSLTE_INVALID_RNTI) {
    return nullptr;
  }
  auto it = std::find_if(
      users.begin(), users.end(), [rnti](const user_list::pair_type& v) { return v.second->ctxt.rnti == rnti; });
  return it != users.end() ? it->second.get() : nullptr;
}

s1ap::ue* s1ap::user_list::find_ue_enbid(uint32_t enbid)
{
  auto it = users.find(enbid);
  return (it != users.end()) ? it->second.get() : nullptr;
}

s1ap::ue* s1ap::user_list::find_ue_mmeid(uint32_t mmeid)
{
  auto it = std::find_if(users.begin(), users.end(), [mmeid](const user_list::pair_type& v) {
    return v.second->ctxt.mme_ue_s1ap_id_present and v.second->ctxt.mme_ue_s1ap_id == mmeid;
  });
  return it != users.end() ? it->second.get() : nullptr;
}

/**
 * @brief Adds a user to the user list, avoiding any rnti, enb_s1ap_id, mme_s1ap_id duplication
 * @param %user to be inserted
 * @return ptr of inserted %user. If failure, returns nullptr
 */
s1ap::ue* s1ap::user_list::add_user(std::unique_ptr<s1ap::ue> user)
{
  // Check for ID repetitions
  if (find_ue_rnti(user->ctxt.rnti) != nullptr) {
    srslte::logmap::get("S1AP")->error("The user to be added with rnti=0x%x already exists\n", user->ctxt.rnti);
    return nullptr;
  }
  if (find_ue_enbid(user->ctxt.enb_ue_s1ap_id) != nullptr) {
    srslte::logmap::get("S1AP")->error("The user to be added with enb id=%d already exists\n",
                                       user->ctxt.enb_ue_s1ap_id);
    return nullptr;
  }
  if (find_ue_mmeid(user->ctxt.mme_ue_s1ap_id) != nullptr) {
    srslte::logmap::get("S1AP")->error("The user to be added with mme id=%d already exists\n",
                                       user->ctxt.mme_ue_s1ap_id);
    return nullptr;
  }
  auto p = users.insert(std::make_pair(user->ctxt.enb_ue_s1ap_id, std::move(user)));
  return p.second ? p.first->second.get() : nullptr;
}

void s1ap::user_list::erase(ue* ue_ptr)
{
  auto it = users.find(ue_ptr->ctxt.enb_ue_s1ap_id);
  if (it == users.end()) {
    srslte::logmap::get("S1AP")->error("User to be erased does not exist\n");
    return;
  }
  users.erase(it);
}

/*******************************************************************************
/* General helpers
********************************************************************************/

bool s1ap::sctp_send_s1ap_pdu(const asn1::s1ap::s1ap_pdu_c& tx_pdu, uint32_t rnti, const char* procedure_name)
{
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool, false);
  if (buf == nullptr) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer for %s.\n", procedure_name);
    return false;
  }
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  tx_pdu.pack(bref);
  buf->N_bytes = bref.distance_bytes();

  // Save message to PCAP
  if (pcap != nullptr) {
    pcap->write_s1ap(buf->msg, buf->N_bytes);
  }

  if (rnti != SRSLTE_INVALID_RNTI) {
    s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending %s for rnti=0x%x", procedure_name, rnti);
  } else {
    s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending %s to MME", procedure_name);
  }
  uint16_t streamid = rnti == SRSLTE_INVALID_RNTI ? NONUE_STREAM_ID : users.find_ue_rnti(rnti)->stream_id;

  ssize_t n_sent = sctp_sendmsg(s1ap_socket.fd(),
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                streamid,
                                0,
                                0);
  if (n_sent == -1) {
    if (rnti > 0) {
      s1ap_log->error("Failed to send %s for rnti=0x%x\n", procedure_name, rnti);
    } else {
      s1ap_log->error("Failed to send %s\n", procedure_name);
    }
    return false;
  }
  return true;
}

/**
 * Helper method to find user based on the enb_ue_s1ap_id stored in an S1AP Msg, and update mme_ue_s1ap_id
 * @param enb_id enb_ue_s1ap_id value stored in S1AP message
 * @param mme_id mme_ue_s1ap_id value stored in S1AP message
 * @return pointer to user if it has been found
 */
s1ap::ue* s1ap::find_s1apmsg_user(uint32_t enb_id, uint32_t mme_id)
{
  ue* user_ptr = users.find_ue_enbid(enb_id);
  if (user_ptr == nullptr) {
    s1ap_log->warning("enb_ue_s1ap_id=%d not found - discarding message\n", enb_id);
    return nullptr;
  }
  if (user_ptr->ctxt.mme_ue_s1ap_id_present and user_ptr->ctxt.mme_ue_s1ap_id != mme_id) {
    s1ap_log->warning("MME_UE_S1AP_ID has changed - old:%d, new:%d\n", user_ptr->ctxt.mme_ue_s1ap_id, mme_id);
  }
  user_ptr->ctxt.mme_ue_s1ap_id_present = true;
  user_ptr->ctxt.mme_ue_s1ap_id         = mme_id;
  return user_ptr;
}

std::string s1ap::get_cause(const cause_c& c)
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

void s1ap::start_pcap(srslte::s1ap_pcap* pcap_)
{
  pcap = pcap_;
}
/*******************************************************************************
/*               s1ap::ue Class
********************************************************************************/

s1ap::ue::ue(s1ap* s1ap_ptr_) : s1ap_ptr(s1ap_ptr_), s1ap_log(s1ap_ptr_->s1ap_log), ho_prep_proc(this)
{
  ctxt.enb_ue_s1ap_id = s1ap_ptr->next_enb_ue_s1ap_id++;
  gettimeofday(&ctxt.init_timestamp, nullptr);

  stream_id = s1ap_ptr->next_ue_stream_id;

  // initialize timers
  ts1_reloc_prep = s1ap_ptr->task_sched.get_unique_timer();
  ts1_reloc_prep.set(ts1_reloc_prep_timeout_ms,
                     [this](uint32_t tid) { ho_prep_proc.trigger(ho_prep_proc_t::ts1_reloc_prep_expired{}); });
  ts1_reloc_overall = s1ap_ptr->task_sched.get_unique_timer();
  ts1_reloc_overall.set(ts1_reloc_overall_timeout_ms, [](uint32_t tid) { /* TODO */ });
}

bool s1ap::ue::send_ho_required(uint32_t                     target_eci,
                                srslte::plmn_id_t            target_plmn,
                                srslte::unique_byte_buffer_t rrc_container)
{
  /*** Setup S1AP PDU as HandoverRequired ***/
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_HO_PREP);
  ho_required_ies_container& container = tx_pdu.init_msg().value.ho_required().protocol_ies;

  /*** fill HO Required message ***/
  container.enb_ue_s1ap_id.value                        = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value                        = ctxt.mme_ue_s1ap_id;
  container.direct_forwarding_path_availability_present = false;                // NOTE: X2 for fwd path not supported
  container.handov_type.value.value               = handov_type_opts::intralte; // NOTE: only intra-LTE HO supported
  container.cause.value.set_radio_network().value = cause_radio_network_opts::unspecified;
  // LIBLTE_S1AP_CAUSERADIONETWORK_S1_INTRA_SYSTEM_HANDOVER_TRIGGERED;

  /*** set the target eNB ***/
  container.csg_id_present           = false; // NOTE: CSG/hybrid target cell not supported
  container.cell_access_mode_present = false; // only for hybrid cells
  // no GERAN/UTRAN/PS
  auto& targetenb = container.target_id.value.set_targetenb_id();
  // set PLMN and TAI of target
  // NOTE: Only HO without TAU supported.
  uint16_t tmp16;
  tmp16 = htons(s1ap_ptr->args.tac);
  memcpy(targetenb.sel_tai.tac.data(), &tmp16, sizeof(uint16_t));
  target_plmn.to_s1ap_plmn_bytes(targetenb.sel_tai.plm_nid.data());
  // NOTE: Only HO to different Macro eNB is supported.
  auto& macroenb = targetenb.global_enb_id.enb_id.set_macro_enb_id();
  target_plmn.to_s1ap_plmn_bytes(targetenb.global_enb_id.plm_nid.data());
  macroenb.from_number(target_eci >> 8u);

  /*** fill the transparent container ***/
  container.source_to_target_transparent_container_secondary_present = false;
  sourceenb_to_targetenb_transparent_container_s transparent_cntr;
  transparent_cntr.erab_info_list_present               = false; // TODO: CHECK
  transparent_cntr.subscriber_profile_idfor_rfp_present = false; // TODO: CHECK
  // - set target cell ID
  target_plmn.to_s1ap_plmn_bytes(transparent_cntr.target_cell_id.plm_nid.data());
  transparent_cntr.target_cell_id.cell_id.from_number(target_eci); // [ENBID|CELLID|0]
  // info specific to source cell and history of UE
  // - set as last visited cell the source eNB PLMN & Cell ID
  transparent_cntr.ue_history_info.resize(1);
  auto& eutra                     = transparent_cntr.ue_history_info[0].set_e_utran_cell();
  eutra.cell_type.cell_size.value = cell_size_opts::medium;
  target_plmn.to_s1ap_plmn_bytes(eutra.global_cell_id.plm_nid.data());
  eutra.global_cell_id.cell_id = s1ap_ptr->eutran_cgi.cell_id;

  // - set time spent in current source cell
  struct timeval ts[3];
  memcpy(&ts[1], &ctxt.init_timestamp, sizeof(struct timeval));
  gettimeofday(&ts[2], nullptr);
  get_time_interval(ts);
  eutra.time_ue_stayed_in_cell = (uint16_t)(ts[0].tv_usec / 1.0e6 + ts[0].tv_sec);
  eutra.time_ue_stayed_in_cell = std::min(eutra.time_ue_stayed_in_cell, (uint16_t)4095);
  // - fill RRC container
  transparent_cntr.rrc_container.resize(rrc_container->N_bytes);
  memcpy(transparent_cntr.rrc_container.data(), rrc_container->msg, rrc_container->N_bytes);

  /*** pack Transparent Container into HORequired message ***/
  uint8_t       buffer[4096];
  asn1::bit_ref bref(buffer, sizeof(buffer));
  if (transparent_cntr.pack(bref) != asn1::SRSASN_SUCCESS) {
    s1ap_log->error("Failed to pack transparent container of HO Required message\n");
    return false;
  }
  container.source_to_target_transparent_container.value.resize(bref.distance_bytes());
  memcpy(container.source_to_target_transparent_container.value.data(), buffer, bref.distance_bytes());

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "HORequired");
}

bool s1ap::ue::send_enb_status_transfer_proc(std::vector<bearer_status_info>& bearer_status_list)
{
  if (bearer_status_list.empty()) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ENB_STATUS_TRANSFER);
  enb_status_transfer_ies_container& container = tx_pdu.init_msg().value.enb_status_transfer().protocol_ies;

  container.enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container.mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id;

  /* Create StatusTransfer transparent container with all the bearer ctxt to transfer */
  auto& list = container.enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list;
  list.resize(bearer_status_list.size());
  for (uint32_t i = 0; i < list.size(); ++i) {
    list[i].load_info_obj(ASN1_S1AP_ID_BEARERS_SUBJECT_TO_STATUS_TRANSFER_ITEM);
    auto&               asn1bearer = list[i].value.bearers_subject_to_status_transfer_item();
    bearer_status_info& item       = bearer_status_list[i];

    asn1bearer.erab_id                = item.erab_id;
    asn1bearer.dl_coun_tvalue.pdcp_sn = item.pdcp_dl_sn;
    asn1bearer.dl_coun_tvalue.hfn     = item.dl_hfn;
    asn1bearer.ul_coun_tvalue.pdcp_sn = item.pdcp_ul_sn;
    asn1bearer.ul_coun_tvalue.hfn     = item.ul_hfn;
    // TODO: asn1bearer.receiveStatusofULPDCPSDUs_present

    //    asn1::json_writer jw;
    //    asn1bearer.to_json(jw);
    //    printf("Bearer to add %s\n", jw.to_string().c_str());
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "ENBStatusTransfer");
}

} // namespace srsenb
