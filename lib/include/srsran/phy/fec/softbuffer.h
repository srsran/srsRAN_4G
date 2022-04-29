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

/******************************************************************************
 *  File:         softbuffer.h
 *
 *  Description:  Buffer for RX and TX soft bits. This should be provided by MAC.
 *                Provided here basically for the examples.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_SOFTBUFFER_H
#define SRSRAN_SOFTBUFFER_H

#include "srsran/config.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SRSRAN_API {
  uint32_t  max_cb;
  uint32_t  max_cb_size;
  int16_t** buffer_f;
  uint8_t** data;
  bool*     cb_crc;
  bool      tb_crc;
} srsran_softbuffer_rx_t;

typedef struct SRSRAN_API {
  uint32_t  max_cb;
  uint32_t  max_cb_size;
  uint8_t** buffer_b;
} srsran_softbuffer_tx_t;

#define SOFTBUFFER_SIZE 18600

SRSRAN_API int srsran_softbuffer_rx_init(srsran_softbuffer_rx_t* q, uint32_t nof_prb);

/**
 * @brief Initialises Rx soft-buffer for a number of code blocks and their size
 * @param q The Rx soft-buffer pointer
 * @param max_cb The maximum number of code blocks to allocate
 * @param max_cb_size The code block size to allocate
 * @return It returns SRSRAN_SUCCESS if it allocates the soft-buffer successfully, otherwise it returns SRSRAN_ERROR
 * code
 */
SRSRAN_API int srsran_softbuffer_rx_init_guru(srsran_softbuffer_rx_t* q, uint32_t max_cb, uint32_t max_cb_size);

SRSRAN_API void srsran_softbuffer_rx_reset(srsran_softbuffer_rx_t* p);

SRSRAN_API void srsran_softbuffer_rx_reset_tbs(srsran_softbuffer_rx_t* q, uint32_t tbs);

SRSRAN_API void srsran_softbuffer_rx_reset_cb(srsran_softbuffer_rx_t* q, uint32_t nof_cb);

SRSRAN_API void srsran_softbuffer_rx_free(srsran_softbuffer_rx_t* p);

/**
 * @brief Resets a number of CB CRCs
 * @note This function is intended to be used if all CB CRC have matched but the TB CRC failed. In this case, all CB
 * should be decoded again
 * @param q Rx soft-buffer object
 * @param nof_cb Number of CB to reset
 */
SRSRAN_API void srsran_softbuffer_rx_reset_cb_crc(srsran_softbuffer_rx_t* q, uint32_t nof_cb);

SRSRAN_API int srsran_softbuffer_tx_init(srsran_softbuffer_tx_t* q, uint32_t nof_prb);

/**
 * @brief Initialises Tx soft-buffer for a number of code blocks and their size
 * @param q The Tx soft-buffer pointer
 * @param max_cb The maximum number of code blocks to allocate
 * @param max_cb_size The code block size to allocate
 * @return It returns SRSRAN_SUCCESS if it allocates the soft-buffer successfully, otherwise it returns SRSRAN_ERROR
 * code
 */
SRSRAN_API int srsran_softbuffer_tx_init_guru(srsran_softbuffer_tx_t* q, uint32_t max_cb, uint32_t max_cb_size);

SRSRAN_API void srsran_softbuffer_tx_reset(srsran_softbuffer_tx_t* p);

SRSRAN_API void srsran_softbuffer_tx_reset_tbs(srsran_softbuffer_tx_t* q, uint32_t tbs);

SRSRAN_API void srsran_softbuffer_tx_reset_cb(srsran_softbuffer_tx_t* q, uint32_t nof_cb);

SRSRAN_API void srsran_softbuffer_tx_free(srsran_softbuffer_tx_t* p);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_SOFTBUFFER_H
