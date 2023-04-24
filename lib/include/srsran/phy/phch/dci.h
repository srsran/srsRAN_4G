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
 *  File:         dci.h
 *
 *  Description:  Downlink control information (DCI).
 *                Packing/Unpacking functions to convert between bit streams
 *                and packed DCI UL/DL grants defined in ra.h
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.3.3
 *****************************************************************************/

#ifndef SRSRAN_DCI_H
#define SRSRAN_DCI_H

#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/ra.h"

#define SRSRAN_DCI_MAX_BITS 128
#define SRSRAN_RAR_GRANT_LEN 20

#define SRSRAN_DCI_IS_TB_EN(tb) (!(tb.mcs_idx == 0 && tb.rv == 1))
#define SRSRAN_DCI_TB_DISABLE(tb)                                                                                      \
  do {                                                                                                                 \
    tb.mcs_idx = 0;                                                                                                    \
    tb.rv      = 1;                                                                                                    \
  } while (0)
#define SRSRAN_DCI_HEXDEBUG 0

typedef struct {
  bool multiple_csi_request_enabled;
  bool cif_enabled;
  bool cif_present;
  bool srs_request_enabled;
  bool ra_format_enabled;
  bool is_not_ue_ss;
} srsran_dci_cfg_t;

typedef struct SRSRAN_API {
  uint32_t L;    // Aggregation level (logarithmic)
  uint32_t ncce; // Position of first CCE of the dci
} srsran_dci_location_t;

typedef struct SRSRAN_API {
  uint8_t               payload[SRSRAN_DCI_MAX_BITS];
  uint32_t              nof_bits;
  srsran_dci_location_t location;
  srsran_dci_format_t   format;
  uint16_t              rnti;
} srsran_dci_msg_t;

typedef struct SRSRAN_API {
  uint32_t mcs_idx;
  int      rv;
  bool     ndi;
  uint32_t cw_idx;
} srsran_dci_tb_t;

typedef struct SRSRAN_API {
  uint16_t              rnti;
  srsran_dci_format_t   format;
  srsran_dci_location_t location;
  uint32_t              ue_cc_idx;

  // Resource Allocation
  srsran_ra_type_t alloc_type;
  union {
    srsran_ra_type0_t type0_alloc;
    srsran_ra_type1_t type1_alloc;
    srsran_ra_type2_t type2_alloc;
  };

  // Codeword information
  srsran_dci_tb_t tb[SRSRAN_MAX_CODEWORDS];
  bool            tb_cw_swap;
  uint32_t        pinfo;

  // Power control
  bool    pconf;
  bool    power_offset;
  uint8_t tpc_pucch;

  // PDCCH order
  bool     is_pdcch_order;
  uint32_t preamble_idx;
  uint32_t prach_mask_idx;

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
#if SRSRAN_DCI_HEXDEBUG
  uint32_t nof_bits;
  char     hex_str[SRSRAN_DCI_MAX_BITS];
#endif
} srsran_dci_dl_t;

/** Unpacked DCI Format0 message */
typedef struct SRSRAN_API {
  uint16_t              rnti;
  srsran_dci_format_t   format;
  srsran_dci_location_t location;
  uint32_t              ue_cc_idx;

  srsran_ra_type2_t type2_alloc;
  /* 36.213 Table 8.4-2: SRSRAN_RA_PUSCH_HOP_HALF is 0 for < 10 Mhz and 10 for > 10 Mhz.
   * SRSRAN_RA_PUSCH_HOP_QUART is 00 for > 10 Mhz and SRSRAN_RA_PUSCH_HOP_QUART_NEG is 01 for > 10 Mhz.
   */
  enum {
    SRSRAN_RA_PUSCH_HOP_DISABLED  = -1,
    SRSRAN_RA_PUSCH_HOP_QUART     = 0,
    SRSRAN_RA_PUSCH_HOP_QUART_NEG = 1,
    SRSRAN_RA_PUSCH_HOP_HALF      = 2,
    SRSRAN_RA_PUSCH_HOP_TYPE2     = 3
  } freq_hop_fl;

  // Codeword information
  srsran_dci_tb_t tb;
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
  srsran_ra_type_t ra_type;
  bool             ra_type_present;

  // For debugging purposes
#if SRSRAN_DCI_HEXDEBUG
  uint32_t nof_bits;
  char     hex_str[SRSRAN_DCI_MAX_BITS];
#endif /* SRSRAN_DCI_HEXDEBUG */

} srsran_dci_ul_t;

typedef struct SRSRAN_API {
  uint32_t rba;
  uint32_t trunc_mcs;
  uint32_t tpc_pusch;
  bool     ul_delay;
  bool     cqi_request;
  bool     hopping_flag;
} srsran_dci_rar_grant_t;

SRSRAN_API void srsran_dci_rar_unpack(uint8_t payload[SRSRAN_RAR_GRANT_LEN], srsran_dci_rar_grant_t* rar);

SRSRAN_API void srsran_dci_rar_pack(srsran_dci_rar_grant_t* rar, uint8_t payload[SRSRAN_RAR_GRANT_LEN]);

SRSRAN_API int srsran_dci_rar_to_ul_dci(srsran_cell_t* cell, srsran_dci_rar_grant_t* rar, srsran_dci_ul_t* dci_ul);

SRSRAN_API int srsran_dci_msg_pack_pusch(srsran_cell_t*      cell,
                                         srsran_dl_sf_cfg_t* sf,
                                         srsran_dci_cfg_t*   cfg,
                                         srsran_dci_ul_t*    dci,
                                         srsran_dci_msg_t*   msg);

SRSRAN_API int srsran_dci_msg_unpack_pusch(srsran_cell_t*      cell,
                                           srsran_dl_sf_cfg_t* sf,
                                           srsran_dci_cfg_t*   cfg,
                                           srsran_dci_msg_t*   msg,
                                           srsran_dci_ul_t*    dci);

SRSRAN_API int srsran_dci_msg_pack_pdsch(srsran_cell_t*      cell,
                                         srsran_dl_sf_cfg_t* sf,
                                         srsran_dci_cfg_t*   cfg,
                                         srsran_dci_dl_t*    dci,
                                         srsran_dci_msg_t*   msg);

SRSRAN_API int srsran_dci_msg_unpack_pdsch(srsran_cell_t*      cell,
                                           srsran_dl_sf_cfg_t* sf,
                                           srsran_dci_cfg_t*   cfg,
                                           srsran_dci_msg_t*   msg,
                                           srsran_dci_dl_t*    dci);

SRSRAN_API uint32_t srsran_dci_format_sizeof(const srsran_cell_t* cell,
                                             srsran_dl_sf_cfg_t*  sf,
                                             srsran_dci_cfg_t*    cfg,
                                             srsran_dci_format_t  format);

SRSRAN_API void srsran_dci_dl_fprint(FILE* f, srsran_dci_dl_t* dci, uint32_t nof_prb);

SRSRAN_API uint32_t srsran_dci_dl_info(const srsran_dci_dl_t* dci_dl, char* str, uint32_t str_len);

SRSRAN_API uint32_t srsran_dci_ul_info(srsran_dci_ul_t* dci_ul, char* info_str, uint32_t len);

SRSRAN_API srsran_dci_format_t srsran_dci_format_from_string(char* str);

SRSRAN_API char* srsran_dci_format_string(srsran_dci_format_t format);

SRSRAN_API char* srsran_dci_format_string_short(srsran_dci_format_t format);

SRSRAN_API bool
srsran_location_find(const srsran_dci_location_t* locations, uint32_t nof_locations, srsran_dci_location_t x);

SRSRAN_API bool srsran_location_find_location(const srsran_dci_location_t* locations,
                                              uint32_t                     nof_locations,
                                              const srsran_dci_location_t* location);

SRSRAN_API int srsran_dci_location_set(srsran_dci_location_t* c, uint32_t L, uint32_t nCCE);

SRSRAN_API bool srsran_dci_location_isvalid(srsran_dci_location_t* c);

SRSRAN_API void srsran_dci_cfg_set_common_ss(srsran_dci_cfg_t* cfg);

SRSRAN_API uint32_t srsran_dci_format_max_tb(srsran_dci_format_t format);

#endif // DCI_
