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

#include "srsue/hdr/stack/upper/usim.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/standard_streams.h"
#include <sstream>

using namespace srsran;

namespace srsue {

usim::usim(srslog::basic_logger& logger) : usim_base(logger) {}

int usim::init(usim_args_t* args)
{
  imsi_str = args->imsi;
  imei_str = args->imei;

  const char* imsi_c = args->imsi.c_str();
  const char* imei_c = args->imei.c_str();

  auth_algo = auth_algo_milenage;
  if ("xor" == args->algo) {
    auth_algo = auth_algo_xor;
  }

  if (32 == args->k.length()) {
    str_to_hex(args->k, k);
  } else {
    logger.error("Invalid length for K: %zu should be %d", args->k.length(), 32);
    srsran::console("Invalid length for K: %zu should be %d\n", args->k.length(), 32);
  }

  if (auth_algo == auth_algo_milenage) {
    if (args->using_op) {
      if (32 == args->op.length()) {
        str_to_hex(args->op, op);
        compute_opc(k, op, opc);
      } else {
        logger.error("Invalid length for OP: %zu should be %d", args->op.length(), 32);
        srsran::console("Invalid length for OP: %zu should be %d\n", args->op.length(), 32);
      }
    } else {
      if (32 == args->opc.length()) {
        str_to_hex(args->opc, opc);
      } else {
        logger.error("Invalid length for OPc: %zu should be %d", args->opc.length(), 32);
        srsran::console("Invalid length for OPc: %zu should be %d\n", args->opc.length(), 32);
      }
    }
  }

  if (15 == args->imsi.length()) {
    imsi = 0;
    for (int i = 0; i < 15; i++) {
      imsi *= 10;
      imsi += imsi_c[i] - '0';
    }
  } else {
    logger.error("Invalid length for IMSI: %zu should be %d", args->imsi.length(), 15);
    srsran::console("Invalid length for IMSI: %zu should be %d\n", args->imsi.length(), 15);
  }

  if (15 == args->imei.length()) {
    imei = 0;
    for (int i = 0; i < 15; i++) {
      imei *= 10;
      imei += imei_c[i] - '0';
    }
  } else {
    logger.error("Invalid length for IMEI: %zu should be %d", args->imei.length(), 15);
    srsran::console("Invalid length for IMEI: %zu should be %d\n", args->imei.length(), 15);
  }

  initiated = true;

  return SRSRAN_SUCCESS;
}

void usim::stop() {}

/*******************************************************************************
  NAS interface
*******************************************************************************/

auth_result_t usim::generate_authentication_response(uint8_t* rand,
                                                     uint8_t* autn_enb,
                                                     uint16_t mcc,
                                                     uint16_t mnc,
                                                     uint8_t* res,
                                                     int*     res_len,
                                                     uint8_t* k_asme_)
{
  auth_result_t auth_result;
  uint8_t       ak_xor_sqn[6];

  if (auth_algo_xor == auth_algo) {
    auth_result = gen_auth_res_xor(rand, autn_enb, res, res_len, ak_xor_sqn);
  } else {
    auth_result = gen_auth_res_milenage(rand, autn_enb, res, res_len, ak_xor_sqn);
  }

  if (auth_result == AUTH_OK) {
    // Generate K_asme
    security_generate_k_asme(ck, ik, ak_xor_sqn, mcc, mnc, k_asme_);
  }
  return auth_result;
}

auth_result_t usim::generate_authentication_response_5g(uint8_t*    rand,
                                                        uint8_t*    autn_enb,
                                                        const char* serving_network_name,
                                                        uint8_t*    abba,
                                                        uint32_t    abba_len,
                                                        uint8_t*    res_star,
                                                        uint8_t*    k_amf)
{
  auth_result_t auth_result;
  uint8_t       ak_xor_sqn[6];
  uint8_t       res[16];
  uint8_t       k_ausf[32];
  uint8_t       k_seaf[32];
  int           res_len;

  if (auth_algo_xor == auth_algo) {
    auth_result = gen_auth_res_xor(rand, autn_enb, res, &res_len, ak_xor_sqn);
  } else {
    auth_result = gen_auth_res_milenage(rand, autn_enb, res, &res_len, ak_xor_sqn);
  }

  if (auth_result == AUTH_OK) {
    // Generate RES STAR
    security_generate_res_star(ck, ik, serving_network_name, rand, res, res_len, res_star);
    logger.debug(res_star, 16, "RES STAR");
    // Generate K_ausf
    security_generate_k_ausf(ck, ik, ak_xor_sqn, serving_network_name, k_ausf);
    logger.debug(k_ausf, 32, "K AUSF");
    // Generate K_seaf
    security_generate_k_seaf(k_ausf, serving_network_name, k_seaf);
    logger.debug(k_seaf, 32, "K SEAF");
    // Generate K_seaf
    logger.debug(abba, abba_len, "ABBA:");
    logger.debug("IMSI: %s", imsi_str.c_str());
    security_generate_k_amf(k_seaf, imsi_str.c_str(), abba, abba_len, k_amf);
    logger.debug(k_amf, 32, "K AMF");
  }
  return auth_result;
}

/*******************************************************************************
  Helpers
*******************************************************************************/

auth_result_t
usim::gen_auth_res_milenage(uint8_t* rand, uint8_t* autn_enb, uint8_t* res, int* res_len, uint8_t* ak_xor_sqn)
{
  auth_result_t result = AUTH_OK;
  uint32_t      i;
  uint8_t       sqn[6];

  // Use RAND and K to compute RES, CK, IK and AK
  security_milenage_f2345(k, opc, rand, res, ck, ik, ak);

  *res_len = 8;

  // Extract sqn from autn
  for (i = 0; i < 6; i++) {
    sqn[i] = autn_enb[i] ^ ak[i];
  }
  // Extract AMF from autn
  for (int i = 0; i < 2; i++) {
    amf[i] = autn_enb[6 + i];
  }

  // Generate MAC
  security_milenage_f1(k, opc, rand, sqn, amf, mac);

  // Construct AUTN
  for (i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }

  // Compare AUTNs
  for (i = 0; i < 16; i++) {
    if (autn[i] != autn_enb[i]) {
      result = AUTH_FAILED;
    }
  }

  for (i = 0; i < 6; i++) {
    ak_xor_sqn[i] = sqn[i] ^ ak[i];
  }

  logger.debug(ck, CK_LEN, "CK:");
  logger.debug(ik, IK_LEN, "IK:");
  logger.debug(ak, AK_LEN, "AK:");
  logger.debug(sqn, 6, "sqn:");
  logger.debug(amf, 2, "amf:");
  logger.debug(mac, 8, "mac:");

  return result;
}

// 3GPP TS 34.108 version 10.0.0 Section 8
auth_result_t usim::gen_auth_res_xor(uint8_t* rand, uint8_t* autn_enb, uint8_t* res, int* res_len, uint8_t* ak_xor_sqn)
{
  auth_result_t result = AUTH_OK;
  uint8_t       sqn[6];
  uint8_t       res_[16];

  logger.debug(k, 16, "K:");

  // Use RAND and K to compute RES, CK, IK and AK
  security_xor_f2345(k, rand, res_, ck, ik, ak);

  for (uint32_t i = 0; i < 8; i++) {
    res[i] = res_[i];
  }

  *res_len = 8;

  // Extract sqn from autn
  for (uint32_t i = 0; i < 6; i++) {
    sqn[i] = autn_enb[i] ^ ak[i];
  }
  // Extract AMF from autn
  for (uint32_t i = 0; i < 2; i++) {
    amf[i] = autn_enb[6 + i];
  }

  // Generate MAC
  security_xor_f1(k, rand, sqn, amf, mac);

  // Construct AUTN
  for (uint32_t i = 0; i < 6; i++) {
    autn[i] = sqn[i] ^ ak[i];
  }
  for (uint32_t i = 0; i < 2; i++) {
    autn[6 + i] = amf[i];
  }
  for (uint32_t i = 0; i < 8; i++) {
    autn[8 + i] = mac[i];
  }

  // Compare AUTNs
  for (uint32_t i = 0; i < 16; i++) {
    if (autn[i] != autn_enb[i]) {
      result = AUTH_FAILED;
    }
  }

  logger.debug(ck, CK_LEN, "CK:");
  logger.debug(ik, IK_LEN, "IK:");
  logger.debug(ak, AK_LEN, "AK:");
  logger.debug(sqn, 6, "sqn:");
  logger.debug(amf, 2, "amf:");
  logger.debug(mac, 8, "mac:");

  for (uint32_t i = 0; i < 6; i++) {
    ak_xor_sqn[i] = sqn[i] ^ ak[i];
  }

  return result;
}

std::string usim::get_mnc_str(const uint8_t* imsi_vec, std::string mcc_str)
{
  uint32_t           mcc_len = 3;
  uint32_t           mnc_len = 2;
  std::ostringstream mnc_oss;

  // US MCC uses 3 MNC digits
  if (!mcc_str.compare("310") || !mcc_str.compare("311") || !mcc_str.compare("312") || !mcc_str.compare("313") ||
      !mcc_str.compare("316")) {
    mnc_len = 3;
  }

  for (uint32_t i = mcc_len; i < mcc_len + mnc_len; i++) {
    mnc_oss << (int)imsi_vec[i];
  }

  return mnc_oss.str();
}

void usim::str_to_hex(std::string str, uint8_t* hex)
{
  uint32_t    i;
  const char* h_str = str.c_str();
  uint32_t    len   = str.length();

  for (i = 0; i < len / 2; i++) {
    if (h_str[i * 2 + 0] >= '0' && h_str[i * 2 + 0] <= '9') {
      hex[i] = (h_str[i * 2 + 0] - '0') << 4;
    } else if (h_str[i * 2 + 0] >= 'A' && h_str[i * 2 + 0] <= 'F') {
      hex[i] = ((h_str[i * 2 + 0] - 'A') + 0xA) << 4;
    } else {
      hex[i] = ((h_str[i * 2 + 0] - 'a') + 0xA) << 4;
    }

    if (h_str[i * 2 + 1] >= '0' && h_str[i * 2 + 1] <= '9') {
      hex[i] |= h_str[i * 2 + 1] - '0';
    } else if (h_str[i * 2 + 1] >= 'A' && h_str[i * 2 + 1] <= 'F') {
      hex[i] |= (h_str[i * 2 + 1] - 'A') + 0xA;
    } else {
      hex[i] |= (h_str[i * 2 + 1] - 'a') + 0xA;
    }
  }
}

} // namespace srsue
