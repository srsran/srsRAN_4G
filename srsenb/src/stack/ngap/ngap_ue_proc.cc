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
                                                                       ngap_ue_ctxt_t*           ue_ctxt_) :
  logger(srslog::fetch_basic_logger("NGAP UE"))
{
  parent  = parent_;
  rrc     = rrc_;
  ue_ctxt = ue_ctxt_;
};

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

proc_outcome_t ngap_ue_initial_context_setup_proc::react(bool security_mode_command_outcome)
{
  if(security_mode_command_outcome == true) {
    parent->send_initial_ctxt_setup_response();
    return proc_outcome_t::success;
  } 
  // TODO: error handling if security mode command fails
  return proc_outcome_t::error;
}

proc_outcome_t ngap_ue_initial_context_setup_proc::step() 
{
  return proc_outcome_t::yield;
}

ngap_ue_ue_context_release_proc::ngap_ue_ue_context_release_proc(ngap_interface_ngap_proc* parent_,
                                                                 rrc_interface_ngap_nr*    rrc_,
                                                                 ngap_ue_ctxt_t*           ue_ctxt_) :
  logger(srslog::fetch_basic_logger("NGAP UE"))
{
  parent  = parent_;
  rrc     = rrc_;
  ue_ctxt = ue_ctxt_;
};

proc_outcome_t ngap_ue_ue_context_release_proc::init(const asn1::ngap_nr::ue_context_release_cmd_s& msg)
{
  // ue_ngap_ids_c ue_ngap_ids = msg.protocol_ies.ue_ngap_ids.value;
  // cause_c       cause       = msg.protocol_ies.cause.value;
  return proc_outcome_t::success;
}

proc_outcome_t ngap_ue_ue_context_release_proc::step()
{
  return proc_outcome_t::success;
}

} // namespace srsenb