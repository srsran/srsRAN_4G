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

#include "srsenb/hdr/stack/s1ap/s1ap.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/enb_events.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/enb_rrc_interface_s1ap.h"

#include <arpa/inet.h> //for inet_ntop()
#include <inttypes.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

using srsran::s1ap_mccmnc_to_plmn;
using srsran::uint32_to_uint8;

#define procError(fmt, ...) s1ap_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) s1ap_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procInfo(fmt, ...) s1ap_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procDebug(fmt, ...) s1ap_ptr->logger.debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

#define WarnUnsupportFeature(cond, featurename)                                                                        \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      logger.warning("Not handling feature - %s", featurename);                                                        \
    }                                                                                                                  \
  } while (0)

using namespace asn1::s1ap;

namespace srsenb {

/*************************
 *    Helper Functions
 ************************/

asn1::bounded_bitstring<1, 160, true, true> addr_to_asn1(const char* addr_str)
{
  asn1::bounded_bitstring<1, 160, true, true> transport_layer_addr(32);
  uint8_t                                     addr[4];
  if (inet_pton(AF_INET, addr_str, addr) != 1) {
    srsran::console("Invalid addr_str: %s\n", addr_str);
    perror("inet_pton");
  }
  for (uint32_t j = 0; j < 4; ++j) {
    transport_layer_addr.data()[j] = addr[3 - j];
  }
  return transport_layer_addr;
}

/// Helper to add ERAB items that are duplicates in the received S1AP message
template <typename List>
void add_repeated_erab_ids(const List&                                                   list,
                           srsran::bounded_vector<erab_item_s, ASN1_S1AP_MAXNOOF_ERABS>& failed_cfg_erabs)
{
  for (auto it = list.begin(); it != list.end(); ++it) {
    for (auto it2 = it + 1; it2 != list.end(); ++it2) {
      if (equal_obj_id(*it, *it2)) {
        failed_cfg_erabs.push_back(erab_item_s());
        failed_cfg_erabs.back().erab_id                         = get_obj_id(*it);
        failed_cfg_erabs.back().cause.set_radio_network().value = cause_radio_network_opts::multiple_erab_id_instances;
      }
    }
  }

  // Sort and remove duplications
  std::sort(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), &lower_obj_id<erab_item_s>);
  failed_cfg_erabs.erase(std::unique(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), &equal_obj_id<erab_item_s>),
                         failed_cfg_erabs.end());
}

bool contains_erab_id(srsran::bounded_vector<erab_item_s, ASN1_S1AP_MAXNOOF_ERABS>& failed_cfg_erabs, uint16_t erab_id)
{
  erab_item_s dummy;
  dummy.erab_id = erab_id;
  return std::find_if(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), [erab_id](const erab_item_s& e) {
           return e.erab_id == erab_id;
         }) != failed_cfg_erabs.end();
}

void sanitize_response_erab_lists(s1ap::erab_item_list& failed_cfg_erabs, s1ap::erab_id_list& erabs)
{
  // Sort and remove duplicates
  std::sort(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), &lower_obj_id<erab_item_s>);
  failed_cfg_erabs.erase(std::unique(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), &equal_obj_id<erab_item_s>),
                         failed_cfg_erabs.end());
  std::sort(erabs.begin(), erabs.end());
  erabs.erase(std::unique(erabs.begin(), erabs.end()), erabs.end());
}

template <typename OutList>
void fill_erab_failed_setup_list(OutList& output_list, const s1ap::erab_item_list& input_list)
{
  output_list.resize(input_list.size());
  for (size_t i = 0; i < input_list.size(); ++i) {
    output_list[i].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);
    output_list[i]->erab_item() = input_list[i];
  }
}

/*********************************************************
 * TS 36.413 - Section 8.4.1 - "Handover Preparation"
 *********************************************************/
s1ap::ue::ho_prep_proc_t::ho_prep_proc_t(s1ap::ue* ue_) : ue_ptr(ue_), s1ap_ptr(ue_->s1ap_ptr) {}

srsran::proc_outcome_t s1ap::ue::ho_prep_proc_t::init(uint32_t                     target_eci_,
                                                      uint16_t                     target_tac_,
                                                      srsran::plmn_id_t            target_plmn_,
                                                      srsran::span<uint32_t>       fwd_erabs,
                                                      srsran::unique_byte_buffer_t rrc_container_,
                                                      bool                         has_direct_fwd_path)
{
  ho_cmd_msg  = nullptr;
  target_eci  = target_eci_;
  target_tac  = target_tac_;
  target_plmn = target_plmn_;

  procInfo("Sending HandoverRequired to MME id=%d", ue_ptr->ctxt.mme_ue_s1ap_id.value());
  if (not ue_ptr->send_ho_required(
          target_eci, target_tac, target_plmn, fwd_erabs, std::move(rrc_container_), has_direct_fwd_path)) {
    procError("Failed to send HORequired to cell 0x%x", target_eci);
    return srsran::proc_outcome_t::error;
  }

  // Start HO preparation timer
  ue_ptr->ts1_reloc_prep.run();

  return srsran::proc_outcome_t::yield;
}
srsran::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(ts1_reloc_prep_expired e)
{
  // do nothing for now
  procError("timer TS1Relocprep has expired.");
  return srsran::proc_outcome_t::error;
}
srsran::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(const ho_prep_fail_s& msg)
{
  ue_ptr->ts1_reloc_prep.stop();

  std::string cause = s1ap_ptr->get_cause(msg->cause.value);
  procError("HO preparation Failure. Cause: %s", cause.c_str());
  srsran::console("HO preparation Failure. Cause: %s\n", cause.c_str());

  return srsran::proc_outcome_t::error;
}

/**
 * TS 36.413 - Section 8.4.1.2 - HandoverPreparation Successful Operation
 * Description: MME returns back an HandoverCommand to the SeNB
 */
srsran::proc_outcome_t s1ap::ue::ho_prep_proc_t::react(const asn1::s1ap::ho_cmd_s& msg)
{
  // update timers
  ue_ptr->ts1_reloc_prep.stop();
  ue_ptr->ts1_reloc_overall.run();

  // Check for unsupported S1AP fields
  if (msg.ext or msg->target_to_source_transparent_container_secondary_present or
      msg->handov_type.value.value != handov_type_opts::intralte or msg->crit_diagnostics_present or
      msg->nas_security_paramsfrom_e_utran_present) {
    procWarning("Not handling HandoverCommand extensions and non-intraLTE params");
  }

  // In case of intra-system Handover, Target to Source Transparent Container IE shall be encoded as
  // Target eNB to Source eNB Transparent Container IE
  asn1::cbit_ref bref(msg->target_to_source_transparent_container.value.data(),
                      msg->target_to_source_transparent_container.value.size());
  asn1::s1ap::targetenb_to_sourceenb_transparent_container_s container;
  if (container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    procError("Failed to decode TargeteNBToSourceeNBTransparentContainer");
    return srsran::proc_outcome_t::error;
  }
  if (container.ie_exts_present or container.ext) {
    procWarning("Not handling extensions");
  }

  // Create a unique buffer out of transparent container to pass to RRC
  rrc_container = srsran::make_byte_buffer();
  if (rrc_container == nullptr) {
    procError("Fatal Error: Couldn't allocate buffer.");
    return srsran::proc_outcome_t::error;
  }
  memcpy(rrc_container->msg, container.rrc_container.data(), container.rrc_container.size());
  rrc_container->N_bytes = container.rrc_container.size();
  ho_cmd_msg             = &msg;

  return srsran::proc_outcome_t::success;
}

void s1ap::ue::ho_prep_proc_t::then(const srsran::proc_state_t& result)
{
  if (result.is_error()) {
    rrc_interface_s1ap::ho_prep_result ho_prep_result = ue_ptr->ts1_reloc_prep.is_expired()
                                                            ? rrc_interface_s1ap::ho_prep_result::timeout
                                                            : rrc_interface_s1ap::ho_prep_result::failure;
    s1ap_ptr->rrc->ho_preparation_complete(ue_ptr->ctxt.rnti, ho_prep_result, *ho_cmd_msg, {});
  } else {
    s1ap_ptr->rrc->ho_preparation_complete(
        ue_ptr->ctxt.rnti, rrc_interface_s1ap::ho_prep_result::success, *ho_cmd_msg, std::move(rrc_container));
    procInfo("Completed with success");
  }
}

/*********************************************************
 *                     MME Connection
 *********************************************************/

srsran::proc_outcome_t s1ap::s1_setup_proc_t::init()
{
  procInfo("Starting new MME connection.");
  connect_count++;
  return start_mme_connection();
}

srsran::proc_outcome_t s1ap::s1_setup_proc_t::start_mme_connection()
{
  if (not s1ap_ptr->running) {
    procInfo("S1AP is not running anymore.");
    return srsran::proc_outcome_t::error;
  }
  if (s1ap_ptr->mme_connected) {
    procInfo("eNB S1AP is already connected to MME");
    return srsran::proc_outcome_t::success;
  }

  auto connect_callback = [this]() {
    bool connected = s1ap_ptr->connect_mme();

    auto notify_result = [this, connected]() {
      s1_setup_proc_t::s1connectresult res;
      res.success = connected;
      s1ap_ptr->s1setup_proc.trigger(res);
    };
    s1ap_ptr->task_sched.notify_background_task_result(notify_result);
  };
  srsran::get_background_workers().push_task(connect_callback);
  procDebug("Connection to MME requested.");

  return srsran::proc_outcome_t::yield;
}

srsran::proc_outcome_t s1ap::s1_setup_proc_t::react(const srsenb::s1ap::s1_setup_proc_t::s1connectresult& event)
{
  if (event.success) {
    procInfo("Connected to MME. Sending S1 setup request.");
    s1ap_ptr->s1setup_timeout.run();
    if (not s1ap_ptr->setup_s1()) {
      procError("S1 setup failed. Exiting...");
      srsran::console("S1 setup failed\n");
      s1ap_ptr->running = false;
      return srsran::proc_outcome_t::error;
    }
    procInfo("S1 setup request sent. Waiting for response.");
    return srsran::proc_outcome_t::yield;
  }

  procInfo("Could not connected to MME. Aborting");
  return srsran::proc_outcome_t::error;
}

srsran::proc_outcome_t s1ap::s1_setup_proc_t::react(const srsenb::s1ap::s1_setup_proc_t::s1setupresult& event)
{
  if (s1ap_ptr->s1setup_timeout.is_running()) {
    s1ap_ptr->s1setup_timeout.stop();
  }
  if (event.success) {
    procInfo("S1Setup procedure completed successfully");
    return srsran::proc_outcome_t::success;
  }
  procError("S1Setup failed.");
  srsran::console("S1setup failed\n");
  return srsran::proc_outcome_t::error;
}

void s1ap::s1_setup_proc_t::then(const srsran::proc_state_t& result)
{
  if (result.is_error()) {
    procInfo("Failed to initiate S1 connection. Attempting reconnection in %d seconds",
             s1ap_ptr->mme_connect_timer.duration() / 1000);
    srsran::console("Failed to initiate S1 connection. Attempting reconnection in %d seconds\n",
                    s1ap_ptr->mme_connect_timer.duration() / 1000);
    s1ap_ptr->rx_socket_handler->remove_socket(s1ap_ptr->mme_socket.get_socket());
    s1ap_ptr->mme_socket.close();
    procInfo("S1AP socket closed.");
    s1ap_ptr->mme_connect_timer.run();
    if (s1ap_ptr->args.max_s1_setup_retries > 0 && connect_count > s1ap_ptr->args.max_s1_setup_retries) {
      s1ap_ptr->alarms_channel("s1apError");
      srsran_terminate("Error connecting to MME");
    }
    // Try again with in 10 seconds
  } else {
    connect_count = 0;
  }
}

/*********************************************************
 *                     S1AP class
 *********************************************************/

s1ap::s1ap(srsran::task_sched_handle   task_sched_,
           srslog::basic_logger&       logger,
           srsran::socket_manager_itf* rx_socket_handler_) :
  s1setup_proc(this),
  logger(logger),
  task_sched(task_sched_),
  rx_socket_handler(rx_socket_handler_),
  alarms_channel(srslog::fetch_log_channel("alarms"))
{
  mme_task_queue = task_sched.make_task_queue();
}

int s1ap::init(const s1ap_args_t& args_, rrc_interface_s1ap* rrc_)
{
  rrc  = rrc_;
  args = args_;

  build_tai_cgi();

  // Setup MME reconnection timer
  mme_connect_timer    = task_sched.get_unique_timer();
  auto mme_connect_run = [this](uint32_t tid) {
    if (s1setup_proc.is_busy()) {
      logger.error("Failed to initiate S1Setup procedure: procedure is busy.");
    }
    s1setup_proc.launch();
  };
  mme_connect_timer.set(args.s1_connect_timer * 1000, mme_connect_run);
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
    logger.error("Failed to initiate S1Setup procedure: error launching procedure.");
  }

  return SRSRAN_SUCCESS;
}

void s1ap::stop()
{
  running = false;
  mme_socket.close();
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
void s1ap::initial_ue(uint16_t                              rnti,
                      uint32_t                              enb_cc_idx,
                      asn1::s1ap::rrc_establishment_cause_e cause,
                      srsran::unique_byte_buffer_t          pdu)
{
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.rnti       = rnti;
  ue_ptr->ctxt.enb_cc_idx = enb_cc_idx;
  ue* u                   = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    logger.error("Failed to add rnti=0x%x", rnti);
    return;
  }
  u->send_initialuemessage(cause, std::move(pdu), false);
}

void s1ap::initial_ue(uint16_t                              rnti,
                      uint32_t                              enb_cc_idx,
                      asn1::s1ap::rrc_establishment_cause_e cause,
                      srsran::unique_byte_buffer_t          pdu,
                      uint32_t                              m_tmsi,
                      uint8_t                               mmec)
{
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.rnti       = rnti;
  ue_ptr->ctxt.enb_cc_idx = enb_cc_idx;
  ue* u                   = users.add_user(std::move(ue_ptr));
  if (u == nullptr) {
    logger.error("Failed to add rnti=0x%x", rnti);
    return;
  }
  u->send_initialuemessage(cause, std::move(pdu), true, m_tmsi, mmec);
}

void s1ap::write_pdu(uint16_t rnti, srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Received RRC SDU");

  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    logger.info("The rnti=0x%x does not exist", rnti);
    return;
  }
  u->send_ulnastransport(std::move(pdu));
}

bool s1ap::user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    logger.warning("Released UE with rnti=0x%x not found", rnti);
    rrc->release_ue(rnti);
    return false;
  }

  cause_c cause;
  cause.set_radio_network().value = cause_radio.value;

  return u->send_uectxtreleaserequest(cause);
}

bool s1ap::user_exists(uint16_t rnti)
{
  return users.find_ue_rnti(rnti) != nullptr;
}

void s1ap::user_mod(uint16_t old_rnti, uint16_t new_rnti)
{
  logger.info("Modifying user context. Old rnti: 0x%x, new rnti: 0x%x", old_rnti, new_rnti);
  if (not user_exists(old_rnti)) {
    logger.error("Old rnti does not exist, aborting.");
    return;
  }
  if (user_exists(new_rnti)) {
    logger.error("New rnti already exists, aborting.");
    return;
  }
  users.find_ue_rnti(old_rnti)->ctxt.rnti = new_rnti;
}

void s1ap::ue_ctxt_setup_complete(uint16_t rnti)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return;
  }
  u->ue_ctxt_setup_complete();
}

void s1ap::notify_rrc_reconf_complete(uint16_t rnti)
{
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return;
  }
  u->notify_rrc_reconf_complete();
}

bool s1ap::is_mme_connected()
{
  return mme_connected;
}

/*******************************************************************************
/* S1AP connection helpers
********************************************************************************/

bool s1ap::connect_mme()
{
  using namespace srsran::net_utils;
  logger.info("Connecting to MME %s:%d", args.mme_addr.c_str(), int(MME_PORT));

  // Open SCTP socket
  if (not mme_socket.open_socket(
          srsran::net_utils::addr_family::ipv4, socket_type::seqpacket, srsran::net_utils::protocol_type::SCTP)) {
    return false;
  }

  // Set SO_REUSE_ADDR if necessary
  if (args.sctp_reuse_addr) {
    if (not mme_socket.reuse_addr()) {
      mme_socket.close();
      return false;
    }
  }

  // Subscribe to shutdown events
  if (not mme_socket.sctp_subscribe_to_events()) {
    mme_socket.close();
    return false;
  }

  // Set SRTO_MAX
  if (not mme_socket.sctp_set_rto_opts(args.sctp_rto_max)) {
    return false;
  }

  // Set SCTP init options
  if (not mme_socket.sctp_set_init_msg_opts(args.sctp_init_max_attempts, args.sctp_max_init_timeo)) {
    return false;
  }

  // Bind socket
  if (not mme_socket.bind_addr(args.s1c_bind_addr.c_str(), args.s1c_bind_port)) {
    mme_socket.close();
    return false;
  }
  logger.info("SCTP socket opened. fd=%d", mme_socket.fd());

  // Connect to the MME address
  if (not mme_socket.connect_to(args.mme_addr.c_str(), MME_PORT, &mme_addr)) {
    return false;
  }
  logger.info("SCTP socket connected with MME. fd=%d", mme_socket.fd());

  // Assign a handler to rx MME packets
  auto rx_callback =
      [this](srsran::unique_byte_buffer_t pdu, const sockaddr_in& from, const sctp_sndrcvinfo& sri, int flags) {
        // Defer the handling of MME packet to eNB stack main thread
        handle_mme_rx_msg(std::move(pdu), from, sri, flags);
      };
  rx_socket_handler->add_socket_handler(mme_socket.fd(),
                                        srsran::make_sctp_sdu_handler(logger, mme_task_queue, rx_callback));

  logger.info("SCTP socket established with MME");
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
  s1_setup_request_s& container             = pdu.init_msg().value.s1_setup_request();
  container->global_enb_id.value.plm_nid[0] = ((uint8_t*)&plmn)[1];
  container->global_enb_id.value.plm_nid[1] = ((uint8_t*)&plmn)[2];
  container->global_enb_id.value.plm_nid[2] = ((uint8_t*)&plmn)[3];

  container->global_enb_id.value.enb_id.set_macro_enb_id().from_number(args.enb_id);

  container->enbname_present = true;
  container->enbname.value.from_string(args.enb_name);

  container->supported_tas.value.resize(1);
  tmp16 = htons(args.tac);
  memcpy(container->supported_tas.value[0].tac.data(), (uint8_t*)&tmp16, 2);
  container->supported_tas.value[0].broadcast_plmns.resize(1);
  container->supported_tas.value[0].broadcast_plmns[0][0] = ((uint8_t*)&plmn)[1];
  container->supported_tas.value[0].broadcast_plmns[0][1] = ((uint8_t*)&plmn)[2];
  container->supported_tas.value[0].broadcast_plmns[0][2] = ((uint8_t*)&plmn)[3];

  container->default_paging_drx.value.value = asn1::s1ap::paging_drx_opts::v128; // Todo: add to args, config file

  return sctp_send_s1ap_pdu(pdu, 0, "s1SetupRequest");
}

/*******************************************************************************
/* S1AP message handlers
********************************************************************************/

bool s1ap::handle_mme_rx_msg(srsran::unique_byte_buffer_t pdu,
                             const sockaddr_in&           from,
                             const sctp_sndrcvinfo&       sri,
                             int                          flags)
{
  // Handle Notification Case
  if (flags & MSG_NOTIFICATION) {
    // Received notification
    union sctp_notification* notification = (union sctp_notification*)pdu->msg;
    logger.info("SCTP Notification %04x", notification->sn_header.sn_type);
    bool restart_s1 = false;
    if (notification->sn_header.sn_type == SCTP_SHUTDOWN_EVENT) {
      logger.info("SCTP Association Shutdown. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP Association Shutdown. Association: %d\n", sri.sinfo_assoc_id);
      restart_s1 = true;
    } else if (notification->sn_header.sn_type == SCTP_PEER_ADDR_CHANGE &&
               notification->sn_paddr_change.spc_state == SCTP_ADDR_UNREACHABLE) {
      logger.info("SCTP peer addres unreachable. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP peer address unreachable. Association: %d\n", sri.sinfo_assoc_id);
      restart_s1 = true;
    } else if (notification->sn_header.sn_type == SCTP_REMOTE_ERROR) {
      logger.info("SCTP remote error. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP remote error. Association: %d\n", sri.sinfo_assoc_id);
      restart_s1 = true;
    } else if (notification->sn_header.sn_type == SCTP_ASSOC_CHANGE) {
      logger.info("SCTP association changed. Association: %d", sri.sinfo_assoc_id);
      srsran::console("SCTP association changed. Association: %d\n", sri.sinfo_assoc_id);
    }
    if (restart_s1) {
      logger.info("Restarting S1 connection");
      srsran::console("Restarting S1 connection\n");
      rx_socket_handler->remove_socket(mme_socket.get_socket());
      mme_socket.close();
      while (users.size() != 0) {
        std::unordered_map<uint32_t, std::unique_ptr<ue> >::iterator it   = users.begin();
        uint16_t                                                     rnti = it->second->ctxt.rnti;
        rrc->release_erabs(rnti);
        rrc->release_ue(rnti);
        users.erase(it->second.get());
      }
    }
  } else if (pdu->N_bytes == 0) {
    logger.error("SCTP return 0 bytes. Closing socket");
    mme_socket.close();
  }

  // Restart MME connection procedure if we lost connection
  if (not mme_socket.is_open()) {
    mme_connected = false;
    if (s1setup_proc.is_busy()) {
      logger.error("Failed to initiate MME connection procedure, as it is already running.");
      return false;
    }
    s1setup_proc.launch();
    return false;
  }

  if ((flags & MSG_NOTIFICATION) == 0 && pdu->N_bytes != 0) {
    handle_s1ap_rx_pdu(pdu.get());
  }

  return true;
}

bool s1ap::handle_s1ap_rx_pdu(srsran::byte_buffer_t* pdu)
{
  // Save message to PCAP
  if (pcap != nullptr) {
    pcap->write_s1ap(pdu->msg, pdu->N_bytes);
  }

  s1ap_pdu_c     rx_pdu;
  asn1::cbit_ref bref(pdu->msg, pdu->N_bytes);

  if (rx_pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error(pdu->msg, pdu->N_bytes, "Failed to unpack received PDU");
    cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::transfer_syntax_error;
    send_error_indication(cause);
    return false;
  }
  log_s1ap_msg(rx_pdu, srsran::make_span(*pdu), true);

  switch (rx_pdu.type().value) {
    case s1ap_pdu_c::types_opts::init_msg:
      return handle_initiatingmessage(rx_pdu.init_msg());
    case s1ap_pdu_c::types_opts::successful_outcome:
      return handle_successfuloutcome(rx_pdu.successful_outcome());
    case s1ap_pdu_c::types_opts::unsuccessful_outcome:
      return handle_unsuccessfuloutcome(rx_pdu.unsuccessful_outcome());
    default:
      logger.error("Unhandled PDU type %d", rx_pdu.type().value);
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
    case s1ap_elem_procs_o::init_msg_c::types_opts::erab_release_cmd:
      return handle_erabreleasecommand(msg.value.erab_release_cmd());
    case s1ap_elem_procs_o::init_msg_c::types_opts::erab_modify_request:
      return handle_erabmodifyrequest(msg.value.erab_modify_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request:
      return handle_uecontextmodifyrequest(msg.value.ue_context_mod_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::ho_request:
      return handle_handover_request(msg.value.ho_request());
    case s1ap_elem_procs_o::init_msg_c::types_opts::mme_status_transfer:
      return handle_mme_status_transfer(msg.value.mme_status_transfer());
    default:
      logger.error("Unhandled initiating message: %s", msg.value.type().to_string());
  }
  return true;
}

bool s1ap::handle_successfuloutcome(const successful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::s1_setup_resp:
      return handle_s1setupresponse(msg.value.s1_setup_resp());
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::ho_cmd:
      return handle_handover_command(msg.value.ho_cmd());
    case s1ap_elem_procs_o::successful_outcome_c::types_opts::ho_cancel_ack:
      return true;
    default:
      logger.error("Unhandled successful outcome message: %s", msg.value.type().to_string());
  }
  return true;
}

bool s1ap::handle_unsuccessfuloutcome(const unsuccessful_outcome_s& msg)
{
  switch (msg.value.type().value) {
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::s1_setup_fail:
      return handle_s1setupfailure(msg.value.s1_setup_fail());
    case s1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail:
      return handle_handover_preparation_failure(msg.value.ho_prep_fail());
    default:
      logger.error("Unhandled unsuccessful outcome message: %s", msg.value.type().to_string());
  }
  return true;
}

bool s1ap::handle_s1setupresponse(const asn1::s1ap::s1_setup_resp_s& msg)
{
  if (s1setup_proc.is_idle()) {
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
    send_error_indication(cause);
    return false;
  }

  s1setupresponse = msg;
  mme_connected   = true;
  s1_setup_proc_t::s1setupresult res;
  res.success = true;
  s1setup_proc.trigger(res);
  return true;
}

bool s1ap::handle_dlnastransport(const dl_nas_transport_s& msg)
{
  if (msg.ext) {
    logger.warning("Not handling S1AP message extension");
  }
  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (msg->ho_restrict_list_present) {
    logger.warning("Not handling HandoverRestrictionList");
  }
  if (msg->subscriber_profile_idfor_rfp_present) {
    logger.warning("Not handling SubscriberProfileIDforRFP");
  }

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Fatal Error: Couldn't allocate buffer in s1ap::run_thread().");
    return false;
  }
  memcpy(pdu->msg, msg->nas_pdu.value.data(), msg->nas_pdu.value.size());
  pdu->N_bytes = msg->nas_pdu.value.size();
  rrc->write_dl_info(u->ctxt.rnti, std::move(pdu));
  return true;
}

bool s1ap::handle_initialctxtsetuprequest(const init_context_setup_request_s& msg)
{
  WarnUnsupportFeature(msg.ext, "message extension");
  WarnUnsupportFeature(msg->add_cs_fallback_ind_present, "AdditionalCSFallbackIndicator");
  WarnUnsupportFeature(msg->csg_membership_status_present, "CSGMembershipStatus");
  WarnUnsupportFeature(msg->gummei_id_present, "GUMMEI_ID");
  WarnUnsupportFeature(msg->ho_restrict_list_present, "HandoverRestrictionList");
  WarnUnsupportFeature(msg->management_based_mdt_allowed_present, "ManagementBasedMDTAllowed");
  WarnUnsupportFeature(msg->management_based_mdtplmn_list_present, "ManagementBasedMDTPLMNList");
  WarnUnsupportFeature(msg->mme_ue_s1ap_id_minus2_present, "MME_UE_S1AP_ID_2");
  WarnUnsupportFeature(msg->registered_lai_present, "RegisteredLAI");
  WarnUnsupportFeature(msg->srvcc_operation_possible_present, "SRVCCOperationPossible");
  WarnUnsupportFeature(msg->subscriber_profile_idfor_rfp_present, "SubscriberProfileIDforRFP");
  WarnUnsupportFeature(msg->trace_activation_present, "TraceActivation");
  WarnUnsupportFeature(msg->ue_radio_cap_present, "UERadioCapability");

  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (u->get_state() == s1ap_proc_id_t::init_context_setup_request) {
    logger.warning("Initial Context Setup Request already in progress. Ignoring ICS request.");
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
    send_error_indication(cause, msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
    return false;
  }

  // Setup UE ctxt in RRC
  if (not rrc->setup_ue_ctxt(u->ctxt.rnti, msg)) {
    return false;
  }

  // Update E-RABs
  erab_id_list   updated_erabs;
  erab_item_list failed_cfg_erabs;
  add_repeated_erab_ids(msg->erab_to_be_setup_list_ctxt_su_req.value, failed_cfg_erabs);

  for (const auto& item : msg->erab_to_be_setup_list_ctxt_su_req.value) {
    const auto& erab = item->erab_to_be_setup_item_ctxt_su_req();
    if (contains_erab_id(failed_cfg_erabs, erab.erab_id)) {
      // E-RAB is duplicate
      continue;
    }
    WarnUnsupportFeature(erab.ext, "E-RABToBeSetupListBearerSUReq extensions");
    WarnUnsupportFeature(erab.ie_exts_present, "E-RABToBeSetupListBearerSUReq extensions");

    if (erab.transport_layer_address.length() > 32) {
      logger.error("IPv6 addresses not currently supported");
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id                         = erab.erab_id;
      failed_cfg_erabs.back().cause.set_radio_network().value = cause_radio_network_opts::invalid_qos_combination;
      continue;
    }

    cause_c cause;
    if (rrc->setup_erab(u->ctxt.rnti,
                        erab.erab_id,
                        erab.erab_level_qos_params,
                        erab.nas_pdu,
                        erab.transport_layer_address,
                        erab.gtp_teid.to_number(),
                        cause) == SRSRAN_SUCCESS) {
      updated_erabs.push_back(erab.erab_id);
    } else {
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id = erab.erab_id;
      failed_cfg_erabs.back().cause   = cause;
    }
  }

  /* Ideally the check below would be "if (users[rnti].is_csfb)" */
  if (msg->cs_fallback_ind_present) {
    if (msg->cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_required ||
        msg->cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_high_prio) {
      // Send RRC Release (cs-fallback-triggered) to MME
      cause_c cause;
      cause.set_radio_network().value = cause_radio_network_opts::cs_fallback_triggered;
      /* TODO: This should normally probably only be sent after the SecurityMode procedure has completed! */
      u->send_uectxtreleaserequest(cause);
    }
  }

  // E-RAB Setup Response is sent after the security cfg is complete
  // Note: No need to notify RRC to send RRC Reconfiguration
  sanitize_response_erab_lists(failed_cfg_erabs, updated_erabs);
  u->set_state(s1ap_proc_id_t::init_context_setup_request, updated_erabs, failed_cfg_erabs);
  return true;
}

bool s1ap::handle_paging(const asn1::s1ap::paging_s& msg)
{
  WarnUnsupportFeature(msg.ext, "S1AP message extension");

  uint32_t ueid = msg->ue_id_idx_value.value.to_number();
  rrc->add_paging_id(ueid, msg->ue_paging_id.value);
  return true;
}

bool s1ap::handle_erabsetuprequest(const erab_setup_request_s& msg)
{
  WarnUnsupportFeature(msg.ext, "S1AP message extension");

  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (msg->ueaggregate_maximum_bitrate_present) {
    rrc->set_aggregate_max_bitrate(u->ctxt.rnti, msg->ueaggregate_maximum_bitrate.value);
  }

  erab_id_list   updated_erabs;
  erab_item_list failed_cfg_erabs;
  add_repeated_erab_ids(msg->erab_to_be_setup_list_bearer_su_req.value, failed_cfg_erabs);

  for (const auto& item : msg->erab_to_be_setup_list_bearer_su_req.value) {
    const auto& erab = item->erab_to_be_setup_item_bearer_su_req();
    if (contains_erab_id(failed_cfg_erabs, erab.erab_id)) {
      // E-RAB is duplicate
      continue;
    }
    WarnUnsupportFeature(erab.ext, "E-RABToBeSetupListBearerSUReq extensions");
    WarnUnsupportFeature(erab.ie_exts_present, "E-RABToBeSetupListBearerSUReq extensions");

    if (erab.transport_layer_address.length() > 32) {
      logger.error("IPv6 addresses not currently supported");
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id                         = erab.erab_id;
      failed_cfg_erabs.back().cause.set_radio_network().value = cause_radio_network_opts::invalid_qos_combination;
      continue;
    }

    cause_c cause;
    if (rrc->setup_erab(u->ctxt.rnti,
                        erab.erab_id,
                        erab.erab_level_qos_params,
                        erab.nas_pdu,
                        erab.transport_layer_address,
                        erab.gtp_teid.to_number(),
                        cause) == SRSRAN_SUCCESS) {
      updated_erabs.push_back(erab.erab_id);
    } else {
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id = erab.erab_id;
      failed_cfg_erabs.back().cause   = cause;
    }
  }

  // Notify UE of updates
  if (not updated_erabs.empty()) {
    rrc->notify_ue_erab_updates(u->ctxt.rnti, {});
  }

  sanitize_response_erab_lists(failed_cfg_erabs, updated_erabs);
  return u->send_erab_setup_response(updated_erabs, failed_cfg_erabs);
}

bool s1ap::handle_erabmodifyrequest(const erab_modify_request_s& msg)
{
  WarnUnsupportFeature(msg.ext, "S1AP message extension");

  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (msg->ueaggregate_maximum_bitrate_present) {
    rrc->set_aggregate_max_bitrate(u->ctxt.rnti, msg->ueaggregate_maximum_bitrate.value);
  }

  erab_id_list   updated_erabs;
  erab_item_list failed_cfg_erabs;
  add_repeated_erab_ids(msg->erab_to_be_modified_list_bearer_mod_req.value, failed_cfg_erabs);

  for (const auto& item : msg->erab_to_be_modified_list_bearer_mod_req.value) {
    const auto& erab = item->erab_to_be_modified_item_bearer_mod_req();
    if (contains_erab_id(failed_cfg_erabs, erab.erab_id)) {
      // E-RAB is duplicate
      continue;
    }
    WarnUnsupportFeature(erab.ext, "E-RABToBeSetupListBearerSUReq extensions");
    WarnUnsupportFeature(erab.ie_exts_present, "E-RABToBeSetupListBearerSUReq extensions");

    cause_c cause;
    if (rrc->modify_erab(u->ctxt.rnti, erab.erab_id, erab.erab_level_qos_params, erab.nas_pdu, cause) ==
        SRSRAN_SUCCESS) {
      updated_erabs.push_back(erab.erab_id);
    } else {
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id = erab.erab_id;
      failed_cfg_erabs.back().cause   = cause;
    }
  }

  // Notify UE of updates
  if (not updated_erabs.empty()) {
    rrc->notify_ue_erab_updates(u->ctxt.rnti, {});
  }

  // send E-RAB modify response back to the mme
  sanitize_response_erab_lists(failed_cfg_erabs, updated_erabs);
  return u->send_erab_modify_response(updated_erabs, failed_cfg_erabs);
}

/**
 * @brief eNB handles MME's message "E-RAB RELEASE COMMAND"
 *        @remark TS 36.413, Section 8.2.3.2 - E-RAB Release - MME initiated (successful operation)
 * @param erabs_successfully_released
 * @param erabs_failed_to_release
 * @return true if message was sent
 */
bool s1ap::handle_erabreleasecommand(const erab_release_cmd_s& msg)
{
  WarnUnsupportFeature(msg.ext, "S1AP message extension");

  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  erab_id_list   updated_erabs;
  erab_item_list failed_cfg_erabs;

  auto is_repeated_erab_id = [&updated_erabs, &failed_cfg_erabs](uint8_t erab_id) {
    return (std::count(updated_erabs.begin(), updated_erabs.end(), erab_id) > 0) or
           (std::any_of(failed_cfg_erabs.begin(), failed_cfg_erabs.end(), [erab_id](const erab_item_s& e) {
             return e.erab_id == erab_id;
           }));
  };
  for (const auto& item : msg->erab_to_be_released_list.value) {
    const auto& erab = item->erab_item();

    if (is_repeated_erab_id(erab.erab_id)) {
      // TS 36.413, 8.2.3.3 - ignore the duplication of E-RAB ID IEs
      continue;
    }

    if (rrc->release_erab(u->ctxt.rnti, erab.erab_id) == SRSRAN_SUCCESS) {
      updated_erabs.push_back(erab.erab_id);
    } else {
      failed_cfg_erabs.push_back(erab_item_s());
      failed_cfg_erabs.back().erab_id                         = erab.erab_id;
      failed_cfg_erabs.back().cause.set_radio_network().value = cause_radio_network_opts::unknown_erab_id;
    }
  }

  // Notify RRC of E-RAB update. (RRC reconf message is going to be sent.
  if (not updated_erabs.empty()) {
    rrc->notify_ue_erab_updates(u->ctxt.rnti, msg->nas_pdu.value);
  }

  // Send E-RAB release response back to the MME
  sanitize_response_erab_lists(failed_cfg_erabs, updated_erabs);
  if (not u->send_erab_release_response(updated_erabs, failed_cfg_erabs)) {
    logger.info("Failed to send ERABReleaseResponse");
    return false;
  }

  return true;
}

bool s1ap::handle_uecontextmodifyrequest(const ue_context_mod_request_s& msg)
{
  WarnUnsupportFeature(msg.ext, "S1AP message extension");
  WarnUnsupportFeature(msg->add_cs_fallback_ind_present, "AdditionalCSFallbackIndicator");
  WarnUnsupportFeature(msg->csg_membership_status_present, "CSGMembershipStatus");
  WarnUnsupportFeature(msg->registered_lai_present, "RegisteredLAI");
  WarnUnsupportFeature(msg->subscriber_profile_idfor_rfp_present, "SubscriberProfileIDforRFP");

  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
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
  if (msg->cs_fallback_ind_present) {
    if (msg->cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_required ||
        msg->cs_fallback_ind.value.value == cs_fallback_ind_opts::cs_fallback_high_prio) {
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
  WarnUnsupportFeature(msg.ext, "S1AP message extension");

  ue* u = nullptr;
  if (msg->ue_s1ap_ids.value.type().value == ue_s1ap_ids_c::types_opts::ue_s1ap_id_pair) {
    const auto& idpair = msg->ue_s1ap_ids.value.ue_s1ap_id_pair();

    if (idpair.ext) {
      logger.warning("Not handling S1AP message extension");
    }
    if (idpair.ie_exts_present) {
      logger.warning("Not handling S1AP message iE_Extensions");
    }
    u = handle_s1apmsg_ue_id(idpair.enb_ue_s1ap_id, idpair.mme_ue_s1ap_id);
    if (u == nullptr) {
      return false;
    }
  } else {
    uint32_t mme_ue_id = msg->ue_s1ap_ids.value.mme_ue_s1ap_id();
    u                  = users.find_ue_mmeid(mme_ue_id);
    if (u == nullptr) {
      logger.warning("UE for mme_ue_s1ap_id:%d not found - discarding message", mme_ue_id);
      return false;
    }
  }

  uint16_t rnti = u->ctxt.rnti;
  rrc->release_erabs(rnti);
  u->send_uectxtreleasecomplete();
  users.erase(u);
  logger.info("UE context for RNTI:0x%x released", rnti);
  rrc->release_ue(rnti);
  return true;
}

bool s1ap::handle_s1setupfailure(const asn1::s1ap::s1_setup_fail_s& msg)
{
  if (s1setup_proc.is_idle()) {
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
    send_error_indication(cause);
    return false;
  }

  s1_setup_proc_t::s1setupresult res;
  res.success = false;
  s1setup_proc.trigger(res);

  std::string cause = get_cause(msg->cause.value);
  logger.error("S1 Setup Failure. Cause: %s", cause.c_str());
  srsran::console("S1 Setup Failure. Cause: %s\n", cause.c_str());
  return true;
}

bool s1ap::handle_handover_preparation_failure(const ho_prep_fail_s& msg)
{
  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (u->ho_prep_proc.is_idle()) {
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
    send_error_indication(cause);
    return false;
  }

  u->ho_prep_proc.trigger(msg);
  return true;
}

bool s1ap::handle_handover_command(const asn1::s1ap::ho_cmd_s& msg)
{
  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  if (u->ho_prep_proc.is_idle()) {
    asn1::s1ap::cause_c cause;
    cause.set_protocol().value = cause_protocol_opts::msg_not_compatible_with_receiver_state;
    send_error_indication(cause);
    return false;
  }
  u->ho_prep_proc.trigger(msg);
  return true;
}

/**************************************************************
 * TS 36.413 - Section 8.4.2 - "Handover Resource Allocation"
 *************************************************************/

bool s1ap::handle_handover_request(const asn1::s1ap::ho_request_s& msg)
{
  uint16_t            rnti           = SRSRAN_INVALID_RNTI;
  uint32_t            mme_ue_s1ap_id = msg->mme_ue_s1ap_id.value.value;
  asn1::s1ap::cause_c cause;
  cause.set_misc().value = cause_misc_opts::unspecified;

  if (msg.ext or msg->ho_restrict_list_present) {
    logger.warning("Not handling S1AP Handover Request extensions or Handover Restriction List");
  }

  if (msg->handov_type.value.value != handov_type_opts::intralte) {
    logger.error("Not handling S1AP non-intra LTE handovers");
    cause.set_radio_network().value = cause_radio_network_opts::interrat_redirection;
    send_ho_failure(mme_ue_s1ap_id, cause);
    return false;
  }

  // Confirm the UE does not exist in TeNB
  if (users.find_ue_mmeid(msg->mme_ue_s1ap_id.value.value) != nullptr) {
    logger.error("The provided MME_UE_S1AP_ID=%" PRIu64 " is already connected to the cell",
                 msg->mme_ue_s1ap_id.value.value);
    cause.set_radio_network().value = cause_radio_network_opts::unknown_mme_ue_s1ap_id;
    send_ho_failure(mme_ue_s1ap_id, cause);
    return false;
  }

  // Create user ctxt object and associated MME context
  std::unique_ptr<ue> ue_ptr{new ue{this}};
  ue_ptr->ctxt.mme_ue_s1ap_id = msg->mme_ue_s1ap_id.value.value;
  srsran_assert(users.add_user(std::move(ue_ptr)) != nullptr, "Unexpected failure to create S1AP UE");

  // Unpack Transparent Container
  sourceenb_to_targetenb_transparent_container_s container;
  asn1::cbit_ref                                 bref{msg->source_to_target_transparent_container.value.data(),
                      msg->source_to_target_transparent_container.value.size()};
  if (container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    logger.warning("Failed to unpack SourceToTargetTransparentContainer");
    cause.set_protocol().value = cause_protocol_opts::transfer_syntax_error;
    send_ho_failure(mme_ue_s1ap_id, cause);
    return false;
  }

  // Handle Handover Resource Allocation
  rnti = rrc->start_ho_ue_resource_alloc(msg, container, cause);
  if (rnti == SRSRAN_INVALID_RNTI) {
    send_ho_failure(mme_ue_s1ap_id, cause);
    return false;
  }
  return true;
}

void s1ap::send_ho_failure(uint32_t mme_ue_s1ap_id, const asn1::s1ap::cause_c& cause)
{
  // Remove created s1ap user
  ue* u = users.find_ue_mmeid(mme_ue_s1ap_id);
  if (u != nullptr) {
    users.erase(u);
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_HO_RES_ALLOC);
  ho_fail_s& container = tx_pdu.unsuccessful_outcome().value.ho_fail();

  container->mme_ue_s1ap_id.value = mme_ue_s1ap_id;
  container->cause.value          = cause;

  sctp_send_s1ap_pdu(tx_pdu, SRSRAN_INVALID_RNTI, "HandoverFailure");
}

bool s1ap::send_ho_req_ack(const asn1::s1ap::ho_request_s&                msg,
                           uint16_t                                       rnti,
                           uint32_t                                       enb_cc_idx,
                           srsran::unique_byte_buffer_t                   ho_cmd,
                           srsran::span<asn1::s1ap::erab_admitted_item_s> admitted_bearers,
                           srsran::const_span<asn1::s1ap::erab_item_s>    not_admitted_bearers)
{
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_HO_RES_ALLOC);
  ho_request_ack_s& container = tx_pdu.successful_outcome().value.ho_request_ack();

  ue* ue_ptr = users.find_ue_mmeid(msg->mme_ue_s1ap_id.value.value);
  if (ue_ptr == nullptr) {
    logger.error("The MME-S1AP-UE-ID=%ld is not valid", msg->mme_ue_s1ap_id.value.value);
    return false;
  }
  ue_ptr->ctxt.rnti       = rnti;
  ue_ptr->ctxt.enb_cc_idx = enb_cc_idx;

  container->mme_ue_s1ap_id.value = msg->mme_ue_s1ap_id.value.value;
  container->enb_ue_s1ap_id.value = ue_ptr->ctxt.enb_ue_s1ap_id;

  // Add admitted E-RABs
  container->erab_admitted_list.value.resize(admitted_bearers.size());
  for (size_t i = 0; i < admitted_bearers.size(); ++i) {
    container->erab_admitted_list.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_ADMITTED_ITEM);
    auto& c = container->erab_admitted_list.value[i]->erab_admitted_item();
    c       = admitted_bearers[i];
    if (!args.gtp_advertise_addr.empty()) {
      c.transport_layer_address = addr_to_asn1(args.gtp_advertise_addr.c_str());
    } else {
      c.transport_layer_address = addr_to_asn1(args.gtp_bind_addr.c_str());
    }

    // If E-RAB is proposed for forward tunneling
    if (c.dl_g_tp_teid_present) {
      c.dl_transport_layer_address_present = true;
      c.dl_transport_layer_address         = c.transport_layer_address;
    }
    if (c.ul_gtp_teid_present) {
      c.ul_transport_layer_address_present = true;
      c.ul_transport_layer_address         = c.transport_layer_address;
    }
  }

  // Add failed to Setup E-RABs
  if (not not_admitted_bearers.empty()) {
    container->erab_failed_to_setup_list_ho_req_ack_present = true;
    container->erab_failed_to_setup_list_ho_req_ack.value.resize(not_admitted_bearers.size());
    for (size_t i = 0; i < not_admitted_bearers.size(); ++i) {
      container->erab_failed_to_setup_list_ho_req_ack.value[i].load_info_obj(
          ASN1_S1AP_ID_ERAB_FAILEDTO_SETUP_ITEM_HO_REQ_ACK);
      auto& erab   = container->erab_failed_to_setup_list_ho_req_ack.value[i]->erab_failedto_setup_item_ho_req_ack();
      erab.erab_id = not_admitted_bearers[i].erab_id;
      erab.cause   = not_admitted_bearers[i].cause;
    }
  }

  // Pack transparent container
  asn1::s1ap::targetenb_to_sourceenb_transparent_container_s transparent_container;
  transparent_container.rrc_container.resize(ho_cmd->N_bytes);
  memcpy(transparent_container.rrc_container.data(), ho_cmd->msg, ho_cmd->N_bytes);

  auto&         pdu = ho_cmd; // reuse pdu
  asn1::bit_ref bref{pdu->msg, pdu->get_tailroom()};
  if (transparent_container.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack TargeteNBToSourceeNBTransparentContainer");
    return false;
  }
  container->target_to_source_transparent_container.value.resize(bref.distance_bytes());
  memcpy(container->target_to_source_transparent_container.value.data(), pdu->msg, bref.distance_bytes());

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "HandoverRequestAcknowledge");
}

bool s1ap::handle_mme_status_transfer(const asn1::s1ap::mme_status_transfer_s& msg)
{
  ue* u = handle_s1apmsg_ue_id(msg->enb_ue_s1ap_id.value.value, msg->mme_ue_s1ap_id.value.value);
  if (u == nullptr) {
    return false;
  }

  rrc->set_erab_status(u->ctxt.rnti,
                       msg->enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list);
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
  ho_notify_s& container = tx_pdu.init_msg().value.ho_notify();

  container->mme_ue_s1ap_id.value = user_ptr->ctxt.mme_ue_s1ap_id.value();
  container->enb_ue_s1ap_id.value = user_ptr->ctxt.enb_ue_s1ap_id;

  container->eutran_cgi.value = eutran_cgi;
  container->eutran_cgi.value.cell_id.from_number(target_eci);
  container->tai.value = tai;

  sctp_send_s1ap_pdu(tx_pdu, rnti, "HandoverNotify");
}

void s1ap::send_ho_cancel(uint16_t rnti, const asn1::s1ap::cause_c& cause)
{
  ue* user_ptr = users.find_ue_rnti(rnti);
  if (user_ptr == nullptr) {
    logger.warning("Canceling handover for non-existent rnti=0x%x", rnti);
    return;
  }

  user_ptr->send_ho_cancel(cause);
}

bool s1ap::release_erabs(uint16_t rnti, const std::vector<uint16_t>& erabs_successfully_released)
{
  ue* user_ptr = users.find_ue_rnti(rnti);
  if (user_ptr == nullptr) {
    return false;
  }
  return user_ptr->send_erab_release_indication(erabs_successfully_released);
}

bool s1ap::send_ue_cap_info_indication(uint16_t rnti, srsran::unique_byte_buffer_t ue_radio_cap)
{
  ue* user_ptr = users.find_ue_rnti(rnti);
  if (user_ptr == nullptr) {
    return false;
  }
  return user_ptr->send_ue_cap_info_indication(std::move(ue_radio_cap));
}

bool s1ap::send_error_indication(const asn1::s1ap::cause_c& cause,
                                 srsran::optional<uint32_t> enb_ue_s1ap_id,
                                 srsran::optional<uint32_t> mme_ue_s1ap_id)
{
  if (not mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ERROR_IND);
  auto& container = tx_pdu.init_msg().value.error_ind();

  uint16_t rnti                     = SRSRAN_INVALID_RNTI;
  container->enb_ue_s1ap_id_present = enb_ue_s1ap_id.has_value();
  if (enb_ue_s1ap_id.has_value()) {
    container->enb_ue_s1ap_id.value = enb_ue_s1ap_id.value();
    ue* user_ptr                    = users.find_ue_enbid(enb_ue_s1ap_id.value());
    rnti                            = user_ptr != nullptr ? user_ptr->ctxt.rnti : SRSRAN_INVALID_RNTI;
  }
  container->mme_ue_s1ap_id_present = mme_ue_s1ap_id.has_value();
  if (mme_ue_s1ap_id.has_value()) {
    container->mme_ue_s1ap_id.value = mme_ue_s1ap_id.value();
  }

  container->s_tmsi_present = false;

  container->cause_present = true;
  container->cause.value   = cause;

  return sctp_send_s1ap_pdu(tx_pdu, rnti, "Error Indication");
}

/*******************************************************************************
/* S1AP message senders
********************************************************************************/

bool s1ap::ue::send_initialuemessage(asn1::s1ap::rrc_establishment_cause_e cause,
                                     srsran::unique_byte_buffer_t          pdu,
                                     bool                                  has_tmsi,
                                     uint32_t                              m_tmsi,
                                     uint8_t                               mmec)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_INIT_UE_MSG);
  init_ue_msg_s& container = tx_pdu.init_msg().value.init_ue_msg();

  // S_TMSI
  if (has_tmsi) {
    container->s_tmsi_present = true;
    uint32_to_uint8(m_tmsi, container->s_tmsi.value.m_tmsi.data());
    container->s_tmsi.value.mmec[0] = mmec;
  }

  // ENB_UE_S1AP_ID
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // NAS_PDU
  container->nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container->nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // TAI
  container->tai.value = s1ap_ptr->tai;

  // EUTRAN_CGI
  container->eutran_cgi.value = s1ap_ptr->eutran_cgi;

  // RRC Establishment Cause
  container->rrc_establishment_cause.value = cause;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "InitialUEMessage");
}

bool s1ap::ue::send_ulnastransport(srsran::unique_byte_buffer_t pdu)
{
  if (not ctxt.mme_ue_s1ap_id.has_value()) {
    logger.error("Trying to send UL NAS Transport message for rnti=0x%x without MME-S1AP-UE-ID", ctxt.rnti);
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UL_NAS_TRANSPORT);
  ul_nas_transport_s& container   = tx_pdu.init_msg().value.ul_nas_transport();
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // NAS PDU
  container->nas_pdu.value.resize(pdu->N_bytes);
  memcpy(container->nas_pdu.value.data(), pdu->msg, pdu->N_bytes);

  // EUTRAN CGI
  container->eutran_cgi.value = s1ap_ptr->eutran_cgi;

  // TAI
  container->tai.value = s1ap_ptr->tai;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UplinkNASTransport");
}

bool s1ap::ue::send_uectxtreleaserequest(const cause_c& cause)
{
  if (not ctxt.mme_ue_s1ap_id.has_value()) {
    logger.error("Cannot send UE context release request without a MME-UE-S1AP-Id allocated.");
    s1ap_ptr->rrc->release_ue(ctxt.rnti);
    s1ap_ptr->users.erase(this);
    return false;
  }

  if (ts1_reloc_overall.is_running() and cause.type().value == asn1::s1ap::cause_c::types_opts::radio_network and
      (cause.radio_network().value == asn1::s1ap::cause_radio_network_opts::user_inactivity or
       cause.radio_network().value == asn1::s1ap::cause_radio_network_opts::radio_conn_with_ue_lost)) {
    logger.info("Ignoring UE context release request from lower layers for UE rnti=0x%x performing S1 Handover.",
                ctxt.rnti);
    // Leave the UE context alive during S1 Handover until ts1_reloc_overall expiry. Ignore releases due to
    // UE inactivity or RLF
    return false;
  }

  if (was_uectxtrelease_requested()) {
    // let timeout auto-remove user.
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE_REQUEST);
  ue_context_release_request_s& container = tx_pdu.init_msg().value.ue_context_release_request();
  container->mme_ue_s1ap_id.value         = ctxt.mme_ue_s1ap_id.value();
  container->enb_ue_s1ap_id.value         = ctxt.enb_ue_s1ap_id;

  // Cause
  container->cause.value = cause;

  release_requested = s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseRequest");
  if (not release_requested) {
    s1ap_ptr->rrc->release_ue(ctxt.rnti);
    s1ap_ptr->users.erase(this);
  } else {
    overall_procedure_timeout.set(10000, [this](uint32_t tid) {
      logger.warning("UE context for RNTI:0x%x is in zombie state. Releasing...", ctxt.rnti);
      s1ap_ptr->rrc->release_ue(ctxt.rnti);
      s1ap_ptr->users.erase(this);
    });
    overall_procedure_timeout.run();
  }
  return release_requested;
}

bool s1ap::ue::send_uectxtreleasecomplete()
{
  overall_procedure_timeout.stop();

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_RELEASE);
  auto& container                 = tx_pdu.successful_outcome().value.ue_context_release_complete();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  // Stop TS1 Reloc Overall
  ts1_reloc_overall.stop();

  // Log event.
  event_logger::get().log_s1_ctx_delete(ctxt.enb_cc_idx, ctxt.mme_ue_s1ap_id.value(), ctxt.enb_ue_s1ap_id, ctxt.rnti);

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextReleaseComplete");
}

void s1ap::ue::notify_rrc_reconf_complete()
{
  if (current_state == s1ap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request) {
    logger.info("Procedure %s,rnti=0x%x - Received RRC reconf complete. Finishing UE context setup.",
                s1ap_elem_procs_o::init_msg_c::types_opts{current_state}.to_string(),
                ctxt.rnti);
    ue_ctxt_setup_complete();
    return;
  }
}

void s1ap::ue::ue_ctxt_setup_complete()
{
  if (current_state != s1ap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request) {
    logger.warning("Procedure %s,rnti=0x%x - Received unexpected complete notification",
                   s1ap_elem_procs_o::init_msg_c::types_opts{current_state}.to_string(),
                   ctxt.rnti);
    return;
  }
  current_state = s1ap_elem_procs_o::init_msg_c::types_opts::nulltype;

  s1ap_pdu_c tx_pdu;
  if (updated_erabs.empty()) {
    // It is ICS Failure
    tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
    auto& container = tx_pdu.unsuccessful_outcome().value.init_context_setup_fail();

    container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
    container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
    if (not failed_cfg_erabs.empty()) {
      container->cause.value = failed_cfg_erabs.front().cause;
    } else {
      logger.warning("Procedure %s,rnti=0x%x - no specified cause for failed configuration",
                     s1ap_elem_procs_o::init_msg_c::types_opts{current_state}.to_string(),
                     ctxt.rnti);
      container->cause.value.set_misc().value = cause_misc_opts::unspecified;
    }
    s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextModificationFailure");
    return;
  }

  // It is ICS Response
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
  auto& container = tx_pdu.successful_outcome().value.init_context_setup_resp();

  // Fill in the MME and eNB IDs
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // Add list of E-RABs that were not setup
  if (not failed_cfg_erabs.empty()) {
    container->erab_failed_to_setup_list_ctxt_su_res_present = true;
    fill_erab_failed_setup_list(container->erab_failed_to_setup_list_ctxt_su_res.value, failed_cfg_erabs);
  }

  // Add setup E-RABs
  container->erab_setup_list_ctxt_su_res.value.resize(updated_erabs.size());
  for (size_t i = 0; i < updated_erabs.size(); ++i) {
    container->erab_setup_list_ctxt_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_CTXT_SU_RES);
    auto& item   = container->erab_setup_list_ctxt_su_res.value[i]->erab_setup_item_ctxt_su_res();
    item.erab_id = updated_erabs[i];
    get_erab_addr(item.erab_id, item.transport_layer_address, item.gtp_teid);
  }

  // Log event.
  event_logger::get().log_s1_ctx_create(ctxt.enb_cc_idx, ctxt.mme_ue_s1ap_id.value(), ctxt.enb_ue_s1ap_id, ctxt.rnti);

  s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "InitialContextSetupResponse");
}

bool s1ap::ue::send_erab_setup_response(const erab_id_list& erabs_setup, const erab_item_list& erabs_failed)
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_ERAB_SETUP);
  erab_setup_resp_s& res = tx_pdu.successful_outcome().value.erab_setup_resp();

  // Fill in the MME and eNB IDs
  res->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
  res->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;

  // Add list of E-RABs that were not setup
  if (not erabs_failed.empty()) {
    res->erab_failed_to_setup_list_bearer_su_res_present = true;
    fill_erab_failed_setup_list(res->erab_failed_to_setup_list_bearer_su_res.value, erabs_failed);
  }

  if (not erabs_setup.empty()) {
    res->erab_setup_list_bearer_su_res_present = true;
    res->erab_setup_list_bearer_su_res.value.resize(erabs_setup.size());
    for (size_t i = 0; i < erabs_setup.size(); ++i) {
      res->erab_setup_list_bearer_su_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_SETUP_ITEM_BEARER_SU_RES);
      auto& item   = res->erab_setup_list_bearer_su_res.value[i]->erab_setup_item_bearer_su_res();
      item.erab_id = erabs_setup[i];
      get_erab_addr(item.erab_id, item.transport_layer_address, item.gtp_teid);
    }
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "E_RABSetupResponse");
}

bool s1ap::ue::send_uectxtmodifyresp()
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.successful_outcome().value.ue_context_mod_resp();

  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextModificationResponse");
}

bool s1ap::ue::send_uectxtmodifyfailure(const cause_c& cause)
{
  if (not s1ap_ptr->mme_connected) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_UE_CONTEXT_MOD);
  auto& container = tx_pdu.unsuccessful_outcome().value.ue_context_mod_fail();

  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
  container->cause.value          = cause;

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UEContextModificationFailure");
}

/**
 * @brief eNB sends MME to "E-RAB RELEASE RESPONSE"
 *        @remark TS 36.413, Section 8.2.3.2 - E-RAB Release - MME initiated (successful operation)
 * @param erabs_successfully_released
 * @param erabs_failed_to_release
 * @return true if message was sent
 */
bool s1ap::ue::send_erab_release_response(const erab_id_list& erabs_released, const erab_item_list& erabs_failed)
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_ERAB_RELEASE);

  auto& container                 = tx_pdu.successful_outcome().value.erab_release_resp();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  // Fill in which E-RABs were successfully released
  if (not erabs_released.empty()) {
    container->erab_release_list_bearer_rel_comp_present = true;
    container->erab_release_list_bearer_rel_comp.value.resize(erabs_released.size());
    for (size_t i = 0; i < erabs_released.size(); ++i) {
      container->erab_release_list_bearer_rel_comp.value[i].load_info_obj(
          ASN1_S1AP_ID_ERAB_RELEASE_ITEM_BEARER_REL_COMP);
      container->erab_release_list_bearer_rel_comp.value[i]->erab_release_item_bearer_rel_comp().erab_id =
          erabs_released[i];
    }
  }

  // Fill in which E-RABs were *not* successfully released
  if (not erabs_failed.empty()) {
    container->erab_failed_to_release_list_present = true;
    fill_erab_failed_setup_list(container->erab_failed_to_release_list.value, erabs_failed);
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "E-RABReleaseResponse");
}

bool s1ap::ue::send_erab_modify_response(const erab_id_list& erabs_modified, const erab_item_list& erabs_failed)
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_ERAB_MODIFY);

  auto& container                 = tx_pdu.successful_outcome().value.erab_modify_resp();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  // Fill in which E-RABs were successfully released
  if (not erabs_modified.empty()) {
    container->erab_modify_list_bearer_mod_res_present = true;
    container->erab_modify_list_bearer_mod_res.value.resize(erabs_modified.size());
    for (uint32_t i = 0; i < container->erab_modify_list_bearer_mod_res.value.size(); i++) {
      container->erab_modify_list_bearer_mod_res.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_MODIFY_ITEM_BEARER_MOD_RES);
      container->erab_modify_list_bearer_mod_res.value[i]->erab_modify_item_bearer_mod_res().erab_id =
          erabs_modified[i];
    }
  }

  // Fill in which E-RABs were *not* successfully released
  if (not erabs_failed.empty()) {
    container->erab_failed_to_modify_list_present = true;
    fill_erab_failed_setup_list(container->erab_failed_to_modify_list.value, erabs_failed);
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "E-RABModifyResponse");
}

bool s1ap::ue::send_erab_release_indication(const std::vector<uint16_t>& erabs_successfully_released)
{
  if (not erabs_successfully_released.empty()) {
    logger.error("Failed to initiate E-RAB RELEASE INDICATION procedure for user rnti=0x%x", ctxt.rnti);
    return false;
  }

  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ERAB_RELEASE_IND);
  erab_release_ind_s& container = tx_pdu.init_msg().value.erab_release_ind();

  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  // Fill in which E-RABs were successfully released
  container->erab_released_list.value.resize(erabs_successfully_released.size());
  for (size_t i = 0; i < container->erab_released_list.value.size(); ++i) {
    container->erab_released_list.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);
    container->erab_released_list.value[i]->erab_item().erab_id = erabs_successfully_released[i];
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "E-RABReleaseIndication");
}

bool s1ap::ue::send_ue_cap_info_indication(srsran::unique_byte_buffer_t ue_radio_cap)
{
  asn1::s1ap::s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_UE_CAP_INFO_IND);
  ue_cap_info_ind_s& container = tx_pdu.init_msg().value.ue_cap_info_ind();

  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  container->ue_radio_cap.value.resize(ue_radio_cap->N_bytes);
  memcpy(container->ue_radio_cap.value.data(), ue_radio_cap->msg, ue_radio_cap->N_bytes);

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "UECapabilityInfoIndication");
}

void s1ap::ue::send_ho_cancel(const asn1::s1ap::cause_c& cause)
{
  // Stop handover timers
  ts1_reloc_prep.stop();
  ts1_reloc_overall.stop();

  // Send S1AP Handover Cancel
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_HO_CANCEL);
  ho_cancel_s& container = tx_pdu.init_msg().value.ho_cancel();

  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();
  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->cause.value          = cause;

  s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "HandoverCancel");
}

void s1ap::ue::set_state(s1ap_proc_id_t        next_state,
                         const erab_id_list&   erabs_updated,
                         const erab_item_list& erabs_failed_to_modify)
{
  current_state = next_state;
  updated_erabs.assign(erabs_updated.begin(), erabs_updated.end());
  failed_cfg_erabs.assign(erabs_failed_to_modify.begin(), erabs_failed_to_modify.end());
}

void s1ap::ue::get_erab_addr(uint16_t erab_id, transp_addr_t& transp_addr, asn1::fixed_octstring<4, true>& gtpu_teid_id)
{
  uint32_t teidin = 0;
  int      ret    = s1ap_ptr->rrc->get_erab_addr_in(ctxt.rnti, erab_id, transp_addr, teidin);
  srsran_expect(ret == SRSRAN_SUCCESS, "Invalid E-RAB setup");
  // Note: RRC does not yet update correctly gtpu transp_addr
  transp_addr.resize(32);
  uint8_t addr[4];
  if (!s1ap_ptr->args.gtp_advertise_addr.empty()) {
    if (inet_pton(AF_INET, s1ap_ptr->args.gtp_advertise_addr.c_str(), addr) != 1) {
      logger.error("Invalid gtp_advertise_addr: %s", s1ap_ptr->args.gtp_advertise_addr.c_str());
      srsran::console("Invalid gtp_advertise_addr: %s\n", s1ap_ptr->args.gtp_advertise_addr.c_str());
      perror("inet_pton");
    }
  } else {
    if (inet_pton(AF_INET, s1ap_ptr->args.gtp_bind_addr.c_str(), addr) != 1) {
      logger.error("Invalid gtp_bind_addr: %s", s1ap_ptr->args.gtp_bind_addr.c_str());
      srsran::console("Invalid gtp_bind_addr: %s\n", s1ap_ptr->args.gtp_bind_addr.c_str());
      perror("inet_pton");
    }
  }
  for (uint32_t j = 0; j < 4; ++j) {
    transp_addr.data()[j] = addr[3 - j];
  }
  gtpu_teid_id.from_number(teidin);
}

/*********************
 * Handover Messages
 ********************/

bool s1ap::send_ho_required(uint16_t                     rnti,
                            uint32_t                     target_eci,
                            uint16_t                     target_tac,
                            srsran::plmn_id_t            target_plmn,
                            srsran::span<uint32_t>       fwd_erabs,
                            srsran::unique_byte_buffer_t rrc_container,
                            bool                         has_direct_fwd_path)
{
  if (!mme_connected) {
    return false;
  }
  ue* u = users.find_ue_rnti(rnti);
  if (u == nullptr) {
    return false;
  }

  // launch procedure
  if (not u->ho_prep_proc.launch(
          target_eci, target_tac, target_plmn, fwd_erabs, std::move(rrc_container), has_direct_fwd_path)) {
    logger.error("Failed to initiate an HandoverPreparation procedure for user rnti=0x%x", u->ctxt.rnti);
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

/*********************************************************
 *              s1ap::user_list class
 *********************************************************/

s1ap::ue* s1ap::user_list::find_ue_rnti(uint16_t rnti)
{
  if (rnti == SRSRAN_INVALID_RNTI) {
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
    return v.second->ctxt.mme_ue_s1ap_id == mmeid;
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
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("S1AP");
  // Check for ID repetitions
  if (find_ue_rnti(user->ctxt.rnti) != nullptr) {
    logger.error("The user to be added with rnti=0x%x already exists", user->ctxt.rnti);
    return nullptr;
  }
  if (find_ue_enbid(user->ctxt.enb_ue_s1ap_id) != nullptr) {
    logger.error("The user to be added with enb id=%d already exists", user->ctxt.enb_ue_s1ap_id);
    return nullptr;
  }
  if (user->ctxt.mme_ue_s1ap_id.has_value() and find_ue_mmeid(user->ctxt.mme_ue_s1ap_id.value()) != nullptr) {
    logger.error("The user to be added with mme id=%d already exists", user->ctxt.mme_ue_s1ap_id.value());
    return nullptr;
  }
  auto p = users.insert(std::make_pair(user->ctxt.enb_ue_s1ap_id, std::move(user)));
  return p.second ? p.first->second.get() : nullptr;
}

void s1ap::user_list::erase(ue* ue_ptr)
{
  static srslog::basic_logger& logger = srslog::fetch_basic_logger("S1AP");
  auto                         it     = users.find(ue_ptr->ctxt.enb_ue_s1ap_id);
  if (it == users.end()) {
    logger.error("User to be erased does not exist");
    return;
  }
  users.erase(it);
}

/*******************************************************************************
/* General helpers
********************************************************************************/
bool s1ap::sctp_send_s1ap_pdu(const asn1::s1ap::s1ap_pdu_c& tx_pdu, uint32_t rnti, const char* procedure_name)
{
  if (not mme_connected and rnti != SRSRAN_INVALID_RNTI) {
    logger.error("Aborting %s for rnti=0x%x. Cause: MME is not connected.", procedure_name, rnti);
    return false;
  }

  // Reset the state if it is a successful or unsuccessfull message
  if (tx_pdu.type() == s1ap_pdu_c::types_opts::successful_outcome ||
      tx_pdu.type() == s1ap_pdu_c::types_opts::unsuccessful_outcome) {
    if (rnti != SRSRAN_INVALID_RNTI) {
      s1ap::ue* u = users.find_ue_rnti(rnti);
      if (u == nullptr) {
        logger.warning("Could not find user for %s. RNTI=%x", procedure_name, rnti);
      } else {
        u->set_state(s1ap_proc_id_t::nulltype, {}, {});
      }
    }
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

  // Save message to PCAP
  if (pcap != nullptr) {
    pcap->write_s1ap(buf->msg, buf->N_bytes);
  }

  if (rnti != SRSRAN_INVALID_RNTI) {
    logger.info(buf->msg, buf->N_bytes, "Tx S1AP SDU, %s, rnti=0x%x", procedure_name, rnti);
  } else {
    logger.info(buf->msg, buf->N_bytes, "Tx S1AP SDU, %s", procedure_name);
  }
  uint16_t streamid = rnti == SRSRAN_INVALID_RNTI ? NONUE_STREAM_ID : users.find_ue_rnti(rnti)->stream_id;

  ssize_t n_sent = sctp_sendmsg(mme_socket.fd(),
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
    if (rnti != SRSRAN_INVALID_RNTI) {
      logger.error("Error: Failure at Tx S1AP SDU, %s, rnti=0x%x", procedure_name, rnti);
    } else {
      logger.error("Error: Failure at Tx S1AP SDU, %s", procedure_name);
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
s1ap::ue* s1ap::handle_s1apmsg_ue_id(uint32_t enb_id, uint32_t mme_id)
{
  ue*     user_ptr     = users.find_ue_enbid(enb_id);
  ue*     user_mme_ptr = nullptr;
  cause_c cause;

  logger.info("Checking UE S1 logical connection. eNB UE S1AP ID=%d, MME UE S1AP ID=%d", enb_id, mme_id);

  if (user_ptr != nullptr) {
    if (user_ptr->ctxt.mme_ue_s1ap_id == mme_id) {
      // No ID inconsistency found
      return user_ptr;
    }

    user_mme_ptr = users.find_ue_mmeid(mme_id);
    if (not user_ptr->ctxt.mme_ue_s1ap_id.has_value() and user_mme_ptr == nullptr) {
      // First "returned message", no inconsistency found (see 36.413, Section 10.6)
      user_ptr->ctxt.mme_ue_s1ap_id = mme_id;
      return user_ptr;
    }

    // TS 36.413, Sec. 10.6 - If a node receives a first returned message that includes a remote AP ID (...)

    logger.warning("MME UE S1AP ID=%d not found - discarding message", mme_id);
    cause.set_radio_network().value = user_mme_ptr != nullptr ? cause_radio_network_opts::unknown_mme_ue_s1ap_id
                                                              : cause_radio_network_opts::unknown_pair_ue_s1ap_id;
  } else {
    // TS 36.413, Sec. 10.6 - If a node receives a message (other than the first or first returned messages) that
    // includes AP ID(s) identifying (...)
    user_mme_ptr = users.find_ue_mmeid(mme_id);

    logger.warning("ENB UE S1AP ID=%d not found - discarding message", enb_id);
    cause.set_radio_network().value = user_mme_ptr != nullptr ? cause_radio_network_opts::unknown_enb_ue_s1ap_id
                                                              : cause_radio_network_opts::unknown_pair_ue_s1ap_id;
  }

  // the node shall initiate an Error Indication procedure with inclusion of the received AP ID(s) from the peer node
  // and an appropriate cause value.
  send_error_indication(cause, enb_id, mme_id);

  // Both nodes shall initiate a local release of any established UE-associated logical connection (for the same S1
  // interface) having the erroneous AP ID(s) as local or remote identifier.
  if (user_ptr != nullptr) {
    rrc->release_ue(user_ptr->ctxt.rnti);
  }
  if (user_mme_ptr != nullptr and user_mme_ptr != user_ptr) {
    rrc->release_ue(user_mme_ptr->ctxt.rnti);
  }
  return nullptr;
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

void s1ap::start_pcap(srsran::s1ap_pcap* pcap_)
{
  pcap = pcap_;
}
/*******************************************************************************
/*               s1ap::ue Class
********************************************************************************/

s1ap::ue::ue(s1ap* s1ap_ptr_) : s1ap_ptr(s1ap_ptr_), ho_prep_proc(this), logger(s1ap_ptr->logger)
{
  ctxt.enb_ue_s1ap_id = s1ap_ptr->next_enb_ue_s1ap_id++;
  gettimeofday(&ctxt.init_timestamp, nullptr);

  stream_id = s1ap_ptr->next_ue_stream_id;

  // initialize timers
  ts1_reloc_prep = s1ap_ptr->task_sched.get_unique_timer();
  ts1_reloc_prep.set(s1ap_ptr->args.ts1_reloc_prep_timeout,
                     [this](uint32_t tid) { ho_prep_proc.trigger(ho_prep_proc_t::ts1_reloc_prep_expired{}); });
  ts1_reloc_overall = s1ap_ptr->task_sched.get_unique_timer();
  ts1_reloc_overall.set(s1ap_ptr->args.ts1_reloc_overall_timeout, [this](uint32_t tid) {
    //> If the UE Context Release procedure is not initiated towards the eNB before the expiry of the timer
    //  TS1RELOCOverall, the eNB shall request the MME to release the UE context.
    s1ap_ptr->user_release(ctxt.rnti, asn1::s1ap::cause_radio_network_opts::ts1relocoverall_expiry);
  });
  overall_procedure_timeout = s1ap_ptr->task_sched.get_unique_timer();
  overall_procedure_timeout.set(10000);
}

bool s1ap::ue::send_ho_required(uint32_t                     target_eci,
                                uint16_t                     target_tac,
                                srsran::plmn_id_t            target_plmn,
                                srsran::span<uint32_t>       fwd_erabs,
                                srsran::unique_byte_buffer_t rrc_container,
                                bool                         has_direct_fwd_path)
{
  /*** Setup S1AP PDU as HandoverRequired ***/
  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_HO_PREP);
  ho_required_s& container = tx_pdu.init_msg().value.ho_required();

  /*** fill HO Required message ***/
  container->enb_ue_s1ap_id.value                  = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value                  = ctxt.mme_ue_s1ap_id.value();
  container->handov_type.value.value               = handov_type_opts::intralte; // NOTE: only intra-LTE HO supported
  container->cause.value.set_radio_network().value = cause_radio_network_opts::ho_desirable_for_radio_reason;

  container->direct_forwarding_path_availability_present = has_direct_fwd_path;
  if (container->direct_forwarding_path_availability_present) {
    container->direct_forwarding_path_availability.value.value =
        asn1::s1ap::direct_forwarding_path_availability_opts::direct_path_available;
  }

  /*** set the target eNB ***/
  container->csg_id_present           = false; // NOTE: CSG/hybrid target cell not supported
  container->cell_access_mode_present = false; // only for hybrid cells
  // no GERAN/UTRAN/PS
  auto& targetenb = container->target_id.value.set_targetenb_id();
  // set PLMN and TAI of target
  // NOTE: Only HO without TAU supported.
  uint16_t tmp16;
  tmp16 = htons(target_tac);
  memcpy(targetenb.sel_tai.tac.data(), &tmp16, sizeof(uint16_t));
  target_plmn.to_s1ap_plmn_bytes(targetenb.sel_tai.plm_nid.data());
  // NOTE: Only HO to different Macro eNB is supported.
  auto& macroenb = targetenb.global_enb_id.enb_id.set_macro_enb_id();
  target_plmn.to_s1ap_plmn_bytes(targetenb.global_enb_id.plm_nid.data());
  macroenb.from_number(target_eci >> 8U);

  /*** fill the transparent container ***/
  container->source_to_target_transparent_container_secondary_present = false;
  sourceenb_to_targetenb_transparent_container_s transparent_cntr;
  transparent_cntr.subscriber_profile_idfor_rfp_present = false; // TODO: CHECK

  transparent_cntr.erab_info_list_present = true;
  transparent_cntr.erab_info_list.resize(fwd_erabs.size());
  for (uint32_t i = 0; i < fwd_erabs.size(); ++i) {
    transparent_cntr.erab_info_list[i].load_info_obj(ASN1_S1AP_ID_ERAB_INFO_LIST_ITEM);
    transparent_cntr.erab_info_list[i]->erab_info_list_item().erab_id               = fwd_erabs[i];
    transparent_cntr.erab_info_list[i]->erab_info_list_item().dl_forwarding_present = true;
    transparent_cntr.erab_info_list[i]->erab_info_list_item().dl_forwarding.value =
        dl_forwarding_opts::dl_forwarding_proposed;
  }
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

  // pack Transparent Container into HORequired message
  srsran::unique_byte_buffer_t buffer = srsran::make_byte_buffer();
  if (buffer == nullptr) {
    logger.error("Failed to allocate buffer for HORequired message packing");
    return false;
  }
  asn1::bit_ref bref(buffer->msg, buffer->get_tailroom());
  if (transparent_cntr.pack(bref) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack transparent container of HO Required message");
    return false;
  }
  container->source_to_target_transparent_container.value.resize(bref.distance_bytes());
  memcpy(container->source_to_target_transparent_container.value.data(), buffer->msg, bref.distance_bytes());

  // Send to HandoverRequired message to MME
  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "Handover Required");
}

bool s1ap::ue::send_enb_status_transfer_proc(std::vector<bearer_status_info>& bearer_status_list)
{
  if (bearer_status_list.empty()) {
    return false;
  }

  s1ap_pdu_c tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ENB_STATUS_TRANSFER);
  enb_status_transfer_s& container = tx_pdu.init_msg().value.enb_status_transfer();

  container->enb_ue_s1ap_id.value = ctxt.enb_ue_s1ap_id;
  container->mme_ue_s1ap_id.value = ctxt.mme_ue_s1ap_id.value();

  /* Create StatusTransfer transparent container with all the bearer ctxt to transfer */
  auto& list = container->enb_status_transfer_transparent_container.value.bearers_subject_to_status_transfer_list;
  list.resize(bearer_status_list.size());
  for (uint32_t i = 0; i < list.size(); ++i) {
    list[i].load_info_obj(ASN1_S1AP_ID_BEARERS_SUBJECT_TO_STATUS_TRANSFER_ITEM);
    auto&               asn1bearer = list[i]->bearers_subject_to_status_transfer_item();
    bearer_status_info& item       = bearer_status_list[i];

    asn1bearer.erab_id                = item.erab_id;
    asn1bearer.dl_coun_tvalue.pdcp_sn = item.pdcp_dl_sn;
    asn1bearer.dl_coun_tvalue.hfn     = item.dl_hfn;
    asn1bearer.ul_coun_tvalue.pdcp_sn = item.pdcp_ul_sn;
    asn1bearer.ul_coun_tvalue.hfn     = item.ul_hfn;
    // TODO: asn1bearer.receiveStatusofULPDCPSDUs_present

    //    asn1::json_writer jw;
    //    asn1bearer.to_json(jw);
    //    printf("Bearer to add %s", jw.to_string().c_str());
  }

  return s1ap_ptr->sctp_send_s1ap_pdu(tx_pdu, ctxt.rnti, "ENBStatusTransfer");
}

void s1ap::log_s1ap_msg(const asn1::s1ap::s1ap_pdu_c& msg, srsran::const_span<uint8_t> sdu, bool is_rx)
{
  const char* msg_type;

  switch (msg.type().value) {
    case s1ap_pdu_c::types_opts::init_msg:
      msg_type = msg.init_msg().value.type().to_string();
      break;
    case s1ap_pdu_c::types_opts::successful_outcome:
      msg_type = msg.successful_outcome().value.type().to_string();
      break;
    case s1ap_pdu_c::types_opts::unsuccessful_outcome:
      msg_type = msg.unsuccessful_outcome().value.type().to_string();
      break;
    default:
      logger.warning("Unrecognized S1AP message type\n");
      return;
  }

  logger.info(sdu.data(), sdu.size(), "%s S1AP SDU - %s", is_rx ? "Rx" : "Tx", msg_type);
}

} // namespace srsenb
