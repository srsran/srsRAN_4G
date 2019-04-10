/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/upper/pdcp_entity_base.h"
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity_base::pdcp_entity_base() {}

pdcp_entity_base::~pdcp_entity_base() {}

void pdcp_entity_base::config_security(uint8_t*                    k_rrc_enc_,
                                       uint8_t*                    k_rrc_int_,
                                       uint8_t*                    k_up_enc_,
                                       uint8_t*                    k_up_int_,
                                       CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                       INTEGRITY_ALGORITHM_ID_ENUM integ_algo_)
{
  for (int i = 0; i < 32; i++) {
    k_rrc_enc[i] = k_rrc_enc_[i];
    k_rrc_int[i] = k_rrc_int_[i];
    k_up_enc[i]  = k_up_enc_[i];
    if (k_up_int_ != nullptr) {
      k_up_int[i] = k_up_int_[i];
    }
  }
  cipher_algo = cipher_algo_;
  integ_algo  = integ_algo_;

  log->info("Configuring security with %s and %s\n",
            integrity_algorithm_id_text[integ_algo],
            ciphering_algorithm_id_text[cipher_algo]);

  log->debug_hex(k_rrc_enc, 32, "K_rrc_enc");
  log->debug_hex(k_up_enc, 32, "K_up_enc");
  log->debug_hex(k_rrc_int, 32, "K_rrc_int");
  log->debug_hex(k_up_int, 32, "K_up_int");
}

/****************************************************************************
 * Security functions
 ***************************************************************************/
void pdcp_entity_base::integrity_generate(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac)
{
  uint8_t* k_int;

  // If control plane use RRC integrity key. If data use user plane key
  if (is_srb()) {
    k_int = k_rrc_int;
  } else {
    k_int = k_up_int;
  }

  switch (integ_algo) {
    case INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(&k_int[16], count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(&k_int[16], count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA3:
      security_128_eia3(&k_int[16], count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, mac);
      break;
    default:
      break;
  }

  log->debug("Integrity gen input: COUNT %d, Bearer ID %d, Direction %s\n",
             count,
             cfg.bearer_id,
             (cfg.tx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink"));
  log->debug_hex(msg, msg_len, "Integrity gen input msg:");
  log->debug_hex(mac, 4, "MAC (generated)");
}

bool pdcp_entity_base::integrity_verify(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac)
{
  uint8_t  mac_exp[4] = {};
  bool     is_valid   = true;
  uint8_t* k_int;

  // If control plane use RRC integrity key. If data use user plane key
  if (is_srb()) {
    k_int = k_rrc_int;
  } else {
    k_int = k_up_int;
  }

  switch (integ_algo) {
    case INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(&k_int[16], count, cfg.bearer_id - 1, cfg.rx_direction, msg, msg_len, mac_exp);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(&k_int[16], count, cfg.bearer_id - 1, cfg.rx_direction, msg, msg_len, mac_exp);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA3:
      security_128_eia3(&k_int[16], count, cfg.bearer_id - 1, cfg.rx_direction, msg, msg_len, mac_exp);
      break;
    default:
      break;
  }

  log->debug("Integrity check input: COUNT %d, Bearer ID %d, Direction %s\n",
             count,
             cfg.bearer_id,
             cfg.rx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink");
  log->debug_hex(msg, msg_len, "Integrity check input msg:");

  if (integ_algo != INTEGRITY_ALGORITHM_ID_EIA0) {
    for (uint8_t i = 0; i < 4; i++) {
      if (mac[i] != mac_exp[i]) {
        log->error_hex(mac_exp, 4, "MAC mismatch (expected)");
        log->error_hex(mac, 4, "MAC mismatch (found)");
        is_valid = false;
        break;
      }
    }
    if (is_valid) {
      log->info_hex(mac_exp, 4, "MAC match");
    }
  }

  return is_valid;
}

void pdcp_entity_base::cipher_encrypt(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* ct)
{
  uint8_t* k_enc;
  uint8_t  ct_tmp[PDCP_MAX_SDU_SIZE];

  // If control plane use RRC encrytion key. If data use user plane key
  if (is_srb()) {
    k_enc = k_rrc_enc;
  } else {
    k_enc = k_up_enc;
  }

  log->debug("Cipher encrypt input: COUNT: %d, Bearer ID: %d, Direction %s\n",
             count,
             cfg.bearer_id,
             cfg.tx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink");
  log->debug_hex(msg, msg_len, "Cipher encrypt input msg");

  switch (cipher_algo) {
    case CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&(k_enc[16]), count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, ct_tmp);
      memcpy(ct, ct_tmp, msg_len);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&(k_enc[16]), count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, ct_tmp);
      memcpy(ct, ct_tmp, msg_len);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA3:
      security_128_eea3(&(k_enc[16]), count, cfg.bearer_id - 1, cfg.tx_direction, msg, msg_len, ct_tmp);
      memcpy(ct, ct_tmp, msg_len);
      break;
    default:
      break;
  }
  log->debug_hex(ct, msg_len, "Cipher encrypt output msg");
}

void pdcp_entity_base::cipher_decrypt(uint8_t* ct, uint32_t ct_len, uint32_t count, uint8_t* msg)
{
  uint8_t* k_enc;
  uint8_t  msg_tmp[PDCP_MAX_SDU_SIZE];

  // If control plane use RRC encrytion key. If data use user plane key
  if (is_srb()) {
    k_enc = k_rrc_enc;
  } else {
    k_enc = k_up_enc;
  }

  log->debug("Cipher decrypt input: COUNT: %d, Bearer ID: %d, Direction %s\n",
             count,
             cfg.bearer_id,
             (cfg.rx_direction == SECURITY_DIRECTION_DOWNLINK) ? "Downlink" : "Uplink");
  log->debug_hex(ct, ct_len, "Cipher decrypt input msg");

  switch (cipher_algo) {
    case CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&k_enc[16], count, cfg.bearer_id - 1, cfg.rx_direction, ct, ct_len, msg_tmp);
      memcpy(msg, msg_tmp, ct_len);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&k_enc[16], count, cfg.bearer_id - 1, cfg.rx_direction, ct, ct_len, msg_tmp);
      memcpy(msg, msg_tmp, ct_len);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA3:
      security_128_eea3(&k_enc[16], count, cfg.bearer_id - 1, cfg.rx_direction, ct, ct_len, msg_tmp);
      memcpy(msg, msg_tmp, ct_len);
      break;
    default:
      break;
  }
  log->debug_hex(msg, ct_len, "Cipher decrypt output msg");
}
} // namespace srslte
