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
 * \file polar_code.c
 * \brief Definition of the function that computes the polar code parameters including,
 * message set (K_Set), the frozen set (F_set), and the parity check bits set (PC_Set)..
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * The message and parity check sets provided by this functions are needed by
 * the subchannel allocation block.
 * The frozen bit set provided by this function is used by the polar decoder.
 */

#include "srsran/phy/utils/vector.h"
#include <stdint.h>
#include <stdlib.h>

#include "srsran/phy/fec/polar/polar_code.h"
#include "srsran/phy/utils/debug.h"

/*!
 * Extracts the elements in x that are smaller than T or are in y.
 * Returns the length of the output vector z.
 */
static uint16_t
setdiff_stable(const uint16_t* x, const uint16_t* y, uint16_t* z, const int T, const uint16_t len1, const uint16_t len2)
{
  uint16_t o    = 0;
  int      flag = 0;
  for (int i = 0; i < len1; i++) {
    // is x[i] in y ?
    flag = 0;
    if (x[i] <= T) {
      flag = 1;
    } else {
      for (int j = 0; j < len2; j++) {
        if (x[i] == y[j]) {
          flag = 1;
          break;
        }
      }
    }

    if (flag == 0) {
      z[o] = x[i];
      o++;
    }
  }
  return o;
}

/*!
 * Compares two uint16_t
 */
int cmpfunc(const void* a, const void* b)
{
  const uint16_t ai = *(const uint16_t*)a;
  const uint16_t bi = *(const uint16_t*)b;

  if (ai < bi) {
    return -1;
  }
  if (ai > bi) {
    return 1;
  }

  return 0;
}

/*!
 * Gets the codeword length N, nPC and nWmPC depending on the code parameters.
 * Returns -1 if not supported configuration, otherwise returns 0.
 */
int get_code_params(srsran_polar_code_t* c, const uint16_t K, const uint16_t E, const uint8_t nMax)
{
  // include here also npc and nwmPC computatoins
  if (E > EMAX) {
    ERROR("Rate-matched codeword size (E) not supported. Chose E<=8192");
    return -1;
  }
  switch (nMax) {
    case 9: // downlink
      // iil = true
      if (K < 36 || K > 164) {
        ERROR("Codeblock length (K=%d) not supported for downlink transmission, choose 165 > K > 35", K);
        return -1;
      }
      break;
    case 10:
      // iil = false
      if (K < 18 || (K > 25 && K < 31) || K > 1023) {
        ERROR("Codeblock length (K=%d) not supported for uplink transmission, choose K > 17 and K < 1024, "
              "excluding 31 > K > 25",
              K);
        return -1;
      }
      break;
    default:
      ERROR("nMax not supported choose 9 for downlink  and 10 for uplink transmissions");
      return -1;
  }
  // number of parity check bits (nPC) and parity check bits of minimum bandwidth nWmPC
  uint8_t nPC   = 0;
  uint8_t nWmPC = 0;
  if (K <= 25) {
    nPC = 3;
    if (E > K + 189) {
      nWmPC = 1;
    }
  }

  if (K + nPC >= E) {
    ERROR(" Rate-matched codeword length (E=%d) not supported, choose E > %d", E, K + nPC);
    return -1;
  }

  // determination of the codeword size (N)
  // ceil(log2(E))
  uint16_t tmpE = 0;
  uint8_t  e    = 1;
  for (; e <= eMAX; e++) {
    tmpE = 1U << e; // 2^e
    if (tmpE >= E) {
      break;
    }
  }

  uint8_t n1  = 0;
  uint8_t e_1 = e - 1;
  if ((8 * E <= 9 * (1U << e_1)) && (16 * K < 9 * E)) {
    n1 = e - 1;
  } else {
    n1 = e;
  }

  // ceil(log2(K))
  uint16_t tmpK = 0;
  uint8_t  k    = 0;
  for (; k <= 10; k++) {
    tmpK = 1U << k; // 2^e
    if (tmpK >= K) {
      break;
    }
  }
  uint8_t n2 = k + 3;

  // min(n1, n2, n3)
  uint8_t n = n1;
  if (n2 < n1) {
    n = n2;
  }
  if (nMax < n) {
    n = nMax;
  }
  if (n < 5) {
    n = 5;
  }
  uint16_t N = (1U << n);

  if (K >= N) {
    ERROR("Codeblock length (K) not supported, choose K < N");
    return -1;
  }

  c->N     = N;
  c->n     = n;
  c->nPC   = nPC;
  c->nWmPC = nWmPC;
  return 0;
}

void srsran_polar_code_free(srsran_polar_code_t* c)
{
  if (c != NULL) {
    free(c->F_set);
    free(c->tmp_K_set); // also removes K_set
  }
}

// allocate resources to the message set, frozen set and parity set, polar code
int srsran_polar_code_init(srsran_polar_code_t* c)
{
  c->tmp_K_set = srsran_vec_u16_malloc(NMAX + 1); // +1 to mark the end with 1024
  if (!c->tmp_K_set) {
    perror("malloc");
    exit(-1);
  }

  c->F_set = srsran_vec_u16_malloc(NMAX);
  if (!c->F_set) {
    free(c->tmp_K_set);
    perror("malloc");
    exit(-1);
  }

  return 0;
}

int srsran_polar_code_get(srsran_polar_code_t* c, uint16_t K, uint16_t E, uint8_t nMax)
{
  if (c == NULL) {
    return -1;
  }
  // check polar code parameters
  if (get_code_params(c, K, E, nMax) == -1) {
    return -1;
  }
  uint8_t  nPC   = c->nPC;
  uint8_t  nWmPC = c->nWmPC;
  uint8_t  n     = c->n;
  uint16_t N     = c->N;

  const uint16_t* blk_interleaver = get_blk_interleaver(n);
  const uint16_t* mother_code     = get_mother_code(n);

  c->F_set_size = N - K - nPC;
  c->K          = K;

  // Frozen bits due to Puncturing and Shortening.
  int T              = -1;
  int tmp_F_set_size = N - E;
  int N_th           = 3 * N / 4;

  if (tmp_F_set_size > 0) {
    if (16 * K <= 7 * E) { // Puncturing
      if (E >= N_th) {
        T = N_th - (E >> 1U) - 1;
      } else {
        T = 9 * N / 16 - (E >> 2U);
      }
      memcpy(c->F_set,
             blk_interleaver,
             tmp_F_set_size * sizeof(uint16_t)); // The first (less reliable) after interleaving

    } else { // Shortening
      memcpy(c->F_set,
             blk_interleaver + E,
             tmp_F_set_size * sizeof(uint16_t)); // The first (less reliable) after interleaving
    }
  } else {
    tmp_F_set_size = 0;
  }

  int tmp_K = setdiff_stable(mother_code, c->F_set, c->tmp_K_set, T, N, tmp_F_set_size);

  // Select only the most reliable (message and parity)
  c->K_set = c->tmp_K_set + tmp_K - K - nPC;

  // take the nPC - nWmPC less reliable
  for (int i = 0; i < nPC - nWmPC; i++) {
    c->PC_set[i] = c->K_set[i];
  }

  // This only happens if K=18:25 and E=E+189+1:8192
  // In this cases there is no puncturing or shortening
  if (nWmPC == 1) {
    if (K <= 21) {
      c->PC_set[nPC - 1] = 252;
    } else {
      c->PC_set[nPC - 1] = 248;
    }
  }

  // sorted K_set (includes parity bits)
  qsort(c->K_set, c->K + c->nPC, sizeof(uint16_t), &cmpfunc);
  // sorted PC_set
  if (nPC > 0) {
    qsort(c->PC_set, nPC, sizeof(uint16_t), &cmpfunc);
  }

  // create the sorted frozen set as the complement of sorted K_set
  uint16_t i_k    = 0;
  uint16_t fvalue = 0;
  uint16_t i_f    = 0;
  while (i_f < c->F_set_size) {
    if (c->K_set[i_k] != fvalue) {
      c->F_set[i_f] = fvalue;
      i_f++;
    } else if (i_k < K + nPC - 1) {
      i_k++; // skip
    }
    fvalue++;
  }

  // mark the end of the sets (useful at subchannel allocation)
  c->K_set[c->K + c->nPC] = 1024;
  c->PC_set[c->nPC]       = 1024;

  return 0;
}
