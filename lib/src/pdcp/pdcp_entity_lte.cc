/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/upper/pdcp_entity_lte.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/security.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include <bitset>

namespace srsran {

/****************************************************************************
 * PDCP Entity LTE class
 ***************************************************************************/

pdcp_entity_lte::pdcp_entity_lte(srsue::rlc_interface_pdcp* rlc_,
                                 srsue::rrc_interface_pdcp* rrc_,
                                 srsue::gw_interface_pdcp*  gw_,
                                 srsran::task_sched_handle  task_sched_,
                                 srslog::basic_logger&      logger,
                                 uint32_t                   lcid_) :
  pdcp_entity_base(task_sched_, logger), rlc(rlc_), rrc(rrc_), gw(gw_)
{
  // Initial state
  integrity_direction  = DIRECTION_NONE;
  encryption_direction = DIRECTION_NONE;

  st.next_pdcp_tx_sn = 0;
  st.tx_hfn          = 0;
  st.rx_hfn          = 0;
  st.next_pdcp_rx_sn = 0;

  lcid = lcid_;
}

pdcp_entity_lte::~pdcp_entity_lte()
{
  reset();
}

bool pdcp_entity_lte::configure(const pdcp_config_t& cnfg_)
{
  if (active) {
    // Already configured
    if (cnfg_ != cfg) {
      logger.error("Bearer reconfiguration not supported. LCID=%s.", rb_name.c_str());
      return false;
    }
    return true;
  }

  cfg     = cnfg_;
  rb_name = cfg.get_rb_name();

  maximum_pdcp_sn              = (1u << cfg.sn_len) - 1u;
  st.last_submitted_pdcp_rx_sn = maximum_pdcp_sn;
  if (is_srb()) {
    reordering_window = 0;
  } else if (is_drb()) {
    reordering_window = 2048;
  }

  if (is_drb() && not rlc->rb_is_um(lcid) && cfg.discard_timer == pdcp_discard_timer_t::infinity) {
    logger.warning(
        "Setting discard timer to 1500ms, to avoid issues with lingering SDUs in the Unacknowledged SDUs map. LCID=%d",
        lcid);
    cfg.discard_timer = pdcp_discard_timer_t::ms1500;
  }

  // Queue Helpers
  maximum_allocated_sns_window = (1u << cfg.sn_len) / 2u;

  logger.info("Init %s with bearer ID: %d", rb_name.c_str(), cfg.bearer_id);
  logger.info("SN len bits: %d, SN len bytes: %d, reordering window: %d, Maximum SN: %d, discard timer: %d ms",
              cfg.sn_len,
              cfg.hdr_len_bytes,
              reordering_window,
              maximum_pdcp_sn,
              static_cast<uint32_t>(cfg.discard_timer));
  logger.info("Status Report Required: %s", cfg.status_report_required ? "True" : "False");

  if (is_drb() and not rlc->rb_is_um(lcid)) {
    undelivered_sdus = std::unique_ptr<undelivered_sdus_queue>(new undelivered_sdus_queue(task_sched, maximum_pdcp_sn));
    rx_counts_info.reserve(reordering_window);
  }

  // Check supported config
  if (!check_valid_config()) {
    srsran::console("Warning: Invalid PDCP config.\n");
    return false;
  }
  active = true;
  return true;
}
// Reestablishment procedure: 36.323 5.2
void pdcp_entity_lte::reestablish()
{
  logger.info("Re-establish %s with bearer ID: %d", rb_name.c_str(), cfg.bearer_id);
  // For SRBs
  if (is_srb()) {
    st.next_pdcp_tx_sn = 0;
    st.tx_hfn          = 0;
    st.rx_hfn          = 0;
    st.next_pdcp_rx_sn = 0;
  } else if (rlc->rb_is_um(lcid)) {
    // Only reset counter in RLC-UM
    st.next_pdcp_tx_sn = 0;
    st.tx_hfn          = 0;
    st.rx_hfn          = 0;
    st.next_pdcp_rx_sn = 0;
  } else {
    // Sending the status report will be triggered by the RRC if required
  }
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_lte::reset()
{
  if (active) {
    logger.debug("Reset %s", rb_name.c_str());
  }
  active = false;
}

// GW/RRC interface
void pdcp_entity_lte::write_sdu(unique_byte_buffer_t sdu, int upper_sn)
{
  if (!active) {
    logger.warning("Dropping %s SDU due to inactive bearer", rb_name.c_str());
    return;
  }

  if (rlc->is_suspended(lcid)) {
    logger.warning("Trying to send SDU while re-establishment is in progress. Dropping SDU. LCID=%d", lcid);
    return;
  }

  if (rlc->sdu_queue_is_full(lcid)) {
    logger.info(sdu->msg, sdu->N_bytes, "Dropping %s SDU due to full queue", rb_name.c_str());
    return;
  }

  // Get COUNT to be used with this packet
  uint32_t used_sn;
  if (upper_sn == -1) {
    used_sn = st.next_pdcp_tx_sn; // Normal scenario
  } else {
    used_sn = upper_sn; // SN provided by the upper layers, due to handover.
  }

  uint32_t tx_count = COUNT(st.tx_hfn, used_sn); // Normal scenario

  // If the bearer is mapped to RLC AM, save TX_COUNT and a copy of the PDU.
  // This will be used for reestablishment, where unack'ed PDUs will be re-transmitted.
  // PDUs will be removed from the queue, either when the lower layers will report
  // a successfull transmission or when the discard timer expires.
  // Status report will also use this queue, to know the First Missing SDU (FMS).
  if (!rlc->rb_is_um(lcid) and is_drb()) {
    if (not store_sdu(used_sn, sdu)) {
      // Could not store the SDU, discarding
      logger.warning("Could not store SDU. Discarding SN=%d", used_sn);
      return;
    }
  }
  // check for pending security config in transmit direction
  if (enable_security_tx_sn != -1 && enable_security_tx_sn == static_cast<int32_t>(tx_count)) {
    enable_integrity(DIRECTION_TX);
    enable_encryption(DIRECTION_TX);
    enable_security_tx_sn = -1;
  }

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
  }

  logger.info(sdu->msg,
              sdu->N_bytes,
              "TX %s PDU, SN=%d, integrity=%s, encryption=%s",
              rb_name.c_str(),
              used_sn,
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  // Set SDU metadata for RLC AM
  sdu->md.pdcp_sn = used_sn;

  // Increment NEXT_PDCP_TX_SN and TX_HFN (only update variables if SN was not provided by upper layers)
  if (upper_sn == -1) {
    st.next_pdcp_tx_sn++;
    if (st.next_pdcp_tx_sn > maximum_pdcp_sn) {
      st.tx_hfn++;
      st.next_pdcp_tx_sn = 0;
    }
  }

  // Pass PDU to lower layers
  metrics.num_tx_pdus++;
  metrics.num_tx_pdu_bytes += sdu->N_bytes;
  // Count TX'd bytes as if they were ACK'd if RLC is UM
  if (rlc->rb_is_um(lcid)) {
    metrics.num_tx_acked_bytes = metrics.num_tx_pdu_bytes;
  }
  rlc->write_sdu(lcid, std::move(sdu));
}

// RLC interface
void pdcp_entity_lte::write_pdu(unique_byte_buffer_t pdu)
{
  if (!active) {
    logger.warning("Dropping %s PDU due to inactive bearer", rb_name.c_str());
    return;
  }

  // Handle control PDUs
  if (is_drb() && is_control_pdu(pdu)) {
    logger.info("Handling PDCP control PDU");
    handle_control_pdu(std::move(pdu));
    return;
  }

  // Sanity check
  if (pdu->N_bytes <= cfg.hdr_len_bytes) {
    logger.error("PDCP PDU smaller than required header size.");
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

  logger.info(pdu->msg,
              pdu->N_bytes,
              "%s Rx PDU SN=%d (%d B, integrity=%s, encryption=%s)",
              rb_name.c_str(),
              sn,
              pdu->N_bytes,
              srsran_direction_text[integrity_direction],
              srsran_direction_text[encryption_direction]);

  // Update metrics
  metrics.num_rx_pdus++;
  metrics.num_rx_pdu_bytes += pdu->N_bytes;

  if (is_srb()) {
    handle_srb_pdu(std::move(pdu));
  } else if (is_drb() && rlc->rb_is_um(lcid)) {
    handle_um_drb_pdu(std::move(pdu));
  } else if (is_drb() && !rlc->rb_is_um(lcid)) {
    handle_am_drb_pdu(std::move(pdu));
  } else {
    logger.error("Invalid PDCP/RLC configuration");
    return;
  }
}

// Handle control PDU
void pdcp_entity_lte::handle_control_pdu(unique_byte_buffer_t pdu)
{
  switch (get_control_pdu_type(pdu)) {
    case PDCP_PDU_TYPE_STATUS_REPORT:
      handle_status_report_pdu(std::move(pdu));
      break;
    default:
      logger.warning("Unhandled control PDU");
      return;
  }
}

/****************************************************************************
 * Rx data handler functions
 * Ref: 3GPP TS 36.323 v10.1.0 Section 5.1.2
 ***************************************************************************/
// SRBs (5.1.2.2)
void pdcp_entity_lte::handle_srb_pdu(srsran::unique_byte_buffer_t pdu)
{
  // Read SN from header
  uint32_t sn = read_data_header(pdu);

  logger.debug("RX SRB PDU. Next_PDCP_RX_SN %d, SN %d", st.next_pdcp_rx_sn, sn);

  // Estimate COUNT for integrity check and decryption
  uint32_t count;
  if (sn < st.next_pdcp_rx_sn) {
    count = COUNT(st.rx_hfn + 1, sn);
  } else {
    count = COUNT(st.rx_hfn, sn);
  }

  // Perform decryption
  if (encryption_direction == DIRECTION_RX || encryption_direction == DIRECTION_TXRX) {
    cipher_decrypt(&pdu->msg[cfg.hdr_len_bytes], pdu->N_bytes - cfg.hdr_len_bytes, count, &pdu->msg[cfg.hdr_len_bytes]);
  }

  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx SDU SN=%d", rb_name.c_str(), sn);

  // Extract MAC
  uint8_t mac[4];
  extract_mac(pdu, mac);

  // Perfrom integrity checks
  if (integrity_direction == DIRECTION_RX || integrity_direction == DIRECTION_TXRX) {
    if (not integrity_verify(pdu->msg, pdu->N_bytes, count, mac)) {
      logger.error(pdu->msg, pdu->N_bytes, "%s Dropping PDU", rb_name.c_str());
      rrc->notify_pdcp_integrity_error(lcid);
      return; // Discard
    } else {
      logger.debug(pdu->msg, pdu->N_bytes, "%s: Integrity verification successful", rb_name.c_str());
    }
  }

  // Discard header
  discard_data_header(pdu);

  // Update state variables
  if (sn < st.next_pdcp_rx_sn) {
    st.rx_hfn++;
  }
  st.next_pdcp_rx_sn = sn + 1;

  if (st.next_pdcp_rx_sn > maximum_pdcp_sn) {
    st.next_pdcp_rx_sn = 0;
    st.rx_hfn++;
  }

  // Pass to upper layers
  rrc->write_pdu(lcid, std::move(pdu));
}

// DRBs mapped on RLC UM (5.1.2.1.3)
void pdcp_entity_lte::handle_um_drb_pdu(srsran::unique_byte_buffer_t pdu)
{
  uint32_t sn = read_data_header(pdu);
  discard_data_header(pdu);

  if (sn < st.next_pdcp_rx_sn) {
    st.rx_hfn++;
  }

  uint32_t count = (st.rx_hfn << cfg.sn_len) | sn;
  if (encryption_direction == DIRECTION_RX || encryption_direction == DIRECTION_TXRX) {
    cipher_decrypt(pdu->msg, pdu->N_bytes, count, pdu->msg);
  }

  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx PDU SN=%d", rb_name.c_str(), sn);

  st.next_pdcp_rx_sn = sn + 1;
  if (st.next_pdcp_rx_sn > maximum_pdcp_sn) {
    st.next_pdcp_rx_sn = 0;
    st.rx_hfn++;
  }

  // Pass to upper layers
  gw->write_pdu(lcid, std::move(pdu));
}

// DRBs mapped on RLC AM, without re-ordering (5.1.2.1.2)
void pdcp_entity_lte::handle_am_drb_pdu(srsran::unique_byte_buffer_t pdu)
{
  uint32_t sn = read_data_header(pdu);
  discard_data_header(pdu);

  int32_t last_submit_diff_sn     = st.last_submitted_pdcp_rx_sn - sn;
  int32_t sn_diff_last_submit     = sn - st.last_submitted_pdcp_rx_sn;
  int32_t sn_diff_next_pdcp_rx_sn = sn - st.next_pdcp_rx_sn;

  logger.debug("RX HFN: %d, SN=%d, Last_Submitted_PDCP_RX_SN=%d, Next_PDCP_RX_SN=%d",
               st.rx_hfn,
               sn,
               st.last_submitted_pdcp_rx_sn,
               st.next_pdcp_rx_sn);

  // Handle PDU
  uint32_t count = 0;
  if ((0 <= sn_diff_last_submit && sn_diff_last_submit > (int32_t)reordering_window) ||
      (0 <= last_submit_diff_sn && last_submit_diff_sn < (int32_t)reordering_window)) {
    // discard
    logger.debug("Discarding SN=%d (sn_diff_last_submit=%d, last_submit_diff_sn=%d, reordering_window=%d)",
                 sn,
                 sn_diff_last_submit,
                 last_submit_diff_sn,
                 reordering_window);
    return;
  }

  if ((int32_t)(st.next_pdcp_rx_sn - sn) > (int32_t)reordering_window) {
    logger.debug("(Next_PDCP_RX_SN - SN) is larger than re-ordering window.");
    st.rx_hfn++;
    count              = (st.rx_hfn << cfg.sn_len) | sn;
    st.next_pdcp_rx_sn = sn + 1;
  } else if (sn_diff_next_pdcp_rx_sn >= (int32_t)reordering_window) {
    logger.debug("(SN - Next_PDCP_RX_SN) is larger or equal than re-ordering window.");
    count = ((st.rx_hfn - 1) << cfg.sn_len) | sn;
  } else if (sn >= st.next_pdcp_rx_sn) {
    logger.debug("SN is larger or equal than Next_PDCP_RX_SN.");
    count              = (st.rx_hfn << cfg.sn_len) | sn;
    st.next_pdcp_rx_sn = sn + 1;
    if (st.next_pdcp_rx_sn > maximum_pdcp_sn) {
      st.next_pdcp_rx_sn = 0;
      st.rx_hfn++;
    }
  } else if (sn < st.next_pdcp_rx_sn) {
    logger.debug("SN is smaller than Next_PDCP_RX_SN.");
    count = (st.rx_hfn << cfg.sn_len) | sn;
  }

  // Decrypt
  cipher_decrypt(pdu->msg, pdu->N_bytes, count, pdu->msg);
  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx SDU SN=%d", rb_name.c_str(), sn);

  // Update info on last PDU submitted to upper layers
  st.last_submitted_pdcp_rx_sn = sn;

  // Store Rx SN/COUNT
  update_rx_counts_queue(count);

  // Pass to upper layers
  gw->write_pdu(lcid, std::move(pdu));
}

void pdcp_entity_lte::update_rx_counts_queue(uint32_t rx_count)
{
  if (rx_count < fmc) {
    return;
  }

  // Update largest RX_COUNT
  if (rx_count > largest_rx_count) {
    largest_rx_count = rx_count;
  }

  // The received COUNT is the first missing COUNT
  if (rx_count == fmc) {
    fmc++;
    // Update the queue for the Status report bitmap, if first missing count changed
    while (not rx_counts_info.empty() && rx_counts_info.back() == fmc) {
      rx_counts_info.pop_back();
      fmc++;
    }
  } else {
    rx_counts_info.push_back(rx_count);
    std::sort(rx_counts_info.begin(), rx_counts_info.end(), std::greater<uint32_t>());
  }

  // If the size of the rx_vector_info is getting very large
  // Consider the FMC as lost and update the vector.
  if (rx_counts_info.size() > reordering_window) {
    logger.debug("Queue too large. Updating. Old FMC=%d, Old back=%d, old queue_size=%zu",
                 fmc,
                 rx_counts_info.back(),
                 rx_counts_info.size());
    fmc = rx_counts_info.back();
    while (not rx_counts_info.empty() && rx_counts_info.back() == fmc) {
      rx_counts_info.pop_back();
      fmc++;
    }
    if (not rx_counts_info.empty()) {
      logger.debug("Queue too large. Updating. New FMC=%d, new back=%d, new queue_size=%zu",
                   fmc,
                   rx_counts_info.back(),
                   rx_counts_info.size());
    } else {
      logger.debug("Queue too large. Updating. New FMC=%d, new queue_size=%zu", fmc, rx_counts_info.size());
    }
  }

  if (rx_counts_info.empty()) {
    logger.info("Updated RX_COUNT info with SDU COUNT=%d, queue_size%zu, FMC=%d", rx_count, rx_counts_info.size(), fmc);
  } else {
    logger.info("Updated RX_COUNT info with SDU COUNT=%d, queue_size=%zu, FMC=%d, back=%d",
                rx_count,
                rx_counts_info.size(),
                fmc,
                rx_counts_info.back());
  }
}
/****************************************************************************
 * Control handler functions (Status Report)
 * Ref: 3GPP TS 36.323 v10.1.0 Section 5.1.3
 ***************************************************************************/

// Section 5.3.1 transmit operation
void pdcp_entity_lte::send_status_report()
{
  // Check wether RLC AM is being used.
  if (rlc->rb_is_um(lcid)) {
    logger.info("Trying to send PDCP Status Report and RLC is not AM");
    return;
  }

  // Don't send status report on SRBs
  if (not is_drb()) {
    logger.debug("Trying to send PDCP Status Report on SRB");
    return;
  }

  if (not cfg.status_report_required) {
    logger.info("Not sending PDCP Status Report as status report required is not set");
    return;
  }

  // Get First Missing Segment (FMS)
  uint32_t fms = SN(fmc);

  // Get Last Missing Segment
  uint32_t nof_sns_in_bitmap = rx_counts_info.size();

  // Allocate Status Report PDU
  unique_byte_buffer_t pdu = make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Error allocating buffer for status report");
    return;
  }

  logger.debug("Status report: FMS=%d, Nof SNs in bitmap=%d", fms, nof_sns_in_bitmap);
  // Set control bit and type of PDU
  pdu->msg[0] = ((uint8_t)PDCP_DC_FIELD_CONTROL_PDU << 7) | ((uint8_t)PDCP_PDU_TYPE_STATUS_REPORT << 4);

  // Set FMS
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_12:
      pdu->msg[0]  = pdu->msg[0] | (0x0F & (fms >> 8));
      pdu->msg[1]  = 0xFF & fms;
      pdu->N_bytes = 2;
      break;
    case PDCP_SN_LEN_18:
      pdu->msg[0]  = pdu->msg[0] | (0x03 & (fms >> 16));
      pdu->msg[1]  = 0xFF & (fms >> 8);
      pdu->msg[2]  = 0xFF & fms;
      pdu->N_bytes = 3;
      break;
    default:
      logger.error("Unsupported SN length for Status Report.");
      return;
  }

  // Add bitmap of missing PDUs, if necessary
  if (not rx_counts_info.empty()) {
    // First check size of bitmap
    int32_t  diff    = largest_rx_count - (fmc - 1);
    uint32_t nof_sns = 1u << cfg.sn_len;
    if (diff < 0) {
      logger.info("FMS and LMS are very far apart. Not generating status report. Largest RX COUNT=%d FMS=%d",
                  largest_rx_count,
                  fms);
      return;
    }
    uint32_t bitmap_sz = std::ceil((float)(diff) / 8);
    memset(&pdu->msg[pdu->N_bytes], 0, bitmap_sz);
    logger.debug(
        "Setting status report bitmap. Last missing SN=%d, Last SN acked in sequence=%d, Bitmap size in bytes=%d",
        largest_rx_count,
        fms - 1,
        bitmap_sz);
    for (uint32_t rx_count : rx_counts_info) {
      logger.debug("Setting bitmap for RX_COUNT=%d", rx_count);
      uint32_t offset      = rx_count - (fmc + 1);
      uint32_t bit_offset  = offset % 8;
      uint32_t byte_offset = offset / 8;
      pdu->msg[pdu->N_bytes + byte_offset] |= 1 << (7 - bit_offset);
    }
    pdu->N_bytes += bitmap_sz;
  }
  pdu->md.pdcp_sn = -1;

  // Write PDU to RLC
  rlc->write_sdu(lcid, std::move(pdu));
}

// Section 5.3.2 receive operation
void pdcp_entity_lte::handle_status_report_pdu(unique_byte_buffer_t pdu)
{
  // Don't handle status report on SRBs
  if (not is_drb()) {
    logger.debug("Not handling PDCP Status Report on SRB");
    return;
  }

  // Check wether RLC AM is being used.
  if (rlc->rb_is_um(lcid)) {
    logger.info("Not handling PDCP Status Report if RLC is not AM");
    return;
  }

  logger.info("Handling Status Report PDU. Size=%ld", pdu->N_bytes);

  uint32_t              fms           = 0;
  std::vector<uint32_t> acked_sns     = {};
  uint32_t              bitmap_offset = 0;

  // Get FMS
  switch (cfg.sn_len) {
    case PDCP_SN_LEN_12: {
      uint16_t tmp16;
      uint8_to_uint16(pdu->msg, &tmp16);
      fms           = tmp16 & 0x0FFF;
      bitmap_offset = 2;
      break;
    }
    case PDCP_SN_LEN_18: {
      uint8_to_uint24(pdu->msg, &fms);
      fms           = fms & 0x3FFF;
      bitmap_offset = 3;
      break;
    }
    default:
      logger.error("Unsupported SN length for Status Report.");
      return;
  }

  // Remove all SDUs with SN smaller than FMS
  for (uint32_t sn = 0; sn < fms; sn++) {
    if (sn < fms && undelivered_sdus->has_sdu(sn)) {
      undelivered_sdus->clear_sdu(sn);
    }
  }

  // Get acked SNs from bitmap
  for (uint32_t i = 0; (i + bitmap_offset) < pdu->N_bytes; i++) {
    std::bitset<8> bset{pdu->msg[bitmap_offset + i]};
    for (uint8_t j = 0; j < 8; j++) {
      if (bset[8 - j]) {
        uint32_t acked_sn = fms + i * 8 + j;
        acked_sns.push_back(acked_sn);
      }
    }
  }

  // Discard ACK'ed SDUs
  for (uint32_t sn : acked_sns) {
    logger.debug("Status report ACKed SN=%d.", sn);
    undelivered_sdus->clear_sdu(sn);
  }
}

/****************************************************************************
 * TX PDUs Queue Helper
 ***************************************************************************/

bool pdcp_entity_lte::store_sdu(uint32_t sn, const unique_byte_buffer_t& sdu)
{
  logger.debug("Storing SDU in undelivered SDUs queue. SN=%d, Queue size=%ld", sn, undelivered_sdus->size());

  // Check wether PDU is already in the queue
  if (undelivered_sdus->has_sdu(sn)) {
    logger.error("PDU already exists in the queue. TX_COUNT=%d", sn);
    return false;
  }

  if (undelivered_sdus->is_full()) {
    logger.error("Undelivered SDUs queue is full. TX_COUNT=%d", sn);
    return false;
  }

  // Make sure we don't associate more than half of the PDCP SN space of contiguous PDCP SDUs
  if (not undelivered_sdus->empty()) {
    uint32_t fms_sn = undelivered_sdus->get_fms();
    int32_t  diff   = sn - fms_sn;
    if (diff > (int32_t)maximum_allocated_sns_window) {
      // This SN is too large to assign, it may cause HFN de-synchronization.
      logger.info("This SN is too large to assign. Discarding. SN=%d, FMS=%d, diff=%d, window=%d, queue_size=%d",
                  sn,
                  fms_sn,
                  diff,
                  maximum_allocated_sns_window,
                  undelivered_sdus->size());
      return false;
    }
    if (diff < 0 && diff > -((int32_t)maximum_allocated_sns_window)) {
      // This SN is too large to assign, it may cause HFN de-synchronization.
      logger.info("This SN is too large to assign. Discarding. SN=%d, FMS=%d, diff=%d, window=%d, queue_size=%d",
                  sn,
                  fms_sn,
                  diff,
                  maximum_allocated_sns_window,
                  undelivered_sdus->size());
      return false;
    }
  }

  // Copy PDU contents into queue and start discard timer
  uint32_t         discard_timeout = static_cast<uint32_t>(cfg.discard_timer);
  discard_callback discard_fnc(this, sn);
  bool             ret = undelivered_sdus->add_sdu(sn, sdu, discard_timeout, discard_fnc);
  if (ret and discard_timeout > 0) {
    logger.debug("Discard Timer set for SN %u. Timeout: %ums", sn, discard_timeout);
  }
  return ret;
}

/****************************************************************************
 * Discard functionality
 ***************************************************************************/
// Discard Timer Callback (discardTimer)
void pdcp_entity_lte::discard_callback::operator()(uint32_t timer_id)
{
  parent->logger.info("Discard timer for SN=%d expired", discard_sn);

  // Notify the RLC of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  parent->rlc->discard_sdu(parent->lcid, discard_sn);

  // Discard PDU if unacknowledged
  if (parent->undelivered_sdus->has_sdu(discard_sn)) {
    parent->logger.debug("Removed undelivered PDU with TX_COUNT=%d", discard_sn);
    parent->undelivered_sdus->clear_sdu(discard_sn);
  } else {
    parent->logger.debug("Could not find PDU to discard. TX_COUNT=%d", discard_sn);
  }
}

/****************************************************************************
 * Handle delivery/failure notifications from RLC
 ***************************************************************************/
void pdcp_entity_lte::notify_delivery(const pdcp_sn_vector_t& pdcp_sns)
{
  if (not is_drb()) {
    return;
  }

  logger.info("Received delivery notification from RLC. Number of PDU notified=%zu", pdcp_sns.size());
  for (uint32_t sn : pdcp_sns) {
    logger.debug("Delivery notification received for PDU with SN=%d", sn);
    if (sn == UINT32_MAX) {
      continue;
    }
    // Find undelivered PDU info
    if (not undelivered_sdus->has_sdu(sn)) {
      logger.info("Could not find PDU for delivery notification. Notified SN=%d", sn);
    } else {
      // Metrics
      auto& sdu = (*undelivered_sdus)[sn];
      tx_pdu_ack_latency_ms.push(std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::high_resolution_clock::now() - sdu->get_timestamp())
                                     .count());
      metrics.num_tx_acked_bytes += sdu->N_bytes;
      metrics.num_tx_buffered_pdus_bytes -= sdu->N_bytes;

      // Remove PDU and disarm timer.
      undelivered_sdus->clear_sdu(sn);
    }
  }
}

void pdcp_entity_lte::notify_failure(const pdcp_sn_vector_t& pdcp_sns)
{
  if (not is_drb()) {
    return;
  }

  logger.info("Received failure notification from RLC. Number of PDU notified=%zu", pdcp_sns.size());

  for (uint32_t sn : pdcp_sns) {
    logger.info("Failure notification received for PDU with SN=%d", sn);
    if (sn == UINT32_MAX) {
      continue;
    }
    // Find undelivered PDU info
    if (not undelivered_sdus->has_sdu(sn)) {
      logger.info("Could not find PDU for failure notification. Notified SN=%d", sn);
    } else {
      // Remove PDU and disarm timer.
      undelivered_sdus->clear_sdu(sn);
    }
  }
}

/****************************************************************************
 * Config checking helper
 ***************************************************************************/
bool pdcp_entity_lte::check_valid_config()
{
  if (cfg.sn_len != PDCP_SN_LEN_5 && cfg.sn_len != PDCP_SN_LEN_7 && cfg.sn_len != PDCP_SN_LEN_12 &&
      cfg.sn_len != PDCP_SN_LEN_18) {
    logger.error("Trying to configure bearer with invalid SN LEN=%d", cfg.sn_len);
    return false;
  }
  if (cfg.sn_len == PDCP_SN_LEN_5 && is_drb()) {
    logger.error("Trying to configure DRB bearer with SN LEN of 5");
    return false;
  }
  if (cfg.sn_len == PDCP_SN_LEN_7 && (is_srb() || !rlc->rb_is_um(lcid))) {
    logger.error("Trying to configure SRB or RLC AM bearer with SN LEN of 7");
    return false;
  }
  return true;
}

/****************************************************************************
 * Internal state getters/setters
 ***************************************************************************/
void pdcp_entity_lte::get_bearer_state(pdcp_lte_state_t* state)
{
  *state = st;
}

void pdcp_entity_lte::set_bearer_state(const pdcp_lte_state_t& state, bool set_fmc)
{
  st = state;
  if (set_fmc) {
    fmc = COUNT(st.rx_hfn, st.last_submitted_pdcp_rx_sn);
  }
}

std::map<uint32_t, srsran::unique_byte_buffer_t> pdcp_entity_lte::get_buffered_pdus()
{
  if (undelivered_sdus == nullptr) {
    logger.error("Buffered PDUs being requested for non-AM DRB");
    return std::map<uint32_t, srsran::unique_byte_buffer_t>{};
  }
  logger.info("Buffered PDUs requested, buffer_size=%zu", undelivered_sdus->size());
  return undelivered_sdus->get_buffered_sdus();
}

/****************************************************************************
 * Metrics helpers
 ***************************************************************************/
pdcp_bearer_metrics_t pdcp_entity_lte::get_metrics()
{
  if (undelivered_sdus != nullptr) {
    metrics.num_tx_buffered_pdus       = undelivered_sdus->size();
    metrics.num_tx_buffered_pdus_bytes = undelivered_sdus->get_bytes(); //< Number of bytes of PDUs waiting for ACK
  }
  metrics.tx_notification_latency_ms =
      tx_pdu_ack_latency_ms.value(); //< Average time in ms from PDU delivery to RLC to ACK notification from RLC
  return metrics;
}

void pdcp_entity_lte::reset_metrics()
{
  // Only reset metrics that have are snapshots, leave the incremental ones untouched.
  metrics.tx_notification_latency_ms = 0;
}

/****************************************************************************
 * Undelivered SDUs queue helpers
 ***************************************************************************/
undelivered_sdus_queue::undelivered_sdus_queue(srsran::task_sched_handle task_sched, uint32_t sn_mod) : sn_mod(sn_mod)
{
  for (auto& e : sdus) {
    e.discard_timer = task_sched.get_unique_timer();
  }
}

bool undelivered_sdus_queue::add_sdu(uint32_t                              sn,
                                     const srsran::unique_byte_buffer_t&   sdu,
                                     uint32_t                              discard_timeout,
                                     srsran::move_callback<void(uint32_t)> callback)
{
  assert(not has_sdu(sn) && "Cannot add repeated SNs");

  if (is_full()) {
    return false;
  }

  // Make sure we don't associate more than half of the PDCP SN space of contiguous PDCP SDUs
  if (not empty()) {
    int32_t diff = sn - fms;
    if (diff > (int32_t)(capacity / 2)) {
      return false;
    }
    if (diff <= 0 && diff > -((int32_t)(capacity / 2))) {
      return false;
    }
  }

  // Allocate buffer and exit on error
  srsran::unique_byte_buffer_t tmp = make_byte_buffer();
  if (tmp == nullptr) {
    return false;
  }

  // Update FMS and LMS if necessary
  if (empty()) {
    fms = sn;
    lms = sn;
  } else {
    update_lms(sn);
  }
  // Add SDU
  count++;
  sdus[sn].sdu             = std::move(tmp);
  sdus[sn].sdu->md.pdcp_sn = sn;
  sdus[sn].sdu->N_bytes    = sdu->N_bytes;
  memcpy(sdus[sn].sdu->msg, sdu->msg, sdu->N_bytes);
  if (discard_timeout > 0) {
    sdus[sn].discard_timer.set(discard_timeout, std::move(callback));
    sdus[sn].discard_timer.run();
  }
  sdus[sn].sdu->set_timestamp(); // Metrics
  bytes += sdu->N_bytes;
  return true;
}

bool undelivered_sdus_queue::clear_sdu(uint32_t sn)
{
  if (not has_sdu(sn)) {
    return false;
  }
  count--;
  bytes -= sdus[sn].sdu->N_bytes;
  sdus[sn].discard_timer.stop();
  sdus[sn].sdu.reset();
  // Find next FMS, if necessary
  if (sn == fms) {
    update_fms();
  }
  return true;
}

void undelivered_sdus_queue::clear()
{
  count = 0;
  bytes = 0;
  fms   = 0;
  for (uint32_t sn = 0; sn < capacity; sn++) {
    sdus[sn].discard_timer.stop();
    sdus[sn].sdu.reset();
  }
}

size_t undelivered_sdus_queue::nof_discard_timers() const
{
  return std::count_if(sdus.begin(), sdus.end(), [](const sdu_data& s) {
    return s.sdu != nullptr and s.discard_timer.is_valid() and s.discard_timer.is_running();
  });
}

void undelivered_sdus_queue::update_fms()
{
  if (empty()) {
    fms = increment_sn(fms);
    return;
  }

  for (uint32_t i = 0; i < capacity / 2; ++i) {
    uint32_t sn = increment_sn(fms + i);
    if (has_sdu(sn)) {
      fms = sn;
      return;
    }
  }

  fms = increment_sn(fms);
}

void undelivered_sdus_queue::update_lms(uint32_t sn)
{
  if (empty()) {
    lms = fms;
    return;
  }

  int32_t diff = sn - lms;
  if (diff > 0 && sn > lms) {
    lms = sn;
  } else if (diff < 0 && sn < lms) {
    lms = sn;
  }
}

std::map<uint32_t, srsran::unique_byte_buffer_t> undelivered_sdus_queue::get_buffered_sdus()
{
  std::map<uint32_t, srsran::unique_byte_buffer_t> fwd_sdus;
  for (auto& sdu : sdus) {
    if (sdu.sdu != nullptr) {
      // TODO: Find ways to avoid deep copy
      srsran::unique_byte_buffer_t fwd_sdu = make_byte_buffer();
      if (fwd_sdu != nullptr) {
        *fwd_sdu = *sdu.sdu;
        fwd_sdus.emplace(sdu.sdu->md.pdcp_sn, std::move(fwd_sdu));
      } else {
        srslog::fetch_basic_logger("PDCP").warning("Can't allocate buffer to forward buffered SDUs.");
      }
    }
  }
  return fwd_sdus;
}

} // namespace srsran
