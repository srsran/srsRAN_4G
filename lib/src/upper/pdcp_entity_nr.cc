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
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity_nr::pdcp_entity_nr() {}

pdcp_entity_nr::~pdcp_entity_nr() {}

void pdcp_entity_nr::init(srsue::rlc_interface_pdcp*  rlc_,
                          srsue::rrc_interface_pdcp*  rrc_,
                          srsue::gw_interface_pdcp*   gw_,
                          srslte::log*                log_,
                          uint32_t                    lcid_,
                          srslte_pdcp_config_nr_t     cfg_)
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

  // TODO
  sn_len_bytes = (int)cfg.sn_len % 8;
}

// Reestablishment procedure: 36.323 5.2
void pdcp_entity_nr::reestablish()
{
  log->info("Re-establish %s with bearer ID: %d\n", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  // For TODO
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
  log->info_hex(sdu->msg, sdu->N_bytes,
        "TX %s SDU, do_integrity = %s, do_encryption = %s",
        rrc->get_rb_name(lcid).c_str(),
        (do_integrity) ? "true" : "false", (do_encryption) ? "true" : "false");

  // Start discard timer TODO
  // Perform header compression TODO

  // Integrity protection and ciphering
  integrity_generate(sdu->msg, sdu->N_bytes - 4, tx_next, &sdu->msg[sdu->N_bytes - 4]);
  ciphering_generate(sdu->msg, sdu->N_bytes - 4, tx_next, &sdu->msg[sdu->N_bytes - 4]);

  // Write PDCP header info
  write_header(pdu, tx_next);
  
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
  if (pdu->N_bytes <= sn_len_bytes) {
    return;
  }

  // Calculate RCVD_COUNT
  uint32_t rcvd_sn = get_rcvd_sn(pdu);
  uint32_t rcvd_hfn, rcvd_count;
  if (rcvd_sn < SN(rx_deliv) - window_size) {
    rcvd_hfn = HFN(rx_deliv) + 1;
  } else if (rcvd_sn >= SN(rx_deliv) + window_size) {
    rcvd_hfn = HFN(rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, rcvd_sn);

  // Integrity check
  uint8_t mac[4];
  bool is_valid = integrity_verify(pdu->msg, pdu->N_bytes, rcvd_count, mac);
  if (!is_valid) {
    return; // Invalid packet, drop.
  }

  // Decripting
  cipher_decrypt(pdu->msg, pdu->N_bytes, rcvd_count, pdu->msg);

  // Check valid rcvd_count
  if (rcvd_count < rx_deliv /*|| check_received_before() TODO*/) {
    return; // Invalid count, drop.
  }

  // Store PDU in reception buffer
  // TODO

  // Update RX_NEXT
  if(rcvd_count >= rx_next){
    rx_next = rcvd_count + 1;
  }
  
  // Deliver to upper layers (TODO support in order delivery)
  if (is_control()) {
    rrc->write_pdu(lcid, std::move(pdu));
  } else {
    gw->write_pdu(lcid, std::move(pdu));
  }
  
  // Not clear how to update RX_DELIV without reception buffer (TODO) 

  // TODO handle reordering timers
}

uint32_t pdcp_entity_nr::get_rcvd_sn(const unique_byte_buffer_t& pdu)
{
  uint32_t rcvd_sn = 0;
  switch (sn_len) {
    case PDCP_SN_LEN_12:
      pdu->msg;
    case PDCP_SN_LEN_18:
      pdu->msg;
    default:
      log->error("Cannot extract RCVD_SN, invalid SN length configured: %d\n", (int)sn_len);
  }
  return rcvd_sn;
}
} // namespace srslte
