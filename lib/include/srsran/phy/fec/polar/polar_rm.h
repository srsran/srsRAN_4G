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
 * \file polar_rm.h
 * \brief Declaration of the polar RateMatcher and RateDematcher.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_POLARRM_H
#define SRSRAN_POLARRM_H

/*!
 * \brief Describes a polar rate matcher or rate dematcher
 */
typedef struct SRSRAN_API {
  void* ptr; /*!< \brief Rate Matcher auxiliary registers. */
} srsran_polar_rm_t;

/*!
 * Initializes the Rate Matcher for the maximum rate-matched codeword length
 * \param[out] q           A pointer to a srsran_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_tx_init(srsran_polar_rm_t* q);

/*!
 * Carries out the actual rate-matching.
 * \param[in] q            A pointer to the Rate-Matcher (a srsran_polar_rm_t structure
 *                         instance) that carries out the rate matching.
 * \param[in] input        The codeword obtained from the polar encoder.
 * \param[out] output      The rate-matched codeword resulting from the rate-matching
 *                         operation.
 * \param[in] n            \f$log_2\f$ of the codeword length.
 * \param[in] E            Rate-matched codeword length.
 * \param[in] K            Message size (including CRC).
 * \param[in] ibil         Indicator of bit interliaver (set to 0 to disable).
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_tx(srsran_polar_rm_t* q,
                                  const uint8_t*     input,
                                  uint8_t*           output,
                                  const uint8_t      n,
                                  const uint32_t     E,
                                  const uint32_t     K,
                                  const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables.
 * \param[out] q           A pointer to a srsran_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_init_f(srsran_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching.
 * \param[in] q          A pointer to the Rate-DeMatcher (a srsran_polar_rm_t structure
 *                       instance) that carries out the rate matching.
 * \param[in] input      The LLRs obtained from the channel samples that correspond to
 *                       the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation.
 * \param[in] E          Rate-matched codeword length.
 * \param[in] n          \f$log_2\f$ of the codeword length.
 * \param[in] K          Message size (including CRC).
 * \param[in] ibil       Indicator of bit interliaver (set to 0 to disable).
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_f(srsran_polar_rm_t* q,
                                    const float*       input,
                                    float*             output,
                                    const uint32_t     E,
                                    const uint8_t      n,
                                    const uint32_t     K,
                                    const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables (int16_t inputs).
 * \param[out] q           A pointer to a srsran_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_init_s(srsran_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching (in16_t inputs)
 * \param[in] q          A pointer to the Rate-DeMatcher (a srsran_polar_rm_t structure
 *                       instance) that carries out the rate matching.
 * \param[in] input      The LLRs obtained from the channel samples that correspond to
 *                       the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation.
 * \param[in] E          Rate-matched codeword length.
 * \param[in] n          \f$log_2\f$ of the codeword length.
 * \param[in] K          Message size (including CRC).
 * \param[in] ibil       Indicator of bit interliaver (set to 0 to disable).
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_s(srsran_polar_rm_t* q,
                                    const int16_t*     input,
                                    int16_t*           output,
                                    const uint32_t     E,
                                    const uint8_t      n,
                                    const uint32_t     K,
                                    const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables (int8_t inputs).
 * \param[out] q           A pointer to a srsran_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_init_c(srsran_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching (int8_t inputs).
 * \param[in] q          A pointer to the Rate-DeMatcher (a srsran_polar_rm_t structure
 *                       instance) that carries out the rate matching.
 * \param[in] input      The LLRs obtained from the channel samples that correspond to
 *                       the codeword to be first, rate-dematched and then decoded.
 * \param[out] output    The rate-dematched codeword resulting from the rate-dematching
 *                       operation.
 * \param[in] E          Rate-matched codeword length.
 * \param[in] n          \f$log_2\f$ of the codeword length.
 * \param[in] K          Message size (including CRC).
 * \param[in] ibil       Indicator of bit interliaver (set to 0 to disable).
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_rm_rx_c(srsran_polar_rm_t* q,
                                    const int8_t*      input,
                                    int8_t*            output,
                                    const uint32_t     E,
                                    const uint8_t      n,
                                    const uint32_t     K,
                                    const uint8_t      ibil);

/*!
 * The Rate Matcher "destructor": it frees all the resources allocated to the rate-matcher.
 * \param[in] q A pointer to the dismantled rate-matcher.
 */
SRSRAN_API void srsran_polar_rm_tx_free(srsran_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor": it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSRAN_API void srsran_polar_rm_rx_free_f(srsran_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor" for short symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSRAN_API void srsran_polar_rm_rx_free_s(srsran_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor" for int8_t symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSRAN_API void srsran_polar_rm_rx_free_c(srsran_polar_rm_t* q);

#endif // SRSRAN_POLARRM_H
