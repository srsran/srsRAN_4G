/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         cqi.h
 *
 *  Description:  Channel quality indicator message packing.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.2.2.6, 5.2.3.3
 *****************************************************************************/

#ifndef SRSRAN_CQI_H
#define SRSRAN_CQI_H

#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

#define SRSRAN_CQI_MAX_BITS 64
#define SRSRAN_DIF_CQI_MAX_BITS 3
#define SRSRAN_PMI_MAX_BITS 4
#define SRSRAN_CQI_STR_MAX_CHAR 64

typedef enum {
  SRSRAN_CQI_MODE_10,
  SRSRAN_CQI_MODE_11,
  SRSRAN_CQI_MODE_12,
  SRSRAN_CQI_MODE_20,
  SRSRAN_CQI_MODE_21,
  SRSRAN_CQI_MODE_22,
  SRSRAN_CQI_MODE_30,
  SRSRAN_CQI_MODE_31,
  SRSRAN_CQI_MODE_NA,
} srsran_cqi_report_mode_t;

typedef struct {
  bool                     periodic_configured;
  bool                     aperiodic_configured;
  uint16_t                 pmi_idx;
  uint32_t                 ri_idx;
  bool                     ri_idx_present;
  bool                     format_is_subband;
  uint8_t                  subband_wideband_ratio; ///< K value in TS 36.331. 0 for wideband reporting, (1..4) otherwise
  srsran_cqi_report_mode_t periodic_mode;
  srsran_cqi_report_mode_t aperiodic_mode;
} srsran_cqi_report_cfg_t;

/* Table 5.2.2.6.2-1: Fields for channel quality information feedback for higher layer configured subband
   CQI reports  (transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
   transmission mode 8 configured without PMI/RI reporting). */

/* Table 5.2.2.6.2-2: Fields for channel quality information (CQI) feedback for higher layer configured subband CQI
   reports (transmission mode 4, transmission mode 5 and transmission mode 6). */

typedef struct SRSRAN_API {
  uint8_t  wideband_cqi_cw0;     // 4-bit width
  uint32_t subband_diff_cqi_cw0; // 2N-bit width
  uint8_t  wideband_cqi_cw1;     // if RI > 1 then 4-bit width otherwise 0-bit width
  uint32_t subband_diff_cqi_cw1; // if RI > 1 then 2N-bit width otherwise 0-bit width
  uint32_t pmi;                  // if RI > 1 then 2-bit width otherwise 1-bit width
} srsran_cqi_hl_subband_t;

/* Table 5.2.2.6.3-1: Fields for channel quality information feedback for UE selected subband CQI
reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting). */
typedef struct SRSRAN_API {
  uint8_t  wideband_cqi;     // 4-bit width
  uint8_t  subband_diff_cqi; // 2-bit width
  uint32_t position_subband; // L-bit width
} srsran_cqi_ue_diff_subband_t;

/* Table 5.2.3.3.1-1: Fields for channel quality information feedback for wideband CQI reports
(transmission mode 1, transmission mode 2, transmission mode 3, transmission mode 7 and
transmission mode 8 configured without PMI/RI reporting).
This is for PUCCH Format 2 reports
*/

/* Table 5.2.3.3.1-2: UCI fields for channel quality and precoding information (CQI/PMI) feedback for
wideband reports (transmission mode 4, transmission mode 5 and transmission mode 6)
This is for PUCCH Format 2 reports
*/

typedef struct SRSRAN_API {
  uint8_t wideband_cqi;     // 4-bit width
  uint8_t spatial_diff_cqi; // If Rank==1 then it is 0-bit width otherwise it is 3-bit width
  uint8_t pmi;
} srsran_cqi_format2_wideband_t;

typedef struct SRSRAN_API {
  uint8_t subband_cqi;   // 4-bit width
  uint8_t subband_label; // 1- or 2-bit width
} srsran_cqi_ue_subband_t;

typedef enum {
  SRSRAN_CQI_TYPE_WIDEBAND = 0,
  SRSRAN_CQI_TYPE_SUBBAND_UE,
  SRSRAN_CQI_TYPE_SUBBAND_UE_DIFF,
  SRSRAN_CQI_TYPE_SUBBAND_HL
} srsran_cqi_type_t;

typedef struct SRSRAN_API {
  bool              data_enable;
  bool              pmi_present;
  bool              four_antenna_ports;   ///< If cell has 4 antenna ports then true otherwise false
  bool              rank_is_not_one;      ///< If rank > 1 then true otherwise false
  bool              subband_label_2_bits; ///< false, label=1-bit, true label=2-ack_value
  uint32_t          scell_index;          ///< Indicates the cell/carrier the measurement belongs, use 0 for PCell
  uint32_t          L;
  uint32_t          N;
  uint32_t          sb_idx;
  srsran_cqi_type_t type;
  uint32_t          ri_len;
} srsran_cqi_cfg_t;

typedef struct {
  union {
    srsran_cqi_format2_wideband_t wideband;
    srsran_cqi_ue_subband_t       subband_ue;
    srsran_cqi_ue_diff_subband_t  subband_ue_diff;
    srsran_cqi_hl_subband_t       subband_hl;
  };
  bool data_crc;
} srsran_cqi_value_t;

SRSRAN_API int srsran_cqi_size(srsran_cqi_cfg_t* cfg);

SRSRAN_API int
srsran_cqi_value_pack(srsran_cqi_cfg_t* cfg, srsran_cqi_value_t* value, uint8_t buff[SRSRAN_CQI_MAX_BITS]);

SRSRAN_API int
srsran_cqi_value_unpack(srsran_cqi_cfg_t* cfg, uint8_t buff[SRSRAN_CQI_MAX_BITS], srsran_cqi_value_t* value);

SRSRAN_API int
srsran_cqi_value_tostring(srsran_cqi_cfg_t* cfg, srsran_cqi_value_t* value, char* buff, uint32_t buff_len);

SRSRAN_API bool
srsran_cqi_periodic_send(const srsran_cqi_report_cfg_t* periodic_cfg, uint32_t tti, srsran_frame_type_t frame_type);

SRSRAN_API bool srsran_cqi_periodic_is_subband(const srsran_cqi_report_cfg_t* cfg,
                                               uint32_t                       tti,
                                               uint32_t                       nof_prb,
                                               srsran_frame_type_t            frame_type);

SRSRAN_API bool
srsran_cqi_periodic_ri_send(const srsran_cqi_report_cfg_t* periodic_cfg, uint32_t tti, srsran_frame_type_t frame_type);

SRSRAN_API uint32_t srsran_cqi_periodic_sb_bw_part_idx(const srsran_cqi_report_cfg_t* cfg,
                                                       uint32_t                       tti,
                                                       uint32_t                       nof_prb,
                                                       srsran_frame_type_t            frame_type);

SRSRAN_API int srsran_cqi_hl_get_no_subbands(int nof_prb);

/**
 * @brief Returns the number of bits to index a bandwidth part (L)
 *
 * @remark Implemented according to TS 38.213 section 7.2.2 Periodic CSI Reporting using PUCCH, paragraph that refers to
 * `L-bit label indexed in the order of increasing frequency, where L = ceil(log2(nof_prb/k/J))`
 *
 */
SRSRAN_API int srsran_cqi_hl_get_L(int nof_prb);

SRSRAN_API uint32_t srsran_cqi_get_sb_idx(uint32_t                       tti,
                                          uint32_t                       subband_label,
                                          const srsran_cqi_report_cfg_t* cqi_report_cfg,
                                          const srsran_cell_t*           cell);

SRSRAN_API uint8_t srsran_cqi_from_snr(float snr);

SRSRAN_API float srsran_cqi_to_coderate(uint32_t cqi, bool use_alt_table);

#endif // SRSRAN_CQI_H
