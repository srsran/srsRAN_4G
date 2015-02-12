/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef CQI_
#define CQI_

#include <stdint.h>

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"

/**
 * CQI message generation according to 36.212 Sections 5.2.2.6 and 5.2.3.3
 *
 */


/* Table 5.2.2.6.2-1: Fields for channel quality information feedback for higher layer configured subband
CQI reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct LIBLTE_API {
  uint8_t wideband_cqi; // 4-bit width
  uint32_t subband_diff_cqi; // 2N-bit width
} cqi_hl_subband_t;

/* Table 5.2.2.6.3-1: Fields for channel quality information feedback for UE selected subband CQI
reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct LIBLTE_API {
  uint8_t wideband_cqi; // 4-bit width
  uint8_t subband_diff_cqi; // 2-bit width
  uint32_t position_subband; // L-bit width
} cqi_ue_subband_t;


LIBLTE_API void cqi_hl_subband_pack(cqi_hl_subband_t *msg, 
                                    uint32_t N, 
                                    uint8_t *buff, 
                                    uint32_t buff_len);

LIBLTE_API void cqi_ue_subband_pack(cqi_ue_subband_t *msg, 
                                    uint32_t L, 
                                    uint8_t *buff, 
                                    uint32_t buff_len);

#endif // CQI_
