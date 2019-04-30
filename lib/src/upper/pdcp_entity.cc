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
  tx_count = 0;
  rx_count = 0;
  rx_hfn = 0;
  next_pdcp_rx_sn = 0;
  cipher_algo = CIPHERING_ALGORITHM_ID_EEA0;
  integ_algo = INTEGRITY_ALGORITHM_ID_EIA0;
  pthread_mutex_init(&mutex, NULL);
}

pdcp_entity::~pdcp_entity()
{
  pthread_mutex_destroy(&mutex);
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

  cfg = cfg_;

  // set length of SN field in bytes
  sn_len_bytes = (cfg.sn_len == 5) ? 1 : 2;

  if (cfg.is_control) {
    reordering_window = 0;
  } else if (cfg.is_data) {
    reordering_window = 2048;
  }

  rx_hfn          = 0;
  next_pdcp_rx_sn = 0;
  maximum_pdcp_sn = (1 << cfg.sn_len) - 1;
  printf("Maximum PDCP SN %d\n", maximum_pdcp_sn);
  last_submitted_pdcp_rx_sn = maximum_pdcp_sn;
  log->info("Init %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
}

// Reestablishment procedure: 36.323 5.2
void pdcp_entity::reestablish()
{
  log->info("Re-establish %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  // For SRBs
  if (cfg.is_control) {
    tx_count        = 0;
    rx_count        = 0;
    rx_hfn          = 0;
    next_pdcp_rx_sn = 0;
  } else {
    // Only reset counter in RLC-UM
    if (rlc->rb_is_um(lcid)) {
      tx_count        = 0;
      rx_count        = 0;
      rx_hfn          = 0;
      next_pdcp_rx_sn = 0;
    }
  }
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity::reset()
{
  active = false;
  if (log) {
    log->debug("Reset %s\n", rrc->get_rb_name(lcid).c_str());
  }
}

bool pdcp_entity::is_active()
{
  return active;
}

// GW/RRC interface
void pdcp_entity::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  log->info_hex(sdu->msg, sdu->N_bytes,
        "TX %s SDU, SN: %d, do_integrity = %s, do_encryption = %s",
        rrc->get_rb_name(lcid).c_str(), tx_count,
        (do_integrity) ? "true" : "false", (do_encryption) ? "true" : "false");

  pthread_mutex_lock(&mutex);

  if (cfg.is_control) {
    pdcp_pack_control_pdu(tx_count, sdu.get());
    if(do_integrity) {
      integrity_generate(sdu->msg,
                         sdu->N_bytes-4,
                         &sdu->msg[sdu->N_bytes-4]);
    }
  }

  if (cfg.is_data) {
    if(12 == cfg.sn_len) {
      pdcp_pack_data_pdu_long_sn(tx_count, sdu.get());
    } else {
      pdcp_pack_data_pdu_short_sn(tx_count, sdu.get());
    }
  }

  if(do_encryption) {
    cipher_encrypt(&sdu->msg[sn_len_bytes],
                   sdu->N_bytes-sn_len_bytes,
                   &sdu->msg[sn_len_bytes]);
    log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU (encrypted)", rrc->get_rb_name(lcid).c_str());
  }
  tx_count++;

  pthread_mutex_unlock(&mutex);

  rlc->write_sdu(lcid, std::move(sdu), blocking);
}

void pdcp_entity::config_security(uint8_t *k_rrc_enc_,
                                  uint8_t *k_rrc_int_,
                                  uint8_t *k_up_enc_,
                                  CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                  INTEGRITY_ALGORITHM_ID_ENUM integ_algo_)
{
  for(int i=0; i<32; i++)
  {
    k_rrc_enc[i] = k_rrc_enc_[i];
    k_rrc_int[i] = k_rrc_int_[i];
    k_up_enc[i] = k_up_enc_[i];
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
void pdcp_entity::write_pdu(unique_byte_buffer_t pdu)
{
  log->info_hex(pdu->msg,
                pdu->N_bytes,
                "RX %s PDU (%d B), do_integrity = %s, do_encryption = %s",
                rrc->get_rb_name(lcid).c_str(),
                pdu->N_bytes,
                (do_integrity) ? "true" : "false",
                (do_encryption) ? "true" : "false");

  // Sanity check
  if (pdu->N_bytes <= sn_len_bytes) {
    return;
  }

  pthread_mutex_lock(&mutex);

  if (cfg.is_data) {
    // Handle DRB messages
    if (rlc->rb_is_um(lcid)) {
      handle_um_drb_pdu(pdu);
    } else {
      handle_am_drb_pdu(pdu);
    }
    gw->write_pdu(lcid, pdu);
  } else {
    // Handle SRB messages
    if (cfg.is_control) {
      uint32_t sn = *pdu->msg & 0x1F;
      if (do_encryption) {
        cipher_decrypt(&pdu->msg[sn_len_bytes],
                       sn,
                       pdu->N_bytes - sn_len_bytes,
                       &(pdu->msg[sn_len_bytes]));
        log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
      }

      if (do_integrity) {
        if (not integrity_verify(pdu->msg,
                         sn,
                         pdu->N_bytes - 4,
                         &(pdu->msg[pdu->N_bytes - 4]))) {
         log->error_hex(pdu->msg, pdu->N_bytes, "%s Dropping PDU", rrc->get_rb_name(lcid).c_str());
          goto exit;
        }
      }

      pdcp_unpack_control_pdu(pdu.get(), &sn);
      log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU SN: %d", rrc->get_rb_name(lcid).c_str(), sn);
    }
    // pass to RRC
    rrc->write_pdu(lcid, std::move(pdu));
  }
exit:
  rx_count++;
  pthread_mutex_unlock(&mutex);
}

/****************************************************************************
 * Rx data/control handler functions
 * Ref: 3GPP TS 36.323 v10.1.0 Section 5.1.2
 ***************************************************************************/
// DRBs mapped on RLC UM (5.1.2.1.3)
void pdcp_entity::handle_um_drb_pdu(srslte::byte_buffer_t* pdu)
{
  uint32_t sn;
  if (12 == cfg.sn_len) {
    pdcp_unpack_data_pdu_long_sn(pdu, &sn);
  } else {
    pdcp_unpack_data_pdu_short_sn(pdu, &sn);
  }

  if (sn < next_pdcp_rx_sn) {
    rx_hfn++;
  }

  uint32_t count = (rx_hfn << cfg.sn_len) | sn;
  if (do_encryption) {
    cipher_decrypt(pdu->msg, count, pdu->N_bytes, pdu->msg);
    log->debug_hex(pdu->msg, pdu->N_bytes, "RX %s PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
  }

  next_pdcp_rx_sn = sn + 1;
  if (next_pdcp_rx_sn > maximum_pdcp_sn) {
    next_pdcp_rx_sn = 0;
    rx_hfn++;
  }

  log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU SN: %d", rrc->get_rb_name(lcid).c_str(), sn);
  return;
}

// DRBs mapped on RLC AM, without re-ordering (5.1.2.1.2)
void pdcp_entity::handle_am_drb_pdu(srslte::byte_buffer_t* pdu)
{
  uint32_t sn, count;
  pdcp_unpack_data_pdu_long_sn(pdu, &sn);

  int32_t last_submit_diff_sn     = last_submitted_pdcp_rx_sn - sn;
  int32_t sn_diff_last_submit     = sn - last_submitted_pdcp_rx_sn;
  int32_t sn_diff_next_pdcp_rx_sn = sn - next_pdcp_rx_sn;

  log->debug("RX HFN: %d, SN: %d, Last_Submitted_PDCP_RX_SN: %d, Next_PDCP_RX_SN %d\n",
             rx_hfn,
             sn,
             last_submitted_pdcp_rx_sn,
             next_pdcp_rx_sn);

  if ((0 <= sn_diff_last_submit && sn_diff_last_submit > (int32_t)reordering_window) ||
      (0 <= last_submit_diff_sn && last_submit_diff_sn > (int32_t)reordering_window)) {
    log->debug("|SN - last_submitted_sn| is larger than re-ordering window.\n");
    if (sn > next_pdcp_rx_sn) {
      count = (rx_hfn - 1) << cfg.sn_len | sn;
    } else {
      count = rx_hfn << cfg.sn_len | sn;
    }
  } else if ((int32_t)(next_pdcp_rx_sn - sn) > (int32_t)reordering_window) {
    log->debug("(Next_PDCP_RX_SN - SN) is larger than re-ordering window.\n");
    rx_hfn++;
    count = (rx_hfn << cfg.sn_len) | sn;
    next_pdcp_rx_sn = sn + 1;
  } else if (sn_diff_next_pdcp_rx_sn >= (int32_t)reordering_window) {
    log->debug("(SN - Next_PDCP_RX_SN) is larger or equal than re-ordering window.\n");
    count = ((rx_hfn - 1) << cfg.sn_len) | sn;
  } else if (sn >= next_pdcp_rx_sn) {
    log->debug("SN is larger or equal than Next_PDCP_RX_SN.\n");
    count = (rx_hfn << cfg.sn_len) | sn;
    next_pdcp_rx_sn = sn + 1;
    if (next_pdcp_rx_sn > maximum_pdcp_sn) {
      next_pdcp_rx_sn = 0;
      rx_hfn++;
    }
  } else if (sn < next_pdcp_rx_sn) {
    log->debug("SN is smaller than Next_PDCP_RX_SN.\n");
    count = (rx_hfn << cfg.sn_len) | sn;
  }

  // FIXME Check if PDU is not due to re-establishment of lower layers?
  cipher_decrypt(pdu->msg, count, pdu->N_bytes, pdu->msg);
  log->debug_hex(pdu->msg, pdu->N_bytes, "RX %s PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
  last_submitted_pdcp_rx_sn = sn; 
  return;
  }

/****************************************************************************
 * Security functions
 ***************************************************************************/
void pdcp_entity::integrity_generate( uint8_t  *msg,
                                      uint32_t  msg_len,
                                      uint8_t  *mac)
{
  switch(integ_algo)
  {
  case INTEGRITY_ALGORITHM_ID_EIA0:
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA1:
    security_128_eia1(&k_rrc_int[16],
                      tx_count,
                      cfg.bearer_id - 1,
                      cfg.direction,
                      msg,
                      msg_len,
                      mac);
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    security_128_eia2(&k_rrc_int[16],
                      tx_count,
                      cfg.bearer_id - 1,
                      cfg.direction,
                      msg,
                      msg_len,
                      mac);
    break;
  default:
    break;
  }

  log->debug("Integrity gen input:\n");
  log->debug_hex(&k_rrc_int[16], 16, "  K_rrc_int");
  log->debug("  Local count: %d\n", tx_count);
  log->debug("  Bearer ID: %d\n", cfg.bearer_id);
  log->debug("  Direction: %s\n", (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? "Downlink" : "Uplink");
  log->debug_hex(msg, msg_len, "  Message");
  log->debug_hex(mac,     4, "MAC (generated)");
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
    security_128_eia1(&k_rrc_int[16],
                      count,
                      cfg.bearer_id - 1,
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      msg,
                      msg_len,
                      mac_exp);
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    security_128_eia2(&k_rrc_int[16],
                      count,
                      cfg.bearer_id - 1,
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      msg,
                      msg_len,
                      mac_exp);
    break;
  default:
    break;
  }

  log->debug("Integrity check input:\n");
  log->debug_hex(&k_rrc_int[16], 16, "  K_rrc_int");
  log->debug("  Local count: %d\n", count);
  log->debug("  Bearer ID: %d\n", cfg.bearer_id);
  log->debug("  Direction: %s\n", (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? "Uplink" : "Downlink");
  log->debug_hex(msg, msg_len, "  Message");

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
      log->info_hex(mac_exp, 4, "MAC match");
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
  uint8_t *k_enc;

  // If control plane use RRC encrytion key. If data use user plane key
  if (cfg.is_control) {
    k_enc = k_rrc_enc;
  } else {
    k_enc = k_up_enc;
  }

  log->debug("Cipher encript input:\n");
  log->debug_hex(&k_enc[16], 16, "  K_enc");
  log->debug("  Local count: %d\n", tx_count);
  log->debug("  TX HFN: %d COUNT %d\n", (tx_count >> cfg.sn_len), (tx_count << (32-cfg.sn_len)) >> (32-cfg.sn_len));
  log->debug("  Bearer ID: %d\n", cfg.bearer_id);
  log->debug("  Direction: %s\n", (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? "Downlink" : "Uplink");
  log->debug("Encripting COUNT %d \n", tx_count);

  switch(cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
    security_128_eea1(&(k_enc[16]),
                      tx_count,
                      cfg.bearer_id - 1,
                      cfg.direction,
                      msg,
                      msg_len,
                      ct_tmp.msg);
    memcpy(ct, ct_tmp.msg, msg_len);
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
    security_128_eea2(&(k_enc[16]),
                      tx_count,
                      cfg.bearer_id - 1,
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
  uint8_t *k_enc;
  // If control plane use RRC encrytion key. If data use user plane key
  if (cfg.is_control) {
    k_enc = k_rrc_enc;
  } else {
    k_enc = k_up_enc;
  }
  log->info("Decripting COUNT %d \n", count);
  switch(cipher_algo)
  {
  case CIPHERING_ALGORITHM_ID_EEA0:
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA1:
    security_128_eea1(&(k_enc[16]),
                      count,
                      cfg.bearer_id - 1,
                      (cfg.direction == SECURITY_DIRECTION_DOWNLINK) ? (SECURITY_DIRECTION_UPLINK) : (SECURITY_DIRECTION_DOWNLINK),
                      ct,
                      ct_len,
                      msg_tmp.msg);
    memcpy(msg, msg_tmp.msg, ct_len);
    break;
  case CIPHERING_ALGORITHM_ID_128_EEA2:
    security_128_eea2(&(k_enc[16]),
                      count,
                      cfg.bearer_id - 1,
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


uint32_t pdcp_entity::get_dl_count()
{
  return rx_count;
}


uint32_t pdcp_entity::get_ul_count()
{
  return tx_count;
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
