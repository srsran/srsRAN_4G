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

pdcp_entity_nr::pdcp_entity_nr(srsue::rlc_interface_pdcp* rlc_,
                               srsue::rrc_interface_pdcp* rrc_,
                               srsue::gw_interface_pdcp*  gw_,
                               srslte::timer_handler*     timers_,
                               srslte::log*               log_) :
  pdcp_entity_base(timers_, log_),
  rlc(rlc_),
  rrc(rrc_),
  gw(gw_),
  reordering_fnc(new pdcp_entity_nr::reordering_callback(this))
{
}

pdcp_entity_nr::~pdcp_entity_nr() {}

void pdcp_entity_nr::init(uint32_t lcid_, pdcp_config_t cfg_)
{
  lcid          = lcid_;
  cfg           = cfg_;
  active        = true;
  do_integrity  = false;
  do_encryption = false;

  window_size = 1 << (cfg.sn_len - 1);

  // Timers
  reordering_timer = timers->get_unique_timer();

  // configure timer
  if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), *reordering_fnc);
  }

  // Mark entity as initialized
  initialized = true;
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
  if (log != nullptr) {
    log->debug("Reset %s\n", rrc->get_rb_name(lcid).c_str());
  }
}

// SDAP/RRC interface
void pdcp_entity_nr::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  // Check initialization
  if (not initialized) {
    return;
  }

  // Log SDU
  log->info_hex(sdu->msg,
                sdu->N_bytes,
                "TX %s SDU, do_integrity = %s, do_encryption = %s",
                rrc->get_rb_name(lcid).c_str(),
                (do_integrity) ? "true" : "false",
                (do_encryption) ? "true" : "false");

  // Check for COUNT overflow
  if (tx_overflow) {
    log->warning("TX_NEXT has overflowed. Droping packet\n");
    return;
  }
  if (tx_next + 1 == 0) {
    tx_overflow = true;
  }

  // Start discard timer
  if (cfg.discard_timer != pdcp_discard_timer_t::infinity) {
    timer_handler::unique_timer discard_timer = timers->get_unique_timer();
    discard_callback            discard_fnc(this, tx_next);
    discard_timer.set(static_cast<uint32_t>(cfg.discard_timer), discard_fnc);
    discard_timer.run();
    discard_timers_map.insert(std::make_pair(tx_next, std::move(discard_timer)));
    log->debug("Discard Timer set for SN %u. Timeout: %ums\n", tx_next, static_cast<uint32_t>(cfg.discard_timer));
  }

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
  // Check initialization
  if (not initialized) {
    return;
  }

  // Log PDU
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

  log->debug("RCVD_HFN %u RCVD_SN %u, RCVD_COUNT %u\n", rcvd_hfn, rcvd_sn, rcvd_count);

  // Decripting
  cipher_decrypt(pdu->msg, pdu->N_bytes, rcvd_count, pdu->msg);

  // Integrity check
  bool is_valid = integrity_verify(pdu->msg, pdu->N_bytes, rcvd_count, mac);
  if (!is_valid) {
    return; // Invalid packet, drop.
  }

  // Check valid rcvd_count
  if (rcvd_count < rx_deliv) {
    log->debug("Out-of-order after time-out, duplicate or COUNT wrap-around\n");
    log->debug("RCVD_COUNT %u, RCVD_COUNT %u\n", rcvd_count, rx_deliv);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (reorder_queue.find(rcvd_count) != reorder_queue.end()) {
    return; // PDU already present, drop.
  }

  // Store PDU in reception buffer
  reorder_queue[rcvd_count] = std::move(pdu);

  // Update RX_NEXT
  if (rcvd_count >= rx_next) {
    rx_next = rcvd_count + 1;
  }

  // TODO if out-of-order configured, submit to upper layer

  if (rcvd_count == rx_deliv) {
    // Deliver to upper layers in ascending order of associeted COUNT
    deliver_all_consecutive_counts();
  }

  // Handle reordering timers
  if (reordering_timer.is_running() and rx_deliv >= rx_reord) {
    reordering_timer.stop();
  }

  if (not reordering_timer.is_running() and rx_deliv < rx_next) {
    rx_reord = rx_next;
    reordering_timer.run();
  }
}

/*
 * Packing / Unpacking Helpers
 */
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

// Deliver all consecutivly associated COUNTs.
// Update RX_NEXT after submitting to higher layers
void pdcp_entity_nr::deliver_all_consecutive_counts()
{
  for (std::map<uint32_t, unique_byte_buffer_t>::iterator it = reorder_queue.begin();
       it != reorder_queue.end() && it->first == rx_deliv;
       reorder_queue.erase(it++)) {
    log->debug("Delivering SDU with RCVD_COUNT %u\n", it->first);

    // Check RX_DELIV overflow
    if (rx_overflow) {
      log->warning("RX_DELIV has overflowed. Droping packet\n");
      return;
    }
    if (rx_deliv + 1 == 0) {
      rx_overflow = true;
    }

    // Pass PDCP SDU to the next layers
    pass_to_upper_layers(std::move(it->second));

    // Update RX_DELIV
    rx_deliv = rx_deliv + 1;
  }
}

/*
 * Timers
 */
// Reordering Timer Callback (t-reordering)
void pdcp_entity_nr::reordering_callback::operator()(uint32_t timer_id)
{
  parent->log->debug("Reordering timer expired\n");

  // Deliver all PDCP SDU(s) with associeted COUNT value(s) < RX_REORD
  for (std::map<uint32_t, unique_byte_buffer_t>::iterator it = parent->reorder_queue.begin();
       it != parent->reorder_queue.end() && it->first < parent->rx_reord;
       parent->reorder_queue.erase(it++)) {
    // Deliver to upper layers
    parent->pass_to_upper_layers(std::move(it->second));
  }

  // Deliver all PDCP SDU(s) consecutivly associeted COUNT value(s) starting from RX_REORD
  parent->deliver_all_consecutive_counts();

  if (parent->rx_deliv < parent->rx_next) {
    parent->rx_reord = parent->rx_next;
    parent->reordering_timer.run();
  }
  return;
}

// Discard Timer Callback (discardTimer)
void pdcp_entity_nr::discard_callback::operator()(uint32_t timer_id)
{
  parent->log->debug("Discard timer expired for PDU with SN = %d\n", discard_sn);

  // Notify the RLC of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  parent->rlc->discard_sdu(parent->lcid, discard_sn);

  // Remove timer from map
  // NOTE: this will delete the callback. It *must* be the last instruction.
  parent->discard_timers_map.erase(discard_sn);
  return;
}

} // namespace srslte
