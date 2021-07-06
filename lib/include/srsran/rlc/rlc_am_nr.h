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

#ifndef SRSRAN_RLC_AM_NR_H
#define SRSRAN_RLC_AM_NR_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/timers.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsran {

// Transmitter sub-class
class rlc_am_nr_tx;
class rlc_am_nr_rx;
using rlc_am_nr = rlc_am_base<rlc_am_nr_tx, rlc_am_nr_rx>;

// Transmitter sub-class
class rlc_am_nr_tx
{
public:
  explicit rlc_am_nr_tx(rlc_am_nr* parent_);
  ~rlc_am_nr_tx() = default;

  bool configure(const rlc_config_t& cfg_);
  void stop();

  int      write_sdu(unique_byte_buffer_t sdu);
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes);
  void     discard_sdu(uint32_t discard_sn);
  bool     sdu_queue_is_full();
  void     reestablish();

  void     empty_queue();
  bool     has_data();
  uint32_t get_buffer_state();
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue);
  void     set_bsr_callback(const bsr_callback_t& callback);

private:
  rlc_am_nr*            parent = nullptr;
  byte_buffer_pool*     pool   = nullptr;
  srslog::basic_logger& logger;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
   ***************************************************************************/
  rlc_am_config_t cfg = {};
};

// Receiver sub-class
class rlc_am_nr_rx
{
public:
  explicit rlc_am_nr_rx(rlc_am_nr* parent_);
  ~rlc_am_nr_rx() = default;

  bool configure(const rlc_config_t& cfg_);
  void stop();
  void reestablish();

  void     write_pdu(uint8_t* payload, uint32_t nof_bytes);
  uint32_t get_sdu_rx_latency_ms();
  uint32_t get_rx_buffered_bytes();

private:
  rlc_am_nr*            parent = nullptr;
  byte_buffer_pool*     pool   = nullptr;
  srslog::basic_logger& logger;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
   ***************************************************************************/
  rlc_am_config_t cfg = {};
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_NR_H
