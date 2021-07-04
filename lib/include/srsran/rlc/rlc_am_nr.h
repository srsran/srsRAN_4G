/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_RLC_AM_NR_H
#define SRSRAN_RLC_AM_NR_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsran {

typedef struct {
  rlc_am_nr_pdu_header_t header;
  unique_byte_buffer_t   buf;
} rlc_amd_pdu_nr_t;

///< add class here

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

#endif // SRSRAN_RLC_AM_NR_H
