/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/rlc/rlc_am_nr.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_am_nr_packing.h"
#include "srsran/srslog/event_trace.h"
#include <iostream>

#define RLC_AM_NR_WINDOW_SIZE 2048

namespace srsran {

const static uint32_t max_tx_queue_size = 128;
/****************************************************************************
 * RLC AM NR entity
 ***************************************************************************/

/***************************************************************************
 *  Tx subclass implementation
 ***************************************************************************/
rlc_am_nr_tx::rlc_am_nr_tx(rlc_am* parent_) : parent(parent_), rlc_am_base_tx(&parent_->logger) {}

bool rlc_am_nr_tx::configure(const rlc_config_t& cfg_)
{
  cfg = cfg_.am_nr;

  if (cfg.tx_sn_field_length != rlc_am_nr_sn_size_t::size12bits) {
    logger->warning("RLC AM NR only supporst 12 bit SN length.");
    return false;
  }

  if (cfg_.tx_queue_length > max_tx_queue_size) {
    logger->error("Configuring Tx queue length of %d PDUs too big. Maximum value is %d.",
                  cfg_.tx_queue_length,
                  max_tx_queue_size);
    return false;
  }

  mod_nr = (cfg.tx_sn_field_length == rlc_am_nr_sn_size_t::size12bits) ? 4096 : 262144;

  tx_enabled = true;

  return true;
}

bool rlc_am_nr_tx::has_data()
{
  return do_status() ||                  // if we have a status PDU to transmit
         tx_sdu_queue.get_n_sdus() != 1; // or if there is a SDU queued up for transmission
}

uint32_t rlc_am_nr_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  logger->debug("MAC opportunity - %d bytes", nof_bytes);

  std::lock_guard<std::mutex> lock(mutex);

  if (not tx_enabled) {
    logger->debug("RLC entity not active. Not generating PDU.");
    return 0;
  }
  logger->debug("tx_window size - %zu PDUs", tx_window.size());

  // Tx STATUS if requested
  if (do_status()) {
    unique_byte_buffer_t tx_pdu = srsran::make_byte_buffer();
    if (tx_pdu == nullptr) {
      logger->error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return 0;
    }
    build_status_pdu(tx_pdu.get(), nof_bytes);
    memcpy(payload, tx_pdu->msg, tx_pdu->N_bytes);
    logger->debug("Status PDU built - %d bytes", tx_pdu->N_bytes);
    return tx_pdu->N_bytes;
  }

  // Section 5.2.2.3 in TS 36.311, if tx_window is full and retx_queue empty, retransmit PDU
  // TODO

  // RETX if required
  if (not retx_queue.empty()) {
    logger->info("Retx required. Retx queue size: %d", retx_queue.size());
    unique_byte_buffer_t tx_pdu = srsran::make_byte_buffer();
    if (tx_pdu == nullptr) {
      logger->error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return 0;
    }
    int retx_err = build_retx_pdu(tx_pdu, nof_bytes);
    if (retx_err >= 0 && tx_pdu->N_bytes <= nof_bytes) {
      memcpy(payload, tx_pdu->msg, tx_pdu->N_bytes);
      return tx_pdu->N_bytes;
    }
  }

  // Read new SDU from TX queue
  if (tx_sdu_queue.is_empty()) {
    logger->info("No data available to be sent");
    return 0;
  }

  unique_byte_buffer_t tx_sdu;
  logger->debug("Reading from RLC SDU queue. Queue size %d", tx_sdu_queue.size());
  do {
    tx_sdu = tx_sdu_queue.read();
  } while (tx_sdu == nullptr && tx_sdu_queue.size() != 0);

  if (tx_sdu != nullptr) {
    logger->debug("Read RLC SDU - %d bytes", tx_sdu->N_bytes);
  }

  uint16_t hdr_size = 2;
  if (tx_sdu->N_bytes + hdr_size > nof_bytes) {
    logger->warning("Segmentation not supported yet");
    return 0;
  }

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing tx_next
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window.add_pdu(st.tx_next);
  tx_pdu.buf                = srsran::make_byte_buffer();
  if (tx_pdu.buf == nullptr) {
    logger->error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return 0;
  }

  memcpy(tx_pdu.buf->msg, tx_sdu->msg, tx_sdu->N_bytes);
  tx_pdu.buf->N_bytes = tx_sdu->N_bytes;

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll();
  hdr.si                     = rlc_nr_si_field_t::full_sdu;
  hdr.sn_size                = rlc_am_nr_sn_size_t::size12bits;
  hdr.sn                     = st.tx_next;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger->info, hdr);

  // Write header
  uint32_t len = rlc_am_nr_write_data_pdu_header(hdr, tx_sdu.get());
  if (len > nof_bytes) {
    logger->error("Error writing AMD PDU header");
  }

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % MOD;

  memcpy(payload, tx_sdu->msg, tx_sdu->N_bytes);
  logger->debug("Wrote RLC PDU - %d bytes", tx_sdu->N_bytes);

  return tx_sdu->N_bytes;
}

int rlc_am_nr_tx::build_retx_pdu(unique_byte_buffer_t& tx_pdu, uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    logger->error("In build_retx_pdu(): retx_queue is empty");
    return SRSRAN_ERROR;
  }

  rlc_amd_retx_t retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window.has_sn(retx.sn)) {
    logger->warning("%s SN=%d not in Tx window. Ignoring retx.", parent->rb_name, retx.sn);
    retx_queue.pop();
    if (!retx_queue.empty()) {
      retx = retx_queue.front();
    } else {
      logger->warning("%s empty retx queue, cannot provide retx PDU", parent->rb_name);
      return SRSRAN_ERROR;
    }
  }
  // Update & write header
  rlc_am_nr_pdu_header_t new_header = tx_window[retx.sn].header;
  new_header.p                      = 0;
  uint32_t hdr_len                  = rlc_am_nr_write_data_pdu_header(new_header, tx_pdu.get());

  // Check if we exceed allocated number of bytes
  if (hdr_len + tx_window[retx.sn].buf->N_bytes > nof_bytes) {
    logger->warning("%s segmentation not supported yet. Cannot provide retx PDU", parent->rb_name);
    return SRSRAN_ERROR;
  }
  // TODO Consider re-segmentation

  memcpy(&tx_pdu->msg[hdr_len], tx_window[retx.sn].buf->msg, tx_window[retx.sn].buf->N_bytes);
  tx_pdu->N_bytes += tx_window[retx.sn].buf->N_bytes;

  retx_queue.pop();

  logger->info(tx_window[retx.sn].buf->msg,
               tx_window[retx.sn].buf->N_bytes,
               "%s Original SDU SN=%d (%d B) (attempt %d/%d)",
               parent->rb_name,
               retx.sn,
               tx_window[retx.sn].buf->N_bytes,
               tx_window[retx.sn].retx_count + 1,
               cfg.max_retx_thresh);
  logger->info(tx_pdu->msg, tx_pdu->N_bytes, "%s ReTx PDU SN=%d (%d B)", parent->rb_name, retx.sn, tx_pdu->N_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger->debug, new_header);

  // debug_state();
  return SRSRAN_SUCCESS;
}

uint32_t rlc_am_nr_tx::build_status_pdu(byte_buffer_t* payload, uint32_t nof_bytes)
{
  logger->info("Generating Status PDU. Bytes available:%d", nof_bytes);
  rlc_am_nr_status_pdu_t tx_status;
  int                    pdu_len = rx->get_status_pdu(&tx_status, nof_bytes);
  if (pdu_len == SRSRAN_ERROR) {
    logger->debug("%s Deferred Status PDU. Cause: Failed to acquire Rx lock", rb_name);
    pdu_len = 0;
  } else if (pdu_len > 0 && nof_bytes >= static_cast<uint32_t>(pdu_len)) {
    logger->debug("Generated Status PDU. Bytes:%d", pdu_len);
    log_rlc_am_nr_status_pdu_to_string(logger->info, "%s Tx status PDU - %s", &tx_status, rb_name);
    pdu_len = rlc_am_nr_write_status_pdu(tx_status, rlc_am_nr_sn_size_t::size12bits, payload);
  } else {
    logger->info("%s Cannot tx status PDU - %d bytes available, %d bytes required", rb_name, nof_bytes, pdu_len);
    pdu_len = 0;
  }

  return payload->N_bytes;
}

void rlc_am_nr_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (not tx_enabled) {
    return;
  }

  rlc_am_nr_status_pdu_t status = {};
  logger->debug(payload, nof_bytes, "%s Rx control PDU", parent->rb_name);
  rlc_am_nr_read_status_pdu(payload, nof_bytes, rlc_am_nr_sn_size_t::size12bits, &status);
  log_rlc_am_nr_status_pdu_to_string(logger->info, "%s Rx Status PDU: %s", &status, parent->rb_name);
  // Local variables for handling Status PDU will be updated with lock
  /*
   * - if the SN of the corresponding RLC SDU falls within the range
   *   TX_Next_Ack <= SN < = the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer:
   *   - consider the RLC SDU or the RLC SDU segment for which a negative acknowledgement was received for
   *     retransmission.
   */
  // Process ACKs
  uint32_t stop_sn = status.N_nack == 0
                         ? status.ack_sn
                         : status.nacks[0].nack_sn - 1; // Stop processing ACKs at the first NACK, if it exists.
  if (stop_sn > st.tx_next) {
    logger->error("Rx'ed ACK or NACK larger than TX_NEXT. Ignoring status report");
    return;
  }
  for (uint32_t sn = st.tx_next_ack; sn < stop_sn; sn++) {
    if (tx_window.has_sn(sn)) {
      tx_window.remove_pdu(sn);
      st.tx_next_ack = sn + 1;
      // TODO notify PDCP
    } else {
      logger->error("Missing ACKed SN from TX window");
      break;
    }
  }

  // Process N_acks
  for (uint32_t nack_idx = 0; nack_idx < status.N_nack; nack_idx++) {
    if (st.tx_next_ack <= status.nacks[nack_idx].nack_sn && status.nacks[nack_idx].nack_sn <= st.tx_next) {
      uint32_t nack_sn = status.nacks[nack_idx].nack_sn;
      if (tx_window.has_sn(nack_sn)) {
        auto& pdu = tx_window[nack_sn];

        // add to retx queue if it's not already there
        if (not retx_queue.has_sn(nack_sn)) {
          // increment Retx counter and inform upper layers if needed
          pdu.retx_count++;
          // check_sn_reached_max_retx(nack_sn);

          rlc_amd_retx_t& retx = retx_queue.push();
          srsran_expect(tx_window[nack_sn].rlc_sn == nack_sn,
                        "Incorrect RLC SN=%d!=%d being accessed",
                        tx_window[nack_sn].rlc_sn,
                        nack_sn);
          retx.sn         = nack_sn;
          retx.is_segment = false;
          retx.so_start   = 0;
          retx.so_end     = pdu.buf->N_bytes;
        }
      }
    }
  }

  /**
   * Section 5.3.3.3: Reception of a STATUS report
   * - if the STATUS report comprises a positive or negative acknowledgement for the RLC SDU with sequence
   *   number equal to POLL_SN:
   *   - if t-PollRetransmit is running:
   *     - stop and reset t-PollRetransmit.
   */
}

uint32_t rlc_am_nr_tx::get_buffer_state()
{
  uint32_t tx_queue      = 0;
  uint32_t prio_tx_queue = 0;
  get_buffer_state(tx_queue, prio_tx_queue);
  return tx_queue + prio_tx_queue;
}

void rlc_am_nr_tx::get_buffer_state(uint32_t& n_bytes_new, uint32_t& n_bytes_prio)
{
  logger->debug("Buffer state requested, %s", rb_name);
  std::lock_guard<std::mutex> lock(mutex);

  logger->debug("%s Buffer state - do_status=%s", rb_name, do_status() ? "yes" : "no");

  // Bytes needed for status report
  if (do_status()) {
    n_bytes_prio += rx->get_status_pdu_length();
    logger->debug("%s Buffer state - total status report: %d bytes", rb_name, n_bytes_prio);
  }

  // Bytes needed for retx
  if (not retx_queue.empty()) {
    rlc_amd_retx_t& retx = retx_queue.front();
    logger->debug("%s Buffer state - retx - SN=%d, Segment: %s, %d:%d",
                  parent->rb_name,
                  retx.sn,
                  retx.is_segment ? "true" : "false",
                  retx.so_start,
                  retx.so_end);
    if (tx_window.has_sn(retx.sn)) {
      int req_bytes     = retx.so_end - retx.so_start;
      int hdr_req_bytes = retx.is_segment ? 4 : 2; // Segmentation not supported yet
      if (req_bytes <= 0) {
        logger->error("In get_buffer_state(): Removing retx.sn=%d from queue", retx.sn);
        retx_queue.pop();
      } else {
        n_bytes_prio += (req_bytes + hdr_req_bytes);
        logger->debug("Buffer state - retx: %d bytes", n_bytes_prio);
      }
    }
  }

  // Bytes needed for tx SDUs
  uint32_t n_sdus = tx_sdu_queue.get_n_sdus();
  n_bytes_new += tx_sdu_queue.size_bytes();

  // Room needed for fixed header of data PDUs
  n_bytes_new += 2 * n_sdus; // TODO make header size configurable
  logger->debug("%s Total buffer state - %d SDUs (%d B)", rb_name, n_sdus, n_bytes_new + n_bytes_prio);

  if (bsr_callback) {
    logger->debug("%s Calling BSR callback - %d new_tx, %d prio bytes", parent->rb_name, n_bytes_new, n_bytes_prio);
    bsr_callback(parent->lcid, n_bytes_new, n_bytes_prio);
  }
}

uint8_t rlc_am_nr_tx::get_pdu_poll()
{
  uint8_t poll = 0;
  if (cfg.poll_pdu > 0) {
    if (st.pdu_without_poll >= (uint32_t)cfg.poll_pdu) {
      poll                = 1;
      st.pdu_without_poll = 0;
    } else {
      st.pdu_without_poll++;
    }
  }
  return poll;
}

bool rlc_am_nr_tx::do_status()
{
  return rx->get_do_status();
}

void rlc_am_nr_tx::reestablish()
{
  stop();
}

void rlc_am_nr_tx::discard_sdu(uint32_t discard_sn) {}

bool rlc_am_nr_tx::sdu_queue_is_full()
{
  return false;
}

void rlc_am_nr_tx::empty_queue() {}

void rlc_am_nr_tx::stop() {}
/*
 * Window helpers
 */
uint32_t rlc_am_nr_tx::tx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.tx_next_ack) % mod_nr;
}

bool rlc_am_nr_tx::inside_tx_window(uint32_t sn)
{
  // TX_Next_Ack <= SN < TX_Next_Ack + AM_Window_Size
  return tx_mod_base_nr(sn) < RLC_AM_NR_WINDOW_SIZE;
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_nr_rx::rlc_am_nr_rx(rlc_am* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  status_prohibit_timer(parent->timers->get_unique_timer()),
  reassembly_timer(parent->timers->get_unique_timer()),
  rlc_am_base_rx(parent_, &parent_->logger)
{}

bool rlc_am_nr_rx::configure(const rlc_config_t& cfg_)
{
  cfg = cfg_.am_nr;

  // Configure status prohibit timer
  if (cfg.t_status_prohibit > 0) {
    status_prohibit_timer.set(static_cast<uint32_t>(cfg.t_status_prohibit),
                              [this](uint32_t timerid) { timer_expired(timerid); });
  }

  // Configure t_reassembly timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.t_reassembly), [this](uint32_t timerid) { timer_expired(timerid); });
    logger->info("Configured reassembly timer. t-Reassembly=%d ms", cfg.t_reassembly);
  }

  mod_nr = (cfg.rx_sn_field_length == rlc_am_nr_sn_size_t::size12bits) ? 4096 : 262144;
  return true;
}

void rlc_am_nr_rx::stop() {}

void rlc_am_nr_rx::reestablish()
{
  stop();
}

void rlc_am_nr_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Get AMD PDU Header
  rlc_am_nr_pdu_header_t header = {};
  uint32_t hdr_len = rlc_am_nr_read_data_pdu_header(payload, nof_bytes, rlc_am_nr_sn_size_t::size12bits, &header);

  logger->info(payload, nof_bytes, "%s Rx data PDU SN=%d (%d B)", parent->rb_name, header.sn, nof_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger->debug, header);

  // Check wether SDU is within Rx Window
  if (!inside_rx_window(header.sn)) {
    logger->info("%s SN=%d outside rx window [%d:%d] - discarding",
                 parent->rb_name,
                 header.sn,
                 st.rx_next,
                 st.rx_next + RLC_AM_NR_WINDOW_SIZE);
    return;
  }

  // Section 5.2.3.2.2, discard duplicate PDUs
  if (rx_window.has_sn(header.sn)) {
    logger->info("%s Discarding duplicate SN=%d", parent->rb_name, header.sn);
    return;
  }

  // Write to rx window
  if (header.si == rlc_nr_si_field_t::full_sdu) {
    // Full SDU received. Add SDU to Rx Window and copy full PDU into SDU buffer.
    rlc_amd_rx_sdu_nr_t& rx_sdu = rx_window.add_pdu(header.sn);
    rx_sdu.buf                  = srsran::make_byte_buffer();
    if (rx_sdu.buf == nullptr) {
      logger->error("Fatal Error: Couldn't allocate PDU in %s.", __FUNCTION__);
      rx_window.remove_pdu(header.sn);
      return;
    }
    rx_sdu.buf->set_timestamp();

    // check available space for payload
    if (nof_bytes > rx_sdu.buf->get_tailroom()) {
      logger->error("%s Discarding SN=%d of size %d B (available space %d B)",
                    parent->rb_name,
                    header.sn,
                    nof_bytes,
                    rx_sdu.buf->get_tailroom());
      return;
    }
    memcpy(rx_sdu.buf->msg, payload + hdr_len, nof_bytes - hdr_len); // Don't copy header
    rx_sdu.buf->N_bytes   = nof_bytes - hdr_len;
    rx_sdu.fully_received = true;
    write_to_upper_layers(parent->lcid, std::move(rx_window[header.sn].buf));
  } else {
    // Check if all bytes of the RLC SDU with SN = x are received:
    // TODO
    if (header.si == rlc_nr_si_field_t::first_segment) { // Check whether it's a full SDU
    } else if (header.si == rlc_nr_si_field_t::last_segment) {
    } else if (header.si == rlc_nr_si_field_t::neither_first_nor_last_segment) {
    }
  }

  // Check poll bit
  if (header.p) {
    logger->info("%s Status packet requested through polling bit", parent->rb_name);
    do_status = true;
    status_prohibit_timer.stop();
  }

  debug_state();

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  // Update Rx_Next_Highest
  if (rx_mod_base_nr(header.sn) >= rx_mod_base_nr(st.rx_next_highest)) {
    st.rx_next_highest = (header.sn + 1) % MOD;
  }

  // Update RX_Highest_Status
  /*
   * - if x = RX_Highest_Status,
   *   - update RX_Highest_Status to the SN of the first RLC SDU with SN > current RX_Highest_Status for which not
   * all bytes have been received.
   */
  if (rx_mod_base_nr(header.sn) == rx_mod_base_nr(st.rx_highest_status)) {
    uint32_t sn_upd     = 0;
    uint32_t window_top = st.rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = st.rx_highest_status; sn_upd < window_top; ++sn_upd) {
      if (rx_window.has_sn(sn_upd)) {
        if (not rx_window[sn_upd].fully_received) {
          break; // first SDU not fully received
        }
      } else {
        break; // first SDU not fully received
      }
    }
    // Update to the SN of the first SDU with missing bytes.
    // If it not exists, update to the end of the rx_window.
    st.rx_highest_status = sn_upd;
  }

  /*
   * - if x = RX_Next:
   *   - update RX_Next to the SN of the first RLC SDU with SN > current RX_Next for which not all bytes
   *     have been received.
   */
  if (rx_mod_base_nr(header.sn) == rx_mod_base_nr(st.rx_next)) {
    uint32_t sn_upd     = 0;
    uint32_t window_top = st.rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = st.rx_next; sn_upd < window_top; ++sn_upd) {
      if (rx_window.has_sn(sn_upd)) {
        if (not rx_window[sn_upd].fully_received) {
          break; // first SDU not fully received
        }
        // RX_Next serves as the lower edge of the receiving window
        // As such, we remove any SDU from the window if we update this value
        rx_window.remove_pdu(sn_upd);
      } else {
        break; // first SDU not fully received
      }
    }
    // Update to the SN of the first SDU with missing bytes.
    // If it not exists, update to the end of the rx_window.
    st.rx_next = sn_upd;
  }

  if (reassembly_timer.is_running()) {
    // if t-Reassembly is running:
    /*
     * - if RX_Next_Status_Trigger = RX_Next; or
     * - if RX_Next_Status_Trigger = RX_Next + 1 and there is no missing byte segment of the SDU associated with
     *   SN = RX_Next before the last byte of all received segments of this SDU; or
     * - if RX_Next_Status_Trigger falls outside of the receiving window and RX_Next_Status_Trigger is not equal
     *   to RX_Next + AM_Window_Size:
     * - stop and reset t-Reassembly.
     */
  } else {
    /*
     * - if RX_Next_Highest> RX_Next +1; or
     * - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
     *   with SN = RX_Next before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    bool restart_reassembly_timer = false;
    if (st.rx_next_highest > st.rx_next + 1) {
      restart_reassembly_timer = true;
    }
    if (st.rx_next_highest == st.rx_next + 1 &&
        rx_window[st.rx_next + 1].fully_received == false) { // TODO: does the last by need to be received?
      restart_reassembly_timer = true;
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_next_status_trigger = st.rx_next_highest;
    }
  }
}

/*
 * Status PDU
 */
uint32_t rlc_am_nr_rx::get_status_pdu(rlc_am_nr_status_pdu_t* status, uint32_t max_len)
{
  std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
  if (not lock.owns_lock()) {
    return SRSRAN_ERROR;
  }

  status->N_nack = 0;
  status->ack_sn = st.rx_next; // Start with the lower end of the window
  byte_buffer_t tmp_buf;

  uint32_t i = status->ack_sn;
  while (rx_mod_base_nr(i) <= rx_mod_base_nr(st.rx_highest_status)) {
    if (rx_window.has_sn(i) || i == st.rx_highest_status) {
      // only update ACK_SN if this SN has been received, or if we reached the maximum possible SN
      status->ack_sn = i;
    } else {
      status->nacks[status->N_nack].nack_sn = i;
      status->N_nack++;
    }

    // make sure we don't exceed grant size (FIXME)
    rlc_am_nr_write_status_pdu(*status, rlc_am_nr_sn_size_t::size12bits, &tmp_buf);
    // TODO
    i = (i + 1) % MOD;
  }

  if (max_len != UINT32_MAX) {
    // UINT32_MAX is used just to querry the status PDU length
    if (status_prohibit_timer.is_valid()) {
      status_prohibit_timer.run();
    }
  }
  return tmp_buf.N_bytes;
}

uint32_t rlc_am_nr_rx::get_status_pdu_length()
{
  rlc_am_nr_status_pdu_t tmp_status; // length for no NACKs
  return get_status_pdu(&tmp_status, UINT32_MAX);
}

bool rlc_am_nr_rx::get_do_status()
{
  return do_status.load(std::memory_order_relaxed) && not status_prohibit_timer.is_running();
}

void rlc_am_nr_rx::timer_expired(uint32_t timeout_id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Status Prohibit
  if (status_prohibit_timer.is_valid() && status_prohibit_timer.id() == timeout_id) {
    logger->debug("%s Status prohibit timer expired after %dms", parent->rb_name, status_prohibit_timer.duration());
    return;
  }

  // Reassembly
  if (reassembly_timer.is_valid() && reassembly_timer.id() == timeout_id) {
    logger->debug("%s Reassembly timer expired after %dms", parent->rb_name, reassembly_timer.duration());
    /*
     * 5.2.3.2.4 Actions when t-Reassembly expires:
     * - update RX_Highest_Status to the SN of the first RLC SDU with SN >= RX_Next_Status_Trigger for which not
     *   all bytes have been received;
     * - if RX_Next_Highest> RX_Highest_Status +1: or
     * - if RX_Next_Highest = RX_Highest_Status + 1 and there is at least one missing byte segment of the SDU
     *   associated with SN = RX_Highest_Status before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    for (uint32_t tmp_sn = st.rx_next_status_trigger; tmp_sn < st.rx_next_status_trigger + RLC_AM_WINDOW_SIZE;
         tmp_sn++) {
      if (not rx_window.has_sn(tmp_sn) || not rx_window[tmp_sn].fully_received) {
        st.rx_highest_status = tmp_sn;
        break;
      }
    }
    bool restart_reassembly_timer = false;
    if (st.rx_next_highest > st.rx_highest_status + 1) {
      restart_reassembly_timer = true;
    }
    if (st.rx_next_highest == st.rx_highest_status + 1 && not rx_window[st.rx_next_highest].fully_received) {
      restart_reassembly_timer = true;
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_next_status_trigger = st.rx_next_highest;
    }

    /* 5.3.4 Status reporting:
     * - The receiving side of an AM RLC entity shall trigger a STATUS report when t-Reassembly expires.
     *   NOTE 2: The expiry of t-Reassembly triggers both RX_Highest_Status to be updated and a STATUS report to be
     *   triggered, but the STATUS report shall be triggered after RX_Highest_Status is updated.
     */
    do_status = true;
    return;
  }
}

void rlc_am_nr_rx::write_to_upper_layers(uint32_t lcid, unique_byte_buffer_t sdu)
{
  uint32_t nof_bytes = sdu->N_bytes;
  parent->pdcp->write_pdu(lcid, std::move(sdu));
  std::lock_guard<std::mutex> lock(parent->metrics_mutex);
  parent->metrics.num_rx_sdus++;
  parent->metrics.num_rx_sdu_bytes += nof_bytes;
}

/*
 * Window Helpers
 */
uint32_t rlc_am_nr_rx::rx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.rx_next) % mod_nr;
}

bool rlc_am_nr_rx::inside_rx_window(uint32_t sn)
{
  // RX_Next <= SN < RX_Next + AM_Window_Size
  return rx_mod_base_nr(sn) < RLC_AM_NR_WINDOW_SIZE;
}

/*
 * Metrics
 */
uint32_t rlc_am_nr_rx::get_sdu_rx_latency_ms()
{
  return 0;
}

uint32_t rlc_am_nr_rx::get_rx_buffered_bytes()
{
  return 0;
}

/*
 * Debug Helpers
 */
void rlc_am_nr_rx::debug_state()
{
  logger->debug("RX entity state: Rx_Next %d, Rx_Next_Status_Trigger %d, Rx_Highest_Status %d, Rx_Next_Highest",
                st.rx_next,
                st.rx_next_status_trigger,
                st.rx_highest_status,
                st.rx_next_highest);
}
} // namespace srsran
