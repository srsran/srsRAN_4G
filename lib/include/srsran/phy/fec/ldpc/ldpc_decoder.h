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

/*!
 * \file ldpc_decoder.h
 * \brief Declaration of the LDPC decoder.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_LDPCDECODER_H
#define SRSRAN_LDPCDECODER_H

#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/ldpc/base_graph.h"

/*!
 * \brief Types of LDPC decoder.
 */
typedef enum {
  SRSRAN_LDPC_DECODER_F = 0,        /*!< \brief %Decoder working with real-valued LLRs. */
  SRSRAN_LDPC_DECODER_S,            /*!< \brief %Decoder working with 16-bit integer-valued LLRs. */
  SRSRAN_LDPC_DECODER_C,            /*!< \brief %Decoder working with 8-bit integer-valued LLRs. */
  SRSRAN_LDPC_DECODER_C_FLOOD,      /*!< \brief %Decoder working with 8-bit integer-valued LLRs, flooded scheduling. */
  SRSRAN_LDPC_DECODER_C_AVX2,       /*!< \brief %Decoder working with 8-bit integer-valued LLRs (AVX2 version). */
  SRSRAN_LDPC_DECODER_C_AVX2_FLOOD, /*!< \brief %Decoder working with 8-bit integer-valued LLRs, flooded scheduling
                                   (AVX2 version). */
  SRSRAN_LDPC_DECODER_C_AVX512,     /*!< \brief %Decoder working with 8-bit integer-valued LLRs (AVX512 version). */
  SRSRAN_LDPC_DECODER_C_AVX512_FLOOD, /*!< \brief %Decoder working with 8-bit integer-valued LLRs, flooded scheduling
                                   (AVX512 version). */
} srsran_ldpc_decoder_type_t;

/*!
 * \brief Describes the LDPC decoder configuration arguments.
 */
typedef struct {
  srsran_ldpc_decoder_type_t type;         /*!< \brief Type of LDPC decoder. */
  srsran_basegraph_t         bg;           /*!< \brief The desired base graph (BG1 or BG2). */
  uint16_t                   ls;           /*!< \brief The desired lifting size. */
  float                      scaling_fctr; /*!< \brief Scaling factor of the normalized min-sum algorithm.*/
  uint32_t                   max_nof_iter; /*!< \brief Maximum number of iterations, set to 0 for default value. */
} srsran_ldpc_decoder_args_t;

/*!
 * \brief Describes an LDPC decoder.
 */
typedef struct SRSRAN_API {
  void*              ptr;          /*!< \brief Registers used by the decoder. */
  srsran_basegraph_t bg;           /*!< \brief Current base graph. */
  uint16_t           ls;           /*!< \brief Current lifting size. */
  uint32_t           max_nof_iter; /*!< \brief Maximum number of iterations. */
  uint8_t            bgN;          /*!< \brief Number of variable nodes in the BG. */
  uint16_t           liftN;        /*!< \brief Number of variable nodes in the lifted graph. */
  uint8_t            bgM;          /*!< \brief Number of check nodes in the BG. */
  uint16_t           liftM;        /*!< \brief Number of check nodes in the lifted graph. */
  uint8_t            bgK;          /*!< \brief Number of "uncoded bits" in the BG. */
  uint16_t           liftK;        /*!< \brief Number of uncoded bits in the lifted graph. */
  uint16_t*          pcm;          /*!< \brief Pointer to the parity check matrix (compact form). */

  int8_t (*var_indices)[MAX_CNCT]; /*!< \brief Pointer to lists of variable indices connected to a given check node. */

  float scaling_fctr; /*!< \brief Scaling factor for the normalized min-sum algorithm. */

  void (*free)(void*); /*!< \brief Pointer to a "destructor". */

  int (*decode_f)(void*,
                  const float*,
                  uint8_t*,
                  uint32_t,
                  srsran_crc_t*); /*!< \brief Pointer to the decoding function (float version). */
  int (*decode_s)(void*,
                  const int16_t*,
                  uint8_t*,
                  uint32_t,
                  srsran_crc_t*); /*!< \brief Pointer to the decoding function (16-bit version). */
  int (*decode_c)(void*,
                  const int8_t*,
                  uint8_t*,
                  uint32_t,
                  srsran_crc_t*); /*!< \brief Pointer to the decoding function (16-bit version). */
} srsran_ldpc_decoder_t;

/*!
 * Initializes all the LDPC decoder variables according to the given base graph
 * and lifting size.
 * \param[out] q            A pointer to a srsran_ldpc_decoder_t structure.
 * \param[in]  args         LDPC configuration arguments.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_ldpc_decoder_init(srsran_ldpc_decoder_t* q, const srsran_ldpc_decoder_args_t* args);

/*!
 * The LDPC decoder "destructor": it frees all the resources allocated to the decoder.
 * \param[in] q A pointer to the dismantled decoder.
 */
SRSRAN_API void srsran_ldpc_decoder_free(srsran_ldpc_decoder_t* q);

/*!
 * Carries out the actual decoding with real-valued LLRs.
 * \param[in] q A pointer to the LDPC decoder (a srsran_ldpc_decoder_t structure
 *    instance) that carries out the decoding.
 * \param[in] llrs The LLRs obtained from the channel samples that correspond to
 *    the codeword to be decoded.
 * \param[out] message The message (uncoded bits) resulting from the decoding
 *    operation.
 * \param[in] cdwd_rm_length The number of bits forming the codeword (after rate matching).
 */
SRSRAN_API int
srsran_ldpc_decoder_decode_f(srsran_ldpc_decoder_t* q, const float* llrs, uint8_t* message, uint32_t cdwd_rm_length);

/*!
 * Carries out the actual decoding with 16-bit integer-valued LLRs. It is
 * recommended to use a 15-bit representation for the LLRs, given that all
 * values exceeding \f$ 2^{15}-1 \f$ (in magnitude) will be considered as infinity.
 * \param[in] q A pointer to the LDPC decoder (a srsran_ldpc_decoder_t structure
 *    instance) that carries out the decoding.
 * \param[in] llrs The LLRs obtained from the channel samples that correspond to
 *    the codeword to be decoded.
 * \param[out] message The message (uncoded bits) resulting from the decoding
 *    operation.
 * \param[in] cdwd_rm_length The number of bits forming the codeword (after rate matching).
 */
SRSRAN_API int
srsran_ldpc_decoder_decode_s(srsran_ldpc_decoder_t* q, const int16_t* llrs, uint8_t* message, uint32_t cdwd_rm_length);

/*!
 * Carries out the actual decoding with 8-bit integer-valued LLRs. It is
 * recommended to use a 7-bit representation for the LLRs, given that all
 * values exceeding \f$ 2^{7}-1 \f$ (in magnitude) will be considered as infinity.
 * \param[in] q A pointer to the LDPC decoder (a srsran_ldpc_decoder_t structure
 *    instance) that carries out the decoding.
 * \param[in] llrs The LLRs obtained from the channel samples that correspond to
 *    the codeword to be decoded.
 * \param[out] message The message (uncoded bits) resulting from the decoding
 *    operation.
 * \param[in] cdwd_rm_length The number of bits forming the codeword (after rate matching).
 */
SRSRAN_API int
srsran_ldpc_decoder_decode_c(srsran_ldpc_decoder_t* q, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length);

/*!
 * Carries out the actual decoding with 8-bit integer-valued LLRs. It is
 * recommended to use a 7-bit representation for the LLRs, given that all
 * values exceeding \f$ 2^{7}-1 \f$ (in magnitude) will be considered as infinity.
 * \param[in] q A pointer to the LDPC decoder (a srsran_ldpc_decoder_t structure
 *    instance) that carries out the decoding.
 * \param[in] llrs The LLRs obtained from the channel samples that correspond to
 *    the codeword to be decoded.
 * \param[out] message The message (uncoded bits) resulting from the decoding
 *    operation.
 * \param[in] cdwd_rm_length The number of bits forming the codeword (after rate matching).
 * \param[in,out] crc Code-block CRC object for early stop. Set for NULL to disable check
 * \return -1 if an error occurred, the number of used iterations, and 0 if CRC is provided and did not match
 */
SRSRAN_API int srsran_ldpc_decoder_decode_crc_c(srsran_ldpc_decoder_t* q,
                                                const int8_t*          llrs,
                                                uint8_t*               message,
                                                uint32_t               cdwd_rm_length,
                                                srsran_crc_t*          crc);

#endif // SRSRAN_LDPCDECODER_H
