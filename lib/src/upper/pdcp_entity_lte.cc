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
#include "srslte/common/security.h"

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

  st.next_pdcp_tx_sn           = 0;
  st.tx_hfn                    = 0;
  st.rx_hfn                    = 0;
  st.next_pdcp_rx_sn           = 0;
  maximum_pdcp_sn              = (1 << cfg.sn_len) - 1;
  st.last_submitted_pdcp_rx_sn = maximum_pdcp_sn;

  logger.info("Init %s with bearer ID: %d", rrc->get_rb_name(lcid).c_str(), cfg.bearer_id);
  logger.info("SN len bits: %d, SN len bytes: %d, reordering window: %d, Maximum SN: %d, discard timer: %d ms",
              cfg.sn_len,
              cfg.hdr_len_bytes,
              reordering_window,
              maximum_pdcp_sn,
              static_cast<uint32_t>(cfg.discard_timer));

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
  } else {
    // Only reset counter in RLC-UM
    if (rlc->rb_is_um(lcid)) {
      st.next_pdcp_tx_sn = 0;
      st.tx_hfn          = 0;
      st.rx_hfn          = 0;
      st.next_pdcp_rx_sn = 0;
    }
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
void pdcp_entity_lte::write_sdu(unique_byte_buffer_t sdu)
{
  if (rlc->sdu_queue_is_full(lcid)) {
    logger.info(sdu->msg, sdu->N_bytes, "Dropping %s SDU due to full queue", rrc->get_rb_name(lcid).c_str());
    return;
  }

  // Get COUNT to be used with this packet
  uint32_t tx_count = COUNT(st.tx_hfn, st.next_pdcp_tx_sn);

  // If the bearer is mapped to RLC AM, save TX_COUNT and a copy of the PDU.
  // This will be used for reestablishment, where unack'ed PDUs will be re-transmitted.
  // PDUs will be removed from the queue, either when the lower layers will report
  // a succesfull transmission or when the discard timer expires.
  // Status report will also use this queue, to know the First Missing SDU (FMS).
  if (!rlc->rb_is_um(lcid)) {
    store_sdu(st.next_pdcp_tx_sn, sdu);
  }

  // check for pending security config in transmit direction
  if (enable_security_tx_sn != -1 && enable_security_tx_sn == static_cast<int32_t>(tx_count)) {
    enable_integrity(DIRECTION_TX);
    enable_encryption(DIRECTION_TX);
    enable_security_tx_sn = -1;
  }

  write_data_header(sdu, tx_count);

  // Start discard timer
  if (cfg.discard_timer != pdcp_discard_timer_t::infinity) {
    timer_handler::unique_timer discard_timer = task_sched.get_unique_timer();
    discard_callback            discard_fnc(this, st.next_pdcp_tx_sn);
    discard_timer.set(static_cast<uint32_t>(cfg.discard_timer), discard_fnc);
    discard_timer.run();
    discard_timers_map.insert(std::make_pair(tx_count, std::move(discard_timer)));
    logger.debug("Discard Timer set for SN %u. Timeout: %ums", tx_count, static_cast<uint32_t>(cfg.discard_timer));
  }

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
              st.next_pdcp_tx_sn,
              srslte_direction_text[integrity_direction],
              srslte_direction_text[encryption_direction]);

  // Set SDU metadata for RLC AM
  sdu->md.pdcp_sn = st.next_pdcp_tx_sn;

  // Increment NEXT_PDCP_TX_SN and TX_HFN
  st.next_pdcp_tx_sn++;
  if (st.next_pdcp_tx_sn > maximum_pdcp_sn) {
    st.tx_hfn++;
    st.next_pdcp_tx_sn = 0;
  }

  // Pass PDU to lower layers
  rlc->write_sdu(lcid, std::move(sdu));
}

// RLC interface
void pdcp_entity_lte::write_pdu(unique_byte_buffer_t pdu)
{
  // drop control PDUs
  if (is_drb() && is_control_pdu(pdu)) {
    logger.info("Dropping PDCP control PDU");
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

  if (is_srb()) {
    handle_srb_pdu(std::move(pdu));
  } else if (is_drb() && rlc->rb_is_um(lcid)) {
    handle_um_drb_pdu(std::move(pdu));
  } else if (is_drb() && !rlc->rb_is_um(lcid)) {
    handle_am_drb_pdu(std::move(pdu));
  } else {
    logger.error("Invalid PDCP/RLC configuration");
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
    return; // Discard
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
 * TX PDUs Queue Helper
 ***************************************************************************/
bool pdcp_entity_lte::store_sdu(uint32_t tx_count, const unique_byte_buffer_t& sdu)
{
  logger.debug(
      "Storing SDU in undelivered SDUs queue. TX_COUNT=%d, Queue size=%ld", tx_count, undelivered_sdus_queue.size());

  // Check wether PDU is already in the queue
  if (undelivered_sdus_queue.find(tx_count) != undelivered_sdus_queue.end()) {
    logger.error("PDU already exists in the queue. TX_COUNT=%d", tx_count);
    return false;
  }

  // Copy PDU contents into queue
  unique_byte_buffer_t sdu_copy = make_byte_buffer();
  memcpy(sdu_copy->msg, sdu->msg, sdu->N_bytes);
  sdu_copy->N_bytes = sdu->N_bytes;

  undelivered_sdus_queue.insert(std::make_pair(tx_count, std::move(sdu_copy)));
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
  // NOTE: this will delete the callback. It *must* be the last instruction.
  parent->discard_timers_map.erase(discard_sn);
}

/****************************************************************************
 * Handle delivery notifications from RLC
 ***************************************************************************/
void pdcp_entity_lte::notify_delivery(const std::vector<uint32_t>& pdcp_sns)
{
  logger.debug("Received delivery notification from RLC. Number of PDU notified=%ld", pdcp_sns.size());

  for (uint32_t sn : pdcp_sns) {
    // Find undelivered PDU info
    std::map<uint32_t, unique_byte_buffer_t>::iterator it = undelivered_sdus_queue.find(sn);
    if (it == undelivered_sdus_queue.end()) {
      logger.warning("Could not find PDU for delivery notification. Notified SN=%d", sn);
      return;
    }

    // If ACK'ed bytes are equal to (or exceed) PDU size, remove PDU and disarm timer.
    undelivered_sdus_queue.erase(sn);
    discard_timers_map.erase(sn);
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
  std::map<uint32_t, srslte::unique_byte_buffer_t> cpy{};
  // Deep copy undelivered SDUs
  // TODO: investigate wheter the deep copy can be avoided by moving the undelivered SDU queue.
  // That can only be done just before the PDCP is disabled though.
  for (auto it = undelivered_sdus_queue.begin(); it != undelivered_sdus_queue.end(); it++) {
    cpy[it->first]    = make_byte_buffer();
    (*cpy[it->first]) = *(it->second);
  }
  return cpy;
}

} // namespace srslte
