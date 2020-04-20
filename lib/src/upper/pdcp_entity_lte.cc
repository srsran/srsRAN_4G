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

#include "srslte/upper/pdcp_entity_lte.h"
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity_lte::pdcp_entity_lte(srsue::rlc_interface_pdcp*      rlc_,
                                 srsue::rrc_interface_pdcp*      rrc_,
                                 srsue::gw_interface_pdcp*       gw_,
                                 srslte::task_handler_interface* task_executor_,
                                 srslte::log_ref                 log_) :
  pdcp_entity_base(task_executor_, log_),
  rlc(rlc_),
  rrc(rrc_),
  gw(gw_)
{
}

pdcp_entity_lte::~pdcp_entity_lte()
{
  reset();
}

void pdcp_entity_lte::init(uint32_t lcid_, pdcp_config_t cfg_)
{
  lcid                 = lcid_;
  cfg                  = cfg_;
  active               = true;
  tx_count             = 0;
  integrity_direction  = DIRECTION_NONE;
  encryption_direction = DIRECTION_NONE;

  if (is_srb()) {
    reordering_window = 0;
  } else if (is_drb()) {
    reordering_window = 2048;
  }

  rx_hfn                    = 0;
  next_pdcp_rx_sn           = 0;
  maximum_pdcp_sn           = (1 << cfg.sn_len) - 1;
  last_submitted_pdcp_rx_sn = maximum_pdcp_sn;
  log->info("Init %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  log->info("SN len bits: %d, SN len bytes: %d, reordering window: %d, Maximum SN %d\n",
            cfg.sn_len,
            cfg.hdr_len_bytes,
            reordering_window,
            maximum_pdcp_sn);

  // Check supported config
  if (!check_valid_config()) {
    log->console("Warning: Invalid PDCP config.\n");
  }
}

// Reestablishment procedure: 36.323 5.2
void pdcp_entity_lte::reestablish()
{
  log->info("Re-establish %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  // For SRBs
  if (is_srb()) {
    tx_count           = 0;
    rx_hfn             = 0;
    next_pdcp_rx_sn    = 0;
  } else {
    // Only reset counter in RLC-UM
    if (rlc->rb_is_um(lcid)) {
      tx_count           = 0;
      rx_hfn             = 0;
      next_pdcp_rx_sn    = 0;
    }
  }
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_lte::reset()
{
  if (active and log) {
    log->debug("Reset %s\n", rrc->get_rb_name(lcid).c_str());
  }
  active = false;
}

// GW/RRC interface
void pdcp_entity_lte::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  // check for pending security config in transmit direction
  if (enable_security_tx_sn != -1 && enable_security_tx_sn == static_cast<int32_t>(tx_count)) {
    enable_integrity(DIRECTION_TX);
    enable_encryption(DIRECTION_TX);
    enable_security_tx_sn = -1;
  }

  log->info_hex(sdu->msg,
                sdu->N_bytes,
                "TX %s SDU, SN=%d, integrity=%s, encryption=%s",
                rrc->get_rb_name(lcid).c_str(),
                tx_count,
                srslte_direction_text[integrity_direction],
                srslte_direction_text[encryption_direction]);

  write_data_header(sdu, tx_count);

  // Append MAC (SRBs only)
  uint8_t mac[4]       = {};
  bool    do_integrity = integrity_direction == DIRECTION_TX || integrity_direction == DIRECTION_TXRX;
  if (do_integrity && is_srb()) {
    integrity_generate(sdu->msg, sdu->N_bytes, tx_count, mac);
  }

  if (is_srb()) {
    append_mac(sdu, mac);
  }

  if (encryption_direction == DIRECTION_TX || encryption_direction == DIRECTION_TXRX) {
    cipher_encrypt(
        &sdu->msg[cfg.hdr_len_bytes], sdu->N_bytes - cfg.hdr_len_bytes, tx_count, &sdu->msg[cfg.hdr_len_bytes]);
    log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU (encrypted)", rrc->get_rb_name(lcid).c_str());
  }
  tx_count++;

  rlc->write_sdu(lcid, std::move(sdu), blocking);
}

// RLC interface
void pdcp_entity_lte::write_pdu(unique_byte_buffer_t pdu)
{
  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    log->error("PDCP PDU smaller than required header size.\n");
    return;
  }

  // Pull out SN
  uint32_t sn = read_data_header(pdu);

  // check for pending security config in receive direction
  if (enable_security_rx_sn != -1 && enable_security_rx_sn == static_cast<int32_t>(sn)) {
    enable_integrity(DIRECTION_RX);
    enable_encryption(DIRECTION_RX);
    enable_security_rx_sn = -1;
  }

  log->info_hex(pdu->msg,
                pdu->N_bytes,
                "%s Rx PDU SN=%d (%d B, integrity=%s, encryption=%s)",
                rrc->get_rb_name(lcid).c_str(),
                sn,
                pdu->N_bytes,
                srslte_direction_text[integrity_direction],
                srslte_direction_text[encryption_direction]);

  if (is_srb()) {
    handle_srb_pdu(std::move(pdu));
  } else if (is_drb() && rlc->rb_is_um(lcid)) {
    handle_um_drb_pdu(std::move(pdu));
  } else if (is_drb() && !rlc->rb_is_um(lcid)) {
    handle_am_drb_pdu(std::move(pdu));
  } else {
    log->error("Invalid PDCP/RLC configuration");
  }
}

/****************************************************************************
 * Rx data/control handler functions
 * Ref: 3GPP TS 36.323 v10.1.0 Section 5.1.2
 ***************************************************************************/
// SRBs (5.1.2.2)
void pdcp_entity_lte::handle_srb_pdu(srslte::unique_byte_buffer_t pdu)
{
  // Read SN from header
  uint32_t sn = read_data_header(pdu);

  log->debug("RX SRB PDU. Next_PDCP_RX_SN %d, SN %d", next_pdcp_rx_sn, sn);

  // Estimate COUNT for integrity check and decryption
  uint32_t count;
  if (sn < next_pdcp_rx_sn) {
    count = COUNT(rx_hfn + 1, sn);
  } else {
    count = COUNT(rx_hfn, sn);
  }

  // Perform decryption
  if (encryption_direction == DIRECTION_RX || encryption_direction == DIRECTION_TXRX) {
    cipher_decrypt(&pdu->msg[cfg.hdr_len_bytes], pdu->N_bytes - cfg.hdr_len_bytes, count, &pdu->msg[cfg.hdr_len_bytes]);
    log->info_hex(pdu->msg, pdu->N_bytes, "%s Rx PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
  }

  // Extract MAC
  uint8_t mac[4];
  extract_mac(pdu, mac);

  // Perfrom integrity checks
  if (integrity_direction == DIRECTION_RX || integrity_direction == DIRECTION_TXRX) {
    if (not integrity_verify(pdu->msg, pdu->N_bytes, count, mac)) {
      log->error_hex(pdu->msg, pdu->N_bytes, "%s Dropping PDU", rrc->get_rb_name(lcid).c_str());
      return; // Discard
    }
  }

  // Discard header
  discard_data_header(pdu);

  // Update state variables
  if (sn < next_pdcp_rx_sn) {
    rx_hfn++;
  }
  next_pdcp_rx_sn = sn + 1;

  if (next_pdcp_rx_sn > maximum_pdcp_sn) {
    next_pdcp_rx_sn = 0;
    rx_hfn++;
  }

  // Pass to upper layers
  log->debug_hex(pdu->msg, pdu->N_bytes, "Passing SDU to upper layers");
  rrc->write_pdu(lcid, std::move(pdu));
}

// DRBs mapped on RLC UM (5.1.2.1.3)
void pdcp_entity_lte::handle_um_drb_pdu(srslte::unique_byte_buffer_t pdu)
{
  uint32_t sn = read_data_header(pdu);
  discard_data_header(pdu);

  if (sn < next_pdcp_rx_sn) {
    rx_hfn++;
  }

  uint32_t count = (rx_hfn << cfg.sn_len) | sn;
  if (encryption_direction == DIRECTION_RX || encryption_direction == DIRECTION_TXRX) {
    cipher_decrypt(pdu->msg, pdu->N_bytes, count, pdu->msg);
    log->debug_hex(pdu->msg, pdu->N_bytes, "%s Rx PDU (decrypted)", rrc->get_rb_name(lcid).c_str());
  }

  next_pdcp_rx_sn = sn + 1;
  if (next_pdcp_rx_sn > maximum_pdcp_sn) {
    next_pdcp_rx_sn = 0;
    rx_hfn++;
  }

  // Pass to upper layers
  log->info_hex(pdu->msg, pdu->N_bytes, "%s Rx PDU SN=%d", rrc->get_rb_name(lcid).c_str(), sn);
  gw->write_pdu(lcid, std::move(pdu));
}

// DRBs mapped on RLC AM, without re-ordering (5.1.2.1.2)
void pdcp_entity_lte::handle_am_drb_pdu(srslte::unique_byte_buffer_t pdu)
{
  uint32_t sn = read_data_header(pdu);
  discard_data_header(pdu);

  int32_t last_submit_diff_sn     = last_submitted_pdcp_rx_sn - sn;
  int32_t sn_diff_last_submit     = sn - last_submitted_pdcp_rx_sn;
  int32_t sn_diff_next_pdcp_rx_sn = sn - next_pdcp_rx_sn;

  log->debug("RX HFN: %d, SN=%d, Last_Submitted_PDCP_RX_SN=%d, Next_PDCP_RX_SN=%d\n",
             rx_hfn,
             sn,
             last_submitted_pdcp_rx_sn,
             next_pdcp_rx_sn);

  // Handle PDU
  uint32_t count = 0;
  if ((0 <= sn_diff_last_submit && sn_diff_last_submit > (int32_t)reordering_window) ||
      (0 <= last_submit_diff_sn && last_submit_diff_sn < (int32_t)reordering_window)) {
    // discard
    log->debug("Discarding SN=%d (sn_diff_last_submit=%d, last_submit_diff_sn=%d, reordering_window=%d)\n",
               sn,
               sn_diff_last_submit,
               last_submit_diff_sn,
               reordering_window);
    return; // Discard
  } else if ((int32_t)(next_pdcp_rx_sn - sn) > (int32_t)reordering_window) {
    log->debug("(Next_PDCP_RX_SN - SN) is larger than re-ordering window.\n");
    rx_hfn++;
    count           = (rx_hfn << cfg.sn_len) | sn;
    next_pdcp_rx_sn = sn + 1;
  } else if (sn_diff_next_pdcp_rx_sn >= (int32_t)reordering_window) {
    log->debug("(SN - Next_PDCP_RX_SN) is larger or equal than re-ordering window.\n");
    count = ((rx_hfn - 1) << cfg.sn_len) | sn;
  } else if (sn >= next_pdcp_rx_sn) {
    log->debug("SN is larger or equal than Next_PDCP_RX_SN.\n");
    count           = (rx_hfn << cfg.sn_len) | sn;
    next_pdcp_rx_sn = sn + 1;
    if (next_pdcp_rx_sn > maximum_pdcp_sn) {
      next_pdcp_rx_sn = 0;
      rx_hfn++;
    }
  } else if (sn < next_pdcp_rx_sn) {
    log->debug("SN is smaller than Next_PDCP_RX_SN.\n");
    count = (rx_hfn << cfg.sn_len) | sn;
  }

  // Decrypt
  cipher_decrypt(pdu->msg, pdu->N_bytes, count, pdu->msg);
  log->debug_hex(pdu->msg, pdu->N_bytes, "%s Rx PDU (decrypted)", rrc->get_rb_name(lcid).c_str());

  // Update info on last PDU submitted to upper layers
  last_submitted_pdcp_rx_sn = sn;

  // Pass to upper layers
  log->info_hex(pdu->msg, pdu->N_bytes, "%s Rx PDU SN=%d", rrc->get_rb_name(lcid).c_str(), sn);
  gw->write_pdu(lcid, std::move(pdu));
}

/****************************************************************************
 * Security functions
 ***************************************************************************/
void pdcp_entity_lte::get_bearer_status(uint16_t* dlsn, uint16_t* dlhfn, uint16_t* ulsn, uint16_t* ulhfn)
{
  if (cfg.rb_type == PDCP_RB_IS_DRB) {
    if (12 == cfg.sn_len) {
      *dlsn  = (uint16_t)(tx_count & 0xFFFu);
      *dlhfn = (uint16_t)((tx_count - *dlsn) >> 12u);
    } else {
      *dlsn  = (uint16_t)(tx_count & 0x7Fu);
      *dlhfn = (uint16_t)((tx_count - *dlsn) >> 7u);
    }
  } else { // is control
    *dlsn  = (uint16_t)(tx_count & 0x1Fu);
    *dlhfn = (uint16_t)((tx_count - *dlsn) >> 5u);
  }
  *ulsn  = (uint16_t)next_pdcp_rx_sn;
  *ulhfn = (uint16_t)rx_hfn;
}

/****************************************************************************
 * Config checking helper
 ***************************************************************************/
bool pdcp_entity_lte::check_valid_config()
{
  if (cfg.sn_len != PDCP_SN_LEN_5 && cfg.sn_len != PDCP_SN_LEN_7 && cfg.sn_len != PDCP_SN_LEN_12) {
    log->error("Trying to configure bearer with invalid SN LEN=%d\n", cfg.sn_len);
    return false;
  }
  if (cfg.sn_len == PDCP_SN_LEN_5 && is_drb()) {
    log->error("Trying to configure DRB bearer with SN LEN of 5\n");
    return false;
  }
  if (cfg.sn_len == PDCP_SN_LEN_7 && (is_srb() || !rlc->rb_is_um(lcid))) {
    log->error("Trying to configure SRB or RLC AM bearer with SN LEN of 7\n");
    return false;
  }
  if (cfg.sn_len == PDCP_SN_LEN_12 && is_srb()) {
    log->error("Trying to configure SRB with SN LEN of 12.\n");
    return false;
  }
  return true;
}

} // namespace srslte
