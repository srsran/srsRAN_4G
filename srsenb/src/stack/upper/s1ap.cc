/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/int_helpers.h"

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

  procInfo("Sending HandoverRequired to MME id=%d\n", ue_ptr->ctxt.MME_UE_S1AP_ID);
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
  s1ap_ptr->s1ap_log->console("HO preparation Failure. Cause: %s\n", cause.c_str());

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
  if (msg.protocol_ies.e_ra_bto_release_list_ho_cmd_present) {
    procWarning("Not handling E-RABtoReleaseList\n");
    // TODO
  }

  // Check for E-RABs subject to being forwarded
  if (msg.protocol_ies.e_rab_subjectto_data_forwarding_list_present) {
    procWarning("Not handling E-RABSubjecttoDataForwardingList\n");
    // TODO
  }

  // In case of intra-system Handover, Target to Source Transparent Container IE shall be encoded as
  // Target eNB to Source eNB Transparent Container IE
  uint8_t*      buf = const_cast<uint8_t*>(msg.protocol_ies.target_to_source_transparent_container.value.data());
  asn1::bit_ref bref(buf, msg.protocol_ies.target_to_source_transparent_container.value.size());
  asn1::s1ap::targete_nb_to_sourcee_nb_transparent_container_s container;
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
    procError("Failed to initiate SCTP socket. Attempting reconnection in %d seconds\n",
              s1ap_ptr->mme_connect_timer.duration() / 1000);
    s1ap_ptr->s1ap_log->console("Failed to initiate SCTP socket. Attempting reconnection in %d seconds\n",
                                s1ap_ptr->mme_connect_timer.duration() / 1000);
    s1ap_ptr->mme_connect_timer.run();
    return srslte::proc_outcome_t::error;
  }

  if (not s1ap_ptr->setup_s1()) {
    procError("S1 setup failed. Exiting...\n");
    s1ap_ptr->s1ap_log->console("S1 setup failed\n");
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
  s1ap_ptr->s1ap_log->console("S1setup failed\n");
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

s1ap::s1ap() : s1setup_proc(this) {}

bool s1ap::init(s1ap_args_t                       args_,
                rrc_interface_s1ap*               rrc_,
                srslte::log*                      s1ap_log_,
                srslte::timer_handler*            timers_,
                srsenb::stack_interface_s1ap_lte* stack_)
{
  rrc      = rrc_;
  args     = args_;
  s1ap_log = s1ap_log_;
  timers   = timers_;
  stack    = stack_;
  pool     = srslte::byte_buffer_pool::get_instance();

  build_tai_cgi();

  // Setup MME reconnection timer
  mme_connect_timer    = timers->get_unique_timer();
  auto mme_connect_run = [this](uint32_t tid) {
    if (not s1setup_proc.launch()) {
      s1ap_log->error("Failed to initiate S1Setup procedure.\n");
    }
  };
  mme_connect_timer.set(10000, mme_connect_run);
  // Setup S1Setup timeout
  s1setup_timeout              = timers->get_unique_timer();
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

  return true;
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
  uint32_t tmp32;
  uint16_t tmp16;

  // TAI
  tai.ext                   = false;
  tai.iE_Extensions_present = false;
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tmp32                      = htonl(plmn);
  tai.pLMNidentity.buffer[0] = ((uint8_t*)&tmp32)[1];
  tai.pLMNidentity.buffer[1] = ((uint8_t*)&tmp32)[2];
  tai.pLMNidentity.buffer[2] = ((uint8_t*)&tmp32)[3];
  tmp16                      = htons(args.tac);
  memcpy(tai.tAC.buffer, (uint8_t*)&tmp16, 2);

  // EUTRAN_CGI
  eutran_cgi.ext                   = false;
  eutran_cgi.iE_Extensions_present = false;
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tmp32                             = htonl(plmn);
  eutran_cgi.pLMNidentity.buffer[0] = ((uint8_t*)&tmp32)[1];
  eutran_cgi.pLMNidentity.buffer[1] = ((uint8_t*)&tmp32)[2];
  eutran_cgi.pLMNidentity.buffer[2] = ((uint8_t*)&tmp32)[3];

  tmp32 = htonl(args.enb_id);
  uint8_t enb_id_bits[4 * 8];
  liblte_unpack((uint8_t*)&tmp32, 4, enb_id_bits);
  uint8_t cell_id_bits[1 * 8];
  liblte_unpack(&args.cell_id, 1, cell_id_bits);
  memcpy(eutran_cgi.cell_ID.buffer,
         &enb_id_bits[32 - LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN],
         LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);
  memcpy(&eutran_cgi.cell_ID.buffer[LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN], cell_id_bits, 8);
}

/*******************************************************************************
/* RRC interface
********************************************************************************/
void s1ap::initial_ue(uint16_t rnti, asn1::s1ap::rrc_establishment_cause_e cause, srslte::unique_byte_buffer_t pdu)
{
  users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue{rnti, this})));
  send_initialuemessage(rnti, cause, std::move(pdu), false);
}

void s1ap::initial_ue(uint16_t                              rnti,
                      asn1::s1ap::rrc_establishment_cause_e cause,
                      srslte::unique_byte_buffer_t          pdu,
                      uint32_t                              m_tmsi,
                      uint8_t                               mmec)
{
  users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue{rnti, this})));
  send_initialuemessage(rnti, cause, std::move(pdu), true, m_tmsi, mmec);
}

void s1ap::write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  s1ap_log->info_hex(pdu->msg, pdu->N_bytes, "Received RRC SDU");

  if (get_user_ctxt(rnti) != nullptr) {
    send_ulnastransport(rnti, std::move(pdu));
  }
}

bool s1ap::user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio)
{
  s1ap_log->info("User inactivity - RNTI:0x%x\n", rnti);

  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (ctxt == nullptr) {
    return false;
  }

  if (ctxt->release_requested) {
    s1ap_log->warning("UE context for RNTI:0x%x is in zombie state. Releasing...\n", rnti);
    users.erase(rnti);
    rrc->release_complete(rnti);
    return false;
  }

  cause_c cause;
  cause.set_radio_network().value = cause_radio.value;

  ctxt->release_requested = true;
  return send_uectxtreleaserequest(rnti, cause);
}

bool s1ap::user_exists(uint16_t rnti)
{
  return users.end() != users.find(rnti);
}

void s1ap::ue_ctxt_setup_complete(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res)
{
  if (res.protocol_ies.e_rab_setup_list_ctxt_su_res.value.size() > 0) {
    send_initial_ctxt_setup_response(rnti, res);
  } else {
    send_initial_ctxt_setup_failure(rnti);
  }
}

void s1ap::ue_erab_setup_complete(uint16_t rnti, const asn1::s1ap::e_rab_setup_resp_s& res)
{
  send_erab_setup_response(rnti, res);
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
          &s1ap_socket, srslte::net_utils::socket_type::seqpacket, args.s1c_bind_addr.c_str(), s1ap_log)) {
    return false;
  }

  // Connect to the MME address
  if (not s1ap_socket.connect_to(args.mme_addr.c_str(), MME_PORT, &mme_addr, s1ap_log)) {
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
      s1ap_log->console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
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
  s1ap_pdu_c    rx_pdu;
  asn1::bit_ref bref(pdu->msg, pdu->N_bytes);

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
    case s1ap_elem_procs_o::init_msg_c::types_opts::e_rab_setup_request:
      return handle_erabsetuprequest(msg.value.e_rab_setup_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request:
      return handle_uecontextmodifyrequest(msg.value.ue_context_mod_request());
    default:
      s1ap_log->error("Unhandled intiating message: %s\n", msg.value.type().to_string().c_str());
  }
  return true;
}

bool s1ap::handle_successfuloutcome(const successful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::s1_setup_request:
      return handle_s1setupresponse(msg.value.s1_setup_request());
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::ho_required:
      return handle_s1hocommand(msg.value.ho_required());
    default:
      s1ap_log->error("Unhandled successful outcome message: %s\n", msg.value.type().to_string().c_str());
  }
  return true;
}

bool s1ap::handle_unsuccessfuloutcome(const unsuccessful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::s1_setup_request:
      return handle_s1setupfailure(msg.value.s1_setup_request());
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_required:
      return handle_hopreparationfailure(msg.value.ho_required());
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
  if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg.protocol_ies.enb_ue_s1ap_id.value.value)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti      = enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value];
  ue_ctxt_t* ctxt      = get_user_ctxt(rnti);
  ctxt->MME_UE_S1AP_ID = msg.protocol_ies.mme_ue_s1ap_id.value.value;

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
  rrc->write_dl_info(rnti, std::move(pdu));
  return true;
}

bool s1ap::handle_initialctxtsetuprequest(const init_context_setup_request_s& msg)
{
  s1ap_log->info("Received InitialContextSetupRequest\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg.protocol_ies.enb_ue_s1ap_id.value.value)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg.protocol_ies.mme_ue_s1ap_id.value.value != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning("MME_UE_S1AP_ID has changed - old:%d, new: %" PRIu64 "\n",
                      ctxt->MME_UE_S1AP_ID,
                      msg.protocol_ies.mme_ue_s1ap_id.value.value);
    ctxt->MME_UE_S1AP_ID = msg.protocol_ies.mme_ue_s1ap_id.value.value;
  }

  // Setup UE ctxt in RRC
  if (!rrc->setup_ue_ctxt(rnti, msg)) {
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
      ctxt->release_requested = true;
      send_uectxtreleaserequest(rnti, cause);
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

bool s1ap::handle_erabsetuprequest(const e_rab_setup_request_s& msg)
{
  s1ap_log->info("Received ERABSetupRequest\n");
  if (msg.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }

  if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg.protocol_ies.enb_ue_s1ap_id.value.value)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg.protocol_ies.mme_ue_s1ap_id.value.value != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning("MME_UE_S1AP_ID has changed - old:%d, new:%" PRIu64 "\n",
                      ctxt->MME_UE_S1AP_ID,
                      msg.protocol_ies.mme_ue_s1ap_id.value.value);
    ctxt->MME_UE_S1AP_ID = msg.protocol_ies.mme_ue_s1ap_id.value.value;
  }

  // Setup UE ctxt in RRC
  return rrc->setup_ue_erabs(rnti, msg);
}

bool s1ap::handle_uecontextmodifyrequest(const ue_context_mod_request_s& msg)
{
  s1ap_log->info("Received UeContextModificationRequest\n");
  if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg.protocol_ies.enb_ue_s1ap_id.value.value)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg.protocol_ies.mme_ue_s1ap_id.value.value != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning("MME_UE_S1AP_ID has changed - old:%d, new:%" PRIu64 "\n",
                      ctxt->MME_UE_S1AP_ID,
                      msg.protocol_ies.mme_ue_s1ap_id.value.value);
    ctxt->MME_UE_S1AP_ID = msg.protocol_ies.mme_ue_s1ap_id.value.value;
  }

  if (!rrc->modify_ue_ctxt(rnti, msg)) {
    cause_c cause;
    cause.set_misc().value = cause_misc_opts::unspecified;
    send_uectxmodifyfailure(rnti, cause);
    return true;
  }

  // Send UEContextModificationResponse
  send_uectxmodifyresp(rnti);

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg.protocol_ies.cs_fallback_ind_present) {
    if (msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_required ||
        msg.protocol_ies.cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_high_prio) {
      // Send RRC Release (cs-fallback-triggered) to MME
      cause_c cause;
      cause.set_radio_network().value = cause_radio_network_opts::cs_fallback_triggered;

      ctxt->release_requested = true;
      send_uectxtreleaserequest(rnti, cause);
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

  uint16_t rnti = 0;
  if (msg.protocol_ies.ue_s1ap_ids.value.type().value == ue_s1ap_ids_c::types_opts::ue_s1ap_id_pair) {
    auto& idpair = msg.protocol_ies.ue_s1ap_ids.value.ue_s1ap_id_pair();

    if (idpair.ext) {
      s1ap_log->warning("Not handling S1AP message extension\n");
    }
    if (idpair.ie_exts_present) {
      s1ap_log->warning("Not handling S1AP message iE_Extensions\n");
    }
    uint32_t enb_ue_id = idpair.enb_ue_s1ap_id;
    if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(enb_ue_id)) {
      s1ap_log->warning("eNB_UE_S1AP_ID:%d not found - discarding message\n", enb_ue_id);
      return false;
    }
    rnti = enbid_to_rnti_map[enb_ue_id];
    enbid_to_rnti_map.erase(enb_ue_id);

  } else { // LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_MME_UE_S1AP_ID

    uint32_t mme_ue_id = msg.protocol_ies.ue_s1ap_ids.value.mme_ue_s1ap_id();
    uint32_t enb_ue_id;
    if (!find_mme_ue_id(mme_ue_id, &rnti, &enb_ue_id)) {
      s1ap_log->warning("UE for MME_UE_S1AP_ID:%d not found - discarding message\n", mme_ue_id);
      return false;
    }
    enbid_to_rnti_map.erase(enb_ue_id);
  }

  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (ctxt == nullptr) {
    return false;
  }

  rrc->release_erabs(rnti);
  send_uectxtreleasecomplete(rnti, ctxt->MME_UE_S1AP_ID, ctxt->eNB_UE_S1AP_ID);
  users.erase(rnti);
  s1ap_log->info("UE context for RNTI:0x%x released\n", rnti);
  rrc->release_complete(rnti);
  return true;
}

bool s1ap::handle_s1setupfailure(const asn1::s1ap::s1_setup_fail_s& msg)
{
  std::string cause = get_cause(msg.protocol_ies.cause.value);
  s1ap_log->error("S1 Setup Failure. Cause: %s\n", cause.c_str());
  s1ap_log->console("S1 Setup Failure. Cause: %s\n", cause.c_str());
  return true;
}

bool s1ap::handle_hopreparationfailure(const ho_prep_fail_s& msg)
{
  auto user_it = users.find(enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value]);
  if (user_it == users.end()) {
    s1ap_log->error("user rnti=0x%x no longer exists\n", user_it->first);
  }
  user_it->second->get_ho_prep_proc().trigger(msg);
  return true;
}

bool s1ap::handle_s1hocommand(const asn1::s1ap::ho_cmd_s& msg)
{
  auto user_it = users.find(enbid_to_rnti_map[msg.protocol_ies.enb_ue_s1ap_id.value.value]);
  if (user_it == users.end()) {
    s1ap_log->error("user rnti=0x%x no longer exists\n", user_it->first);
    return false;
  }
  user_it->second->get_ho_prep_proc().trigger(msg);
  return true;
}

/*******************************************************************************
/* S1AP message senders
********************************************************************************/

bool s1ap::send_initialuemessage(uint16_t                              rnti,
                                 asn1::s1ap::rrc_establishment_cause_e cause,
                                 srslte::unique_byte_buffer_t          pdu,
                                 bool                                  has_tmsi,
                                 uint32_t                              m_tmsi,
                                 uint8_t                               mmec)
{
  if (!mme_connected) {
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
  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // NAS_PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // TAI
  container.tai.value.ie_exts_present = tai.iE_Extensions_present;
  container.tai.value.ext             = tai.ext;
  memcpy(container.tai.value.tac.data(), tai.tAC.buffer, 2);
  memcpy(container.tai.value.plm_nid.data(), tai.pLMNidentity.buffer, 3);

  // EUTRAN_CGI
  container.eutran_cgi.value.ext             = eutran_cgi.ext;
  container.eutran_cgi.value.ie_exts_present = eutran_cgi.iE_Extensions_present;
  memcpy(container.eutran_cgi.value.plm_nid.data(), eutran_cgi.pLMNidentity.buffer, 3);
  for (uint32_t i = 0; i < 28; ++i) {
    container.eutran_cgi.value.cell_id.set(i, (bool)eutran_cgi.cell_ID.buffer[i]);
  }

  // RRC Establishment Cause
  container.rrc_establishment_cause.value = cause;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "InitialUEMessage");
}

bool s1ap::send_ulnastransport(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UL_NAS_TRANSPORT);
  asn1::s1ap::ul_nas_transport_ies_container& container = tx_pdu.init_msg().value.ul_nas_transport().protocol_ies;
  // MME_UE_S1AP_ID
  container.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  // ENB_UE_S1AP_ID
  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // NAS PDU
  container.nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container.nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // EUTRAN CGI
  container.eutran_cgi.value.ext             = eutran_cgi.ext;
  container.eutran_cgi.value.ie_exts_present = eutran_cgi.iE_Extensions_present;
  memcpy(container.eutran_cgi.value.plm_nid.data(), eutran_cgi.pLMNidentity.buffer, 3);
  for (uint32_t i = 0; i < 28; ++i) {
    container.eutran_cgi.value.cell_id.set(i, (bool)eutran_cgi.cell_ID.buffer[i]);
  }

  // TAI
  container.tai.value.ie_exts_present = tai.iE_Extensions_present;
  container.tai.value.ext             = tai.ext;
  memcpy(container.tai.value.tac.data(), tai.tAC.buffer, 2);
  memcpy(container.tai.value.plm_nid.data(), tai.pLMNidentity.buffer, 3);

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "UplinkNASTransport");
}

bool s1ap::send_uectxtreleaserequest(uint16_t rnti, const cause_c& cause)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE_REQUEST);
  ue_context_release_request_ies_container& container =
      tx_pdu.init_msg().value.ue_context_release_request().protocol_ies;

  // MME_UE_S1AP_ID
  container.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  // ENB_UE_S1AP_ID
  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // Cause
  container.cause.value = cause;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "UEContextReleaseRequest");
}

bool s1ap::send_uectxtreleasecomplete(uint16_t rnti, uint32_t mme_ue_id, uint32_t enb_ue_id)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE);
  auto& container                = tx_pdu.successful_outcome().value.ue_context_release_cmd().protocol_ies;
  container.enb_ue_s1ap_id.value = enb_ue_id;
  container.mme_ue_s1ap_id.value = mme_ue_id;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "UEContextReleaseComplete");
}

bool s1ap::send_initial_ctxt_setup_response(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res_)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);

  // Copy in the provided response message
  tx_pdu.successful_outcome().value.init_context_setup_request() = res_;

  // Fill in the MME and eNB IDs
  auto& container                = tx_pdu.successful_outcome().value.init_context_setup_request().protocol_ies;
  container.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // Fill in the GTP bind address for all bearers
  for (uint32_t i = 0; i < container.e_rab_setup_list_ctxt_su_res.value.size(); ++i) {
    auto& item = container.e_rab_setup_list_ctxt_su_res.value[i].value.e_rab_setup_item_ctxt_su_res();
    item.transport_layer_address.resize(32);
    uint8_t addr[4];
    inet_pton(AF_INET, args.gtp_bind_addr.c_str(), addr);
    for (uint32_t j = 0; j < 4; ++j) {
      item.transport_layer_address.data()[j] = addr[3 - j];
    }
  }

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "InitialContextSetupResponse");
}

bool s1ap::send_erab_setup_response(uint16_t rnti, const e_rab_setup_resp_s& res_)
{
  if (!mme_connected) {
    return false;
  }

  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_E_RAB_SETUP);
  e_rab_setup_resp_s& res = tx_pdu.successful_outcome().value.e_rab_setup_request();

  res = res_;

  // Fill in the GTP bind address for all bearers
  if (res.protocol_ies.e_rab_setup_list_bearer_su_res_present) {
    for (uint32_t i = 0; i < res.protocol_ies.e_rab_setup_list_bearer_su_res.value.size(); ++i) {
      auto& item = res.protocol_ies.e_rab_setup_list_bearer_su_res.value[i].value.e_rab_setup_item_bearer_su_res();
      item.transport_layer_address.resize(32);
      uint8_t addr[4];
      inet_pton(AF_INET, args.gtp_bind_addr.c_str(), addr);
      for (uint32_t j = 0; j < 4; ++j) {
        item.transport_layer_address.data()[j] = addr[3 - j];
      }
    }
  }

  // Fill in the MME and eNB IDs
  res.protocol_ies.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  res.protocol_ies.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "E_RABSetupResponse");
}

bool s1ap::send_initial_ctxt_setup_failure(uint16_t rnti)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
  auto& container = tx_pdu.unsuccessful_outcome().value.init_context_setup_request().protocol_ies;

  container.enb_ue_s1ap_id.value                  = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  container.mme_ue_s1ap_id.value                  = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;
  container.cause.value.set_radio_network().value = cause_radio_network_opts::unspecified;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "InitialContextSetupFailure");
}

bool s1ap::send_uectxmodifyresp(uint16_t rnti)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.successful_outcome().value.ue_context_mod_request().protocol_ies;

  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  container.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "UEContextModificationResponse");
}

bool s1ap::send_uectxmodifyfailure(uint16_t rnti, const cause_c& cause)
{
  if (!mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.unsuccessful_outcome().value.ue_context_mod_request().protocol_ies;

  container.enb_ue_s1ap_id.value = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  container.mme_ue_s1ap_id.value = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;
  container.cause.value          = cause;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "UEContextModificationFailure");
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
  auto it = users.find(rnti);
  if (it == users.end()) {
    return false;
  }

  // launch procedure
  if (not it->second->get_ho_prep_proc().launch(target_eci, target_plmn, std::move(rrc_container))) {
    s1ap_log->error("Failed to initiate an HandoverPreparation procedure for user rnti=0x%x\n",
                    it->second->get_ctxt().rnti);
    return false;
  }
  return true;
}

bool s1ap::send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list)
{
  if (not mme_connected) {
    return false;
  }
  auto it = users.find(rnti);
  if (it == users.end()) {
    return false;
  }

  return it->second->send_enb_status_transfer_proc(bearer_status_list);
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
//  caps->MME_UE_S1AP_ID.MME_UE_S1AP_ID = ue_ctxt_map[rnti]->MME_UE_S1AP_ID;
//  caps->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = ue_ctxt_map[rnti]->eNB_UE_S1AP_ID;
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
  if (rnti > 0) {
    s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending %s for rnti=0x%x", procedure_name, rnti);
  } else {
    s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending %s to MME", procedure_name);
  }
  uint16_t streamid = rnti == 0 ? NONUE_STREAM_ID : get_user_ctxt(rnti)->stream_id;

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

bool s1ap::find_mme_ue_id(uint32_t mme_ue_id, uint16_t* rnti, uint32_t* enb_ue_id)
{
  for (auto& it : users) {
    if (it.second->get_ctxt().MME_UE_S1AP_ID == mme_ue_id) {
      *rnti      = it.second->get_ctxt().rnti;
      *enb_ue_id = it.second->get_ctxt().eNB_UE_S1AP_ID;
      return true;
    }
  }
  return false;
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

ue_ctxt_t* s1ap::get_user_ctxt(uint16_t rnti)
{
  auto it = users.find(rnti);
  if (it == users.end()) {
    s1ap_log->warning("User rnti=0x%x context not found\n", rnti);
    return nullptr;
  }
  return &it->second->get_ctxt();
}

/*******************************************************************************
/*               s1ap::ue Class
********************************************************************************/

s1ap::ue::ue(uint16_t rnti_, s1ap* s1ap_ptr_) : s1ap_ptr(s1ap_ptr_), s1ap_log(s1ap_ptr_->s1ap_log), ho_prep_proc(this)
{
  ctxt.rnti              = rnti_;
  ctxt.eNB_UE_S1AP_ID    = s1ap_ptr->next_eNB_UE_S1AP_ID++;
  ctxt.stream_id         = 1;
  ctxt.release_requested = false;
  gettimeofday(&ctxt.init_timestamp, nullptr);
  s1ap_ptr->enbid_to_rnti_map[ctxt.eNB_UE_S1AP_ID] = ctxt.rnti;

  // initialize timers
  ts1_reloc_prep = s1ap_ptr->timers->get_unique_timer();
  ts1_reloc_prep.set(ts1_reloc_prep_timeout_ms,
                     [this](uint32_t tid) { ho_prep_proc.trigger(ho_prep_proc_t::ts1_reloc_prep_expired{}); });
  ts1_reloc_overall = s1ap_ptr->timers->get_unique_timer();
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
  container.enb_ue_s1ap_id.value                        = ctxt.eNB_UE_S1AP_ID;
  container.mme_ue_s1ap_id.value                        = ctxt.MME_UE_S1AP_ID;
  container.direct_forwarding_path_availability_present = false;                // NOTE: X2 for fwd path not supported
  container.handov_type.value.value               = handov_type_opts::intralte; // NOTE: only intra-LTE HO supported
  container.cause.value.set_radio_network().value = cause_radio_network_opts::unspecified;
  // LIBLTE_S1AP_CAUSERADIONETWORK_S1_INTRA_SYSTEM_HANDOVER_TRIGGERED;

  /*** set the target eNB ***/
  container.csg_id_present           = false; // NOTE: CSG/hybrid target cell not supported
  container.cell_access_mode_present = false; // only for hybrid cells
  // no GERAN/UTRAN/PS
  auto& targetenb = container.target_id.value.set_targete_nb_id();
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
  sourcee_nb_to_targete_nb_transparent_container_s transparent_cntr;
  transparent_cntr.e_rab_info_list_present              = false; // TODO: CHECK
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
  for (uint32_t i = 0; i < LIBLTE_S1AP_CELLIDENTITY_BIT_STRING_LEN; ++i) {
    eutra.global_cell_id.cell_id.set(i, s1ap_ptr->eutran_cgi.cell_ID.buffer[i]);
  }
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
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_E_NB_STATUS_TRANSFER);
  enb_status_transfer_ies_container& container = tx_pdu.init_msg().value.enb_status_transfer().protocol_ies;

  container.enb_ue_s1ap_id.value = ctxt.eNB_UE_S1AP_ID;
  container.mme_ue_s1ap_id.value = ctxt.MME_UE_S1AP_ID;

  /* Create StatusTransfer transparent container with all the bearer ctxt to transfer */
  auto& list = container.enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list;
  list.resize(bearer_status_list.size());
  for (uint32_t i = 0; i < list.size(); ++i) {
    list[i].load_info_obj(ASN1_S1AP_ID_BEARERS_SUBJECT_TO_STATUS_TRANSFER_ITEM);
    auto&               asn1bearer = list[i].value.bearers_subject_to_status_transfer_item();
    bearer_status_info& item       = bearer_status_list[i];

    asn1bearer.e_rab_id               = item.erab_id;
    asn1bearer.dl_coun_tvalue.pdcp_sn = item.pdcp_dl_sn;
    asn1bearer.dl_coun_tvalue.hfn     = item.dl_hfn;
    asn1bearer.ul_coun_tvalue.pdcp_sn = item.pdcp_ul_sn;
    asn1bearer.ul_coun_tvalue.hfn     = item.ul_hfn;
    // TODO: asn1bearer.receiveStatusofULPDCPSDUs_present
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "ENBStatusTransfer");
}

} // namespace srsenb
