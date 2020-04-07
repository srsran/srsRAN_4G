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
 *  File:         cqi.h
 *
 *  Description:  Channel quality indicator message packing.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.2.2.6, 5.2.3.3
 *****************************************************************************/

#ifndef SRSLTE_CQI_H
#define SRSLTE_CQI_H

#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

#define SRSLTE_CQI_MAX_BITS 64
#define SRSLTE_DIF_CQI_MAX_BITS 3
#define SRSLTE_PMI_MAX_BITS 4
#define SRSLTE_CQI_STR_MAX_CHAR 64

typedef enum {
  SRSLTE_CQI_MODE_10,
  SRSLTE_CQI_MODE_11,
  SRSLTE_CQI_MODE_12,
  SRSLTE_CQI_MODE_20,
  SRSLTE_CQI_MODE_21,
  SRSLTE_CQI_MODE_22,
  SRSLTE_CQI_MODE_30,
  SRSLTE_CQI_MODE_31,
  SRSLTE_CQI_MODE_NA,
} srslte_cqi_report_mode_t;

typedef struct {
  bool                     periodic_configured;
  bool                     aperiodic_configured;
  uint16_t                 pmi_idx;
  uint32_t                 ri_idx;
  bool                     ri_idx_present;
  bool                     format_is_subband;
  uint32_t                 subband_size;
  srslte_cqi_report_mode_t periodic_mode;
  srslte_cqi_report_mode_t aperiodic_mode;
} srslte_cqi_report_cfg_t;

/* Table 5.2.2.6.2-1: Fields for channel quality information feedback for higher layer configured subband
   CQI reports  (transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
   transmission mode 8 configured without PMI/RI reporting). */

/* Table 5.2.2.6.2-2: Fields for channel quality information (CQI) feedback for higher layer configured subband CQI
   reports (transmission mode 4, transmission mode 5 and transmission mode 6). */

typedef struct SRSLTE_API {
  uint8_t  wideband_cqi_cw0;     // 4-bit width
  uint32_t subband_diff_cqi_cw0; // 2N-bit width
  uint8_t  wideband_cqi_cw1;     // if RI > 1 then 4-bit width otherwise 0-bit width
  uint32_t subband_diff_cqi_cw1; // if RI > 1 then 2N-bit width otherwise 0-bit width
  uint32_t pmi;                  // if RI > 1 then 2-bit width otherwise 1-bit width
} srslte_cqi_hl_subband_t;

/* Table 5.2.2.6.3-1: Fields for channel quality information feedback for UE selected subband CQI
reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct SRSLTE_API {
  uint8_t  wideband_cqi;     // 4-bit width
  uint8_t  subband_diff_cqi; // 2-bit width
  uint32_t position_subband; // L-bit width
} srslte_cqi_ue_subband_t;

/* Table 5.2.3.3.1-1: Fields for channel quality information feedback for wideband CQI reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting).
This is for PUCCH Format 2 reports
*/

/* Table 5.2.3.3.1-2: UCI fields for channel quality and precoding information (CQI/PMI) feedback for
wideband reports (transmission mode 4, transmission mode 5 and transmission mode 6)
This is for PUCCH Format 2 reports
*/

typedef struct SRSLTE_API {
  uint8_t wideband_cqi;     // 4-bit width
  uint8_t spatial_diff_cqi; // If Rank==1 then it is 0-bit width otherwise it is 3-bit width
  uint8_t pmi;
} srslte_cqi_format2_wideband_t;

typedef struct SRSLTE_API {
  uint8_t subband_cqi;   // 4-bit width
  uint8_t subband_label; // 1- or 2-bit width
} srslte_cqi_format2_subband_t;

typedef enum {
  SRSLTE_CQI_TYPE_WIDEBAND = 0,
  SRSLTE_CQI_TYPE_SUBBAND,
  SRSLTE_CQI_TYPE_SUBBAND_UE,
  SRSLTE_CQI_TYPE_SUBBAND_HL
} srslte_cqi_type_t;

typedef struct SRSLTE_API {
  bool              data_enable;
  bool              pmi_present;
  bool              four_antenna_ports;   ///< If cell has 4 antenna ports then true otherwise false
  bool              rank_is_not_one;      ///< If rank > 1 then true otherwise false
  bool              subband_label_2_bits; ///< false, label=1-bit, true label=2-ack_value
  uint32_t          scell_index;          ///< Indicates the cell/carrier the measurement belongs, use 0 for PCell
  uint32_t          L;
  uint32_t          N;
  srslte_cqi_type_t type;
  uint32_t          ri_len;
} srslte_cqi_cfg_t;

typedef struct {
  union {
    srslte_cqi_format2_wideband_t wideband;
    srslte_cqi_format2_subband_t  subband;
    srslte_cqi_ue_subband_t       subband_ue;
    srslte_cqi_hl_subband_t       subband_hl;
  };
  bool data_crc;
} srslte_cqi_value_t;

SRSLTE_API int srslte_cqi_size(srslte_cqi_cfg_t* cfg);

SRSLTE_API int srslte_cqi_value_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_value_t* value, uint8_t* buff);

SRSLTE_API int
srslte_cqi_value_unpack(srslte_cqi_cfg_t* cfg, uint8_t buff[SRSLTE_CQI_MAX_BITS], srslte_cqi_value_t* value);

SRSLTE_API int
srslte_cqi_value_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_value_t* value, char* buff, uint32_t buff_len);

SRSLTE_API bool
srslte_cqi_periodic_send(const srslte_cqi_report_cfg_t* periodic_cfg, uint32_t tti, srslte_frame_type_t frame_type);

SRSLTE_API bool
srslte_cqi_periodic_ri_send(const srslte_cqi_report_cfg_t* periodic_cfg, uint32_t tti, srslte_frame_type_t frame_type);

SRSLTE_API int srslte_cqi_hl_get_no_subbands(int nof_prb);

SRSLTE_API uint8_t srslte_cqi_from_snr(float snr);

SRSLTE_API float srslte_cqi_to_coderate(uint32_t cqi);

#endif // SRSLTE_CQI_H
