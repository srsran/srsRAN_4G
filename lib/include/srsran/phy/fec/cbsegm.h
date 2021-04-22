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

#ifndef SRSRAN_CBSEGM_H
#define SRSRAN_CBSEGM_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"

#define SRSRAN_NOF_TC_CB_SIZES 188

typedef struct SRSRAN_API {
  uint32_t F;      ///< \brief Total number of the transport block bit
  uint32_t C;      ///< \brief Total number of code blocks
  uint32_t K1;     ///< \brief Code block size 1
  uint32_t K2;     ///< \brief Code block size 2
  uint32_t K1_idx; ///< \brief Code block size index 1
  uint32_t K2_idx; ///< \brief Code block size index 2
  uint32_t C1;     ///< \brief Number of code blocks of size 1
  uint32_t C2;     ///< \brief Number of code blocks of size 2
  uint32_t tbs;    ///< \brief Actual transport block size
  uint32_t L_tb;   ///< \brief Transport block CRC length
  uint32_t L_cb;   ///< \brief Code block CRC length
  uint32_t Z;      ///< \brief Lifting size, LDPC only
} srsran_cbsegm_t;

/**
 * @brief Calculate code block Segmentation for LTE
 *
 * @remark Defined as in TS 36.212 V8.8.0 section 5.1.2 Code block segmentation and code block CRC attachment
 *
 * @param[out] s Output of code block segmentation calculation
 * @param[in] tbs Input Transport Block Size in bits. CRC's will be added to this
 * @return It returns SRSRAN_SUCCESS if the provided arguments are valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int srsran_cbsegm(srsran_cbsegm_t* s, uint32_t tbs);

/**
 * @brief provides interleaver size  Table 5.1.3-3 (36.212) index
 *
 * @remark Defined as in TS 36.212 V8.8.0 Table 5.1.3-3 Turbo code internal interleaver parameters
 *
 * @return It returns Turbo coder interleaver size if the provided arguments are valid, otherwise it returns
 * SRSRAN_ERROR code
 */
SRSRAN_API int srsran_cbsegm_cbsize(uint32_t index);

/**
 * @brief Check is code block size is valid for LTE Turbo Code
 *
 * @param[in] size Size of code block in bits
 * @return true if Code Block size is allowed
 */
SRSRAN_API bool srsran_cbsegm_cbsize_isvalid(uint32_t size);

/**
 * @brief Finds index of minimum K>=long_cb in Table 5.1.3-3 of 36.212
 *
 * @return I_TBS or error code
 */
SRSRAN_API int srsran_cbsegm_cbindex(uint32_t long_cb);

/**
 * @brief Calculate code block Segmentation for NR LDPC base graph 1
 *
 * @remark Defined as in Section 5.2.2 of 38.212 V15.9.0
 *
 * @param[out] s Output of code block segmentation calculation
 * @param[in] tbs Input Transport Block Size in bits. CRC's will be added to this
 * @return It returns SRSRAN_SUCCESS if the provided arguments are valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int srsran_cbsegm_ldpc_bg1(srsran_cbsegm_t* s, uint32_t tbs);

/**
 * @brief Calculate code block Segmentation for NR LDPC base graph 2
 *
 * @remark Defined as in Section 5.2.2 of 38.212 V15.9.0
 *
 * @param[out] s Output of code block segmentation calculation
 * @param[in] tbs Input Transport Block Size in bits. CRC's will be added to this
 * @return It returns SRSRAN_SUCCESS if the provided arguments are valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int srsran_cbsegm_ldpc_bg2(srsran_cbsegm_t* s, uint32_t tbs);

#endif // SRSRAN_CBSEGM_H
