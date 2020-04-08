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

#include "srsue/hdr/stack/upper/usim.h"
#include "srslte/common/bcd_helpers.h"
#include <sstream>

using namespace srslte;

namespace srsue {

usim::usim(srslte::log* log_) : usim_log(log_) {}

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
    usim_log->error("Invalid length for K: %zu should be %d\n", args->k.length(), 32);
    usim_log->console("Invalid length for K: %zu should be %d\n", args->k.length(), 32);
  }

  if (auth_algo == auth_algo_milenage) {
    if (args->using_op) {
      if (32 == args->op.length()) {
        str_to_hex(args->op, op);
        compute_opc(k, op, opc);
      } else {
        usim_log->error("Invalid length for OP: %zu should be %d\n", args->op.length(), 32);
        usim_log->console("Invalid length for OP: %zu should be %d\n", args->op.length(), 32);
      }
    } else {
      if (32 == args->opc.length()) {
        str_to_hex(args->opc, opc);
      } else {
        usim_log->error("Invalid length for OPc: %zu should be %d\n", args->opc.length(), 32);
        usim_log->console("Invalid length for OPc: %zu should be %d\n", args->opc.length(), 32);
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
    usim_log->error("Invalid length for IMSI: %zu should be %d\n", args->imsi.length(), 15);
    usim_log->console("Invalid length for IMSI: %zu should be %d\n", args->imsi.length(), 15);
  }

  if (15 == args->imei.length()) {
    imei = 0;
    for (int i = 0; i < 15; i++) {
      imei *= 10;
      imei += imei_c[i] - '0';
    }
  } else {
    usim_log->error("Invalid length for IMEI: %zu should be %d\n", args->imei.length(), 15);
    usim_log->console("Invalid length for IMEI: %zu should be %d\n", args->imei.length(), 15);
  }

  initiated = true;

  return SRSLTE_SUCCESS;
}

void usim::stop() {}

/*******************************************************************************
  NAS interface
*******************************************************************************/

std::string usim::get_imsi_str()
{
  return imsi_str;
}
std::string usim::get_imei_str()
{
  return imei_str;
}

bool usim::get_imsi_vec(uint8_t* imsi_, uint32_t n)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return false;
  }

  if (NULL == imsi_ || n < 15) {
    usim_log->error("Invalid parameters to get_imsi_vec\n");
    return false;
  }

  uint64_t temp = imsi;
  for (int i = 14; i >= 0; i--) {
    imsi_[i] = temp % 10;
    temp /= 10;
  }
  return true;
}

bool usim::get_imei_vec(uint8_t* imei_, uint32_t n)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return false;
  }

  if (NULL == imei_ || n < 15) {
    usim_log->error("Invalid parameters to get_imei_vec\n");
    return false;
  }

  uint64 temp = imei;
  for (int i = 14; i >= 0; i--) {
    imei_[i] = temp % 10;
    temp /= 10;
  }
  return true;
}

bool usim::get_home_plmn_id(srslte::plmn_id_t* home_plmn_id)
{
  if (!initiated) {
    ERROR("USIM not initiated!\n");
    return false;
  }

  int mcc_len = 3;
  int mnc_len = 2;

  uint8_t imsi_vec[15];
  get_imsi_vec(imsi_vec, 15);

  std::ostringstream mcc_str, mnc_str;

  for (int i = 0; i < mcc_len; i++) {
    mcc_str << (int)imsi_vec[i];
  }

  // US MCC uses 3 MNC digits
  if (!mcc_str.str().compare("310") || !mcc_str.str().compare("311") || !mcc_str.str().compare("312") ||
      !mcc_str.str().compare("313") || !mcc_str.str().compare("316")) {
    mnc_len = 3;
  }
  for (int i = mcc_len; i < mcc_len + mnc_len; i++) {
    mnc_str << (int)imsi_vec[i];
  }

  home_plmn_id->from_string(mcc_str.str() + mnc_str.str());

  usim_log->info("Read Home PLMN Id=%s\n", home_plmn_id->to_string().c_str());

  return true;
}

auth_result_t usim::generate_authentication_response(uint8_t* rand,
                                                     uint8_t* autn_enb,
                                                     uint16_t mcc,
                                                     uint16_t mnc,
                                                     uint8_t* res,
                                                     int*     res_len,
                                                     uint8_t* k_asme)
{
  if (auth_algo_xor == auth_algo) {
    return gen_auth_res_xor(rand, autn_enb, mcc, mnc, res, res_len, k_asme);
  } else {
    return gen_auth_res_milenage(rand, autn_enb, mcc, mnc, res, res_len, k_asme);
  }
}

void usim::generate_nas_keys(uint8_t*                    k_asme,
                             uint8_t*                    k_nas_enc,
                             uint8_t*                    k_nas_int,
                             CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                             INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  // Generate K_nas_enc and K_nas_int
  security_generate_k_nas(k_asme, cipher_algo, integ_algo, k_nas_enc, k_nas_int);
}

/*******************************************************************************
  RRC interface
*******************************************************************************/

void usim::generate_as_keys(uint8_t* k_asme, uint32_t count_ul, srslte::as_security_config_t *sec_cfg)
{
  // Generate K_enb
  security_generate_k_enb(k_asme, count_ul, k_enb);

  memcpy(this->k_asme, k_asme, 32);

  // Save initial k_enb
  memcpy(k_enb_initial, k_enb, 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_up_enc.data(), sec_cfg->k_up_int.data());

  current_ncc  = 0;
  is_first_ncc = true;
}

void usim::generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srslte::as_security_config_t* sec_cfg)
{
  uint8_t* enb_star_key = k_enb;

  if (ncc < 0) {
    ncc = current_ncc;
  }

  // Generate successive NH
  while (current_ncc != (uint32_t)ncc) {
    uint8_t* sync = NULL;
    if (is_first_ncc) {
      sync         = k_enb_initial;
      is_first_ncc = false;
    } else {
      sync = nh;
    }
    // Generate NH
    security_generate_nh(k_asme, sync, nh);

    current_ncc++;
    if (current_ncc == 8) {
      current_ncc = 0;
    }
    enb_star_key = nh;
  }

  // Generate K_enb
  security_generate_k_enb_star(enb_star_key, pci, earfcn, k_enb_star);

  // K_enb becomes K_enb*
  memcpy(k_enb, k_enb_star, 32);

  // Generate K_rrc_enc and K_rrc_int
  security_generate_k_rrc(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_rrc_enc.data(), sec_cfg->k_rrc_int.data());

  // Generate K_up_enc and K_up_int
  security_generate_k_up(
      k_enb, sec_cfg->cipher_algo, sec_cfg->integ_algo, sec_cfg->k_up_enc.data(), sec_cfg->k_up_int.data());
}

/*******************************************************************************
  Helpers
*******************************************************************************/

auth_result_t usim::gen_auth_res_milenage(uint8_t* rand,
                                          uint8_t* autn_enb,
                                          uint16_t mcc,
                                          uint16_t mnc,
                                          uint8_t* res,
                                          int*     res_len,
                                          uint8_t* k_asme)
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

  // Generate K_asme
  security_generate_k_asme(ck, ik, ak, sqn, mcc, mnc, k_asme);

  return result;
}

// 3GPP TS 34.108 version 10.0.0 Section 8
auth_result_t usim::gen_auth_res_xor(uint8_t* rand,
                                     uint8_t* autn_enb,
                                     uint16_t mcc,
                                     uint16_t mnc,
                                     uint8_t* res,
                                     int*     res_len,
                                     uint8_t* k_asme)
{
  auth_result_t result = AUTH_OK;
  uint32_t      i;
  uint8_t       sqn[6];
  uint8_t       xdout[16];
  uint8_t       cdout[8];

  // Use RAND and K to compute RES, CK, IK and AK
  for (i = 0; i < 16; i++) {
    xdout[i] = k[i] ^ rand[i];
  }
  for (i = 0; i < 16; i++) {
    res[i] = xdout[i];
    ck[i]  = xdout[(i + 1) % 16];
    ik[i]  = xdout[(i + 2) % 16];
  }
  for (i = 0; i < 6; i++) {
    ak[i] = xdout[i + 3];
  }

  *res_len = 8;

  // Extract sqn from autn
  for (i = 0; i < 6; i++) {
    sqn[i] = autn_enb[i] ^ ak[i];
  }
  // Extract AMF from autn
  for (int i = 0; i < 2; i++) {
    amf[i] = autn_enb[6 + i];
  }

  // Generate cdout
  for (i = 0; i < 6; i++) {
    cdout[i] = sqn[i];
  }
  for (i = 0; i < 2; i++) {
    cdout[6 + i] = amf[i];
  }

  // Generate MAC
  for (i = 0; i < 8; i++) {
    mac[i] = xdout[i] ^ cdout[i];
  }

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

  // Generate K_asme
  security_generate_k_asme(ck, ik, ak, sqn, mcc, mnc, k_asme);

  return result;
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
