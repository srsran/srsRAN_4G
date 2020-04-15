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

#include "srslte/upper/rlc_um_nr.h"
#include <sstream>

#define RX_MOD_NR_BASE(x) (((x)-RX_Next_Highest - cfg.um_nr.UM_Window_Size) % cfg.um_nr.mod)

namespace srslte {

rlc_um_nr::rlc_um_nr(srslte::log_ref            log_,
                     uint32_t                   lcid_,
                     srsue::pdcp_interface_rlc* pdcp_,
                     srsue::rrc_interface_rlc*  rrc_,
                     srslte::timer_handler*     timers_) :
  rlc_um_base(log_, lcid_, pdcp_, rrc_, timers_)
{
}

rlc_um_nr::~rlc_um_nr()
{
  stop();
}

bool rlc_um_nr::configure(const rlc_config_t& cnfg_)
{
  // determine bearer name and configure Rx/Tx objects
  rb_name = get_rb_name(rrc, lcid, cnfg_.um.is_mrb);

  // store config
  cfg = cnfg_;

  rx.reset(new rlc_um_nr_rx(this));
  if (not rx->configure()) {
    return false;
  }

  tx.reset(new rlc_um_nr_tx(this));
  if (not tx->configure(cfg, rb_name)) {
    return false;
  }

  log->info("%s configured in %s: sn_field_length=%u bits\n",
            rb_name.c_str(),
            srslte::to_string(cnfg_.rlc_mode).c_str(),
            srslte::to_number(cfg.um_nr.sn_field_length));

  rx_enabled = true;
  tx_enabled = true;

  return true;
}

/****************************************************************************
 * Tx Subclass implementation
 ***************************************************************************/

rlc_um_nr::rlc_um_nr_tx::rlc_um_nr_tx(rlc_um_base* parent_) : rlc_um_base_tx(parent_) {}

uint32_t rlc_um_nr::rlc_um_nr_tx::get_buffer_state()
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

bool rlc_um_nr::rlc_um_nr_tx::configure(const rlc_config_t& cnfg_, std::string rb_name_)
{
  cfg = cnfg_;

  if (cfg.um_nr.mod == 0) {
    log->error("Error configuring %s RLC UM: tx_mod==0\n", rb_name.c_str());
    return false;
  }

  tx_sdu_queue.resize(cnfg_.tx_queue_length);

  rb_name = rb_name_;

  return true;
}

int rlc_um_nr::rlc_um_nr_tx::build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);
  rlc_um_nr_pdu_header_t      header = {};
  header.si                          = rlc_nr_si_field_t::full_sdu;
  header.sn                          = TX_Next;
  header.sn_size                     = cfg.um_nr.sn_field_length;

  uint32_t to_move = 0;
  uint8_t* pdu_ptr = pdu->msg;

  int head_len  = rlc_um_nr_packed_length(header);
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
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug(
          "%s Complete SDU scheduled for tx. Stack latency: %ld us\n", rb_name.c_str(), tx_sdu->get_latency_us());
      tx_sdu.reset();
      header.si = rlc_nr_si_field_t::last_segment;
    } else {
      header.si = rlc_nr_si_field_t::neither_first_nor_last_segment;
    }
    pdu_space -= SRSLTE_MIN(to_move, pdu->get_tailroom());
    header.so = next_so;
  } else {
    // Pull SDU from queue
    log->debug("pdu_space=%d, head_len=%d\n", pdu_space, head_len);

    head_len       = rlc_um_nr_packed_length(header);
    tx_sdu         = tx_sdu_queue.read();
    uint32_t space = pdu_space - head_len;
    to_move        = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    log->debug("%s adding new SDU - %d bytes of %d remaining\n", rb_name.c_str(), to_move, tx_sdu->N_bytes);
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug(
          "%s Complete SDU scheduled for tx. Stack latency: %ld us\n", rb_name.c_str(), tx_sdu->get_latency_us());
      tx_sdu.reset();
      header.si = rlc_nr_si_field_t::full_sdu;
    } else {
      header.si = rlc_nr_si_field_t::first_segment;
    }
    pdu_space -= to_move;
  }

  // advance SO offset
  next_so += to_move;

  // Update SN if needed
  if (header.si == rlc_nr_si_field_t::last_segment) {
    TX_Next = (TX_Next + 1) % cfg.um_nr.mod;
    next_so = 0;
  }

  // Add header and TX
  rlc_um_nr_write_data_pdu_header(header, pdu.get());
  memcpy(payload, pdu->msg, pdu->N_bytes);
  uint32_t ret = pdu->N_bytes;

  log->info_hex(payload, ret, "%s Tx PDU SN=%d (%d B)\n", rb_name.c_str(), header.sn, pdu->N_bytes);

  debug_state();

  return ret;
}

void rlc_um_nr::rlc_um_nr_tx::debug_state()
{
  log->debug("%s TX_Next = %d\n", rb_name.c_str(), TX_Next);
}

/****************************************************************************
 * Rx Subclass implementation
 ***************************************************************************/

rlc_um_nr::rlc_um_nr_rx::rlc_um_nr_rx(rlc_um_base* parent_) :
  rlc_um_base_rx(parent_),
  reassembly_timer(timers->get_unique_timer())
{
}

bool rlc_um_nr::rlc_um_nr_rx::configure()
{
  if (cfg.um_nr.mod == 0) {
    log->error("Error configuring %s RLC UM: rx_mod==0\n", rb_name.c_str());
    return false;
  }

  // check timer
  if (not reassembly_timer.is_valid()) {
    log->error("Configuring RLC UM NR RX: timers not configured\n");
    return false;
  }

  // configure timer
  if (cfg.um_nr.t_reassembly_ms > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.um_nr.t_reassembly_ms),
                         [this](uint32_t tid) { timer_expired(tid); });
  }

  return true;
}

void rlc_um_nr::rlc_um_nr_rx::stop()
{
  reset();
  reassembly_timer.stop();
}

void rlc_um_nr::rlc_um_nr_rx::reset()
{
  RX_Next_Reassembly = 0;
  RX_Timer_Trigger   = 0;
  RX_Next_Highest    = 0;

  rx_sdu.reset();

  // Drop all messages in RX window
  rx_window.clear();

  // stop timer
  if (reassembly_timer.is_valid()) {
    reassembly_timer.stop();
  }
}

// TS 38.322 Sec. 5.1.2
void rlc_um_nr::rlc_um_nr_rx::reestablish()
{
  // drop all SDUs, SDU segments, PDUs and reset timers
  reset();
}

// TS 38.322 v15.003 Section 5.2.2.2.4
void rlc_um_nr::rlc_um_nr_rx::timer_expired(uint32_t timeout_id)
{
  if (reassembly_timer.id() == timeout_id) {
    log->info("%s reassembly timeout expiry - updating RX_Next_Reassembly and reassembling\n", rb_name.c_str());

    log->warning("Lost PDU SN: %d\n", RX_Next_Reassembly);
    metrics.num_lost_pdus++;

    if (rx_sdu != nullptr) {
      rx_sdu->clear();
    }

    // update RX_Next_Reassembly to the next SN that has not been reassembled yet
    RX_Next_Reassembly = RX_Timer_Trigger;
    while (RX_MOD_NR_BASE(RX_Next_Reassembly) < RX_MOD_NR_BASE(RX_Next_Highest)) {
      RX_Next_Reassembly = (RX_Next_Reassembly + 1) % cfg.um_nr.mod;
      debug_state();
    }

    // discard all segments with SN < updated RX_Next_Reassembly
    for (auto it = rx_window.begin(); it != rx_window.end();) {
      if (it->first < RX_Next_Reassembly) {
        it = rx_window.erase(it);
      } else {
        ++it;
      }
    }

    // check start of t_reassembly
    if (RX_MOD_NR_BASE(RX_Next_Highest) > RX_MOD_NR_BASE(RX_Next_Reassembly + 1) ||
        (RX_MOD_NR_BASE(RX_Next_Highest) == RX_MOD_NR_BASE(RX_Next_Reassembly + 1) &&
         has_missing_byte_segment(RX_Next_Reassembly))) {
      reassembly_timer.run();
      RX_Timer_Trigger = RX_Next_Highest;
    }

    debug_state();
  }
}

// Sec 5.2.2.2.1
bool rlc_um_nr::rlc_um_nr_rx::sn_in_reassembly_window(const uint32_t sn)
{
  return (RX_MOD_NR_BASE(RX_Next_Highest - cfg.um_nr.UM_Window_Size) <= RX_MOD_NR_BASE(sn) &&
          RX_MOD_NR_BASE(sn) < RX_MOD_NR_BASE(RX_Next_Highest));
}

// Sec 5.2.2.2.2
bool rlc_um_nr::rlc_um_nr_rx::sn_invalid_for_rx_buffer(const uint32_t sn)
{
  return (RX_MOD_NR_BASE(RX_Next_Highest - cfg.um_nr.UM_Window_Size) <= RX_MOD_NR_BASE(sn) &&
          RX_MOD_NR_BASE(sn) < RX_MOD_NR_BASE(RX_Next_Reassembly));
}

unique_byte_buffer_t rlc_um_nr::rlc_um_nr_rx::rlc_um_nr_strip_pdu_header(const rlc_um_nr_pdu_header_t& header,
                                                                         const uint8_t*                payload,
                                                                         const uint32_t                nof_bytes)
{
  unique_byte_buffer_t sdu = allocate_unique_buffer(*pool);
  if (!sdu) {
    log->error("Discarting packet: no space in buffer pool\n");
    return nullptr;
  }
  memcpy(sdu->msg, payload, nof_bytes);
  sdu->N_bytes = nof_bytes;

  // strip RLC header
  int header_len = rlc_um_nr_packed_length(header);
  sdu->msg += header_len;
  sdu->N_bytes -= header_len;
  return sdu;
}

bool rlc_um_nr::rlc_um_nr_rx::has_missing_byte_segment(const uint32_t sn)
{
  // is at least one missing byte segment of the RLC SDU associated with SN = RX_Next_Reassembly before the last byte of
  // all received segments of this RLC SDU
  // TODO: check assumption
  // if SN can be found in rx_window, it means that at least one segment is missing
  return (rx_window.find(sn) != rx_window.end());
}

// Sect 5.2.2.2.3
void rlc_um_nr::rlc_um_nr_rx::handle_rx_buffer_update(const uint32_t sn)
{
  if (rx_window.find(sn) != rx_window.end()) {
    // iterate over received segments and try to assemble full SDU
    auto& pdu = rx_window.at(sn);
    for (auto it = pdu.segments.begin(); it != pdu.segments.end();) {
      log->debug("Have %s segment with SO=%d for SN=%d\n",
                 to_string_short(it->second.header.si).c_str(),
                 it->second.header.so,
                 it->second.header.sn);
      if (it->second.header.so == pdu.next_expected_so) {
        if (pdu.next_expected_so == 0) {
          if (pdu.sdu == nullptr) {
            // reuse buffer of first segment for final SDU
            pdu.sdu              = std::move(it->second.buf);
            pdu.next_expected_so = pdu.sdu->N_bytes;
            log->debug("Reusing first segment of SN=%d for final SDU\n", it->second.header.sn);
            it = pdu.segments.erase(it);
          } else {
            log->debug("SDU buffer already allocated. Possible retransmission of first segment.\n");
            if (it->second.header.so != pdu.next_expected_so) {
              log->error("Invalid PDU. SO doesn't match. Discarting all segments of SN=%d.\n", sn);
              rx_window.erase(sn);
              return;
            }
          }
        } else {
          if (it->second.buf->N_bytes > pdu.sdu->get_tailroom()) {
            log->error("Cannot fit RLC PDU in SDU buffer (tailroom=%d, len=%d), dropping both. Erasing SN=%d.\n",
                       rx_sdu->get_tailroom(),
                       it->second.buf->N_bytes,
                       it->second.header.sn);
            rx_window.erase(sn);
            metrics.num_lost_pdus++;
            return;
          }

          // add this segment to the end of the SDU buffer
          memcpy(pdu.sdu->msg + pdu.sdu->N_bytes, it->second.buf->msg, it->second.buf->N_bytes);
          pdu.sdu->N_bytes += it->second.buf->N_bytes;
          pdu.next_expected_so += it->second.buf->N_bytes;
          log->debug("Appended SO=%d of SN=%d\n", it->second.header.so, it->second.header.sn);
          it = pdu.segments.erase(it);

          if (pdu.next_expected_so == pdu.total_sdu_length) {
            // deliver full SDU to upper layers
            log->info("Delivering %s SDU SN=%d (%d B)", rb_name.c_str(), sn, pdu.sdu->N_bytes);
            pdcp->write_pdu(lcid, std::move(pdu.sdu));

            // find next SN in rx buffer
            if (sn == RX_Next_Reassembly) {
              RX_Next_Reassembly = ((RX_Next_Reassembly + 1) % cfg.um_nr.mod);
              while (RX_MOD_NR_BASE(RX_Next_Reassembly) < RX_MOD_NR_BASE(RX_Next_Highest)) {
                RX_Next_Reassembly = (RX_Next_Reassembly + 1) % cfg.um_nr.mod;
              }
              log->debug("Updating RX_Next_Reassembly=%d\n", RX_Next_Reassembly);
            }

            // delete PDU from rx_window
            rx_window.erase(sn);
            return;
          }
        }
      } else {
        // handle next segment
        ++it;
      }
    }

    // check for SN outside of rx window
    if (not sn_in_reassembly_window(sn)) {
      // update RX_Next_highest
      RX_Next_Highest = sn + 1;
      log->debug("Updating RX_Next_Highest=%d\n", RX_Next_Highest);

      // drop all SNs outside of new rx window
      for (auto it = rx_window.begin(); it != rx_window.end();) {
        if (not sn_in_reassembly_window(it->first)) {
          log->info("%s SN: %d outside rx window [%d:%d] - discarding\n",
                    rb_name.c_str(),
                    it->first,
                    RX_Next_Highest - cfg.um_nr.UM_Window_Size,
                    RX_Next_Highest);
          it = rx_window.erase(it);
          metrics.num_lost_pdus++;
        } else {
          ++it;
        }
      }

      if (not sn_in_reassembly_window(RX_Next_Reassembly)) {
        // update RX_Next_Reassembly to first SN that has not been reassembled and delivered
        for (const auto& rx_pdu : rx_window) {
          if (rx_pdu.first >= RX_MOD_NR_BASE(RX_Next_Highest - cfg.um_nr.UM_Window_Size)) {
            RX_Next_Reassembly = rx_pdu.first;
            log->debug("Updating RX_Next_Reassembly=%d\n", RX_Next_Reassembly);
            break;
          }
        }
      }

      if (reassembly_timer.is_running()) {
        if (RX_Timer_Trigger <= RX_Next_Reassembly ||
            (not sn_in_reassembly_window(RX_Timer_Trigger) and RX_Timer_Trigger != RX_Next_Highest) ||
            ((RX_Next_Highest == RX_Next_Reassembly + 1) && not has_missing_byte_segment(sn))) {
          reassembly_timer.stop();
        }
      }

      if (not reassembly_timer.is_running() && has_missing_byte_segment(sn)) {
        if (RX_Next_Highest > RX_Next_Reassembly + 1) {
          reassembly_timer.run();
          RX_Timer_Trigger = RX_Next_Highest;
        }
      }
    }
  } else {
    log->error("SN=%d does not exist in Rx buffer\n", sn);
  }
}

inline void rlc_um_nr::rlc_um_nr_rx::update_total_sdu_length(rlc_umd_pdu_segments_nr_t& pdu_segments,
                                                             const rlc_umd_pdu_nr_t&    rx_pdu)
{
  if (rx_pdu.header.si == rlc_nr_si_field_t::last_segment) {
    pdu_segments.total_sdu_length = rx_pdu.header.so + rx_pdu.buf->N_bytes;
    log->info("%s updating total SDU length for SN=%d to %d B\n",
              rb_name.c_str(),
              rx_pdu.header.sn,
              pdu_segments.total_sdu_length);
  }
};

// Section 5.2.2.2.2
void rlc_um_nr::rlc_um_nr_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rlc_um_nr_pdu_header_t header = {};
  rlc_um_nr_read_data_pdu_header(payload, nof_bytes, cfg.um_nr.sn_field_length, &header);
  log->debug_hex(payload, nof_bytes, "RX %s Rx data PDU (%d B)", rb_name.c_str(), nof_bytes);

  // check if PDU contains a SN
  if (header.si == rlc_nr_si_field_t::full_sdu) {
    // copy full PDU into buffer
    unique_byte_buffer_t sdu = rlc_um_nr_strip_pdu_header(header, payload, nof_bytes);

    // deliver to PDCP
    log->info("Delivering %s SDU (%d B)", rb_name.c_str(), sdu->N_bytes);
    pdcp->write_pdu(lcid, std::move(sdu));
  } else if (sn_invalid_for_rx_buffer(header.sn)) {
    log->info("%s Discarding SN=%d\n", rb_name.c_str(), header.sn);
    // Nothing else to do here ..
  } else {
    // place PDU in receive buffer
    rlc_umd_pdu_nr_t rx_pdu = {};
    rx_pdu.header           = header;
    rx_pdu.buf              = rlc_um_nr_strip_pdu_header(header, payload, nof_bytes);

    // check if this SN is already present in rx buffer
    if (rx_window.find(header.sn) == rx_window.end()) {
      // first received segment of this SN, add to rx buffer
      log->info("%s placing %s segment of SN=%d in Rx buffer\n",
                rb_name.c_str(),
                to_string_short(header.si).c_str(),
                header.sn);
      rlc_umd_pdu_segments_nr_t pdu_segments = {};
      update_total_sdu_length(pdu_segments, rx_pdu);
      pdu_segments.segments.emplace(header.so, std::move(rx_pdu));
      rx_window[header.sn] = std::move(pdu_segments);
    } else {
      // other segment for this SN already present, update received data
      log->info("%s updating SN=%d at SO=%d with %d B\n",
                rb_name.c_str(),
                rx_pdu.header.sn,
                rx_pdu.header.so,
                rx_pdu.buf->N_bytes);

      auto& pdu_segments = rx_window.at(header.sn);

      // calculate total SDU length
      update_total_sdu_length(pdu_segments, rx_pdu);

      // append to list of segments
      pdu_segments.segments.emplace(header.so, std::move(rx_pdu));
    }

    // handle received segments
    handle_rx_buffer_update(header.sn);
  }

  debug_state();
}

void rlc_um_nr::rlc_um_nr_rx::debug_state()
{
  log->debug("%s RX_Next_Reassembly=%d, RX_Timer_Trigger=%d, RX_Next_Highest=%d, t_Reassembly=%s\n",
             rb_name.c_str(),
             RX_Next_Reassembly,
             RX_Timer_Trigger,
             RX_Next_Highest,
             reassembly_timer.is_running() ? "running" : "stopped");
}
/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.3
 ***************************************************************************/

uint32_t rlc_um_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header)
{
  return rlc_um_nr_read_data_pdu_header(pdu->msg, pdu->N_bytes, sn_size, header);
}

uint32_t rlc_um_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header)
{
  uint8_t* ptr = const_cast<uint8_t*>(payload);

  header->sn_size = sn_size;

  // Fixed part
  if (sn_size == rlc_um_nr_sn_size_t::size6bits) {
    header->si = (rlc_nr_si_field_t)((*ptr >> 6) & 0x03); // 2 bits SI
    header->sn = *ptr & 0x3F;                             // 6 bits SN
    // sanity check
    if (header->si == rlc_nr_si_field_t::full_sdu and !(header->sn == 0)) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }
    ptr++;
  } else if (sn_size == rlc_um_nr_sn_size_t::size12bits) {
    header->si = (rlc_nr_si_field_t)((*ptr >> 6) & 0x03); // 2 bits SI
    header->sn = (*ptr & 0x0F) << 4;                      // 4 bits SN

    // sanity check
    if (header->si == rlc_nr_si_field_t::first_segment) {
      // make sure two reserved bits are not set
      if (((*ptr >> 4) & 0x03) != 0) {
        fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
        return 0;
      }
    }

    // continue unpacking remaining SN
    ptr++;
    header->sn |= (*ptr & 0xFF); // 8 bits SN
    ptr++;
  } else {
    fprintf(stderr, "Unsupported SN length\n");
    return 0;
  }

  // Read optional part
  if (header->si == rlc_nr_si_field_t::last_segment ||
      header->si == rlc_nr_si_field_t::neither_first_nor_last_segment) {
    // read SO
    header->so = (*ptr & 0xFF) << 8;
    ptr++;
    header->so |= (*ptr & 0xFF);
    ptr++;
  }

  // return consumed bytes
  return (ptr - payload);
}

uint32_t rlc_um_nr_packed_length(const rlc_um_nr_pdu_header_t& header)
{
  uint32_t len = 0;
  if (header.si == rlc_nr_si_field_t::full_sdu || header.si == rlc_nr_si_field_t::first_segment) {
    len = 1;
    if (header.sn_size == rlc_um_nr_sn_size_t::size12bits) {
      len++;
    }
  } else {
    if (header.sn_size == rlc_um_nr_sn_size_t::size6bits) {
      len = 3;
    } else {
      len = 4;
    }
  }
  return len;
}

uint32_t rlc_um_nr_write_data_pdu_header(const rlc_um_nr_pdu_header_t& header, byte_buffer_t* pdu)
{
  // Make room for the header
  uint32_t len = rlc_um_nr_packed_length(header);
  pdu->msg -= len;
  uint8_t* ptr = pdu->msg;

  // write SI field
  *ptr = (header.si & 0x03) << 6; // 2 bits SI

  if (header.si == rlc_nr_si_field_t::full_sdu) {
    // that's all ..
    ptr++;
  } else {
    if (header.sn_size == rlc_um_nr_sn_size_t::size6bits) {
      // write SN
      *ptr |= (header.sn & 0x3f); // 6 bit SN
      ptr++;
    } else {
      // 12bit SN
      *ptr |= (header.sn & 0xf); // 4 bit SN
      ptr++;
      *ptr = (header.sn & 0xFF); // remaining 8 bit SN
      ptr++;
    }
    if (header.so) {
      // write SO
      *ptr = (header.so) >> 8; // first part of SO
      ptr++;
      *ptr = (header.so & 0xFF); // second part of SO
      ptr++;
    }
  }

  pdu->N_bytes += ptr - pdu->msg;

  return len;
}

} // namespace srslte
