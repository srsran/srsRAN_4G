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

#ifndef SRSLTE_NPDCCH_H
#define SRSLTE_NPDCCH_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/scrambling/scrambling.h"

#define SRSLTE_RARNTI_END_NBIOT 0x0100
#define SRSLTE_NBIOT_NUM_NRS_SYMS 8
#define SRSLTE_NPDCCH_MAX_RE (SRSLTE_NRE * SRSLTE_CP_NORM_SF_NSYMB - SRSLTE_NBIOT_NUM_NRS_SYMS)

#define SRSLTE_NBIOT_DCI_MAX_SIZE 23
#define SRSLTE_AL_REPETITION_USS 64 // Higher layer configured parameter al-Repetition-USS

typedef enum SRSLTE_API {
  SRSLTE_NPDCCH_FORMAT1 = 0,
  SRSLTE_NPDCCH_FORMAT0_LOWER_HALF,
  SRSLTE_NPDCCH_FORMAT0_UPPER_HALF,
  SRSLTE_NPDCCH_FORMAT_NITEMS
} srslte_npdcch_format_t;
static const char srslte_npdcch_format_text[SRSLTE_NPDCCH_FORMAT_NITEMS][30] = {"Format 1",
                                                                                "Format 0 (Lower Half)",
                                                                                "Format 0 (Upper Half)"};

/**
 * @brief Narrowband Physical downlink control channel (NPDCCH)
 *
 * Reference: 3GPP TS 36.211 version 13.2.0 Release 11 Sec. 6.8 and 10.2.5
 */
typedef struct SRSLTE_API {
  srslte_nbiot_cell_t cell;
  uint32_t            nof_cce;
  uint32_t            ncce_bits;
  uint32_t            max_bits;
  uint32_t            i_n_start;      /// start of the first OFDM symbol (signalled through NB-SIB1)
  uint32_t            nof_nbiot_refs; /// number of NRS symbols per OFDM symbol
  uint32_t            nof_lte_refs;   /// number of CRS symbols per OFDM symbol
  uint32_t            num_decoded_symbols;

  /* buffers */
  cf_t*    ce[SRSLTE_MAX_PORTS];
  cf_t*    symbols[SRSLTE_MAX_PORTS];
  cf_t*    x[SRSLTE_MAX_PORTS];
  cf_t*    d;
  uint8_t* e;
  float    rm_f[3 * (SRSLTE_DCI_MAX_BITS + 16)];
  float*   llr[2]; // Two layers of LLRs for Format0 and Format1 NPDCCH

  /* tx & rx objects */
  srslte_modem_table_t mod;
  srslte_sequence_t    seq[SRSLTE_NOF_SF_X_FRAME];
  srslte_viterbi_t     decoder;
  srslte_crc_t         crc;

} srslte_npdcch_t;

SRSLTE_API int srslte_npdcch_init(srslte_npdcch_t* q);

SRSLTE_API void srslte_npdcch_free(srslte_npdcch_t* q);

SRSLTE_API int srslte_npdcch_set_cell(srslte_npdcch_t* q, srslte_nbiot_cell_t cell);

/// Encoding function
SRSLTE_API int srslte_npdcch_encode(srslte_npdcch_t*      q,
                                    srslte_dci_msg_t*     msg,
                                    srslte_dci_location_t location,
                                    uint16_t              rnti,
                                    cf_t*                 sf_symbols[SRSLTE_MAX_PORTS],
                                    uint32_t              nsubframe);

/// Decoding functions: Extract the LLRs and save them in the srslte_npdcch_t object
SRSLTE_API int srslte_npdcch_extract_llr(srslte_npdcch_t* q,
                                         cf_t*            sf_symbols,
                                         cf_t*            ce[SRSLTE_MAX_PORTS],
                                         float            noise_estimate,
                                         uint32_t         sf_idx);

/// Decoding functions: Try to decode a DCI message after calling srslte_npdcch_extract_llr
SRSLTE_API int srslte_npdcch_decode_msg(srslte_npdcch_t*       q,
                                        srslte_dci_msg_t*      msg,
                                        srslte_dci_location_t* location,
                                        srslte_dci_format_t    format,
                                        uint16_t*              crc_rem);

SRSLTE_API int
srslte_npdcch_dci_decode(srslte_npdcch_t* q, float* e, uint8_t* data, uint32_t E, uint32_t nof_bits, uint16_t* crc);

SRSLTE_API int
srslte_npdcch_dci_encode(srslte_npdcch_t* q, uint8_t* data, uint8_t* e, uint32_t nof_bits, uint32_t E, uint16_t rnti);

SRSLTE_API void
srslte_npdcch_dci_encode_conv(srslte_npdcch_t* q, uint8_t* data, uint32_t nof_bits, uint8_t* coded_data, uint16_t rnti);

SRSLTE_API uint32_t srslte_npdcch_ue_locations(srslte_dci_location_t* c, uint32_t max_candidates);

SRSLTE_API uint32_t srslte_npdcch_common_locations(srslte_dci_location_t* c, uint32_t max_candidates);

int srslte_npdcch_cp(srslte_npdcch_t* q, cf_t* input, cf_t* output, bool put, srslte_npdcch_format_t format);
int srslte_npdcch_put(srslte_npdcch_t* q, cf_t* symbols, cf_t* sf_symbols, srslte_npdcch_format_t format);
int srslte_npdcch_get(srslte_npdcch_t* q, cf_t* symbols, cf_t* sf_symbols, srslte_npdcch_format_t format);

#endif // SRSLTE_NPDCCH_H
