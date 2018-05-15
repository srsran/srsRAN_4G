/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_RLC_COMMON_H
#define SRSLTE_RLC_COMMON_H

#include "srslte/upper/rlc_interface.h"

namespace srslte {

/****************************************************************************
 * Structs and Defines
 * Ref: 3GPP TS 36.322 v10.0.0
 ***************************************************************************/

#define RLC_AM_WINDOW_SIZE  512

typedef enum{
  RLC_MODE_TM = 0,
  RLC_MODE_UM,
  RLC_MODE_AM,
  RLC_MODE_N_ITEMS,
}rlc_mode_t;
static const char rlc_mode_text[RLC_MODE_N_ITEMS][20] = {"Transparent Mode",
                                                         "Unacknowledged Mode",
                                                         "Acknowledged Mode"};

typedef enum{
  RLC_FI_FIELD_START_AND_END_ALIGNED = 0,
  RLC_FI_FIELD_NOT_END_ALIGNED,
  RLC_FI_FIELD_NOT_START_ALIGNED,
  RLC_FI_FIELD_NOT_START_OR_END_ALIGNED,
  RLC_FI_FIELD_N_ITEMS,
}rlc_fi_field_t;
static const char rlc_fi_field_text[RLC_FI_FIELD_N_ITEMS][32] = {"Start and end aligned",
                                                                 "Not end aligned",
                                                                 "Not start aligned",
                                                                 "Not start or end aligned"};

typedef enum{
  RLC_DC_FIELD_CONTROL_PDU = 0,
  RLC_DC_FIELD_DATA_PDU,
  RLC_DC_FIELD_N_ITEMS,
}rlc_dc_field_t;
static const char rlc_dc_field_text[RLC_DC_FIELD_N_ITEMS][20] = {"Control PDU",
                                                                 "Data PDU"};

// UMD PDU Header
typedef struct{
  uint8_t           fi;                     // Framing info
  rlc_umd_sn_size_t sn_size;                // Sequence number size (5 or 10 bits)
  uint16_t          sn;                     // Sequence number
  uint32_t          N_li;                   // Number of length indicators
  uint16_t          li[RLC_AM_WINDOW_SIZE]; // Array of length indicators
}rlc_umd_pdu_header_t;

// AMD PDU Header
struct rlc_amd_pdu_header_t{
  rlc_dc_field_t dc;                      // Data or control
  uint8_t        rf;                      // Resegmentation flag
  uint8_t        p;                       // Polling bit
  uint8_t        fi;                      // Framing info
  uint16_t       sn;                      // Sequence number
  uint8_t        lsf;                     // Last segment flag
  uint16_t       so;                      // Segment offset
  uint32_t       N_li;                    // Number of length indicators
  uint16_t       li[RLC_AM_WINDOW_SIZE];  // Array of length indicators

  rlc_amd_pdu_header_t(){
    dc = RLC_DC_FIELD_CONTROL_PDU;
    rf = 0; 
    p  = 0; 
    fi = 0; 
    sn = 0; 
    lsf = 0; 
    so = 0; 
    N_li=0;
    for(int i=0;i<RLC_AM_WINDOW_SIZE;i++)
      li[i] = 0;
  }
  rlc_amd_pdu_header_t(const rlc_amd_pdu_header_t& h)
  {
    copy(h);
  }
  rlc_amd_pdu_header_t& operator= (const rlc_amd_pdu_header_t& h)
  {
    copy(h);
    return *this;
  }
  void copy(const rlc_amd_pdu_header_t& h)
  {
    dc   = h.dc;
    rf   = h.rf;
    p    = h.p;
    fi   = h.fi;
    sn   = h.sn;
    lsf  = h.lsf;
    so   = h.so;
    N_li = h.N_li;
    for(uint32_t i=0;i<h.N_li;i++)
      li[i] = h.li[i];
  }
};

// NACK helper
struct rlc_status_nack_t{
  uint16_t nack_sn;
  bool     has_so;
  uint16_t so_start;
  uint16_t so_end;

  rlc_status_nack_t(){has_so=false; nack_sn=0; so_start=0; so_end=0;}
};

// STATUS PDU
struct rlc_status_pdu_t{
  uint16_t          ack_sn;
  uint32_t          N_nack;
  rlc_status_nack_t nacks[RLC_AM_WINDOW_SIZE];

  rlc_status_pdu_t(){N_nack=0; ack_sn=0;}
};

/****************************************************************************
 * RLC Common interface
 * Common interface for all RLC entities
 ***************************************************************************/
class rlc_common
{
public:
  virtual void init(srslte::log                       *rlc_entity_log_,
                    uint32_t                           lcid_,
                    srsue::pdcp_interface_rlc         *pdcp_,
                    srsue::rrc_interface_rlc          *rrc_,
                    srslte::mac_interface_timers      *mac_timers_) = 0;
  virtual void configure(srslte_rlc_config_t cnfg) = 0;
  virtual void reset() = 0;
  virtual void stop() = 0;
  virtual void empty_queue() = 0; 

  virtual rlc_mode_t    get_mode() = 0;
  virtual uint32_t      get_bearer() = 0;

  // PDCP interface
  virtual void write_sdu(byte_buffer_t *sdu) = 0;

  // MAC interface
  virtual uint32_t get_buffer_state() = 0;
  virtual uint32_t get_total_buffer_state() = 0;
  virtual int      read_pdu(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void     write_pdu(uint8_t *payload, uint32_t nof_bytes) = 0;
};

} // namespace srslte

#endif // SRSLTE_RLC_COMMON_H
