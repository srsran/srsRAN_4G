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

#ifndef SRSLTE_RRC_UE_H
#define SRSLTE_RRC_UE_H

#include "srsenb/hdr/stack/rrc/rrc_config.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_rrc_interface_types.h"

namespace srsenb {

class bearer_handler
{
public:
  struct erab_t {
    uint8_t                                     id;
    asn1::s1ap::erab_level_qos_params_s         qos_params;
    asn1::bounded_bitstring<1, 160, true, true> address;
    uint32_t                                    teid_out;
    uint32_t                                    teid_in;
  };

  bearer_handler(uint16_t            rnti_,
                 const rrc_cfg_t&    cfg_,
                 pdcp_interface_rrc* pdcp_,
                 rlc_interface_rrc*  rlc_,
                 gtpu_interface_rrc* gtpu_);

  void setup_srb(uint8_t srb_id);
  int  setup_erab(uint8_t                                            id,
                  const asn1::s1ap::erab_level_qos_params_s&         qos,
                  const asn1::bounded_bitstring<1, 160, true, true>& addr,
                  uint32_t                                           teid_out,
                  const asn1::unbounded_octstring<true>*             nas_pdu);

  void handle_rrc_setup(asn1::rrc::rrc_conn_setup_r8_ies_s* msg);
  void handle_rrc_reest(asn1::rrc::rrc_conn_reest_r8_ies_s* msg);
  void handle_rrc_reconf(asn1::rrc::rrc_conn_recfg_r8_ies_s* msg);

private:
  void fill_and_apply_bearer_updates(asn1::rrc::rr_cfg_ded_s& msg);
  void fill_pending_nas_info(asn1::rrc::rrc_conn_recfg_r8_ies_s* msg);

  srslte::log_ref           log_h{"RRC"};
  uint16_t                  rnti;
  const rrc_cfg_t*          cfg;
  pdcp_interface_rrc*       pdcp;
  rlc_interface_rrc*        rlc;
  gtpu_interface_rrc*       gtpu;
  srslte::byte_buffer_pool* pool = nullptr;

  std::map<uint8_t, srslte::unique_byte_buffer_t> erab_info_list;
  std::map<uint8_t, erab_t>                       erabs;

  // last cfg
  asn1::rrc::srb_to_add_mod_list_l last_srbs;
  asn1::rrc::drb_to_add_mod_list_l last_drbs;

  // pending cfg updates
  asn1::rrc::srb_to_add_mod_list_l srbs_to_add;
  asn1::rrc::drb_to_add_mod_list_l drbs_to_add;
  asn1::rrc::drb_to_release_list_l drbs_to_release;
};

} // namespace srsenb

#endif // SRSLTE_RRC_UE_H
