/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/ngap/ngap_ue_bearer_manager.h"

namespace srsenb {
ngap_ue_bearer_manager::ngap_ue_bearer_manager(rrc_interface_ngap_nr* rrc_,
                                               gtpu_interface_rrc*    gtpu_,
                                               srslog::basic_logger&  logger_) :
  gtpu(gtpu_), rrc(rrc_), logger(logger_)
{}
ngap_ue_bearer_manager::~ngap_ue_bearer_manager(){};

int ngap_ue_bearer_manager::add_pdu_session(uint16_t                                           rnti,
                                            uint8_t                                            pdu_session_id,
                                            const asn1::ngap_nr::qos_flow_level_qos_params_s&  qos,
                                            const asn1::bounded_bitstring<1, 160, true, true>& addr,
                                            uint32_t                                           teid_out,
                                            asn1::ngap_nr::cause_c&                            cause)
{
  // RRC call for QoS parameter and lcid <-> ID mapping
  int lcid = rrc->allocate_lcid(rnti);

  // Only add session if gtpu was successful
  pdu_session_t::gtpu_tunnel tunnel;

  if (addr.length() > 32) {
    logger.error("Only addresses with length <= 32 (IPv4) are supported");
    cause.set_radio_network().value = asn1::ngap_nr::cause_radio_network_opts::invalid_qos_combination;
    return SRSRAN_ERROR;
  }

  // TODO long term remove lcid and just use pdu_session_id and rnti as id for GTP tunnel

  int rtn = add_gtpu_bearer(rnti, lcid, pdu_session_id, teid_out, addr, tunnel);
  if (rtn != SRSRAN_SUCCESS) {
    logger.error("Adding PDU Session ID=%d to GTPU", pdu_session_id);
    return SRSRAN_ERROR;
  }

  pdu_session_list[pdu_session_id].id         = pdu_session_id;
  pdu_session_list[pdu_session_id].lcid       = lcid;
  pdu_session_list[pdu_session_id].qos_params = qos;
  pdu_session_list[pdu_session_id].tunnels.push_back(tunnel);
  return SRSRAN_SUCCESS;
}

int ngap_ue_bearer_manager::add_gtpu_bearer(uint16_t                                    rnti,
                                            uint32_t                                    lcid,
                                            uint32_t                                    pdu_session_id,
                                            uint32_t                                    teid_out,
                                            asn1::bounded_bitstring<1, 160, true, true> address,
                                            pdu_session_t::gtpu_tunnel&                 tunnel,
                                            const gtpu_interface_rrc::bearer_props*     props)
{
  // Initialize ERAB tunnel in GTPU right-away. DRBs are only created during RRC setup/reconf
  srsran::expected<uint32_t> rtn = gtpu->add_bearer(rnti, lcid, address.to_number(), teid_out, props);
  if (rtn.is_error()) {
    logger.error("Failed adding pdu_session_id=%d to GTPU", pdu_session_id);
    return SRSRAN_ERROR;
  }
  tunnel.teid_out = teid_out;
  tunnel.address  = address;
  tunnel.teid_in  = rtn.value();

  logger.info("Added GTPU tunnel for rnti %x, lcid %d, pdu_session_id=%d, address %s",
              rnti,
              lcid,
              pdu_session_id,
              address.to_string());
  return SRSRAN_SUCCESS;
}

void ngap_ue_bearer_manager::rem_gtpu_bearer(uint16_t rnti, uint32_t pdu_session_id)
{
  auto it = pdu_session_list.find(pdu_session_id);
  if (it == pdu_session_list.end()) {
    logger.warning("Removing pdu_session=%d from GTPU", pdu_session_id);
    return;
  }
  gtpu->rem_bearer(rnti, it->second.lcid);
}

} // namespace srsenb