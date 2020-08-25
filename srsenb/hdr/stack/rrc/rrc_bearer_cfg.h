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

#ifndef SRSLTE_RRC_BEARER_CFG_H
#define SRSLTE_RRC_BEARER_CFG_H

#include "srsenb/hdr/stack/rrc/rrc_config.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_rrc_interface_types.h"

namespace srsenb {

class security_cfg_handler
{
public:
  explicit security_cfg_handler(const rrc_cfg_t& cfg_) : cfg(&cfg_) {}

  bool set_security_capabilities(const asn1::s1ap::ue_security_cap_s& caps);
  void set_security_key(const asn1::fixed_bitstring<256, false, true>& key);
  void set_ncc(uint8_t ncc_) { ncc = ncc_; }

  asn1::rrc::security_algorithm_cfg_s get_security_algorithm_cfg();
  const srslte::as_security_config_t& get_as_sec_cfg() const { return sec_cfg; }
  uint8_t                             get_ncc() const { return ncc; }
  bool                                is_as_sec_cfg_valid() const { return k_enb_present; }

  void regenerate_keys_handover(uint32_t new_pci, uint32_t new_dl_earfcn);

private:
  void generate_as_keys();

  srslte::log_ref               log_h{"RRC"};
  const rrc_cfg_t*              cfg                   = nullptr;
  bool                          k_enb_present         = false;
  asn1::s1ap::ue_security_cap_s security_capabilities = {};
  uint8_t                       k_enb[32]             = {}; // Provided by MME
  srslte::as_security_config_t  sec_cfg               = {};
  uint8_t                       ncc                   = 0;
};

class bearer_cfg_handler
{
public:
  struct erab_t {
    uint8_t                                     id = 0;
    asn1::s1ap::erab_level_qos_params_s         qos_params;
    asn1::bounded_bitstring<1, 160, true, true> address;
    uint32_t                                    teid_out = 0;
    uint32_t                                    teid_in  = 0;
  };

  bearer_cfg_handler(uint16_t rnti_, const rrc_cfg_t& cfg_);

  void add_srb(uint8_t srb_id);
  int  add_erab(uint8_t                                            erab_id,
                const asn1::s1ap::erab_level_qos_params_s&         qos,
                const asn1::bounded_bitstring<1, 160, true, true>& addr,
                uint32_t                                           teid_out,
                const asn1::unbounded_octstring<true>*             nas_pdu);
  void release_erab(uint8_t erab_id);
  void release_erabs();
  void reest_bearers();

  bool fill_rr_cfg_ded(asn1::rrc::rr_cfg_ded_s& msg);
  void rr_ded_cfg_complete();

  // Methods to apply bearer updates
  void add_gtpu_bearer(gtpu_interface_rrc* gtpu, uint32_t erab_id);
  void fill_pending_nas_info(asn1::rrc::rrc_conn_recfg_r8_ies_s* msg);

  const std::map<uint8_t, erab_t>&                get_erabs() const { return erabs; }
  const asn1::rrc::drb_to_add_mod_list_l&         get_established_drbs() const { return current_drbs; }
  const asn1::rrc::srb_to_add_mod_list_l&         get_established_srbs() const { return current_srbs; }
  srslte::span<const asn1::rrc::drb_to_add_mod_s> get_pending_addmod_drbs() const { return drbs_to_add; }
  srslte::span<const uint8_t>                     get_pending_rem_drbs() const { return drbs_to_release; }
  srslte::span<const asn1::rrc::srb_to_add_mod_s> get_pending_addmod_srbs() const { return srbs_to_add; }

  std::map<uint8_t, std::vector<uint8_t> > erab_info_list;
  std::map<uint8_t, erab_t>                erabs;

private:
  srslte::log_ref  log_h{"RRC"};
  uint16_t         rnti = 0;
  const rrc_cfg_t* cfg  = nullptr;

  // last cfg
  asn1::rrc::srb_to_add_mod_list_l current_srbs;
  asn1::rrc::drb_to_add_mod_list_l current_drbs;

  // pending cfg updates
  asn1::rrc::srb_to_add_mod_list_l srbs_to_add;
  asn1::rrc::drb_to_add_mod_list_l drbs_to_add;
  asn1::rrc::drb_to_release_list_l drbs_to_release;
};

} // namespace srsenb

#endif // SRSLTE_RRC_BEARER_CFG_H
