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

/*!
 * \file ldpc_enc_all.h
 * \brief Declaration of the LDPC encoder inner functions.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSLTE_LDPCENC_ALL_H
#define SRSLTE_LDPCENC_ALL_H
#include "srslte/phy/fec/ldpc/ldpc_encoder.h"

/*! Computes the product between the first (K - 2) columns of the PCM and the systematic bits.
 * \param[in,out] q     A pointer to an encoder.
 * \param[in]     input The message to encode.
 */
void preprocess_systematic_bits(srslte_ldpc_encoder_t* q, const uint8_t* input);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}.
 * \param[in]  o      A pointer to an encoder.
 * \param[out] output The resulting codeword.
 */
void encode_high_rate_case1(void* o, uint8_t* output);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6}.
 * \param[in]  o      A pointer to an encoder.
 * \param[out] output The resulting codeword.
 */
void encode_high_rate_case2(void* o, uint8_t* output);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6}.
 * \param[in]  o      A pointer to an encoder.
 * \param[out] output The resulting codeword.
 */
void encode_high_rate_case3(void* o, uint8_t* output);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7}.
 * \param[in]  o      A pointer to an encoder.
 * \param[out] output The resulting codeword.
 */
void encode_high_rate_case4(void* o, uint8_t* output);

/*! Computes the extended-region parity bits.
 * \param[in]  q      A pointer to an encoder.
 * \param[out] output The resulting codeword.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region(srslte_ldpc_encoder_t* q, uint8_t* output, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx2(srslte_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx2(void* p);

/*!
 * Loads the message in the opimized encoder registers (LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx2(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx2(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx2(srslte_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx2(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS <= \ref
 * SRSLTE_AVX2_B_SIZE). \param[in,out]  q  A pointer to an encoder.
 */
void encode_high_rate_case2_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS <= \ref
 * SRSLTE_AVX2_B_SIZE). \param[in,out]  q  A pointer to an encoder.
 */
void encode_high_rate_case3_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS <= \ref
 * SRSLTE_AVX2_B_SIZE). \param[in,out]  q  A pointer to an encoder.
 */
void encode_high_rate_case4_avx2(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS <= \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx2(srslte_ldpc_encoder_t* q, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (for LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx2long(srslte_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx2long(void* p);

/*!
 * Loads the message in the optimized encoder registers (LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx2long(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx2long(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx2long(srslte_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx2long(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS > \ref
 * SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx2long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS > \ref
 * SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx2long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS > \ref
 * SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx2long(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS > \ref SRSLTE_AVX2_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx2long(srslte_ldpc_encoder_t* q, uint8_t n_layers);

#endif // SRSLTE_LDPCENC_ALL_H
