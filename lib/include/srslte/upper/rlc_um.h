/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_RLC_UM_H
#define SRSLTE_RLC_UM_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc_common.h"
#include "srslte/upper/rlc_tx_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srslte {

struct rlc_umd_pdu_t{
  rlc_umd_pdu_header_t  header;
  unique_byte_buffer_t  buf;
};

typedef struct {
  rlc_um_nr_pdu_header_t header;
  unique_byte_buffer_t   buf;
} rlc_umd_pdu_nr_t;

class rlc_um
    :public rlc_common
{
public:
  rlc_um(srslte::log*               log_,
         uint32_t                   lcid_,
         srsue::pdcp_interface_rlc* pdcp_,
         srsue::rrc_interface_rlc*  rrc_,
         srslte::timers*            timers_);
  ~rlc_um();
  bool configure(rlc_config_t cnfg);
  void reestablish();
  void stop();
  void empty_queue();
  bool is_mrb();

  rlc_mode_t    get_mode();
  uint32_t      get_bearer();

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu, bool blocking = true);

  // MAC interface
  bool     has_data();
  uint32_t get_buffer_state();
  int      read_pdu(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t *payload, uint32_t nof_bytes);
  int get_increment_sequence_num();

  uint32_t get_num_tx_bytes();
  uint32_t get_num_rx_bytes();
  void reset_metrics();

private:
  // Transmitter sub-class base
  class rlc_um_tx_base
  {
  public:
    rlc_um_tx_base(srslte::log* log_);
    virtual ~rlc_um_tx_base();
    virtual bool configure(rlc_config_t cfg, std::string rb_name) = 0;
    int  build_data_pdu(uint8_t *payload, uint32_t nof_bytes);
    void stop();
    void reestablish();
    void empty_queue();
    void     write_sdu(unique_byte_buffer_t sdu);
    void     try_write_sdu(unique_byte_buffer_t sdu);
    uint32_t get_num_tx_bytes();
    void reset_metrics();
    bool has_data();
    uint32_t get_buffer_state();

  protected:
    byte_buffer_pool*       pool = nullptr;
    srslte::log*            log  = nullptr;
    std::string             rb_name;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    rlc_config_t cfg = {};

    // TX SDU buffers
    rlc_tx_queue            tx_sdu_queue;
    unique_byte_buffer_t    tx_sdu;

    // Mutexes
    std::mutex mutex;

    bool tx_enabled = false;

    uint32_t num_tx_bytes = 0;

    virtual int build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes) = 0;

    // helper functions
    virtual void debug_state() = 0;
    const char*  get_rb_name();
  };

  // Transmitter sub-class for LTE
  class rlc_um_tx : public rlc_um_tx_base
  {
  public:
    rlc_um_tx(srslte::log* log_);

    bool configure(rlc_config_t cfg, std::string rb_name);
    int  build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes);

  private:
    /****************************************************************************
     * State variables and counters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    uint32_t vt_us = 0; // Send state. SN to be assigned for next PDU.

    void debug_state();
  };

  // Transmitter sub-class for NR
  class rlc_um_tx_nr : public rlc_um_tx_base
  {
  public:
    rlc_um_tx_nr(srslte::log* log_);

    bool configure(rlc_config_t cfg, std::string rb_name);
    int  build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes);

  private:
    uint32_t TX_Next = 0; // send state as defined in TS 38.322 v15.3 Section 7

    void debug_state();
  };

  // Receiver sub-class
  class rlc_um_rx : public timer_callback {
  public:
    rlc_um_rx(srslte::log*               log_,
              uint32_t                   lcid_,
              srsue::pdcp_interface_rlc* pdcp_,
              srsue::rrc_interface_rlc*  rrc_,
              srslte::timers*            timers_);
    ~rlc_um_rx();
    void     stop();
    void     reestablish();
    bool     configure(rlc_config_t cfg, std::string rb_name);
    void handle_data_pdu(uint8_t *payload, uint32_t nof_bytes);
    void reassemble_rx_sdus();
    bool pdu_belongs_to_rx_sdu();
    bool inside_reordering_window(uint16_t sn);
    uint32_t get_num_rx_bytes();
    void reset_metrics();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

  private:
    void reset();

    byte_buffer_pool* pool   = nullptr;
    srslte::log*      log    = nullptr;
    srslte::timers*   timers = nullptr;
    std::string       rb_name;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    rlc_config_t cfg = {};

    // Rx window
    std::map<uint32_t, rlc_umd_pdu_t>   rx_window;

    // RX SDU buffers
    unique_byte_buffer_t                rx_sdu;
    uint32_t                            vr_ur_in_rx_sdu = 0;

    // Rx state variables and counter
    uint32_t vr_ur    = 0; // Receive state. SN of earliest PDU still considered for reordering.
    uint32_t vr_ux    = 0; // t_reordering state. SN following PDU which triggered t_reordering.
    uint32_t vr_uh    = 0; // Highest rx state. SN following PDU with highest SN among rxed PDUs.
    bool     pdu_lost = false;

    uint32_t num_rx_bytes = 0;

    // Upper layer handles and variables
    srsue::pdcp_interface_rlc* pdcp = nullptr;
    srsue::rrc_interface_rlc*  rrc  = nullptr;
    uint32_t                   lcid = 0;

    // Mutexes
    std::mutex mutex;

    bool rx_enabled = false;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    srslte::timers::timer* reordering_timer    = nullptr;
    uint32_t               reordering_timer_id = 0;

    // helper functions
    void debug_state();
    const char* get_rb_name();
  };

  // Common variables needed by parent class
  srsue::rrc_interface_rlc* rrc               = nullptr;
  srslte::log*              log               = nullptr;
  uint32_t                  lcid              = 0;
  rlc_config_t              cfg               = {};
  std::string               rb_name;
  byte_buffer_pool*         pool = nullptr;

  std::string               get_rb_name(srsue::rrc_interface_rlc *rrc, uint32_t lcid, bool is_mrb);

  // Rx and Tx objects
  std::unique_ptr<rlc_um_tx_base> tx;
  rlc_um_rx rx;
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void rlc_um_read_data_pdu_header(byte_buffer_t* pdu, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t* header);
void        rlc_um_read_data_pdu_header(uint8_t *payload, uint32_t nof_bytes, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t *header);
void rlc_um_write_data_pdu_header(rlc_umd_pdu_header_t* header, byte_buffer_t* pdu);

uint32_t    rlc_um_packed_length(rlc_umd_pdu_header_t *header);
bool        rlc_um_start_aligned(uint8_t fi);
bool        rlc_um_end_aligned(uint8_t fi);

/****************************************************************************
 * Header pack/unpack helper functions for NR
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.3
 ***************************************************************************/
uint32_t rlc_um_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header);

uint32_t rlc_um_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header);

uint32_t rlc_um_nr_write_data_pdu_header(const rlc_um_nr_pdu_header_t& header, byte_buffer_t* pdu);

uint32_t rlc_um_nr_packed_length(const rlc_um_nr_pdu_header_t& header);

} // namespace srslte

#endif // SRSLTE_RLC_UM_H
