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

#include "srsenb/hdr/stack/ngap/ngap_ue_proc.h"

using namespace srsran;

namespace srsenb {

ngap_ue_initial_context_setup_proc::ngap_ue_initial_context_setup_proc(ngap_interface_ngap_proc* parent_,
                                                                       rrc_interface_ngap_nr*    rrc_,
                                                                       ngap_ue_ctxt_t*           ue_ctxt_,
                                                                       srslog::basic_logger&     logger_) :
  logger(logger_), parent(parent_), rrc(rrc_), ue_ctxt(ue_ctxt_){};

proc_outcome_t ngap_ue_initial_context_setup_proc::init(const asn1::ngap_nr::init_context_setup_request_s& msg)
{
  ue_ctxt->amf_pointer   = msg.protocol_ies.guami.value.amf_pointer.to_number();
  ue_ctxt->amf_set_id    = msg.protocol_ies.guami.value.amf_set_id.to_number();
  ue_ctxt->amf_region_id = msg.protocol_ies.guami.value.amf_region_id.to_number();

  if (msg.protocol_ies.ue_aggregate_maximum_bit_rate_present == true) {
    rrc->ue_set_bitrates(ue_ctxt->rnti, msg.protocol_ies.ue_aggregate_maximum_bit_rate.value);
  }
  rrc->ue_set_security_cfg_capabilities(ue_ctxt->rnti, msg.protocol_ies.ue_security_cap.value);
  rrc->ue_set_security_cfg_key(ue_ctxt->rnti, msg.protocol_ies.security_key.value);
  rrc->start_security_mode_procedure(ue_ctxt->rnti);

  if (msg.protocol_ies.nas_pdu_present) {
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      logger.error("Fatal Error: Couldn't allocate buffer in ngap_ue_initial_context_setup_proc::init().");
      return proc_outcome_t::error;
    }
    memcpy(pdu->msg, msg.protocol_ies.nas_pdu.value.data(), msg.protocol_ies.nas_pdu.value.size());
    pdu->N_bytes = msg.protocol_ies.nas_pdu.value.size();
    rrc->write_dl_info(ue_ctxt->rnti, std::move(pdu));
  }
  return proc_outcome_t::yield;
};

proc_outcome_t ngap_ue_initial_context_setup_proc::react(bool rrc_reconf_outcome)
{
  if (rrc_reconf_outcome == true) {
    parent->send_initial_ctxt_setup_response();
    return proc_outcome_t::success;
  }

  return proc_outcome_t::error;
}

proc_outcome_t ngap_ue_initial_context_setup_proc::step()
{
  return proc_outcome_t::yield;
}

ngap_ue_ue_context_release_proc::ngap_ue_ue_context_release_proc(ngap_interface_ngap_proc* parent_,
                                                                 rrc_interface_ngap_nr*    rrc_,
                                                                 ngap_ue_ctxt_t*           ue_ctxt_,
                                                                 ngap_ue_bearer_manager*   bearer_manager_,
                                                                 srslog::basic_logger&     logger_) :
  logger(logger_)
{
  parent         = parent_;
  rrc            = rrc_;
  ue_ctxt        = ue_ctxt_;
  bearer_manager = bearer_manager_;
};

proc_outcome_t ngap_ue_ue_context_release_proc::init(const asn1::ngap_nr::ue_context_release_cmd_s& msg)
{
  logger.info("Started %s", name());
  // TODO: How to approach erasing users ?
  bearer_manager->reset_pdu_sessions(ue_ctxt->rnti);
  rrc->release_bearers(ue_ctxt->rnti);
  parent->send_ue_ctxt_release_complete();
  return proc_outcome_t::success;
}

proc_outcome_t ngap_ue_ue_context_release_proc::step()
{
  return proc_outcome_t::success;
}

ngap_ue_pdu_session_res_setup_proc::ngap_ue_pdu_session_res_setup_proc(ngap_interface_ngap_proc* parent_,
                                                                       rrc_interface_ngap_nr*    rrc_,
                                                                       ngap_ue_ctxt_t*           ue_ctxt_,
                                                                       ngap_ue_bearer_manager*   bearer_manager_,
                                                                       srslog::basic_logger&     logger_) :
  parent(parent_), rrc(rrc_), ue_ctxt(ue_ctxt_), bearer_manager(bearer_manager_), logger(logger_)
{}

proc_outcome_t ngap_ue_pdu_session_res_setup_proc::init(const asn1::ngap_nr::pdu_session_res_setup_request_s& msg)
{
  if (msg.protocol_ies.pdu_session_res_setup_list_su_req.value.size() != 1) {
    logger.error("Not handling zero or multiple su requests");
    return proc_outcome_t::error;
  }

  asn1::ngap_nr::pdu_session_res_setup_item_su_req_s su_req =
      msg.protocol_ies.pdu_session_res_setup_list_su_req.value[0];

  asn1::cbit_ref pdu_session_bref(su_req.pdu_session_res_setup_request_transfer.data(),
                                  su_req.pdu_session_res_setup_request_transfer.size());

  asn1::ngap_nr::pdu_session_res_setup_request_transfer_s pdu_ses_res_setup_req_trans;

  if (pdu_ses_res_setup_req_trans.unpack(pdu_session_bref) != SRSRAN_SUCCESS) {
    logger.error("Unable to unpack PDU session response setup request");
    return proc_outcome_t::error;
  }

  if (pdu_ses_res_setup_req_trans.protocol_ies.qos_flow_setup_request_list.value.size() != 1) {
    logger.error("Expected one item in QoS flow setup request list");
    return proc_outcome_t::error;
  }

  if (pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.type() !=
      asn1::ngap_nr::up_transport_layer_info_c::types::gtp_tunnel) {
    logger.error("Expected GTP Tunnel");
    return proc_outcome_t::error;
  }
  asn1::ngap_nr::qos_flow_setup_request_item_s qos_flow_setup =
      pdu_ses_res_setup_req_trans.protocol_ies.qos_flow_setup_request_list.value[0];
  srsran::const_span<uint8_t> nas_pdu_dummy;
  uint32_t                    teid_out = 0;

  teid_out |= pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid[0] << 24u;
  teid_out |= pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid[1] << 16u;
  teid_out |= pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid[2] << 8u;
  teid_out |= pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().gtp_teid[3];

  // TODO: Check cause
  asn1::ngap_nr::cause_c                      cause;
  uint32_t                                    teid_in;
  uint16_t                                    lcid;
  asn1::bounded_bitstring<1, 160, true, true> addr_in;

  if (bearer_manager->add_pdu_session(
          ue_ctxt->rnti,
          su_req.pdu_session_id,
          qos_flow_setup.qos_flow_level_qos_params,
          pdu_ses_res_setup_req_trans.protocol_ies.ul_ngu_up_tnl_info.value.gtp_tunnel().transport_layer_address,
          teid_out,
          lcid,
          addr_in,
          teid_in,
          cause) != SRSRAN_SUCCESS) {
    logger.warning("Failed to add pdu session\n");
    return proc_outcome_t::error;
  }

  logger.info("Added PDU Session with LCID %d, teid_out %d, teid_in %d, addr_in %s",
              lcid,
              teid_out,
              teid_in,
              addr_in.to_string());

  // QoS parameter mapping in config in LTE enb
  if (su_req.pdu_session_nas_pdu_present) {
    if (rrc->establish_rrc_bearer(ue_ctxt->rnti, su_req.pdu_session_id, su_req.pdu_session_nas_pdu, lcid) ==
        SRSRAN_SUCCESS) {
      parent->send_pdu_session_resource_setup_response(su_req.pdu_session_id, teid_in, addr_in);
      return proc_outcome_t::success;
    }
  }

  return proc_outcome_t::yield;
}

proc_outcome_t ngap_ue_pdu_session_res_setup_proc::step()
{
  return proc_outcome_t::success;
}

} // namespace srsenb