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
 * \file subchannel_allocation.c
 * \brief Defiition of the auxiliary subchannel allocation block.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * These functions are not fully functional nor tested to be 3gpp-5G compliant.
 * Please, use only for testing purposes.
 *
 */

#include "subchannel_allocation.h"
#include <string.h> //memset

void srsran_subchannel_allocation_init(srsran_subchn_alloc_t* c,
                                       const uint8_t          code_size_log,
                                       const uint16_t         message_set_size,
                                       uint16_t*              message_set)
{
  c->code_size    = 1U << code_size_log;
  c->message_size = message_set_size;
  c->message_set  = message_set;
}

void srsran_subchannel_allocation(const srsran_subchn_alloc_t* c, const uint8_t* message, uint8_t* input_encoder)
{
  memset(input_encoder, 0, c->code_size * sizeof(uint8_t));

  uint16_t i_o = 0;
  for (uint16_t i = 0; i < c->message_size; i++) {
    i_o                = c->message_set[i];
    input_encoder[i_o] = message[i];
  }
}

void srsran_subchannel_deallocation(const srsran_subchn_alloc_t* c, const uint8_t* output_decoder, uint8_t* message)
{
  uint16_t i_o = 0;
  for (uint16_t i = 0; i < c->message_size; i++) {
    i_o        = c->message_set[i];
    message[i] = output_decoder[i_o];
  }
}
