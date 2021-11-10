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
#include "srsran/rlc/rlc_am_data_structs.h"
#include "srsran/rlc/rlc_am_nr_packing.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsran {

/******************************
 *
 * RLC AM NR entity
 *
 *****************************/
class rlc_am_nr : public rlc_am
{
public:
  rlc_am_nr(srslog::basic_logger&      logger,
            uint32_t                   lcid_,
            srsue::pdcp_interface_rlc* pdcp_,
            srsue::rrc_interface_rlc*  rrc_,
            srsran::timer_handler*     timers_);

  // Transmitter sub-class
  class rlc_am_nr_tx : public rlc_am_base_tx
  {
  public:
    explicit rlc_am_nr_tx(rlc_am_nr* parent_);
    ~rlc_am_nr_tx() = default;

    bool     configure(const rlc_config_t& cfg_) final;
    uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;
    void     handle_control_pdu(uint8_t* payload, uint32_t nof_bytes) final;

    void discard_sdu(uint32_t discard_sn) final;
    bool sdu_queue_is_full() final;
    void reestablish() final;

    int      write_sdu(unique_byte_buffer_t sdu);
    void     empty_queue() final;
    bool     has_data() final;
    uint32_t get_buffer_state() final;
    void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue);

    void stop() final;

  private:
    rlc_am_nr* parent = nullptr;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
     ***************************************************************************/
    rlc_am_config_t cfg = {};

    /****************************************************************************
     * Tx state variables
     * Ref: 3GPP TS 38.322 v10.0.0 Section 7.1
     ***************************************************************************/
    struct rlc_nr_tx_state_t {
      uint32_t tx_next_ack;
      uint32_t tx_next;
      uint32_t poll_sn;
      uint32_t pdu_without_poll;
      uint32_t byte_without_poll;
    } st = {};

    using rlc_amd_tx_pdu_nr = rlc_amd_tx_pdu<rlc_am_nr_pdu_header_t>;
    rlc_ringbuffer_t<rlc_amd_tx_pdu_nr, RLC_AM_WINDOW_SIZE> tx_window;
  };

  // Receiver sub-class
  class rlc_am_nr_rx : public rlc_am_base_rx
  {
  public:
    explicit rlc_am_nr_rx(rlc_am_nr* parent_);
    ~rlc_am_nr_rx() = default;

    bool configure(const rlc_config_t& cfg_) final;

    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) final;

    void stop();
    void reestablish();

    uint32_t get_sdu_rx_latency_ms();
    uint32_t get_rx_buffered_bytes();

  private:
    rlc_am_nr*        parent = nullptr;
    byte_buffer_pool* pool   = nullptr;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
     ***************************************************************************/
    rlc_am_config_t cfg = {};
  };

private:
  rlc_am_nr_tx* tx = nullptr;
  rlc_am_nr_rx* rx = nullptr;
};

} // namespace srsran
#endif // SRSRAN_RLC_AM_NR_H
