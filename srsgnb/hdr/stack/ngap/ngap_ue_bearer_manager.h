/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSENB_NGAP_UE_BEARER_MANAGER_H
#define SRSENB_NGAP_UE_BEARER_MANAGER_H

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/ngap.h"
#include "srsran/config.h"
#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"

namespace srsenb {

/**
 * @brief Manages the GTPU bearers as part of the NGAP session procedures
 * */

class ngap_ue_bearer_manager
{
public:
  struct pdu_session_t {
    struct gtpu_tunnel {
      uint32_t                                    teid_out = 0;
      uint32_t                                    teid_in  = 0;
      asn1::bounded_bitstring<1, 160, true, true> address_out;
      asn1::bounded_bitstring<1, 160, true, true> address_in;
    };
    uint8_t                                 id   = 0;
    uint8_t                                 lcid = 0;
    asn1::ngap::qos_flow_level_qos_params_s qos_params;
    std::vector<gtpu_tunnel>                tunnels;
  };

  ngap_ue_bearer_manager(gtpu_interface_rrc* gtpu_, srslog::basic_logger& logger_);
  ~ngap_ue_bearer_manager();

  int add_pdu_session(uint16_t                                           rnti,
                      uint8_t                                            pdu_session_id,
                      const asn1::ngap::qos_flow_level_qos_params_s&     qos,
                      const asn1::bounded_bitstring<1, 160, true, true>& addr,
                      uint32_t                                           teid_out,
                      uint16_t&                                          lcid,
                      asn1::bounded_bitstring<1, 160, true, true>&       addr_in,
                      uint32_t&                                          teid_in,
                      asn1::ngap::cause_c&                               cause);

  int reset_pdu_sessions(uint16_t rnti);

  using pdu_session_list_t = std::map<uint8_t, pdu_session_t>;
  const pdu_session_list_t& pdu_sessions() const { return pdu_session_list; }

private:
  gtpu_interface_rrc*   gtpu = nullptr;
  pdu_session_list_t    pdu_session_list;
  srslog::basic_logger& logger;

  std::map<uint32_t, uint32_t> next_lcid_list; // Map RNTI to next LCID to be allocated

  int     add_gtpu_bearer(uint16_t                                    rnti,
                          uint32_t                                    pdu_session_id,
                          uint32_t                                    teid_out,
                          asn1::bounded_bitstring<1, 160, true, true> address,
                          pdu_session_t::gtpu_tunnel&                 tunnel, // out parameter
                          const gtpu_interface_rrc::bearer_props*     props = nullptr);
  void    rem_gtpu_bearer(uint16_t rnti, uint32_t pdu_session_id);
  uint8_t allocate_lcid(uint32_t rnti);
};
} // namespace srsenb
#endif // SRSENB_NGAP_UE_BEARER_MANAGER_H
