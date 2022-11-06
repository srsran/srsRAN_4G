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

#include "srsran/common/security.h"
#include "mbedtls/md5.h"
#include "srsran/common/liblte_security.h"
#include "srsran/common/s3g.h"
#include "srsran/common/ssl.h"
#include "srsran/config.h"
#include <arpa/inet.h>

#define FC_EPS_K_ASME_DERIVATION 0x10
#define FC_EPS_K_ENB_DERIVATION 0x11
#define FC_EPS_NH_DERIVATION 0x12
#define FC_EPS_K_ENB_STAR_DERIVATION 0x13
#define FC_EPS_ALGORITHM_KEY_DERIVATION 0x15

#define ALGO_EPS_DISTINGUISHER_NAS_ENC_ALG 0x01
#define ALGO_EPS_DISTINGUISHER_NAS_INT_ALG 0x02
#define ALGO_EPS_DISTINGUISHER_RRC_ENC_ALG 0x03
#define ALGO_EPS_DISTINGUISHER_RRC_INT_ALG 0x04
#define ALGO_EPS_DISTINGUISHER_UP_ENC_ALG 0x05
#define ALGO_EPS_DISTINGUISHER_UP_INT_ALG 0x06

#define FC_5G_K_GNB_STAR_DERIVATION 0x70
#define FC_5G_ALGORITHM_KEY_DERIVATION 0x69
#define FC_5G_KAUSF_DERIVATION 0x6A
#define FC_5G_RES_STAR_DERIVATION 0x6B
#define FC_5G_KSEAF_DERIVATION 0x6C
#define FC_5G_KAMF_DERIVATION 0x6D
#define FC_5G_KGNB_KN3IWF_DERIVATION 0x6E
#define FC_5G_NH_GNB_DERIVATION 0x6F

#define ALGO_5G_DISTINGUISHER_NAS_ENC_ALG 0x01
#define ALGO_5G_DISTINGUISHER_NAS_INT_ALG 0x02
#define ALGO_5G_DISTINGUISHER_RRC_ENC_ALG 0x03
#define ALGO_5G_DISTINGUISHER_RRC_INT_ALG 0x04
#define ALGO_5G_DISTINGUISHER_UP_ENC_ALG 0x05
#define ALGO_5G_DISTINGUISHER_UP_INT_ALG 0x06
namespace srsran {

/******************************************************************************
 * Key Generation
 *****************************************************************************/

uint8_t security_generate_k_asme(const uint8_t* ck,
                                 const uint8_t* ik,
                                 const uint8_t* ak_xor_sqn_,
                                 const uint16_t mcc,
                                 const uint16_t mnc,
                                 uint8_t*       k_asme)
{
  if (ck == NULL || ik == NULL || ak_xor_sqn_ == NULL || k_asme == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  // The input key Key shall be equal to the concatenation CK || IK of CK and IK.
  memcpy(key.data(), ck, 16);
  memcpy(key.data() + 16, ik, 16);

  // Serving Network id
  std::vector<uint8_t> sn_id;
  sn_id.resize(3);
  sn_id[0] = (mcc & 0x00F0) | ((mcc & 0x0F00) >> 8); // First byte of P0
  if ((mnc & 0xFF00) == 0xFF00) {
    // 2-digit MNC
    sn_id[1] = 0xF0 | (mcc & 0x000F);                         // Second byte of P0
    sn_id[2] = ((mnc & 0x000F) << 4) | ((mnc & 0x00F0) >> 4); // Third byte of P0
  } else {
    // 3-digit MNC
    sn_id[1] = ((mnc & 0x000F) << 4) | (mcc & 0x000F);   // Second byte of P0
    sn_id[2] = ((mnc & 0x00F0)) | ((mnc & 0x0F00) >> 8); // Third byte of P0
  }

  // AK XOR SQN
  std::vector<uint8_t> ak_xor_sqn;
  ak_xor_sqn.resize(AK_LEN);
  memcpy(ak_xor_sqn.data(), ak_xor_sqn_, ak_xor_sqn.size());

  uint8_t output[32];
  if (kdf_common(FC_EPS_K_ASME_DERIVATION, key, sn_id, ak_xor_sqn, output) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  memcpy(k_asme, output, 32);
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_ausf(const uint8_t* ck,
                                 const uint8_t* ik,
                                 const uint8_t* ak_xor_sqn_,
                                 const char*    serving_network_name,
                                 uint8_t*       k_ausf)
{
  if (ck == NULL || ik == NULL || ak_xor_sqn_ == NULL || serving_network_name == NULL || k_ausf == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }

  std::array<uint8_t, 32> key;

  // The input key Key shall be equal to the concatenation CK || IK of CK and IK.
  memcpy(key.data(), ck, 16);
  memcpy(key.data() + 16, ik, 16);

  // Serving Network Name
  std::vector<uint8_t> ssn;
  ssn.resize(strlen(serving_network_name));
  memcpy(ssn.data(), serving_network_name, ssn.size());

  // AK XOR SQN
  std::vector<uint8_t> ak_xor_sqn;
  ak_xor_sqn.resize(AK_LEN);
  memcpy(ak_xor_sqn.data(), ak_xor_sqn_, ak_xor_sqn.size());

  uint8_t output[32];
  if (kdf_common(FC_5G_KAUSF_DERIVATION, key, ssn, ak_xor_sqn, output) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  memcpy(k_ausf, output, 32);

  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_seaf(const uint8_t* k_ausf, const char* serving_network_name, uint8_t* k_seaf)
{
  if (k_ausf == NULL || serving_network_name == NULL || k_seaf == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }

  std::array<uint8_t, 32> key;
  memcpy(key.data(), k_ausf, 32);

  // Serving Network Name
  std::vector<uint8_t> ssn;
  ssn.resize(strlen(serving_network_name));
  memcpy(ssn.data(), serving_network_name, ssn.size());

  if (kdf_common(FC_5G_KSEAF_DERIVATION, key, ssn, k_seaf) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_amf(const uint8_t* k_seaf,
                                const char*    supi_,
                                const uint8_t* abba_,
                                const uint32_t abba_len,
                                uint8_t*       k_amf)
{
  if (k_seaf == NULL || supi_ == NULL || abba_ == NULL || k_amf == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }

  std::array<uint8_t, 32> key;
  memcpy(key.data(), k_seaf, 32);

  // SUPI
  std::vector<uint8_t> supi;
  supi.resize(strlen(supi_));
  memcpy(supi.data(), supi_, supi.size());

  // ABBA
  std::vector<uint8_t> abba;
  abba.resize(abba_len);
  memcpy(abba.data(), abba_, abba.size());

  if (kdf_common(FC_5G_KAMF_DERIVATION, key, supi, abba, k_amf) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_gnb(const as_key_t& k_amf, const uint32_t nas_count_, as_key_t& k_gnb)
{
  if (k_amf.empty()) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }

  // NAS Count
  std::vector<uint8_t> nas_count;
  nas_count.resize(4);
  nas_count[0] = (nas_count_ >> 24) & 0xFF;
  nas_count[1] = (nas_count_ >> 16) & 0xFF;
  nas_count[2] = (nas_count_ >> 8) & 0xFF;
  nas_count[3] = nas_count_ & 0xFF;

  // Access Type Distinguisher 3GPP access = 0x01 (TS 33501 Annex A.9)
  std::vector<uint8_t> access_type_distinguisher = {1};

  if (kdf_common(FC_5G_KGNB_KN3IWF_DERIVATION, k_amf, nas_count, access_type_distinguisher, k_gnb.data()) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_enb(const uint8_t* k_asme, const uint32_t nas_count_, uint8_t* k_enb)
{
  if (k_asme == NULL || k_enb == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_asme, 32);

  // NAS Count
  std::vector<uint8_t> nas_count;
  nas_count.resize(4);
  nas_count[0] = (nas_count_ >> 24) & 0xFF;
  nas_count[1] = (nas_count_ >> 16) & 0xFF;
  nas_count[2] = (nas_count_ >> 8) & 0xFF;
  nas_count[3] = nas_count_ & 0xFF;

  if (kdf_common(FC_EPS_K_ENB_DERIVATION, key, nas_count, k_enb) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

// Generate k_enb* according to TS 33.401 Appendix A.5
uint8_t
security_generate_k_enb_star(const uint8_t* k_enb, const uint32_t pci_, const uint32_t earfcn_, uint8_t* k_enb_star)

{
  return security_generate_k_nb_star_common(FC_EPS_K_ENB_STAR_DERIVATION, k_enb, pci_, earfcn_, k_enb_star);
}

// Generate k_gnb* according to TS 33.501 Appendix A.11
uint8_t
security_generate_k_gnb_star(const uint8_t* k_gnb, const uint32_t pci_, const uint32_t dl_arfcn_, uint8_t* k_gnb_star)

{
  return security_generate_k_nb_star_common(FC_5G_K_GNB_STAR_DERIVATION, k_gnb, pci_, dl_arfcn_, k_gnb_star);
}

uint8_t security_generate_k_nb_star_common(uint8_t        fc,
                                           const uint8_t* k_enb,
                                           const uint32_t pci_,
                                           const uint32_t earfcn_,
                                           uint8_t*       k_enb_star)
{
  if (k_enb == NULL || k_enb_star == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;
  memcpy(key.data(), k_enb, 32);

  // PCI
  std::vector<uint8_t> pci;
  pci.resize(2);
  pci[0] = (pci_ >> 8) & 0xFF;
  pci[1] = pci_ & 0xFF;

  // ARFCN, can be two or three bytes
  std::vector<uint8_t> earfcn;
  if (earfcn_ < pow(2, 16)) {
    earfcn.resize(2);
    earfcn[0] = (earfcn_ >> 8) & 0xFF;
    earfcn[1] = earfcn_ & 0xFF;
  } else if (earfcn_ < pow(2, 24)) {
    earfcn.resize(3);
    earfcn[0] = (earfcn_ >> 16) & 0xFF;
    earfcn[1] = (earfcn_ >> 8) & 0xFF;
    earfcn[2] = earfcn_ & 0xFF;
  }

  if (kdf_common(fc, key, pci, earfcn, k_enb_star) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_nh(const uint8_t* k_asme, const uint8_t* sync_, uint8_t* nh)
{
  if (k_asme == NULL || sync_ == NULL || nh == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;
  memcpy(key.data(), k_asme, 32);

  // PCI
  std::vector<uint8_t> sync;
  sync.resize(32);
  memcpy(sync.data(), sync_, 32);

  if (kdf_common(FC_EPS_NH_DERIVATION, key, sync, nh) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_nas(const uint8_t*                    k_asme,
                                const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                          k_nas_enc,
                                uint8_t*                          k_nas_int)
{
  if (k_asme == NULL || k_nas_enc == NULL || k_nas_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_asme, 32);

  // Derive NAS ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_NAS_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_nas_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive NAS INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_NAS_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive NAS int
  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_nas_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_nas_5g(const uint8_t*                    k_amf,
                                   const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                   const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                   uint8_t*                          k_nas_enc,
                                   uint8_t*                          k_nas_int)
{
  if (k_amf == NULL || k_nas_enc == NULL || k_nas_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_amf, 32);

  // Derive NAS ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_NAS_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_nas_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive NAS INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_NAS_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive NAS int
  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_nas_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_rrc(const uint8_t*                    k_enb,
                                const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                uint8_t*                          k_rrc_enc,
                                uint8_t*                          k_rrc_int)
{
  if (k_enb == NULL || k_rrc_enc == NULL || k_rrc_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_enb, 32);

  // Derive RRC ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_RRC_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_rrc_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive RRC INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_RRC_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive RRC int
  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_rrc_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_up(const uint8_t*                    k_enb,
                               const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                               const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                               uint8_t*                          k_up_enc,
                               uint8_t*                          k_up_int)
{
  if (k_enb == NULL || k_up_enc == NULL || k_up_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_enb, 32);

  // Derive UP ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_UP_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_up_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive UP INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_EPS_DISTINGUISHER_UP_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive UP int
  if (kdf_common(FC_EPS_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_up_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_sk_gnb(const uint8_t* k_enb, const uint16_t scg_count_, uint8_t* sk_gnb)
{
  if (k_enb == NULL || sk_gnb == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_enb, 32);

  // SCG Count
  std::vector<uint8_t> scg_count;
  scg_count.resize(2);
  scg_count[0] = (scg_count_ >> 8) & 0xFF; // first byte of P0
  scg_count[1] = scg_count_ & 0xFF;        // second byte of P0

  // Derive sk_gnb
  uint8_t output[32];
  if (kdf_common(0x1C, key, scg_count, output) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  memcpy(sk_gnb, output, 32);

  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_nr_rrc(const uint8_t*                    k_gnb,
                                   const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                   const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                   uint8_t*                          k_rrc_enc,
                                   uint8_t*                          k_rrc_int)
{
  if (k_gnb == NULL || k_rrc_enc == NULL || k_rrc_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_gnb, 32);

  // Derive RRC ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_RRC_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_rrc_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive RRC INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_RRC_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive RRC int
  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_rrc_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_k_nr_up(const uint8_t*                    k_gnb,
                                  const CIPHERING_ALGORITHM_ID_ENUM enc_alg_id,
                                  const INTEGRITY_ALGORITHM_ID_ENUM int_alg_id,
                                  uint8_t*                          k_up_enc,
                                  uint8_t*                          k_up_int)
{
  if (k_gnb == NULL || k_up_enc == NULL || k_up_int == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  memcpy(key.data(), k_gnb, 32);

  // Derive UP ENC
  // algorithm type distinguisher
  std::vector<uint8_t> algo_distinguisher;
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_UP_ENC_ALG;

  // algorithm type distinguisher
  std::vector<uint8_t> algorithm_identity;
  algorithm_identity.resize(1);
  algorithm_identity[0] = enc_alg_id;

  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_up_enc) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }

  // Derive UP INT
  // algorithm type distinguisher
  algo_distinguisher.resize(1);
  algo_distinguisher[0] = ALGO_5G_DISTINGUISHER_UP_INT_ALG;

  // algorithm type distinguisher
  algorithm_identity.resize(1);
  algorithm_identity[0] = int_alg_id;

  // Derive UP int
  if (kdf_common(FC_5G_ALGORITHM_KEY_DERIVATION, key, algo_distinguisher, algorithm_identity, k_up_int) !=
      SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

uint8_t security_generate_res_star(const uint8_t* ck,
                                   const uint8_t* ik,
                                   const char*    serving_network_name,
                                   const uint8_t* rand_,
                                   const uint8_t* res_,
                                   const size_t   res_len_,
                                   uint8_t*       res_star)
{
  if (ck == NULL || ik == NULL || serving_network_name == NULL || rand_ == NULL || res_ == NULL || res_star == NULL) {
    log_error("Invalid inputs");
    return SRSRAN_ERROR;
  }
  std::array<uint8_t, 32> key;

  // The input key Key shall be equal to the concatenation CK || IK of CK and IK.
  memcpy(key.data(), ck, 16);
  memcpy(key.data() + 16, ik, 16);

  // Serving Network Name
  std::vector<uint8_t> ssn;
  ssn.resize(strlen(serving_network_name));
  memcpy(ssn.data(), serving_network_name, strlen(serving_network_name));

  // RAND
  std::vector<uint8_t> rand;
  rand.resize(AKA_RAND_LEN);
  memcpy(rand.data(), rand_, rand.size());

  // RES
  std::vector<uint8_t> res;
  res.resize(res_len_);
  memcpy(res.data(), res_, res.size());

  uint8_t output[32];
  if (kdf_common(FC_5G_RES_STAR_DERIVATION, key, ssn, rand, res, output) != SRSRAN_SUCCESS) {
    log_error("Failed to run kdf_common");
    return SRSRAN_ERROR;
  }
  memcpy(res_star, output + 16, 16);

  return SRSRAN_SUCCESS;
}

int kdf_common(const uint8_t fc, const std::array<uint8_t, 32>& key, const std::vector<uint8_t>& P0, uint8_t* output)
{
  uint8_t* s;
  uint32_t s_len = 1 + P0.size() + 2;

  s = (uint8_t*)calloc(s_len, sizeof(uint8_t));

  if (s == nullptr) {
    log_error("Unable to allocate memory in %s()", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  uint32_t i = 0;
  s[i]       = fc; // FC
  i++;

  // P0
  memcpy(&s[i], P0.data(), P0.size());
  i += P0.size();
  uint16_t p0_length_value = htons(P0.size());
  memcpy(&s[i], &p0_length_value, sizeof(p0_length_value));
  i += sizeof(p0_length_value);

  sha256(key.data(), key.size(), s, i, output, 0);
  free(s);

  return SRSRAN_SUCCESS;
}

int kdf_common(const uint8_t                  fc,
               const std::array<uint8_t, 32>& key,
               const std::vector<uint8_t>&    P0,
               const std::vector<uint8_t>&    P1,
               uint8_t*                       output)
{
  uint8_t* s;
  uint32_t s_len = 1 + P0.size() + 2 + P1.size() + 2;

  s = (uint8_t*)calloc(s_len, sizeof(uint8_t));

  if (s == nullptr) {
    log_error("Unable to allocate memory in %s()", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  uint32_t i = 0;
  s[i]       = fc; // FC
  i++;

  // P0
  memcpy(&s[i], P0.data(), P0.size());
  i += P0.size();
  uint16_t p0_length_value = htons(P0.size());
  memcpy(&s[i], &p0_length_value, sizeof(p0_length_value));
  i += sizeof(p0_length_value);

  // P1
  memcpy(&s[i], P1.data(), P1.size());
  i += P1.size();
  uint16_t p1_length_value = htons(P1.size());
  memcpy(&s[i], &p1_length_value, sizeof(p1_length_value));
  i += sizeof(p1_length_value);

  sha256(key.data(), key.size(), s, i, output, 0);
  free(s);

  return SRSRAN_SUCCESS;
}

int kdf_common(const uint8_t                  fc,
               const std::array<uint8_t, 32>& key,
               const std::vector<uint8_t>&    P0,
               const std::vector<uint8_t>&    P1,
               const std::vector<uint8_t>&    P2,
               uint8_t*                       output)
{
  uint8_t* s;
  uint32_t s_len = 1 + P0.size() + 2 + P1.size() + 2 + P2.size() + 2;

  s = (uint8_t*)calloc(s_len, sizeof(uint8_t));

  if (s == nullptr) {
    log_error("Unable to allocate memory in %s()", __FUNCTION__);
    return SRSRAN_ERROR;
  }

  uint32_t i = 0;
  s[i]       = fc; // FC
  i++;

  // P0
  memcpy(&s[i], P0.data(), P0.size());
  i += P0.size();
  uint16_t p0_length_value = htons(P0.size());
  memcpy(&s[i], &p0_length_value, sizeof(p0_length_value));
  i += sizeof(p0_length_value);

  // P1
  memcpy(&s[i], P1.data(), P1.size());
  i += P1.size();
  uint16_t p1_length_value = htons(P1.size());
  memcpy(&s[i], &p1_length_value, sizeof(p1_length_value));
  i += sizeof(p1_length_value);

  // P2
  memcpy(&s[i], P2.data(), P2.size());
  i += P2.size();
  uint16_t p2_length_value = htons(P2.size());
  memcpy(&s[i], &p2_length_value, sizeof(p2_length_value));
  i += sizeof(p2_length_value);

  sha256(key.data(), key.size(), s, i, output, 0);
  free(s);

  return SRSRAN_SUCCESS;
}
/******************************************************************************
 * Integrity Protection
 *****************************************************************************/

uint8_t security_128_eia1(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia1(key, count, bearer, direction, msg, msg_len, mac);
}

uint8_t security_128_eia2(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia2(key, count, bearer, direction, msg, msg_len, mac);
}

uint8_t security_128_eia3(const uint8_t* key,
                          uint32_t       count,
                          uint32_t       bearer,
                          uint8_t        direction,
                          uint8_t*       msg,
                          uint32_t       msg_len,
                          uint8_t*       mac)
{
  return liblte_security_128_eia3(key, count, bearer, direction, msg, msg_len * 8, mac);
}

uint8_t security_md5(const uint8_t* input, size_t len, uint8_t* output)
{
  memset(output, 0x00, 16);
  mbedtls_md5(input, len, output);
  return SRSRAN_SUCCESS;
}

/******************************************************************************
 * Encryption / Decryption
 *****************************************************************************/

uint8_t security_128_eea1(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{
  return liblte_security_encryption_eea1(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

uint8_t security_128_eea2(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{
  return liblte_security_encryption_eea2(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

uint8_t security_128_eea3(uint8_t* key,
                          uint32_t count,
                          uint8_t  bearer,
                          uint8_t  direction,
                          uint8_t* msg,
                          uint32_t msg_len,
                          uint8_t* msg_out)
{
  return liblte_security_encryption_eea3(key, count, bearer, direction, msg, msg_len * 8, msg_out);
}

/******************************************************************************
 * Authentication
 *****************************************************************************/
uint8_t compute_opc(uint8_t* k, uint8_t* op, uint8_t* opc)
{
  return liblte_compute_opc(k, op, opc);
}

uint8_t security_milenage_f1(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a)
{
  return liblte_security_milenage_f1(k, op, rand, sqn, amf, mac_a);
}

uint8_t security_milenage_f1_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_s)
{
  return liblte_security_milenage_f1_star(k, op, rand, sqn, amf, mac_s);
}

uint8_t
security_milenage_f2345(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak)
{
  return liblte_security_milenage_f2345(k, op, rand, res, ck, ik, ak);
}

uint8_t security_milenage_f5_star(uint8_t* k, uint8_t* op, uint8_t* rand, uint8_t* ak)
{
  return liblte_security_milenage_f5_star(k, op, rand, ak);
}

int security_xor_f2345(uint8_t* k, uint8_t* rand, uint8_t* res, uint8_t* ck, uint8_t* ik, uint8_t* ak)
{
  uint8_t xdout[16];
  uint8_t cdout[8];
  // Use RAND and K to compute RES, CK, IK and AK
  for (uint32_t i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }
  for (uint32_t i = 0; i < 16; i++) {
    res[i] = xdout[i];
    ck[i]  = xdout[(i + 1) % 16];
    ik[i]  = xdout[(i + 2) % 16];
  }
  for (uint32_t i = 0; i < 6; i++) {
    ak[i] = xdout[i + 3];
  }
  return SRSRAN_SUCCESS;
}

int security_xor_f1(uint8_t* k, uint8_t* rand, uint8_t* sqn, uint8_t* amf, uint8_t* mac_a)
{
  uint8_t xdout[16];
  uint8_t cdout[8];
  // Use RAND and K to compute RES, CK, IK and AK
  for (uint32_t i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }
  // Generate cdout
  for (uint32_t i = 0; i < 6; i++) {
    cdout[i] = sqn[i];
  }
  for (uint32_t i = 0; i < 2; i++) {
    cdout[6 + i] = amf[i];
  }

  // Generate MAC
  for (uint32_t i = 0; i < 8; i++) {
    mac_a[i] = xdout[i] ^ cdout[i];
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsran
