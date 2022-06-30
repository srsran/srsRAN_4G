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

#ifndef SRSRAN_RRC_BEARER_CFG_H
#define SRSRAN_RRC_BEARER_CFG_H

#include "srsenb/hdr/stack/rrc/rrc_config.h"
#include "srsran/asn1/s1ap.h"
#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

class security_cfg_handler
{
public:
  explicit security_cfg_handler(const rrc_cfg_t& cfg_) : cfg(&cfg_), logger(srslog::fetch_basic_logger("RRC")) {}
  explicit security_cfg_handler(const security_cfg_handler& other) : logger(srslog::fetch_basic_logger("RRC"))
  {
    cfg                   = other.cfg;
    k_enb_present         = other.k_enb_present;
    security_capabilities = other.security_capabilities;
    std::copy(other.k_enb, other.k_enb + 32, k_enb);
    sec_cfg = other.sec_cfg;
    ncc     = other.ncc;
  }

  security_cfg_handler& operator=(const security_cfg_handler& other)
  {
    cfg                   = other.cfg;
    k_enb_present         = other.k_enb_present;
    security_capabilities = other.security_capabilities;
    std::copy(other.k_enb, other.k_enb + 32, k_enb);
    sec_cfg = other.sec_cfg;
    ncc     = other.ncc;
    return *this;
  }

  bool set_security_capabilities(const asn1::s1ap::ue_security_cap_s& caps);
  void set_security_key(const asn1::fixed_bitstring<256, false, true>& key);
  void set_ncc(uint8_t ncc_) { ncc = ncc_; }

  asn1::rrc::security_algorithm_cfg_s get_security_algorithm_cfg();
  const srsran::as_security_config_t& get_as_sec_cfg() const { return sec_cfg; }
  uint8_t                             get_ncc() const { return ncc; }
  bool                                is_as_sec_cfg_valid() const { return k_enb_present; }

  void regenerate_keys_handover(uint32_t new_pci, uint32_t new_dl_earfcn);

private:
  void generate_as_keys();

  srslog::basic_logger&         logger;
  const rrc_cfg_t*              cfg                   = nullptr;
  bool                          k_enb_present         = false;
  asn1::s1ap::ue_security_cap_s security_capabilities = {};
  uint8_t                       k_enb[32]             = {}; // Provided by MME
  srsran::as_security_config_t  sec_cfg               = {};
  uint8_t                       ncc                   = 0;
};

class bearer_cfg_handler
{
public:
  struct erab_t {
    struct gtpu_tunnel {
      uint32_t teid_out = 0;
      uint32_t teid_in  = 0;
      uint32_t addr     = 0;
    };
    uint8_t                                     id   = 0;
    uint8_t                                     lcid = 0;
    asn1::s1ap::erab_level_qos_params_s         qos_params;
    asn1::bounded_bitstring<1, 160, true, true> address;
    uint32_t                                    teid_out = 0;
    uint32_t                                    teid_in  = 0;
    std::vector<gtpu_tunnel>                    tunnels;
  };

  bearer_cfg_handler(uint16_t rnti_, const rrc_cfg_t& cfg_, gtpu_interface_rrc* gtpu_);

  /// Called after RRCReestablishmentComplete, to add E-RABs of old rnti
  void reestablish_bearers(bearer_cfg_handler&& old_rnti_bearers);

  int  addmod_erab(uint8_t                                            erab_id,
                   const asn1::s1ap::erab_level_qos_params_s&         qos,
                   const asn1::bounded_bitstring<1, 160, true, true>& addr,
                   uint32_t                                           teid_out,
                   srsran::const_span<uint8_t>                        nas_pdu,
                   asn1::s1ap::cause_c&                               cause);
  int  release_erab(uint8_t erab_id);
  void release_erabs();
  int  modify_erab(uint8_t                                    erab_id,
                   const asn1::s1ap::erab_level_qos_params_s& qos,
                   srsran::const_span<uint8_t>                nas_pdu,
                   asn1::s1ap::cause_c&                       cause);

  // Methods to apply bearer updates
  int                        add_gtpu_bearer(uint32_t erab_id);
  srsran::expected<uint32_t> add_gtpu_bearer(uint32_t                                erab_id,
                                             uint32_t                                teid_out,
                                             uint32_t                                addr,
                                             const gtpu_interface_rrc::bearer_props* props = nullptr);
  void                       rem_gtpu_bearer(uint32_t erab_id);
  void                       fill_pending_nas_info(asn1::rrc::rrc_conn_recfg_r8_ies_s* msg);

  const std::map<uint8_t, erab_t>&        get_erabs() const { return erabs; }
  const asn1::rrc::drb_to_add_mod_list_l& get_established_drbs() const { return current_drbs; }

  std::map<uint8_t, std::vector<uint8_t> > erab_info_list;
  std::map<uint8_t, erab_t>                erabs;

private:
  srslog::basic_logger* logger;
  uint16_t              rnti = 0;
  const rrc_cfg_t*      cfg  = nullptr;
  gtpu_interface_rrc*   gtpu = nullptr;

  // last cfg
  asn1::rrc::drb_to_add_mod_list_l current_drbs;
};

} // namespace srsenb

#endif // SRSRAN_RRC_BEARER_CFG_H
