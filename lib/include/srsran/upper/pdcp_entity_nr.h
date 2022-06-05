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

#ifndef SRSRAN_PDCP_ENTITY_NR_H
#define SRSRAN_PDCP_ENTITY_NR_H

#include "pdcp_entity_base.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/common/security.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include <map>

namespace srsran {
/****************************************************************************
 * NR PDCP Entity
 * PDCP entity for 5G NR
 ***************************************************************************/
class pdcp_entity_nr final : public pdcp_entity_base
{
public:
  pdcp_entity_nr(srsue::rlc_interface_pdcp* rlc_,
                 srsue::rrc_interface_pdcp* rrc_,
                 srsue::gw_interface_pdcp*  gw_,
                 srsran::task_sched_handle  task_sched_,
                 srslog::basic_logger&      logger,
                 uint32_t                   lcid);
  ~pdcp_entity_nr() final = default;
  bool configure(const pdcp_config_t& cnfg_) final;
  void reset() final;
  void reestablish() final;

  // RRC interface
  void write_sdu(unique_byte_buffer_t sdu, int sn = -1) final;

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu) final;
  void notify_delivery(const pdcp_sn_vector_t& pdcp_sns) final;
  void notify_failure(const pdcp_sn_vector_t& pdcp_sns) final;

  void get_bearer_state(pdcp_lte_state_t* state) override;
  void set_bearer_state(const pdcp_lte_state_t& state, bool set_fmc) override;

  void                  send_status_report() override {}
  pdcp_bearer_metrics_t get_metrics() override;
  void                  reset_metrics() override;

  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus() override { return {}; }

  // State variable getters (useful for testing)
  uint32_t nof_discard_timers() { return discard_timers_map.size(); }
  bool     is_reordering_timer_running() { return reordering_timer.is_running(); }

  // State variable setters (should be used only for testing)
  void     set_tx_next(uint32_t tx_next_) { tx_next = tx_next_; }
  void     set_rx_next(uint32_t rx_next_) { rx_next = rx_next_; }
  void     set_rx_deliv(uint32_t rx_deliv_) { rx_deliv = rx_deliv_; }
  void     set_rx_reord(uint32_t rx_reord_) { rx_reord = rx_reord_; }
  uint32_t get_tx_next() const { return tx_next; }
  uint32_t get_rx_next() const { return rx_next; }
  uint32_t get_rx_deliv() const { return rx_deliv; }
  uint32_t get_rx_reord() const { return rx_reord; }

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;

  // State variables: 3GPP TS 38.323 v15.2.0, section 7.1
  uint32_t tx_next  = 0; // COUNT value of next SDU to be transmitted.
  uint32_t rx_next  = 0; // COUNT value of next SDU expected to be received.
  uint32_t rx_deliv = 0; // COUNT value of first SDU not delivered to upper layers, but still waited for.
  uint32_t rx_reord = 0; // COUNT value following the COUNT value of PDCP Data PDU which triggered t-Reordering.

  // Constants: 3GPP TS 38.323 v15.2.0, section 7.2
  uint32_t window_size = 0;

  // Reordering Queue / Timers
  std::map<uint32_t, unique_byte_buffer_t> reorder_queue;
  timer_handler::unique_timer              reordering_timer;

  // Pass to Upper Layers Helper function
  void deliver_all_consecutive_counts();
  void pass_to_upper_layers(unique_byte_buffer_t pdu);

  // Reodering callback (t-Reordering)
  class reordering_callback;
  std::unique_ptr<reordering_callback> reordering_fnc;

  // Discard callback (discardTimer)
  class discard_callback;
  std::map<uint32_t, timer_handler::unique_timer> discard_timers_map;

  // COUNT overflow protection
  bool tx_overflow = false;
  bool rx_overflow = false;

  enum class rlc_mode_t {
    UM,
    AM,
  } rlc_mode;
};

/*
 * Timer callbacks
 */
// Reordering callback (t-Reordering)
class pdcp_entity_nr::reordering_callback
{
public:
  reordering_callback(pdcp_entity_nr* parent_) { parent = parent_; };
  void operator()(uint32_t timer_id);

private:
  pdcp_entity_nr* parent;
};

// Discard callback (discardTimer)
class pdcp_entity_nr::discard_callback
{
public:
  discard_callback(pdcp_entity_nr* parent_, uint32_t sn_)
  {
    parent     = parent_;
    discard_sn = sn_;
  };
  void operator()(uint32_t timer_id);

private:
  pdcp_entity_nr* parent;
  uint32_t        discard_sn;
};

/*
 * Helpers
 */
inline void pdcp_entity_nr::pass_to_upper_layers(unique_byte_buffer_t sdu)
{
  if (is_srb()) {
    rrc->write_pdu(lcid, std::move(sdu));
  } else {
    gw->write_pdu(lcid, std::move(sdu));
  }
}

} // namespace srsran
#endif // SRSRAN_PDCP_ENTITY_NR_H
