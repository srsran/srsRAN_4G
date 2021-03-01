/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_PDCP_ENTITY_LTE_H
#define SRSLTE_PDCP_ENTITY_LTE_H

#include "srslte/adt/circular_array.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/ue_rrc_interfaces.h"
#include "srslte/upper/pdcp_entity_base.h"

namespace srsue {

class gw_interface_pdcp;
class rlc_interface_pdcp;

} // namespace srsue

namespace srslte {

class undelivered_sdus_queue
{
public:
  explicit undelivered_sdus_queue(srslte::task_sched_handle task_sched)
  {
    for (auto& e : sdus) {
      e.discard_timer = task_sched.get_unique_timer();
    }
  }

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
  size_t nof_discard_timers() const
  {
    return std::count_if(sdus.begin(), sdus.end(), [](const sdu_data& s) {
      return s.sdu != nullptr and s.discard_timer.is_valid() and s.discard_timer.is_running();
    });
  }

  bool add_sdu(uint32_t                             sn,
               const srslte::unique_byte_buffer_t&  sdu,
               uint32_t                             discard_timeout,
               const std::function<void(uint32_t)>& callback)
  {
    assert(discard_timeout < capacity and "Invalid discard timeout value");
    assert(not has_sdu(sn) && "Cannot add repeated SNs");

    if (is_full()) {
      return false;
    }

    // Make sure we don't associate more than half of the PDCP SN space of contiguous PDCP SDUs
    if (not empty()) {
      int32_t diff = sn - fms;
      if (diff > (int32_t)(capacity / 2)) {
        return false;
      }
      if (diff <= 0 && diff > -((int32_t)(capacity / 2))) {
        return false;
      }
    }

    // Allocate buffer and exit on error
    srslte::unique_byte_buffer_t tmp = make_byte_buffer();
    if (tmp == nullptr) {
      return false;
    }

    // Update FMS and LMS if necessary
    if (empty()) {
      fms = sn;
      lms = sn;
    } else {
      update_lms(sn);
    }
    // Add SDU
    count++;
    sdus[sn].sdu             = std::move(tmp);
    sdus[sn].sdu->md.pdcp_sn = sn;
    sdus[sn].sdu->N_bytes    = sdu->N_bytes;
    memcpy(sdus[sn].sdu->msg, sdu->msg, sdu->N_bytes);
    if (discard_timeout > 0) {
      sdus[sn].discard_timer.set(discard_timeout, callback);
      sdus[sn].discard_timer.run();
    }
    sdus[sn].sdu->set_timestamp(); // Metrics
    bytes += sdu->N_bytes;
    return true;
  }

  unique_byte_buffer_t& operator[](uint32_t sn)
  {
    assert(has_sdu(sn));
    return sdus[sn].sdu;
  }

  bool clear_sdu(uint32_t sn)
  {
    if (not has_sdu(sn)) {
      return false;
    }
    count--;
    bytes -= sdus[sn].sdu->N_bytes;
    sdus[sn].discard_timer.clear();
    sdus[sn].sdu.reset();
    // Find next FMS,
    update_fms();
    return true;
  }

  void clear()
  {
    count = 0;
    bytes = 0;
    fms   = 0;
    for (uint32_t sn = 0; sn < capacity; sn++) {
      sdus[sn].discard_timer.clear();
      sdus[sn].sdu.reset();
    }
  }

  uint32_t get_bytes() const { return bytes; }

  uint32_t get_fms() const { return fms; }

  void set_fms(uint32_t fms_) { fms = fms_; }

  void update_fms()
  {
    if (empty()) {
      fms = increment_sn(fms);
      return;
    }

    for (uint32_t i = 0; i < capacity; ++i) {
      uint32_t sn = increment_sn(fms + i);
      if (has_sdu(sn)) {
        fms = sn;
        return;
      }
    }

    fms = increment_sn(fms);
  }

  void update_lms(uint32_t sn)
  {
    if (empty()) {
      lms = fms;
      return;
    }

    int32_t diff = sn - lms;
    if (diff > 0 && sn > lms) {
      lms = sn;
    } else if (diff < 0 && sn < lms) {
      lms = sn;
    }
  }

  uint32_t get_lms() const { return lms; }

private:
  const static uint32_t capacity   = 4096;
  const static uint32_t invalid_sn = -1;

  static uint32_t increment_sn(uint32_t sn) { return (sn + 1) % capacity; }

  struct sdu_data {
    srslte::unique_byte_buffer_t sdu;
    srslte::unique_timer         discard_timer;
  };

  uint32_t                                   count = 0;
  uint32_t                                   bytes = 0;
  uint32_t                                   fms   = 0;
  uint32_t                                   lms   = 0;
  srslte::circular_array<sdu_data, capacity> sdus;
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
                  srslte::task_sched_handle  task_sched_,
                  srslog::basic_logger&      logger,
                  uint32_t                   lcid_,
                  pdcp_config_t              cfg_);
  ~pdcp_entity_lte() override;
  void reset() override;
  void reestablish() override;

  // GW/RRC interface
  void write_sdu(unique_byte_buffer_t sdu, int sn = -1) override;

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu) override;
  void notify_failure(const std::vector<uint32_t>& pdcp_sns) override;
  void notify_delivery(const std::vector<uint32_t>& pdcp_sns) override;

  // Config helpers
  bool check_valid_config();

  // TX SDU queue helper
  bool store_sdu(uint32_t tx_count, const unique_byte_buffer_t& pdu);

  // Getter for unacknowledged PDUs. Used for handover
  std::map<uint32_t, srslte::unique_byte_buffer_t> get_buffered_pdus() override;

  // Status report helper(s)
  void send_status_report() override;
  void handle_status_report_pdu(srslte::unique_byte_buffer_t pdu);

  // Internal state getters/setters
  void get_bearer_state(pdcp_lte_state_t* state) override;
  void set_bearer_state(const pdcp_lte_state_t& state) override;

  // Metrics helpers
  pdcp_bearer_metrics_t get_metrics() override;
  void                  reset_metrics() override;

  size_t nof_discard_timers() const { return undelivered_sdus.nof_discard_timers(); }

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;

  // State variables, as defined in TS 36 323, section 7.1
  pdcp_lte_state_t st = {};

  uint32_t reordering_window = 0;
  uint32_t maximum_pdcp_sn   = 0;

  // PDU handlers
  void handle_control_pdu(srslte::unique_byte_buffer_t pdu);
  void handle_srb_pdu(srslte::unique_byte_buffer_t pdu);
  void handle_um_drb_pdu(srslte::unique_byte_buffer_t pdu);
  void handle_am_drb_pdu(srslte::unique_byte_buffer_t pdu);

  // Discard callback (discardTimer)
  class discard_callback;

  // TX Queue
  uint32_t               maximum_allocated_sns_window = 2048;
  undelivered_sdus_queue undelivered_sdus;
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

} // namespace srslte
#endif // SRSLTE_PDCP_ENTITY_LTE_H
