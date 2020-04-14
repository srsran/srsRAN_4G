/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srslte/upper/rlc_um_lte.h"
#include <sstream>

#define RX_MOD_BASE(x) (((x)-vr_uh - cfg.um.rx_window_size) % cfg.um.rx_mod)

namespace srslte {

rlc_um_lte::rlc_um_lte(srslte::log_ref            log_,
                       uint32_t                   lcid_,
                       srsue::pdcp_interface_rlc* pdcp_,
                       srsue::rrc_interface_rlc*  rrc_,
                       srslte::timer_handler*     timers_) :
  rlc_um_base(log_, lcid_, pdcp_, rrc_, timers_)
{
}

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
  if (not rx->configure()) {
    return false;
  }

  tx.reset(new rlc_um_lte_tx(this));
  if (not tx->configure(cfg, rb_name)) {
    return false;
  }

  log->info("%s configured in %s: t_reordering=%d ms, rx_sn_field_length=%u bits, tx_sn_field_length=%u bits\n",
            rb_name.c_str(),
            srslte::to_string(cnfg_.rlc_mode).c_str(),
            cfg.um.t_reordering,
            srslte::to_number(cfg.um.rx_sn_field_length),
            srslte::to_number(cfg.um.tx_sn_field_length));

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

  return n_bytes;
}

bool rlc_um_lte::rlc_um_lte_tx::configure(const rlc_config_t& cnfg_, std::string rb_name_)
{
  cfg = cnfg_;

  if (cfg.um.tx_mod == 0) {
    log->error("Error configuring %s RLC UM: tx_mod==0\n", rb_name.c_str());
    return false;
  }

  tx_sdu_queue.resize(cnfg_.tx_queue_length);

  rb_name = rb_name_;

  return true;
}

int rlc_um_lte::rlc_um_lte_tx::build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes)
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
  int pdu_space = SRSLTE_MIN(nof_bytes, pdu->get_tailroom());

  if (pdu_space <= head_len + 1) {
    log->warning("%s Cannot build a PDU - %d bytes available, %d bytes required for header\n",
                 rb_name.c_str(),
                 nof_bytes,
                 head_len);
    return 0;
  }

  // Check for SDU segment
  if (tx_sdu) {
    uint32_t space = pdu_space - head_len;
    to_move        = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    log->debug(
        "%s adding remainder of SDU segment - %d bytes of %d remaining\n", rb_name.c_str(), to_move, tx_sdu->N_bytes);
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug(
          "%s Complete SDU scheduled for tx. Stack latency: %ld us\n", rb_name.c_str(), tx_sdu->get_latency_us());

      tx_sdu.reset();
    }
    pdu_space -= SRSLTE_MIN(to_move, pdu->get_tailroom());
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU
  }

  // Pull SDUs from queue
  while (pdu_space > head_len + 1 && tx_sdu_queue.size() > 0) {
    log->debug("pdu_space=%d, head_len=%d\n", pdu_space, head_len);
    if (last_li > 0)
      header.li[header.N_li++] = last_li;
    head_len       = rlc_um_packed_length(&header);
    tx_sdu         = tx_sdu_queue.read();
    uint32_t space = pdu_space - head_len;
    to_move        = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    log->debug("%s adding new SDU segment - %d bytes of %d remaining\n", rb_name.c_str(), to_move, tx_sdu->N_bytes);
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug(
          "%s Complete SDU scheduled for tx. Stack latency: %ld us\n", rb_name.c_str(), tx_sdu->get_latency_us());

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
  uint32_t ret = pdu->N_bytes;

  log->info_hex(payload, ret, "%s Tx PDU SN=%d (%d B)\n", rb_name.c_str(), header.sn, pdu->N_bytes);

  debug_state();

  return ret;
}

void rlc_um_lte::rlc_um_lte_tx::debug_state()
{
  log->debug("%s vt_us = %d\n", rb_name.c_str(), vt_us);
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/

rlc_um_lte::rlc_um_lte_rx::rlc_um_lte_rx(rlc_um_base* parent_) :
  rlc_um_base_rx(parent_),
  reordering_timer(timers->get_unique_timer())
{
}

rlc_um_lte::rlc_um_lte_rx::~rlc_um_lte_rx() {}

bool rlc_um_lte::rlc_um_lte_rx::configure()
{
  if (cfg.um.rx_mod == 0) {
    log->error("Error configuring %s RLC UM: rx_mod==0\n", rb_name.c_str());
    return false;
  }

  // check timer
  if (not reordering_timer.is_valid()) {
    log->error("Configuring RLC UM RX: timers not configured\n");
    return false;
  }

  // configure timer
  if (cfg.um.t_reordering > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.um.t_reordering), [this](uint32_t tid) { timer_expired(tid); });
  }

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
  log->info_hex(payload, nof_bytes, "RX %s Rx data PDU SN: %d (%d B)", rb_name.c_str(), header.sn, nof_bytes);

  if (RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_uh - cfg.um.rx_window_size) &&
      RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ur)) {
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n", rb_name.c_str(), header.sn, vr_ur, vr_uh);
    return;
  }

  std::map<uint32_t, rlc_umd_pdu_t>::iterator it = rx_window.find(header.sn);
  if (rx_window.end() != it) {
    log->info("%s Discarding duplicate SN: %d\n", rb_name.c_str(), header.sn);
    return;
  }

  // Write to rx window
  rlc_umd_pdu_t pdu = {};
  pdu.buf           = allocate_unique_buffer(*pool);
  if (!pdu.buf) {
    log->error("Discarting packet: no space in buffer pool\n");
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
  log->debug("Entering Reassemble from received PDU\n");
  reassemble_rx_sdus();
  log->debug("Finished reassemble from received PDU\n");

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
    rx_sdu = allocate_unique_buffer(*pool);
    if (!rx_sdu) {
      log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
      return;
    }
  }

  // First catch up with lower edge of reordering window
  while (!inside_reordering_window(vr_ur)) {
    log->debug("SN=%d is not inside reordering windows\n", vr_ur);

    if (rx_window.end() == rx_window.find(vr_ur)) {
      log->debug("SN=%d not in rx_window. Reset received SDU\n", vr_ur);
      rx_sdu->clear();
    } else {
      // Handle any SDU segments
      for (uint32_t i = 0; i < rx_window[vr_ur].header.N_li; i++) {
        int len = rx_window[vr_ur].header.li[i];
        log->debug_hex(rx_window[vr_ur].buf->msg,
                       len,
                       "Handling segment %d/%d of length %d B of SN=%d\n",
                       i + 1,
                       rx_window[vr_ur].header.N_li,
                       len,
                       vr_ur);
        // Check if we received a middle or end segment
        if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
          log->warning("Dropping PDU %d due to lost start segment\n", vr_ur);
          // Advance data pointers and continue with next segment
          rx_window[vr_ur].buf->msg += len;
          rx_window[vr_ur].buf->N_bytes -= len;
          rx_sdu->clear();
          break;
        }

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, len);
        rx_sdu->N_bytes += len;
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        if ((pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) ||
            (vr_ur != ((vr_ur_in_rx_sdu + 1) % cfg.um.rx_mod))) {
          log->warning("Dropping remainder of lost PDU (lower edge middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)\n",
                       vr_ur,
                       vr_ur_in_rx_sdu);
          rx_sdu->clear();
        } else {
          log->info_hex(rx_sdu->msg,
                        rx_sdu->N_bytes,
                        "%s Rx SDU vr_ur=%d, i=%d (lower edge middle segments)",
                        rb_name.c_str(),
                        vr_ur,
                        i);
          rx_sdu->set_timestamp();
          if (cfg.um.is_mrb) {
            pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
          } else {
            pdcp->write_pdu(lcid, std::move(rx_sdu));
          }
          rx_sdu = allocate_unique_buffer(*pool);
          if (!rx_sdu) {
            log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
            return;
          }
        }
        pdu_lost = false;
      }

      // Handle last segment
      if (rx_sdu->N_bytes > 0 || rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->info("Writing last segment in SDU buffer. Lower edge vr_ur=%d, Buffer size=%d, segment size=%d\n",
                  vr_ur,
                  rx_sdu->N_bytes,
                  rx_window[vr_ur].buf->N_bytes);

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
        rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
        vr_ur_in_rx_sdu = vr_ur;
        if (rlc_um_end_aligned(rx_window[vr_ur].header.fi)) {
          if (pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
            log->warning("Dropping remainder of lost PDU (lower edge last segments)\n");
            rx_sdu->clear();
          } else {
            log->info_hex(
                rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (lower edge last segments)", rb_name.c_str(), vr_ur);
            rx_sdu->set_timestamp();
            if (cfg.um.is_mrb) {
              pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
            } else {
              pdcp->write_pdu(lcid, std::move(rx_sdu));
            }
            rx_sdu = allocate_unique_buffer(*pool);
            if (!rx_sdu) {
              log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
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
    log->debug("Reassemble loop for vr_ur=%d\n", vr_ur);

    if (not pdu_belongs_to_rx_sdu()) {
      log->warning(
          "PDU SN=%d lost, stop reassambling SDU (vr_ur_in_rx_sdu=%d)\n", vr_ur_in_rx_sdu + 1, vr_ur_in_rx_sdu);
      pdu_lost = false; // Reset flag to not prevent reassembling of further segments
      rx_sdu->clear();
    }

    // Handle any SDU segments
    for (uint32_t i = 0; i < rx_window[vr_ur].header.N_li; i++) {
      uint16_t len = rx_window[vr_ur].header.li[i];
      log->debug("Handling SDU segment i=%d with len=%d of vr_ur=%d N_li=%d [%s]\n",
                 i,
                 len,
                 vr_ur,
                 rx_window[vr_ur].header.N_li,
                 rlc_fi_field_text[rx_window[vr_ur].header.fi]);
      // Check if the first part of the PDU is a middle or end segment
      if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->warning_hex(
            rx_window[vr_ur].buf->msg, len, "Dropping first %d B of SN %d due to lost start segment\n", len, vr_ur);

        if (rx_window[vr_ur].buf->N_bytes < len) {
          log->error("Dropping remaining remainder of SN %d too (N_bytes=%u < len=%d)\n",
                     vr_ur,
                     rx_window[vr_ur].buf->N_bytes,
                     len);
          goto clean_up_rx_window;
        }

        // Advance data pointers and continue with next segment
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        rx_sdu->clear();

        // Reset flag, it is safe to process all remaining segments of this PDU
        pdu_lost = false;
        continue;
      }

      // Check available space in SDU
      if ((uint32_t)len > rx_sdu->get_tailroom()) {
        log->error("Dropping PDU %d due to buffer mis-alignment (current segment len %d B, received %d B)\n",
                   vr_ur,
                   rx_sdu->N_bytes,
                   len);
        rx_sdu->clear();
        goto clean_up_rx_window;
      }

      if (not pdu_belongs_to_rx_sdu()) {
        log->info_hex(rx_window[vr_ur].buf->msg, len, "Copying first %d bytes of new SDU\n", len);
        log->info("Updating vr_ur_in_rx_sdu. old=%d, new=%d\n", vr_ur_in_rx_sdu, vr_ur);
        vr_ur_in_rx_sdu = vr_ur;
      } else {
        log->info_hex(rx_window[vr_ur].buf->msg,
                      len,
                      "Concatenating %d bytes in to current length %d. rx_window remaining bytes=%d, "
                      "vr_ur_in_rx_sdu=%d, vr_ur=%d, rx_mod=%d, last_mod=%d\n",
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
        log->info_hex(rx_sdu->msg,
                      rx_sdu->N_bytes,
                      "%s Rx SDU vr_ur=%d, i=%d, (update vr_ur middle segments)",
                      rb_name.c_str(),
                      vr_ur,
                      i);
        rx_sdu->set_timestamp();
        if (cfg.um.is_mrb) {
          pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
        } else {
          pdcp->write_pdu(lcid, std::move(rx_sdu));
        }
        rx_sdu = allocate_unique_buffer(*pool);
        if (!rx_sdu) {
          log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
          return;
        }
      } else {
        log->warning("Dropping remainder of lost PDU (update vr_ur middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)\n",
                     vr_ur,
                     vr_ur_in_rx_sdu);
        // Advance data pointers and continue with next segment
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
      }
      pdu_lost = false;
    }

    // Handle last segment
    if (rx_sdu->N_bytes == 0 && rx_window[vr_ur].header.N_li == 0 &&
        !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
      log->warning("Dropping PDU %d due to lost start segment\n", vr_ur);
      rx_sdu->clear();
      goto clean_up_rx_window;
    }

    if (rx_sdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BYTES &&
        rx_window[vr_ur].buf->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BYTES &&
        rx_window[vr_ur].buf->N_bytes + rx_sdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BYTES) {
      log->info_hex(rx_window[vr_ur].buf->msg,
                    rx_window[vr_ur].buf->N_bytes,
                    "Writing last segment in SDU buffer. Updating vr_ur=%d, vr_ur_in_rx_sdu=%d, Buffer size=%d, "
                    "segment size=%d\n",
                    vr_ur,
                    vr_ur_in_rx_sdu,
                    rx_sdu->N_bytes,
                    rx_window[vr_ur].buf->N_bytes);
      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
      rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
    } else {
      log->error("Out of bounds while reassembling SDU buffer in UM: sdu_len=%d, window_buffer_len=%d, vr_ur=%d\n",
                 rx_sdu->N_bytes,
                 rx_window[vr_ur].buf->N_bytes,
                 vr_ur);
    }
    vr_ur_in_rx_sdu = vr_ur;
    if (rlc_um_end_aligned(rx_window[vr_ur].header.fi)) {
      if (pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->warning("Dropping remainder of lost PDU (update vr_ur last segments)\n");
        rx_sdu->clear();
      } else {
        log->info_hex(
            rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (update vr_ur last segments)", rb_name.c_str(), vr_ur);
        rx_sdu->set_timestamp();
        if (cfg.um.is_mrb) {
          pdcp->write_pdu_mch(lcid, std::move(rx_sdu));
        } else {
          pdcp->write_pdu(lcid, std::move(rx_sdu));
        }
        rx_sdu = allocate_unique_buffer(*pool);
        if (!rx_sdu) {
          log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
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
    log->info("%s reordering timeout expiry - updating vr_ur and reassembling\n", rb_name.c_str());

    log->warning("Lost PDU SN: %d\n", vr_ur);

    pdu_lost = true;
    if (rx_sdu != NULL) {
      rx_sdu->clear();
    }

    while (RX_MOD_BASE(vr_ur) < RX_MOD_BASE(vr_ux)) {
      vr_ur = (vr_ur + 1) % cfg.um.rx_mod;
      log->debug("Entering Reassemble from timeout id=%d\n", timeout_id);
      reassemble_rx_sdus();
      log->debug("Finished reassemble from timeout id=%d\n", timeout_id);
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
  log->debug("%s vr_ur = %d, vr_ux = %d, vr_uh = %d\n", rb_name.c_str(), vr_ur, vr_ux, vr_uh);
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

} // namespace srslte
