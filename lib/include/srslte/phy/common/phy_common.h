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

/**********************************************************************************************
 *  File:         phy_common.h
 *
 *  Description:  Common parameters and lookup functions for PHY
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10
 *********************************************************************************************/

#ifndef SRSLTE_PHY_COMMON_H
#define SRSLTE_PHY_COMMON_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/config.h"

#define SRSLTE_NOF_SF_X_FRAME 10
#define SRSLTE_NOF_SLOTS_PER_SF 2
#define SRSLTE_NSLOTS_X_FRAME (SRSLTE_NOF_SLOTS_PER_SF * SRSLTE_NOF_SF_X_FRAME)

#define SRSLTE_NSOFT_BITS 250368 // Soft buffer size for Category 1 UE

#define SRSLTE_PC_MAX 23 // Maximum TX power for Category 1 UE (in dBm)

#define SRSLTE_NOF_NID_1 (168)
#define SRSLTE_NOF_NID_2 (3)
#define SRSLTE_NUM_PCI (SRSLTE_NOF_NID_1 * SRSLTE_NOF_NID_2)

#define SRSLTE_MAX_CARRIERS 5 // Maximum number of supported simultaneous carriers
#define SRSLTE_MAX_PORTS 4
#define SRSLTE_MAX_CHANNELS (SRSLTE_MAX_CARRIERS * SRSLTE_MAX_PORTS)
#define SRSLTE_MAX_LAYERS 4
#define SRSLTE_MAX_CODEWORDS 2
#define SRSLTE_MAX_TB SRSLTE_MAX_CODEWORDS

#define SRSLTE_MAX_CODEBLOCKS 32

#define SRSLTE_MAX_CODEBOOKS 4

#define SRSLTE_LTE_CRC24A 0x1864CFB
#define SRSLTE_LTE_CRC24B 0X1800063
#define SRSLTE_LTE_CRC16 0x11021
#define SRSLTE_LTE_CRC8 0x19B

#define SRSLTE_MAX_MBSFN_AREA_IDS 256
#define SRSLTE_PMCH_RV 0

typedef enum { SRSLTE_CP_NORM = 0, SRSLTE_CP_EXT } srslte_cp_t;
typedef enum { SRSLTE_SF_NORM = 0, SRSLTE_SF_MBSFN } srslte_sf_t;

#define SRSLTE_INVALID_RNTI 0x0 // TS 36.321 - Table 7.1-1 RNTI 0x0 isn't a valid DL RNTI
#define SRSLTE_CRNTI_START 0x000B
#define SRSLTE_CRNTI_END 0xFFF3
#define SRSLTE_RARNTI_START 0x0001
#define SRSLTE_RARNTI_END 0x000A
#define SRSLTE_SIRNTI 0xFFFF
#define SRSLTE_PRNTI 0xFFFE
#define SRSLTE_MRNTI 0xFFFD

#define SRSLTE_RNTI_ISRAR(rnti) (rnti >= SRSLTE_RARNTI_START && rnti <= SRSLTE_RARNTI_END)
#define SRSLTE_RNTI_ISUSER(rnti) (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END)
#define SRSLTE_RNTI_ISSI(rnti) (rnti == SRSLTE_SIRNTI)
#define SRSLTE_RNTI_ISPA(rnti) (rnti == SRSLTE_PRNTI)
#define SRSLTE_RNTI_ISMBSFN(rnti) (rnti == SRSLTE_MRNTI)
#define SRSLTE_RNTI_ISSIRAPA(rnti) (SRSLTE_RNTI_ISSI(rnti) || SRSLTE_RNTI_ISRAR(rnti) || SRSLTE_RNTI_ISPA(rnti))

#define SRSLTE_CELL_ID_UNKNOWN 1000

#define SRSLTE_MAX_NSYMB 7

#define SRSLTE_MAX_PRB 110
#define SRSLTE_NRE 12

#define SRSLTE_SYMBOL_SZ_MAX 2048

#define SRSLTE_CP_NORM_NSYMB 7
#define SRSLTE_CP_NORM_SF_NSYMB (2 * SRSLTE_CP_NORM_NSYMB)
#define SRSLTE_CP_NORM_0_LEN 160
#define SRSLTE_CP_NORM_LEN 144

#define SRSLTE_CP_EXT_NSYMB 6
#define SRSLTE_CP_EXT_SF_NSYMB (2 * SRSLTE_CP_EXT_NSYMB)
#define SRSLTE_CP_EXT_LEN 512
#define SRSLTE_CP_EXT_7_5_LEN 1024

#define SRSLTE_CP_ISNORM(cp) (cp == SRSLTE_CP_NORM)
#define SRSLTE_CP_ISEXT(cp) (cp == SRSLTE_CP_EXT)
#define SRSLTE_CP_NSYMB(cp) (SRSLTE_CP_ISNORM(cp) ? SRSLTE_CP_NORM_NSYMB : SRSLTE_CP_EXT_NSYMB)

#define SRSLTE_CP_LEN(symbol_sz, c) ((int)ceilf((((float)(c) * (symbol_sz)) / 2048.0f)))
#define SRSLTE_CP_LEN_NORM(symbol, symbol_sz)                                                                          \
  (((symbol) == 0) ? SRSLTE_CP_LEN((symbol_sz), SRSLTE_CP_NORM_0_LEN) : SRSLTE_CP_LEN((symbol_sz), SRSLTE_CP_NORM_LEN))
#define SRSLTE_CP_LEN_EXT(symbol_sz) (SRSLTE_CP_LEN((symbol_sz), SRSLTE_CP_EXT_LEN))

#define SRSLTE_CP_SZ(symbol_sz, cp)                                                                                    \
  (SRSLTE_CP_LEN(symbol_sz, (SRSLTE_CP_ISNORM(cp) ? SRSLTE_CP_NORM_LEN : SRSLTE_CP_EXT_LEN)))
#define SRSLTE_SYMBOL_SZ(symbol_sz, cp) (symbol_sz + SRSLTE_CP_SZ(symbol_sz, cp))
#define SRSLTE_SLOT_LEN(symbol_sz) (symbol_sz * 15 / 2)
#define SRSLTE_SF_LEN(symbol_sz) (symbol_sz * 15)
#define SRSLTE_SF_LEN_MAX (SRSLTE_SF_LEN(SRSLTE_SYMBOL_SZ_MAX))

#define SRSLTE_SLOT_LEN_PRB(nof_prb) (SRSLTE_SLOT_LEN(srslte_symbol_sz(nof_prb)))
#define SRSLTE_SF_LEN_PRB(nof_prb) ((uint32_t)SRSLTE_SF_LEN(srslte_symbol_sz(nof_prb)))

#define SRSLTE_SLOT_LEN_RE(nof_prb, cp) (nof_prb * SRSLTE_NRE * SRSLTE_CP_NSYMB(cp))
#define SRSLTE_SF_LEN_RE(nof_prb, cp) (2 * SRSLTE_SLOT_LEN_RE(nof_prb, cp))
#define SRSLTE_NOF_RE(cell) (2 * SRSLTE_SLOT_LEN_RE(cell.nof_prb, cell.cp))

#define SRSLTE_TA_OFFSET (10e-6)

#define SRSLTE_LTE_TS (1.0f / (15000.0f * 2048.0f))

#define SRSLTE_SLOT_IDX_CPNORM(symbol_idx, symbol_sz)                                                                  \
  (symbol_idx == 0 ? 0                                                                                                 \
                   : (symbol_sz + SRSLTE_CP_LEN(symbol_sz, SRSLTE_CP_NORM_0_LEN) +                                     \
                      (symbol_idx - 1) * (symbol_sz + SRSLTE_CP_LEN(symbol_sz, SRSLTE_CP_NORM_LEN))))
#define SRSLTE_SLOT_IDX_CPEXT(idx, symbol_sz) (idx * (symbol_sz + SRSLTE_CP(symbol_sz, SRSLTE_CP_EXT_LEN)))

#define SRSLTE_RE_IDX(nof_prb, symbol_idx, sample_idx) ((symbol_idx) * (nof_prb) * (SRSLTE_NRE) + sample_idx)

#define SRSLTE_RS_VSHIFT(cell_id) (cell_id % 6)

#define SRSLTE_GUARD_RE(nof_prb) ((srslte_symbol_sz(nof_prb) - nof_prb * SRSLTE_NRE) / 2)

#define SRSLTE_SYMBOL_HAS_REF(l, cp, nof_ports) ((l == 1 && nof_ports == 4) || l == 0 || l == SRSLTE_CP_NSYMB(cp) - 3)

#define SRSLTE_NOF_CTRL_SYMBOLS(cell, cfi) (cfi + (cell.nof_prb < 10 ? 1 : 0))

#define SRSLTE_SYMBOL_HAS_REF_MBSFN(l, s) ((l == 2 && s == 0) || (l == 0 && s == 1) || (l == 4 && s == 1))

#define SRSLTE_NON_MBSFN_REGION_GUARD_LENGTH(non_mbsfn_region, symbol_sz)                                              \
  ((non_mbsfn_region == 1)                                                                                             \
       ? (SRSLTE_CP_LEN_EXT(symbol_sz) - SRSLTE_CP_LEN_NORM(0, symbol_sz))                                             \
       : (2 * SRSLTE_CP_LEN_EXT(symbol_sz) - SRSLTE_CP_LEN_NORM(0, symbol_sz) - SRSLTE_CP_LEN_NORM(1, symbol_sz)))

#define SRSLTE_FDD_NOF_HARQ (FDD_HARQ_DELAY_DL_MS + FDD_HARQ_DELAY_UL_MS)
#define SRSLTE_MAX_HARQ_PROC 15

#define SRSLTE_NOF_LTE_BANDS 58

#define SRSLTE_DEFAULT_MAX_FRAMES_PBCH 500
#define SRSLTE_DEFAULT_MAX_FRAMES_PSS 10
#define SRSLTE_DEFAULT_NOF_VALID_PSS_FRAMES 10

#define ZERO_OBJECT(x) memset(&(x), 0x0, sizeof((x)))

typedef enum SRSLTE_API { SRSLTE_PHICH_NORM = 0, SRSLTE_PHICH_EXT } srslte_phich_length_t;

typedef enum SRSLTE_API {
  SRSLTE_PHICH_R_1_6 = 0,
  SRSLTE_PHICH_R_1_2,
  SRSLTE_PHICH_R_1,
  SRSLTE_PHICH_R_2
} srslte_phich_r_t;

typedef enum SRSLTE_API { SRSLTE_FDD = 0, SRSLTE_TDD = 1 } srslte_frame_type_t;

typedef struct SRSLTE_API {
  uint32_t sf_config;
  uint32_t ss_config;
  bool     configured;
} srslte_tdd_config_t;

typedef enum SRSLTE_API {
  SRSLTE_TDD_SF_D = 0,
  SRSLTE_TDD_SF_U = 1,
  SRSLTE_TDD_SF_S = 2,
} srslte_tdd_sf_t;

typedef struct {
  uint8_t mbsfn_area_id;
  uint8_t non_mbsfn_region_length;
  uint8_t mbsfn_mcs;
  bool    enable;
  bool    is_mcch;
} srslte_mbsfn_cfg_t;

// Common cell constant properties that require object reconfiguration
typedef struct SRSLTE_API {
  uint32_t              nof_prb;
  uint32_t              nof_ports;
  uint32_t              id;
  srslte_cp_t           cp;
  srslte_phich_length_t phich_length;
  srslte_phich_r_t      phich_resources;
  srslte_frame_type_t   frame_type;
} srslte_cell_t;

// Common downlink properties that may change every subframe
typedef struct SRSLTE_API {
  srslte_tdd_config_t tdd_config;
  uint32_t            tti;
  uint32_t            cfi;
  srslte_sf_t         sf_type;
  uint32_t            non_mbsfn_region;
} srslte_dl_sf_cfg_t;

typedef struct SRSLTE_API {
  srslte_tdd_config_t tdd_config;
  uint32_t            tti;
  bool                shortened;
} srslte_ul_sf_cfg_t;

typedef enum SRSLTE_API {
  SRSLTE_TM1 = 0,
  SRSLTE_TM2,
  SRSLTE_TM3,
  SRSLTE_TM4,
  SRSLTE_TM5,
  SRSLTE_TM6,
  SRSLTE_TM7,
  SRSLTE_TM8,
  SRSLTE_TMINV // Invalid Transmission Mode
} srslte_tm_t;

typedef enum SRSLTE_API {
  SRSLTE_TXSCHEME_PORT0,
  SRSLTE_TXSCHEME_DIVERSITY,
  SRSLTE_TXSCHEME_SPATIALMUX,
  SRSLTE_TXSCHEME_CDD
} srslte_tx_scheme_t;

typedef enum SRSLTE_API { SRSLTE_MIMO_DECODER_ZF, SRSLTE_MIMO_DECODER_MMSE } srslte_mimo_decoder_t;

typedef enum SRSLTE_API {
  SRSLTE_MOD_BPSK = 0,
  SRSLTE_MOD_QPSK,
  SRSLTE_MOD_16QAM,
  SRSLTE_MOD_64QAM,
  SRSLTE_MOD_256QAM,
  SRSLTE_MOD_NITEMS
} srslte_mod_t;

typedef enum {
  SRSLTE_DCI_FORMAT0 = 0,
  SRSLTE_DCI_FORMAT1,
  SRSLTE_DCI_FORMAT1A,
  SRSLTE_DCI_FORMAT1C,
  SRSLTE_DCI_FORMAT1B,
  SRSLTE_DCI_FORMAT1D,
  SRSLTE_DCI_FORMAT2,
  SRSLTE_DCI_FORMAT2A,
  SRSLTE_DCI_FORMAT2B,
  // SRSLTE_DCI_FORMAT3,
  // SRSLTE_DCI_FORMAT3A,
  SRSLTE_DCI_FORMATN0,
  SRSLTE_DCI_FORMATN1,
  SRSLTE_DCI_FORMATN2,
  SRSLTE_DCI_FORMAT_RAR, // Not a real LTE format. Used internally to indicate RAR grant
  SRSLTE_DCI_NOF_FORMATS
} srslte_dci_format_t;

typedef enum {
  SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL = 0, /* No cell selection no pucch3 */
  SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS,
  SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3,
  SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_ERROR,
} srslte_ack_nack_feedback_mode_t;

typedef struct SRSLTE_API {
  int   id;
  float fd;
} srslte_earfcn_t;

enum band_geographical_area {
  SRSLTE_BAND_GEO_AREA_ALL,
  SRSLTE_BAND_GEO_AREA_NAR,
  SRSLTE_BAND_GEO_AREA_APAC,
  SRSLTE_BAND_GEO_AREA_EMEA,
  SRSLTE_BAND_GEO_AREA_JAPAN,
  SRSLTE_BAND_GEO_AREA_CALA,
  SRSLTE_BAND_GEO_AREA_NA
};

///< NB-IoT specific structs
typedef enum {
  SRSLTE_NBIOT_MODE_INBAND_SAME_PCI = 0,
  SRSLTE_NBIOT_MODE_INBAND_DIFFERENT_PCI,
  SRSLTE_NBIOT_MODE_GUARDBAND,
  SRSLTE_NBIOT_MODE_STANDALONE,
  SRSLTE_NBIOT_MODE_N_ITEMS,
} srslte_nbiot_mode_t;

typedef struct SRSLTE_API {
  srslte_cell_t       base;      // the umbrella or super cell
  uint32_t            nbiot_prb; // the index of the NB-IoT PRB within the cell
  uint32_t            n_id_ncell;
  uint32_t            nof_ports; // The number of antenna ports for NB-IoT
  bool                is_r14;    // Whether the cell is a R14 cell
  srslte_nbiot_mode_t mode;
} srslte_nbiot_cell_t;

#define SRSLTE_NBIOT_MAX_PORTS 2
#define SRSLTE_NBIOT_MAX_CODEWORDS SRSLTE_MAX_CODEWORDS

#define SRSLTE_SF_LEN_PRB_NBIOT (SRSLTE_SF_LEN_PRB(1))

#define SRSLTE_SF_LEN_RE_NBIOT (SRSLTE_SF_LEN_RE(1, SRSLTE_CP_NORM))

#define SRSLTE_NBIOT_FFT_SIZE 128
#define SRSLTE_NBIOT_FREQ_SHIFT_FACTOR ((float)-0.5)
#define SRSLTE_NBIOT_NUM_RX_ANTENNAS 1
#define SRSLTE_NBIOT_MAX_PRB 1

#define SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL 1
#define SRSLTE_NBIOT_DEFAULT_PRB_OFFSET 0

#define SRSLTE_DEFAULT_MAX_FRAMES_NPBCH 500
#define SRSLTE_DEFAULT_MAX_FRAMES_NPSS 20
#define SRSLTE_DEFAULT_NOF_VALID_NPSS_FRAMES 20

#define SRSLTE_NBIOT_NPBCH_NOF_TOTAL_BITS (1600) ///< Number of bits for the entire NPBCH (See 36.211 Sec 10.2.4.1)
#define SRSLTE_NBIOT_NPBCH_NOF_BITS_SF                                                                                 \
  (SRSLTE_NBIOT_NPBCH_NOF_TOTAL_BITS / 8) ///< The NPBCH is transmitted in 8 blocks (See 36.211 Sec 10.2.4.4)

#define SRSLTE_MAX_DL_BITS_CAT_NB1 (680) ///< TS 36.306 v15.4.0 Table 4.1C-1

///< PHY common function declarations

SRSLTE_API bool srslte_cell_isvalid(srslte_cell_t* cell);

SRSLTE_API void srslte_cell_fprint(FILE* stream, srslte_cell_t* cell, uint32_t sfn);

SRSLTE_API bool srslte_cellid_isvalid(uint32_t cell_id);

SRSLTE_API bool srslte_nofprb_isvalid(uint32_t nof_prb);

SRSLTE_API srslte_tdd_sf_t srslte_sfidx_tdd_type(srslte_tdd_config_t tdd_config, uint32_t sf_idx);

SRSLTE_API uint32_t srslte_tdd_nof_harq(srslte_tdd_config_t tdd_config);

SRSLTE_API uint32_t srslte_sfidx_tdd_nof_up(srslte_tdd_config_t tdd_config);

SRSLTE_API uint32_t srslte_sfidx_tdd_nof_gp(srslte_tdd_config_t tdd_config);

SRSLTE_API uint32_t srslte_sfidx_tdd_nof_dw(srslte_tdd_config_t tdd_config);

SRSLTE_API uint32_t srslte_sfidx_tdd_nof_dw_slot(srslte_tdd_config_t tdd_config, uint32_t slot, srslte_cp_t cp);

SRSLTE_API bool srslte_sfidx_isvalid(uint32_t sf_idx);

SRSLTE_API bool srslte_portid_isvalid(uint32_t port_id);

SRSLTE_API bool srslte_N_id_2_isvalid(uint32_t N_id_2);

SRSLTE_API bool srslte_N_id_1_isvalid(uint32_t N_id_1);

SRSLTE_API bool srslte_symbol_sz_isvalid(uint32_t symbol_sz);

SRSLTE_API int srslte_symbol_sz(uint32_t nof_prb);

SRSLTE_API int srslte_symbol_sz_power2(uint32_t nof_prb);

SRSLTE_API int srslte_nof_prb(uint32_t symbol_sz);

SRSLTE_API uint32_t srslte_max_cce(uint32_t nof_prb);

SRSLTE_API int srslte_sampling_freq_hz(uint32_t nof_prb);

SRSLTE_API void srslte_use_standard_symbol_size(bool enabled);

SRSLTE_API uint32_t srslte_re_x_prb(uint32_t ns, uint32_t symbol, uint32_t nof_ports, uint32_t nof_symbols);

SRSLTE_API uint32_t srslte_voffset(uint32_t symbol_id, uint32_t cell_id, uint32_t nof_ports);

SRSLTE_API int srslte_group_hopping_f_gh(uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME], uint32_t cell_id);

SRSLTE_API uint32_t srslte_N_ta_new_rar(uint32_t ta);

SRSLTE_API uint32_t srslte_N_ta_new(uint32_t N_ta_old, uint32_t ta);

SRSLTE_API float srslte_coderate(uint32_t tbs, uint32_t nof_re);

SRSLTE_API char* srslte_cp_string(srslte_cp_t cp);

SRSLTE_API srslte_mod_t srslte_str2mod(char* mod_str);

SRSLTE_API char* srslte_mod_string(srslte_mod_t mod);

SRSLTE_API uint32_t srslte_mod_bits_x_symbol(srslte_mod_t mod);

SRSLTE_API uint8_t srslte_band_get_band(uint32_t dl_earfcn);

SRSLTE_API bool srslte_band_is_tdd(uint32_t band);

SRSLTE_API double srslte_band_fd(uint32_t dl_earfcn);

SRSLTE_API double srslte_band_fu(uint32_t ul_earfcn);

SRSLTE_API uint32_t srslte_band_ul_earfcn(uint32_t dl_earfcn);

SRSLTE_API int
srslte_band_get_fd_band(uint32_t band, srslte_earfcn_t* earfcn, int earfcn_start, int earfcn_end, uint32_t max_elems);

SRSLTE_API int srslte_band_get_fd_band_all(uint32_t band, srslte_earfcn_t* earfcn, uint32_t max_nelems);

SRSLTE_API int
srslte_band_get_fd_region(enum band_geographical_area region, srslte_earfcn_t* earfcn, uint32_t max_elems);

SRSLTE_API int srslte_str2mimotype(char* mimo_type_str, srslte_tx_scheme_t* type);

SRSLTE_API char* srslte_mimotype2str(srslte_tx_scheme_t mimo_type);

/* Returns the interval tti1-tti2 mod 10240 */
SRSLTE_API uint32_t srslte_tti_interval(uint32_t tti1, uint32_t tti2);

SRSLTE_API uint32_t srslte_print_check(char* s, size_t max_len, uint32_t cur_len, const char* format, ...);

SRSLTE_API bool  srslte_nbiot_cell_isvalid(srslte_nbiot_cell_t* cell);
SRSLTE_API bool  srslte_nbiot_portid_isvalid(uint32_t port_id);
SRSLTE_API float srslte_band_fu_nbiot(uint32_t ul_earfcn, const float m_ul);

SRSLTE_API char* srslte_nbiot_mode_string(srslte_nbiot_mode_t mode);

/**
 * Returns a constant string pointer with the ACK/NACK feedback mode
 *
 * @param ack_nack_feedback_mode Mode
 * @return Returns constant pointer with the text of the mode if succesful, `error` otherwise
 */
SRSLTE_API const char* srslte_ack_nack_feedback_mode_string(srslte_ack_nack_feedback_mode_t ack_nack_feedback_mode);

/**
 * Returns a constant string pointer with the ACK/NACK feedback mode
 *
 * @param ack_nack_feedback_mode Mode
 * @return Returns constant pointer with the text of the mode if succesful, `error` otherwise
 */
SRSLTE_API srslte_ack_nack_feedback_mode_t srslte_string_ack_nack_feedback_mode(const char* str);

/**
 * Returns the number of bits for Rank Indicador reporting depending on the cell
 *
 * @param cell
 */
SRSLTE_API uint32_t srslte_ri_nof_bits(const srslte_cell_t* cell);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_PHY_COMMON_H
