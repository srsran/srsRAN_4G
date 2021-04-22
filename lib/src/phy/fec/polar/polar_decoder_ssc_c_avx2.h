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
 * \file polar_decoder_ssc_c_avx2.h
 * \brief Declaration of the SSC polar decoder inner functions working with
 * 8-bit integer-valued LLRs and AVX2 instructions
 * \author Jesus Gomez \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_DECODER_SSC_C_AVX2_H
#define POLAR_DECODER_SSC_C_AVX2_H

#include "polar_decoder_ssc_all.h"

/*!
 * Creates an SSC polar decoder structure of type pSSC_c_avx2, and allocates memory for the decoding buffers.
 *
 * \param[in] nMax \f$log_2\f$ of the number of bits in the codeword.
 * \return A pointer to a pSSC_c_avx2 structure if the function executes correctly, NULL otherwise.
 */
void* create_polar_decoder_ssc_c_avx2(uint8_t nMax);

/*!
 * The (8-bit, avx2) polar decoder SSC "destructor": it frees all the resources allocated to the decoder.
 *
 * \param[in, out] p A pointer to the dismantled decoder.
 */
void delete_polar_decoder_ssc_c_avx2(void* p);

/*!
 * Initializes an (8-bit, avx2) SSC polar decoder before processing a new codeword.
 *
 * \param[in, out] p A void pointer used to declare a pSSC_c_avx2 structure.
 * \param[in] llr LLRs for the new codeword.
 * \param[out] data_decoded Pointer to the decoded message.
 * \param[in] code_size_log \f$log_2\f$ of the number of bits in the codeword.
 * \param[in] frozen_set The position of the frozen bits in the codeword.
 * \param[in] frozen_set_size Number of frozen bits.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int init_polar_decoder_ssc_c_avx2(void*           p,
                                  const int8_t*   llr,
                                  uint8_t*        data_decoded,
                                  const uint8_t   code_size_log,
                                  const uint16_t* frozen_set,
                                  const uint16_t  frozen_set_size);

/*!
 * Decodes a data message from a 8 bit resolution codeword with the specified decoder. Note that
 * a pointer to the codeword LLRs is included in \a p and initialized by init_polar_decoder_ssc_c_avx2().
 *
 * \param[in] p A pointer to the desired decoder.
 * \param[out] data The decoded message.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int polar_decoder_ssc_c_avx2(void* p, uint8_t* data);

#endif // POLAR_DECODER_SSC_C_AVX2_H
