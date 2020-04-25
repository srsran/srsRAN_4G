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

#ifndef SRSLTE_SCI_H
#define SRSLTE_SCI_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_sl.h"

#define SRSLTE_SCI_MSG_MAX_LEN 80

#define SRSLTE_SCI_CRC_LEN (16)
#define SRSLTE_SCI_MAX_LEN (45)
#define SRSLTE_SCI_TM34_LEN (32)

typedef enum { SRSLTE_SCI_FORMAT0 = 0, SRSLTE_SCI_FORMAT1 } srslte_sci_format_t;

/**
 * Sidelink Control Information - Sidelink (SCI0 and SCI1).
 *
 * \brief SCI0/1 packing/unpacking functions to convert between bit streams
 *
 * Reference:  3GPP TS 36.212 version 15.6.0 Release 15 Section 5.4.3
 */
typedef struct SRSLTE_API {

  srslte_sl_tm_t tm;
  uint32_t       sci_len;
  uint32_t       nof_prb;

  srslte_sci_format_t format;

  // sizeSubChannel
  uint32_t size_sub_channel;

  // numSubChannel
  uint32_t num_sub_channel;

  ///< SCI Format 0: Resource block assignment and hopping resource allocation
  ///< SCI Format 1: Frequency resource location (0,3,6,7,8 bits)
  uint32_t riv;

  // Modulation and coding scheme (bits);
  uint32_t mcs_idx;

  ///< SCI Format 0
  // Frequency hopping flag (1 bit)
  bool freq_hopping_flag;

  uint32_t hopping_bits;

  // TRP - Time resource pattern (7bits)
  uint32_t trp_idx;

  // Timing advance indication (11 bits)
  uint32_t timing_advance;

  // Group ID (8 bits)
  uint32_t N_sa_id;

  ///< SCI Format 1
  // Priority (3bits)
  uint32_t priority;

  // Resource reservation (4 bits)
  uint32_t resource_reserv;

  // Time gap (4 bits)
  uint32_t time_gap;

  // Retransmission index (1 bit)
  bool retransmission;

  // Transmission format (1 bit)
  // 1 means transmission format including rate-matching and TBS scaling
  // 0 means transmission format including puncturing and no TBS-scaling
  uint32_t transmission_format;

} srslte_sci_t;

SRSLTE_API int
                srslte_sci_init(srslte_sci_t* q, srslte_cell_sl_t cell, srslte_sl_comm_resource_pool_t sl_comm_resource_pool);
SRSLTE_API int  srslte_sci_format0_pack(srslte_sci_t* q, uint8_t* output);
SRSLTE_API int  srslte_sci_format1_pack(srslte_sci_t* q, uint8_t* output);
SRSLTE_API int  srslte_sci_format0_unpack(srslte_sci_t* q, uint8_t* input);
SRSLTE_API int  srslte_sci_format1_unpack(srslte_sci_t* q, uint8_t* input);
SRSLTE_API void srslte_sci_info(const srslte_sci_t* sci, char* str, uint32_t len);
SRSLTE_API void srslte_sci_free(srslte_sci_t* q);

#endif // SRSLTE_SCI_H
