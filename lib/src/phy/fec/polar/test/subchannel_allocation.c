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
 * \file subchannel_allocation.c
 * \brief Defiition of the auxiliary subchannel allocation block.
 * \author Jesus Gomez (CTTC)
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

void srslte_subchannel_allocation_init(srslte_subchn_alloc_t* c,
                                       const uint8_t          code_size_log,
                                       const uint16_t         message_set_size,
                                       uint16_t*              message_set)
{
  c->code_size    = 1U << code_size_log;
  c->message_size = message_set_size;
  c->message_set  = message_set;
}

void srslte_subchannel_allocation(const srslte_subchn_alloc_t* c, const uint8_t* message, uint8_t* input_encoder)
{
  memset(input_encoder, 0, c->code_size * sizeof(uint8_t));

  uint16_t i_o = 0;
  for (uint16_t i = 0; i < c->message_size; i++) {
    i_o                = c->message_set[i];
    input_encoder[i_o] = message[i];
  }
}

void srslte_subchannel_deallocation(const srslte_subchn_alloc_t* c, const uint8_t* output_decoder, uint8_t* message)
{
  uint16_t i_o = 0;
  for (uint16_t i = 0; i < c->message_size; i++) {
    i_o        = c->message_set[i];
    message[i] = output_decoder[i_o];
  }
}
