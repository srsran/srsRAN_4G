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

/*!
 * \file ldpc_enc_all.h
 * \brief Declaration of the LDPC encoder inner functions.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_LDPCENC_ALL_H
#define SRSRAN_LDPCENC_ALL_H
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"

/*! Computes the product between the first (K - 2) columns of the PCM and the systematic bits.
 * \param[in,out] q     A pointer to an encoder.
 * \param[in]     input The message to encode.
 */
void preprocess_systematic_bits(srsran_ldpc_encoder_t* q, const uint8_t* input);

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
void encode_ext_region(srsran_ldpc_encoder_t* q, uint8_t* output, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx2(srsran_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx2(void* p);

/*!
 * Loads the message in the opimized encoder registers (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx2(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx2(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx2(srsran_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx2(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in,out] o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx2(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx2(srsran_ldpc_encoder_t* q, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (for LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx2long(srsran_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx2long(void* p);

/*!
 * Loads the message in the optimized encoder registers (LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx2long(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx2long(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx2long(srsran_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx2long(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS > \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx2long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS > \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx2long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS > \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx2long(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx2long(srsran_ldpc_encoder_t* q, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx2(srsran_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx2(void* p);

/*!
 * Loads the message in the opimized encoder registers (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx2(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx2(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx2(srsran_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx2(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx2(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE).
 \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx2(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx2(srsran_ldpc_encoder_t* q, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (for LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx512long(srsran_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx512long(void* p);

/*!
 * Loads the message in the optimized encoder registers (LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx512long(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx512long(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx512long(srsran_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS > \ref SRSRAN_avx512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx512long(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS > \ref
 * SRSRAN_avx512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx512long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS > \ref
 * SRSRAN_avx512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx512long(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS > \ref
 * SRSRAN_avx512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx512long(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx512long(srsran_ldpc_encoder_t* q, uint8_t n_layers);

/*!
 * Creates the inner registers required by the optimized LDPC encoder (for LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] q A pointer to an encoder.
 * \return A pointer to the newly created structure of registers.
 */
void* create_ldpc_enc_avx512(srsran_ldpc_encoder_t* q);

/*!
 * Deletes the inner registers of an optimized LDPC encoder (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in] p A pointer to the register structure.
 */
void delete_ldpc_enc_avx512(void* p);

/*!
 * Loads the message in the optimized encoder registers (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in] p        The register structure.
 * \param[in] input    The message to encode.
 * \param[in] msg_len  Number of variable nodes in one message.
 * \param[in] cdwd_len Number of variable nodes in one message.
 * \param[in] ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int load_avx512(void* p, const uint8_t* input, uint8_t msg_len, uint8_t cdwd_len, uint16_t ls);

/*! Extracts the final codeword from the optimized encoder registers (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in]  p        The register structure.
 * \param[out] output   The output codeword.
 * \param[in]  cdwd_len The number of variable nodes (after rate-matching, if enabled).
 * \param[in]  ls       The lifting size.
 * \return Error code: 0 if correct, -1 otherwise.
 */
int return_codeword_avx512(void* p, uint8_t* output, uint8_t cdwd_len, uint16_t ls);

/*! Computes the product between the first (K - 2) columns of the PCM and the
 * systematic bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] q     A pointer to an encoder.
 */
void preprocess_systematic_bits_avx512(srsran_ldpc_encoder_t* q);

/*! Computes the high-rate parity bits for BG1 and ls_index in {0, 1, 2, 3, 4, 5, 7}
 * (SIMD-optimized version, LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case1_avx512(void* o);

/*! Computes the high-rate parity bits for BG1 and ls_index in {6} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case2_avx512(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {0, 1, 2, 4, 5, 6} (SIMD-optimized version, LS > \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case3_avx512(void* o);

/*! Computes the high-rate parity bits for BG2 and ls_index in {3, 7} (SIMD-optimized version, LS <= \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  o  A pointer to an encoder.
 */
void encode_high_rate_case4_avx512(void* o);

/*! Computes the extended-region parity bits (SIMD-optimized version, LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out]  q      A pointer to an encoder.
 * \param[in]  n_layers The number of layers to process (when doing rate matching not all
 *                       layers are needed).
 */
void encode_ext_region_avx512(srsran_ldpc_encoder_t* q, uint8_t n_layers);

#endif // SRSRAN_LDPCENC_ALL_H
