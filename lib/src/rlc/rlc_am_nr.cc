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
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/srslog/event_trace.h"
#include <iostream>

namespace srsran {

/****************************************************************************
 * RLC AM NR entity
 ***************************************************************************/

/***************************************************************************
 *  Tx subclass implementation
 ***************************************************************************/
rlc_am_nr_tx::rlc_am_nr_tx(rlc_am* parent_) : parent(parent_), rlc_am_base_tx(&parent_->logger)
{
  parent->logger.debug("Initializing RLC AM NR TX: Tx_Next: %d",
                       st.tx_next); // Temporarly silence unused variable warning
}

bool rlc_am_nr_tx::configure(const rlc_config_t& cfg_)
{
  /*
    if (cfg_.tx_queue_length > MAX_SDUS_PER_RLC_PDU) {
      logger.error("Configuring Tx queue length of %d PDUs too big. Maximum value is %d.",
                   cfg_.tx_queue_length,
                   MAX_SDUS_PER_RLC_PDU);
      return false;
    }
  */
  cfg = cfg_.am;

  tx_enabled = true;

  return true;
}

bool rlc_am_nr_tx::has_data()
{
  return true;
}

uint32_t rlc_am_nr_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  return 0;
}

void rlc_am_nr_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes) {}

uint32_t rlc_am_nr_tx::get_buffer_state()
{
  uint32_t tx_queue      = 0;
  uint32_t prio_tx_queue = 0;
  get_buffer_state(tx_queue, prio_tx_queue);
  return tx_queue + prio_tx_queue;
}

void rlc_am_nr_tx::get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t                    n_bytes = 0;
  uint32_t                    n_sdus  = 0;

  /*
  logger.debug("%s Buffer state - do_status=%s, status_prohibit_running=%s (%d/%d)",
               rb_name,
               do_status() ? "yes" : "no",
               status_prohibit_timer.is_running() ? "yes" : "no",
               status_prohibit_timer.time_elapsed(),
               status_prohibit_timer.duration());
  */

  // Bytes needed for status report
  // TODO

  // Bytes needed for retx
  // TODO

  // Bytes needed for tx SDUs
  n_sdus = tx_sdu_queue.get_n_sdus();
  n_bytes += tx_sdu_queue.size_bytes();

  // Room needed for fixed header of data PDUs
  n_bytes += 2 * n_sdus; // TODO make header size configurable
  if (n_bytes > 0 && n_sdus > 0) {
    logger->debug("%s Total buffer state - %d SDUs (%d B)", rb_name, n_sdus, n_bytes);
  }

  tx_queue = n_bytes;
  return;
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

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_nr_rx::rlc_am_nr_rx(rlc_am* parent_) :
  parent(parent_), pool(byte_buffer_pool::get_instance()), rlc_am_base_rx(parent_, &parent_->logger)
{
  parent->logger.debug("Initializing RLC AM NR RX"); // Temporarly silence unused variable warning
}

bool rlc_am_nr_rx::configure(const rlc_config_t& cfg_)
{
  cfg = cfg_.am;

  return true;
}

void rlc_am_nr_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) {}

void rlc_am_nr_rx::stop() {}

void rlc_am_nr_rx::reestablish()
{
  stop();
}

uint32_t rlc_am_nr_rx::get_sdu_rx_latency_ms()
{
  return 0;
}

uint32_t rlc_am_nr_rx::get_rx_buffered_bytes()
{
  return 0;
}
} // namespace srsran
