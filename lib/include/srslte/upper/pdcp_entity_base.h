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

#ifndef SRSLTE_PDCP_ENTITY_BASE_H
#define SRSLTE_PDCP_ENTITY_BASE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/ue_interfaces.h"
#include <mutex>

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
  pdcp_entity_base();
  virtual ~pdcp_entity_base();
  virtual void reset()       = 0;
  virtual void reestablish() = 0;

  bool is_active() { return active; }
  bool is_srb() { return cfg.rb_type == PDCP_RB_IS_SRB; }
  bool is_drb() { return cfg.rb_type == PDCP_RB_IS_DRB; }

  // RRC interface
  void enable_integrity() { do_integrity = true; }
  void enable_encryption() { do_encryption = true; }

  void config_security(uint8_t*                    k_rrc_enc_,
                       uint8_t*                    k_rrc_int_,
                       uint8_t*                    k_up_enc_,
                       uint8_t*                    k_up_int_, // NR Only, pass nullptr in LTE
                       CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                       INTEGRITY_ALGORITHM_ID_ENUM integ_algo_);

  // GW/SDAP/RRC interface
  void write_sdu(unique_byte_buffer_t sdu, bool blocking);

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu);

  // COUNT, HFN and SN helpers
  uint32_t HFN(uint32_t count);
  uint32_t SN(uint32_t count);
  uint32_t COUNT(uint32_t hfn, uint32_t sn);

protected:
  srslte::log* log = nullptr;

  bool     active        = false;
  uint32_t lcid          = 0;
  bool     do_integrity  = false;
  bool     do_encryption = false;

  pdcp_config_t cfg = {1, PDCP_RB_IS_DRB, SECURITY_DIRECTION_DOWNLINK, SECURITY_DIRECTION_UPLINK, PDCP_SN_LEN_12};

  std::mutex mutex;

  uint8_t k_rrc_enc[32] = {};
  uint8_t k_rrc_int[32] = {};
  uint8_t k_up_enc[32]  = {};
  uint8_t k_up_int[32]  = {};

  CIPHERING_ALGORITHM_ID_ENUM cipher_algo = CIPHERING_ALGORITHM_ID_EEA0;
  INTEGRITY_ALGORITHM_ID_ENUM integ_algo  = INTEGRITY_ALGORITHM_ID_EIA0;

  void integrity_generate(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac);
  bool integrity_verify(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* mac);
  void cipher_encrypt(uint8_t* msg, uint32_t msg_len, uint32_t count, uint8_t* ct);
  void cipher_decrypt(uint8_t* ct, uint32_t ct_len, uint32_t count, uint8_t* msg);
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
