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

#include <stdio.h>
#include <string.h>
#include <unistd.h>		//for close(), sleep()
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>	//for inet_ntop()

using srslte::s1ap_mccmnc_to_plmn;
using srslte::uint32_to_uint8;

#define procError(fmt, ...) s1ap_ptr->s1ap_log->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procInfo(fmt, ...) s1ap_ptr->s1ap_log->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

namespace srsenb {

/*********************************************************
 * TS 36.413 - Section 8.4.1 - "Handover Preparation"
 *********************************************************/
s1ap::ue::ho_prep_proc_t::ho_prep_proc_t(s1ap::ue* ue_) : ue_ptr(ue_), s1ap_ptr(ue_->s1ap_ptr) {}

srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::init(uint32_t                     target_eci_,
                                                      srslte::plmn_id_t            target_plmn_,
                                                      srslte::unique_byte_buffer_t rrc_container)
{
  target_eci  = target_eci_;
  target_plmn = target_plmn_;

  procInfo("Sending HandoverRequired to MME id=%d\n", ue_ptr->ctxt.MME_UE_S1AP_ID);
  if (not ue_ptr->send_ho_required(target_eci, target_plmn, std::move(rrc_container))) {
    procError("Failed to send HORequired to cell 0x%x\n", target_eci);
    return srslte::proc_outcome_t::error;
  }

  ue_ptr->ts1_reloc_prep.run();
  return srslte::proc_outcome_t::yield;
}
srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(ts1_reloc_prep_expired e)
{
  // do nothing for now
  procError("timer TS1Relocprep has expired.\n");
  return srslte::proc_outcome_t::error;
}
srslte::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(const LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT& msg)
{
  std::string cause = s1ap_ptr->get_cause(&msg.Cause);
  procError("HO preparation Failure. Cause: %s\n", cause.c_str());
  s1ap_ptr->s1ap_log->console("HO preparation Failure. Cause: %s\n", cause.c_str());
  return srslte::proc_outcome_t::error;
}

void s1ap::ue::ho_prep_proc_t::then(const srslte::proc_state_t& result)
{
  s1ap_ptr->rrc->ho_preparation_complete(ue_ptr->ctxt.rnti, result.is_success());
}

/*********************************************************
 *                     S1AP class
 *********************************************************/

s1ap::s1ap() :
  thread("S1AP"),
  rrc(nullptr),
  s1ap_log(nullptr),
  pool(nullptr),
  mme_connected(false),
  running(false),
  next_eNB_UE_S1AP_ID(1),
  next_ue_stream_id(1)
{
}

bool s1ap::init(s1ap_args_t args_, rrc_interface_s1ap* rrc_, srslte::log* s1ap_log_, srslte::timer_handler* timers_)
{
  rrc      = rrc_;
  args     = args_;
  s1ap_log = s1ap_log_;
  timers   = timers_;

  pool                = srslte::byte_buffer_pool::get_instance();
  mme_connected       = false;
  running             = false;
  next_eNB_UE_S1AP_ID = 1;
  next_ue_stream_id   = 1;

  build_tai_cgi();

  start(S1AP_THREAD_PRIO);

  return true;
}

void s1ap::stop()
{
  if(running) {
    running = false;
    thread_cancel();
    wait_thread_finish();
  }

  if(close(socket_fd) == -1) {
    s1ap_log->error("Failed to close SCTP socket\n");
  }
}

void s1ap::get_metrics(s1ap_metrics_t &m)
{
  if(!running) {
    m.status = S1AP_ERROR;
    return;
  }
  if(mme_connected) {
    m.status = S1AP_READY;
  } else {
    m.status = S1AP_ATTACHING;
  }
}

void s1ap::run_thread()
{
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, "s1ap::run_thread");
  if (!pdu) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::run_thread().\n");
    return;
  }

  uint32_t sz = SRSLTE_MAX_BUFFER_SIZE_BYTES - SRSLTE_BUFFER_HEADER_OFFSET;
  running = true;

  // Connect to MME
  while(running && !connect_mme()) {
    s1ap_log->error("Failed to connect to MME - retrying in 10 seconds\n");
    s1ap_log->console("Failed to connect to MME - retrying in 10 seconds\n");
    sleep(10);
  }
  if(!setup_s1()) {
    s1ap_log->error("S1 setup failed\n");
    s1ap_log->console("S1 setup failed\n");
    running = false;
    return;
  }

  // S1AP rx loop
  while(running) {
    pdu->clear();
    ssize_t n_recv = recv(socket_fd, pdu->msg, sz, 0);
    if (n_recv <= 0) {
      mme_connected = false;
      do {
        s1ap_log->error("Disconnected - attempting reconnection in 10 seconds\n");
        s1ap_log->console("Disconnected - attempting reconnection in 10 seconds\n");
        sleep(10);
      } while(running && !connect_mme());

      if(!setup_s1()) {
        s1ap_log->error("S1 setup failed\n");
        s1ap_log->console("S1 setup failed\n");
        running = false;
        return;
      }
    }

    pdu->N_bytes = static_cast<uint32_t>(n_recv);

    s1ap_log->info_hex(pdu->msg, pdu->N_bytes, "Received S1AP PDU");
    handle_s1ap_rx_pdu(pdu.get());
  }
  printf("%s ended\n", __PRETTY_FUNCTION__);
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
  tmp32 = htonl(plmn);
  tai.pLMNidentity.buffer[0] = ((uint8_t*)&tmp32)[1];
  tai.pLMNidentity.buffer[1] = ((uint8_t*)&tmp32)[2];
  tai.pLMNidentity.buffer[2] = ((uint8_t*)&tmp32)[3];
  tmp16 = htons(args.tac);
  memcpy(tai.tAC.buffer, (uint8_t*)&tmp16, 2);

  // EUTRAN_CGI
  eutran_cgi.ext                    = false;
  eutran_cgi.iE_Extensions_present  = false;
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tmp32 = htonl(plmn);
  eutran_cgi.pLMNidentity.buffer[0] = ((uint8_t*)&tmp32)[1];
  eutran_cgi.pLMNidentity.buffer[1] = ((uint8_t*)&tmp32)[2];
  eutran_cgi.pLMNidentity.buffer[2] = ((uint8_t*)&tmp32)[3];

  tmp32 = htonl(args.enb_id);
  uint8_t enb_id_bits[4*8];
  liblte_unpack((uint8_t*)&tmp32, 4, enb_id_bits);
  uint8_t cell_id_bits[1*8];
  liblte_unpack(&args.cell_id, 1, cell_id_bits);
  memcpy(eutran_cgi.cell_ID.buffer, &enb_id_bits[32-LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN], LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);
  memcpy(&eutran_cgi.cell_ID.buffer[LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN], cell_id_bits, 8);
}

/*******************************************************************************
/* RRC interface
********************************************************************************/
void s1ap::initial_ue(uint16_t rnti, LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause, srslte::unique_byte_buffer_t pdu)
{
  users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue{rnti, this})));
  send_initialuemessage(rnti, cause, std::move(pdu), false);
}

void s1ap::initial_ue(uint16_t                                 rnti,
                      LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause,
                      srslte::unique_byte_buffer_t             pdu,
                      uint32_t                                 m_tmsi,
                      uint8_t                                  mmec)
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

bool s1ap::user_release(uint16_t rnti, LIBLTE_S1AP_CAUSERADIONETWORK_ENUM cause_radio)
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

  LIBLTE_S1AP_CAUSE_STRUCT cause;
  cause.ext                     = false;
  cause.choice_type             = LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK;
  cause.choice.radioNetwork.ext = false;
  cause.choice.radioNetwork.e   = cause_radio;

  ctxt->release_requested = true;
  return send_uectxtreleaserequest(rnti, &cause);
}

bool s1ap::user_exists(uint16_t rnti)
{
  return users.end() != users.find(rnti);
}

void s1ap::ue_ctxt_setup_complete(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT* res)
{
  if(res->E_RABSetupListCtxtSURes.len > 0) {
    send_initial_ctxt_setup_response(rnti, res);
  } else {
    send_initial_ctxt_setup_failure(rnti);
  }
}

void s1ap::ue_erab_setup_complete(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT *res)
{
  send_erab_setup_response(rnti, res);
}

//void ue_capabilities(uint16_t rnti, LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *caps)
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
  socket_fd = 0;

  s1ap_log->info("Connecting to MME %s:%d\n", args.mme_addr.c_str(), MME_PORT);

  if((socket_fd = socket(ADDR_FAMILY, SOCK_TYPE, PROTO)) == -1) {
    s1ap_log->error("Failed to create S1AP socket\n");
    return false;
  }

  // Bind to the local address
  struct sockaddr_in local_addr;
  memset(&local_addr, 0, sizeof(struct sockaddr_in));
  local_addr.sin_family = ADDR_FAMILY;
  local_addr.sin_port = 0;  // Any local port will do
  if(inet_pton(AF_INET, args.s1c_bind_addr.c_str(), &(local_addr.sin_addr)) != 1) {
    s1ap_log->error("Error converting IP address (%s) to sockaddr_in structure\n", args.s1c_bind_addr.c_str());
    return false;
  }
  if (bind(socket_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) != 0) {
    s1ap_log->error("Failed to bind on S1-C address %s: %s errno %d\n", args.s1c_bind_addr.c_str(), strerror(errno), errno);
    return false; 
  }

  // Connect to the MME address
  memset(&mme_addr, 0, sizeof(struct sockaddr_in));
  mme_addr.sin_family = ADDR_FAMILY;
  mme_addr.sin_port = htons(MME_PORT);
  if(inet_pton(AF_INET, args.mme_addr.c_str(), &(mme_addr.sin_addr)) != 1) {
    s1ap_log->error("Error converting IP address (%s) to sockaddr_in structure\n", args.mme_addr.c_str());
    return false;
  }

  if(connect(socket_fd, (struct sockaddr*)&mme_addr, sizeof(mme_addr)) == -1) {
    s1ap_log->error("Failed to establish socket connection to MME\n");
    return false;
  }

  s1ap_log->info("SCTP socket established with MME\n");
  return true;
}

bool s1ap::setup_s1()
{
  uint32_t                    tmp32;
  uint16_t                    tmp16;
  srslte::byte_buffer_t       msg;
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));

  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &pdu.choice.initiatingMessage;

  init->procedureCode = LIBLTE_S1AP_PROC_ID_S1SETUP;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_S1SETUPREQUEST;

  LIBLTE_S1AP_MESSAGE_S1SETUPREQUEST_STRUCT *s1setup = &init->choice.S1SetupRequest;
  s1setup->ext                = false;
  s1setup->CSG_IdList_present = false;

  s1setup->Global_ENB_ID.ext = false;
  s1setup->Global_ENB_ID.iE_Extensions_present = false;
  uint32_t plmn;
  s1ap_mccmnc_to_plmn(args.mcc, args.mnc, &plmn);
  tmp32 = htonl(plmn);
  s1setup->Global_ENB_ID.pLMNidentity.buffer[0] = ((uint8_t*)&tmp32)[1];
  s1setup->Global_ENB_ID.pLMNidentity.buffer[1] = ((uint8_t*)&tmp32)[2];
  s1setup->Global_ENB_ID.pLMNidentity.buffer[2] = ((uint8_t*)&tmp32)[3];

  s1setup->Global_ENB_ID.ext = false;
  s1setup->Global_ENB_ID.eNB_ID.ext = false;
  s1setup->Global_ENB_ID.eNB_ID.choice_type = LIBLTE_S1AP_ENB_ID_CHOICE_MACROENB_ID;
  tmp32 = htonl(args.enb_id);
  uint8_t  enb_id_bits[4*8];
  liblte_unpack((uint8_t*)&tmp32, 4, enb_id_bits);
  memcpy(s1setup->Global_ENB_ID.eNB_ID.choice.macroENB_ID.buffer, &enb_id_bits[32-LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN], LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);

  s1setup->eNBname_present    = true;
  s1setup->eNBname.ext        = false;
  if(args.enb_name.length() >= 150) {
      args.enb_name.resize(150-1);
  }
  memcpy(s1setup->eNBname.buffer, args.enb_name.c_str(), args.enb_name.length());
  s1setup->eNBname.n_octets = args.enb_name.length();

  s1setup->SupportedTAs.len = 1;
  s1setup->SupportedTAs.buffer[0].ext = false;
  s1setup->SupportedTAs.buffer[0].iE_Extensions_present = false;
  tmp16 = htons(args.tac);
  memcpy(s1setup->SupportedTAs.buffer[0].tAC.buffer, (uint8_t*)&tmp16, 2);
  s1setup->SupportedTAs.buffer[0].broadcastPLMNs.len = 1;
  tmp32 = htonl(plmn);
  s1setup->SupportedTAs.buffer[0].broadcastPLMNs.buffer[0].buffer[0] = ((uint8_t*)&tmp32)[1];
  s1setup->SupportedTAs.buffer[0].broadcastPLMNs.buffer[0].buffer[1] = ((uint8_t*)&tmp32)[2];
  s1setup->SupportedTAs.buffer[0].broadcastPLMNs.buffer[0].buffer[2] = ((uint8_t*)&tmp32)[3];

  s1setup->DefaultPagingDRX.ext = false;
  s1setup->DefaultPagingDRX.e   = LIBLTE_S1AP_PAGINGDRX_V128; // Todo: add to args, config file

  liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending s1SetupRequest");

  ssize_t n_sent = sctp_sendmsg(socket_fd, msg.msg, msg.N_bytes,
                                (struct sockaddr*)&mme_addr, sizeof(struct sockaddr_in),
                                htonl(PPID), 0, NONUE_STREAM_ID, 0, 0);
  if(n_sent == -1) {
    s1ap_log->error("Failed to send s1SetupRequest\n");
    return false;
  }

  return true;
}

/*******************************************************************************
/* S1AP message handlers
********************************************************************************/

bool s1ap::handle_s1ap_rx_pdu(srslte::byte_buffer_t* pdu)
{
  LIBLTE_S1AP_S1AP_PDU_STRUCT rx_pdu;

  if(liblte_s1ap_unpack_s1ap_pdu((LIBLTE_BYTE_MSG_STRUCT*)pdu, &rx_pdu) != LIBLTE_SUCCESS) {
    s1ap_log->error("Failed to unpack received PDU\n");
    return false;
  }

  switch(rx_pdu.choice_type) {
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE:
    return handle_initiatingmessage(&rx_pdu.choice.initiatingMessage);
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME:
    return handle_successfuloutcome(&rx_pdu.choice.successfulOutcome);
  case LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME:
    return handle_unsuccessfuloutcome(&rx_pdu.choice.unsuccessfulOutcome);
  default:
    s1ap_log->error("Unhandled PDU type %d\n", rx_pdu.choice_type);
    return false;
  }

  return true;
}

bool s1ap::handle_initiatingmessage(LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *msg)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_DOWNLINKNASTRANSPORT:
    return handle_dlnastransport(&msg->choice.DownlinkNASTransport);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALCONTEXTSETUPREQUEST:
    return handle_initialctxtsetuprequest(&msg->choice.InitialContextSetupRequest);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASECOMMAND:
    return handle_uectxtreleasecommand(&msg->choice.UEContextReleaseCommand);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PAGING:
    return handle_paging(&msg->choice.Paging);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_E_RABSETUPREQUEST:
    return handle_erabsetuprequest(&msg->choice.E_RABSetupRequest);
  case LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTMODIFICATIONREQUEST:
    return handle_uecontextmodifyrequest(&msg->choice.UEContextModificationRequest);
  default:
    s1ap_log->error("Unhandled intiating message: %s\n", liblte_s1ap_initiatingmessage_choice_text[msg->choice_type]);
  }
  return true;
}

bool s1ap::handle_successfuloutcome(LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *msg)
{
  switch(msg->choice_type) {
  case LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_S1SETUPRESPONSE:
    return handle_s1setupresponse(&msg->choice.S1SetupResponse);
  default:
    s1ap_log->error("Unhandled successful outcome message: %s\n", liblte_s1ap_successfuloutcome_choice_text[msg->choice_type]);
  }
  return true;
}

bool s1ap::handle_unsuccessfuloutcome(LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT *msg)
{
  switch (msg->choice_type) {
    case LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_S1SETUPFAILURE:
      return handle_s1setupfailure(&msg->choice.S1SetupFailure);
    case LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_HANDOVERPREPARATIONFAILURE:
      return handle_hopreparationfailure(&msg->choice.HandoverPreparationFailure);
    default:
      s1ap_log->error("Unhandled unsuccessful outcome message: %s\n",
                      liblte_s1ap_unsuccessfuloutcome_choice_text[msg->choice_type]);
  }
  return true;
}

bool s1ap::handle_s1setupresponse(LIBLTE_S1AP_MESSAGE_S1SETUPRESPONSE_STRUCT *msg)
{
  s1ap_log->info("Received S1SetupResponse\n");
  s1setupresponse = *msg;
  mme_connected = true;
  return true;
}

bool s1ap::handle_dlnastransport(LIBLTE_S1AP_MESSAGE_DOWNLINKNASTRANSPORT_STRUCT *msg)
{
  s1ap_log->info("Received DownlinkNASTransport\n");
  if(msg->ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  if(enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti      = enbid_to_rnti_map[msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID];
  ue_ctxt_t* ctxt      = get_user_ctxt(rnti);
  ctxt->MME_UE_S1AP_ID = msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID;

  if (msg->HandoverRestrictionList_present) {
    s1ap_log->warning("Not handling HandoverRestrictionList\n");
  }
  if(msg->SubscriberProfileIDforRFP_present) {
    s1ap_log->warning("Not handling SubscriberProfileIDforRFP\n");
  }

  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);
  if (pdu == nullptr) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::run_thread().\n");
    return false;
  }
  memcpy(pdu->msg, msg->NAS_PDU.buffer, msg->NAS_PDU.n_octets);
  pdu->N_bytes = msg->NAS_PDU.n_octets;
  rrc->write_dl_info(rnti, std::move(pdu));
  return true;
}

bool s1ap::handle_initialctxtsetuprequest(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT* msg)
{
  s1ap_log->info("Received InitialContextSetupRequest\n");
  if(msg->ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  if(enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning(
        "MME_UE_S1AP_ID has changed - old:%d, new:%d\n", ctxt->MME_UE_S1AP_ID, msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID);
    ctxt->MME_UE_S1AP_ID = msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  }

  // Setup UE ctxt in RRC
  if(!rrc->setup_ue_ctxt(rnti, msg)) {
    return false;
  }

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg->CSFallbackIndicator_present) {
    if (msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_REQUIRED ||
        msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_HIGH_PRIORITY) {
      // Send RRC Release (cs-fallback-triggered) to MME
      LIBLTE_S1AP_CAUSE_STRUCT cause;
      cause.ext                     = false;
      cause.choice_type             = LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK;
      cause.choice.radioNetwork.ext = false;
      cause.choice.radioNetwork.e   = LIBLTE_S1AP_CAUSERADIONETWORK_CS_FALLBACK_TRIGGERED;

      /* FIXME: This should normally probably only be sent after the SecurityMode procedure has completed! */
      ctxt->release_requested = true;
      send_uectxtreleaserequest(rnti, &cause);
    }
  }

  return true;
}

bool s1ap::handle_paging(LIBLTE_S1AP_MESSAGE_PAGING_STRUCT *msg)
{
  if(msg->ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  uint8_t *ptr = msg->UEIdentityIndexValue.buffer;
  uint32_t ueid = srslte_bit_pack(&ptr, 10); 

  rrc->add_paging_id(ueid, msg->UEPagingID);
  return true;
}

bool s1ap::handle_erabsetuprequest(LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT *msg)
{
  s1ap_log->info("Received ERABSetupRequest\n");
  if(msg->ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }

  if(enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning(
        "MME_UE_S1AP_ID has changed - old:%d, new:%d\n", ctxt->MME_UE_S1AP_ID, msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID);
    ctxt->MME_UE_S1AP_ID = msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  }

  // Setup UE ctxt in RRC
  return rrc->setup_ue_erabs(rnti, msg);
}

bool s1ap::handle_uecontextmodifyrequest(LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT* msg)
{
  s1ap_log->info("Received UeContextModificationRequest\n");
  if (enbid_to_rnti_map.end() == enbid_to_rnti_map.find(msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID)) {
    s1ap_log->warning("eNB_UE_S1AP_ID not found - discarding message\n");
    return false;
  }
  uint16_t   rnti = enbid_to_rnti_map[msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID];
  ue_ctxt_t* ctxt = get_user_ctxt(rnti);
  if (msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID != ctxt->MME_UE_S1AP_ID) {
    s1ap_log->warning(
        "MME_UE_S1AP_ID has changed - old:%d, new:%d\n", ctxt->MME_UE_S1AP_ID, msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID);
    ctxt->MME_UE_S1AP_ID = msg->MME_UE_S1AP_ID.MME_UE_S1AP_ID;
  }

  if (!rrc->modify_ue_ctxt(rnti, msg)) {
    LIBLTE_S1AP_CAUSE_STRUCT cause;
    cause.ext             = false;
    cause.choice_type     = LIBLTE_S1AP_CAUSE_CHOICE_MISC;
    cause.choice.misc.ext = false;
    cause.choice.misc.e   = LIBLTE_S1AP_CAUSEMISC_UNSPECIFIED;
    send_uectxmodifyfailure(rnti, &cause);
    return true;
  }

  // Send UEContextModificationResponse
  send_uectxmodifyresp(rnti);

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg->CSFallbackIndicator_present) {
    if (msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_REQUIRED ||
        msg->CSFallbackIndicator.e == LIBLTE_S1AP_CSFALLBACKINDICATOR_CS_FALLBACK_HIGH_PRIORITY) {
      // Send RRC Release (cs-fallback-triggered) to MME
      LIBLTE_S1AP_CAUSE_STRUCT cause;
      cause.ext                     = false;
      cause.choice_type             = LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK;
      cause.choice.radioNetwork.ext = false;
      cause.choice.radioNetwork.e   = LIBLTE_S1AP_CAUSERADIONETWORK_CS_FALLBACK_TRIGGERED;

      ctxt->release_requested = true;
      send_uectxtreleaserequest(rnti, &cause);
    }
  }

  return true;
}

bool s1ap::handle_uectxtreleasecommand(LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMMAND_STRUCT *msg)
{
  s1ap_log->info("Received UEContextReleaseCommand\n");
  if(msg->ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }
  if(msg->UE_S1AP_IDs.ext) {
    s1ap_log->warning("Not handling S1AP message extension\n");
  }

  uint16_t rnti = 0;
  if(msg->UE_S1AP_IDs.choice_type == LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_UE_S1AP_ID_PAIR) {

    if(msg->UE_S1AP_IDs.choice.uE_S1AP_ID_pair.ext) {
      s1ap_log->warning("Not handling S1AP message extension\n");
    }
    if(msg->UE_S1AP_IDs.choice.uE_S1AP_ID_pair.iE_Extensions_present) {
      s1ap_log->warning("Not handling S1AP message iE_Extensions\n");
    }
    uint32_t enb_ue_id = msg->UE_S1AP_IDs.choice.uE_S1AP_ID_pair.eNB_UE_S1AP_ID.ENB_UE_S1AP_ID;
    if(enbid_to_rnti_map.end() == enbid_to_rnti_map.find(enb_ue_id)) {
      s1ap_log->warning("eNB_UE_S1AP_ID:%d not found - discarding message\n", enb_ue_id);
      return false;
    }
    rnti = enbid_to_rnti_map[enb_ue_id];
    enbid_to_rnti_map.erase(enb_ue_id);

  } else { // LIBLTE_S1AP_UE_S1AP_IDS_CHOICE_MME_UE_S1AP_ID

    uint32_t mme_ue_id = msg->UE_S1AP_IDs.choice.mME_UE_S1AP_ID.MME_UE_S1AP_ID;
    uint32_t enb_ue_id;
    if(!find_mme_ue_id(mme_ue_id, &rnti, &enb_ue_id)) {
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

bool s1ap::handle_s1setupfailure(LIBLTE_S1AP_MESSAGE_S1SETUPFAILURE_STRUCT *msg) {
  std::string cause = get_cause(&msg->Cause);
  s1ap_log->error("S1 Setup Failure. Cause: %s\n", cause.c_str());
  s1ap_log->console("S1 Setup Failure. Cause: %s\n", cause.c_str());
  return true;
}

bool s1ap::handle_hopreparationfailure(LIBLTE_S1AP_MESSAGE_HANDOVERPREPARATIONFAILURE_STRUCT* msg)
{
  auto user_it = users.find(enbid_to_rnti_map[msg->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID]);
  if (user_it == users.end()) {
    s1ap_log->error("user rnti=0x%x no longer exists\n", user_it->first);
  }
  user_it->second->get_ho_prep_proc().trigger(*msg);
  return true;
}

/*******************************************************************************
/* S1AP message senders
********************************************************************************/

bool s1ap::send_initialuemessage(uint16_t                                 rnti,
                                 LIBLTE_S1AP_RRC_ESTABLISHMENT_CAUSE_ENUM cause,
                                 srslte::unique_byte_buffer_t             pdu,
                                 bool                                     has_tmsi,
                                 uint32_t                                 m_tmsi,
                                 uint8_t                                  mmec)
{
  if(!mme_connected) {
    return false;
  }
  srslte::byte_buffer_t msg;

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_INITIALUEMESSAGE;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_INITIALUEMESSAGE;

  LIBLTE_S1AP_MESSAGE_INITIALUEMESSAGE_STRUCT *initue = &init->choice.InitialUEMessage;
  initue->ext                                       = false;
  initue->CellAccessMode_present                    = false;
  initue->CSG_Id_present                            = false;
  initue->GUMMEIType_present                        = false;
  initue->GUMMEI_ID_present                         = false;
  initue->GW_TransportLayerAddress_present          = false;
  initue->LHN_ID_present                            = false;
  initue->RelayNode_Indicator_present               = false;
  initue->SIPTO_L_GW_TransportLayerAddress_present  = false;
  initue->S_TMSI_present                            = false;
  initue->Tunnel_Information_for_BBF_present        = false;

  // S_TMSI
  if(has_tmsi) {
    initue->S_TMSI_present = true;
    initue->S_TMSI.ext                    = false;
    initue->S_TMSI.iE_Extensions_present  = false;

    uint32_to_uint8(m_tmsi, initue->S_TMSI.m_TMSI.buffer);
    initue->S_TMSI.mMEC.buffer[0] = mmec;
  }

  // ENB_UE_S1AP_ID
  initue->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // NAS_PDU
  memcpy(initue->NAS_PDU.buffer, pdu->msg, pdu->N_bytes);
  initue->NAS_PDU.n_octets = pdu->N_bytes;

  // TAI
  memcpy(&initue->TAI, &tai, sizeof(LIBLTE_S1AP_TAI_STRUCT));

  // EUTRAN_CGI
  memcpy(&initue->EUTRAN_CGI, &eutran_cgi, sizeof(LIBLTE_S1AP_EUTRAN_CGI_STRUCT));

  // RRC Establishment Cause
  initue->RRC_Establishment_Cause.ext = false;
  initue->RRC_Establishment_Cause.e   = cause;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending InitialUEMessage for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                msg.msg,
                                msg.N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);
  if (n_sent == -1) {
    s1ap_log->error("Failed to send InitialUEMessage for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_ulnastransport(uint16_t rnti, srslte::unique_byte_buffer_t pdu)
{
  if(!mme_connected) {
    return false;
  }
  srslte::byte_buffer_t msg;

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_UPLINKNASTRANSPORT;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UPLINKNASTRANSPORT;

  LIBLTE_S1AP_MESSAGE_UPLINKNASTRANSPORT_STRUCT *ultx = &init->choice.UplinkNASTransport;
  ultx->ext                                       = false;
  ultx->GW_TransportLayerAddress_present          = false;
  ultx->LHN_ID_present                            = false;
  ultx->SIPTO_L_GW_TransportLayerAddress_present      = false;

  // MME_UE_S1AP_ID
  ultx->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  // ENB_UE_S1AP_ID
  ultx->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // NAS_PDU
  memcpy(ultx->NAS_PDU.buffer, pdu->msg, pdu->N_bytes);
  ultx->NAS_PDU.n_octets = pdu->N_bytes;

  // EUTRAN_CGI
  memcpy(&ultx->EUTRAN_CGI, &eutran_cgi, sizeof(LIBLTE_S1AP_EUTRAN_CGI_STRUCT));

  // TAI
  memcpy(&ultx->TAI, &tai, sizeof(LIBLTE_S1AP_TAI_STRUCT));

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending UplinkNASTransport for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                msg.msg,
                                msg.N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);
  if (n_sent == -1) {
    s1ap_log->error("Failed to send UplinkNASTransport for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_uectxtreleaserequest(uint16_t rnti, LIBLTE_S1AP_CAUSE_STRUCT *cause)
{
  if(!mme_connected) {
    return false;
  }
  srslte::byte_buffer_t msg;

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT *init = &tx_pdu.choice.initiatingMessage;
  init->procedureCode = LIBLTE_S1AP_PROC_ID_UECONTEXTRELEASEREQUEST;
  init->choice_type   = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_UECONTEXTRELEASEREQUEST;

  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASEREQUEST_STRUCT *req = &init->choice.UEContextReleaseRequest;
  req->ext                                = false;
  req->GWContextReleaseIndication_present                 = false;

  // MME_UE_S1AP_ID
  req->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  // ENB_UE_S1AP_ID
  req->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  // Cause
  memcpy(&req->Cause, cause, sizeof(LIBLTE_S1AP_CAUSE_STRUCT));

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending UEContextReleaseRequest for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                msg.msg,
                                msg.N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);
  if (n_sent == -1) {
    s1ap_log->error("Failed to send UEContextReleaseRequest for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_uectxtreleasecomplete(uint16_t rnti, uint32_t mme_ue_id, uint32_t enb_ue_id)
{
  if(!mme_connected) {
    return false;
  }
  srslte::byte_buffer_t msg;

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *succ = &tx_pdu.choice.successfulOutcome;
  succ->procedureCode = LIBLTE_S1AP_PROC_ID_UECONTEXTRELEASE;
  succ->choice_type   = LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UECONTEXTRELEASECOMPLETE;

  LIBLTE_S1AP_MESSAGE_UECONTEXTRELEASECOMPLETE_STRUCT *comp = &succ->choice.UEContextReleaseComplete;
  comp->ext                             = false;
  comp->CriticalityDiagnostics_present  = false;
  comp->UserLocationInformation_present = false;

  comp->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = enb_ue_id;
  comp->MME_UE_S1AP_ID.MME_UE_S1AP_ID = mme_ue_id;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)&msg);
  s1ap_log->info_hex(msg.msg, msg.N_bytes, "Sending UEContextReleaseComplete for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                msg.msg,
                                msg.N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);
  if (n_sent == -1) {
    s1ap_log->error("Failed to send UEContextReleaseComplete for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_initial_ctxt_setup_response(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *res_)
{
  if(!mme_connected) {
    return false;
  }
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool);
  if (!buf) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::send_initial_ctxt_setup_response().\n");
    return false;
  }

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *succ = &tx_pdu.choice.successfulOutcome;
  succ->procedureCode = LIBLTE_S1AP_PROC_ID_INITIALCONTEXTSETUP;
  succ->choice_type   = LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPRESPONSE;

  // Copy in the provided response message
  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT *res = &succ->choice.InitialContextSetupResponse;
  memcpy(res, res_, sizeof(LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPRESPONSE_STRUCT));

  // Fill in the GTP bind address for all bearers
  for(uint32_t i=0;i<res->E_RABSetupListCtxtSURes.len; i++) {
    uint8_t addr[4];
    inet_pton(AF_INET, args.gtp_bind_addr.c_str(), addr);
    liblte_unpack(addr, 4, res->E_RABSetupListCtxtSURes.buffer[i].transportLayerAddress.buffer);
    res->E_RABSetupListCtxtSURes.buffer[i].transportLayerAddress.n_bits = 32;
    res->E_RABSetupListCtxtSURes.buffer[i].transportLayerAddress.ext    = false;
  }

  // Fill in the MME and eNB IDs
  res->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  res->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending InitialContextSetupResponse for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);

  if (n_sent == -1) {
    s1ap_log->error("Failed to send InitialContextSetupResponse for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_erab_setup_response(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT *res_)
{
  if(!mme_connected) {
    return false;
  }
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool);
  if (!buf) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::send_erab_setup_response().\n");
    return false;
  }

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;

  tx_pdu.ext          = false;
  tx_pdu.choice_type  = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT *succ = &tx_pdu.choice.successfulOutcome;
  succ->procedureCode = LIBLTE_S1AP_PROC_ID_E_RABSETUP;
  succ->choice_type   = LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_E_RABSETUPRESPONSE;

  // Copy in the provided response message
  LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT *res = &succ->choice.E_RABSetupResponse;
  memcpy(res, res_, sizeof(LIBLTE_S1AP_MESSAGE_E_RABSETUPRESPONSE_STRUCT));

  // Fill in the GTP bind address for all bearers
  for(uint32_t i=0;i<res->E_RABSetupListBearerSURes.len; i++) {
    uint8_t addr[4];
    inet_pton(AF_INET, args.gtp_bind_addr.c_str(), addr);
    liblte_unpack(addr, 4, res->E_RABSetupListBearerSURes.buffer[i].transportLayerAddress.buffer);
    res->E_RABSetupListBearerSURes.buffer[i].transportLayerAddress.n_bits = 32;
    res->E_RABSetupListBearerSURes.buffer[i].transportLayerAddress.ext    = false;
  }

  // Fill in the MME and eNB IDs
  res->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  res->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending E_RABSetupResponse for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);

  if (n_sent == -1) {
    s1ap_log->error("Failed to send E_RABSetupResponse for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_initial_ctxt_setup_failure(uint16_t rnti)
{
  if(!mme_connected) {
    return false;
  }
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool);
  if (!buf) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::send_initial_ctxt_setup_failure().\n");
    return false;
  }

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext         = false;
  tx_pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME;

  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT *unsucc = &tx_pdu.choice.unsuccessfulOutcome;
  unsucc->procedureCode = LIBLTE_S1AP_PROC_ID_INITIALCONTEXTSETUP;
  unsucc->choice_type   = LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_INITIALCONTEXTSETUPFAILURE;

  LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPFAILURE_STRUCT *fail = &unsucc->choice.InitialContextSetupFailure;
  fail->ext                                                   = false;
  fail->CriticalityDiagnostics_present                        = false;

  fail->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  fail->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  fail->Cause.ext                     = false;
  fail->Cause.choice_type             = LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK;
  fail->Cause.choice.radioNetwork.ext = false;
  fail->Cause.choice.radioNetwork.e   = LIBLTE_S1AP_CAUSERADIONETWORK_UNSPECIFIED;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending InitialContextSetupFailure for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);

  if (n_sent == -1) {
    s1ap_log->error("Failed to send UplinkNASTransport for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_uectxmodifyresp(uint16_t rnti)
{
  if (!mme_connected) {
    return false;
  }
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool);
  if (!buf) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::send_uectxmodifyresp().\n");
    return false;
  }

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext         = false;
  tx_pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_SUCCESSFULOUTCOME;

  LIBLTE_S1AP_SUCCESSFULOUTCOME_STRUCT* succ = &tx_pdu.choice.successfulOutcome;
  succ->procedureCode                        = LIBLTE_S1AP_PROC_ID_UECONTEXTMODIFICATION;
  succ->choice_type                          = LIBLTE_S1AP_SUCCESSFULOUTCOME_CHOICE_UECONTEXTMODIFICATIONRESPONSE;

  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONRESPONSE_STRUCT* resp = &succ->choice.UEContextModificationResponse;
  resp->ext                                                      = false;
  resp->CriticalityDiagnostics_present                           = false;

  resp->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  resp->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending ContextModificationFailure for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);

  if (n_sent == -1) {
    s1ap_log->error("Failed to send ContextModificationFailure for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
}

bool s1ap::send_uectxmodifyfailure(uint16_t rnti, LIBLTE_S1AP_CAUSE_STRUCT* cause)
{
  if (!mme_connected) {
    return false;
  }
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool);
  if (!buf) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer in s1ap::send_initial_ctxt_setup_failure().\n");
    return false;
  }

  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  tx_pdu.ext         = false;
  tx_pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_UNSUCCESSFULOUTCOME;

  LIBLTE_S1AP_UNSUCCESSFULOUTCOME_STRUCT* unsucc = &tx_pdu.choice.unsuccessfulOutcome;
  unsucc->procedureCode                          = LIBLTE_S1AP_PROC_ID_UECONTEXTMODIFICATION;
  unsucc->choice_type                            = LIBLTE_S1AP_UNSUCCESSFULOUTCOME_CHOICE_UECONTEXTMODIFICATIONFAILURE;

  LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONFAILURE_STRUCT* fail = &unsucc->choice.UEContextModificationFailure;
  fail->ext                                                     = false;
  fail->CriticalityDiagnostics_present                          = false;

  fail->MME_UE_S1AP_ID.MME_UE_S1AP_ID = get_user_ctxt(rnti)->MME_UE_S1AP_ID;
  fail->eNB_UE_S1AP_ID.ENB_UE_S1AP_ID = get_user_ctxt(rnti)->eNB_UE_S1AP_ID;

  memcpy(&fail->Cause, cause, sizeof(LIBLTE_S1AP_CAUSE_STRUCT));

  liblte_s1ap_pack_s1ap_pdu(&tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending UEContextModificationFailure for RNTI:0x%x", rnti);

  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);

  if (n_sent == -1) {
    s1ap_log->error("Failed to send UEContextModificationFailure for RNTI:0x%x\n", rnti);
    return false;
  }

  return true;
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

bool s1ap::sctp_send_s1ap_pdu(LIBLTE_S1AP_S1AP_PDU_STRUCT* tx_pdu, uint32_t rnti, const char* procedure_name)
{
  srslte::unique_byte_buffer_t buf = srslte::allocate_unique_buffer(*pool, false);
  if (buf == nullptr) {
    s1ap_log->error("Fatal Error: Couldn't allocate buffer for %s.\n", procedure_name);
    return false;
  }

  liblte_s1ap_pack_s1ap_pdu(tx_pdu, (LIBLTE_BYTE_MSG_STRUCT*)buf.get());
  s1ap_log->info_hex(buf->msg, buf->N_bytes, "Sending %s for rnti=0x%x", procedure_name, rnti);
  ssize_t n_sent = sctp_sendmsg(socket_fd,
                                buf->msg,
                                buf->N_bytes,
                                (struct sockaddr*)&mme_addr,
                                sizeof(struct sockaddr_in),
                                htonl(PPID),
                                0,
                                get_user_ctxt(rnti)->stream_id,
                                0,
                                0);
  if (n_sent == -1) {
    s1ap_log->error("Failed to send %s for rnti=0x%x\n", procedure_name, rnti);
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

std::string s1ap::get_cause(const LIBLTE_S1AP_CAUSE_STRUCT* c)
{
  std::string cause = liblte_s1ap_cause_choice_text[c->choice_type];
  cause += " - ";
  switch(c->choice_type) {
  case LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK:
    cause += liblte_s1ap_causeradionetwork_text[c->choice.radioNetwork.e];
    break;
  case LIBLTE_S1AP_CAUSE_CHOICE_TRANSPORT:
    cause += liblte_s1ap_causetransport_text[c->choice.transport.e];
    break;
  case LIBLTE_S1AP_CAUSE_CHOICE_NAS:
    cause += liblte_s1ap_causenas_text[c->choice.nas.e];
    break;
  case LIBLTE_S1AP_CAUSE_CHOICE_PROTOCOL:
    cause += liblte_s1ap_causeprotocol_text[c->choice.protocol.e];
    break;
  case LIBLTE_S1AP_CAUSE_CHOICE_MISC:
    cause += liblte_s1ap_causemisc_text[c->choice.misc.e];
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
  ts1_reloc_prep.set(10000, [this](uint32_t tid) { ho_prep_proc.trigger(ho_prep_proc_t::ts1_reloc_prep_expired{}); });
}

bool s1ap::ue::send_ho_required(uint32_t                     target_eci,
                                srslte::plmn_id_t            target_plmn,
                                srslte::unique_byte_buffer_t rrc_container)
{
  /*** Setup S1AP PDU as HandoverRequired ***/
  LIBLTE_S1AP_S1AP_PDU_STRUCT tx_pdu;
  bzero(&tx_pdu, sizeof(tx_pdu));
  tx_pdu.choice_type                                 = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;
  tx_pdu.choice.initiatingMessage.choice_type        = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_HANDOVERREQUIRED;
  tx_pdu.choice.initiatingMessage.criticality        = LIBLTE_S1AP_CRITICALITY_IGNORE;
  tx_pdu.choice.initiatingMessage.procedureCode      = LIBLTE_S1AP_PROC_ID_HANDOVERPREPARATION;
  LIBLTE_S1AP_MESSAGE_HANDOVERREQUIRED_STRUCT& horeq = tx_pdu.choice.initiatingMessage.choice.HandoverRequired;

  /*** fill HO Required message ***/
  horeq.eNB_UE_S1AP_ID.ENB_UE_S1AP_ID               = ctxt.eNB_UE_S1AP_ID;
  horeq.MME_UE_S1AP_ID.MME_UE_S1AP_ID               = ctxt.MME_UE_S1AP_ID;
  horeq.Direct_Forwarding_Path_Availability_present = false;             // NOTE: X2 for fwd path not supported
  horeq.HandoverType.e              = LIBLTE_S1AP_HANDOVERTYPE_INTRALTE; // NOTE: only intra-LTE HO supported
  horeq.Cause.choice_type           = LIBLTE_S1AP_CAUSE_CHOICE_RADIONETWORK;
  horeq.Cause.choice.radioNetwork.e = LIBLTE_S1AP_CAUSERADIONETWORK_UNSPECIFIED;
  // LIBLTE_S1AP_CAUSERADIONETWORK_S1_INTRA_SYSTEM_HANDOVER_TRIGGERED;

  /*** set the target eNB ***/
  horeq.TargetID.choice_type                 = LIBLTE_S1AP_TARGETID_CHOICE_TARGETENB_ID;
  LIBLTE_S1AP_TARGETENB_ID_STRUCT* targetenb = &horeq.TargetID.choice.targeteNB_ID;
  horeq.CSG_Id_present                       = false; // NOTE: CSG/hybrid target cell not supported
  horeq.CellAccessMode_present               = false; // only for hybrid cells
  // no GERAN/UTRAN/PS
  horeq.SRVCCHOIndication_present      = false;
  horeq.MSClassmark2_present           = false;
  horeq.MSClassmark3_present           = false;
  horeq.PS_ServiceNotAvailable_present = false;
  // set PLMN of target and TAI
  if (horeq.TargetID.choice_type != LIBLTE_S1AP_TARGETID_CHOICE_TARGETENB_ID) {
    s1ap_log->error("Non-intraLTE HO not supported.\n");
    return false;
  }
  // NOTE: Only HO without TAU supported.
  uint16_t tmp16;
  tmp16 = htons(s1ap_ptr->args.tac);
  memcpy(targetenb->selected_TAI.tAC.buffer, &tmp16, sizeof(uint16_t));
  target_plmn.to_s1ap_plmn_bytes(targetenb->selected_TAI.pLMNidentity.buffer);
  // NOTE: Only HO to different Macro eNB is supported.
  targetenb->global_ENB_ID.eNB_ID.choice_type = LIBLTE_S1AP_ENB_ID_CHOICE_MACROENB_ID;
  target_plmn.to_s1ap_plmn_bytes(targetenb->global_ENB_ID.pLMNidentity.buffer);
  uint32_t tmp32 = htonl(target_eci >> 8u);
  uint8_t  enb_id_bits[sizeof(uint32_t) * 8];
  liblte_unpack((uint8_t*)&tmp32, sizeof(uint32_t), enb_id_bits);
  memcpy(targetenb->global_ENB_ID.eNB_ID.choice.macroENB_ID.buffer,
         &enb_id_bits[32 - LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN],
         LIBLTE_S1AP_MACROENB_ID_BIT_STRING_LEN);

  /*** fill the transparent container ***/
  horeq.Source_ToTarget_TransparentContainer_Secondary_present = false;
  LIBLTE_S1AP_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_STRUCT transparent_cntr;
  bzero(&transparent_cntr, sizeof(LIBLTE_S1AP_SOURCEENB_TOTARGETENB_TRANSPARENTCONTAINER_STRUCT));
  transparent_cntr.e_RABInformationList_present      = false; // TODO: CHECK
  transparent_cntr.subscriberProfileIDforRFP_present = false; // TODO: CHECK
  // - set target cell ID
  target_plmn.to_s1ap_plmn_bytes(transparent_cntr.targetCell_ID.pLMNidentity.buffer);
  tmp32 = htonl(target_eci);
  uint8_t eci_bits[32];
  liblte_unpack((uint8_t*)&tmp32, sizeof(uint32_t), eci_bits);
  memcpy(transparent_cntr.targetCell_ID.cell_ID.buffer,
         &eci_bits[32 - LIBLTE_S1AP_CELLIDENTITY_BIT_STRING_LEN],
         LIBLTE_S1AP_CELLIDENTITY_BIT_STRING_LEN); // [ENBID|CELLID|0]
  // info specific to source cell and history of UE
  // - set as last visited cell the source eNB PLMN & Cell ID
  transparent_cntr.uE_HistoryInformation.len                   = 1;
  transparent_cntr.uE_HistoryInformation.buffer[0].choice_type = LIBLTE_S1AP_LASTVISITEDCELL_ITEM_CHOICE_E_UTRAN_CELL;
  LIBLTE_S1AP_LASTVISITEDEUTRANCELLINFORMATION_STRUCT* lastvisited =
      &transparent_cntr.uE_HistoryInformation.buffer[0].choice.e_UTRAN_Cell;
  lastvisited->cellType.cell_Size.e = LIBLTE_S1AP_CELL_SIZE_MEDIUM;
  target_plmn.to_s1ap_plmn_bytes(lastvisited->global_Cell_ID.pLMNidentity.buffer);
  memcpy(lastvisited->global_Cell_ID.cell_ID.buffer,
         s1ap_ptr->eutran_cgi.cell_ID.buffer,
         LIBLTE_S1AP_CELLIDENTITY_BIT_STRING_LEN);
  // - set time spent in current source cell
  struct timeval ts[3];
  memcpy(&ts[1], &ctxt.init_timestamp, sizeof(struct timeval));
  gettimeofday(&ts[2], nullptr);
  get_time_interval(ts);
  lastvisited->time_UE_StayedInCell.Time_UE_StayedInCell = (uint16_t)(ts[0].tv_usec / 1.0e6 + ts[0].tv_sec);
  lastvisited->time_UE_StayedInCell.Time_UE_StayedInCell =
      std::min(lastvisited->time_UE_StayedInCell.Time_UE_StayedInCell, (uint16_t)4095);
  // - fill RRC container
  memcpy(transparent_cntr.rRC_Container.buffer, rrc_container->msg, rrc_container->N_bytes);
  transparent_cntr.rRC_Container.n_octets = rrc_container->N_bytes;

  /*** pack Transparent Container into HORequired message ***/
  LIBLTE_BYTE_MSG_STRUCT bytemsg;
  bytemsg.N_bytes = 0;
  LIBLTE_BIT_MSG_STRUCT bitmsg;
  uint8_t*              msg_ptr = bitmsg.msg;
  liblte_s1ap_pack_sourceenb_totargetenb_transparentcontainer(&transparent_cntr, &msg_ptr);
  bitmsg.N_bits = msg_ptr - bitmsg.msg;
  liblte_pack(&bitmsg, &bytemsg);
  memcpy(horeq.Source_ToTarget_TransparentContainer.buffer, bytemsg.msg, bytemsg.N_bytes);
  horeq.Source_ToTarget_TransparentContainer.n_octets = bytemsg.N_bytes;

  return s1ap_ptr->sctp_send_s1ap_pdu(&tx_pdu, ctxt.rnti, "HORequired");
}

} // namespace srsenb
