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

#ifndef SRSRAN_RLC_TM_H
#define SRSRAN_RLC_TM_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/rlc/rlc_common.h"
#include "srsran/upper/byte_buffer_queue.h"

namespace srsue {

class pdcp_interface_rlc;
class rrc_interface_rlc;

} // namespace srsue

namespace srsran {

class rlc_tm final : public rlc_common
{
public:
  rlc_tm(srslog::basic_logger&      logger,
         uint32_t                   lcid_,
         srsue::pdcp_interface_rlc* pdcp_,
         srsue::rrc_interface_rlc*  rrc_);
  ~rlc_tm() override;
  bool configure(const rlc_config_t& cnfg) override;
  void stop() override;
  void reestablish() override;
  void empty_queue() override;

  rlc_mode_t get_mode() override;
  uint32_t   get_bearer() override;

  rlc_bearer_metrics_t get_metrics() override;
  void                 reset_metrics() override;

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu) override;
  void discard_sdu(uint32_t discard_sn) override;
  bool sdu_queue_is_full() override;

  // MAC interface
  bool     has_data() override;
  uint32_t get_buffer_state() override;
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) override;
  void     write_pdu(uint8_t* payload, uint32_t nof_bytes) override;

  void set_bsr_callback(bsr_callback_t callback) override {}

private:
  byte_buffer_pool*          pool = nullptr;
  srslog::basic_logger&      logger;
  uint32_t                   lcid = 0;
  srsue::pdcp_interface_rlc* pdcp = nullptr;
  srsue::rrc_interface_rlc*  rrc  = nullptr;

  std::atomic<bool> tx_enabled = {true};

  rlc_bearer_metrics_t metrics = {};

  // Thread-safe queues for MAC messages
  byte_buffer_queue ul_queue;
};

} // namespace srsran

#endif // SRSRAN_RLC_TM_H
