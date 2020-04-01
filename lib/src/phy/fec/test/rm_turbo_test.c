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

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/srslte.h"

uint32_t nof_e_bits = 0;
uint32_t rv_idx     = 0;
uint32_t cb_idx     = 0;

uint8_t systematic[6148], parity[2 * 6148];
uint8_t systematic_bytes[6148 / 8 + 1], parity_bytes[2 * 6148 / 8 + 1];

#define BUFFSZ (6176 * 3)

uint8_t bits[3 * 6144 + 12];
uint8_t buff_b[BUFFSZ];
float   buff_f[BUFFSZ];
float   bits_f[3 * 6144 + 12];
short   bits2_s[3 * 6144 + 12];

void usage(char* prog)
{
  printf("Usage: %s -c cb_idx -e nof_e_bits [-i rv_idx]\n", prog);
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cei")) != -1) {
    switch (opt) {
      case 'c':
        cb_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        nof_e_bits = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'i':
        rv_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (nof_e_bits == 0) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char** argv)
{
  int      i;
  uint8_t *rm_bits, *rm_bits2, *rm_bits2_bytes;
  short*   rm_bits_s;
  float*   rm_bits_f;

  parse_args(argc, argv);

  srslte_rm_turbo_gentables();

  rm_bits_s = srslte_vec_i16_malloc(nof_e_bits);
  if (!rm_bits_s) {
    perror("malloc");
    exit(-1);
  }
  rm_bits_f = srslte_vec_f_malloc(nof_e_bits);
  if (!rm_bits_f) {
    perror("malloc");
    exit(-1);
  }
  rm_bits = srslte_vec_u8_malloc(nof_e_bits);
  if (!rm_bits) {
    perror("malloc");
    exit(-1);
  }
  rm_bits2 = srslte_vec_u8_malloc(nof_e_bits);
  if (!rm_bits2) {
    perror("malloc");
    exit(-1);
  }
  rm_bits2_bytes = srslte_vec_u8_malloc(nof_e_bits / 8 + 1);
  if (!rm_bits2_bytes) {
    perror("malloc");
    exit(-1);
  }

  uint32_t st = 0, end = 188;
  if (cb_idx != -1) {
    st  = cb_idx;
    end = cb_idx + 1;
  }
  uint32_t rv_st = 0, rv_end = 4;
  if (rv_idx != -1) {
    rv_st  = rv_idx;
    rv_end = rv_idx + 1;
  }

  for (cb_idx = st; cb_idx < end; cb_idx++) {
    for (rv_idx = rv_st; rv_idx < rv_end; rv_idx++) {
      uint32_t long_cb_enc = 3 * srslte_cbsegm_cbsize(cb_idx) + 12;

      printf("checking cb_idx=%3d rv_idx=%d...", cb_idx, rv_idx);

      for (i = 0; i < long_cb_enc; i++) {
        bits[i] = rand() % 2;
      }

      bzero(buff_b, BUFFSZ * sizeof(uint8_t));

      srslte_rm_turbo_tx(buff_b, BUFFSZ, bits, long_cb_enc, rm_bits, nof_e_bits, 0);

      if (rv_idx > 0) {
        srslte_rm_turbo_tx(buff_b, BUFFSZ, bits, long_cb_enc, rm_bits, nof_e_bits, rv_idx);
      }

      for (int i = 0; i < long_cb_enc / 3; i++) {
        systematic[i]               = bits[3 * i];
        parity[i]                   = bits[3 * i + 1];
        parity[i + long_cb_enc / 3] = bits[3 * i + 2];
      }

      srslte_bit_pack_vector(systematic, systematic_bytes, long_cb_enc / 3);
      srslte_bit_pack_vector(parity, parity_bytes, 2 * long_cb_enc / 3);

      bzero(buff_b, BUFFSZ * sizeof(uint8_t));

      bzero(rm_bits2_bytes, nof_e_bits / 8);
      srslte_rm_turbo_tx_lut(buff_b, systematic_bytes, parity_bytes, rm_bits2_bytes, cb_idx, nof_e_bits, 0, 0);
      if (rv_idx > 0) {
        bzero(rm_bits2_bytes, nof_e_bits / 8);
        srslte_rm_turbo_tx_lut(buff_b, systematic_bytes, parity_bytes, rm_bits2_bytes, cb_idx, nof_e_bits, 0, rv_idx);
      }

      srslte_bit_unpack_vector(rm_bits2_bytes, rm_bits2, nof_e_bits);

      for (int i = 0; i < nof_e_bits; i++) {
        if (rm_bits2[i] != rm_bits[i]) {
          printf("Error in TX bit %d\n", i);
          exit(-1);
        }
      }

      printf("OK TX...");

      for (int i = 0; i < nof_e_bits; i++) {
        rm_bits_f[i] = rand() % 10 - 5;
        rm_bits_s[i] = (short)rm_bits_f[i];
      }

      srslte_vec_f_zero(buff_f, BUFFSZ);
      srslte_rm_turbo_rx(buff_f, BUFFSZ, rm_bits_f, nof_e_bits, bits_f, long_cb_enc, rv_idx, 0);

      bzero(bits2_s, long_cb_enc * sizeof(short));
      srslte_rm_turbo_rx_lut_(rm_bits_s, bits2_s, nof_e_bits, cb_idx, rv_idx, false);

      for (int i = 0; i < long_cb_enc; i++) {
        if (bits_f[i] != bits2_s[i]) {
          printf("error RX in bit %d %f!=%d\n", i, bits_f[i], bits2_s[i]);
          exit(-1);
        }
      }

      printf("OK RX\n");
    }
  }

  srslte_rm_turbo_free_tables();
  free(rm_bits_s);
  free(rm_bits_f);
  free(rm_bits);
  free(rm_bits2);
  free(rm_bits2_bytes);

  exit(0);
}
