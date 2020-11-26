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
 *  File:         rm_turbo.h
 *
 *  Description:  Rate matching for turbo coded transport channels.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.4.1
 *********************************************************************************************/

#ifndef SRSLTE_RM_TURBO_H
#define SRSLTE_RM_TURBO_H

#include "srslte/config.h"
#include "srslte/phy/fec/turbo/turbodecoder.h"

#ifndef SRSLTE_RX_NULL
#define SRSLTE_RX_NULL 10000
#endif

#ifndef SRSLTE_TX_NULL
#define SRSLTE_TX_NULL 100
#endif

#include "srslte/config.h"

SRSLTE_API int srslte_rm_turbo_tx(uint8_t* w_buff,
                                  uint32_t buff_len,
                                  uint8_t* input,
                                  uint32_t in_len,
                                  uint8_t* output,
                                  uint32_t out_len,
                                  uint32_t rv_idx);

SRSLTE_API void srslte_rm_turbo_gentables();

SRSLTE_API void srslte_rm_turbo_free_tables();

SRSLTE_API int srslte_rm_turbo_tx_lut(uint8_t* w_buff,
                                      uint8_t* systematic,
                                      uint8_t* parity,
                                      uint8_t* output,
                                      uint32_t cb_idx,
                                      uint32_t out_len,
                                      uint32_t w_offset,
                                      uint32_t rv_idx);

SRSLTE_API int srslte_rm_turbo_rx(float*   w_buff,
                                  uint32_t buff_len,
                                  float*   input,
                                  uint32_t in_len,
                                  float*   output,
                                  uint32_t out_len,
                                  uint32_t rv_idx,
                                  uint32_t nof_filler_bits);

SRSLTE_API int
srslte_rm_turbo_rx_lut(int16_t* input, int16_t* output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx);

SRSLTE_API int srslte_rm_turbo_rx_lut_(int16_t* input,
                                       int16_t* output,
                                       uint32_t in_len,
                                       uint32_t cb_idx,
                                       uint32_t rv_idx,
                                       bool     enable_input_tdec);

SRSLTE_API int
srslte_rm_turbo_rx_lut_8bit(int8_t* input, int8_t* output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx);

#endif // SRSLTE_RM_TURBO_H
