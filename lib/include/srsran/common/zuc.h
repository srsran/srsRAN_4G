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

#ifndef SRSRAN_ZUC_H
#define SRSRAN_ZUC_H

typedef unsigned char u8;
typedef unsigned int  u32;

/* the state registers of LFSR */
typedef struct {
  u32 LFSR_S0;
  u32 LFSR_S1;
  u32 LFSR_S2;
  u32 LFSR_S3;
  u32 LFSR_S4;
  u32 LFSR_S5;
  u32 LFSR_S6;
  u32 LFSR_S7;
  u32 LFSR_S8;
  u32 LFSR_S9;
  u32 LFSR_S10;
  u32 LFSR_S11;
  u32 LFSR_S12;
  u32 LFSR_S13;
  u32 LFSR_S14;
  u32 LFSR_S15;
  /* the registers of F */
  u32 F_R1;
  u32 F_R2;
  /* the outputs of BitReorganization */
  u32 BRC_X0;
  u32 BRC_X1;
  u32 BRC_X2;
  u32 BRC_X3;
} zuc_state_t;

void zuc_initialize(zuc_state_t* state, const u8* k, u8* iv);
void zuc_generate_keystream(zuc_state_t* state, int key_stream_len, u32* p_keystream);

#endif // SRSRAN_ZUC_H
