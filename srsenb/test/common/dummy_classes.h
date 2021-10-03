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

#ifndef SRSENB_DUMMY_CLASSES_H
#define SRSENB_DUMMY_CLASSES_H

#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_mac_interfaces.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_rrc_interfaces.h"
#include "srsran/interfaces/enb_s1ap_interfaces.h"

namespace srsenb {

class mac_dummy : public mac_interface_rrc
{
public:
  int  cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg) override { return 0; }
  int  ue_cfg(uint16_t rnti, const sched_interface::ue_cfg_t* cfg) override { return 0; }
  int  ue_rem(uint16_t rnti) override { return 0; }
  int  ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, const sched_interface::ue_cfg_t& cfg) override { return 0; }
  int  bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, mac_lc_ch_cfg_t* cfg) override { return 0; }
  int  bearer_ue_rem(uint16_t rnti, uint32_t lc_id) override { return 0; }
  void phy_config_enabled(uint16_t rnti, bool enabled) override {}
  void write_mcch(const srsran::sib2_mbms_t* sib2_,
                  const srsran::sib13_t*     sib13_,
                  const srsran::mcch_msg_t*  mcch_,
                  const uint8_t*             mcch_payload,
                  const uint8_t              mcch_payload_length) override
  {}
  uint16_t reserve_new_crnti(const sched_interface::ue_cfg_t& ue_cfg) override { return last_rnti++; }

  uint16_t last_rnti = 70;
};

class s1ap_dummy : public s1ap_interface_rrc
{
public:
  void initial_ue(uint16_t                              rnti,
                  uint32_t                              enb_cc_idx,
                  asn1::s1ap::rrc_establishment_cause_e cause,
                  srsran::unique_byte_buffer_t          pdu) override
  {}
  void initial_ue(uint16_t                              rnti,
                  uint32_t                              enb_cc_idx,
                  asn1::s1ap::rrc_establishment_cause_e cause,
                  srsran::unique_byte_buffer_t          pdu,
                  uint32_t                              m_tmsi,
                  uint8_t                               mmec) override
  {}

  void write_pdu(uint16_t rnti, srsran::unique_byte_buffer_t pdu) override {}
  bool user_exists(uint16_t rnti) override { return true; }
  bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio) override { return true; }
  void notify_rrc_reconf_complete(uint16_t rnti) override {}
  bool is_mme_connected() override { return true; }
  bool send_ho_required(uint16_t                     rnti,
                        uint32_t                     target_eci,
                        srsran::plmn_id_t            target_plmn,
                        srsran::span<uint32_t>       fwd_erabs,
                        srsran::unique_byte_buffer_t rrc_container,
                        bool                         has_direct_fwd_path) override
  {
    return true;
  }
  bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) override
  {
    return true;
  }
  bool send_ho_req_ack(const asn1::s1ap::ho_request_s&                msg,
                       uint16_t                                       rnti,
                       uint32_t                                       enb_cc_idx,
                       srsran::unique_byte_buffer_t                   ho_cmd,
                       srsran::span<asn1::s1ap::erab_admitted_item_s> admitted_bearers,
                       srsran::const_span<asn1::s1ap::erab_item_s>    not_admitted_bearers) override
  {
    return true;
  }
  void send_ho_notify(uint16_t rnti, uint64_t target_eci) override {}

  void send_ho_cancel(uint16_t rnti, const asn1::s1ap::cause_c& cause) override {}

  bool release_erabs(uint16_t rnti, const std::vector<uint16_t>& erabs_successfully_released) override { return true; }
  bool send_ue_cap_info_indication(uint16_t rnti, const srsran::unique_byte_buffer_t ue_radio_cap) override
  {
    return true;
  }
};

class phy_dummy : public phy_interface_rrc_lte
{
public:
  void configure_mbsfn(srsran::sib2_mbms_t* sib2, srsran::sib13_t* sib13, const srsran::mcch_msg_t& mcch) override {}
  void set_config(uint16_t rnti, const phy_rrc_cfg_list_t& dedicated_list) override {}
  void complete_config(uint16_t rnti) override{};
};

class gtpu_dummy : public srsenb::gtpu_interface_rrc
{
public:
  srsran::expected<uint32_t> add_bearer(uint16_t            rnti,
                                        uint32_t            lcid,
                                        uint32_t            addr,
                                        uint32_t            teid_out,
                                        uint32_t&           addr_in,
                                        const bearer_props* props) override
  {
    return 1;
  }
  void set_tunnel_status(uint32_t teidin, bool dl_active) override {}
  void rem_bearer(uint16_t rnti, uint32_t lcid) override {}
  void mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) override {}
  void rem_user(uint16_t rnti) override {}
};

class rrc_dummy : public rrc_interface_s1ap
{
public:
  void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) override {}
  void release_ue(uint16_t rnti) override {}
  bool setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) override { return true; }
  bool modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg) override { return true; }
  int  get_erab_addr_in(uint16_t rnti, uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const override
  {
    return SRSRAN_SUCCESS;
  }
  void set_aggregate_max_bitrate(uint16_t rnti, const asn1::s1ap::ue_aggregate_maximum_bitrate_s& bitrate) override {}
  int  setup_erab(uint16_t                                           rnti,
                  uint16_t                                           erab_id,
                  const asn1::s1ap::erab_level_qos_params_s&         qos_params,
                  srsran::const_byte_span                            nas_pdu,
                  const asn1::bounded_bitstring<1, 160, true, true>& addr,
                  uint32_t                                           gtpu_teid_out,
                  asn1::s1ap::cause_c&                               cause) override
  {
    return SRSRAN_SUCCESS;
  }
  int modify_erab(uint16_t                                   rnti,
                  uint16_t                                   erab_id,
                  const asn1::s1ap::erab_level_qos_params_s& qos_params,
                  srsran::const_byte_span                    nas_pdu,
                  asn1::s1ap::cause_c&                       cause) override
  {
    return SRSRAN_SUCCESS;
  }
  bool has_erab(uint16_t rnti, uint32_t erab_id) const override { return true; }
  bool release_erabs(uint32_t rnti) override { return true; }
  int  release_erab(uint16_t rnti, uint16_t erab_id) override { return SRSRAN_SUCCESS; }
  void add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id) override {}
  void ho_preparation_complete(uint16_t                     rnti,
                               ho_prep_result               result,
                               const asn1::s1ap::ho_cmd_s&  msg,
                               srsran::unique_byte_buffer_t container) override
  {}
  uint16_t start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                      const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                      asn1::s1ap::cause_c& failure_cause) override
  {
    return SRSRAN_INVALID_RNTI;
  }
  void set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs) override {}

  int notify_ue_erab_updates(uint16_t rnti, srsran::const_byte_span nas_pdu) override { return SRSRAN_SUCCESS; }
};

} // namespace srsenb

#endif // SRSENB_DUMMY_CLASSES_H
