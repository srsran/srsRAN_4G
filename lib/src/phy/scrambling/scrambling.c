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

#include "srsran/phy/scrambling/scrambling.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/vector.h"
#include <assert.h>

void srsran_scrambling_f(srsran_sequence_t* s, float* data)
{
  srsran_scrambling_f_offset(s, data, 0, s->cur_len);
}

void srsran_scrambling_f_offset(srsran_sequence_t* s, float* data, int offset, int len)
{
  assert(len + offset <= s->cur_len);
  srsran_vec_prod_fff(data, &s->c_float[offset], data, len);
}

void srsran_scrambling_s(srsran_sequence_t* s, short* data)
{
  srsran_scrambling_s_offset(s, data, 0, s->cur_len);
}

void srsran_scrambling_s_offset(srsran_sequence_t* s, short* data, int offset, int len)
{
  assert(len + offset <= s->cur_len);
  srsran_vec_neg_sss(data, &s->c_short[offset], data, len);
}

void srsran_scrambling_sb_offset(srsran_sequence_t* s, int8_t* data, int offset, int len)
{
  assert(len + offset <= s->cur_len);
  srsran_vec_neg_bbb(data, &s->c_char[offset], data, len);
}

void srsran_scrambling_c(srsran_sequence_t* s, cf_t* data)
{
  srsran_scrambling_c_offset(s, data, 0, s->cur_len);
}

void srsran_scrambling_c_offset(srsran_sequence_t* s, cf_t* data, int offset, int len)
{
  assert(len + offset <= s->cur_len);
  srsran_vec_prod_cfc(data, &s->c_float[offset], data, len);
}

static inline void scrambling_b(uint8_t* c, uint8_t* data, int len)
{
  srsran_vec_xor_bbb(c, data, data, len);
}

void srsran_scrambling_b(srsran_sequence_t* s, uint8_t* data)
{
  srsran_scrambling_b_offset(s, data, 0, s->cur_len);
}

void srsran_scrambling_b_offset(srsran_sequence_t* s, uint8_t* data, int offset, int len)
{
  scrambling_b(&s->c[offset], data, len);
}

void srsran_scrambling_bytes(srsran_sequence_t* s, uint8_t* data, int len)
{
  scrambling_b(s->c_bytes, data, len / 8);
  // Scramble last bits
  if (len % 8) {
    uint8_t tmp_bits[8];
    srsran_bit_unpack_vector(&data[len / 8], tmp_bits, len % 8);
    scrambling_b(&s->c[8 * (len / 8)], tmp_bits, len % 8);
    srsran_bit_pack_vector(tmp_bits, &data[len / 8], len % 8);
  }
}
