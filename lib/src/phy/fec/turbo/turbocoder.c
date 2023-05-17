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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srsran/phy/fec/cbsegm.h"
#include "srsran/phy/fec/turbo/turbocoder.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define NOF_REGS 3

#define RATE 3
#define TOTALTAIL 12

typedef struct {
  uint8_t next_state;
  uint8_t output;
} tcod_lut_t;

static tcod_lut_t               tcod_lut[8][256];
static uint16_t                 tcod_per_fw[188][6144];
static srsran_bit_interleaver_t tcod_interleavers[188];

static bool table_initiated = false;

int srsran_tcod_init(srsran_tcod_t* h, uint32_t max_long_cb)
{
  h->max_long_cb = max_long_cb;
  h->temp        = srsran_vec_malloc(max_long_cb / 8);

  if (!table_initiated) {
    table_initiated = true;
    srsran_tcod_gentable();
  }
  return 0;
}

void srsran_tcod_free(srsran_tcod_t* h)
{
  h->max_long_cb = 0;
  if (h->temp) {
    free(h->temp);
  }

  if (table_initiated) {
    for (int i = 0; i < 188; i++) {
      srsran_bit_interleaver_free(&tcod_interleavers[i]);
    }
    table_initiated = false;
  }
}

/* Expects bits (1 byte = 1 bit) and produces bits. The systematic and parity bits are interlaced in the output */
int srsran_tcod_encode(srsran_tcod_t* h, uint8_t* input, uint8_t* output, uint32_t long_cb)
{
  uint8_t   reg1_0, reg1_1, reg1_2, reg2_0, reg2_1, reg2_2;
  uint32_t  i, k = 0, j;
  uint8_t   bit;
  uint8_t   in, out;
  uint16_t* per;

  if (long_cb > h->max_long_cb) {
    ERROR("Turbo coder initiated for max_long_cb=%d", h->max_long_cb);
    return -1;
  }

  int longcb_idx = srsran_cbsegm_cbindex(long_cb);
  if (longcb_idx < 0) {
    ERROR("Invalid CB size %d", long_cb);
    return -1;
  }

  per = tcod_per_fw[longcb_idx];

  reg1_0 = 0;
  reg1_1 = 0;
  reg1_2 = 0;

  reg2_0 = 0;
  reg2_1 = 0;
  reg2_2 = 0;

  k = 0;
  for (i = 0; i < long_cb; i++) {
    if (input[i] == SRSRAN_TX_NULL) {
      bit = 0;
    } else {
      bit = input[i];
    }
    output[k] = input[i];

    k++;

    in  = bit ^ (reg1_2 ^ reg1_1);
    out = reg1_2 ^ (reg1_0 ^ in);

    reg1_2 = reg1_1;
    reg1_1 = reg1_0;
    reg1_0 = in;

    if (input[i] == SRSRAN_TX_NULL) {
      output[k] = SRSRAN_TX_NULL;
    } else {
      output[k] = out;
    }
    k++;

    bit = input[per[i]];
    if (bit == SRSRAN_TX_NULL) {
      bit = 0;
    }

    in  = bit ^ (reg2_2 ^ reg2_1);
    out = reg2_2 ^ (reg2_0 ^ in);

    reg2_2 = reg2_1;
    reg2_1 = reg2_0;
    reg2_0 = in;

    output[k] = out;
    k++;
  }

  k = 3 * long_cb;

  /* TAILING CODER #1 */
  for (j = 0; j < NOF_REGS; j++) {
    bit = reg1_2 ^ reg1_1;

    output[k] = bit;
    k++;

    in  = bit ^ (reg1_2 ^ reg1_1);
    out = reg1_2 ^ (reg1_0 ^ in);

    reg1_2 = reg1_1;
    reg1_1 = reg1_0;
    reg1_0 = in;

    output[k] = out;
    k++;
  }

  /* TAILING CODER #2 */
  for (j = 0; j < NOF_REGS; j++) {
    bit = reg2_2 ^ reg2_1;

    output[k] = bit;
    k++;

    in  = bit ^ (reg2_2 ^ reg2_1);
    out = reg2_2 ^ (reg2_0 ^ in);

    reg2_2 = reg2_1;
    reg2_1 = reg2_0;
    reg2_0 = in;

    output[k] = out;
    k++;
  }
  return 0;
}

/* Expects bytes and produces bytes. The systematic and parity bits are interlaced in the output */
int srsran_tcod_encode_lut(srsran_tcod_t* h,
                           srsran_crc_t*  crc_tb,
                           srsran_crc_t*  crc_cb,
                           uint8_t*       input,
                           uint8_t*       parity,
                           uint32_t       cblen_idx,
                           bool           last_cb)
{
  if (cblen_idx < 188) {
    uint32_t long_cb = (uint32_t)srsran_cbsegm_cbsize(cblen_idx);

    if (long_cb % 8) {
      ERROR("Turbo coder LUT implementation long_cb must be multiple of 8");
      return -1;
    }

    /* Reset CRC */
    if (crc_cb) {
      srsran_crc_set_init(crc_cb, 0);
    }

    /* Parity bits for the 1st constituent encoders */
    uint8_t state0 = 0;
    if (crc_cb) {
      int block_size_nocrc = (long_cb - crc_cb->order - ((last_cb) ? crc_tb->order : 0)) / 8;

      /* if CRC pointer is given */
      for (int i = 0; i < block_size_nocrc; i++) {
        uint8_t in = input[i];

        /* Put byte in TB CRC and save latest checksum */
        srsran_crc_checksum_put_byte(crc_tb, in);

        /* Put byte in CB CRC and save latest checksum */
        srsran_crc_checksum_put_byte(crc_cb, in);

        /* Run actual encoder */
        tcod_lut_t l = tcod_lut[state0][in];
        parity[i]    = l.output;
        state0       = l.next_state;
      }

      if (last_cb) {
        uint32_t checksum = (uint32_t)srsran_crc_checksum_get(crc_tb);
        for (int i = 0; i < crc_tb->order / 8; i++) {
          int     mask_shift = 8 * (crc_tb->order / 8 - i - 1);
          int     idx        = block_size_nocrc + i;
          uint8_t in         = (uint8_t)((checksum >> mask_shift) & 0xff);

          /* Put byte in CB CRC and save latest checksum */
          srsran_crc_checksum_put_byte(crc_cb, in);

          input[idx]   = in;
          tcod_lut_t l = tcod_lut[state0][in];
          parity[idx]  = l.output;
          state0       = l.next_state;
        }
      }

      uint32_t checksum = (uint32_t)srsran_crc_checksum_get(crc_cb);
      for (int i = 0; i < crc_cb->order / 8; i++) {
        int     mask_shift = 8 * (crc_cb->order / 8 - i - 1);
        int     idx        = (long_cb - crc_cb->order) / 8 + i;
        uint8_t in         = (uint8_t)((checksum >> mask_shift) & 0xff);

        input[idx]   = in;
        tcod_lut_t l = tcod_lut[state0][in];
        parity[idx]  = l.output;
        state0       = l.next_state;
      }

    } else {
      /* No CRC given */
      int block_size_nocrc = (long_cb - ((last_cb) ? crc_tb->order : 0)) / 8;

      for (uint32_t i = 0; i < block_size_nocrc; i++) {
        uint8_t in = input[i];

        srsran_crc_checksum_put_byte(crc_tb, in);

        tcod_lut_t l = tcod_lut[state0][in];
        parity[i]    = l.output;
        state0       = l.next_state;
      }

      if (last_cb) {
        uint32_t checksum = (uint32_t)srsran_crc_checksum_get(crc_tb);
        for (int i = 0; i < crc_tb->order / 8; i++) {
          int     mask_shift = 8 * (crc_tb->order / 8 - i - 1);
          int     idx        = block_size_nocrc + i;
          uint8_t in         = (uint8_t)((checksum >> mask_shift) & 0xff);

          input[idx]   = in;
          tcod_lut_t l = tcod_lut[state0][in];
          parity[idx]  = l.output;
          state0       = l.next_state;
        }
      }
    }
    parity[long_cb / 8] = 0; // will put tail here later

    /* Interleave input */
    srsran_bit_interleaver_run(&tcod_interleavers[cblen_idx], input, h->temp, 0);
    // srsran_bit_interleave(input, h->temp, tcod_per_fw[cblen_idx], long_cb);

    /* Parity bits for the 2nd constituent encoders */
    uint8_t state1 = 0;
    for (uint32_t i = 0; i < long_cb / 8; i++) {
      tcod_lut_t l   = tcod_lut[state1][h->temp[i]];
      uint8_t    out = l.output;
      parity[long_cb / 8 + i] |= (out & 0xf0) >> 4;
      parity[long_cb / 8 + i + 1] = (out & 0xf) << 4;
      state1                      = l.next_state;
    }

    /* Tail bits */
    uint8_t reg1_0, reg1_1, reg1_2, reg2_0, reg2_1, reg2_2;
    uint8_t bit, in, out;
    uint8_t k = 0;
    uint8_t tail[12];

    reg2_0 = (state1 & 4) >> 2;
    reg2_1 = (state1 & 2) >> 1;
    reg2_2 = state1 & 1;

    reg1_0 = (state0 & 4) >> 2;
    reg1_1 = (state0 & 2) >> 1;
    reg1_2 = state0 & 1;

    /* TAILING CODER #1 */
    for (uint32_t j = 0; j < NOF_REGS; j++) {
      bit = reg1_2 ^ reg1_1;

      tail[k] = bit;
      k++;

      in  = bit ^ (reg1_2 ^ reg1_1);
      out = reg1_2 ^ (reg1_0 ^ in);

      reg1_2 = reg1_1;
      reg1_1 = reg1_0;
      reg1_0 = in;

      tail[k] = out;
      k++;
    }

    /* TAILING CODER #2 */
    for (uint32_t j = 0; j < NOF_REGS; j++) {
      bit = reg2_2 ^ reg2_1;

      tail[k] = bit;
      k++;

      in  = bit ^ (reg2_2 ^ reg2_1);
      out = reg2_2 ^ (reg2_0 ^ in);

      reg2_2 = reg2_1;
      reg2_1 = reg2_0;
      reg2_0 = in;

      tail[k] = out;
      k++;
    }

    uint8_t tailv[3][4];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        tailv[j][i] = tail[3 * i + j];
      }
    }
    uint8_t* x         = tailv[0];
    input[long_cb / 8] = (srsran_bit_pack(&x, 4) << 4);
    x                  = tailv[1];
    parity[long_cb / 8] |= (srsran_bit_pack(&x, 4) << 4);
    x = tailv[2];
    parity[2 * long_cb / 8] |= (srsran_bit_pack(&x, 4) & 0xf);

    return 3 * long_cb + TOTALTAIL;
  } else {
    return -1;
  }
}

void srsran_tcod_gentable()
{
  srsran_tc_interl_t interl;

  if (srsran_tc_interl_init(&interl, 6144)) {
    ERROR("Error initiating interleave");
    return;
  }

  for (uint32_t len = 0; len < 188; len++) {
    uint32_t long_cb = srsran_cbsegm_cbsize(len);
    if (srsran_tc_interl_LTE_gen(&interl, long_cb)) {
      ERROR("Error initiating TC interleaver for long_cb=%d", long_cb);
      return;
    }
    // Save fw/bw permutation tables
    for (uint32_t i = 0; i < long_cb; i++) {
      tcod_per_fw[len][i] = interl.forward[i];
    }
    srsran_bit_interleaver_init(&tcod_interleavers[len], tcod_per_fw[len], long_cb);
    for (uint32_t i = long_cb; i < 6144; i++) {
      tcod_per_fw[len][i] = 0;
    }
  }
  // Compute state transitions
  for (uint32_t state = 0; state < 8; state++) {
    for (uint32_t data = 0; data < 256; data++) {
      uint8_t reg_0, reg_1, reg_2;
      reg_0 = (state & 4) >> 2;
      reg_1 = (state & 2) >> 1;
      reg_2 = state & 1;

      tcod_lut[state][data].output = 0;
      uint8_t bit, in, out;
      for (uint32_t i = 0; i < 8; i++) {
        bit = (data & (1 << (7 - i))) ? 1 : 0;

        in  = bit ^ (reg_2 ^ reg_1);
        out = reg_2 ^ (reg_0 ^ in);

        reg_2 = reg_1;
        reg_1 = reg_0;
        reg_0 = in;

        tcod_lut[state][data].output |= out << (7 - i);
      }
      tcod_lut[state][data].next_state = (uint8_t)((reg_0 << 2 | reg_1 << 1 | reg_2) % 8);
    }
  }

  srsran_tc_interl_free(&interl);
}
