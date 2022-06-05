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

bool pdcp_entity_nr::configure(const pdcp_config_t& cnfg_)
{
  if (active) {
    // Already configured
    if (cnfg_ != cfg) {
      logger.error("Bearer reconfiguration not supported. LCID=%s.", rb_name.c_str());
      return false;
    }
    return true;
  }

  cfg         = cnfg_;
  rb_name     = cfg.get_rb_name();
  window_size = 1 << (cfg.sn_len - 1);

  rlc_mode = rlc->rb_is_um(lcid) ? rlc_mode_t::UM : rlc_mode_t::AM;

  // t-Reordering timer
  if (cfg.t_reordering != pdcp_t_reordering_t::infinity) {
    reordering_timer = task_sched.get_unique_timer();
    if (static_cast<uint32_t>(cfg.t_reordering) > 0) {
      reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), *reordering_fnc);
    }
  } else if (rlc_mode == rlc_mode_t::UM) {
    logger.warning("%s possible PDCP-NR misconfiguration: using infinite re-ordering timer with RLC UM bearer.",
                   rb_name);
  }

  active = true;
  logger.info("%s PDCP-NR entity configured. SN_LEN=%d, Discard timer %d, Re-ordering timer %d, RLC=%s, RAT=%s",
              rb_name,
              cfg.sn_len,
              cfg.discard_timer,
              cfg.t_reordering,
              rlc_mode == rlc_mode_t::UM ? "UM" : "AM",
              to_string(cfg.rat));

  // disable discard timer if using UM
  if (rlc_mode == rlc_mode_t::UM) {
    cfg.discard_timer = pdcp_discard_timer_t::infinity;
  }
  return true;
}

// Reestablishment procedure: 38.323 5.2
void pdcp_entity_nr::reestablish()
{
  logger.info("Re-establish %s with bearer ID: %d", rb_name.c_str(), cfg.bearer_id);
  // TODO
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_nr::reset()
{
  active = false;
  logger.debug("Reset %s", rb_name.c_str());
}

// SDAP/RRC interface
void pdcp_entity_nr::write_sdu(unique_byte_buffer_t sdu, int sn)
{
  // Log SDU
  logger.info(sdu->msg,
              sdu->N_bytes,
              "TX %s SDU (%dB), integrity=%s, encryption=%s",
              rb_name.c_str(),
              sdu->N_bytes,
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  if (rlc->sdu_queue_is_full(lcid)) {
    logger.info(sdu->msg, sdu->N_bytes, "Dropping %s SDU due to full queue", rb_name.c_str());
    return;
  }

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

  // Write PDCP header info
  write_data_header(sdu, tx_next);

  // TS 38.323, section 5.9: Integrity protection
  // The data unit that is integrity protected is the PDU header
  // and the data part of the PDU before ciphering.
  uint8_t mac[4] = {};
  if (is_srb() || (is_drb() && (integrity_direction == DIRECTION_TX || integrity_direction == DIRECTION_TXRX))) {
    integrity_generate(sdu->msg, sdu->N_bytes, tx_next, mac);
  }
  // Append MAC-I
  if (is_srb() || (is_drb() && (integrity_direction == DIRECTION_TX || integrity_direction == DIRECTION_TXRX))) {
    append_mac(sdu, mac);
  }

  // TS 38.323, section 5.8: Ciphering
  // The data unit that is ciphered is the MAC-I and the
  // data part of the PDCP Data PDU except the
  // SDAP header and the SDAP Control PDU if included in the PDCP SDU.
  if (encryption_direction == DIRECTION_TX || encryption_direction == DIRECTION_TXRX) {
    cipher_encrypt(
        &sdu->msg[cfg.hdr_len_bytes], sdu->N_bytes - cfg.hdr_len_bytes, tx_next, &sdu->msg[cfg.hdr_len_bytes]);
  }

  // Set meta-data for RLC AM
  sdu->md.pdcp_sn = tx_next;

  logger.info(sdu->msg,
              sdu->N_bytes,
              "TX %s PDU (%dB), HFN=%d, SN=%d, integrity=%s, encryption=%s",
              rb_name.c_str(),
              sdu->N_bytes,
              HFN(tx_next),
              SN(tx_next),
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

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
              rb_name.c_str(),
              pdu->N_bytes,
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  if (rx_overflow) {
    logger.warning("Rx PDCP COUNTs have overflowed. Discarding SDU.");
    return;
  }

  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    return;
  }
  logger.debug("Rx PDCP state - RX_NEXT=%u, RX_DELIV=%u, RX_REORD=%u", rx_next, rx_deliv, rx_reord);

  // Extract RCVD_SN from header
  uint32_t rcvd_sn = read_data_header(pdu);

  /*
   * Calculate RCVD_COUNT:
   *
   * - if RCVD_SN < SN(RX_DELIV) – Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) + 1.
   * - else if RCVD_SN >= SN(RX_DELIV) + Window_Size:
   *   - RCVD_HFN = HFN(RX_DELIV) – 1.
   * - else:
   *   - RCVD_HFN = HFN(RX_DELIV);
   * - RCVD_COUNT = [RCVD_HFN, RCVD_SN].
   */
  uint32_t rcvd_hfn, rcvd_count;
  if ((int64_t)rcvd_sn < (int64_t)SN(rx_deliv) - (int64_t)window_size) {
    rcvd_hfn = HFN(rx_deliv) + 1;
  } else if (rcvd_sn >= SN(rx_deliv) + window_size) {
    rcvd_hfn = HFN(rx_deliv) - 1;
  } else {
    rcvd_hfn = HFN(rx_deliv);
  }
  rcvd_count = COUNT(rcvd_hfn, rcvd_sn);

  logger.debug("Estimated RCVD_HFN=%u, RCVD_SN=%u, RCVD_COUNT=%u", rcvd_hfn, rcvd_sn, rcvd_count);

  /*
   * TS 38.323, section 5.8: Deciphering
   *
   * The data unit that is ciphered is the MAC-I and the
   * data part of the PDCP Data PDU except the
   * SDAP header and the SDAP Control PDU if included in the PDCP SDU.
   */
  if (encryption_direction == DIRECTION_RX || encryption_direction == DIRECTION_TXRX) {
    cipher_decrypt(
        &pdu->msg[cfg.hdr_len_bytes], pdu->N_bytes - cfg.hdr_len_bytes, rcvd_count, &pdu->msg[cfg.hdr_len_bytes]);
  }

  /*
   * Extract MAC-I:
   * Always extract from SRBs, only extract from DRBs if integrity is enabled
   */
  uint8_t mac[4] = {};
  if (is_srb() || (is_drb() && (integrity_direction == DIRECTION_TX || integrity_direction == DIRECTION_TXRX))) {
    extract_mac(pdu, mac);
  }

  /*
   * TS 38.323, section 5.9: Integrity verification
   *
   * The data unit that is integrity protected is the PDU header
   * and the data part of the PDU before ciphering.
   */
  if (integrity_direction == DIRECTION_TX || integrity_direction == DIRECTION_TXRX) {
    bool is_valid = integrity_verify(pdu->msg, pdu->N_bytes, rcvd_count, mac);
    if (!is_valid) {
      logger.error(pdu->msg, pdu->N_bytes, "%s Dropping PDU", rb_name.c_str());
      rrc->notify_pdcp_integrity_error(lcid);
      return; // Invalid packet, drop.
    } else {
      logger.debug(pdu->msg, pdu->N_bytes, "%s: Integrity verification successful", rb_name.c_str());
    }
  }

  // After checking the integrity, we can discard the header.
  discard_data_header(pdu);

  /*
   * Check valid rcvd_count:
   *
   * - if RCVD_COUNT < RX_DELIV; or
   * - if the PDCP Data PDU with COUNT = RCVD_COUNT has been received before:
   *   - discard the PDCP Data PDU;
   */
  if (rcvd_count < rx_deliv) {
    logger.debug("Out-of-order after time-out, duplicate or COUNT wrap-around");
    logger.debug("RCVD_COUNT %u, RCVD_COUNT %u", rcvd_count, rx_deliv);
    return; // Invalid count, drop.
  }

  // Check if PDU has been received
  if (reorder_queue.find(rcvd_count) != reorder_queue.end()) {
    logger.debug("Duplicate PDU, dropping");
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
    // Deliver to upper layers in ascending order of associated COUNT
    deliver_all_consecutive_counts();
  }

  // Handle reordering timers
  if (reordering_timer.is_running() and rx_deliv >= rx_reord) {
    reordering_timer.stop();
    logger.debug("Stopped t-Reordering - RX_DELIV=%d, RX_REORD=%ld", rx_deliv, rx_reord);
  }

  if (cfg.t_reordering != pdcp_t_reordering_t::infinity) {
    if (not reordering_timer.is_running() and rx_deliv < rx_next) {
      rx_reord = rx_next;
      reordering_timer.run();
      logger.debug("Started t-Reordering - RX_REORD=%ld, RX_DELIV=%ld, RX_NEXT=%ld", rx_reord, rx_deliv, rx_next);
    }
  }

  logger.debug("Rx PDCP state - RX_NEXT=%u, RX_DELIV=%u, RX_REORD=%u", rx_next, rx_deliv, rx_reord);
}

// Notification of delivery/failure
void pdcp_entity_nr::notify_delivery(const pdcp_sn_vector_t& pdcp_sns)
{
  logger.debug("Received delivery notification from RLC. Nof SNs=%ld", pdcp_sns.size());
  for (uint32_t sn : pdcp_sns) {
    // Remove timer from map
    logger.debug("Stopping discard timer for SN=%ld", sn);
    discard_timers_map.erase(sn);
  }
}

void pdcp_entity_nr::notify_failure(const pdcp_sn_vector_t& pdcp_sns)
{
  logger.debug("Received failure notification from RLC. Nof SNs=%ld", pdcp_sns.size());
}

/*
 * Packing / Unpacking Helpers
 */

// Deliver all consecutively associated COUNTs.
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
  parent->logger.info(
      "Reordering timer expired. RX_REORD=%u, re-order queue size=%ld", parent->rx_reord, parent->reorder_queue.size());

  // Deliver all PDCP SDU(s) with associated COUNT value(s) < RX_REORD
  for (std::map<uint32_t, unique_byte_buffer_t>::iterator it = parent->reorder_queue.begin();
       it != parent->reorder_queue.end() && it->first < parent->rx_reord;
       parent->reorder_queue.erase(it++)) {
    // Deliver to upper layers
    parent->pass_to_upper_layers(std::move(it->second));
  }

  // Update RX_DELIV to the first PDCP SDU not delivered to the upper layers
  parent->rx_deliv = parent->rx_reord;

  // Deliver all PDCP SDU(s) consecutively associated COUNT value(s) starting from RX_REORD
  parent->deliver_all_consecutive_counts();

  if (parent->rx_deliv < parent->rx_next) {
    parent->logger.debug("Updating RX_REORD to %ld. Old RX_REORD=%ld, RX_DELIV=%ld",
                         parent->rx_next,
                         parent->rx_reord,
                         parent->rx_deliv);
    parent->rx_reord = parent->rx_next;
    parent->reordering_timer.run();
  }
}

// Discard Timer Callback (discardTimer)
void pdcp_entity_nr::discard_callback::operator()(uint32_t timer_id)
{
  parent->logger.debug("Discard timer expired for PDU with SN=%d", discard_sn);

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
