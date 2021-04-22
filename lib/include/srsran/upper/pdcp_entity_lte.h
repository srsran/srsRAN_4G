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

#ifndef SRSRAN_PDCP_ENTITY_LTE_H
#define SRSRAN_PDCP_ENTITY_LTE_H

#include "srsran/adt/circular_array.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/security.h"
#include "srsran/common/threads.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/upper/pdcp_entity_base.h"

namespace srsue {

class gw_interface_pdcp;
class rlc_interface_pdcp;

} // namespace srsue

namespace srsran {

class undelivered_sdus_queue
{
public:
  explicit undelivered_sdus_queue(srsran::task_sched_handle task_sched);

  bool            empty() const { return count == 0; }
  bool            is_full() const { return count >= capacity; }
  uint32_t        size() const { return count; }
  static uint32_t get_capacity() { return capacity; }
  bool            has_sdu(uint32_t sn) const
  {
    assert(sn != invalid_sn && "provided PDCP SN is invalid");
    return sdus[sn].sdu != nullptr and sdus[sn].sdu->md.pdcp_sn == sn;
  }
  // Getter for the number of discard timers. Used for debugging.
  size_t nof_discard_timers() const;

  bool add_sdu(uint32_t                              sn,
               const srsran::unique_byte_buffer_t&   sdu,
               uint32_t                              discard_timeout,
               srsran::move_callback<void(uint32_t)> callback);

  unique_byte_buffer_t& operator[](uint32_t sn)
  {
    assert(has_sdu(sn));
    return sdus[sn].sdu;
  }
  bool clear_sdu(uint32_t sn);
  void clear();

  uint32_t get_bytes() const { return bytes; }
  uint32_t get_fms() const { return fms; }
  void     set_fms(uint32_t fms_) { fms = fms_; }
  void     update_fms();
  void     update_lms(uint32_t sn);

  uint32_t get_lms() const { return lms; }

  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_sdus();

private:
  const static uint32_t capacity   = 4096;
  const static uint32_t invalid_sn = -1;

  static uint32_t increment_sn(uint32_t sn) { return (sn + 1) % capacity; }

  struct sdu_data {
    srsran::unique_byte_buffer_t sdu;
    srsran::unique_timer         discard_timer;
  };

  uint32_t                                   count = 0;
  uint32_t                                   bytes = 0;
  uint32_t                                   fms   = 0; // SN of the first missing PDCP SDU
  uint32_t                                   lms   = 0;
  srsran::circular_array<sdu_data, capacity> sdus;
};

/****************************************************************************
 * Structs and Defines
 * Ref: 3GPP TS 36.323 v10.1.0
 ***************************************************************************/

#define PDCP_CONTROL_MAC_I 0x00000000

/****************************************************************************
 * LTE PDCP Entity
 * Class for LTE PDCP entities
 ***************************************************************************/

class pdcp_entity_lte final : public pdcp_entity_base
{
public:
  pdcp_entity_lte(srsue::rlc_interface_pdcp* rlc_,
                  srsue::rrc_interface_pdcp* rrc_,
                  srsue::gw_interface_pdcp*  gw_,
                  srsran::task_sched_handle  task_sched_,
                  srslog::basic_logger&      logger,
                  uint32_t                   lcid_);
  ~pdcp_entity_lte() override;
  bool configure(const pdcp_config_t& cnfg_) override;
  void reset() override;
  void reestablish() override;

  // GW/RRC interface
  void write_sdu(unique_byte_buffer_t sdu, int sn = -1) override;

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu) override;
  void notify_failure(const pdcp_sn_vector_t& pdcp_sns) override;
  void notify_delivery(const pdcp_sn_vector_t& pdcp_sns) override;

  // Config helpers
  bool check_valid_config();

  // TX SDU queue helper
  bool store_sdu(uint32_t tx_count, const unique_byte_buffer_t& pdu);

  // Getter for unacknowledged PDUs. Used for handover
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus() override;

  // Status report helper(s)
  void send_status_report() override;
  void handle_status_report_pdu(srsran::unique_byte_buffer_t pdu);

  // Internal state getters/setters
  void get_bearer_state(pdcp_lte_state_t* state) override;
  void set_bearer_state(const pdcp_lte_state_t& state, bool set_fmc) override;

  // Metrics helpers
  pdcp_bearer_metrics_t get_metrics() override;
  void                  reset_metrics() override;

  size_t nof_discard_timers() const { return undelivered_sdus != nullptr ? undelivered_sdus->nof_discard_timers() : 0; }

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;

  // State variables, as defined in TS 36 323, section 7.1
  pdcp_lte_state_t st = {};

  uint32_t reordering_window = 0;
  uint32_t maximum_pdcp_sn   = 0;

  // PDU handlers
  void handle_control_pdu(srsran::unique_byte_buffer_t pdu);
  void handle_srb_pdu(srsran::unique_byte_buffer_t pdu);
  void handle_um_drb_pdu(srsran::unique_byte_buffer_t pdu);
  void handle_am_drb_pdu(srsran::unique_byte_buffer_t pdu);

  // Discard callback (discardTimer)
  class discard_callback;

  // Tx info queue
  uint32_t                                maximum_allocated_sns_window = 2048;
  std::unique_ptr<undelivered_sdus_queue> undelivered_sdus;

  // Rx info queue
  uint32_t              fmc              = 0;
  uint32_t              largest_rx_count = 0;
  std::vector<uint32_t> rx_counts_info; // Keeps the RX_COUNT for generation of the stauts report
  void                  update_rx_counts_queue(uint32_t rx_count);

  /*
   * Helper function to see if an SN is larger
   */
  bool is_sn_larger(uint32_t sn1, uint32_t sn2)
  {
    int32_t  diff    = sn2 - sn1;
    uint32_t nof_sns = 1u << cfg.sn_len;
    if (diff > (int32_t)(nof_sns / 2)) {
      return false;
    }
    if (diff <= 0 && diff > -((int32_t)(nof_sns / 2))) {
      return false;
    }
    return true;
  }
};

// Discard callback (discardTimer)
class pdcp_entity_lte::discard_callback
{
public:
  discard_callback(pdcp_entity_lte* parent_, uint32_t sn_)
  {
    parent     = parent_;
    discard_sn = sn_;
  };
  void operator()(uint32_t timer_id);

private:
  pdcp_entity_lte* parent;
  uint32_t         discard_sn;
};

} // namespace srsran
#endif // SRSRAN_PDCP_ENTITY_LTE_H
