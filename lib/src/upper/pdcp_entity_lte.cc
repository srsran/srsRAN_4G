/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/upper/pdcp_entity_lte.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/security.h"
#include <bitset>

namespace srslte {

pdcp_entity_lte::pdcp_entity_lte(srsue::rlc_interface_pdcp* rlc_,
                                 srsue::rrc_interface_pdcp* rrc_,
                                 srsue::gw_interface_pdcp*  gw_,
                                 srslte::task_sched_handle  task_sched_,
                                 srslog::basic_logger&      logger,
                                 uint32_t                   lcid_,
                                 pdcp_config_t              cfg_) :
  pdcp_entity_base(task_sched_, logger), rlc(rlc_), rrc(rrc_), gw(gw_)
{
  lcid                 = lcid_;
  cfg                  = cfg_;
  active               = true;
  integrity_direction  = DIRECTION_NONE;
  encryption_direction = DIRECTION_NONE;

  if (is_srb()) {
    reordering_window = 0;
  } else if (is_drb()) {
    reordering_window = 2048;
  }

  // Initial state
  st.next_pdcp_tx_sn           = 0;
  st.tx_hfn                    = 0;
  st.rx_hfn                    = 0;
  st.next_pdcp_rx_sn           = 0;
  maximum_pdcp_sn              = (1 << cfg.sn_len) - 1;
  st.last_submitted_pdcp_rx_sn = maximum_pdcp_sn;

  if (is_drb() && not rlc->rb_is_um(lcid) && cfg.discard_timer == pdcp_discard_timer_t::infinity) {
    logger.warning(
        "Setting discard timer to 1500ms, to avoid issues with lingering SDUs in the Unacknowledged SDUs map. LCID=%d",
        lcid);
    cfg.discard_timer = pdcp_discard_timer_t::ms1500;
  }

  uint32_t discard_time_value = static_cast<uint32_t>(cfg.discard_timer);
  if (discard_time_value > 0) {
    // Note: One extra position is reserved at the end for the status report
    discard_timers.reserve(maximum_pdcp_sn + 2);
    for (uint32_t sn = 0; sn < maximum_pdcp_sn + 2; ++sn) {
      discard_timers.emplace_back(task_sched.get_unique_timer());
      discard_callback discard_fnc(this, sn);
      discard_timers[sn].set(discard_time_value, discard_fnc);
    }
  }

  // Queue Helpers
  maximum_allocated_sns_window = (1 << cfg.sn_len) / 2;

  logger.info("Init %s with bearer ID: %d", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  logger.info("SN len bits: %d, SN len bytes: %d, reordering window: %d, Maximum SN: %d, discard timer: %d ms",
              cfg.sn_len,
              cfg.hdr_len_bytes,
              reordering_window,
              maximum_pdcp_sn,
              static_cast<uint32_t>(cfg.discard_timer));
  logger.info("Status Report Required: %s", cfg.status_report_required ? "True" : "False");

  // Check supported config
  if (!check_valid_config()) {
    srslte::console("Warning: Invalid PDCP config.\n");
  }
}

pdcp_entity_lte::~pdcp_entity_lte()
{
  reset();
}

// Reestablishment procedure: 36.323 5.2
void pdcp_entity_lte::reestablish()
{
  logger.info("Re-establish %s with bearer ID: %d", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  // For SRBs
  if (is_srb()) {
    st.next_pdcp_tx_sn = 0;
    st.tx_hfn          = 0;
    st.rx_hfn          = 0;
    st.next_pdcp_rx_sn = 0;
    undelivered_sdus_queue.clear();
  } else if (rlc->rb_is_um(lcid)) {
    // Only reset counter in RLC-UM
    st.next_pdcp_tx_sn = 0;
    st.tx_hfn          = 0;
    st.rx_hfn          = 0;
    st.next_pdcp_rx_sn = 0;
  } else {
    // Send status report if required on reestablishment in RLC AM
    // send_status_report();

    // Re-transmit unacknowledged SDUs
    /*
    send_status_report();

    // Re-transmit unacknowledged SDUs
    std::map<uint32_t, unique_byte_buffer_t> undelivered_sdus = std::move(undelivered_sdus_queue);
    undelivered_sdus_queue.clear();

    for (std::map<uint32_t, unique_byte_buffer_t>::iterator it = undelivered_sdus.begin(); it != undelivered_sdus.end();
         ++it) {
      write_sdu(std::move(it->second), it->first);
    }*/
  }
}

// Used to stop/pause the entity (called on RRC conn release)
void pdcp_entity_lte::reset()
{
  if (active) {
    logger.debug("Reset %s", rrc->get_rb_name(lcid).c_str());
  }
  active = false;
}

// GW/RRC interface
void pdcp_entity_lte::write_sdu(unique_byte_buffer_t sdu, int upper_sn)
{
  if (rlc->sdu_queue_is_full(lcid)) {
    logger.info(sdu->msg, sdu->N_bytes, "Dropping %s SDU due to full queue", rrc->get_rb_name(lcid).c_str());
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
  // a succesfull transmission or when the discard timer expires.
  // Status report will also use this queue, to know the First Missing SDU (FMS).
  if (!rlc->rb_is_um(lcid)) {
    if (not store_sdu(used_sn, sdu)) {
      // Could not store the SDU, discarding
      return;
    }

    // Start discard timer
    if (cfg.discard_timer != pdcp_discard_timer_t::infinity) {
      unique_timer* timer = get_discard_timer(used_sn);
      if (timer != nullptr) {
        timer->run();
        logger.debug("Discard Timer set for SN %u. Timeout: %ums", used_sn, static_cast<uint32_t>(cfg.discard_timer));
      }
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
              rrc->get_rb_name(lcid).c_str(),
              used_sn,
              srslte_direction_text[integrity_direction],
              srslte_direction_text[encryption_direction]);

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
  rlc->write_sdu(lcid, std::move(sdu));
}

// RLC interface
void pdcp_entity_lte::write_pdu(unique_byte_buffer_t pdu)
{
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
              rrc->get_rb_name(lcid).c_str(),
              sn,
              pdu->N_bytes,
              srslte_direction_text[integrity_direction],
              srslte_direction_text[encryption_direction]);

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
  return;
}

/****************************************************************************
 * Rx data handler functions
 * Ref: 3GPP TS 36.323 v10.1.0 Section 5.1.2
 ***************************************************************************/
// SRBs (5.1.2.2)
void pdcp_entity_lte::handle_srb_pdu(srslte::unique_byte_buffer_t pdu)
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

  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx SDU SN=%d", rrc->get_rb_name(lcid).c_str(), sn);

  // Extract MAC
  uint8_t mac[4];
  extract_mac(pdu, mac);

  // Perfrom integrity checks
  if (integrity_direction == DIRECTION_RX || integrity_direction == DIRECTION_TXRX) {
    if (not integrity_verify(pdu->msg, pdu->N_bytes, count, mac)) {
      logger.error(pdu->msg, pdu->N_bytes, "%s Dropping PDU", rrc->get_rb_name(lcid).c_str());
      return; // Discard
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
void pdcp_entity_lte::handle_um_drb_pdu(srslte::unique_byte_buffer_t pdu)
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

  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx PDU SN=%d", rrc->get_rb_name(lcid).c_str(), sn);

  st.next_pdcp_rx_sn = sn + 1;
  if (st.next_pdcp_rx_sn > maximum_pdcp_sn) {
    st.next_pdcp_rx_sn = 0;
    st.rx_hfn++;
  }

  // Pass to upper layers
  gw->write_pdu(lcid, std::move(pdu));
}

// DRBs mapped on RLC AM, without re-ordering (5.1.2.1.2)
void pdcp_entity_lte::handle_am_drb_pdu(srslte::unique_byte_buffer_t pdu)
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
  logger.debug(pdu->msg, pdu->N_bytes, "%s Rx SDU SN=%d", rrc->get_rb_name(lcid).c_str(), sn);

  // Update info on last PDU submitted to upper layers
  st.last_submitted_pdcp_rx_sn = sn;

  // Pass to upper layers
  gw->write_pdu(lcid, std::move(pdu));
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

  if (not cfg.status_report_required) {
    logger.info("Not sending PDCP Status Report as status report required is not set");
    return;
  }

  // Get First Missing Segment (FMS)
  uint32_t fms = 0;
  if (undelivered_sdus_queue.empty()) {
    fms = st.next_pdcp_tx_sn;
  } else {
    fms = undelivered_sdus_queue.begin()->first;
  }

  logger.debug("Status report: FMS=%d", fms);

  // Allocate Status Report PDU
  unique_byte_buffer_t pdu = make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Error allocating buffer for status report");
    return;
  }

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
  if (not undelivered_sdus_queue.empty()) {
    // First check size of bitmap
    uint32_t last_sn   = undelivered_sdus_queue.rbegin()->first;
    uint32_t bitmap_sz = std::ceil((float)(last_sn - (fms - 1)) / 8);
    memset(&pdu->msg[pdu->N_bytes], 0, bitmap_sz);
    logger.debug(
        "Setting status report bitmap. Last SN acked=%d, Last SN acked in sequence=%d, Bitmap size in bytes=%d",
        last_sn,
        fms - 1,
        bitmap_sz);
    for (auto it = undelivered_sdus_queue.begin(); it != undelivered_sdus_queue.end(); it++) {
      uint32_t offset      = it->first - fms;
      uint32_t bit_offset  = offset % 8;
      uint32_t byte_offset = offset / 8;
      pdu->msg[pdu->N_bytes + byte_offset] |= 1 << (7 - bit_offset);
    }
    pdu->N_bytes += bitmap_sz;
  }
  pdu->md.pdcp_sn = -1;

  // Write PDU to RLC
  rlc->write_sdu(lcid, std::move(pdu));

  return;
}

// Section 5.3.2 receive operation
void pdcp_entity_lte::handle_status_report_pdu(unique_byte_buffer_t pdu)
{
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
  for (auto it = undelivered_sdus_queue.begin(); it != undelivered_sdus_queue.end();) {
    if (it->first < fms) {
      stop_discard_timer(it->first);
      it = undelivered_sdus_queue.erase(it);
    } else {
      ++it;
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
    undelivered_sdus_queue.erase(sn);
    stop_discard_timer(sn);
  }
}
/****************************************************************************
 * TX PDUs Queue Helper
 ***************************************************************************/
bool pdcp_entity_lte::store_sdu(uint32_t sn, const unique_byte_buffer_t& sdu)
{
  logger.debug("Storing SDU in undelivered SDUs queue. SN=%d, Queue size=%ld", sn, undelivered_sdus_queue.size());

  // Check wether PDU is already in the queue
  if (undelivered_sdus_queue.find(sn) != undelivered_sdus_queue.end()) {
    logger.error("PDU already exists in the queue. TX_COUNT=%d", sn);
    return false;
  }

  // Make sure we don't associate more than half of the PDCP SN space of contiguous PDCP SDUs
  if (not undelivered_sdus_queue.empty()) {
    auto     fms_it = undelivered_sdus_queue.begin();
    uint32_t fms_sn = fms_it->first;
    int32_t  diff   = sn - fms_sn;
    if (diff > (int32_t)maximum_allocated_sns_window) {
      // This SN is too large to assign, it may cause HFN de-synchronization.
      logger.info("This SN is too large to assign. Discarding. SN=%d, FMS=%d, diff=%d, window=%d, queue_size=%d",
                  sn,
                  fms_sn,
                  diff,
                  maximum_allocated_sns_window,
                  undelivered_sdus_queue.size());
      return false;
    }
    if (diff < 0 && diff > -((int32_t)maximum_allocated_sns_window)) {
      // This SN is too large to assign, it may cause HFN de-synchronization.
      logger.info("This SN is too large to assign. Discarding. SN=%d, FMS=%d, diff=%d, window=%d, queue_size=%d",
                  sn,
                  fms_sn,
                  diff,
                  maximum_allocated_sns_window,
                  undelivered_sdus_queue.size());
      return false;
    }
  }

  // Copy PDU contents into queue
  unique_byte_buffer_t sdu_copy = make_byte_buffer();
  memcpy(sdu_copy->msg, sdu->msg, sdu->N_bytes);
  sdu_copy->N_bytes = sdu->N_bytes;

  // Metrics
  sdu_copy->set_timestamp();

  undelivered_sdus_queue.insert(std::make_pair(sn, std::move(sdu_copy)));
  return true;
}

/****************************************************************************
 * Discard functionality
 ***************************************************************************/
// Discard Timer Callback (discardTimer)
void pdcp_entity_lte::discard_callback::operator()(uint32_t timer_id)
{
  parent->logger.debug("Discard timer expired for PDU with SN = %d", discard_sn);

  // Discard PDU if unacknowledged
  if (parent->undelivered_sdus_queue.find(discard_sn) != parent->undelivered_sdus_queue.end()) {
    parent->undelivered_sdus_queue.erase(discard_sn);
    parent->logger.debug("Removed undelivered PDU with TX_COUNT=%d", discard_sn);
  } else {
    parent->logger.debug("Could not find PDU to discard. TX_COUNT=%d", discard_sn);
  }

  // Notify the RLC of the discard. It's the RLC to actually discard, if no segment was transmitted yet.
  parent->rlc->discard_sdu(parent->lcid, discard_sn);

  // Remove timer from map
  parent->stop_discard_timer(discard_sn);
}

/****************************************************************************
 * Handle delivery/failure notifications from RLC
 ***************************************************************************/
void pdcp_entity_lte::notify_delivery(const std::vector<uint32_t>& pdcp_sns)
{
  logger.info("Received delivery notification from RLC. Number of PDU notified=%ld", pdcp_sns.size());

  for (uint32_t sn : pdcp_sns) {
    logger.debug("Delivery notification received for PDU with SN=%d", sn);
    // Find undelivered PDU info
    std::map<uint32_t, unique_byte_buffer_t>::iterator it = undelivered_sdus_queue.find(sn);
    if (it == undelivered_sdus_queue.end()) {
      logger.warning("Could not find PDU for delivery notification. Notified SN=%d", sn);
    } else {
      // Metrics
      tx_pdu_ack_latency_ms.push(std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::high_resolution_clock::now() - it->second->get_timestamp())
                                     .count());
      metrics.num_tx_acked_bytes += it->second->N_bytes;
      metrics.num_tx_buffered_pdus_bytes -= it->second->N_bytes;

      // Remove PDU and disarm timer.
      undelivered_sdus_queue.erase(sn);
      stop_discard_timer(sn);
    }
  }
}

void pdcp_entity_lte::notify_failure(const std::vector<uint32_t>& pdcp_sns)
{
  logger.info("Received failure notification from RLC. Number of PDU notified=%ld", pdcp_sns.size());

  for (uint32_t sn : pdcp_sns) {
    logger.info("Failure notification received for PDU with SN=%d", sn);
    // Find undelivered PDU info
    std::map<uint32_t, unique_byte_buffer_t>::iterator it = undelivered_sdus_queue.find(sn);
    if (it == undelivered_sdus_queue.end()) {
      logger.info("Could not find PDU for failure notification. Notified SN=%d", sn);
    } else {
      // Remove PDU and disarm timer.
      undelivered_sdus_queue.erase(sn);
      stop_discard_timer(sn);
    }
  }
}

/****************************************************************************
 * Config checking helper
 ***************************************************************************/
bool pdcp_entity_lte::check_valid_config()
{
  if (cfg.sn_len != PDCP_SN_LEN_5 && cfg.sn_len != PDCP_SN_LEN_7 && cfg.sn_len != PDCP_SN_LEN_12) {
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
  if (cfg.sn_len == PDCP_SN_LEN_12 && is_srb()) {
    logger.error("Trying to configure SRB with SN LEN of 12.");
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

void pdcp_entity_lte::set_bearer_state(const pdcp_lte_state_t& state)
{
  st = state;
}

std::map<uint32_t, srslte::unique_byte_buffer_t> pdcp_entity_lte::get_buffered_pdus()
{
  logger.info("Buffered PDUs requested, buffer_size=%d", undelivered_sdus_queue.size());

  std::map<uint32_t, srslte::unique_byte_buffer_t> cpy{};
  // Deep copy undelivered SDUs
  // TODO: investigate wheter the deep copy can be avoided by moving the undelivered SDU queue.
  // That can only be done just before the PDCP is disabled though.
  for (auto it = undelivered_sdus_queue.begin(); it != undelivered_sdus_queue.end(); it++) {
    cpy[it->first]    = make_byte_buffer();
    (*cpy[it->first]) = *(it->second);
    logger.debug(it->second->msg, it->second->N_bytes, "Forwarding buffered PDU with SN=%d", it->first);
  }
  return cpy;
}

uint32_t pdcp_entity_lte::nof_discard_timers() const
{
  return std::count_if(
      discard_timers.begin(), discard_timers.end(), [](const unique_timer& t) { return t.is_running(); });
}

unique_timer* pdcp_entity_lte::get_discard_timer(uint32_t sn)
{
  // Note: When SN>max PDCP SN (Status report case), the position will be the last in the vector of discard timers
  if (not discard_timers.empty()) {
    uint32_t sn_idx = std::min((uint32_t)sn, (uint32_t)(discard_timers.size() - 1));
    return &discard_timers[sn_idx];
  }
  return nullptr;
}

void pdcp_entity_lte::stop_discard_timer(uint32_t sn)
{
  unique_timer* timer = get_discard_timer(sn);
  if (timer != nullptr) {
    timer->stop();
  }
}

/****************************************************************************
 * Metrics helpers
 ***************************************************************************/
pdcp_bearer_metrics_t pdcp_entity_lte::get_metrics()
{
  metrics.num_tx_buffered_pdus       = undelivered_sdus_queue.size();
  metrics.num_tx_buffered_pdus_bytes = 0;
  for (auto sdu_it = undelivered_sdus_queue.begin(); sdu_it != undelivered_sdus_queue.end(); ++sdu_it) {
    metrics.num_tx_buffered_pdus_bytes += sdu_it->second->N_bytes; //< Number of bytes of PDUs waiting for ACK
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

} // namespace srslte
