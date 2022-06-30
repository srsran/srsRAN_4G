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

#include "srsue/hdr/stack/upper/nas_base.h"

using namespace srsran;
namespace srsue {
nas_base::nas_base(srslog::basic_logger& logger_, uint32_t mac_offset_, uint32_t seq_offset_, uint32_t bearer_id_) :
  logger(logger_), mac_offset(mac_offset_), seq_offset(seq_offset_), bearer_id(bearer_id_)
{}

int nas_base::parse_security_algorithm_list(std::string algorithm_string, bool* algorithm_caps)
{
  // parse and sanity check security algorithm list
  std::vector<uint8_t> cap_list;
  srsran::string_parse_list(algorithm_string, ',', cap_list);
  if (cap_list.empty()) {
    logger.error("Empty security list. Select at least one security algorithm.");
    return SRSRAN_ERROR;
  }
  for (std::vector<uint8_t>::const_iterator it = cap_list.begin(); it != cap_list.end(); ++it) {
    if (*it < 4) {
      algorithm_caps[*it] = true;
    } else {
      logger.error("EEA/EIA/5G-EA/5G-IA %d is not a valid algorithm.", *it);
      return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

/*******************************************************************************
 * Security
 ******************************************************************************/

void nas_base::integrity_generate(uint8_t* key_128,
                                  uint32_t count,
                                  uint8_t  direction,
                                  uint8_t* msg,
                                  uint32_t msg_len,
                                  uint8_t* mac)
{
  switch (ctxt_base.integ_algo) {
    case INTEGRITY_ALGORITHM_ID_EIA0:
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(key_128, count, bearer_id, direction, msg, msg_len, mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(key_128, count, bearer_id, direction, msg, msg_len, mac);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA3:
      security_128_eia3(key_128, count, bearer_id, direction, msg, msg_len, mac);
      break;
    default:
      break;
  }
}

// This function depends to a valid k_nas_int.
// This key is generated in the security mode command.
bool nas_base::integrity_check(byte_buffer_t* pdu)
{
  if (pdu == nullptr) {
    logger.error("Invalid PDU");
    return false;
  }

  if (pdu->N_bytes > seq_offset) {
    uint8_t  exp_mac[4] = {0};
    uint8_t* mac        = &pdu->msg[mac_offset];

    // generate expected MAC
    uint32_t count_est = (ctxt_base.rx_count & 0x00FFFF00u) | pdu->msg[seq_offset];
    integrity_generate(&ctxt_base.k_nas_int[16],
                       count_est,
                       SECURITY_DIRECTION_DOWNLINK,
                       &pdu->msg[seq_offset],
                       pdu->N_bytes - seq_offset,
                       &exp_mac[0]);

    // Check if expected mac equals the sent mac
    for (int i = 0; i < 4; i++) {
      if (exp_mac[i] != mac[i]) {
        logger.warning("Integrity check failure. Local: count=%d, [%02x %02x %02x %02x], "
                       "Received: count=%d, [%02x %02x %02x %02x]",
                       count_est,
                       exp_mac[0],
                       exp_mac[1],
                       exp_mac[2],
                       exp_mac[3],
                       pdu->msg[seq_offset],
                       mac[0],
                       mac[1],
                       mac[2],
                       mac[3]);
        return false;
      }
    }
    logger.info("Integrity check ok. Local: count=%d, Received: count=%d  [%02x %02x %02x %02x]",
                count_est,
                pdu->msg[seq_offset],
                mac[0],
                mac[1],
                mac[2],
                mac[3]);

    // Updated local count (according to TS 24.301 Sec. 4.4.3.3)
    if (count_est != ctxt_base.rx_count) {
      logger.info("Update local count to estimated count %d", count_est);
      ctxt_base.rx_count = count_est;
    }
    return true;
  } else {
    logger.error("Invalid integrity check PDU size (%d)", pdu->N_bytes);
    return false;
  }
}

void nas_base::cipher_encrypt(byte_buffer_t* pdu)
{
  byte_buffer_t pdu_tmp;

  if (ctxt_base.cipher_algo != CIPHERING_ALGORITHM_ID_EEA0) {
    logger.debug("Encrypting PDU. count=%d", ctxt_base.tx_count);
  }

  switch (ctxt_base.cipher_algo) {
    case CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&ctxt_base.k_nas_enc[16],
                        ctxt_base.tx_count,
                        bearer_id,
                        SECURITY_DIRECTION_UPLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &pdu_tmp.msg[seq_offset + 1]);
      memcpy(&pdu->msg[seq_offset + 1], &pdu_tmp.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&ctxt_base.k_nas_enc[16],
                        ctxt_base.tx_count,
                        bearer_id,
                        SECURITY_DIRECTION_UPLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &pdu_tmp.msg[seq_offset + 1]);
      memcpy(&pdu->msg[seq_offset + 1], &pdu_tmp.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA3:
      security_128_eea3(&ctxt_base.k_nas_enc[16],
                        ctxt_base.tx_count,
                        bearer_id,
                        SECURITY_DIRECTION_UPLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &pdu_tmp.msg[seq_offset + 1]);
      memcpy(&pdu->msg[seq_offset + 1], &pdu_tmp.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    default:
      logger.error("Ciphering algorithm not known");
      break;
  }
}

void nas_base::cipher_decrypt(byte_buffer_t* pdu)
{
  byte_buffer_t tmp_pdu;

  uint32_t count_est = (ctxt_base.rx_count & 0x00FFFF00u) | pdu->msg[5];
  if (ctxt_base.cipher_algo != CIPHERING_ALGORITHM_ID_EEA0) {
    logger.debug("Decrypting PDU. Local: count=%d, Received: count=%d", ctxt_base.rx_count, count_est);
  }

  switch (ctxt_base.cipher_algo) {
    case CIPHERING_ALGORITHM_ID_EEA0:
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA1:
      security_128_eea1(&ctxt_base.k_nas_enc[16],
                        count_est,
                        bearer_id,
                        SECURITY_DIRECTION_DOWNLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &tmp_pdu.msg[seq_offset + 1]);
      memcpy(&pdu->msg[seq_offset + 1], &tmp_pdu.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA2:
      security_128_eea2(&ctxt_base.k_nas_enc[16],
                        count_est,
                        bearer_id,
                        SECURITY_DIRECTION_DOWNLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &tmp_pdu.msg[seq_offset + 1]);
      logger.debug(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      memcpy(&pdu->msg[seq_offset + 1], &tmp_pdu.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    case CIPHERING_ALGORITHM_ID_128_EEA3:
      security_128_eea3(&ctxt_base.k_nas_enc[16],
                        count_est,
                        bearer_id,
                        SECURITY_DIRECTION_DOWNLINK,
                        &pdu->msg[seq_offset + 1],
                        pdu->N_bytes - seq_offset + 1,
                        &tmp_pdu.msg[seq_offset + 1]);
      logger.debug(tmp_pdu.msg, pdu->N_bytes, "Decrypted");
      memcpy(&pdu->msg[seq_offset + 1], &tmp_pdu.msg[seq_offset + 1], pdu->N_bytes - seq_offset + 1);
      break;
    default:
      logger.error("Ciphering algorithms not known");
      break;
  }
}

} // namespace srsue