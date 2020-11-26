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
 * \file subchannel_allocation.h
 * \brief Declaration of the auxiliary subchannel allocation block.
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * These functions are not fully functional nor tested to be 3gpp-5G compliant.
 * Please, use only for testing purposes.
 *
 */

#ifndef SRSLTE_SUB_CHANNEL_ALLOC_H
#define SRSLTE_SUB_CHANNEL_ALLOC_H

#include "srslte/config.h"
#include "stdint.h"

/*!
 * \brief Describes a subchannel allocation.
 */
typedef struct SRSLTE_API srslte_subchn_alloc_t {
  uint16_t  code_size;    /*!< \brief Number of bits, \f$N\f$, of the encoder input/output vector. */
  uint16_t  message_size; /*!< \brief Number of bits, \f$K\f$, of data + CRC. */
  uint16_t* message_set; /*!< \brief Pointer to the indices of the encoder input vector containing data and CRC bits. */
} srslte_subchn_alloc_t;

/*!
 * Initializes a subchannel allocation instance.
 * \param[out] c  A pointer to the  srslte_subchn_alloc_t structure
 *     containing the parameters needed by the subchannel allocation function.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
 * \param[in] message_set_size Number of data + CRC bits.
 * \param[in] message_set Pointer to the indices of the encoder input vector containing
 * data and CRC bits.
 */
void srslte_subchannel_allocation_init(srslte_subchn_alloc_t* c,
                                       uint8_t                code_size_log,
                                       uint16_t               message_set_size,
                                       uint16_t*              message_set);

/*!
 * Allocates message bits (data + CRC) to the encoder input bit vector at the
 * positions specified in \a c->message_set and zeros to the remaining
 * positions. This function is not fully 5G compliant as parity bits positions
 * are set to 0.
 * \param[in] c A pointer to the srslte_subchn_alloc_t structure containing
 *     the parameters needed by the subchannel allocation function.
 * \param[in] message A pointer to the vector with the message bits (data and CRC).
 * \param[out] input_encoder A pointer to the encoder input bit vector.
 */
void srslte_subchannel_allocation(const srslte_subchn_alloc_t* c, const uint8_t* message, uint8_t* input_encoder);

/*!
 * Extracts message bits (data + CRC) from the decoder output vector
 * according to the positions specified in  \a c->message_set.
 * \param[in] c A pointer to the srslte_subchn_alloc_t structure containing the
 *     parameters needed by the subchannel allocation function.
 * \param[in] output_decoder A pointer to the decoder output bit vector.
 * \param[out] message A pointer to the vector with the message bits (data and CRC).
 */
void srslte_subchannel_deallocation(const srslte_subchn_alloc_t* c, const uint8_t* output_decoder, uint8_t* message);

#endif // SRSLTE_SUB_CHANNEL_ALLOC_H
