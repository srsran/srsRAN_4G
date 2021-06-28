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

#ifndef SRSRAN_RLC_AM_NR_PACKING_H
#define SRSRAN_RLC_AM_NR_PACKING_H

#include "srsran/rlc/rlc_am_base.h"

namespace srsran {

///< AM NR PDU header
typedef struct {
  rlc_dc_field_t      dc;      ///< Data/Control (D/C) field
  uint8_t             p;       ///< Polling bit
  rlc_nr_si_field_t   si;      ///< Segmentation info
  rlc_am_nr_sn_size_t sn_size; ///< Sequence number size (12 or 18 bits)
  uint32_t            sn;      ///< Sequence number
  uint16_t            so;      ///< Sequence offset
} rlc_am_nr_pdu_header_t;

struct rlc_amd_pdu_nr_t {
  rlc_am_nr_pdu_header_t header;
  unique_byte_buffer_t   buf;
};

///< AM NR Status PDU header (perhaps merge with LTE version)
typedef struct {
  rlc_am_nr_control_pdu_type_t cpt;
  uint32_t                     ack_sn; ///< SN of the next not received RLC Data PDU
  uint16_t                     N_nack; ///< number of NACKs
  uint8_t           nack_range;        ///< number of consecutively lost RLC SDUs starting from and including NACK_SN
  rlc_status_nack_t nacks[RLC_AM_WINDOW_SIZE];
} rlc_am_nr_status_pdu_t;

/****************************************************************************
 * Header pack/unpack helper functions for NR
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.3
 ***************************************************************************/
uint32_t rlc_am_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header);

uint32_t rlc_am_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header);

uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, byte_buffer_t* pdu);

uint32_t rlc_am_nr_packed_length(const rlc_am_nr_pdu_header_t& header);

uint32_t
rlc_am_nr_read_status_pdu(const byte_buffer_t* pdu, const rlc_am_nr_sn_size_t sn_size, rlc_am_nr_status_pdu_t* status);

uint32_t rlc_am_nr_read_status_pdu(const uint8_t*            payload,
                                   const uint32_t            nof_bytes,
                                   const rlc_am_nr_sn_size_t sn_size,
                                   rlc_am_nr_status_pdu_t*   status);

int32_t rlc_am_nr_write_status_pdu(const rlc_am_nr_status_pdu_t& status_pdu,
                                   const rlc_am_nr_sn_size_t     sn_size,
                                   byte_buffer_t*                pdu);

} // namespace srsran

#endif // SRSRAN_RLC_AM_NR_PACKING_H
