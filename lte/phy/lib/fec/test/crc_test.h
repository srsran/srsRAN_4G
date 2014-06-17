/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdbool.h>

#include "liblte/phy/fec/crc.h"

typedef struct {
  int n;
  int l;
  unsigned int p;
  unsigned int s;
  unsigned int word;
}expected_word_t;


static expected_word_t expected_words[] = {

    {5001, 24, LTE_CRC24A, 1, 0x1C5C97},    // LTE CRC24A (36.212 Sec 5.1.1)
    {5001, 24, LTE_CRC24B, 1, 0x36D1F0},    // LTE CRC24B
    {5001, 16, LTE_CRC16, 1, 0x7FF4},      // LTE CRC16: 0x7FF4
    {5001, 8, LTE_CRC8, 1,  0xF0},      // LTE CRC8 0xF8

    {-1, -1, 0, 0, 0}
};

int get_expected_word(int n, int l, unsigned int p, unsigned int s, unsigned int *word) {
  int i;
  i=0;
  while(expected_words[i].n != -1) {
    if (expected_words[i].l == l
      && expected_words[i].p == p
      && expected_words[i].s == s) {
      break;
    } else {
      i++;
    }
  }
  if (expected_words[i].n == -1) {
    return -1;
  } else {
    if (word) {
      *word = expected_words[i].word;
    }
    return 0;
  }
}
