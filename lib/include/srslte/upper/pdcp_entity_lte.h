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

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/pdcp_entity_base.h"

namespace srslte {

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
  void write_sdu(unique_byte_buffer_t sdu) override;

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu) override;
  void notify_delivery(const std::vector<uint32_t>& pdcp_sns) override;

  // Config helpers
  bool check_valid_config();

  // TX SDU queue helper
  bool store_sdu(uint32_t tx_count, const unique_byte_buffer_t& pdu);

  // Getter for unacknowledged PDUs. Used for handover
  std::map<uint32_t, srslte::unique_byte_buffer_t> get_buffered_pdus() override;

  // Internal state getters/setters
  void get_bearer_state(pdcp_lte_state_t* state) override;
  void set_bearer_state(const pdcp_lte_state_t& state) override;

  // Getter for the number of discard timers. Used for debugging.
  uint32_t nof_discard_timers() { return discard_timers_map.size(); }

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;

  byte_buffer_pool* pool = nullptr;

  // State variables, as defined in TS 36 323, section 7.1
  pdcp_lte_state_t st = {};

  uint32_t reordering_window = 0;
  uint32_t maximum_pdcp_sn   = 0;

  // Discard callback (discardTimer)
  class discard_callback;
  std::map<uint32_t, timer_handler::unique_timer> discard_timers_map;

  // TX Queue
  uint32_t                                 tx_queue_capacity = 512;
  std::map<uint32_t, unique_byte_buffer_t> undelivered_sdus_queue;

  void handle_srb_pdu(srslte::unique_byte_buffer_t pdu);
  void handle_um_drb_pdu(srslte::unique_byte_buffer_t pdu);
  void handle_am_drb_pdu(srslte::unique_byte_buffer_t pdu);
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
