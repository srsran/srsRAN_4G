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

/*******************************
 *     RLC AM NR class
 ******************************/
rlc_am_nr::rlc_am_nr(srslog::basic_logger&      logger,
                     uint32_t                   lcid_,
                     srsue::pdcp_interface_rlc* pdcp_,
                     srsue::rrc_interface_rlc*  rrc_,
                     srsran::timer_handler*     timers_) :
  logger(logger), rrc(rrc_), pdcp(pdcp_), timers(timers_), lcid(lcid_), tx(this), rx(this)
{}

// Applies new configuration. Must be just reestablished or initiated
bool rlc_am_nr::configure(const rlc_config_t& cfg_)
{
  // determine bearer name and configure Rx/Tx objects
  rb_name = rrc->get_rb_name(lcid);

  // store config
  cfg = cfg_;

  if (not rx.configure(cfg.am)) {
    logger.error("Error configuring bearer (RX)");
    return false;
  }

  if (not tx.configure(cfg.am)) {
    logger.error("Error configuring bearer (TX)");
    return false;
  }

  logger.info("%s configured: t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, max_retx_thresh=%d, "
              "t_reordering=%d, t_status_prohibit=%d",
              rb_name.c_str(),
              cfg.am.t_poll_retx,
              cfg.am.poll_pdu,
              cfg.am.poll_byte,
              cfg.am.max_retx_thresh,
              cfg.am.t_reordering,
              cfg.am.t_status_prohibit);
  return true;
}

void rlc_am_nr::stop() {}

rlc_mode_t rlc_am_nr::get_mode()
{
  return rlc_mode_t::am;
}

uint32_t rlc_am_nr::get_bearer()
{
  return 0;
}

void rlc_am_nr::reestablish() {}

void rlc_am_nr::empty_queue() {}

void rlc_am_nr::set_bsr_callback(bsr_callback_t callback) {}

rlc_bearer_metrics_t rlc_am_nr::get_metrics()
{
  return {};
}

void rlc_am_nr::reset_metrics() {}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/
void rlc_am_nr::write_sdu(unique_byte_buffer_t sdu)
{
  if (tx.write_sdu(std::move(sdu)) == SRSRAN_SUCCESS) {
    metrics.num_tx_sdus++;
  }
}

void rlc_am_nr::discard_sdu(uint32_t pdcp_sn)
{
  tx.discard_sdu(pdcp_sn);
  metrics.num_lost_sdus++;
}

bool rlc_am_nr::sdu_queue_is_full()
{
  return tx.sdu_queue_is_full();
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/

bool rlc_am_nr::has_data()
{
  return tx.has_data();
}

uint32_t rlc_am_nr::get_buffer_state()
{
  return tx.get_buffer_state();
}

void rlc_am_nr::get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue)
{
  // TODO
  tx_queue      = tx.get_buffer_state();
  prio_tx_queue = 0;
}

uint32_t rlc_am_nr::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t read_bytes = tx.read_pdu(payload, nof_bytes);
  metrics.num_tx_pdus++;
  metrics.num_tx_pdu_bytes += read_bytes;
  return read_bytes;
}

void rlc_am_nr::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rx.write_pdu(payload, nof_bytes);
  metrics.num_rx_pdus++;
  metrics.num_rx_pdu_bytes += nof_bytes;
}

/****************************************************************************
 * Tx subclass implementation
 ***************************************************************************/
rlc_am_nr::rlc_am_nr_tx::rlc_am_nr_tx(rlc_am_nr* parent_) :
  parent(parent_), logger(parent_->logger), pool(byte_buffer_pool::get_instance())
{}

bool rlc_am_nr::rlc_am_nr_tx::configure(const rlc_am_config_t& cfg_)
{
  /*
    if (cfg_.tx_queue_length > MAX_SDUS_PER_RLC_PDU) {
      logger.error("Configuring Tx queue length of %d PDUs too big. Maximum value is %d.",
                   cfg_.tx_queue_length,
                   MAX_SDUS_PER_RLC_PDU);
      return false;
    }
  */
  cfg = cfg_;

  return true;
}

int rlc_am_nr::rlc_am_nr_tx::write_sdu(unique_byte_buffer_t sdu)
{
  return 0;
}

void rlc_am_nr::rlc_am_nr_tx::discard_sdu(uint32_t sn)
{
  return;
}

bool rlc_am_nr::rlc_am_nr_tx::sdu_queue_is_full()
{
  return false;
}

bool rlc_am_nr::rlc_am_nr_tx::has_data()
{
  return true;
}

uint32_t rlc_am_nr::rlc_am_nr_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  return 0;
}

uint32_t rlc_am_nr::rlc_am_nr_tx::get_buffer_state()
{
  return 0;
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_nr::rlc_am_nr_rx::rlc_am_nr_rx(rlc_am_nr* parent_) :
  parent(parent_), pool(byte_buffer_pool::get_instance()), logger(parent_->logger)
{}

bool rlc_am_nr::rlc_am_nr_rx::configure(const rlc_am_config_t& cfg_)
{
  cfg = cfg_;

  return true;
}

void rlc_am_nr::rlc_am_nr_rx::stop() {}

void rlc_am_nr::rlc_am_nr_rx::write_pdu(uint8_t* payload, uint32_t nof_bytes) {}

} // namespace srsran
