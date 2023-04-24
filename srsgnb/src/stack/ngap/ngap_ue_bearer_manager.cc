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

#include "srsgnb/hdr/stack/ngap/ngap_ue_bearer_manager.h"
#include "srsran/common/common_nr.h"

namespace srsenb {
ngap_ue_bearer_manager::ngap_ue_bearer_manager(gtpu_interface_rrc* gtpu_, srslog::basic_logger& logger_) :
  gtpu(gtpu_), logger(logger_)
{
}
ngap_ue_bearer_manager::~ngap_ue_bearer_manager(){};

int ngap_ue_bearer_manager::add_pdu_session(uint16_t                                           rnti,
                                            uint8_t                                            pdu_session_id,
                                            const asn1::ngap::qos_flow_level_qos_params_s&     qos,
                                            const asn1::bounded_bitstring<1, 160, true, true>& addr_out,
                                            uint32_t                                           teid_out,
                                            uint16_t&                                          lcid,
                                            asn1::bounded_bitstring<1, 160, true, true>&       addr_in,
                                            uint32_t&                                          teid_in,
                                            asn1::ngap::cause_c&                               cause)
{
  // Only add session if gtpu was successful
  pdu_session_t::gtpu_tunnel tunnel;

  if (addr_out.length() > 32) {
    logger.error("Only addresses with length <= 32 (IPv4) are supported");
    cause.set_radio_network().value = asn1::ngap::cause_radio_network_opts::invalid_qos_combination;
    return SRSRAN_ERROR;
  }

  lcid = allocate_lcid(rnti);
  if (lcid >= srsran::MAX_NR_NOF_BEARERS) {
    logger.error("Adding PDU Session ID=%d to GTPU. No free LCID.", pdu_session_id);
    return SRSRAN_ERROR;
  }

  // TODO: remove lcid and just use pdu_session_id and rnti as id for GTP tunnel
  int rtn = add_gtpu_bearer(rnti, pdu_session_id, teid_out, addr_out, tunnel);
  if (rtn != SRSRAN_SUCCESS) {
    logger.error("Adding PDU Session ID=%d to GTPU", pdu_session_id);
    return SRSRAN_ERROR;
  }

  pdu_session_list[pdu_session_id].id         = pdu_session_id;
  pdu_session_list[pdu_session_id].lcid       = lcid;
  pdu_session_list[pdu_session_id].qos_params = qos;
  pdu_session_list[pdu_session_id].tunnels.push_back(tunnel);

  // return values
  teid_in = tunnel.teid_in;
  addr_in = tunnel.address_in;

  return SRSRAN_SUCCESS;
}

int ngap_ue_bearer_manager::reset_pdu_sessions(uint16_t rnti)
{
  for (auto iter = pdu_session_list.begin(); iter != pdu_session_list.end(); iter++) {
    auto pdu_session_id = iter->first;
    rem_gtpu_bearer(rnti, pdu_session_id);
  }
  return true;
}

int ngap_ue_bearer_manager::add_gtpu_bearer(uint16_t                                    rnti,
                                            uint32_t                                    pdu_session_id,
                                            uint32_t                                    teid_out,
                                            asn1::bounded_bitstring<1, 160, true, true> address_out,
                                            pdu_session_t::gtpu_tunnel&                 tunnel,
                                            const gtpu_interface_rrc::bearer_props*     props)
{
  // Initialize ERAB tunnel in GTPU right-away. DRBs are only created during RRC setup/reconf
  uint32_t                   addr_in;
  srsran::expected<uint32_t> rtn =
      gtpu->add_bearer(rnti, pdu_session_id, address_out.to_number(), teid_out, addr_in, props);
  if (rtn.is_error()) {
    logger.error("Failed adding pdu_session_id=%d to GTPU", pdu_session_id);
    return SRSRAN_ERROR;
  }

  tunnel.teid_out    = teid_out;
  tunnel.address_out = address_out;

  logger.info("Addr in %x", addr_in);

  tunnel.address_in.from_number(addr_in, 32);
  tunnel.teid_in = rtn.value();

  logger.info("Added GTPU tunnel rnti 0x%04x, pdu_session_id=%d, teid_out %d, teid_in %d, address out 0x%x, "
              "address in 0x%x",
              rnti,
              pdu_session_id,
              tunnel.teid_out,
              tunnel.teid_in,
              tunnel.address_out.to_number(),
              tunnel.address_in.to_number());
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

uint8_t ngap_ue_bearer_manager::allocate_lcid(uint32_t rnti)
{
  if (pdu_session_list.empty()) {
    return 4;
  }
  for (unsigned lcid = 4; lcid < srsran::MAX_NR_NOF_BEARERS; lcid++) {
    const auto pdu_session_it =
        std::find_if(pdu_session_list.cbegin(),
                     pdu_session_list.cend(),
                     [lcid](const std::pair<uint8_t, pdu_session_t>& t) { return t.second.lcid != lcid; });
    if (pdu_session_it != pdu_session_list.cend()) {
      return lcid;
    }
  }
  // All LCIDs are used.
  return srsran::MAX_NR_NOF_BEARERS;
}

} // namespace srsenb
