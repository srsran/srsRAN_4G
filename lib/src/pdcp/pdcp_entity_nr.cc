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

#include "srsran/upper/pdcp_entity_nr.h"
#include "srsran/common/security.h"

namespace srsran {

pdcp_entity_nr::pdcp_entity_nr(srsue::rlc_interface_pdcp* rlc_,
                               srsue::rrc_interface_pdcp* rrc_,
                               srsue::gw_interface_pdcp*  gw_,
                               srsran::task_sched_handle  task_sched_,
                               srslog::basic_logger&      logger,
                               uint32_t                   lcid_) :
  pdcp_entity_base(task_sched_, logger),
  rlc(rlc_),
  rrc(rrc_),
  gw(gw_),
  reordering_fnc(new pdcp_entity_nr::reordering_callback(this))
{
  lcid                 = lcid_;
  integrity_direction  = DIRECTION_NONE;
  encryption_direction = DIRECTION_NONE;
}

pdcp_entity_nr::~pdcp_entity_nr() {}

// Reestablishment procedure: 38.323 5.2
void pdcp_entity_nr::reestablish()
{
  logger.info("Re-establish %s with bearer ID: %d", rrc->get_rb_name(lcid), cfg.bearer_id);
  // TODO
}

bool pdcp_entity_nr::configure(const pdcp_config_t& cnfg_)
{
  cfg         = cnfg_;
  window_size = 1 << (cfg.sn_len - 1);

  // Timers
  reordering_timer = task_sched.get_unique_timer();

  // configure timer
  if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), *reordering_fnc);
  }
  active = true;
  return true;
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_nr::reset()
{
  active = false;
  logger.debug("Reset %s", rrc->get_rb_name(lcid));
}

// SDAP/RRC interface
void pdcp_entity_nr::write_sdu(unique_byte_buffer_t sdu, int sn)
{
  // Log SDU
  logger.info(sdu->msg,
              sdu->N_bytes,
              "TX %s SDU, integrity=%s, encryption=%s",
              rrc->get_rb_name(lcid),
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  // Check for COUNT overflow
  if (tx_overflow) {
    logger.warning("TX_NEXT has overflowed. Dropping packet");
    return;
  }
  if (tx_next + 1 == 0) {
    tx_overflow = true;
  }

  // Start discard timer
  if (cfg.discard_timer != pdcp_discard_timer_t::infinity) {
    timer_handler::unique_timer discard_timer = task_sched.get_unique_timer();
    discard_callback            discard_fnc(this, tx_next);
    discard_timer.set(static_cast<uint32_t>(cfg.discard_timer), discard_fnc);
    discard_timer.run();
    discard_timers_map.insert(std::make_pair(tx_next, std::move(discard_timer)));
    logger.debug("Discard Timer set for SN %u. Timeout: %ums", tx_next, static_cast<uint32_t>(cfg.discard_timer));
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

  // Set meta-data for RLC AM
  sdu->md.pdcp_sn = tx_next;

  // Check if PDCP is associated with more than on RLC entity TODO
  // Write to lower layers
  rlc->write_sdu(lcid, std::move(sdu));

  // Increment TX_NEXT
  tx_next++;
}

// RLC interface
void pdcp_entity_nr::write_pdu(unique_byte_buffer_t pdu)
{
  // Log PDU
  logger.info(pdu->msg,
              pdu->N_bytes,
              "RX %s PDU (%d B), integrity=%s, encryption=%s",
              rrc->get_rb_name(lcid),
              pdu->N_bytes,
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    return;
  }

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = read_data_header(pdu);
  discard_data_header(pdu); // TODO: Check wheather the header is part of integrity check.

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

  logger.debug("RCVD_HFN %u RCVD_SN %u, RCVD_COUNT %u", rcvd_hfn, rcvd_sn, rcvd_count);

  // Decripting
  cipher_decrypt(pdu->msg, pdu->N_bytes, rcvd_count, pdu->msg);

  // Integrity check
  bool is_valid = integrity_verify(pdu->msg, pdu->N_bytes, rcvd_count, mac);
  if (!is_valid) {
    return; // Invalid packet, drop.
  }

  // Check valid rcvd_count
  if (rcvd_count < rx_deliv) {
    logger.debug("Out-of-order after time-out, duplicate or COUNT wrap-around");
    logger.debug("RCVD_COUNT %u, RCVD_COUNT %u", rcvd_count, rx_deliv);
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

// Notification of delivery/failure
void pdcp_entity_nr::notify_delivery(const pdcp_sn_vector_t& pdcp_sns)
{
  logger.debug("Received delivery notification from RLC. Nof SNs=%ld", pdcp_sns.size());
}

void pdcp_entity_nr::notify_failure(const pdcp_sn_vector_t& pdcp_sns)
{
  logger.debug("Received failure notification from RLC. Nof SNs=%ld", pdcp_sns.size());
}

/*
 * Packing / Unpacking Helpers
 */

// Deliver all consecutivly associated COUNTs.
// Update RX_NEXT after submitting to higher layers
void pdcp_entity_nr::deliver_all_consecutive_counts()
{
  for (std::map<uint32_t, unique_byte_buffer_t>::iterator it = reorder_queue.begin();
       it != reorder_queue.end() && it->first == rx_deliv;
       reorder_queue.erase(it++)) {
    logger.debug("Delivering SDU with RCVD_COUNT %u", it->first);

    // Check RX_DELIV overflow
    if (rx_overflow) {
      logger.warning("RX_DELIV has overflowed. Droping packet");
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
  parent->logger.debug("Reordering timer expired");

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
}

// Discard Timer Callback (discardTimer)
void pdcp_entity_nr::discard_callback::operator()(uint32_t timer_id)
{
  parent->logger.debug("Discard timer expired for PDU with SN = %d", discard_sn);

  // Notify the RLC of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  parent->rlc->discard_sdu(parent->lcid, discard_sn);

  // Remove timer from map
  // NOTE: this will delete the callback. It *must* be the last instruction.
  parent->discard_timers_map.erase(discard_sn);
}

void pdcp_entity_nr::get_bearer_state(pdcp_lte_state_t* state)
{
  // TODO
}

void pdcp_entity_nr::set_bearer_state(const pdcp_lte_state_t& state, bool set_fmc)
{
  // TODO
}

pdcp_bearer_metrics_t pdcp_entity_nr::get_metrics()
{
  // TODO
  return metrics;
}

void pdcp_entity_nr::reset_metrics()
{
  metrics = {};
}

} // namespace srsran
