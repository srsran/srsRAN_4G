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

#include "srsran/upper/pdcp_entity_base.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/security.h"
#include <inttypes.h>

namespace srsran {

pdcp_entity_base::pdcp_entity_base(task_sched_handle task_sched_, srslog::basic_logger& logger) :
  logger(logger), task_sched(task_sched_)
{}

pdcp_entity_base::~pdcp_entity_base() {}

void pdcp_entity_base::config_security(const as_security_config_t& sec_cfg_)
{
  sec_cfg = sec_cfg_;

  logger.info("Configuring security with %s and %s",
              integrity_algorithm_id_text[sec_cfg.integ_algo],
              ciphering_algorithm_id_text[sec_cfg.cipher_algo]);

  logger.debug(sec_cfg.k_rrc_enc.data(), 32, "K_rrc_enc");
  logger.debug(sec_cfg.k_up_enc.data(), 32, "K_up_enc");
  logger.debug(sec_cfg.k_rrc_int.data(), 32, "K_rrc_int");
  logger.debug(sec_cfg.k_up_int.data(), 32, "K_up_int");
}

/****************************************************************************
 * Security functions
 ***************************************************************************/
void pdcp_entity_base::integrity_generate(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac)
{
  uint8_t* k_int;

  // If control plane use RRC integrity key. If data use user plane key
  if (is_srb()) {
    k_int = sec_cfg.k_rrc_int.data();
  } else {
    k_int = sec_cfg.k_up_int.data();
  }

  switch (sec_cfg.integ_algo) {
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

  logger.debug("Integrity gen input: COUNT %" PRIu32 ", Bearer ID %d, Direction %s",
               count,
               cfg.bearer_id,
               (cfg.tx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink"));
  logger.debug(k_int, 32, "Integrity gen key:");
  logger.debug(msg, msg_len, "Integrity gen input msg:");
  logger.debug(mac, 4, "MAC (generated)");
}

bool pdcp_entity_base::integrity_verify(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac)
{
  uint8_t  mac_exp[4] = {};
  bool     is_valid   = true;
  uint8_t* k_int;

  // If control plane use RRC integrity key. If data use user plane key
  if (is_srb()) {
    k_int = sec_cfg.k_rrc_int.data();
  } else {
    k_int = sec_cfg.k_up_int.data();
  }

  switch (sec_cfg.integ_algo) {
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

  if (sec_cfg.integ_algo != INTEGRITY_ALGORITHM_ID_EIA0) {
    for (uint8_t i = 0; i < 4; i++) {
      if (mac[i] != mac_exp[i]) {
        logger.error(mac_exp, 4, "MAC mismatch (expected)");
        logger.error(mac, 4, "MAC mismatch (found)");
        is_valid = false;
        break;
      }
    }
    srslog::log_channel& channel = is_valid ? logger.debug : logger.warning;
    channel("Integrity check input: COUNT %" PRIu32 ", Bearer ID %d, Direction %s",
            count,
            cfg.bearer_id,
            cfg.rx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink");
    channel(k_int, 32, "Integrity check key:");
    channel(msg, msg_len, "Integrity check input msg (Bytes=%" PRIu32 "):", msg_len);
  }

  return is_valid;
}

void pdcp_entity_base::cipher_encrypt(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* ct)
{
  uint8_t* k_enc;
  uint8_t  ct_tmp[PDCP_MAX_SDU_SIZE];

  // If control plane use RRC encrytion key. If data use user plane key
  if (is_srb()) {
    k_enc = sec_cfg.k_rrc_enc.data();
  } else {
    k_enc = sec_cfg.k_up_enc.data();
  }

  logger.debug("Cipher encrypt input: COUNT: %" PRIu32 ", Bearer ID: %d, Direction %s",
               count,
               cfg.bearer_id,
               cfg.tx_direction == SECURITY_DIRECTION_DOWNLINK ? "Downlink" : "Uplink");
  logger.debug(k_enc, 32, "Cipher encrypt key:");
  logger.debug(msg, msg_len, "Cipher encrypt input msg");

  switch (sec_cfg.cipher_algo) {
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
  logger.debug(ct, msg_len, "Cipher encrypt output msg");
}

void pdcp_entity_base::cipher_decrypt(uint8_t* ct, uint32_t ct_len, uint32_t count, uint8_t* msg)
{
  uint8_t* k_enc;
  uint8_t  msg_tmp[PDCP_MAX_SDU_SIZE];

  // If control plane use RRC encrytion key. If data use user plane key
  if (is_srb()) {
    k_enc = sec_cfg.k_rrc_enc.data();
  } else {
    k_enc = sec_cfg.k_up_enc.data();
  }

  logger.debug("Cipher decrypt input: COUNT: %" PRIu32 ", Bearer ID: %d, Direction %s",
               count,
               cfg.bearer_id,
               (cfg.rx_direction == SECURITY_DIRECTION_DOWNLINK) ? "Downlink" : "Uplink");
  logger.debug(k_enc, 32, "Cipher decrypt key:");
  logger.debug(ct, ct_len, "Cipher decrypt input msg");

  switch (sec_cfg.cipher_algo) {
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
  logger.debug(msg, ct_len, "Cipher decrypt output msg");
}

/****************************************************************************
 * Common pack functions
 ***************************************************************************/

bool pdcp_entity_base::is_control_pdu(const unique_byte_buffer_t& pdu)
{
  const uint8_t* payload = pdu->msg;
  return ((*(payload) >> 7) & 0x01) == PDCP_DC_FIELD_CONTROL_PDU;
}

pdcp_pdu_type_t pdcp_entity_base::get_control_pdu_type(const unique_byte_buffer_t& pdu)
{
  return pdcp_pdu_type_t((pdu->msg[0] >> 4U) & 0x03U);
}

uint32_t pdcp_entity_base::read_data_header(const unique_byte_buffer_t& pdu)
{
  // Check PDU is long enough to extract header
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    logger.error("PDU too small to extract header");
    return 0;
  }

  // Extract RCVD_SN
  uint16_t rcvd_sn_16 = 0;
  uint32_t rcvd_sn_32 = 0;
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_5:
      rcvd_sn_32 = SN(pdu->msg[0]);
      break;
    case PDCP_SN_LEN_7:
      rcvd_sn_32 = SN(pdu->msg[0]);
      break;
    case PDCP_SN_LEN_12:
      srsran::uint8_to_uint16(pdu->msg, &rcvd_sn_16);
      rcvd_sn_32 = SN(rcvd_sn_16);
      break;
    case PDCP_SN_LEN_18:
      srsran::uint8_to_uint24(pdu->msg, &rcvd_sn_32);
      rcvd_sn_32 = SN(rcvd_sn_32);
      break;
    default:
      logger.error("Cannot extract RCVD_SN, invalid SN length configured: %d", cfg.sn_len);
  }

  return rcvd_sn_32;
}

void pdcp_entity_base::discard_data_header(const unique_byte_buffer_t& pdu)
{
  pdu->msg += cfg.hdr_len_bytes;
  pdu->N_bytes -= cfg.hdr_len_bytes;
}

void pdcp_entity_base::write_data_header(const srsran::unique_byte_buffer_t& sdu, uint32_t count)
{
  // Add room for header
  if (cfg.hdr_len_bytes > sdu->get_headroom()) {
    logger.error("Not enough space to add header");
    return;
  }
  sdu->msg -= cfg.hdr_len_bytes;
  sdu->N_bytes += cfg.hdr_len_bytes;

  // Add SN
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_5:
      sdu->msg[0] = SN(count); // Data PDU and SN LEN 5 implies SRB, D flag must not be present
      break;
    case PDCP_SN_LEN_7:
      sdu->msg[0] = SN(count);
      if (is_drb()) {
        sdu->msg[0] |= 0x80; // On Data PDUs for DRBs we must set the D flag.
      }
      break;
    case PDCP_SN_LEN_12:
      srsran::uint16_to_uint8(SN(count), sdu->msg);
      if (is_drb()) {
        sdu->msg[0] |= 0x80; // On Data PDUs for DRBs we must set the D flag.
      }
      break;
    case PDCP_SN_LEN_18:
      srsran::uint24_to_uint8(SN(count), sdu->msg);
      sdu->msg[0] |= 0x80; // Data PDU and SN LEN 18 implies DRB, D flag must be present
      break;
    default:
      logger.error("Invalid SN length configuration: %d bits", cfg.sn_len);
  }
}

void pdcp_entity_base::extract_mac(const unique_byte_buffer_t& pdu, uint8_t* mac)
{
  // Check enough space for MAC
  if (pdu->N_bytes < 4) {
    logger.error("PDU too small to extract MAC-I");
    return;
  }

  // Extract MAC
  memcpy(mac, &pdu->msg[pdu->N_bytes - 4], 4);
  pdu->N_bytes -= 4;
}

void pdcp_entity_base::append_mac(const unique_byte_buffer_t& sdu, uint8_t* mac)
{
  // Check enough space for MAC
  if (sdu->N_bytes + 4 > sdu->get_tailroom()) {
    logger.error("Not enough space to add MAC-I");
    return;
  }

  // Append MAC
  memcpy(&sdu->msg[sdu->N_bytes], mac, 4);
  sdu->N_bytes += 4;
}
} // namespace srsran
