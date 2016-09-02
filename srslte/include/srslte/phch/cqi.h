/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
 *  File:         cqi.h
 *
 *  Description:  Channel quality indicator message packing.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.2.2.6, 5.2.3.3
 *****************************************************************************/

#ifndef CQI_
#define CQI_

#include <stdint.h>

#include "srslte/config.h"
#include "srslte/common/phy_common.h"

#define SRSLTE_CQI_MAX_BITS 20

typedef struct {
  bool     configured; 
  uint32_t pmi_idx; 
  bool     simul_cqi_ack; 
  bool     format_is_subband; 
  uint32_t subband_size; 
} srslte_cqi_periodic_cfg_t; 
  
/* Table 5.2.2.6.2-1: Fields for channel quality information feedback for higher layer configured subband
CQI reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct SRSLTE_API {
  uint8_t  wideband_cqi; // 4-bit width
  uint32_t subband_diff_cqi; // 2N-bit width
  uint32_t N; 
} srslte_cqi_hl_subband_t;

/* Table 5.2.2.6.3-1: Fields for channel quality information feedback for UE selected subband CQI
reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct SRSLTE_API {
  uint8_t  wideband_cqi; // 4-bit width
  uint8_t  subband_diff_cqi; // 2-bit width
  uint32_t position_subband; // L-bit width
  uint32_t L;
} srslte_cqi_ue_subband_t;

/* Table 5.2.3.3.1-1: Fields for channel quality information feedback for wideband CQI reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). 
This is for PUCCH Format 2 reports
*/
typedef struct SRSLTE_API {
  uint8_t  wideband_cqi; // 4-bit width
} srslte_cqi_format2_wideband_t;

typedef struct SRSLTE_API {
  uint8_t  subband_cqi; // 4-bit width
  uint8_t  subband_label; // 1- or 2-bit width
  bool     subband_label_2_bits; // false, label=1-bit, true label=2-bits
} srslte_cqi_format2_subband_t;

typedef enum {
  SRSLTE_CQI_TYPE_WIDEBAND = 0,
  SRSLTE_CQI_TYPE_SUBBAND,
  SRSLTE_CQI_TYPE_SUBBAND_UE,
  SRSLTE_CQI_TYPE_SUBBAND_HL
} srslte_cqi_type_t; 

typedef struct {
  union {
    srslte_cqi_format2_wideband_t wideband;
    srslte_cqi_format2_subband_t  subband; 
    srslte_cqi_ue_subband_t       subband_ue;
    srslte_cqi_hl_subband_t       subband_hl;
  };
  srslte_cqi_type_t type; 
} srslte_cqi_value_t;


SRSLTE_API int srslte_cqi_value_pack(srslte_cqi_value_t *value, 
                                     uint8_t buff[SRSLTE_CQI_MAX_BITS]);

SRSLTE_API int srslte_cqi_hl_subband_pack(srslte_cqi_hl_subband_t *msg, 
                                    uint8_t buff[SRSLTE_CQI_MAX_BITS]);

SRSLTE_API int srslte_cqi_ue_subband_pack(srslte_cqi_ue_subband_t *msg, 
                                    uint8_t buff[SRSLTE_CQI_MAX_BITS]);

SRSLTE_API int srslte_cqi_format2_wideband_pack(srslte_cqi_format2_wideband_t *msg, 
                                          uint8_t buff[SRSLTE_CQI_MAX_BITS]);

SRSLTE_API int srslte_cqi_format2_subband_pack(srslte_cqi_format2_subband_t *msg, 
                                        uint8_t buff[SRSLTE_CQI_MAX_BITS]);

SRSLTE_API bool srslte_cqi_send(uint32_t I_cqi_pmi, 
                                uint32_t tti); 

SRSLTE_API uint8_t srslte_cqi_from_snr(float snr);

SRSLTE_API int srslte_cqi_hl_get_subband_size(int num_prbs);

SRSLTE_API int srslte_cqi_hl_get_no_subbands(int num_prbs);

#endif // CQI_
