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
 * \file ldpc_dec_all.h
 * \brief Declaration of the LDPC decoder inner functions.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_LDPCDEC_ALL_H
#define SRSRAN_LDPCDEC_ALL_H

#include <srsran/phy/fec/ldpc/base_graph.h>
#include <stdint.h>

/*!
 * Creates the registers used by the float-based implementation of the LDPC decoder.
 * \param[in] bgN Codeword length.
 * \param[in] bgM Number of check nodes.
 * \param[in] ls  Lifting size.
 * \param[in]  scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs structure).
 */
void* create_ldpc_dec_f(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the float-based LDPC decoder.
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs structure).
 */
void delete_ldpc_dec_f(void* p);

/*!
 * Initializes the inner registers of the float-based LDPC decoder before
 * carrying out the actual decoding.
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_f(void* p, const float* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (float version).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_f(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (float version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_f(void*           p,
                               int             i_layer,
                               const uint16_t* this_pcm,
                               const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (float version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_f(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits.
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_f(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the 16-bit-based implementation of the LDPC decoder.
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_s structure).
 */
void* create_ldpc_dec_s(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the 16-bit integer-based LDPC decoder.
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_s structure).
 */
void delete_ldpc_dec_s(void* p);

/*!
 * Initializes the inner registers of the 16-bit integer-based LDPC decoder before
 * carrying out the actual decoding.
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_s structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_s(void* p, const int16_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (16-bit version).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_s structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_s(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (16-bit version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_s structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_s(void*           p,
                               int             i_layer,
                               const uint16_t* this_pcm,
                               const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (16-bit version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_s structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_s(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits.
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_s structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_s(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the 8-bit-based implementation of the LDPC decoder.
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c structure).
 */
void* create_ldpc_dec_c(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the 8-bit integer-based LDPC decoder.
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c structure).
 */
void delete_ldpc_dec_c(void* p);

/*!
 * Initializes the inner registers of the 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding.
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (8-bit version).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (8-bit version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c(void*           p,
                               int             i_layer,
                               const uint16_t* this_pcm,
                               const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (8-bit version).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits.
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the 8-bit-based implementation of the LDPC decoder (flooded scheduling).
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_flood structure).
 */
void* create_ldpc_dec_c_flood(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the 8-bit integer-based LDPC decoder (flooded scheduling).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_flood structure).
 */
void delete_ldpc_dec_c_flood(void* p);

/*!
 * Initializes the inner registers of the 8-bit integer-based LDPC decoder (flooded scheduling) before
 * carrying out the actual decoding.
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_flood structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_flood(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (8-bit version, flooded scheduling).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_flood structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_flood(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (8-bit version, flooded scheduling).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_flood structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_flood(void*           p,
                                     int             i_layer,
                                     const uint16_t* this_pcm,
                                     const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (8-bit version, flooded scheduling).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_flood structure).
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to each layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_flood(void* p, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits.
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_flood structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_flood(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder (LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in] bgN          Codeword length. \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size. \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx2 structure).
 */
void* create_ldpc_dec_c_avx2(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx2 structure).
 */
void delete_ldpc_dec_c_avx2(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx2 structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx2(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx2 structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx2(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2 structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx2(void*           p,
                                    int             i_layer,
                                    const uint16_t* this_pcm,
                                    const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2 structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx2(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version, LS <= \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx2 structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx2(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder (LS > \ref
 * SRSRAN_AVX2_B_SIZE).
 * \param[in] bgN          Codeword length. \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size. \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx2long structure).
 */
void* create_ldpc_dec_c_avx2long(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx2long structure).
 */
void delete_ldpc_dec_c_avx2long(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx2long structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx2long(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx2long structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx2long(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2long structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx2long(void*           p,
                                        int             i_layer,
                                        const uint16_t* this_pcm,
                                        const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version, LS > \ref
 * SRSRAN_AVX2_B_SIZE). \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2long structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx2long(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version, LS > \ref
 * SRSRAN_AVX2_B_SIZE). \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx2long structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx2long(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder
 * (flooded scheduling, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx2_flood structure).
 */
void* create_ldpc_dec_c_avx2_flood(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder
 * (flooded scheduling, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx2_flood structure).
 */
void delete_ldpc_dec_c_avx2_flood(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (flooded scheduling, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx2_flood structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx2_flood(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx2_flood structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx2_flood(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes
 * (optimized 8-bit version, flooded scheduling, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2_flood structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx2_flood(void*           p,
                                          int             i_layer,
                                          const uint16_t* this_pcm,
                                          const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword
 * (optimized 8-bit version, flooded scheduling, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2_flood structure).
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to each layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx2_flood(void* p, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits
 * (flooded scheduling, optimized 8-bit version, LS <= \ref SRSRAN_AVX2_B_SIZE).
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx2_flood structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx2_flood(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder
 * (flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx2long_flood structure).
 */
void* create_ldpc_dec_c_avx2long_flood(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (flooded scheduling, LS > \ref
 * SRSRAN_AVX2_B_SIZE). \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx2long_flood
 * structure).
 */
void delete_ldpc_dec_c_avx2long_flood(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx2long_flood structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx2long_flood(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx2long_flood structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx2long_flood(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2long_flood structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx2long_flood(void*           p,
                                              int             i_layer,
                                              const uint16_t* this_pcm,
                                              const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx2long_flood structure).
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to each layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx2long_flood(void* p, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX2_B_SIZE).
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx2long_flood structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx2long_flood(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder (LS > \ref
 * SRSRAN_AVX512_B_SIZE). \param[in] bgN          Codeword length. \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size. \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx512long structure).
 */
void* create_ldpc_dec_c_avx512long(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx512long structure).
 */
void delete_ldpc_dec_c_avx512long(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx512long structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx512long(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx512long structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx512long(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512long structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx512long(void*           p,
                                          int             i_layer,
                                          const uint16_t* this_pcm,
                                          const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version, LS > \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512long structure).
 * \param[in] i_layer     The index of the variable-to-check layer to update.
 * \param[in] these_var_indices Contains the indices of the variable nodes connected to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx512long(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version, LS > \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx512long structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx512long(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder (LS <= \ref
 * SRSRAN_AVX512_B_SIZE).
 * \param[in] bgN          Codeword length. \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size. \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx512 structure).
 */
void* create_ldpc_dec_c_avx512(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx512 structure).
 */
void delete_ldpc_dec_c_avx512(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx512 structure).
 * \param[in] llrs    A pointer to the array of LLR values from the channel.
 * \param[in] ls      The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx512(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version, LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx512 structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx512(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version, LS <= \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512 structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx512(void*           p,
                                      int             i_layer,
                                      const uint16_t* this_pcm,
                                      const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version, LS <= \ref
 * SRSRAN_AVX512_B_SIZE). \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512 structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx512(void* p, int i_layer, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version, LS <= \ref
 * SRSRAN_AVX512_B_SIZE). \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx512 structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx512(void* p, uint8_t* message, uint16_t liftK);

/*!
 * Creates the registers used by the optimized 8-bit-based implementation of the LDPC decoder
 * (flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in] bgN          Codeword length.
 * \param[in] bgM          Number of check nodes.
 * \param[in] ls           Lifting size.
 * \param[in] scaling_fctr Scaling factor of the normalized min-sum algorithm.
 * \return A pointer to the created registers (an ldpc_regs_c_avx512long_flood structure).
 */
void* create_ldpc_dec_c_avx512long_flood(uint8_t bgN, uint8_t bgM, uint16_t ls, float scaling_fctr);

/*!
 * Destroys the inner registers of the optimized 8-bit integer-based LDPC decoder (flooded scheduling, LS > \ref
 * SRSRAN_AVX512_B_SIZE). \param[in] p A pointer to the dismantled decoder registers (an ldpc_regs_c_avx512long_flood
 * structure).
 */
void delete_ldpc_dec_c_avx512long_flood(void* p);

/*!
 * Initializes the inner registers of the optimized 8-bit integer-based LDPC decoder before
 * carrying out the actual decoding (flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p    A pointer to the decoder registers (an ldpc_regs_c_avx512long_flood structure).
 * \param[in]     llrs A pointer to the array of LLR values from the channel.
 * \param[in]     ls   The lifting size.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_ldpc_dec_c_avx512long_flood(void* p, const int8_t* llrs, uint16_t ls);

/*!
 * Updates the messages from variable nodes to check nodes (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p       A pointer to the decoder registers (an ldpc_regs_c_avx512long_flood structure).
 * \param[in]     i_layer The index of the variable-to-check layer to update.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_var_to_check_c_avx512long_flood(void* p, int i_layer);

/*!
 * Updates the messages from check nodes to variable nodes (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512long_flood structure).
 * \param[in]     i_layer  The index of the variable-to-check layer to update.
 * \param[in]     this_pcm A pointer to the row of the parity check matrix (i.e. base
 *                         graph) corresponding to the selected layer.
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to the current layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_check_to_var_c_avx512long_flood(void*           p,
                                                int             i_layer,
                                                const uint16_t* this_pcm,
                                                const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Updates the current estimate of the (soft) bits of the codeword (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in,out] p        A pointer to the decoder registers (an ldpc_regs_c_avx512long_flood structure).
 * \param[in]     these_var_indices
 *                         Contains the indices of the variable nodes connected
 *                         to each layer.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int update_ldpc_soft_bits_c_avx512long_flood(void* p, const int8_t (*these_var_indices)[MAX_CNCT]);

/*!
 * Returns the decoded message (hard bits) from the current soft bits (optimized 8-bit version,
 * flooded scheduling, LS > \ref SRSRAN_AVX512_B_SIZE).
 * \param[in]  p       A pointer to the decoder registers (an ldpc_regs_c_avx512long_flood structure).
 * \param[out] message A pointer to the decoded message.
 * \param[in]  liftK   The length of the decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int extract_ldpc_message_c_avx512long_flood(void* p, uint8_t* message, uint16_t liftK);

#endif // SRSRAN_LDPCDEC_ALL_H
