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
#define MOD_NR 4096
#define RX_MOD_BASE_NR(x) (((x)-rx_next) % MOD_NR)
//#define TX_MOD_BASE_NR(x) (((x)-vt_a) % MOD_NR)

namespace srsran {

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

  /*
    if (cfg_.tx_queue_length > MAX_SDUS_PER_RLC_PDU) {
      logger.error("Configuring Tx queue length of %d PDUs too big. Maximum value is %d.",
                   cfg_.tx_queue_length,
                   MAX_SDUS_PER_RLC_PDU);
      return false;
    }
  */
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
  //  logger.debug("tx_window size - %zu PDUs", tx_window.size());

  // Tx STATUS if requested
  if (do_status() /*&& not status_prohibit_timer.is_running()*/) {
    unique_byte_buffer_t tx_pdu = srsran::make_byte_buffer();
    build_status_pdu(tx_pdu.get(), nof_bytes);
    memcpy(payload, tx_pdu->msg, tx_pdu->N_bytes);
    logger->debug("Status PDU built - %d bytes", tx_pdu->N_bytes);
    return tx_pdu->N_bytes;
  }

  // Section 5.2.2.3 in TS 36.311, if tx_window is full and retx_queue empty, retransmit PDU
  // TODO

  // RETX if required
  // TODO

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

  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = 1; // FIXME
  hdr.si                     = rlc_nr_si_field_t::full_sdu;
  hdr.sn_size                = rlc_am_nr_sn_size_t::size12bits;
  hdr.sn                     = st.tx_next;
  log_rlc_am_nr_pdu_header_to_string(logger->info, hdr);

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
    // if (cfg.t_status_prohibit > 0 && status_prohibit_timer.is_valid()) {
    // re-arm timer
    //  status_prohibit_timer.run();
    //}
    // debug_state();
    pdu_len = rlc_am_nr_write_status_pdu(tx_status, rlc_am_nr_sn_size_t::size12bits, payload);
  } else {
    logger->info("%s Cannot tx status PDU - %d bytes available, %d bytes required", rb_name, nof_bytes, pdu_len);
    pdu_len = 0;
  }

  return payload->N_bytes;
}

void rlc_am_nr_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes) {}

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

  /*
  logger.debug("%s Buffer state - do_status=%s, status_prohibit_running=%s (%d/%d)",
               rb_name,
               do_status() ? "yes" : "no",
               status_prohibit_timer.is_running() ? "yes" : "no",
               status_prohibit_timer.time_elapsed(),
               status_prohibit_timer.duration());
  */

  // Bytes needed for status report
  if (do_status()) {
    n_bytes_prio += rx->get_status_pdu_length();
    logger->debug("%s Buffer state - total status report: %d bytes", rb_name, n_bytes_prio);
  }

  // Bytes needed for retx
  // TODO

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

bool rlc_am_nr_tx::do_status()
{
  return rx->get_do_status();
}

void rlc_am_nr_tx::stop() {}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_nr_rx::rlc_am_nr_rx(rlc_am* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  status_prohibit_timer(parent->timers->get_unique_timer()),
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
  }

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
                 rx_next,
                 rx_next + RLC_AM_NR_WINDOW_SIZE);
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
      logger->error("Fatal Error: Couldn't allocate PDU in handle_data_pdu().");
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
    parent->pdcp->write_pdu(parent->lcid, std::move(rx_window[header.sn].buf));
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
  }

  debug_state();

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  // Update Rx_Next_Highest
  if (RX_MOD_BASE_NR(header.sn) >= RX_MOD_BASE_NR(rx_next_highest)) {
    rx_next_highest = (header.sn + 1) % MOD;
  }

  // Update RX_Highest_Status
  /*
   * - if x = RX_Highest_Status,
   *   - update RX_Highest_Status to the SN of the first RLC SDU with SN > current RX_Highest_Status for which not all
   *     bytes have been received.
   */
  if (RX_MOD_BASE_NR(header.sn) == RX_MOD_BASE_NR(rx_highest_status)) {
    uint32_t sn_upd     = 0;
    uint32_t window_top = rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = rx_highest_status; sn_upd < window_top; ++sn_upd) {
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
    rx_highest_status = sn_upd;
  }

  /*
   * - if x = RX_Next:
   *   - update RX_Next to the SN of the first RLC SDU with SN > current RX_Next for which not all bytes
   *     have been received.
   */
  if (RX_MOD_BASE_NR(header.sn) == RX_MOD_BASE_NR(rx_next)) {
    uint32_t sn_upd     = 0;
    uint32_t window_top = rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = rx_next; sn_upd < window_top; ++sn_upd) {
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
    rx_next = sn_upd;
  }

  // if t-Reassembly is running: (TODO)
  // if t-Reassembly is not running (includes the case t-Reassembly is stopped due to actions above): (TODO)
}

bool rlc_am_nr_rx::inside_rx_window(uint32_t sn)
{
  return (RX_MOD_BASE_NR(sn) >= RX_MOD_BASE_NR(rx_next)) &&
         (RX_MOD_BASE_NR(sn) < RX_MOD_BASE_NR(rx_next + RLC_AM_NR_WINDOW_SIZE));
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
  status->ack_sn = rx_highest_status; // ACK RX_Highest_Status
  byte_buffer_t tmp_buf;
  uint32_t      len;

  uint32_t i = status->ack_sn;
  while (RX_MOD_BASE_NR(i) <= RX_MOD_BASE_NR(rx_highest_status)) {
    if (rx_window.has_sn(i) || i == rx_highest_status) {
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
    status_prohibit_timer.run(); // UINT32_MAX is used just to querry the status PDU length
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
    for (uint32_t tmp_sn = rx_next_status_trigger; tmp_sn < rx_next_status_trigger + RLC_AM_WINDOW_SIZE; tmp_sn++) {
      if (not rx_window.has_sn(tmp_sn) || not rx_window[tmp_sn].fully_received) {
        rx_highest_status = tmp_sn;
        break;
      }
    }
    bool restart_reassembly_timer = false;
    if (rx_next_highest > rx_highest_status + 1) {
      restart_reassembly_timer = true;
    }
    if (rx_next_highest == rx_highest_status + 1 && not rx_window[rx_next_highest].fully_received) {
      restart_reassembly_timer = true;
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      rx_next_status_trigger = rx_next_highest;
    }
    return;
  }
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
 * Helpers
 */
void rlc_am_nr_rx::debug_state()
{
  logger->debug("RX entity state: Rx_Next %d, Rx_Next_Status_Trigger %d, Rx_Highest_Status %d, Rx_Next_Highest",
                rx_next,
                rx_next_status_trigger,
                rx_highest_status,
                rx_next_highest);
}
} // namespace srsran
