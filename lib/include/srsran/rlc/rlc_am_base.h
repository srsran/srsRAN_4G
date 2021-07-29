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

#ifndef SRSRAN_RLC_AM_BASE_H
#define SRSRAN_RLC_AM_BASE_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_common.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsue {

class pdcp_interface_rlc;
class rrc_interface_rlc;

} // namespace srsue

namespace srsran {

bool rlc_am_is_control_pdu(uint8_t* payload);
bool rlc_am_is_control_pdu(byte_buffer_t* pdu);

/*******************************************************
 *     RLC AM entity
 *     This entity is common between LTE and NR
 *     and only the TX/RX entities change between them
 *******************************************************/
class rlc_am_base : public rlc_common
{
protected:
  class rlc_am_base_tx;
  class rlc_am_base_rx;

public:
  rlc_am_base(srslog::basic_logger&      logger,
              uint32_t                   lcid_,
              srsue::pdcp_interface_rlc* pdcp_,
              srsue::rrc_interface_rlc*  rrc_,
              srsran::timer_handler*     timers_,
              rlc_am_base_tx*            tx_,
              rlc_am_base_rx*            rx_) :
    logger(logger), rrc(rrc_), pdcp(pdcp_), timers(timers_), lcid(lcid_), tx_base(tx_), rx_base(rx_)
  {}

  bool configure(const rlc_config_t& cfg_) final;

  void reestablish() final;

  void stop() final;

  void empty_queue() final { tx_base->empty_queue(); }

  rlc_mode_t get_mode() final { return rlc_mode_t::am; }

  uint32_t get_bearer() final { return lcid; }

  /****************************************************************************
   * PDCP interface
   ***************************************************************************/
  void write_sdu(unique_byte_buffer_t sdu) final;

  void discard_sdu(uint32_t discard_sn) final;

  bool sdu_queue_is_full() final;

  /****************************************************************************
   * MAC interface
   ***************************************************************************/
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio) final;

  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  void write_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  /****************************************************************************
   * Metrics
   ***************************************************************************/
  rlc_bearer_metrics_t get_metrics() final;
  void                 reset_metrics() final;

  /****************************************************************************
   * BSR Callback
   ***************************************************************************/
  void set_bsr_callback(bsr_callback_t callback) final;

protected:
  // Common variables needed/provided by parent class
  srslog::basic_logger&  logger;
  srsran::timer_handler* timers = nullptr;
  uint32_t               lcid   = 0;
  rlc_config_t           cfg    = {};
  std::string            rb_name;

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested

  std::mutex           metrics_mutex;
  rlc_bearer_metrics_t metrics = {};

  srsue::rrc_interface_rlc*  rrc  = nullptr;
  srsue::pdcp_interface_rlc* pdcp = nullptr;

  /*******************************************************
   *     RLC AM TX entity
   *     This class is used for common code between the
   *     LTE and NR TX entitites
   *******************************************************/
  class rlc_am_base_tx
  {
  public:
    explicit rlc_am_base_tx(srslog::basic_logger* logger_) : logger(logger_) {}

    virtual bool     configure(const rlc_config_t& cfg_)                           = 0;
    virtual void     handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)      = 0;
    virtual uint32_t get_buffer_state()                                            = 0;
    virtual void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) = 0;
    virtual void     reestablish()                                                 = 0;
    virtual void     empty_queue()                                                 = 0;
    virtual void     discard_sdu(uint32_t pdcp_sn)                                 = 0;
    virtual bool     sdu_queue_is_full()                                           = 0;
    virtual bool     has_data()                                                    = 0;
    virtual void     stop()                                                        = 0;

    void set_bsr_callback(bsr_callback_t callback);

    int              write_sdu(unique_byte_buffer_t sdu);
    virtual uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;

    bool                  tx_enabled = false;
    byte_buffer_pool*     pool       = nullptr;
    srslog::basic_logger* logger;
    std::string           rb_name;

    bsr_callback_t bsr_callback;

    // Tx SDU buffers
    byte_buffer_queue tx_sdu_queue;

    // Mutexes
    std::mutex mutex;
  };

  /*******************************************************
   *     RLC AM RX entity
   *     This class is used for common code between the
   *     LTE and NR RX entitites
   *******************************************************/
  class rlc_am_base_rx
  {
  public:
    explicit rlc_am_base_rx(rlc_am_base* parent_, srslog::basic_logger* logger_) : parent(parent_), logger(logger_) {}

    virtual bool     configure(const rlc_config_t& cfg_)                   = 0;
    virtual void     handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;
    virtual void     reestablish()                                         = 0;
    virtual void     stop()                                                = 0;
    virtual uint32_t get_sdu_rx_latency_ms()                               = 0;
    virtual uint32_t get_rx_buffered_bytes()                               = 0;

    void write_pdu(uint8_t* payload, uint32_t nof_bytes);

    srslog::basic_logger* logger = nullptr;
    byte_buffer_pool*     pool   = nullptr;
    rlc_am_base*          parent = nullptr;
  };

  rlc_am_base_tx* tx_base = nullptr;
  rlc_am_base_rx* rx_base = nullptr;
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_BASE_H
