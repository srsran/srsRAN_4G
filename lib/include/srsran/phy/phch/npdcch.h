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

#ifndef SRSRAN_NPDCCH_H
#define SRSRAN_NPDCCH_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/rm_conv.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/scrambling/scrambling.h"

#define SRSRAN_RARNTI_END_NBIOT 0x0100
#define SRSRAN_NBIOT_NUM_NRS_SYMS 8
#define SRSRAN_NPDCCH_MAX_RE (SRSRAN_NRE * SRSRAN_CP_NORM_SF_NSYMB - SRSRAN_NBIOT_NUM_NRS_SYMS)

#define SRSRAN_NBIOT_DCI_MAX_SIZE 23
#define SRSRAN_AL_REPETITION_USS 64 // Higher layer configured parameter al-Repetition-USS

typedef enum SRSRAN_API {
  SRSRAN_NPDCCH_FORMAT1 = 0,
  SRSRAN_NPDCCH_FORMAT0_LOWER_HALF,
  SRSRAN_NPDCCH_FORMAT0_UPPER_HALF,
  SRSRAN_NPDCCH_FORMAT_NITEMS
} srsran_npdcch_format_t;
static const char srsran_npdcch_format_text[SRSRAN_NPDCCH_FORMAT_NITEMS][30] = {"Format 1",
                                                                                "Format 0 (Lower Half)",
                                                                                "Format 0 (Upper Half)"};

/**
 * @brief Narrowband Physical downlink control channel (NPDCCH)
 *
 * Reference: 3GPP TS 36.211 version 13.2.0 Release 11 Sec. 6.8 and 10.2.5
 */
typedef struct SRSRAN_API {
  srsran_nbiot_cell_t cell;
  uint32_t            nof_cce;
  uint32_t            ncce_bits;
  uint32_t            max_bits;
  uint32_t            i_n_start;      /// start of the first OFDM symbol (signalled through NB-SIB1)
  uint32_t            nof_nbiot_refs; /// number of NRS symbols per OFDM symbol
  uint32_t            nof_lte_refs;   /// number of CRS symbols per OFDM symbol
  uint32_t            num_decoded_symbols;

  /* buffers */
  cf_t*    ce[SRSRAN_MAX_PORTS];
  cf_t*    symbols[SRSRAN_MAX_PORTS];
  cf_t*    x[SRSRAN_MAX_PORTS];
  cf_t*    d;
  uint8_t* e;
  float    rm_f[3 * (SRSRAN_DCI_MAX_BITS + 16)];
  float*   llr[2]; // Two layers of LLRs for Format0 and Format1 NPDCCH

  /* tx & rx objects */
  srsran_modem_table_t mod;
  srsran_sequence_t    seq[SRSRAN_NOF_SF_X_FRAME];
  srsran_viterbi_t     decoder;
  srsran_crc_t         crc;

} srsran_npdcch_t;

SRSRAN_API int srsran_npdcch_init(srsran_npdcch_t* q);

SRSRAN_API void srsran_npdcch_free(srsran_npdcch_t* q);

SRSRAN_API int srsran_npdcch_set_cell(srsran_npdcch_t* q, srsran_nbiot_cell_t cell);

/// Encoding function
SRSRAN_API int srsran_npdcch_encode(srsran_npdcch_t*      q,
                                    srsran_dci_msg_t*     msg,
                                    srsran_dci_location_t location,
                                    uint16_t              rnti,
                                    cf_t*                 sf_symbols[SRSRAN_MAX_PORTS],
                                    uint32_t              nsubframe);

/// Decoding functions: Extract the LLRs and save them in the srsran_npdcch_t object
SRSRAN_API int srsran_npdcch_extract_llr(srsran_npdcch_t* q,
                                         cf_t*            sf_symbols,
                                         cf_t*            ce[SRSRAN_MAX_PORTS],
                                         float            noise_estimate,
                                         uint32_t         sf_idx);

/// Decoding functions: Try to decode a DCI message after calling srsran_npdcch_extract_llr
SRSRAN_API int srsran_npdcch_decode_msg(srsran_npdcch_t*       q,
                                        srsran_dci_msg_t*      msg,
                                        srsran_dci_location_t* location,
                                        srsran_dci_format_t    format,
                                        uint16_t*              crc_rem);

SRSRAN_API int
srsran_npdcch_dci_decode(srsran_npdcch_t* q, float* e, uint8_t* data, uint32_t E, uint32_t nof_bits, uint16_t* crc);

SRSRAN_API int
srsran_npdcch_dci_encode(srsran_npdcch_t* q, uint8_t* data, uint8_t* e, uint32_t nof_bits, uint32_t E, uint16_t rnti);

SRSRAN_API void
srsran_npdcch_dci_encode_conv(srsran_npdcch_t* q, uint8_t* data, uint32_t nof_bits, uint8_t* coded_data, uint16_t rnti);

SRSRAN_API uint32_t srsran_npdcch_ue_locations(srsran_dci_location_t* c, uint32_t max_candidates);

SRSRAN_API uint32_t srsran_npdcch_common_locations(srsran_dci_location_t* c, uint32_t max_candidates);

int srsran_npdcch_cp(srsran_npdcch_t* q, cf_t* input, cf_t* output, bool put, srsran_npdcch_format_t format);
int srsran_npdcch_put(srsran_npdcch_t* q, cf_t* symbols, cf_t* sf_symbols, srsran_npdcch_format_t format);
int srsran_npdcch_get(srsran_npdcch_t* q, cf_t* symbols, cf_t* sf_symbols, srsran_npdcch_format_t format);

#endif // SRSRAN_NPDCCH_H
