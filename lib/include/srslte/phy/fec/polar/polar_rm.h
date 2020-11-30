/**
 *
 * section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#ifndef SRSLTE_POLARRM_H
#define SRSLTE_POLARRM_H

/*!
 * \brief Describes a polar rate matcher or rate dematcher
 */
typedef struct SRSLTE_API {
  void* ptr; /*!< \brief Rate Matcher auxiliary registers. */
} srslte_polar_rm_t;

/*!
 * Initializes the Rate Matcher for the maximum rate-matched codeword length
 * \param[out] q           A pointer to a srslte_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_rm_tx_init(srslte_polar_rm_t* q);

/*!
 * Carries out the actual rate-matching.
 * \param[in] q            A pointer to the Rate-Matcher (a srslte_polar_rm_t structure
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
SRSLTE_API int srslte_polar_rm_tx(srslte_polar_rm_t* q,
                                  const uint8_t*     input,
                                  uint8_t*           output,
                                  const uint8_t      n,
                                  const uint32_t     E,
                                  const uint32_t     K,
                                  const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables.
 * \param[out] q           A pointer to a srslte_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_rm_rx_init_f(srslte_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching.
 * \param[in] q          A pointer to the Rate-DeMatcher (a srslte_polar_rm_t structure
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
SRSLTE_API int srslte_polar_rm_rx_f(srslte_polar_rm_t* q,
                                    const float*       input,
                                    float*             output,
                                    const uint32_t     E,
                                    const uint8_t      n,
                                    const uint32_t     K,
                                    const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables (int16_t inputs).
 * \param[out] q           A pointer to a srslte_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_rm_rx_init_s(srslte_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching (in16_t inputs)
 * \param[in] q          A pointer to the Rate-DeMatcher (a srslte_polar_rm_t structure
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
SRSLTE_API int srslte_polar_rm_rx_s(srslte_polar_rm_t* q,
                                    const int16_t*     input,
                                    int16_t*           output,
                                    const uint32_t     E,
                                    const uint8_t      n,
                                    const uint32_t     K,
                                    const uint8_t      ibil);

/*!
 * Initializes all the Rate DeMatcher variables (int8_t inputs).
 * \param[out] q           A pointer to a srslte_polar_rm_t structure.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_rm_rx_init_c(srslte_polar_rm_t* q);

/*!
 * Carries out the actual rate-dematching (int8_t inputs).
 * \param[in] q          A pointer to the Rate-DeMatcher (a srslte_polar_rm_t structure
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
SRSLTE_API int srslte_polar_rm_rx_c(srslte_polar_rm_t* q,
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
SRSLTE_API void srslte_polar_rm_tx_free(srslte_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor": it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_polar_rm_rx_free_f(srslte_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor" for short symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_polar_rm_rx_free_s(srslte_polar_rm_t* q);

/*!
 * The Rate Matcher "destructor" for int8_t symbols: it frees all the resources allocated to the rate-dematcher.
 * \param[in] q A pointer to the dismantled rate-dematcher.
 */
SRSLTE_API void srslte_polar_rm_rx_free_c(srslte_polar_rm_t* q);

#endif // SRSLTE_POLARRM_H
