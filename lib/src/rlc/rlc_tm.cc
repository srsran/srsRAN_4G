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

#include "srsran/rlc/rlc_tm.h"
#include "srsran/common/common_lte.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"

namespace srsran {

rlc_tm::rlc_tm(srslog::basic_logger&      logger,
               uint32_t                   lcid_,
               srsue::pdcp_interface_rlc* pdcp_,
               srsue::rrc_interface_rlc*  rrc_) :
  rlc_common(logger), pdcp(pdcp_), rrc(rrc_), lcid(lcid_)
{
  pool    = byte_buffer_pool::get_instance();
  rb_name = "SRB0";
}

// Warning: must call stop() to properly deallocate all buffers
rlc_tm::~rlc_tm()
{
  pool = NULL;
}

bool rlc_tm::configure(const rlc_config_t& cnfg)
{
  RlcError("Attempted to configure TM RLC entity");
  return true;
}

void rlc_tm::empty_queue()
{
  // Drop all messages in TX queue
  unique_byte_buffer_t buf;
  while (ul_queue.try_read(&buf)) {
  }
  ul_queue.reset();
}

void rlc_tm::reestablish()
{
  stop();
  tx_enabled = true;
}

void rlc_tm::stop()
{
  tx_enabled = false;
  empty_queue();
}

rlc_mode_t rlc_tm::get_mode()
{
  return rlc_mode_t::tm;
}

uint32_t rlc_tm::get_lcid()
{
  return lcid;
}

// PDCP interface
void rlc_tm::write_sdu(unique_byte_buffer_t sdu)
{
  if (!tx_enabled) {
    return;
  }
  if (sdu != nullptr) {
    uint8_t*                                 msg_ptr   = sdu->msg;
    uint32_t                                 nof_bytes = sdu->N_bytes;
    srsran::error_type<unique_byte_buffer_t> ret       = ul_queue.try_write(std::move(sdu));
    if (ret) {
      RlcHexInfo(msg_ptr, nof_bytes, "Tx SDU, queue size=%d, bytes=%d", ul_queue.size(), ul_queue.size_bytes());
    } else {
      RlcHexWarning(ret.error()->msg,
                    ret.error()->N_bytes,
                    "[Dropped SDU] Tx SDU, queue size=%d, bytes=%d",
                    ul_queue.size(),
                    ul_queue.size_bytes());
    }

  } else {
    RlcWarning("NULL SDU pointer in write_sdu()");
  }
}

void rlc_tm::discard_sdu(uint32_t discard_sn)
{
  if (!tx_enabled) {
    return;
  }
  RlcWarning("SDU discard not implemented on RLC TM");
}

bool rlc_tm::sdu_queue_is_full()
{
  return ul_queue.is_full();
}

// MAC interface
bool rlc_tm::has_data()
{
  return not ul_queue.is_empty();
}

uint32_t rlc_tm::get_buffer_state()
{
  uint32_t newtx_queue   = 0;
  uint32_t prio_tx_queue = 0;
  get_buffer_state(newtx_queue, prio_tx_queue);
  return newtx_queue + prio_tx_queue;
}

void rlc_tm::get_buffer_state(uint32_t& newtx_queue, uint32_t& prio_tx_queue)
{
  std::lock_guard<std::mutex> lock(bsr_callback_mutex);
  newtx_queue   = ul_queue.size_bytes();
  prio_tx_queue = 0;
  if (bsr_callback) {
    bsr_callback(lcid, newtx_queue, prio_tx_queue);
  }
}

rlc_bearer_metrics_t rlc_tm::get_metrics()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  return metrics;
}

void rlc_tm::reset_metrics()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics = {};
}

uint32_t rlc_tm::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t pdu_size = ul_queue.size_tail_bytes();
  if (pdu_size > nof_bytes) {
    RlcInfo("Tx PDU size larger than MAC opportunity (%d > %d)", pdu_size, nof_bytes);
    return 0;
  }
  unique_byte_buffer_t buf;
  if (ul_queue.try_read(&buf)) {
    pdu_size = buf->N_bytes;
    memcpy(payload, buf->msg, buf->N_bytes);
    RlcDebug("Complete SDU scheduled for tx. Stack latency: %" PRIu64 " us", (uint64_t)buf->get_latency_us().count());
    RlcHexInfo(payload,
               pdu_size,
               "Tx %s PDU, queue size=%d, bytes=%d",
               srsran::to_string(rlc_mode_t::tm),
               ul_queue.size(),
               ul_queue.size_bytes());

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_tx_pdu_bytes += pdu_size;
    return pdu_size;
  }

  if (ul_queue.size_bytes() > 0) {
    RlcWarning("Corrupted queue: empty but size_bytes > 0. Resetting queue");
    ul_queue.reset();
  }
  return 0;
}

void rlc_tm::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  unique_byte_buffer_t buf = make_byte_buffer();
  if (buf != nullptr) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    {
      std::lock_guard<std::mutex> lock(metrics_mutex);
      metrics.num_rx_pdu_bytes += nof_bytes;
      metrics.num_rx_pdus++;
    }
    if (srsran::srb_to_lcid(srsran::lte_srb::srb0) == lcid) {
      rrc->write_pdu(lcid, std::move(buf));
    } else {
      pdcp->write_pdu(lcid, std::move(buf));
    }
  } else {
    RlcError("Fatal Error: Couldn't allocate buffer in rlc_tm::write_pdu().");
  }
}

void rlc_tm::set_bsr_callback(bsr_callback_t callback)
{
  bsr_callback = std::move(callback);
}

} // namespace srsran
