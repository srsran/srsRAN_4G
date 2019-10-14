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

#include "srslte/upper/pdcp_entity_nr.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity_nr::pdcp_entity_nr() {}

pdcp_entity_nr::~pdcp_entity_nr() {}

void pdcp_entity_nr::init(srsue::rlc_interface_pdcp* rlc_,
                          srsue::rrc_interface_pdcp* rrc_,
                          srsue::gw_interface_pdcp*  gw_,
                          srslte::log*               log_,
                          uint32_t                   lcid_,
                          pdcp_config_t              cfg_)
{
  rlc           = rlc_;
  rrc           = rrc_;
  gw            = gw_;
  log           = log_;
  lcid          = lcid_;
  cfg           = cfg_;
  active        = true;
  do_integrity  = false;
  do_encryption = false;

  window_size = 1 << (cfg.sn_len - 1);
}

// Reestablishment procedure: 38.323 5.2
void pdcp_entity_nr::reestablish()
{
  log->info("Re-establish %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  // TODO
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_nr::reset()
{
  active = false;
  if (log) {
    log->debug("Reset %s\n", rrc->get_rb_name(lcid).c_str());
  }
}

// SDAP/RRC interface
void pdcp_entity_nr::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  log->info_hex(sdu->msg,
                sdu->N_bytes,
                "TX %s SDU, do_integrity = %s, do_encryption = %s",
                rrc->get_rb_name(lcid).c_str(),
                (do_integrity) ? "true" : "false",
                (do_encryption) ? "true" : "false");

  // Start discard timer TODO
  // Perform header compression TODO

  // Integrity protection
  uint8_t mac[4];
  integrity_generate(sdu->msg, sdu->N_bytes, tx_next, mac);

  // Ciphering
  cipher_encrypt(sdu->msg, sdu->N_bytes, tx_next, sdu->msg);

  // Write PDCP header info
  write_data_header(sdu, tx_next);

  // Append MAC-I
  append_mac(sdu, mac);

  // Increment TX_NEXT
  tx_next++;

  // Check if PDCP is associated with more than on RLC entity TODO
  // Write to lower layers
  rlc->write_sdu(lcid, std::move(sdu), blocking);
}

// RLC interface
void pdcp_entity_nr::write_pdu(unique_byte_buffer_t pdu)
{
  log->info_hex(pdu->msg,
                pdu->N_bytes,
                "RX %s PDU (%d B), do_integrity = %s, do_encryption = %s",
                rrc->get_rb_name(lcid).c_str(),
                pdu->N_bytes,
                (do_integrity) ? "true" : "false",
                (do_encryption) ? "true" : "false");

  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    return;
  }

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = read_data_header(pdu);

  // Extract MAC
  uint8_t mac[4];
  extract_mac(pdu, mac);

  // Calculate RCVD_COUNT
  uint32_t rcvd_hfn, rcvd_count;
  if ((int64_t)rcvd_sn < (int64_t)SN(rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(rx_deliv) + 1;
  } else if (rcvd_sn >= SN(rx_deliv) + window_size) {
    rcvd_hfn = HFN(rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, rcvd_sn);

  // Decripting
  cipher_decrypt(pdu->msg, pdu->N_bytes, rcvd_count, pdu->msg);

  // Integrity check
  bool is_valid = integrity_verify(pdu->msg, pdu->N_bytes, rcvd_count, mac);
  if (!is_valid) {
    return; // Invalid packet, drop.
  }

  // Check valid rcvd_count
  if (rcvd_count < rx_deliv) {
    return; // Invalid count, drop.
  }

  // TODO check if PDU has been received
  // TODO Store PDU in reception buffer

  // Update RX_NEXT
  if (rcvd_count >= rx_next) {
    rx_next = rcvd_count + 1;
  }

  // TODO if out-of-order configured, submit to upper layer

  if (rcvd_count == rx_deliv) {
    // Deliver to upper layers (TODO queueing needs to be implemented)
    if (is_srb()) {
      rrc->write_pdu(lcid, std::move(pdu));
    } else {
      gw->write_pdu(lcid, std::move(pdu));
    }

    // Update RX_DELIV
    rx_deliv = rcvd_count + 1; // TODO needs to be corrected when queueing is implemented
    printf("New RX_deliv %d, rcvd_count %d\n", rx_deliv, rcvd_count);
  }

  // Not clear how to update RX_DELIV without reception buffer (TODO)

  // TODO handle reordering timers
}

uint32_t pdcp_entity_nr::read_data_header(const unique_byte_buffer_t& pdu)
{
  // Check PDU is long enough to extract header
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    log->error("PDU too small to extract header\n");
    return 0;
  }

  // Extract RCVD_SN
  uint16_t rcvd_sn_16 = 0;
  uint32_t rcvd_sn_32 = 0;
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_12:
      srslte::uint8_to_uint16(pdu->msg, &rcvd_sn_16);
      rcvd_sn_32 = SN(rcvd_sn_16);
      break;
    case PDCP_SN_LEN_18:
      srslte::uint8_to_uint24(pdu->msg, &rcvd_sn_32);
      rcvd_sn_32 = SN(rcvd_sn_32);
      break;
      break;
    default:
      log->error("Cannot extract RCVD_SN, invalid SN length configured: %d\n", cfg.sn_len);
  }

  // Discard header
  pdu->msg += cfg.hdr_len_bytes;
  pdu->N_bytes -= cfg.hdr_len_bytes;
  return rcvd_sn_32;
}

void pdcp_entity_nr::write_data_header(const srslte::unique_byte_buffer_t& sdu, uint32_t count)
{
  // Add room for header
  if (cfg.hdr_len_bytes > sdu->get_headroom()) {
    log->error("Not enough space to add header\n");
    return;
  }
  sdu->msg -= cfg.hdr_len_bytes;
  sdu->N_bytes += cfg.hdr_len_bytes;

  // Add SN
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_12:
      srslte::uint16_to_uint8(SN(count), sdu->msg);
      if (is_drb()) {
        sdu->msg[0] |= 0x80; // On Data PDUs for DRBs we must set the D flag.
      }
      break;
    case PDCP_SN_LEN_18:
      srslte::uint24_to_uint8(SN(count), sdu->msg);
      sdu->msg[0] |= 0x80; // Data PDU and SN LEN 18 implies DRB, D flag must be present
      break;
    default:
      log->error("Invalid SN length configuration: %d bits\n", cfg.sn_len);
  }
}

void pdcp_entity_nr::extract_mac(const unique_byte_buffer_t& pdu, uint8_t* mac)
{
  // Check enough space for MAC
  if (pdu->N_bytes < 4) {
    log->error("PDU too small to extract MAC-I\n");
    return;
  }

  // Extract MAC
  memcpy(mac, &pdu->msg[pdu->N_bytes - 4], 4);
  pdu->N_bytes -= 4;
}

void pdcp_entity_nr::append_mac(const unique_byte_buffer_t& sdu, uint8_t* mac)
{
  // Check enough space for MAC
  if (sdu->N_bytes + 4 > sdu->get_tailroom()) {
    log->error("Not enough space to add MAC-I\n");
    return;
  }

  // Append MAC
  memcpy(&sdu->msg[sdu->N_bytes], mac, 4);
  sdu->N_bytes += 4;
}
} // namespace srslte
