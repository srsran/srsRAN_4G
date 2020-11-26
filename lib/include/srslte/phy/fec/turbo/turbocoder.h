/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/**********************************************************************************************
 *  File:         turbocoder.h
 *
 *  Description:  Turbo coder.
 *                Parallel Concatenated Convolutional Code (PCCC) with two 8-state constituent
 *                encoders and one turbo code internal interleaver. The coding rate of turbo
 *                encoder is 1/3.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.2
 *********************************************************************************************/

#ifndef SRSLTE_TURBOCODER_H
#define SRSLTE_TURBOCODER_H

#include "srslte/config.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/turbo/tc_interl.h"
#define SRSLTE_TCOD_MAX_LEN_CB_BYTES (6144 / 8)

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif

typedef struct SRSLTE_API {
  uint32_t max_long_cb;
  uint8_t* temp;
} srslte_tcod_t;

/* This structure is used as an output for the LUT version of the encoder.
 * The encoder produces parity bits only and rate matching will interleave them
 * with the systematic bits
 */

SRSLTE_API int srslte_tcod_init(srslte_tcod_t* h, uint32_t max_long_cb);

SRSLTE_API void srslte_tcod_free(srslte_tcod_t* h);

SRSLTE_API int srslte_tcod_encode(srslte_tcod_t* h, uint8_t* input, uint8_t* output, uint32_t long_cb);

SRSLTE_API int srslte_tcod_encode_lut(srslte_tcod_t* h,
                                      srslte_crc_t*  crc_tb,
                                      srslte_crc_t*  crc_cb,
                                      uint8_t*       input,
                                      uint8_t*       parity,
                                      uint32_t       cblen_idx,
                                      bool           last_cb);

SRSLTE_API void srslte_tcod_gentable();

#endif // SRSLTE_TURBOCODER_H
