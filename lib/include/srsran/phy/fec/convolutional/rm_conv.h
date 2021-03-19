/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/**********************************************************************************************
 *  File:         rm_conv.h
 *
 *  Description:  Rate matching for convolutionally coded transport channels and control
 *                information.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.4.2
 *********************************************************************************************/

#ifndef SRSRAN_RM_CONV_H
#define SRSRAN_RM_CONV_H

#include "srsran/config.h"

#ifndef SRSRAN_RX_NULL
#define SRSRAN_RX_NULL 10000
#endif

#ifndef SRSRAN_TX_NULL
#define SRSRAN_TX_NULL 100
#endif
SRSRAN_API int srsran_rm_conv_tx(uint8_t* input, uint32_t in_len, uint8_t* output, uint32_t out_len);

SRSRAN_API int srsran_rm_conv_rx(float* input, uint32_t in_len, float* output, uint32_t out_len);

/************* FIX THIS. MOVE ALL PROCESSING TO INT16 AND HAVE ONLY 1 IMPLEMENTATION ******/

SRSRAN_API int srsran_rm_conv_rx_s(int16_t* input, uint32_t in_len, int16_t* output, uint32_t out_len);

#endif // SRSRAN_RM_CONV_H
