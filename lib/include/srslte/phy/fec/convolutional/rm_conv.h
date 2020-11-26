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
 *  File:         rm_conv.h
 *
 *  Description:  Rate matching for convolutionally coded transport channels and control
 *                information.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.4.2
 *********************************************************************************************/

#ifndef SRSLTE_RM_CONV_H
#define SRSLTE_RM_CONV_H

#include "srslte/config.h"

#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif
SRSLTE_API int srslte_rm_conv_tx(uint8_t* input, uint32_t in_len, uint8_t* output, uint32_t out_len);

SRSLTE_API int srslte_rm_conv_rx(float* input, uint32_t in_len, float* output, uint32_t out_len);

/************* FIX THIS. MOVE ALL PROCESSING TO INT16 AND HAVE ONLY 1 IMPLEMENTATION ******/

SRSLTE_API int srslte_rm_conv_rx_s(int16_t* input, uint32_t in_len, int16_t* output, uint32_t out_len);

#endif // SRSLTE_RM_CONV_H
