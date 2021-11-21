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
  return 0;
}

void rlc_am_nr_tx::get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) {}

int rlc_am_nr_tx::write_sdu(unique_byte_buffer_t sdu)
{
  return 0;
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
