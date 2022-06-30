/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_TURBOCODER_H
#define SRSRAN_TURBOCODER_H

#include "srsran/config.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/turbo/tc_interl.h"
#define SRSRAN_TCOD_MAX_LEN_CB_BYTES (6144 / 8)

#ifndef SRSRAN_TX_NULL
#define SRSRAN_TX_NULL 100
#endif

typedef struct SRSRAN_API {
  uint32_t max_long_cb;
  uint8_t* temp;
} srsran_tcod_t;

/* This structure is used as an output for the LUT version of the encoder.
 * The encoder produces parity bits only and rate matching will interleave them
 * with the systematic bits
 */

SRSRAN_API int srsran_tcod_init(srsran_tcod_t* h, uint32_t max_long_cb);

SRSRAN_API void srsran_tcod_free(srsran_tcod_t* h);

SRSRAN_API int srsran_tcod_encode(srsran_tcod_t* h, uint8_t* input, uint8_t* output, uint32_t long_cb);

SRSRAN_API int srsran_tcod_encode_lut(srsran_tcod_t* h,
                                      srsran_crc_t*  crc_tb,
                                      srsran_crc_t*  crc_cb,
                                      uint8_t*       input,
                                      uint8_t*       parity,
                                      uint32_t       cblen_idx,
                                      bool           last_cb);

SRSRAN_API void srsran_tcod_gentable();

#endif // SRSRAN_TURBOCODER_H
