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
 *  File:         dci.h
 *
 *  Description:  Downlink control information (DCI).
 *                Packing/Unpacking functions to convert between bit streams
 *                and packed DCI UL/DL grants defined in ra.h
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.3.3
 *****************************************************************************/

#ifndef SRSLTE_DCI_H
#define SRSLTE_DCI_H

#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/ra.h"

#define SRSLTE_DCI_MAX_BITS 128
#define SRSLTE_RAR_GRANT_LEN 20

#define SRSLTE_DCI_IS_TB_EN(tb) (!(tb.mcs_idx == 0 && tb.rv == 1))
#define SRSLTE_DCI_TB_DISABLE(tb)                                                                                      \
  do {                                                                                                                 \
    tb.mcs_idx = 0;                                                                                                    \
    tb.rv      = 1;                                                                                                    \
  } while (0)
#define SRSLTE_DCI_HEXDEBUG 0

typedef struct {
  bool multiple_csi_request_enabled;
  bool cif_enabled;
  bool cif_present;
  bool srs_request_enabled;
  bool ra_format_enabled;
  bool is_not_ue_ss;
} srslte_dci_cfg_t;

typedef struct SRSLTE_API {
  uint32_t L;    // Aggregation level
  uint32_t ncce; // Position of first CCE of the dci
} srslte_dci_location_t;

typedef struct SRSLTE_API {
  uint8_t               payload[SRSLTE_DCI_MAX_BITS];
  uint32_t              nof_bits;
  srslte_dci_location_t location;
  srslte_dci_format_t   format;
  uint16_t              rnti;
} srslte_dci_msg_t;

typedef struct SRSLTE_API {
  uint32_t mcs_idx;
  int      rv;
  bool     ndi;
  uint32_t cw_idx;
} srslte_dci_tb_t;

typedef struct SRSLTE_API {

  uint16_t              rnti;
  srslte_dci_format_t   format;
  srslte_dci_location_t location;
  uint32_t              ue_cc_idx;

  // Resource Allocation
  srslte_ra_type_t alloc_type;
  union {
    srslte_ra_type0_t type0_alloc;
    srslte_ra_type1_t type1_alloc;
    srslte_ra_type2_t type2_alloc;
  };

  // Codeword information
  srslte_dci_tb_t tb[SRSLTE_MAX_CODEWORDS];
  bool            tb_cw_swap;
  uint32_t        pinfo;

  // Power control
  bool    pconf;
  bool    power_offset;
  uint8_t tpc_pucch;

  // RA order
  bool     is_ra_order;
  uint32_t ra_preamble;
  uint32_t ra_mask_idx;

  // Release 10
  uint32_t cif;
  bool     cif_present;
  bool     srs_request;
  bool     srs_request_present;

  // Other parameters
  uint32_t pid;
  uint32_t dai;
  bool     is_tdd;
  bool     is_dwpts;
  bool     sram_id;

  // For debugging purposes
#if SRSLTE_DCI_HEXDEBUG
  uint32_t nof_bits;
  char     hex_str[SRSLTE_DCI_MAX_BITS];
#endif
} srslte_dci_dl_t;

/** Unpacked DCI Format0 message */
typedef struct SRSLTE_API {

  uint16_t              rnti;
  srslte_dci_format_t   format;
  srslte_dci_location_t location;
  uint32_t              ue_cc_idx;

  srslte_ra_type2_t type2_alloc;
  /* 36.213 Table 8.4-2: SRSLTE_RA_PUSCH_HOP_HALF is 0 for < 10 Mhz and 10 for > 10 Mhz.
   * SRSLTE_RA_PUSCH_HOP_QUART is 00 for > 10 Mhz and SRSLTE_RA_PUSCH_HOP_QUART_NEG is 01 for > 10 Mhz.
   */
  enum {
    SRSLTE_RA_PUSCH_HOP_DISABLED  = -1,
    SRSLTE_RA_PUSCH_HOP_QUART     = 0,
    SRSLTE_RA_PUSCH_HOP_QUART_NEG = 1,
    SRSLTE_RA_PUSCH_HOP_HALF      = 2,
    SRSLTE_RA_PUSCH_HOP_TYPE2     = 3
  } freq_hop_fl;

  // Codeword information
  srslte_dci_tb_t tb;
  uint32_t        n_dmrs;
  bool            cqi_request;

  // TDD parametres
  uint32_t dai;
  uint32_t ul_idx;
  bool     is_tdd;

  // Power control
  uint8_t tpc_pusch;

  // Release 10
  uint32_t         cif;
  bool             cif_present;
  uint8_t          multiple_csi_request;
  bool             multiple_csi_request_present;
  bool             srs_request;
  bool             srs_request_present;
  srslte_ra_type_t ra_type;
  bool             ra_type_present;

  // For debugging purposes
#if SRSLTE_DCI_HEXDEBUG
  uint32_t nof_bits;
  char     hex_str[SRSLTE_DCI_MAX_BITS];
#endif /* SRSLTE_DCI_HEXDEBUG */

} srslte_dci_ul_t;

typedef struct SRSLTE_API {
  uint32_t rba;
  uint32_t trunc_mcs;
  uint32_t tpc_pusch;
  bool     ul_delay;
  bool     cqi_request;
  bool     hopping_flag;
} srslte_dci_rar_grant_t;

SRSLTE_API void srslte_dci_rar_unpack(uint8_t payload[SRSLTE_RAR_GRANT_LEN], srslte_dci_rar_grant_t* rar);

SRSLTE_API void srslte_dci_rar_pack(srslte_dci_rar_grant_t* rar, uint8_t payload[SRSLTE_RAR_GRANT_LEN]);

SRSLTE_API int srslte_dci_rar_to_ul_dci(srslte_cell_t* cell, srslte_dci_rar_grant_t* rar, srslte_dci_ul_t* dci_ul);

SRSLTE_API int srslte_dci_msg_pack_pusch(srslte_cell_t*      cell,
                                         srslte_dl_sf_cfg_t* sf,
                                         srslte_dci_cfg_t*   cfg,
                                         srslte_dci_ul_t*    dci,
                                         srslte_dci_msg_t*   msg);

SRSLTE_API int srslte_dci_msg_unpack_pusch(srslte_cell_t*      cell,
                                           srslte_dl_sf_cfg_t* sf,
                                           srslte_dci_cfg_t*   cfg,
                                           srslte_dci_msg_t*   msg,
                                           srslte_dci_ul_t*    dci);

SRSLTE_API int srslte_dci_msg_pack_pdsch(srslte_cell_t*      cell,
                                         srslte_dl_sf_cfg_t* sf,
                                         srslte_dci_cfg_t*   cfg,
                                         srslte_dci_dl_t*    dci,
                                         srslte_dci_msg_t*   msg);

SRSLTE_API int srslte_dci_msg_unpack_pdsch(srslte_cell_t*      cell,
                                           srslte_dl_sf_cfg_t* sf,
                                           srslte_dci_cfg_t*   cfg,
                                           srslte_dci_msg_t*   msg,
                                           srslte_dci_dl_t*    dci);

SRSLTE_API uint32_t srslte_dci_format_sizeof(const srslte_cell_t* cell,
                                             srslte_dl_sf_cfg_t*  sf,
                                             srslte_dci_cfg_t*    cfg,
                                             srslte_dci_format_t  format);

SRSLTE_API void srslte_dci_dl_fprint(FILE* f, srslte_dci_dl_t* dci, uint32_t nof_prb);

SRSLTE_API uint32_t srslte_dci_dl_info(const srslte_dci_dl_t* dci_dl, char* str, uint32_t str_len);

SRSLTE_API uint32_t srslte_dci_ul_info(srslte_dci_ul_t* dci_ul, char* info_str, uint32_t len);

SRSLTE_API srslte_dci_format_t srslte_dci_format_from_string(char* str);

SRSLTE_API char* srslte_dci_format_string(srslte_dci_format_t format);

SRSLTE_API char* srslte_dci_format_string_short(srslte_dci_format_t format);

SRSLTE_API bool
srslte_location_find(const srslte_dci_location_t* locations, uint32_t nof_locations, srslte_dci_location_t x);

SRSLTE_API bool
srslte_location_find_ncce(const srslte_dci_location_t* locations, uint32_t nof_locations, uint32_t ncce);

SRSLTE_API int srslte_dci_location_set(srslte_dci_location_t* c, uint32_t L, uint32_t nCCE);

SRSLTE_API bool srslte_dci_location_isvalid(srslte_dci_location_t* c);

SRSLTE_API void srslte_dci_cfg_set_common_ss(srslte_dci_cfg_t* cfg);

SRSLTE_API uint32_t srslte_dci_format_max_tb(srslte_dci_format_t format);

#endif // DCI_
