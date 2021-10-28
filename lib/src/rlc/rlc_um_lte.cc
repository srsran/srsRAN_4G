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

#include "srsran/rlc/rlc_um_lte.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include <sstream>

#define RX_MOD_BASE(x) (((x)-vr_uh - cfg.um.rx_window_size) % cfg.um.rx_mod)

namespace srsran {

rlc_um_lte::rlc_um_lte(srslog::basic_logger&      logger,
                       uint32_t                   lcid_,
                       srsue::pdcp_interface_rlc* pdcp_,
                       srsue::rrc_interface_rlc*  rrc_,
                       srsran::timer_handler*     timers_) :
  rlc_um_base(logger, lcid_, pdcp_, rrc_, timers_)
{}

// Warning: must call stop() to properly deallocate all buffers
rlc_um_lte::~rlc_um_lte()
{
  stop();
}

bool rlc_um_lte::configure(const rlc_config_t& cnfg_)
{
  // determine bearer name and configure Rx/Tx objects
  rb_name = get_rb_name(rrc, lcid, cnfg_.um.is_mrb);

  // store config
  cfg = cnfg_;

  rx.reset(new rlc_um_lte_rx(this));
  if (not rx->configure(cfg, rb_name)) {
    return false;
  }

  tx.reset(new rlc_um_lte_tx(this));
  if (not tx->configure(cfg, rb_name)) {
    return false;
  }

  logger.info("%s configured in %s: t_reordering=%d ms, rx_sn_field_length=%u bits, tx_sn_field_length=%u bits",
              rb_name.c_str(),
              srsran::to_string(cnfg_.rlc_mode),
              cfg.um.t_reordering,
              srsran::to_number(cfg.um.rx_sn_field_length),
              srsran::to_number(cfg.um.tx_sn_field_length));

  rx_enabled = true;
  tx_enabled = true;

  return true;
}

/****************************************************************************
 * Tx Subclass implementation for LTE
 ***************************************************************************/

rlc_um_lte::rlc_um_lte_tx::rlc_um_lte_tx(rlc_um_base* parent_) : rlc_um_base_tx(parent_) {}

uint32_t rlc_um_lte::rlc_um_lte_tx::get_buffer_state()
{
  std::lock_guard<std::mutex> lock(mutex);

  // Bytes needed for tx SDUs
  uint32_t n_sdus  = tx_sdu_queue.size();
  uint32_t n_bytes = tx_sdu_queue.size_bytes();
  if (tx_sdu) {
    n_sdus++;
    n_bytes += tx_sdu->N_bytes;
  }

  // Room needed for header extensions? (integer rounding)
  if (n_sdus > 1) {
    n_bytes += ((n_sdus - 1) * 1.5) + 0.5;
  }

  // Room needed for fixed header?
  if (n_bytes > 0)
    n_bytes += (cfg.um.is_mrb) ? 2 : 3;

  if (bsr_callback) {
    bsr_callback(parent->get_lcid(), n_bytes, 0);
  }

  return n_bytes;
}

bool rlc_um_lte::rlc_um_lte_tx::configure(const rlc_config_t& cnfg_, std::string rb_name_)
{
  cfg = cnfg_;

  if (cfg.um.tx_mod == 0) {
    logger.error("Error configuring %s RLC UM: tx_mod==0", rb_name.c_str());
    return false;
  }

  tx_sdu_queue.resize(cnfg_.tx_queue_length);

  rb_name = rb_name_;

  return true;
}

uint32_t rlc_um_lte::rlc_um_lte_tx::build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);
  rlc_umd_pdu_header_t        header;
  header.fi      = RLC_FI_FIELD_START_AND_END_ALIGNED;
  header.sn      = vt_us;
  header.N_li    = 0;
  header.sn_size = cfg.um.tx_sn_field_length;

  uint32_t to_move = 0;
  uint32_t last_li = 0;
  uint8_t* pdu_ptr = pdu->msg;

  int head_len  = rlc_um_packed_length(&header);
  int pdu_space = SRSRAN_MIN(nof_bytes, pdu->get_tailroom());

  if (pdu_space <= head_len + 1) {
    logger.info("%s Cannot build a PDU - %d bytes available, %d bytes required for header",
                rb_name.c_str(),
                nof_bytes,
                head_len);
    return 0;
  }

  // Check for SDU segment
  if (tx_sdu) {
    uint32_t space = pdu_space - head_len;
    to_move        = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    logger.debug(
        "%s adding remainder of SDU segment - %d bytes of %d remaining", rb_name.c_str(), to_move, tx_sdu->N_bytes);
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
#ifdef ENABLE_TIMESTAMP
      auto latency_us = tx_sdu->get_latency_us().count();
      mean_pdu_latency_us.push(latency_us);
      logger.debug("%s Complete SDU scheduled for tx. Stack latency (last/average): %" PRIu64 "/%ld us",
                   rb_name.c_str(),
                   (uint64_t)latency_us,
                   (long)mean_pdu_latency_us.value());
#else
      logger.debug("%s Complete SDU scheduled for tx.", rb_name.c_str());
#endif
      tx_sdu.reset();
    }
    pdu_space -= SRSRAN_MIN(to_move, pdu->get_tailroom());
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU
  }

  // Pull SDUs from queue
  while (pdu_space > head_len + 1 && tx_sdu_queue.size() > 0) {
    logger.debug("pdu_space=%d, head_len=%d", pdu_space, head_len);
    if (last_li > 0) {
      header.li[header.N_li++] = last_li;
    }
    head_len       = rlc_um_packed_length(&header);
    uint32_t space = pdu_space - head_len;
    if (space == 0) {
      // we cannot even fit a single byte of the newly added SDU, remove it again
      header.N_li--;
      break;
    }
    tx_sdu  = tx_sdu_queue.read();
    to_move = (space >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : space;
    logger.debug("%s adding new SDU segment - %d bytes of %d remaining", rb_name.c_str(), to_move, tx_sdu->N_bytes);
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
#ifdef ENABLE_TIMESTAMP
      auto latency_us = tx_sdu->get_latency_us().count();
      mean_pdu_latency_us.push(latency_us);
      logger.debug("%s Complete SDU scheduled for tx. Stack latency (last/average): %" PRIu64 "/%ld us",
                   rb_name.c_str(),
                   (uint64_t)latency_us,
                   (long)mean_pdu_latency_us.value());
#else
      logger.debug("%s Complete SDU scheduled for tx.", rb_name.c_str());
#endif
      tx_sdu.reset();
    }
    pdu_space -= to_move;
  }

  if (tx_sdu) {
    header.fi |= RLC_FI_FIELD_NOT_END_ALIGNED; // Last byte does not correspond to last byte of SDU
  }

  // Set SN
  header.sn = vt_us;
  vt_us     = (vt_us + 1) % cfg.um.tx_mod;

  // Add header and TX
  rlc_um_write_data_pdu_header(&header, pdu.get());
  memcpy(payload, pdu->msg, pdu->N_bytes);

  logger.info(payload, pdu->N_bytes, "%s Tx PDU SN=%d (%d B)", rb_name.c_str(), header.sn, pdu->N_bytes);

  debug_state();

  return pdu->N_bytes;
}

void rlc_um_lte::rlc_um_lte_tx::debug_state()
{
  logger.debug("%s vt_us = %d", rb_name.c_str(), vt_us);
}

void rlc_um_lte::rlc_um_lte_tx::reset()
{
  vt_us = 0;
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/

rlc_um_lte::rlc_um_lte_rx::rlc_um_lte_rx(rlc_um_base* parent_) :
  rlc_um_base_rx(parent_), reordering_timer(timers->get_unique_timer())
{}

rlc_um_lte::rlc_um_lte_rx::~rlc_um_lte_rx() {}

bool rlc_um_lte::rlc_um_lte_rx::configure(const rlc_config_t& cnfg_, std::string rb_name_)
{
  cfg = cnfg_;

  if (cfg.um.rx_mod == 0) {
    logger.error("Error configuring %s RLC UM: rx_mod==0", rb_name.c_str());
    return false;
  }

  // check timer
  if (not reordering_timer.is_valid()) {
    logger.error("Configuring RLC UM RX: timers not configured");
    return false;
  }

  // configure timer
  if (cfg.um.t_reordering > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.um.t_reordering), [this](uint32_t tid) { timer_expired(tid); });
  }

  rb_name = rb_name_;

  return true;
}

void rlc_um_lte::rlc_um_lte_rx::reestablish()
{
  // try to reassemble any SDUs if possible
  if (reordering_timer.is_valid() && reordering_timer.is_running()) {
    reordering_timer.stop();
    timer_expired(reordering_timer.id());
  }

  reset();
}

void rlc_um_lte::rlc_um_lte_rx::stop()
{
  reset();

  reordering_timer.stop();
}

void rlc_um_lte::rlc_um_lte_rx::reset()
{
  vr_ur    = 0;
  vr_ux    = 0;
  vr_uh    = 0;
  pdu_lost = false;

  rx_sdu.reset();

  // Drop all messages in RX window
  rx_window.clear();
}

void rlc_um_lte::rlc_um_lte_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rlc_umd_pdu_header_t header;
  rlc_um_read_data_pdu_header(payload, nof_bytes, cfg.um.rx_sn_field_length, &header);
  logger.info(payload, nof_bytes, "%s Rx data PDU SN=%d (%d B)", rb_name.c_str(), header.sn, nof_bytes);

  if (RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_uh - cfg.um.rx_window_size) &&
      RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ur)) {
    logger.info("%s SN=%d outside rx window [%d:%d] - discarding", rb_name.c_str(), header.sn, vr_ur, vr_uh);
    return;
  }

  std::map<uint32_t, rlc_umd_pdu_t>::iterator it = rx_window.find(header.sn);
  if (rx_window.end() != it) {
    logger.info("%s Discarding duplicate SN=%d", rb_name.c_str(), header.sn);
    return;
  }

  // Write to rx window
  rlc_umd_pdu_t pdu = {};
  pdu.buf           = make_byte_buffer();
  if (!pdu.buf) {
    logger.error("Discarting packet: no space in buffer pool");
    return;
  }
  memcpy(pdu.buf->msg, payload, nof_bytes);
  pdu.buf->N_bytes = nof_bytes;
  // Strip header from PDU
  int header_len = rlc_um_packed_length(&header);
  pdu.buf->msg += header_len;
  pdu.buf->N_bytes -= header_len;
  pdu.header           = header;
  rx_window[header.sn] = std::move(pdu);

  // Update vr_uh
  if (!inside_reordering_window(header.sn)) {
    vr_uh = (header.sn + 1) % cfg.um.rx_mod;
  }

  // Reassemble and deliver SDUs, while updating vr_ur
  logger.debug("Entering Reassemble from received PDU");
  reassemble_rx_sdus();
  logger.debug("Finished reassemble from received PDU");

  // Update reordering variables and timers
  if (reordering_timer.is_running()) {
    if (RX_MOD_BASE(vr_ux) <= RX_MOD_BASE(vr_ur) || (!inside_reordering_window(vr_ux) && vr_ux != vr_uh)) {
      reordering_timer.stop();
    }
  }
  if (!reordering_timer.is_running()) {
    if (RX_MOD_BASE(vr_uh) > RX_MOD_BASE(vr_ur)) {
      reordering_timer.run();
      vr_ux = vr_uh;
    }
  }

  debug_state();
}

// No locking required as only called from within handle_data_pdu and timer_expired which lock
void rlc_um_lte::rlc_um_lte_rx::reassemble_rx_sdus()
{
  if (!rx_sdu) {
    rx_sdu = make_byte_buffer();
    if (!rx_sdu) {
      logger.error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().");
      return;
    }
  }

  // First catch up with lower edge of reordering window
  while (!inside_reordering_window(vr_ur)) {
    logger.debug("SN=%d is not inside reordering windows", vr_ur);

    if (rx_window.end() == rx_window.find(vr_ur)) {
      logger.debug("SN=%d not in rx_window. Reset received SDU", vr_ur);
      rx_sdu->clear();
    } else {
      // Handle any SDU segments
      for (uint32_t i = 0; i < rx_window[vr_ur].header.N_li; i++) {
        int len = rx_window[vr_ur].header.li[i];
        logger.debug(rx_window[vr_ur].buf->msg,
                     len,
                     "Handling segment %d/%d of length %d B of SN=%d",
                     i + 1,
                     rx_window[vr_ur].header.N_li,
                     len,
                     vr_ur);
        // Check if we received a middle or end segment
        if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
          logger.warning("Dropping PDU %d in reassembly due to lost start segment", vr_ur);
          // Advance data pointers and continue with next segment
          rx_window[vr_ur].buf->msg += len;
          rx_window[vr_ur].buf->N_bytes -= len;
          rx_sdu->clear();
          metrics.num_lost_pdus++;
          break;
        }

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, len);
        rx_sdu->N_bytes += len;
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        if ((pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) ||
            (vr_ur != ((vr_ur_in_rx_sdu + 1) % cfg.um.rx_mod))) {
          logger.warning("Dropping remainder of lost PDU (lower edge middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)",
                         vr_ur,
                         vr_ur_in_rx_sdu);
          rx_sdu->clear();
          metrics.num_lost_pdus++;
        } else {
          logger.info(rx_sdu->msg,
                      rx_sdu->N_bytes,
                      "%s Rx SDU vr_ur=%d, i=%d (lower edge middle segments)",
                      rb_name.c_str(),
                      vr_ur,
                      i);
          rx_sdu->set_timestamp();
          metrics.num_rx_sdus++;
          metrics.num_rx_sdu_bytes += rx_sdu->N_bytes;
          if (cfg.um.is_mrb) {
            pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
          } else {
            pdcp->write_pdu(lcid, std::move(rx_sdu));
          }
          rx_sdu = make_byte_buffer();
          if (!rx_sdu) {
            logger.error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().");
            return;
          }
        }
        pdu_lost = false;
      }

      // Handle last segment
      if (rx_sdu->N_bytes > 0 || rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        logger.info("Writing last segment in SDU buffer. Lower edge vr_ur=%d, Buffer size=%d, segment size=%d",
                    vr_ur,
                    rx_sdu->N_bytes,
                    rx_window[vr_ur].buf->N_bytes);

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
        rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
        vr_ur_in_rx_sdu = vr_ur;
        if (rlc_um_end_aligned(rx_window[vr_ur].header.fi)) {
          if (pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
            logger.warning("Dropping remainder of lost PDU (lower edge last segments)");
            rx_sdu->clear();
            metrics.num_lost_pdus++;
          } else {
            logger.info(
                rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (lower edge last segments)", rb_name.c_str(), vr_ur);
            rx_sdu->set_timestamp();
            metrics.num_rx_sdus++;
            metrics.num_rx_sdu_bytes += rx_sdu->N_bytes;
            if (cfg.um.is_mrb) {
              pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
            } else {
              pdcp->write_pdu(lcid, std::move(rx_sdu));
            }
            rx_sdu = make_byte_buffer();
            if (!rx_sdu) {
              logger.error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().");
              return;
            }
          }
          pdu_lost = false;
        }
      }

      // Clean up rx_window
      rx_window.erase(vr_ur);
    }

    vr_ur = (vr_ur + 1) % cfg.um.rx_mod;
  }

  // Now update vr_ur until we reach an SN we haven't yet received
  while (rx_window.end() != rx_window.find(vr_ur)) {
    logger.debug("Reassemble loop for vr_ur=%d", vr_ur);

    if (not pdu_belongs_to_rx_sdu()) {
      logger.info("PDU SN=%d lost, stop reassambling SDU (vr_ur_in_rx_sdu=%d)", vr_ur_in_rx_sdu + 1, vr_ur_in_rx_sdu);
      pdu_lost = false; // Reset flag to not prevent reassembling of further segments
      rx_sdu->clear();
    }

    // Handle any SDU segments
    for (uint32_t i = 0; i < rx_window[vr_ur].header.N_li; i++) {
      uint16_t len = rx_window[vr_ur].header.li[i];
      logger.debug("Handling SDU segment i=%d with len=%d of vr_ur=%d N_li=%d [%s]",
                   i,
                   len,
                   vr_ur,
                   rx_window[vr_ur].header.N_li,
                   rlc_fi_field_text[rx_window[vr_ur].header.fi]);
      // Check if the first part of the PDU is a middle or end segment
      if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        logger.info(
            rx_window[vr_ur].buf->msg, len, "Dropping first %d B of SN=%d due to lost start segment", len, vr_ur);

        if (rx_window[vr_ur].buf->N_bytes < len) {
          logger.error("Dropping remaining remainder of SN=%d too (N_bytes=%u < len=%d)",
                       vr_ur,
                       rx_window[vr_ur].buf->N_bytes,
                       len);
          goto clean_up_rx_window;
        }

        // Advance data pointers and continue with next segment
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        rx_sdu->clear();
        metrics.num_lost_pdus++;

        // Reset flag, it is safe to process all remaining segments of this PDU
        pdu_lost = false;
        continue;
      }

      // Check available space in SDU
      if ((uint32_t)len > rx_sdu->get_tailroom()) {
        logger.error("Dropping PDU %d due to buffer mis-alignment (current segment len %d B, received %d B)",
                     vr_ur,
                     rx_sdu->N_bytes,
                     len);
        rx_sdu->clear();
        metrics.num_lost_pdus++;
        goto clean_up_rx_window;
      }

      if (not pdu_belongs_to_rx_sdu()) {
        logger.info(rx_window[vr_ur].buf->msg, len, "Copying first %d bytes of new SDU", len);
        logger.info("Updating vr_ur_in_rx_sdu. old=%d, new=%d", vr_ur_in_rx_sdu, vr_ur);
        vr_ur_in_rx_sdu = vr_ur;
      } else {
        logger.info(rx_window[vr_ur].buf->msg,
                    len,
                    "Concatenating %d bytes in to current length %d. rx_window remaining bytes=%d, "
                    "vr_ur_in_rx_sdu=%d, vr_ur=%d, rx_mod=%d, last_mod=%d",
                    len,
                    rx_sdu->N_bytes,
                    rx_window[vr_ur].buf->N_bytes,
                    vr_ur_in_rx_sdu,
                    vr_ur,
                    cfg.um.rx_mod,
                    (vr_ur_in_rx_sdu + 1) % cfg.um.rx_mod);
      }

      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, len);
      rx_sdu->N_bytes += len;
      rx_window[vr_ur].buf->msg += len;
      rx_window[vr_ur].buf->N_bytes -= len;
      vr_ur_in_rx_sdu = vr_ur;

      if (pdu_belongs_to_rx_sdu()) {
        logger.info(rx_sdu->msg,
                    rx_sdu->N_bytes,
                    "%s Rx SDU vr_ur=%d, i=%d, (update vr_ur middle segments)",
                    rb_name.c_str(),
                    vr_ur,
                    i);
        rx_sdu->set_timestamp();
        metrics.num_rx_sdus++;
        metrics.num_rx_sdu_bytes += rx_sdu->N_bytes;
        if (cfg.um.is_mrb) {
          pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
        } else {
          pdcp->write_pdu(lcid, std::move(rx_sdu));
        }
        rx_sdu = make_byte_buffer();
        if (!rx_sdu) {
          logger.error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().");
          return;
        }
      } else {
        logger.warning("Dropping remainder of lost PDU (update vr_ur middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)",
                       vr_ur,
                       vr_ur_in_rx_sdu);
        // Advance data pointers and continue with next segment
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        metrics.num_lost_pdus++;
      }
      pdu_lost = false;
    }

    // Handle last segment
    if (rx_sdu->N_bytes == 0 && rx_window[vr_ur].header.N_li == 0 &&
        !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
      logger.warning("Dropping PDU %d during last segment handling due to lost start segment", vr_ur);
      rx_sdu->clear();
      metrics.num_lost_pdus++;
      goto clean_up_rx_window;
    }

    if (rx_sdu->N_bytes < SRSRAN_MAX_BUFFER_SIZE_BYTES &&
        rx_window[vr_ur].buf->N_bytes < SRSRAN_MAX_BUFFER_SIZE_BYTES &&
        rx_window[vr_ur].buf->N_bytes + rx_sdu->N_bytes < SRSRAN_MAX_BUFFER_SIZE_BYTES) {
      logger.info(rx_window[vr_ur].buf->msg,
                  rx_window[vr_ur].buf->N_bytes,
                  "Writing last segment in SDU buffer. Updating vr_ur=%d, vr_ur_in_rx_sdu=%d, Buffer size=%d, "
                  "segment size=%d",
                  vr_ur,
                  vr_ur_in_rx_sdu,
                  rx_sdu->N_bytes,
                  rx_window[vr_ur].buf->N_bytes);
      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
      rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
    } else {
      logger.error("Out of bounds while reassembling SDU buffer in UM: sdu_len=%d, window_buffer_len=%d, vr_ur=%d",
                   rx_sdu->N_bytes,
                   rx_window[vr_ur].buf->N_bytes,
                   vr_ur);
    }
    vr_ur_in_rx_sdu = vr_ur;
    if (rlc_um_end_aligned(rx_window[vr_ur].header.fi)) {
      if (pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        logger.warning("Dropping remainder of lost PDU (update vr_ur last segments)");
        rx_sdu->clear();
        metrics.num_lost_pdus++;
      } else {
        logger.info(
            rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (update vr_ur last segments)", rb_name.c_str(), vr_ur);
        rx_sdu->set_timestamp();
        metrics.num_rx_sdus++;
        metrics.num_rx_sdu_bytes += rx_sdu->N_bytes;
        if (cfg.um.is_mrb) {
          pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
        } else {
          pdcp->write_pdu(lcid, std::move(rx_sdu));
        }
        rx_sdu = make_byte_buffer();
        if (!rx_sdu) {
          logger.error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().");
          return;
        }
      }
      pdu_lost = false;
    }

  clean_up_rx_window:
    // Clean up rx_window
    rx_window.erase(vr_ur);

    vr_ur = (vr_ur + 1) % cfg.um.rx_mod;
  }
}

// Only called when lock is hold
bool rlc_um_lte::rlc_um_lte_rx::pdu_belongs_to_rx_sdu()
{
  // return true if the currently received SDU
  if (((vr_ur_in_rx_sdu + 1) % cfg.um.rx_mod == vr_ur) || (vr_ur == vr_ur_in_rx_sdu)) {
    return true;
  }
  return false;
}

// Only called when lock is hold
// 36.322 Section 5.1.2.2.1
bool rlc_um_lte::rlc_um_lte_rx::inside_reordering_window(uint16_t sn)
{
  if (cfg.um.rx_window_size == 0 || rx_window.empty()) {
    return true;
  }
  if (RX_MOD_BASE(vr_uh - cfg.um.rx_window_size) <= RX_MOD_BASE(sn) && RX_MOD_BASE(sn) < RX_MOD_BASE(vr_uh)) {
    return true;
  } else {
    return false;
  }
}

/****************************************************************************
 * Timeout callback interface
 ***************************************************************************/

void rlc_um_lte::rlc_um_lte_rx::timer_expired(uint32_t timeout_id)
{
  if (reordering_timer.id() == timeout_id) {
    // 36.322 v10 Section 5.1.2.2.4
    logger.info("%s reordering timeout expiry - updating vr_ur and reassembling", rb_name.c_str());

    logger.warning("Lost PDU SN=%d", vr_ur);

    pdu_lost = true;
    if (rx_sdu != NULL) {
      rx_sdu->clear();
    }

    while (RX_MOD_BASE(vr_ur) < RX_MOD_BASE(vr_ux)) {
      vr_ur = (vr_ur + 1) % cfg.um.rx_mod;
      logger.debug("Entering Reassemble from timeout id=%d", timeout_id);
      reassemble_rx_sdus();
      logger.debug("Finished reassemble from timeout id=%d", timeout_id);
    }

    if (RX_MOD_BASE(vr_uh) > RX_MOD_BASE(vr_ur)) {
      reordering_timer.run();
      vr_ux = vr_uh;
    }

    debug_state();
  }
}

/****************************************************************************
 * Helper functions
 ***************************************************************************/

void rlc_um_lte::rlc_um_lte_rx::debug_state()
{
  logger.debug("%s vr_ur = %d, vr_ux = %d, vr_uh = %d", rb_name.c_str(), vr_ur, vr_ux, vr_uh);
}

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/

void rlc_um_read_data_pdu_header(byte_buffer_t* pdu, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t* header)
{
  rlc_um_read_data_pdu_header(pdu->msg, pdu->N_bytes, sn_size, header);
}

void rlc_um_read_data_pdu_header(uint8_t*              payload,
                                 uint32_t              nof_bytes,
                                 rlc_umd_sn_size_t     sn_size,
                                 rlc_umd_pdu_header_t* header)
{
  uint8_t  ext;
  uint8_t* ptr = payload;
  // Fixed part
  if (sn_size == rlc_umd_sn_size_t::size5bits) {
    header->fi = (rlc_fi_field_t)((*ptr >> 6) & 0x03); // 2 bits FI
    ext        = ((*ptr >> 5) & 0x01);                 // 1 bit EXT
    header->sn = *ptr & 0x1F;                          // 5 bits SN
    ptr++;
  } else {
    header->fi = (rlc_fi_field_t)((*ptr >> 3) & 0x03); // 2 bits FI
    ext        = ((*ptr >> 2) & 0x01);                 // 1 bit EXT
    header->sn = (*ptr & 0x03) << 8;                   // 2 bits SN
    ptr++;
    header->sn |= (*ptr & 0xFF); // 8 bits SN
    ptr++;
  }

  header->sn_size = sn_size;

  // Extension part
  header->N_li = 0;
  while (ext) {
    if (header->N_li % 2 == 0) {
      ext                      = ((*ptr >> 7) & 0x01);
      header->li[header->N_li] = (*ptr & 0x7F) << 4; // 7 bits of LI
      ptr++;
      header->li[header->N_li] |= (*ptr & 0xF0) >> 4; // 4 bits of LI
      header->N_li++;
    } else {
      ext                      = (*ptr >> 3) & 0x01;
      header->li[header->N_li] = (*ptr & 0x07) << 8; // 3 bits of LI
      ptr++;
      header->li[header->N_li] |= (*ptr & 0xFF); // 8 bits of LI
      header->N_li++;
      ptr++;
    }
  }
}

void rlc_um_write_data_pdu_header(rlc_umd_pdu_header_t* header, byte_buffer_t* pdu)
{
  uint32_t i;
  uint8_t  ext = (header->N_li > 0) ? 1 : 0;
  // Make room for the header
  uint32_t len = rlc_um_packed_length(header);
  pdu->msg -= len;
  uint8_t* ptr = pdu->msg;

  // Fixed part
  if (header->sn_size == rlc_umd_sn_size_t::size5bits) {
    *ptr = (header->fi & 0x03) << 6; // 2 bits FI
    *ptr |= (ext & 0x01) << 5;       // 1 bit EXT
    *ptr |= header->sn & 0x1F;       // 5 bits SN
    ptr++;
  } else {
    *ptr = (header->fi & 0x03) << 3;   // 3 Reserved bits | 2 bits FI
    *ptr |= (ext & 0x01) << 2;         // 1 bit EXT
    *ptr |= (header->sn & 0x300) >> 8; // 2 bits SN
    ptr++;
    *ptr = (header->sn & 0xFF); // 8 bits SN
    ptr++;
  }

  // Extension part
  i = 0;
  while (i < header->N_li) {
    ext  = ((i + 1) == header->N_li) ? 0 : 1;
    *ptr = (ext & 0x01) << 7;             // 1 bit header
    *ptr |= (header->li[i] & 0x7F0) >> 4; // 7 bits of LI
    ptr++;
    *ptr = (header->li[i] & 0x00F) << 4; // 4 bits of LI
    i++;
    if (i < header->N_li) {
      ext = ((i + 1) == header->N_li) ? 0 : 1;
      *ptr |= (ext & 0x01) << 3;            // 1 bit header
      *ptr |= (header->li[i] & 0x700) >> 8; // 3 bits of LI
      ptr++;
      *ptr = (header->li[i] & 0x0FF); // 8 bits of LI
      ptr++;
      i++;
    }
  }
  // Pad if N_li is odd
  if (header->N_li % 2 == 1)
    ptr++;

  pdu->N_bytes += ptr - pdu->msg;
}

uint32_t rlc_um_packed_length(rlc_umd_pdu_header_t* header)
{
  uint32_t len = 0;
  if (header->sn_size == rlc_umd_sn_size_t::size5bits) {
    len += 1; // Fixed part is 1 byte
  } else {
    len += 2; // Fixed part is 2 bytes
  }
  len += header->N_li * 1.5 + 0.5; // Extension part - integer rounding up
  return len;
}

bool rlc_um_start_aligned(uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_END_ALIGNED);
}

bool rlc_um_end_aligned(uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_START_ALIGNED);
}

} // namespace srsran
