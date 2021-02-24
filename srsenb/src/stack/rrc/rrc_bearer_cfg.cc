/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/rrc/rrc_bearer_cfg.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/asn1/rrc_utils.h"
#include "srslte/rrc/rrc_cfg_utils.h"

namespace srsenb {

using namespace asn1::rrc;

asn1::rrc::security_algorithm_cfg_s security_cfg_handler::get_security_algorithm_cfg()
{
  asn1::rrc::security_algorithm_cfg_s ret;
  // TODO: select these based on UE capabilities and preference order
  ret.integrity_prot_algorithm = (security_algorithm_cfg_s::integrity_prot_algorithm_e_::options)sec_cfg.integ_algo;
  ret.ciphering_algorithm      = (ciphering_algorithm_r12_e::options)sec_cfg.cipher_algo;
  return ret;
}

bool security_cfg_handler::set_security_capabilities(const asn1::s1ap::ue_security_cap_s& caps)
{
  security_capabilities = caps;

  // Selects security algorithms (cipher_algo and integ_algo) based on capabilities and config preferences
  // Each position in the bitmap represents an encryption algorithm:
  // “all bits equal to 0” – UE supports no other algorithm than EEA0,
  // “first bit” – 128-EEA1,
  // “second bit” – 128-EEA2,
  // “third bit” – 128-EEA3,
  // other bits reserved for future use. Value ‘1’ indicates support and value
  // ‘0’ indicates no support of the algorithm.
  // Algorithms are defined in TS 33.401 [15].
  // Note: information missing

  bool enc_algo_found   = false;
  bool integ_algo_found = false;

  for (auto& cipher_item : cfg->eea_preference_list) {
    auto& v = security_capabilities.encryption_algorithms;
    switch (cipher_item) {
      case srslte::CIPHERING_ALGORITHM_ID_EEA0:
        // “all bits equal to 0” – UE supports no other algorithm than EEA0,
        // specification does not cover the case in which EEA0 is supported with other algorithms
        // just assume that EEA0 is always supported even this can not be explicity signaled by S1AP
        sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_EEA0;
        enc_algo_found      = true;
        logger.info("Selected EEA0 as RRC encryption algorithm");
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA1:
        // “first bit” – 128-EEA1,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA1)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
          enc_algo_found      = true;
          logger.info("Selected EEA1 as RRC encryption algorithm");
          break;
        } else {
          logger.info("Failed to selected EEA1 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA2:
        // “second bit” – 128-EEA2,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA2)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA2;
          enc_algo_found      = true;
          logger.info("Selected EEA2 as RRC encryption algorithm");
          break;
        } else {
          logger.info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srslte::CIPHERING_ALGORITHM_ID_128_EEA3:
        // “third bit” – 128-EEA3,
        if (v.get(v.length() - srslte::CIPHERING_ALGORITHM_ID_128_EEA3)) {
          sec_cfg.cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA3;
          enc_algo_found      = true;
          logger.info("Selected EEA3 as RRC encryption algorithm");
          break;
        } else {
          logger.info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      default:
        enc_algo_found = false;
        break;
    }
    if (enc_algo_found) {
      break;
    }
  }

  for (auto& eia_enum : cfg->eia_preference_list) {
    auto& v = security_capabilities.integrity_protection_algorithms;
    switch (eia_enum) {
      case srslte::INTEGRITY_ALGORITHM_ID_EIA0:
        // Null integrity is not supported
        logger.info("Skipping EIA0 as RRC integrity algorithm. Null integrity is not supported.");
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA1:
        // “first bit” – 128-EIA1,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA1)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;
          integ_algo_found   = true;
          logger.info("Selected EIA1 as RRC integrity algorithm.");
        } else {
          logger.info("Failed to selected EIA1 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA2:
        // “second bit” – 128-EIA2,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA2)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA2;
          integ_algo_found   = true;
          logger.info("Selected EIA2 as RRC integrity algorithm.");
        } else {
          logger.info("Failed to selected EIA2 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srslte::INTEGRITY_ALGORITHM_ID_128_EIA3:
        // “third bit” – 128-EIA3,
        if (v.get(v.length() - srslte::INTEGRITY_ALGORITHM_ID_128_EIA3)) {
          sec_cfg.integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA3;
          integ_algo_found   = true;
          logger.info("Selected EIA3 as RRC integrity algorithm.");
        } else {
          logger.info("Failed to selected EIA3 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      default:
        integ_algo_found = false;
        break;
    }

    if (integ_algo_found) {
      break;
    }
  }

  if (not integ_algo_found || not enc_algo_found) {
    // TODO: if no security algorithm found abort radio connection and issue
    // encryption-and-or-integrity-protection-algorithms-not-supported message
    logger.error("Did not find a matching integrity or encryption algorithm with the UE");
    return false;
  }
  return true;
}

void security_cfg_handler::set_security_key(const asn1::fixed_bitstring<256, false, true>& key)
{
  k_enb_present = true;
  for (uint32_t i = 0; i < key.nof_octets(); ++i) {
    k_enb[i] = key.data()[key.nof_octets() - 1 - i];
  }
  logger.info(k_enb, 32, "Key eNodeB (k_enb)");

  generate_as_keys();
}

void security_cfg_handler::generate_as_keys()
{
  // Generate K_rrc_enc and K_rrc_int
  srslte::security_generate_k_rrc(
      k_enb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_rrc_enc.data(), sec_cfg.k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_up_enc.data(), sec_cfg.k_up_int.data());

  logger.info(k_enb, 32, "K_eNB (k_enb)");
  logger.info(sec_cfg.k_rrc_enc.data(), 32, "RRC Encryption Key (k_rrc_enc)");
  logger.info(sec_cfg.k_rrc_int.data(), 32, "RRC Integrity Key (k_rrc_int)");
  logger.info(sec_cfg.k_up_enc.data(), 32, "UP Encryption Key (k_up_enc)");
}

void security_cfg_handler::regenerate_keys_handover(uint32_t new_pci, uint32_t new_dl_earfcn)
{
  logger.info("Regenerating KeNB with PCI=0x%02x, DL-EARFCN=%d", new_pci, new_dl_earfcn);
  logger.info(k_enb, 32, "Old K_eNB (k_enb)");
  // Generate K_enb*
  uint8_t k_enb_star[32];
  srslte::security_generate_k_enb_star(k_enb, new_pci, new_dl_earfcn, k_enb_star);

  // K_enb becomes K_enb*
  memcpy(k_enb, k_enb_star, 32);

  generate_as_keys();
}

/*****************************
 *      Bearer Handler
 ****************************/

bearer_cfg_handler::bearer_cfg_handler(uint16_t rnti_, const rrc_cfg_t& cfg_, gtpu_interface_rrc* gtpu_) :
  rnti(rnti_), cfg(&cfg_), gtpu(gtpu_), logger(srslog::fetch_basic_logger("RRC"))
{}

int bearer_cfg_handler::add_erab(uint8_t                                            erab_id,
                                 const asn1::s1ap::erab_level_qos_params_s&         qos,
                                 const asn1::bounded_bitstring<1, 160, true, true>& addr,
                                 uint32_t                                           teid_out,
                                 const asn1::unbounded_octstring<true>*             nas_pdu)
{
  if (erab_id < 5) {
    logger.error("ERAB id=%d is invalid", erab_id);
    return SRSLTE_ERROR;
  }
  uint8_t lcid  = erab_id - 2; // Map e.g. E-RAB 5 to LCID 3 (==DRB1)
  uint8_t drbid = erab_id - 4;

  auto qci_it = cfg->qci_cfg.find(qos.qci);
  if (qci_it == cfg->qci_cfg.end() or not qci_it->second.configured) {
    logger.error("QCI=%d not configured", qos.qci);
    return SRSLTE_ERROR;
  }
  if (lcid < 3 or lcid > 10) {
    logger.error("DRB logical channel ids must be within 3 and 10");
    return SRSLTE_ERROR;
  }
  const rrc_cfg_qci_t& qci_cfg = qci_it->second;

  erabs[erab_id].id         = erab_id;
  erabs[erab_id].qos_params = qos;
  erabs[erab_id].address    = addr;
  erabs[erab_id].teid_out   = teid_out;

  if (addr.length() > 32) {
    logger.error("Only addresses with length <= 32 are supported");
    return SRSLTE_ERROR;
  }

  if (nas_pdu != nullptr and nas_pdu->size() > 0) {
    erab_info_list[erab_id].assign(nas_pdu->data(), nas_pdu->data() + nas_pdu->size());
    logger.info(
        &erab_info_list[erab_id][0], erab_info_list[erab_id].size(), "setup_erab nas_pdu -> erab_info rnti 0x%x", rnti);
  }

  // Set DRBtoAddMod
  auto drb_it                                              = srslte::add_rrc_obj_id(current_drbs, drbid);
  drb_it->lc_ch_id_present                                 = true;
  drb_it->lc_ch_id                                         = (uint8_t)lcid;
  drb_it->eps_bearer_id_present                            = true;
  drb_it->eps_bearer_id                                    = (uint8_t)erab_id;
  drb_it->lc_ch_cfg_present                                = true;
  drb_it->lc_ch_cfg.ul_specific_params_present             = true;
  drb_it->lc_ch_cfg.ul_specific_params.lc_ch_group_present = true;
  drb_it->lc_ch_cfg.ul_specific_params                     = qci_cfg.lc_cfg;
  drb_it->pdcp_cfg_present                                 = true;
  drb_it->pdcp_cfg                                         = qci_cfg.pdcp_cfg;
  drb_it->rlc_cfg_present                                  = true;
  drb_it->rlc_cfg                                          = qci_cfg.rlc_cfg;

  return SRSLTE_SUCCESS;
}

bool bearer_cfg_handler::release_erab(uint8_t erab_id)
{
  auto it = erabs.find(erab_id);
  if (it == erabs.end()) {
    logger.warning("The user rnti=0x%x does not contain ERAB-ID=%d", rnti, erab_id);
    return false;
  }

  uint8_t drb_id = erab_id - 4;

  srslte::rem_rrc_obj_id(current_drbs, drb_id);

  erabs.erase(it);
  erab_info_list.erase(erab_id);

  return true;
}

void bearer_cfg_handler::release_erabs()
{
  // TODO: notify GTPU layer for each ERAB
  erabs.clear();
  while (not erabs.empty()) {
    release_erab(erabs.begin()->first);
  }
}

bool bearer_cfg_handler::modify_erab(uint8_t                                    erab_id,
                                     const asn1::s1ap::erab_level_qos_params_s& qos,
                                     const asn1::unbounded_octstring<true>*     nas_pdu)
{
  logger.info("Modifying E-RAB %d", erab_id);
  std::map<uint8_t, erab_t>::iterator erab_it = erabs.find(erab_id);
  if (erab_it == erabs.end()) {
    logger.error("Could not find E-RAB to modify");
    return false;
  }
  auto     address  = erab_it->second.address;
  uint32_t teid_out = erab_it->second.teid_out;
  release_erab(erab_id);
  add_erab(erab_id, qos, address, teid_out, nas_pdu);
  return true;
}

void bearer_cfg_handler::add_gtpu_bearer(uint32_t erab_id)
{
  auto it = erabs.find(erab_id);
  if (it == erabs.end()) {
    logger.error("Adding erab_id=%d to GTPU", erab_id);
    return;
  }
  it->second.teid_in = add_gtpu_bearer(erab_id, it->second.teid_out, it->second.address.to_number(), nullptr);
}

uint32_t bearer_cfg_handler::add_gtpu_bearer(uint32_t                                erab_id,
                                             uint32_t                                teid_out,
                                             uint32_t                                addr,
                                             const gtpu_interface_rrc::bearer_props* props)
{
  auto it = erabs.find(erab_id);
  if (it == erabs.end()) {
    logger.error("Adding erab_id=%d to GTPU", erab_id);
    return 0;
  }

  // Initialize ERAB tunnel in GTPU right-away. DRBs are only created during RRC setup/reconf
  erab_t&             erab = it->second;
  erab_t::gtpu_tunnel bearer;
  bearer.teid_out = teid_out;
  bearer.addr     = addr;
  bearer.teid_in  = gtpu->add_bearer(rnti, erab.id - 2, addr, teid_out, props);
  erab.tunnels.push_back(bearer);
  return bearer.teid_in;
}

void bearer_cfg_handler::rem_gtpu_bearer(uint32_t erab_id)
{
  auto it = erabs.find(erab_id);
  if (it != erabs.end()) {
    // Map e.g. E-RAB 5 to LCID 3 (==DRB1)
    gtpu->rem_bearer(rnti, erab_id - 2);
  } else {
    logger.error("Removing erab_id=%d to GTPU\n", erab_id);
  }
}

void bearer_cfg_handler::fill_pending_nas_info(asn1::rrc::rrc_conn_recfg_r8_ies_s* msg)
{
  // Add space for NAS messages
  uint8_t n_nas = erab_info_list.size();
  if (n_nas > 0) {
    msg->ded_info_nas_list_present = true;
    msg->ded_info_nas_list.resize(n_nas);
  }

  uint32_t idx = 0;
  // DRBs have already been configured in GTPU during bearer setup
  // Add E-RAB info message for the E-RABs
  if (msg->rr_cfg_ded.drb_to_add_mod_list_present) {
    for (const drb_to_add_mod_s& drb : msg->rr_cfg_ded.drb_to_add_mod_list) {
      uint8_t erab_id = drb.drb_id + 4;
      auto    it      = erab_info_list.find(erab_id);
      if (it != erab_info_list.end()) {
        const std::vector<uint8_t>& erab_info = it->second;
        logger.info(&erab_info[0], erab_info.size(), "connection_reconf erab_info -> nas_info rnti 0x%x", rnti);
        msg->ded_info_nas_list[idx].resize(erab_info.size());
        memcpy(msg->ded_info_nas_list[idx].data(), &erab_info[0], erab_info.size());
        erab_info_list.erase(it);
      } else {
        logger.debug("Not adding NAS message to connection reconfiguration. E-RAB id %d", erab_id);
      }
      idx++;
    }
  }
}

} // namespace srsenb
