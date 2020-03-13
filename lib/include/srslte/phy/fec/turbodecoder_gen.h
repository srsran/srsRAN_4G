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
 *  File:         turbodecoder.h
 *
 *  Description:  Turbo Decoder.
 *                Parallel Concatenated Convolutional Code (PCCC) with two 8-state constituent
 *                encoders and one turbo code internal interleaver. The coding rate of turbo
 *                encoder is 1/3.
 *                MAP_GEN is the MAX-LOG-MAP generic implementation of the decoder.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.2
 *********************************************************************************************/

#ifndef SRSLTE_TURBODECODER_GEN_H
#define SRSLTE_TURBODECODER_GEN_H

#include "srslte/config.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/tc_interl.h"

#define SRSLTE_TCOD_RATE 3
#define SRSLTE_TCOD_TOTALTAIL 12

#define SRSLTE_TCOD_MAX_LEN_CB 6144

typedef struct SRSLTE_API {
  uint32_t max_long_cb;
  int16_t* beta;
} tdec_gen_t;

int  tdec_gen_init(void** h, uint32_t max_long_cb);
void tdec_gen_free(void* h);
void tdec_gen_dec(void* h, int16_t* input, int16_t* app, int16_t* parity, int16_t* output, uint32_t long_cb);
void tdec_gen_extract_input(int16_t* input,
                            int16_t* syst,
                            int16_t* parity0,
                            int16_t* parity1,
                            int16_t* app2,
                            uint32_t long_cb);
void tdec_gen_decision_byte(int16_t* app1, uint8_t* output, uint32_t long_cb);

#endif // SRSLTE_TURBODECODER_GEN_H
