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

#include "srsepc/hdr/mme/s1ap.h"
#include "srsran/asn1/gtpc.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/liblte_security.h"
#include "srsran/common/network_utils.h"
#include <cmath>
#include <inttypes.h> // for printing uint64_t
#include <random>

namespace srsepc {

s1ap*           s1ap::m_instance    = NULL;
pthread_mutex_t s1ap_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap::s1ap() : m_s1mme(-1), m_next_mme_ue_s1ap_id(1), m_mme_gtpc(NULL) {}

s1ap::~s1ap()
{
  return;
}

s1ap* s1ap::get_instance(void)
{
  pthread_mutex_lock(&s1ap_instance_mutex);
  if (m_instance == NULL) {
    m_instance = new s1ap();
  }
  pthread_mutex_unlock(&s1ap_instance_mutex);
  return (m_instance);
}

void s1ap::cleanup(void)
{
  pthread_mutex_lock(&s1ap_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_instance_mutex);
}

int s1ap::init(const s1ap_args_t& s1ap_args)
{
  m_s1ap_args = s1ap_args;
  srsran::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &m_plmn);

  std::random_device                      rd;
  std::mt19937                            generator(rd());
  std::uniform_int_distribution<uint32_t> distr(0, std::numeric_limits<uint32_t>::max());
  m_next_m_tmsi = distr(generator);

  // Get pointer to the HSS
  m_hss = hss::get_instance();

  // Init message handlers
  m_s1ap_mngmt_proc = s1ap_mngmt_proc::get_instance(); // Managment procedures
  m_s1ap_mngmt_proc->init();
  m_s1ap_nas_transport = s1ap_nas_transport::get_instance(); // NAS Transport procedures
  m_s1ap_nas_transport->init();
  m_s1ap_ctx_mngmt_proc = s1ap_ctx_mngmt_proc::get_instance(); // Context Management Procedures
  m_s1ap_ctx_mngmt_proc->init();
  m_s1ap_erab_mngmt_proc = s1ap_erab_mngmt_proc::get_instance(); // E-RAB Management Procedures
  m_s1ap_erab_mngmt_proc->init();
  m_s1ap_paging = s1ap_paging::get_instance(); // Paging
  m_s1ap_paging->init();

  // Get pointer to GTP-C class
  m_mme_gtpc = mme_gtpc::get_instance();

  // Initialize S1-MME
  m_s1mme = enb_listen();
  if (m_s1mme == SRSRAN_ERROR) {
    return SRSRAN_ERROR;
  }

  // Init PCAP
  m_pcap_enable = s1ap_args.pcap_enable;
  if (m_pcap_enable) {
    m_pcap.open(s1ap_args.pcap_filename.c_str());
  }
  m_logger.info("S1AP Initialized");
  return SRSRAN_SUCCESS;
}

void s1ap::stop()
{
  if (m_s1mme != -1) {
    close(m_s1mme);
  }
  std::map<uint16_t, enb_ctx_t*>::iterator enb_it = m_active_enbs.begin();
  while (enb_it != m_active_enbs.end()) {
    m_logger.info("Deleting eNB context. eNB Id: 0x%x", enb_it->second->enb_id);
    srsran::console("Deleting eNB context. eNB Id: 0x%x\n", enb_it->second->enb_id);
    delete enb_it->second;
    m_active_enbs.erase(enb_it++);
  }

  std::map<uint64_t, nas*>::iterator ue_it = m_imsi_to_nas_ctx.begin();
  while (ue_it != m_imsi_to_nas_ctx.end()) {
    m_logger.info("Deleting UE EMM context. IMSI: %015" PRIu64 "", ue_it->first);
    srsran::console("Deleting UE EMM context. IMSI: %015" PRIu64 "\n", ue_it->first);
    delete ue_it->second;
    m_imsi_to_nas_ctx.erase(ue_it++);
  }

  // Cleanup message handlers
  s1ap_mngmt_proc::cleanup();
  s1ap_nas_transport::cleanup();
  s1ap_ctx_mngmt_proc::cleanup();

  // PCAP
  if (m_pcap_enable) {
    m_pcap.close();
  }
  return;
}

int s1ap::get_s1_mme()
{
  return m_s1mme;
}

uint32_t s1ap::get_next_mme_ue_s1ap_id()
{
  return m_next_mme_ue_s1ap_id++;
}

int s1ap::enb_listen()
{
  /*This function sets up the SCTP socket for eNBs to connect to*/
  int                         sock_fd, err;
  struct sockaddr_in          s1mme_addr;
  struct sctp_event_subscribe evnts;

  m_logger.info("S1-MME Initializing");
  sock_fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  if (sock_fd == -1) {
    srsran::console("Could not create SCTP socket\n");
    return SRSRAN_ERROR;
  }

  // Sets the data_io_event to be able to use sendrecv_info
  // Subscribes to the SCTP_SHUTDOWN event, to handle graceful shutdown
  bzero(&evnts, sizeof(evnts));
  evnts.sctp_data_io_event  = 1;
  evnts.sctp_shutdown_event = 1;
  if (setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts))) {
    close(sock_fd);
    srsran::console("Subscribing to sctp_data_io_events failed\n");
    return SRSRAN_ERROR;
  }

  // S1-MME bind
  bzero(&s1mme_addr, sizeof(s1mme_addr));
  if (not srsran::net_utils::set_sockaddr(&s1mme_addr, m_s1ap_args.mme_bind_addr.c_str(), S1MME_PORT)) {
    close(sock_fd);
    m_logger.error("Invalid mme_bind_addr: %s", m_s1ap_args.mme_bind_addr.c_str());
    srsran::console("Invalid mme_bind_addr: %s\n", m_s1ap_args.mme_bind_addr.c_str());
    return SRSRAN_ERROR;
  }

  if (not srsran::net_utils::bind_addr(sock_fd, s1mme_addr)) {
    close(sock_fd);
    m_logger.error("Error binding SCTP socket");
    srsran::console("Error binding SCTP socket\n");
    return SRSRAN_ERROR;
  }

  // Listen for connections
  err = listen(sock_fd, SOMAXCONN);
  if (err != 0) {
    close(sock_fd);
    m_logger.error("Error in SCTP socket listen");
    srsran::console("Error in SCTP socket listen\n");
    return SRSRAN_ERROR;
  }

  return sock_fd;
}

bool s1ap::s1ap_tx_pdu(const asn1::s1ap::s1ap_pdu_c& pdu, struct sctp_sndrcvinfo* enb_sri)
{
  m_logger.debug("Transmitting S1AP PDU. eNB SCTP association Id: %d", enb_sri->sinfo_assoc_id);

  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  if (buf == nullptr) {
    m_logger.error("Fatal Error: Couldn't allocate buffer for S1AP PDU.");
    return false;
  }
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
    m_logger.error("Could not pack S1AP PDU correctly.");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();

  ssize_t n_sent = sctp_send(m_s1mme, buf->msg, buf->N_bytes, enb_sri, MSG_NOSIGNAL);
  if (n_sent == -1) {
    srsran::console("Failed to send S1AP PDU. Error: %s\n", strerror(errno));
    m_logger.error("Failed to send S1AP PDU. Error: %s ", strerror(errno));
    return false;
  }

  if (m_pcap_enable) {
    m_pcap.write_s1ap(buf->msg, buf->N_bytes);
  }

  return true;
}

void s1ap::handle_s1ap_rx_pdu(srsran::byte_buffer_t* pdu, struct sctp_sndrcvinfo* enb_sri)
{
  // Save PCAP
  if (m_pcap_enable) {
    m_pcap.write_s1ap(pdu->msg, pdu->N_bytes);
  }

  // Get PDU type
  s1ap_pdu_t     rx_pdu;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);
  if (rx_pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    m_logger.error("Failed to unpack received PDU");
    return;
  }

  switch (rx_pdu.type().value) {
    case s1ap_pdu_t::types_opts::init_msg:
      m_logger.info("Received Initiating PDU");
      handle_initiating_message(rx_pdu.init_msg(), enb_sri);
      break;
    case s1ap_pdu_t::types_opts::successful_outcome:
      m_logger.info("Received Succeseful Outcome PDU");
      handle_successful_outcome(rx_pdu.successful_outcome());
      break;
    case s1ap_pdu_t::types_opts::unsuccessful_outcome:
      m_logger.info("Received Unsucceseful Outcome PDU");
      // TODO handle_unsuccessfuloutcome(&rx_pdu.choice.unsuccessfulOutcome);
      break;
    default:
      m_logger.warning("Unhandled PDU type %d", rx_pdu.type().value);
  }
}

void s1ap::handle_initiating_message(const asn1::s1ap::init_msg_s& msg, struct sctp_sndrcvinfo* enb_sri)
{
  using init_msg_type_opts_t = asn1::s1ap::s1ap_elem_procs_o::init_msg_c::types_opts;

  switch (msg.value.type().value) {
    case init_msg_type_opts_t::s1_setup_request:
      m_logger.info("Received S1 Setup Request.");
      m_s1ap_mngmt_proc->handle_s1_setup_request(msg.value.s1_setup_request(), enb_sri);
      break;
    case init_msg_type_opts_t::init_ue_msg:
      m_logger.info("Received Initial UE Message.");
      m_s1ap_nas_transport->handle_initial_ue_message(msg.value.init_ue_msg(), enb_sri);
      break;
    case init_msg_type_opts_t::ul_nas_transport:
      m_logger.info("Received Uplink NAS Transport Message.");
      m_s1ap_nas_transport->handle_uplink_nas_transport(msg.value.ul_nas_transport(), enb_sri);
      break;
    case init_msg_type_opts_t::ue_context_release_request:
      m_logger.info("Received UE Context Release Request Message.");
      m_s1ap_ctx_mngmt_proc->handle_ue_context_release_request(msg.value.ue_context_release_request(), enb_sri);
      break;
    case init_msg_type_opts_t::ue_cap_info_ind:
      m_logger.info("Ignoring UE capability Info Indication.");
      break;
    default:
      m_logger.error("Unhandled S1AP initiating message: %s", msg.value.type().to_string());
      srsran::console("Unhandled S1APinitiating message: %s\n", msg.value.type().to_string());
  }
}

void s1ap::handle_successful_outcome(const asn1::s1ap::successful_outcome_s& msg)
{
  using successful_outcome_type_opts_t = asn1::s1ap::s1ap_elem_procs_o::successful_outcome_c::types_opts;

  switch (msg.value.type().value) {
    case successful_outcome_type_opts_t::init_context_setup_resp:
      m_logger.info("Received Initial Context Setup Response.");
      m_s1ap_ctx_mngmt_proc->handle_initial_context_setup_response(msg.value.init_context_setup_resp());
      break;
    case successful_outcome_type_opts_t::ue_context_release_complete:
      m_logger.info("Received UE Context Release Complete");
      m_s1ap_ctx_mngmt_proc->handle_ue_context_release_complete(msg.value.ue_context_release_complete());
      break;
    default:
      m_logger.error("Unhandled successful outcome message: %s", msg.value.type().to_string());
  }
}

// eNB Context Managment
void s1ap::add_new_enb_ctx(const enb_ctx_t& enb_ctx, const struct sctp_sndrcvinfo* enb_sri)
{
  m_logger.info("Adding new eNB context. eNB ID %d", enb_ctx.enb_id);
  std::set<uint32_t> ue_set;
  enb_ctx_t*         enb_ptr = new enb_ctx_t;
  *enb_ptr                   = enb_ctx;
  m_active_enbs.insert(std::pair<uint16_t, enb_ctx_t*>(enb_ptr->enb_id, enb_ptr));
  m_sctp_to_enb_id.insert(std::pair<int32_t, uint16_t>(enb_sri->sinfo_assoc_id, enb_ptr->enb_id));
  m_enb_assoc_to_ue_ids.insert(std::pair<int32_t, std::set<uint32_t> >(enb_sri->sinfo_assoc_id, ue_set));
}

enb_ctx_t* s1ap::find_enb_ctx(uint16_t enb_id)
{
  std::map<uint16_t, enb_ctx_t*>::iterator it = m_active_enbs.find(enb_id);
  if (it == m_active_enbs.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}

void s1ap::delete_enb_ctx(int32_t assoc_id)
{
  std::map<int32_t, uint16_t>::iterator it_assoc = m_sctp_to_enb_id.find(assoc_id);
  uint16_t                              enb_id   = it_assoc->second;

  std::map<uint16_t, enb_ctx_t*>::iterator it_ctx = m_active_enbs.find(enb_id);
  if (it_ctx == m_active_enbs.end() || it_assoc == m_sctp_to_enb_id.end()) {
    m_logger.error("Could not find eNB to delete. Association: %d", assoc_id);
    return;
  }

  m_logger.info("Deleting eNB context. eNB Id: 0x%x", enb_id);
  srsran::console("Deleting eNB context. eNB Id: 0x%x\n", enb_id);

  // Delete connected UEs ctx
  release_ues_ecm_ctx_in_enb(assoc_id);

  // Delete eNB
  delete it_ctx->second;
  m_active_enbs.erase(it_ctx);
  m_sctp_to_enb_id.erase(it_assoc);
  return;
}

// UE Context Management
bool s1ap::add_nas_ctx_to_imsi_map(nas* nas_ctx)
{
  std::map<uint64_t, nas*>::iterator ctx_it = m_imsi_to_nas_ctx.find(nas_ctx->m_emm_ctx.imsi);
  if (ctx_it != m_imsi_to_nas_ctx.end()) {
    m_logger.error("UE Context already exists. IMSI %015" PRIu64 "", nas_ctx->m_emm_ctx.imsi);
    return false;
  }
  if (nas_ctx->m_ecm_ctx.mme_ue_s1ap_id != 0) {
    std::map<uint32_t, nas*>::iterator ctx_it2 = m_mme_ue_s1ap_id_to_nas_ctx.find(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
    if (ctx_it2 != m_mme_ue_s1ap_id_to_nas_ctx.end() && ctx_it2->second != nas_ctx) {
      m_logger.error("Context identified with IMSI does not match context identified by MME UE S1AP Id.");
      return false;
    }
  }
  m_imsi_to_nas_ctx.insert(std::pair<uint64_t, nas*>(nas_ctx->m_emm_ctx.imsi, nas_ctx));
  m_logger.debug("Saved UE context corresponding to IMSI %015" PRIu64 "", nas_ctx->m_emm_ctx.imsi);
  return true;
}

bool s1ap::add_nas_ctx_to_mme_ue_s1ap_id_map(nas* nas_ctx)
{
  if (nas_ctx->m_ecm_ctx.mme_ue_s1ap_id == 0) {
    m_logger.error("Could not add UE context to MME UE S1AP map. MME UE S1AP ID 0 is not valid.");
    return false;
  }
  std::map<uint32_t, nas*>::iterator ctx_it = m_mme_ue_s1ap_id_to_nas_ctx.find(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
  if (ctx_it != m_mme_ue_s1ap_id_to_nas_ctx.end()) {
    m_logger.error("UE Context already exists. MME UE S1AP Id %015" PRIu64 "", nas_ctx->m_emm_ctx.imsi);
    return false;
  }
  if (nas_ctx->m_emm_ctx.imsi != 0) {
    std::map<uint32_t, nas*>::iterator ctx_it2 = m_mme_ue_s1ap_id_to_nas_ctx.find(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
    if (ctx_it2 != m_mme_ue_s1ap_id_to_nas_ctx.end() && ctx_it2->second != nas_ctx) {
      m_logger.error("Context identified with MME UE S1AP Id does not match context identified by IMSI.");
      return false;
    }
  }
  m_mme_ue_s1ap_id_to_nas_ctx.insert(std::pair<uint32_t, nas*>(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id, nas_ctx));
  m_logger.debug("Saved UE context corresponding to MME UE S1AP Id %d", nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
  return true;
}

bool s1ap::add_ue_to_enb_set(int32_t enb_assoc, uint32_t mme_ue_s1ap_id)
{
  std::map<int32_t, std::set<uint32_t> >::iterator ues_in_enb = m_enb_assoc_to_ue_ids.find(enb_assoc);
  if (ues_in_enb == m_enb_assoc_to_ue_ids.end()) {
    m_logger.error("Could not find eNB from eNB SCTP association %d", enb_assoc);
    return false;
  }
  std::set<uint32_t>::iterator ue_id = ues_in_enb->second.find(mme_ue_s1ap_id);
  if (ue_id != ues_in_enb->second.end()) {
    m_logger.error("UE with MME UE S1AP Id already exists %d", mme_ue_s1ap_id);
    return false;
  }
  ues_in_enb->second.insert(mme_ue_s1ap_id);
  m_logger.debug("Added UE with MME-UE S1AP Id %d to eNB with association %d", mme_ue_s1ap_id, enb_assoc);
  return true;
}

nas* s1ap::find_nas_ctx_from_mme_ue_s1ap_id(uint32_t mme_ue_s1ap_id)
{
  std::map<uint32_t, nas*>::iterator it = m_mme_ue_s1ap_id_to_nas_ctx.find(mme_ue_s1ap_id);
  if (it == m_mme_ue_s1ap_id_to_nas_ctx.end()) {
    return NULL;
  } else {
    return it->second;
  }
}

nas* s1ap::find_nas_ctx_from_imsi(uint64_t imsi)
{
  std::map<uint64_t, nas*>::iterator it = m_imsi_to_nas_ctx.find(imsi);
  if (it == m_imsi_to_nas_ctx.end()) {
    return NULL;
  } else {
    return it->second;
  }
}

void s1ap::release_ues_ecm_ctx_in_enb(int32_t enb_assoc)
{
  srsran::console("Releasing UEs context\n");
  std::map<int32_t, std::set<uint32_t> >::iterator ues_in_enb = m_enb_assoc_to_ue_ids.find(enb_assoc);
  std::set<uint32_t>::iterator                     ue_id      = ues_in_enb->second.begin();
  if (ue_id == ues_in_enb->second.end()) {
    srsran::console("No UEs to be released\n");
  } else {
    while (ue_id != ues_in_enb->second.end()) {
      std::map<uint32_t, nas*>::iterator nas_ctx = m_mme_ue_s1ap_id_to_nas_ctx.find(*ue_id);
      emm_ctx_t*                         emm_ctx = &nas_ctx->second->m_emm_ctx;
      ecm_ctx_t*                         ecm_ctx = &nas_ctx->second->m_ecm_ctx;

      m_logger.info(
          "Releasing UE context. IMSI: %015" PRIu64 ", UE-MME S1AP Id: %d", emm_ctx->imsi, ecm_ctx->mme_ue_s1ap_id);
      if (emm_ctx->state == EMM_STATE_REGISTERED) {
        m_mme_gtpc->send_delete_session_request(emm_ctx->imsi);
        emm_ctx->state = EMM_STATE_DEREGISTERED;
      }
      srsran::console("Releasing UE ECM context. UE-MME S1AP Id: %d\n", ecm_ctx->mme_ue_s1ap_id);
      ecm_ctx->state          = ECM_STATE_IDLE;
      ecm_ctx->mme_ue_s1ap_id = 0;
      ecm_ctx->enb_ue_s1ap_id = 0;
      ues_in_enb->second.erase(ue_id++);
    }
  }
}

bool s1ap::release_ue_ecm_ctx(uint32_t mme_ue_s1ap_id)
{
  nas* nas_ctx = find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == NULL) {
    m_logger.error("Cannot release UE ECM context, UE not found. MME-UE S1AP Id: %d", mme_ue_s1ap_id);
    return false;
  }
  ecm_ctx_t* ecm_ctx = &nas_ctx->m_ecm_ctx;

  // Delete UE within eNB UE set
  std::map<int32_t, uint16_t>::iterator it = m_sctp_to_enb_id.find(ecm_ctx->enb_sri.sinfo_assoc_id);
  if (it == m_sctp_to_enb_id.end()) {
    m_logger.error("Could not find eNB for UE release request.");
    return false;
  }
  uint16_t                                         enb_id = it->second;
  std::map<int32_t, std::set<uint32_t> >::iterator ue_set = m_enb_assoc_to_ue_ids.find(ecm_ctx->enb_sri.sinfo_assoc_id);
  if (ue_set == m_enb_assoc_to_ue_ids.end()) {
    m_logger.error("Could not find the eNB's UEs.");
    return false;
  }
  ue_set->second.erase(mme_ue_s1ap_id);

  // Release UE ECM context
  m_mme_ue_s1ap_id_to_nas_ctx.erase(mme_ue_s1ap_id);
  ecm_ctx->state          = ECM_STATE_IDLE;
  ecm_ctx->mme_ue_s1ap_id = 0;
  ecm_ctx->enb_ue_s1ap_id = 0;

  m_logger.info("Released UE ECM Context.");
  return true;
}

bool s1ap::delete_ue_ctx(uint64_t imsi)
{
  nas* nas_ctx = find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    m_logger.info("Cannot delete UE context, UE not found. IMSI: %" PRIu64 "", imsi);
    return false;
  }

  // Make sure to release ECM ctx
  if (nas_ctx->m_ecm_ctx.mme_ue_s1ap_id != 0) {
    release_ue_ecm_ctx(nas_ctx->m_ecm_ctx.mme_ue_s1ap_id);
  }

  // Delete UE context
  m_imsi_to_nas_ctx.erase(imsi);
  delete nas_ctx;
  m_logger.info("Deleted UE Context.");
  return true;
}

// UE Bearer Managment
void s1ap::activate_eps_bearer(uint64_t imsi, uint8_t ebi)
{
  std::map<uint64_t, nas*>::iterator ue_ctx_it = m_imsi_to_nas_ctx.find(imsi);
  if (ue_ctx_it == m_imsi_to_nas_ctx.end()) {
    m_logger.error("Could not activate EPS bearer: Could not find UE context");
    return;
  }
  // Make sure NAS is active
  uint32_t                           mme_ue_s1ap_id = ue_ctx_it->second->m_ecm_ctx.mme_ue_s1ap_id;
  std::map<uint32_t, nas*>::iterator it             = m_mme_ue_s1ap_id_to_nas_ctx.find(mme_ue_s1ap_id);
  if (it == m_mme_ue_s1ap_id_to_nas_ctx.end()) {
    m_logger.error("Could not activate EPS bearer: ECM context seems to be missing");
    return;
  }

  ecm_ctx_t* ecm_ctx = &ue_ctx_it->second->m_ecm_ctx;
  esm_ctx_t* esm_ctx = &ue_ctx_it->second->m_esm_ctx[ebi];
  if (esm_ctx->state != ERAB_CTX_SETUP) {
    m_logger.error(
        "Could not be activate EPS Bearer, bearer in wrong state: MME S1AP Id %d, EPS Bearer id %d, state %d",
        mme_ue_s1ap_id,
        ebi,
        esm_ctx->state);
    srsran::console(
        "Could not be activate EPS Bearer, bearer in wrong state: MME S1AP Id %d, EPS Bearer id %d, state %d\n",
        mme_ue_s1ap_id,
        ebi,
        esm_ctx->state);
    return;
  }

  esm_ctx->state = ERAB_ACTIVE;
  ecm_ctx->state = ECM_STATE_CONNECTED;
  m_logger.info("Activated EPS Bearer: Bearer id %d", ebi);
  return;
}

uint32_t s1ap::allocate_m_tmsi(uint64_t imsi)
{
  uint32_t m_tmsi = m_next_m_tmsi;
  m_next_m_tmsi   = (m_next_m_tmsi + 1) % UINT32_MAX;

  m_tmsi_to_imsi.insert(std::pair<uint32_t, uint64_t>(m_tmsi, imsi));
  m_logger.debug("Allocated M-TMSI 0x%x to IMSI %015" PRIu64 ",", m_tmsi, imsi);
  return m_tmsi;
}

uint64_t s1ap::find_imsi_from_m_tmsi(uint32_t m_tmsi)
{
  std::map<uint32_t, uint64_t>::iterator it = m_tmsi_to_imsi.find(m_tmsi);
  if (it != m_tmsi_to_imsi.end()) {
    m_logger.debug("Found IMSI %015" PRIu64 " from M-TMSI 0x%x", it->second, m_tmsi);
    return it->second;
  } else {
    m_logger.debug("Could not find IMSI from M-TMSI 0x%x", m_tmsi);
    return SRSRAN_SUCCESS;
  }
}

void s1ap::print_enb_ctx_info(const std::string& prefix, const enb_ctx_t& enb_ctx)
{
  std::string mnc_str, mcc_str;

  if (enb_ctx.enb_name_present) {
    srsran::console("%s - eNB Name: %s, eNB id: 0x%x\n", prefix.c_str(), enb_ctx.enb_name.c_str(), enb_ctx.enb_id);
    m_logger.info("%s - eNB Name: %s, eNB id: 0x%x", prefix.c_str(), enb_ctx.enb_name.c_str(), enb_ctx.enb_id);
  } else {
    srsran::console("%s - eNB Id 0x%x\n", prefix.c_str(), enb_ctx.enb_id);
    m_logger.info("%s - eNB Id 0x%x", prefix.c_str(), enb_ctx.enb_id);
  }
  srsran::mcc_to_string(enb_ctx.mcc, &mcc_str);
  srsran::mnc_to_string(enb_ctx.mnc, &mnc_str);
  m_logger.info("%s - MCC:%s, MNC:%s, PLMN: %d", prefix.c_str(), mcc_str.c_str(), mnc_str.c_str(), enb_ctx.plmn);
  srsran::console("%s - MCC:%s, MNC:%s\n", prefix.c_str(), mcc_str.c_str(), mnc_str.c_str());
  for (int i = 0; i < enb_ctx.nof_supported_ta; i++) {
    for (int j = 0; i < enb_ctx.nof_supported_ta; i++) {
      m_logger.info("%s - TAC %d, B-PLMN 0x%x", prefix.c_str(), enb_ctx.tacs[i], enb_ctx.bplmns[i][j]);
      srsran::console("%s - TAC %d, B-PLMN 0x%x\n", prefix.c_str(), enb_ctx.tacs[i], enb_ctx.bplmns[i][j]);
    }
  }
  srsran::console("%s - Paging DRX %s\n", prefix.c_str(), enb_ctx.drx.to_string());
  return;
}

/*
 * Interfaces
 */

// GTP-C -> S1AP interface
bool s1ap::send_paging(uint64_t imsi, uint16_t erab_to_setup)
{
  m_s1ap_paging->send_paging(imsi, erab_to_setup);
  return true;
}

// GTP-C || NAS -> S1AP interface
bool s1ap::send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup)
{
  nas* nas_ctx = find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    m_logger.error("Error finding NAS context when sending initial context Setup Request");
    return false;
  }
  m_s1ap_ctx_mngmt_proc->send_initial_context_setup_request(nas_ctx, erab_to_setup);
  return true;
}

// NAS -> S1AP interface
bool s1ap::send_ue_context_release_command(uint32_t mme_ue_s1ap_id)
{
  nas* nas_ctx = find_nas_ctx_from_mme_ue_s1ap_id(mme_ue_s1ap_id);
  if (nas_ctx == NULL) {
    m_logger.error("Error finding NAS context when sending UE Context Setup Release");
    return false;
  }
  m_s1ap_ctx_mngmt_proc->send_ue_context_release_command(nas_ctx);
  return true;
}

bool s1ap::send_erab_release_command(uint32_t               enb_ue_s1ap_id,
                                     uint32_t               mme_ue_s1ap_id,
                                     std::vector<uint16_t>  erabs_to_be_released,
                                     struct sctp_sndrcvinfo enb_sri)
{
  return m_s1ap_erab_mngmt_proc->send_erab_release_command(
      enb_ue_s1ap_id, mme_ue_s1ap_id, erabs_to_be_released, enb_sri);
}

bool s1ap::send_erab_modify_request(uint32_t                     enb_ue_s1ap_id,
                                    uint32_t                     mme_ue_s1ap_id,
                                    std::map<uint16_t, uint16_t> erabs_to_be_modified,
                                    srsran::byte_buffer_t*       nas_msg,
                                    struct sctp_sndrcvinfo       enb_sri)
{
  return m_s1ap_erab_mngmt_proc->send_erab_modify_request(
      enb_ue_s1ap_id, mme_ue_s1ap_id, erabs_to_be_modified, nas_msg, enb_sri);
}

bool s1ap::send_downlink_nas_transport(uint32_t               enb_ue_s1ap_id,
                                       uint32_t               mme_ue_s1ap_id,
                                       srsran::byte_buffer_t* nas_msg,
                                       struct sctp_sndrcvinfo enb_sri)
{
  return m_s1ap_nas_transport->send_downlink_nas_transport(enb_ue_s1ap_id, mme_ue_s1ap_id, nas_msg, enb_sri);
}

bool s1ap::expire_nas_timer(enum nas_timer_type type, uint64_t imsi)
{
  nas* nas_ctx = find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    m_logger.error("Error finding NAS context to handle timer");
    return false;
  }
  bool err = nas_ctx->expire_timer(type);
  return err;
}

} // namespace srsepc
