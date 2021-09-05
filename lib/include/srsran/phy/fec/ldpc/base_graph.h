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

/*!
 * \file base_graph.h
 * \brief Declaration of the two LDPC base graphs employed in the 5G NR
 * standard.
 * \author David Gregoratti
 * \date 2020
 *
 * This file declares the dimensions of the base graphs and provides an interface
 * for obtaining the set index and the permutation matrix corresponding to a
 * given *lifting size*.
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_BASEGRAPH_H
#define SRSRAN_BASEGRAPH_H

#include "srsran/config.h"

#include <stdint.h>

#define SRSRAN_LDPC_BG1_MAX_LEN_CB 8448                   /*!< \brief Maximum code block size for LDPC BG1 */
#define SRSRAN_LDPC_BG2_MAX_LEN_CB 3840                   /*!< \brief Maximum code block size for LDPC BG2 */
#define SRSRAN_LDPC_MAX_LEN_CB SRSRAN_LDPC_BG1_MAX_LEN_CB /*!< \brief Maximum code block size for LDPC BG1 or BG2 */

#define BG1Nfull 68 /*!< \brief Number of variable nodes in BG1. */
#define BG1N 66     /*!< \brief Number of variable nodes in BG1 after puncturing. */
#define BG1M 46     /*!< \brief Number of check nodes in BG1. */
#define BG1K 22     /*!< \brief Number of "uncoded bits" in BG1. */

#define BG2Nfull 52 /*!< \brief Number of variable nodes in BG2. */
#define BG2N 50     /*!< \brief Number of variable nodes in BG2 after puncturing. */
#define BG2M 42     /*!< \brief Number of check nodes in BG2. */
#define BG2K 10     /*!< \brief Number of "uncoded bits" in BG2. */

#define MAX_CNCT 20 /*!< \brief Maximum number (+1) of connected variables per check node. */

#define NOF_LIFTSIZE 8 /*!< \brief Number of possible lifting size indices. */

#define MAX_LIFTSIZE 384 /*!< \brief Maximum lifting size. */

#define SRSRAN_LDPC_MAX_LEN_ENCODED_CB (MAX_LIFTSIZE * SRSRAN_MAX(BG1N, BG2N))

#define VOID_LIFTSIZE 255 /*!< \brief Identifies an invalid lifting size in the lookup table. */
/*!
 * \brief Identifies a missing connection between a check node and a variable node
 * in the protograph. */
#define NO_CNCT 0xFFFF

/*! \brief Possible base graphs, BG1 or BG2. */
typedef enum SRSRAN_API {
  BG1 = 0, /*!< \brief Base Graph 1 */
  BG2,     /*!< \brief Base Graph 2 */
} srsran_basegraph_t;

/*!
 * Creates the parity-check matrix for the given base graph and lifting size
 * in the compact form (a normalized permutation matrix). Also returns the
 * indices of the variable nodes associated to each check node.
 * \param[out] pcm       The compact parity-check matrix: entry \f$(m,n)\f$ is an
 *                       integer between 0 and LS-1 if check-node \f$m\f$ is
 *                       connected to variable node \f$n\f$ in the protograph.
 *                       This number specifies the order of the circular
 *                       rotation applied to the identity matrix in the full
 *                       graph (see also Section 3.4.1 of Deliverable 1). This
 *                       pointer can be safely cast to 'uint16_t(*)[BGbgNfull]'
 *                       (see also ::BG1Nfull and ::BG2Nfull) to get an BGbgM x
 *                       BGbgNfull matrix.
 * \param[out] positions For each check node, the corresponding row of this
 *                       matrix contains the indices of the connected variable
 *                       nodes (see also ::BG1_positions and ::BG2_positions).
 * \param[in]  bg        The desired base graph (BG1 or BG2).
 * \param[in]  ls        The desired lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int create_compact_pcm(uint16_t* pcm, int8_t (*positions)[MAX_CNCT], srsran_basegraph_t bg, uint16_t ls);

/*!
 * Reads the lookup table and returns the set index corresponding to the given
 * lifting size.
 * \param[in] ls A lifting size.
 * \return An integer between 0 and 7 (included), ::VOID_LIFTSIZE if ls is an
 * invalid lifting size
 */
static inline uint8_t get_ls_index(uint16_t ls)
{
  extern const uint8_t LSindex[];
  return (ls <= MAX_LIFTSIZE ? LSindex[ls] : VOID_LIFTSIZE);
}

#endif // SRSRAN_BASEGRAPH_H
