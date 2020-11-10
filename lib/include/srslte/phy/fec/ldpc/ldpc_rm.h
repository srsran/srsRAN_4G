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

/*!
 * \file ldpc_rm.h
 * \brief Declaration of the LDPC RateMatcher and RateDematcher.
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSLTE_LDPCRM_H
#define SRSLTE_LDPCRM_H

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/ldpc/base_graph.h"

/*!
 * \brief Describes a rate matcher or rate dematcher (K, F are ignored at rate matcher)
 */
typedef struct SRSLTE_API {
  void*              ptr;       /*!< \brief %Rate Matcher auxiliary registers. */
  srslte_basegraph_t bg;        /*!< \brief Current base graph. */
  uint16_t           ls;        /*!< \brief Current lifting size. */
  uint32_t           N;         /*!< \brief Codeword size. */
  uint32_t           E;         /*!< \brief Rate-Matched codeword size. */
  uint32_t           K;         /*!< \brief Codeblock size (including punctured and filler bits). */
  uint32_t           F;         /*!< \brief Number of filler bits in the codeblock. */
  uint32_t           k0;        /*!< \brief Starting position in the circular buffer. */
  uint32_t           mod_order; /*!< \brief Modulation order. */
  uint32_t           Ncb;       /*!< \brief Limit to the number of bits in the circular buffer. */
} srslte_ldpc_rm_t;

/*!
 * Initializes the Rate Matcher for the maximum rate-matched codeword length
 * \param[out] q           A pointer to a srslte_ldpc_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_tx_init(srslte_ldpc_rm_t* q);

/*!
 * Carries out the actual rate-matching.
 * \param[in] q            A pointer to the Rate-Matcher (a srslte_ldpc_rm_t structure
 *                         instance) that carries out the rate matching.
 * \param[in] input        The codeword obtained from the ldpc encoder.
 * \param[out] output      The rate-matched codeword resulting from the rate-matching
 *                         operation.
 * \param[in]  E           Rate-matched codeword length.
 * \param[in]  bg;         Current base graph.
 * \param[in]  ls          Current lifting size.
 * \param[in]  rv          Redundancy version 0,1,2,3.
 * \param[in]  mod_type    Modulation type.
 * \param[in]  Nref        Size of limited buffer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_tx(srslte_ldpc_rm_t*        q,
                                 const uint8_t*           input,
                                 uint8_t*                 output,
                                 const uint32_t           E,
                                 const srslte_basegraph_t bg,
                                 const uint32_t           ls,
                                 const uint8_t            rv,
                                 const srslte_mod_t       mod_type,
                                 const uint32_t           Nref);

/*!
 * Initializes all the Rate DeMatcher variables.
 * \param[out] q           A pointer to a srslte_ldpc_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_rx_init_f(srslte_ldpc_rm_t* q);

/*!
 * Carries out the actual rate-dematching.
 * \param[in] q          A pointer to the Rate-DeMatcher (a srslte_ldpc_rm_t structure
 *                        instance) that carries out the rate matching.
 * \param[in] input      The LLRs obtained from the channel samples that correspond to
 *                       the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation. Shall be either initialized to all zeros or to the
 *                       result of previous redundancy versions is available.
 * \param[in] E          Rate-matched codeword length.
 * \param[in] F          Number of filler bits.
 * \param[in] bg;        Current base graph.
 * \param[in] ls         Current lifting size.
 * \param[in] rv         Redundancy version 0,1,2,3.
 * \param[in] mod_type   Modulation type.
 * \param[in] Nref        Size of limited buffer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_rx_f(srslte_ldpc_rm_t*        q,
                                   const float*             input,
                                   float*                   output,
                                   const uint32_t           E,
                                   const uint32_t           F,
                                   const srslte_basegraph_t bg,
                                   const uint32_t           ls,
                                   const uint8_t            rv,
                                   const srslte_mod_t       mod_type,
                                   const uint32_t           Nref);

/*!
 * Initializes all the Rate DeMatcher variables (short inputs).
 * \param[out] q           A pointer to a srslte_ldpc_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_rx_init_s(srslte_ldpc_rm_t* q);

/*!
 * Carries out the actual rate-dematching (short symbols).
 * \param[in] q           A pointer to the Rate-DeMatcher (a srslte_ldpc_rm_t structure
 *                        instance) that carries out the rate matching.
 * \param[in] input       The LLRs obtained from the channel samples that correspond to
 *                        the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation. Shall be either initialized to all zeros or to the
 *                       result of previous redundancy versions is available.
 * \param[in] E           Rate-matched codeword length.
 * \param[in] F           Number of filler bits.
 * \param[in] bg;         Current base graph.
 * \param[in] ls          Current lifting size.
 * \param[in] rv          Redundancy version 0,1,2,3.
 * \param[in] mod_type    Modulation type.
 * \param[in] Nref        Size of limited buffer.
 * \param[out] output The rate-dematched codeword resulting from the rate-dematching
 *    operation.
 */
SRSLTE_API int srslte_ldpc_rm_rx_s(srslte_ldpc_rm_t*        q,
                                   const int16_t*           input,
                                   int16_t*                 output,
                                   const uint32_t           E,
                                   const uint32_t           F,
                                   const srslte_basegraph_t bg,
                                   const uint32_t           ls,
                                   const uint8_t            rv,
                                   const srslte_mod_t       mod_type,
                                   const uint32_t           Nref);

/*!
 * Initializes all the Rate DeMatcher variables (char inputs).
 * \param[out] q           A pointer to a srslte_ldpc_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_rx_init_c(srslte_ldpc_rm_t* q);

/*!
 * Carries out the actual rate-dematching (int8_t symbols).
 * \param[in] q           A pointer to the Rate-DeMatcher (a srslte_ldpc_rm_t structure
 *                        instance) that carries out the rate matching.
 * \param[in] input       The LLRs obtained from the channel samples that correspond to
 *                        the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation. Shall be either initialized to all zeros or to the
 *                       result of previous redundancy versions is available.
 * \param[in] E           Rate-matched codeword length.
 * \param[in] F           Number of filler bits.
 * \param[in] bg;         Current base graph.
 * \param[in] ls          Current lifting size.
 * \param[in] rv          Redundancy version 0,1,2,3.
 * \param[in] mod_type    Modulation type.
 * \param[in] Nref        Size of limited buffer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_ldpc_rm_rx_c(srslte_ldpc_rm_t*        q,
                                   const int8_t*            input,
                                   int8_t*                  output,
                                   const uint32_t           E,
                                   const uint32_t           F,
                                   const srslte_basegraph_t bg,
                                   const uint32_t           ls,
                                   const uint8_t            rv,
                                   const srslte_mod_t       mod_type,
                                   const uint32_t           Nref);

/*!
 * The Rate Matcher "destructor": it frees all the resources allocated to the rate-matcher.
 * \param[in] q A pointer to the dismantled rate-matcher.
 */
SRSLTE_API void srslte_ldpc_rm_tx_free(srslte_ldpc_rm_t* q);

/*!
 * The Rate Matcher "destructor": it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_ldpc_rm_rx_free_f(srslte_ldpc_rm_t* q);

/*!
 * The Rate Matcher "destructor" for short symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_ldpc_rm_rx_free_s(srslte_ldpc_rm_t* q);

/*!
 * The Rate Matcher "destructor" for int8_t symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_ldpc_rm_rx_free_c(srslte_ldpc_rm_t* q);

#endif // SRSLTE_LDPCENCODER_H
