/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/*
 * Adapted from ETSI/SAGE specifications:
 * "Specification of the 3GPP Confidentiality and
 * Integrity Algorithms UEA2 & UIA2.
 * Document 1: UEA2 and UIA2 Specification"
 * "Specification of the 3GPP Confidentiality
 * and Integrity Algorithms UEA2 & UIA2.
 * Document 2: SNOW 3G Specification"
 */

#ifndef SRSLTE_S3G_H
#define SRSLTE_S3G_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t* lfsr;
  uint32_t* fsm;
} S3G_STATE;

/* Initialization.
 * Input k[4]: Four 32-bit words making up 128-bit key.
 * Input IV[4]: Four 32-bit words making 128-bit initialization variable.
 * Output: All the LFSRs and FSM are initialized for key generation.
 * See Section 4.1.
 */

void s3g_initialize(S3G_STATE* state, uint32_t k[4], uint32_t iv[4]);

/*********************************************************************
    Name: s3g_deinitialize

    Description: Deinitialization.

    Document Reference: Specification of the 3GPP Confidentiality and
                            Integrity Algorithms UEA2 & UIA2 D2 v1.1
*********************************************************************/
void s3g_deinitialize(S3G_STATE* state);

/* Generation of Keystream.
 * input n: number of 32-bit words of keystream.
 * input z: space for the generated keystream, assumes
 * memory is allocated already.
 * output: generated keystream which is filled in z
 * See section 4.2.
 */

void s3g_generate_keystream(S3G_STATE* state, uint32_t n, uint32_t* ks);

/* f8.
 * Input key: 128 bit Confidentiality Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input bearer: 5-bit Bearer identity (in the LSB side).
 * Input dir:1 bit, direction of transmission.
 * Input data: length number of bits, input bit stream.
 * Input length: 32 bit Length, i.e., the number of bits to be encrypted or
 * decrypted.
 * Output data: Output bit stream. Assumes data is suitably memory
 * allocated.
 * Encrypts/decrypts blocks of data between 1 and 2^32 bits in length as
 * defined in Section 3.
 */

// void snow3g_f8(u8* key, u32 count, u32 bearer, u32 dir, u8* data, u32 length);

/* f9.
 * Input key: 128 bit Integrity Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input fresh: 32-bit Random number.
 * Input dir:1 bit, direction of transmission (in the LSB).
 * Input data: length number of bits, input bit stream.
 * Input length: 64 bit Length, i.e., the number of bits to be MAC'd.
 * Output : 32 bit block used as MAC
 * Generates 32-bit MAC using UIA2 algorithm as defined in Section 4.
 */

uint8_t* s3g_f9(const uint8_t* key, uint32_t count, uint32_t fresh, uint32_t dir, uint8_t* data, uint64_t length);

#endif // SRSLTE_S3G_H