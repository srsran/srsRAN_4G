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
 * \file polar_sets.h
 * \brief Declaration of the auxiliary function that reads polar index sets from a file.
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * The message and parity check sets provided by this functions are needed by
 * the subchannel allocation block.
 * The frozen bit set provided by this function is used by the polar decoder.
 *
 */

#ifndef SRSLTE_POLAR_SETS_H
#define SRSLTE_POLAR_SETS_H

#include "srslte/config.h"
#include <stdint.h>

/*!
 * \brief Describes a polar set.
 */
typedef struct {
  uint16_t  message_set_size; /*!< \brief Number of message bits (data and CRC). */
  uint16_t  info_set_size;    /*!< \brief Number of message bits plus parity bits. */
  uint16_t  parity_set_size;  /*!< \brief Number of parity check bits. */
  uint16_t  frozen_set_size;  /*!< \brief Number of frozen bits. */
  uint16_t* message_set; /*!< \brief Pointer to the indices of the encoder input vector containing data and CRC bits. */
  uint16_t* info_set;    /*!< \brief Pointer to the indices of the encoder input vector containing data, CRC and
                       parity check bits.*/
  uint16_t* parity_set;  /*!< \brief Pointer to the indices of the encoder input vector containing the parity bits.*/
  uint16_t* frozen_set;  /*!< \brief Pointer to the indices of the encoder input vector containing frozen bits.*/
} srslte_polar_sets_t;

/*!
 * Initializes the different index sets as needed by the subchannel allocation block and/or by the polar decoder.
 * \param[out] c A pointer to the initialized polar set.
 * \param[in] message_size Number of data + CRC bits.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
 * \param[in] rate_matching_size Number of bits of the codeword after rate matching.
 * \param[in] parity_set_size Number of parity bits.
 * \param[in] nWmPC Number of parity bits of minimum weight type.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int srslte_polar_code_sets_read(srslte_polar_sets_t* c,
                                uint16_t             message_size,
                                uint8_t              code_size_log,
                                uint16_t             rate_matching_size,
                                uint8_t              parity_set_size,
                                uint8_t              nWmPC);

/*!
 * The polar set "destructor": it frees all the resources.
 * \param[in] c A pointer to the dismantled polar set.
 */
void srslte_polar_code_sets_free(srslte_polar_sets_t* c);

#endif // SRSLTE_POLAR_SETS_H
