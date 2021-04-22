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
 * \file polar_decoder_ssc_all.h
 * \brief Declaration of the SSC polar decoder functions common to all implementations
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_DECODER_SSC_ALL_H
#define POLAR_DECODER_SSC_ALL_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!
 * \brief Types of node in an SSC decoder.
 */
typedef enum {
  RATE_0 = 0, /*!< \brief See function rate_0_node(). */
  RATE_R = 2, /*!< \brief See function rate_r_node(). */
  RATE_1 = 3, /*!< \brief See function rate_1_node(). */
} node_rate;

/*!
 * \brief Stores constants.
 */
struct Params {
  uint8_t   code_size_log;   /*!< \brief \f$log_2\f$ of code size. */
  uint16_t* code_stage_size; /*!< \brief Number of bits of the encoder input/output vector at a given stage. */
  uint16_t  frozen_set_size; /*!< \brief Number of frozen bits. */
  uint8_t** node_type;       /*!< \brief Node type indicator 1 at all stages 3 (rate-1), 2 (rate-r), 0 (rate-0). */
};

/*!
 * \brief Describes the state of a SSC polar decoder
 */
struct State {
  uint8_t stage;         /*!< \brief Current stage [0 - code_size_log] of the decoding algorithm. */
  bool    flag_finished; /*!< \brief True if the last bit is decoded. False otherwise. */
  uint16_t*
      active_node_per_stage; /*!< \brief Indicates the active node in each stage of the algorithm at a given moment. */
};

/*!
 * Computes node types and initializes struct Params.
 * \param[in] frozen_set The position of the frozen bits in the codeword.
 * \param[in, out] param A struct Params
 */
int init_node_type(const uint16_t* frozen_set, struct Params* param);

/*!
 * Computes node types for the decoding tree associated to the given frozen set.
 * \param[in] p Pointer of a Tmp_node_type structure with the memory resources needed.
 * \param[out] node_type Double pointer containing the node type at each stage of the decoding tree.
 * \param[in] code_size_log \f$log_2\f$ of code size.
 * \param[in] frozen_set The position of the frozen bits in the codeword.
 * \param[in] frozen_set_size The size of the frozen set.
 */
int compute_node_type(void*           p,
                      uint8_t**       node_type,
                      const uint16_t* frozen_set,
                      const uint16_t  code_size_log,
                      const uint16_t  frozen_set_size);

/*!
 * The "destructor" of the memory resources used to compute the node types.
 * \param[in, out] p A pointer to the dismantled resources..
 */
void delete_tmp_node_type(void* p);

/*!
 * Allocates memory resources for the computation of the node_type.
 * \param[in] nMax \f$log_2\f$ of the maximum number of bits in the codeword.
 * \return A pointer to a Tmp_node_type structure if the function executes correctly, NULL otherwise.
 */
void* create_tmp_node_type(const uint8_t nMax);

#endif // polar_decoder_SSC_ALL_H
