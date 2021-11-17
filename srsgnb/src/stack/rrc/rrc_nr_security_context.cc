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

#include "srsgnb/hdr/stack/rrc/rrc_nr_security_context.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc_utils.h"

namespace srsgnb {

asn1::rrc_nr::security_algorithm_cfg_s nr_security_context::get_security_algorithm_cfg() const
{
  asn1::rrc_nr::security_algorithm_cfg_s ret;
  // TODO: select these based on UE capabilities and preference order
  ret.integrity_prot_algorithm = (asn1::rrc_nr::integrity_prot_algorithm_e::options)sec_cfg.integ_algo;
  ret.ciphering_algorithm      = (asn1::rrc_nr::ciphering_algorithm_e::options)sec_cfg.cipher_algo;
  return ret;
}

bool nr_security_context::set_security_capabilities(const asn1::ngap_nr::ue_security_cap_s& caps)
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
    auto& v = security_capabilities.nrencryption_algorithms;
    switch (cipher_item) {
      case srsran::CIPHERING_ALGORITHM_ID_EEA0:
        // “all bits equal to 0” – UE supports no other algorithm than EEA0,
        // specification does not cover the case in which EEA0 is supported with other algorithms
        // just assume that EEA0 is always supported even this can not be explicity signaled by S1AP
        sec_cfg.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_EEA0;
        enc_algo_found      = true;
        logger.info("Selected EEA0 as RRC encryption algorithm");
        break;
      case srsran::CIPHERING_ALGORITHM_ID_128_EEA1:
        // “first bit” – 128-EEA1,
        if (v.get(v.length() - srsran::CIPHERING_ALGORITHM_ID_128_EEA1)) {
          sec_cfg.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_128_EEA1;
          enc_algo_found      = true;
          logger.info("Selected EEA1 as RRC encryption algorithm");
          break;
        } else {
          logger.info("Failed to selected EEA1 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srsran::CIPHERING_ALGORITHM_ID_128_EEA2:
        // “second bit” – 128-EEA2,
        if (v.get(v.length() - srsran::CIPHERING_ALGORITHM_ID_128_EEA2)) {
          sec_cfg.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_128_EEA2;
          enc_algo_found      = true;
          logger.info("Selected EEA2 as RRC encryption algorithm");
          break;
        } else {
          logger.info("Failed to selected EEA2 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srsran::CIPHERING_ALGORITHM_ID_128_EEA3:
        // “third bit” – 128-EEA3,
        if (v.get(v.length() - srsran::CIPHERING_ALGORITHM_ID_128_EEA3)) {
          sec_cfg.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_128_EEA3;
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
    auto& v = security_capabilities.nrintegrity_protection_algorithms;
    switch (eia_enum) {
      case srsran::INTEGRITY_ALGORITHM_ID_EIA0:
        // Null integrity is not supported
        logger.info("Skipping EIA0 as RRC integrity algorithm. Null integrity is not supported.");
        sec_cfg.integ_algo = srsran::INTEGRITY_ALGORITHM_ID_EIA0;
        integ_algo_found   = true;
        break;
      case srsran::INTEGRITY_ALGORITHM_ID_128_EIA1:
        // “first bit” – 128-EIA1,
        if (v.get(v.length() - srsran::INTEGRITY_ALGORITHM_ID_128_EIA1)) {
          sec_cfg.integ_algo = srsran::INTEGRITY_ALGORITHM_ID_128_EIA1;
          integ_algo_found   = true;
          logger.info("Selected EIA1 as RRC integrity algorithm.");
        } else {
          logger.info("Failed to selected EIA1 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srsran::INTEGRITY_ALGORITHM_ID_128_EIA2:
        // “second bit” – 128-EIA2,
        if (v.get(v.length() - srsran::INTEGRITY_ALGORITHM_ID_128_EIA2)) {
          sec_cfg.integ_algo = srsran::INTEGRITY_ALGORITHM_ID_128_EIA2;
          integ_algo_found   = true;
          logger.info("Selected EIA2 as RRC integrity algorithm.");
        } else {
          logger.info("Failed to selected EIA2 as RRC encryption algorithm, due to unsupported algorithm");
        }
        break;
      case srsran::INTEGRITY_ALGORITHM_ID_128_EIA3:
        // “third bit” – 128-EIA3,
        if (v.get(v.length() - srsran::INTEGRITY_ALGORITHM_ID_128_EIA3)) {
          sec_cfg.integ_algo = srsran::INTEGRITY_ALGORITHM_ID_128_EIA3;
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
    logger.error("Did not find a matching integrity or encryption algorithm with the UE");
    return false;
  }
  return true;
}

void nr_security_context::set_security_key(const asn1::fixed_bitstring<256, false, true>& key)
{
  k_gnb_present = true;
  for (uint32_t i = 0; i < key.nof_octets(); ++i) {
    k_gnb[i] = key.data()[key.nof_octets() - 1 - i];
  }
  logger.info(k_gnb, 32, "Key gNodeB (k_gnb)");

  generate_as_keys();
}

void nr_security_context::generate_as_keys()
{
  // Generate K_rrc_enc and K_rrc_int
  srsran::security_generate_k_nr_rrc(
      k_gnb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_rrc_enc.data(), sec_cfg.k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_nr_up(
      k_gnb, sec_cfg.cipher_algo, sec_cfg.integ_algo, sec_cfg.k_up_enc.data(), sec_cfg.k_up_int.data());

  logger.info(k_gnb, 32, "K_gNB (k_gnb)");
  logger.info(sec_cfg.k_rrc_enc.data(), 32, "RRC Encryption Key (k_rrc_enc)");
  logger.info(sec_cfg.k_rrc_int.data(), 32, "RRC Integrity Key (k_rrc_int)");
  logger.info(sec_cfg.k_up_enc.data(), 32, "UP Encryption Key (k_up_enc)");
  logger.info(sec_cfg.k_up_int.data(), 32, "UP Encryption Key (k_up_enc)");
}

void nr_security_context::regenerate_keys_handover(uint32_t new_pci, uint32_t new_dl_earfcn)
{
  logger.info("Regenerating KeNB with PCI=0x%02x, DL-EARFCN=%d", new_pci, new_dl_earfcn);
  logger.info(k_gnb, 32, "Old K_eNB (k_enb)");
  // Generate K_enb*
  uint8_t k_gnb_star[32];
  srsran::security_generate_k_enb_star(k_gnb, new_pci, new_dl_earfcn, k_gnb_star);

  // K_enb becomes K_enb*
  memcpy(k_gnb, k_gnb_star, 32);

  generate_as_keys();
}

} // namespace srsgnb
