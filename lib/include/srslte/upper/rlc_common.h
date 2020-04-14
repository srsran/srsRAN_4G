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

#ifndef SRSLTE_RLC_COMMON_H
#define SRSLTE_RLC_COMMON_H

#include "srslte/common/block_queue.h"
#include "srslte/common/logmap.h"
#include "srslte/upper/rlc_metrics.h"
#include <stdlib.h>

namespace srslte {

/****************************************************************************
 * Structs and Defines
 * Ref: 3GPP TS 36.322 v10.0.0
 ***************************************************************************/

#define RLC_AM_WINDOW_SIZE 512
#define RLC_MAX_SDU_SIZE ((1 << 11) - 1) // Length of LI field is 11bits

typedef enum {
  RLC_FI_FIELD_START_AND_END_ALIGNED = 0,
  RLC_FI_FIELD_NOT_END_ALIGNED,
  RLC_FI_FIELD_NOT_START_ALIGNED,
  RLC_FI_FIELD_NOT_START_OR_END_ALIGNED,
  RLC_FI_FIELD_N_ITEMS,
} rlc_fi_field_t;
static const char rlc_fi_field_text[RLC_FI_FIELD_N_ITEMS][32] = {"Start and end aligned",
                                                                 "Not end aligned",
                                                                 "Not start aligned",
                                                                 "Not start or end aligned"};

enum class rlc_nr_si_field_t : unsigned {
  full_sdu                       = 0b00,
  first_segment                  = 0b01,
  last_segment                   = 0b10,
  neither_first_nor_last_segment = 0b11,
  nulltype
};
inline std::string to_string(const rlc_nr_si_field_t& si)
{
  constexpr static const char* options[] = {"Data field contains full SDU",
                                            "Data field contains first segment of SDU",
                                            "Data field contains last segment of SDU",
                                            "Data field contains neither first nor last segment of SDU"};
  return enum_to_text(options, (uint32_t)rlc_nr_si_field_t::nulltype, (uint32_t)si);
}

inline std::string to_string_short(const rlc_nr_si_field_t& si)
{
  constexpr static const char* options[] = {"full", "first", "last", "middle"};
  return enum_to_text(options, (uint32_t)rlc_nr_si_field_t::nulltype, (uint32_t)si);
}

static inline uint8_t operator&(rlc_nr_si_field_t lhs, int rhs)
{
  return static_cast<uint8_t>(static_cast<std::underlying_type<rlc_nr_si_field_t>::type>(lhs) &
                              static_cast<std::underlying_type<rlc_nr_si_field_t>::type>(rhs));
}

enum class rlc_am_nr_control_pdu_type_t : unsigned { status_pdu = 0b000, nulltype };
inline std::string to_string(const rlc_am_nr_control_pdu_type_t& type)
{
  constexpr static const char* options[] = {"Control PDU"};
  return enum_to_text(options, (uint32_t)rlc_am_nr_control_pdu_type_t::nulltype, (uint32_t)type);
}

typedef enum {
  RLC_DC_FIELD_CONTROL_PDU = 0,
  RLC_DC_FIELD_DATA_PDU,
  RLC_DC_FIELD_N_ITEMS,
} rlc_dc_field_t;
static const char rlc_dc_field_text[RLC_DC_FIELD_N_ITEMS][20] = {"Control PDU", "Data PDU"};

// UMD PDU Header
typedef struct {
  uint8_t           fi;                     // Framing info
  rlc_umd_sn_size_t sn_size;                // Sequence number size (5 or 10 bits)
  uint16_t          sn;                     // Sequence number
  uint32_t          N_li;                   // Number of length indicators
  uint16_t          li[RLC_AM_WINDOW_SIZE]; // Array of length indicators
} rlc_umd_pdu_header_t;

typedef struct {
  rlc_nr_si_field_t   si;      // Segmentation info
  rlc_um_nr_sn_size_t sn_size; // Sequence number size (6 or 12 bits)
  uint16_t            sn;      // Sequence number
  uint16_t            so;      // Segment offset
} rlc_um_nr_pdu_header_t;

// AMD PDU Header
struct rlc_amd_pdu_header_t {
  rlc_dc_field_t dc;                     // Data or control
  uint8_t        rf;                     // Resegmentation flag
  uint8_t        p;                      // Polling bit
  uint8_t        fi;                     // Framing info
  uint16_t       sn;                     // Sequence number
  uint8_t        lsf;                    // Last segment flag
  uint16_t       so;                     // Segment offset
  uint32_t       N_li;                   // Number of length indicators
  uint16_t       li[RLC_AM_WINDOW_SIZE]; // Array of length indicators

  rlc_amd_pdu_header_t()
  {
    dc   = RLC_DC_FIELD_CONTROL_PDU;
    rf   = 0;
    p    = 0;
    fi   = 0;
    sn   = 0;
    lsf  = 0;
    so   = 0;
    N_li = 0;
    for (int i = 0; i < RLC_AM_WINDOW_SIZE; i++)
      li[i] = 0;
  }
  rlc_amd_pdu_header_t(const rlc_amd_pdu_header_t& h) { copy(h); }
  rlc_amd_pdu_header_t& operator=(const rlc_amd_pdu_header_t& h)
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
    for (uint32_t i = 0; i < h.N_li; i++) {
      li[i] = h.li[i];
    }
  }
};

// NACK helper (for LTE and NR)
struct rlc_status_nack_t {
  uint32_t nack_sn;
  bool     has_so;
  uint16_t so_start;
  uint16_t so_end;

  rlc_status_nack_t()
  {
    has_so   = false;
    nack_sn  = 0;
    so_start = 0;
    so_end   = 0;
  }
};

// STATUS PDU
struct rlc_status_pdu_t {
  uint16_t          ack_sn; // SN of the next not received RLC Data PDU
  uint32_t          N_nack;
  rlc_status_nack_t nacks[RLC_AM_WINDOW_SIZE];

  rlc_status_pdu_t()
  {
    N_nack = 0;
    ack_sn = 0;
  }
};

/** RLC AM NR structs */

///< AM NR PDU header
typedef struct {
  rlc_dc_field_t      dc;      ///< Data/Control (D/C) field
  uint8_t             p;       ///< Polling bit
  rlc_nr_si_field_t   si;      ///< Segmentation info
  rlc_am_nr_sn_size_t sn_size; ///< Sequence number size (12 or 18 bits)
  uint32_t            sn;      ///< Sequence number
  uint16_t            so;      ///< Sequence offset
} rlc_am_nr_pdu_header_t;

///< AM NR Status PDU header (perhaps merge with LTE version)
typedef struct {
  rlc_am_nr_control_pdu_type_t cpt;
  uint32_t                     ack_sn; ///< SN of the next not received RLC Data PDU
  uint16_t                     N_nack; ///< number of NACKs
  uint8_t           nack_range;        ///< number of consecutively lost RLC SDUs starting from and including NACK_SN
  rlc_status_nack_t nacks[RLC_AM_WINDOW_SIZE];
} rlc_am_nr_status_pdu_t;

/****************************************************************************
 * RLC Common interface
 * Common interface for all RLC entities
 ***************************************************************************/
class rlc_common
{
public:
  // Size of the Uplink buffer in number of PDUs
  const static int RLC_BUFFER_NOF_PDU = 128;

  virtual ~rlc_common()                            = default;
  virtual bool configure(const rlc_config_t& cnfg) = 0;
  virtual void stop()                              = 0;
  virtual void reestablish()                       = 0;
  virtual void empty_queue()                       = 0;

  bool suspend()
  {
    if (suspended) {
      return false;
    }
    suspended = true;
    return true;
  }

  // Pops all PDUs from queue and calls write_pdu() method for the bearer type
  bool resume()
  {
    if (!suspended) {
      return false;
    }
    pdu_t p;
    // Do not block
    while (rx_pdu_resume_queue.try_pop(&p)) {
      write_pdu(p.payload, p.nof_bytes);
      free(p.payload);
    }

    unique_byte_buffer_t s;
    while (tx_sdu_resume_queue.try_pop(&s)) {
      write_sdu(std::move(s), false);
    }
    suspended = false;
    return true;
  }

  void write_pdu_s(uint8_t* payload, uint32_t nof_bytes)
  {
    if (suspended) {
      queue_rx_pdu(payload, nof_bytes);
    } else {
      write_pdu(payload, nof_bytes);
    }
  }

  void write_sdu_s(unique_byte_buffer_t sdu, bool blocking)
  {
    if (suspended) {
      queue_tx_sdu(std::move(sdu));
    } else {
      write_sdu(std::move(sdu), blocking);
    }
  }

  virtual rlc_mode_t get_mode()   = 0;
  virtual uint32_t   get_bearer() = 0;

  virtual rlc_bearer_metrics_t get_metrics()   = 0;
  virtual void                 reset_metrics() = 0;

  // PDCP interface
  virtual void write_sdu(unique_byte_buffer_t sdu, bool blocking) = 0;
  virtual void discard_sdu(uint32_t discard_sn)                   = 0;

  // MAC interface
  virtual bool     has_data() = 0;
  bool             is_suspended() { return suspended; };
  virtual uint32_t get_buffer_state()                              = 0;
  virtual int      read_pdu(uint8_t* payload, uint32_t nof_bytes)  = 0;
  virtual void     write_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;

private:
  bool suspended = false;

  // Enqueues the Rx PDU in the resume queue
  void queue_rx_pdu(uint8_t* payload, uint32_t nof_bytes)
  {
    pdu_t p     = {};
    p.nof_bytes = nof_bytes;
    p.payload   = (uint8_t*)malloc(nof_bytes);
    memcpy(p.payload, payload, nof_bytes);

    // Do not block ever
    if (!rx_pdu_resume_queue.try_push(p)) {
      srslte::logmap::get("RLC ")->warning("Dropping SDUs while bearer suspended.\n");
      return;
    }
  }

  // Enqueues the Tx SDU in the resume queue
  void queue_tx_sdu(unique_byte_buffer_t sdu)
  {
    // Do not block ever
    if (!tx_sdu_resume_queue.try_push(std::move(sdu)).first) {
      srslte::logmap::get("RLC ")->warning("Dropping SDUs while bearer suspended.\n");
      return;
    }
  }

  typedef struct {
    uint8_t* payload;
    uint32_t nof_bytes;
  } pdu_t;

  block_queue<pdu_t>                rx_pdu_resume_queue;
  block_queue<unique_byte_buffer_t> tx_sdu_resume_queue{256};
};

} // namespace srslte
#endif // SRSLTE_RLC_COMMON_H
