/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_PBCH_NR_H
#define SRSRAN_PBCH_NR_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/polar/polar_code.h"
#include "srsran/phy/fec/polar/polar_decoder.h"
#include "srsran/phy/fec/polar/polar_encoder.h"
#include "srsran/phy/fec/polar/polar_rm.h"
#include "srsran/phy/modem/modem_table.h"
#include "srsran/phy/phch/pbch_msg_nr.h"

/**
 * @brief Describes the NR PBCH object initialisation arguments
 */
typedef struct SRSRAN_API {
  bool enable_encode; ///< Enable encoder
  bool enable_decode; ///< Enable decoder
  bool disable_simd;  ///< Disable SIMD polar encoder/decoder
} srsran_pbch_nr_args_t;

/**
 * @brief Describes the NR PBCH configuration
 */
typedef struct SRSRAN_API {
  uint32_t N_id;    ///< Physical cell identifier
  uint32_t n_hf;    ///< Number of half radio frame, 0 or 1
  uint32_t ssb_idx; ///< SSB candidate index, up to 4 LSB significant
  uint32_t Lmax;    ///< Number of SSB opportunities, described in TS 38.213 4.1 ...
  float    beta;    ///< Scaling factor for PBCH symbols, set to zero for default
} srsran_pbch_nr_cfg_t;

/**
 * @brief Describes the NR PBCH object initialisation arguments
 */
typedef struct SRSRAN_API {
  srsran_polar_code_t    code;
  srsran_polar_encoder_t polar_encoder;
  srsran_polar_decoder_t polar_decoder;
  srsran_polar_rm_t      polar_rm_tx;
  srsran_polar_rm_t      polar_rm_rx;
  srsran_crc_t           crc;
  srsran_modem_table_t   qpsk;
} srsran_pbch_nr_t;

/**
 * @brief Initialises an NR PBCH object with the provided arguments
 * @param q NR PBCH object
 * @param args Arguments providing the desired configuration
 * @return SRSRAN_SUCCESS if initialization is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_pbch_nr_init(srsran_pbch_nr_t* q, const srsran_pbch_nr_args_t* args);

/**
 * @brief Deallocates an NR PBCH object
 * @param q NR PBCH object
 */
SRSRAN_API void srsran_pbch_nr_free(srsran_pbch_nr_t* q);

/**
 * @brief Encodes an NR PBCH message into a SSB resource grid
 * @param q NR PBCH object
 * @param cfg NR PBCH configuration
 * @param msg NR PBCH message to transmit
 * @param[out] ssb_grid SSB resource grid
 * @return SRSRAN_SUCCESS if encoding is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_pbch_nr_encode(srsran_pbch_nr_t*           q,
                                     const srsran_pbch_nr_cfg_t* cfg,
                                     const srsran_pbch_msg_nr_t* msg,
                                     cf_t                        ssb_grid[SRSRAN_SSB_NOF_RE]);

/**
 * @brief Decodes an NR PBCH message in the SSB resource grid
 * @param q NR PBCH object
 * @param cfg NR PBCH configuration
 * @param[in] ce Channel estimates for the SSB resource grid
 * @param msg NR PBCH message received
 * @return SRSRAN_SUCCESS if decoding is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_pbch_nr_decode(srsran_pbch_nr_t*           q,
                                     const srsran_pbch_nr_cfg_t* cfg,
                                     const cf_t                  ssb_grid[SRSRAN_SSB_NOF_RE],
                                     const cf_t                  ce[SRSRAN_SSB_NOF_RE],
                                     srsran_pbch_msg_nr_t*       msg);

#endif // SRSRAN_PBCH_NR_H
