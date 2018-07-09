/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/upper/pdcp_entity.h"
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity::pdcp_entity()
  :active(false)
  ,tx_count(0)
{
  pool = byte_buffer_pool::get_instance();
  log = NULL;
  rlc = NULL;
  rrc = NULL;
  gw = NULL;
  lcid = 0;
  sn_len_bytes = 0;
  do_integrity = false;
  do_encryption = false;
  rx_count = 0;
  cipher_algo = CIPHERING_ALGORITHM_ID_EEA0;
  integ_algo = INTEGRITY_ALGORITHM_ID_EIA0;
}

void pdcp_entity::init(srsue::rlc_interface_pdcp      *rlc_,
                       srsue::rrc_interface_pdcp      *rrc_,
                       srsue::gw_interface_pdcp       *gw_,
                       srslte::log                    *log_,
                       uint32_t                       lcid_,
                       srslte_pdcp_config_t           cfg_)
{
  rlc           = rlc_;
  rrc           = rrc_;
  gw            = gw_;
  log           = log_;
  lcid          = lcid_;
  cfg           = cfg_;
  active        = true;
  tx_count      = 0;
  rx_count      = 0;
  do_integrity  = false;
  do_encryption = false;

  cfg.sn_len    = 0;
  sn_len_bytes  = 0;

  if(cfg.is_control) {
    cfg.sn_len   = 5;
    sn_len_bytes = 1;
  }
  if(cfg.is_data) {
    cfg.sn_len   = 12;
    sn_len_bytes = 2;
  }

  log->debug("Init %s\n", rrc->get_rb_name(lcid).c_str());
}

// Reestablishment procedure: 36.323 5.2
void pdcp_entity::reestablish() {
  // For SRBs
  if (cfg.is_control) {
    tx_count = 0;
    rx_count = 0;
  } else {
    if (rlc->rb_is_um(lcid)) {
      tx_count = 0;
      rx_count = 0;
    }
  }
}

void pdcp_entity::reset()
{
  active      = false;
  if(log)
    log->debug("Reset %s\n", rrc->get_rb_name(lcid).c_str());
}

bool pdcp_entity::is_active()
{
  return active;
}

// RRC interface
void pdcp_entity::write_sdu(byte_buffer_t *sdu)
{
  log->info_hex(sdu->msg, sdu->N_bytes,
        "TX %s SDU, SN: %d, do_integrity = %s, do_encryption = %s",
        rrc->get_rb_name(lcid).c_str(), tx_count,
        (do_integrity) ? "true" : "false", (do_encryption) ? "true" : "false");

  if (cfg.is_control) {
    pdcp_pack_control_pdu(tx_count, sdu);
    if(do_integrity) {
      integrity_generate(sdu->msg,
                         sdu->N_bytes-4,
                         &sdu->msg[sdu->N_bytes-4]);
    }
  }

  if (cfg.is_data) {
    if(12 == cfg.sn_len) {
      pdcp_pack_data_pdu_long_sn(tx_count, sdu);
    } else {
      pdcp_pack_data_pdu_short_sn(tx_count, sdu);
    }
  }

  if(do_encryption) {
    cipher_encrypt(&sdu->msg[sn_len_bytes],
                   sdu->N_bytes-sn_len_bytes,
                   &sdu->msg[sn_len_bytes]);
    log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU (encrypted)", rrc->get_rb_name(lcid).c_str());
  }
  tx_count++;

  rlc->write_sdu(lcid, sdu);
}

void pdcp_entity::config_security(uint8_t *k_enc_,
                                  uint8_t *k_int_,
                                  CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                  INTEGRITY_ALGORITHM_ID_ENUM integ_algo_)
{
  for(int i=0; i<32; i++)
  {
    k_enc[i] = k_enc_[i];
    k_int[i] = k_int_[i];
  }
  cipher_algo = cipher_algo_;
  integ_algo  = integ_algo_;
}

void pdcp_entity::enable_integrity()
{
  do_integrity = true;
}

void pdcp_entity::enable_encryption()
{
  do_encryption = true;
}

// RLC interface
void pdcp_entity::write_pdu(byte_buffer_t *pdu)
{
  log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU, do_integrity = %s, do_encryption = %s",
                rrc->get_rb_name(lcid).c_str(), (do_integrity) ? "true" : "false", (do_encryption) ? "true" : "false");

  // Sanity check
  if(pdu->N_bytes <= sn_len_bytes) {
    pool->deallocate(pdu);
    return;
  }

  // Handle DRB messages
  if (cfg.is_data) {
    uint32_t sn;
    if (do_encryption) {
      cipher_decrypt(&(pdu->msg[sn_len_bytes]),
                     rx_count,
                     pdu->N_bytes - sn_len_bytes,
                     &(pdu->msg[sn_len_bytes]));
      log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
    }
    if(12 == cfg.sn_len)
    {
      pdcp_unpack_data_pdu_long_sn(pdu, &sn);
    } else {
      pdcp_unpack_data_pdu_short_sn(pdu, &sn);
    }
    log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU SN: %d", rrc->get_rb_name(lcid).c_str(), sn);
    gw->write_pdu(lcid, pdu);
  } else {
    // Handle SRB messages
    if (cfg.is_control) {
      uint32_t sn;
      if (do_encryption) {
        cipher_decrypt(&(pdu->msg[sn_len_bytes]),
                       rx_count,
                       pdu->N_bytes - sn_len_bytes,
                       &(pdu->msg[sn_len_bytes]));
        log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
      }

      if (do_integrity) {
        integrity_verify(pdu->msg,
                         rx_count,
                         pdu->N_bytes - 4,
                         &(pdu->msg[pdu->N_bytes - 4]));
      }

      pdcp_unpack_control_pdu(pdu, &sn);
      log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU SN: %d", rrc->get_rb_name(lcid).c_str(), sn);
    }
    // pass to RRC
    rrc->write_pdu(lcid, pdu);
  }
  rx_count++;
}

void pdcp_entity::integrity_generate( uint8_t  *msg,
                                      uint32_t  msg_len,
                                      uint8_t  *mac)
{
  uint8_t bearer;

  switch(integ_algo)
  {
  case INTEGRITY_ALGORITHM_ID_EIA0:
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA1:
    security_128_eia1(&k_int[16],
                      tx_count,
                      get_bearer_id(lcid),
                      cfg.direction,
                      msg,
                      msg_len,
                      mac);
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    security_128_eia2(&k_int[16],
                      tx_count,
                      get_bearer_id(lcid),
                      cfg.direction,
                      msg,
                      msg_len,
                      mac);
    break;
  default:
    break;
  }
}

bool pdcp_entity::integrity_verify(uint8_t  *msg,
                                   uint32_t  count,
                                   uint32_t  msg_len,
                                   uint8_t  *mac)
{
  uint8_t mac_exp[4] = {0x00};
  uint8_t i = 0;
  bool isValid = true;

  switch(integ_algo)
  {
  case INTEGRITY_ALGORITHM_ID_EIA0:
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA1:
    security_128_eia1(&k_int[16],
                      count,
                      get_bearer_id(lcid),
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      msg,
                      msg_len,
                      mac_exp);
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    security_128_eia2(&k_int[16],
                      count,
                      get_bearer_id(lcid),
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      msg,
                      msg_len,
                      mac_exp);
    break;
  default:
    break;
  }

  switch(integ_algo)
  {
  case INTEGRITY_ALGORITHM_ID_EIA0:
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA1: // Intentional fall-through
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    for(i=0; i<4; i++){
      if(mac[i] != mac_exp[i]){
        log->error_hex(mac_exp, 4, "MAC mismatch (expected)");
        log->error_hex(mac,     4, "MAC mismatch (found)");
        isValid = false;
        break;
      }
    }
    if (isValid){
      log->info_hex(mac_exp, 4, "MAC match (expected)");
      log->info_hex(mac,     4, "MAC match (found)");
    }
    break;
  default:
    break;
  }

  return isValid;
}

void pdcp_entity::cipher_encrypt(uint8_t  *msg,
                                 uint32_t  msg_len,
                                 uint8_t  *ct)
{
  byte_buffer_t ct_tmp;
  switch(cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
    security_128_eea1(&(k_enc[16]),
                      tx_count,
                      get_bearer_id(lcid),
                      cfg.direction,
                      msg,
                      msg_len,
                      ct_tmp.msg);
    memcpy(ct, ct_tmp.msg, msg_len);
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
    security_128_eea2(&(k_enc[16]),
                      tx_count,
                      get_bearer_id(lcid),
                      cfg.direction,
                      msg,
                      msg_len,
                      ct_tmp.msg);
    memcpy(ct, ct_tmp.msg, msg_len);
    break;
  default:
    break;
  }
}

void pdcp_entity::cipher_decrypt(uint8_t  *ct,
                                 uint32_t  count,
                                 uint32_t  ct_len,
                                 uint8_t  *msg)
{
  byte_buffer_t msg_tmp;
  switch(cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
    security_128_eea1(&(k_enc[16]),
                      count,
                      get_bearer_id(lcid),
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      ct,
                      ct_len,
                      msg_tmp.msg);
    memcpy(msg, msg_tmp.msg, ct_len);
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
    security_128_eea2(&(k_enc[16]),
                      count,
                      get_bearer_id(lcid),
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      ct,
                      ct_len,
                      msg_tmp.msg);
    memcpy(msg, msg_tmp.msg, ct_len);
    break;
  default:
    break;
  }
}


uint8_t pdcp_entity::get_bearer_id(uint8_t lcid)
{
  #define RB_ID_SRB2 2
  if(lcid <= RB_ID_SRB2) {
    return lcid - 1;
  } else {
    return lcid - RB_ID_SRB2 - 1;
  }
}


/****************************************************************************
 * Pack/Unpack helper functions
 * Ref: 3GPP TS 36.323 v10.1.0
 ***************************************************************************/

void pdcp_pack_control_pdu(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg--;
  sdu->N_bytes++;
  *sdu->msg = sn & 0x1F;

  // Add MAC
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >> 24) & 0xFF;
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >> 16) & 0xFF;
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >>  8) & 0xFF;
  sdu->msg[sdu->N_bytes++] =  PDCP_CONTROL_MAC_I        & 0xFF;

}

void pdcp_unpack_control_pdu(byte_buffer_t *pdu, uint32_t *sn)
{
  // Strip header
  *sn = *pdu->msg & 0x1F;
  pdu->msg++;
  pdu->N_bytes--;

  // Strip MAC
  pdu->N_bytes -= 4;

  // TODO: integrity check MAC
}

void pdcp_pack_data_pdu_short_sn(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg--;
  sdu->N_bytes++;
  sdu->msg[0] = (PDCP_D_C_DATA_PDU << 7) | (sn & 0x7F);
}

void pdcp_unpack_data_pdu_short_sn(byte_buffer_t *sdu, uint32_t *sn)
{
  // Strip header
  *sn  = sdu->msg[0] & 0x7F;
  sdu->msg++;
  sdu->N_bytes--;
}

void pdcp_pack_data_pdu_long_sn(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg     -= 2;
  sdu->N_bytes += 2;
  sdu->msg[0] = (PDCP_D_C_DATA_PDU << 7) | ((sn >> 8) & 0x0F);
  sdu->msg[1] = sn & 0xFF;
}

void pdcp_unpack_data_pdu_long_sn(byte_buffer_t *sdu, uint32_t *sn)
{
  // Strip header
  *sn  = (sdu->msg[0] & 0x0F) << 8;
  *sn |= sdu->msg[1];
  sdu->msg     += 2;
  sdu->N_bytes -= 2;
}

}
