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

/******************************************************************************
 * File:        pdcp_config.h
 * Description: Structures used for configuration of the PDCP entities
 *****************************************************************************/

#ifndef SRSLTE_PDCP_INTERFACE_TYPES_H
#define SRSLTE_PDCP_INTERFACE_TYPES_H

#include "srslte/common/security.h"
#include <stdint.h>

namespace srslte {

/***************************
 *      PDCP Config
 **************************/
// LTE and NR common config
const uint8_t PDCP_SN_LEN_5  = 5;
const uint8_t PDCP_SN_LEN_7  = 7;
const uint8_t PDCP_SN_LEN_12 = 12;
const uint8_t PDCP_SN_LEN_18 = 18;

typedef enum { PDCP_RB_IS_SRB, PDCP_RB_IS_DRB } pdcp_rb_type_t;

enum pdcp_dc_field_t {
  PDCP_DC_FIELD_CONTROL_PDU = 0,
  PDCP_DC_FIELD_DATA_PDU,
  PDCP_DC_FIELD_N_ITEMS,
};
static const char* pdcp_dc_field_text[PDCP_DC_FIELD_N_ITEMS] = {"Control PDU", "Data PDU"};

// Taken from PDCP-Config (TS 38.331 version 15.2.1)
enum class pdcp_t_reordering_t {
  ms0    = 0,
  ms1    = 1,
  ms2    = 2,
  ms4    = 4,
  ms5    = 5,
  ms8    = 8,
  ms10   = 10,
  ms15   = 15,
  ms20   = 20,
  ms30   = 30,
  ms40   = 40,
  ms50   = 50,
  ms60   = 60,
  ms80   = 80,
  ms100  = 100,
  ms120  = 120,
  ms140  = 140,
  ms160  = 160,
  ms180  = 180,
  ms200  = 200,
  ms220  = 220,
  ms240  = 240,
  ms260  = 260,
  ms280  = 280,
  ms300  = 300,
  ms500  = 500,
  ms750  = 750,
  ms1000 = 1000,
  ms1250 = 1250,
  ms1500 = 1500,
  ms1750 = 1750,
  ms2000 = 2000,
  ms2250 = 2250,
  ms2500 = 2500,
  ms2750 = 2750,
  ms3000 = 3000
};

// Taken from PDCP-Config (TS 38.331 version 15.2.1)
enum class pdcp_discard_timer_t {
  ms10     = 10,
  ms20     = 20,
  ms30     = 30,
  ms40     = 40,
  ms50     = 50,
  ms60     = 60,
  ms75     = 75,
  ms100    = 100,
  ms150    = 150,
  ms200    = 200,
  ms250    = 250,
  ms300    = 300,
  ms500    = 500,
  ms750    = 750,
  ms1500   = 1500,
  infinity = 0
};

class pdcp_config_t
{
public:
  pdcp_config_t(uint8_t              bearer_id_,
                pdcp_rb_type_t       rb_type_,
                security_direction_t tx_direction_,
                security_direction_t rx_direction_,
                uint8_t              sn_len_,
                pdcp_t_reordering_t  t_reordering_,
                pdcp_discard_timer_t discard_timer_) :
    bearer_id(bearer_id_),
    rb_type(rb_type_),
    tx_direction(tx_direction_),
    rx_direction(rx_direction_),
    sn_len(sn_len_),
    t_reordering(t_reordering_),
    discard_timer(discard_timer_)
  {
    hdr_len_bytes = ceil((float)sn_len / 8);
  }

  uint8_t              bearer_id     = 1;
  pdcp_rb_type_t       rb_type       = PDCP_RB_IS_DRB;
  security_direction_t tx_direction  = SECURITY_DIRECTION_DOWNLINK;
  security_direction_t rx_direction  = SECURITY_DIRECTION_UPLINK;
  uint8_t              sn_len        = PDCP_SN_LEN_12;
  uint8_t              hdr_len_bytes = 2;

  pdcp_t_reordering_t  t_reordering  = pdcp_t_reordering_t::ms500;
  pdcp_discard_timer_t discard_timer = pdcp_discard_timer_t::infinity;

  // TODO: Support the following configurations
  // bool do_rohc;
};

// Specifies in which direction security (integrity and ciphering) are enabled for PDCP
enum srslte_direction_t { DIRECTION_NONE = 0, DIRECTION_TX, DIRECTION_RX, DIRECTION_TXRX, DIRECTION_N_ITEMS };
static const char* srslte_direction_text[DIRECTION_N_ITEMS] = {"none", "tx", "rx", "tx/rx"};

// PDCP LTE internal state variables, as defined in TS 36 323, section 7.1
struct pdcp_lte_state_t {
  uint32_t next_pdcp_tx_sn;
  uint32_t tx_hfn;
  uint32_t rx_hfn;
  uint32_t next_pdcp_rx_sn;
  uint32_t last_submitted_pdcp_rx_sn;
  uint32_t reordering_pdcp_rx_count;
};

} // namespace srslte

#endif // SRSLTE_PDCP_INTERFACE_TYPES_H
