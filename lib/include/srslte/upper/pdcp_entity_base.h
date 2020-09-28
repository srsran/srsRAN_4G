/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_PDCP_ENTITY_BASE_H
#define SRSLTE_PDCP_ENTITY_BASE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/logmap.h"
#include "srslte/common/security.h"
#include "srslte/common/task_scheduler.h"
#include "srslte/common/threads.h"
#include "srslte/common/timers.h"
#include "srslte/interfaces/pdcp_interface_types.h"
#include "srslte/upper/byte_buffer_queue.h"

namespace srslte {

/****************************************************************************
 * Structs and Defines common to both LTE and NR
 * Ref: 3GPP TS 36.323 v10.1.0 and TS 38.323 v15.2.0
 ***************************************************************************/

#define PDCP_PDU_TYPE_PDCP_STATUS_REPORT 0x0
#define PDCP_PDU_TYPE_INTERSPERSED_ROHC_FEEDBACK_PACKET 0x1

// Maximum supported PDCP SDU size is 9000 bytes.
// See TS 38.323 v15.2.0, section 4.3.1
#define PDCP_MAX_SDU_SIZE 9000

typedef enum {
  PDCP_D_C_CONTROL_PDU = 0,
  PDCP_D_C_DATA_PDU,
  PDCP_D_C_N_ITEMS,
} pdcp_d_c_t;
static const char pdcp_d_c_text[PDCP_D_C_N_ITEMS][20] = {"Control PDU", "Data PDU"};

/****************************************************************************
 * PDCP Entity interface
 * Common interface for LTE and NR PDCP entities
 ***************************************************************************/
class pdcp_entity_base
{
public:
  pdcp_entity_base(task_sched_handle task_sched_, srslte::log_ref log_);
  pdcp_entity_base(pdcp_entity_base&&) = default;
  virtual ~pdcp_entity_base();
  virtual void reset()       = 0;
  virtual void reestablish() = 0;

  bool is_active() { return active; }
  bool is_srb() { return cfg.rb_type == PDCP_RB_IS_SRB; }
  bool is_drb() { return cfg.rb_type == PDCP_RB_IS_DRB; }

  // RRC interface
  void enable_integrity(srslte_direction_t direction = DIRECTION_TXRX)
  {
    // if either DL or UL is already enabled, both are enabled
    if (integrity_direction == DIRECTION_TX && direction == DIRECTION_RX) {
      integrity_direction = DIRECTION_TXRX;
    } else if (integrity_direction == DIRECTION_RX && direction == DIRECTION_TX) {
      integrity_direction = DIRECTION_TXRX;
    } else {
      integrity_direction = direction;
    }
    log->debug("LCID=%d, integrity=%s\n", lcid, srslte_direction_text[integrity_direction]);
  }

  void enable_encryption(srslte_direction_t direction = DIRECTION_TXRX)
  {
    // if either DL or UL is already enabled, both are enabled
    if (encryption_direction == DIRECTION_TX && direction == DIRECTION_RX) {
      encryption_direction = DIRECTION_TXRX;
    } else if (encryption_direction == DIRECTION_RX && direction == DIRECTION_TX) {
      encryption_direction = DIRECTION_TXRX;
    } else {
      encryption_direction = direction;
    }
    log->debug("LCID=%d encryption=%s\n", lcid, srslte_direction_text[integrity_direction]);
  }

  void enable_security_timed(srslte_direction_t direction, uint32_t sn)
  {
    switch (direction) {
      case DIRECTION_TX:
        enable_security_tx_sn = sn;
        break;
      case DIRECTION_RX:
        enable_security_rx_sn = sn;
        break;
      default:
        log->error("Timed security activation for direction %s not supported.\n", srslte_direction_text[direction]);
        break;
    }
  }

  void config_security(as_security_config_t sec_cfg_);

  // GW/SDAP/RRC interface
  virtual void write_sdu(unique_byte_buffer_t sdu) = 0;

  // RLC interface
  virtual void write_pdu(unique_byte_buffer_t pdu) = 0;

  virtual void get_bearer_state(pdcp_lte_state_t* state)       = 0;
  virtual void set_bearer_state(const pdcp_lte_state_t& state) = 0;

  // COUNT, HFN and SN helpers
  uint32_t HFN(uint32_t count);
  uint32_t SN(uint32_t count);
  uint32_t COUNT(uint32_t hfn, uint32_t sn);

protected:
  srslte::log_ref           log;
  srslte::task_sched_handle task_sched;

  bool               active               = false;
  uint32_t           lcid                 = 0;
  srslte_direction_t integrity_direction  = DIRECTION_NONE;
  srslte_direction_t encryption_direction = DIRECTION_NONE;

  int32_t enable_security_tx_sn = -1; // TX SN at which security will be enabled
  int32_t enable_security_rx_sn = -1; // RX SN at which security will be enabled

  pdcp_config_t cfg = {1,
                       PDCP_RB_IS_DRB,
                       SECURITY_DIRECTION_DOWNLINK,
                       SECURITY_DIRECTION_UPLINK,
                       PDCP_SN_LEN_12,
                       pdcp_t_reordering_t::ms500,
                       pdcp_discard_timer_t::infinity};

  srslte::as_security_config_t sec_cfg = {};

  // Security functions
  void integrity_generate(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac);
  bool integrity_verify(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac);
  void cipher_encrypt(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* ct);
  void cipher_decrypt(uint8_t* ct, uint32_t ct_len, uint32_t count, uint8_t* msg);

  // Common packing functions
  bool     is_control_pdu(const unique_byte_buffer_t& pdu);
  uint32_t read_data_header(const unique_byte_buffer_t& pdu);
  void     discard_data_header(const unique_byte_buffer_t& pdu);
  void     write_data_header(const srslte::unique_byte_buffer_t& sdu, uint32_t count);
  void     extract_mac(const unique_byte_buffer_t& pdu, uint8_t* mac);
  void     append_mac(const unique_byte_buffer_t& sdu, uint8_t* mac);
};

inline uint32_t pdcp_entity_base::HFN(uint32_t count)
{
  return (count >> cfg.sn_len);
}

inline uint32_t pdcp_entity_base::SN(uint32_t count)
{
  return count & (0xFFFFFFFF >> (32 - cfg.sn_len));
}

inline uint32_t pdcp_entity_base::COUNT(uint32_t hfn, uint32_t sn)
{
  return (hfn << cfg.sn_len) | sn;
}

} // namespace srslte

#endif // SRSLTE_PDCP_ENTITY_BASE_H
