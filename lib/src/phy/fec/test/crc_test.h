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

#include <stdbool.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/crc.h"

typedef struct {
  int      n;
  int      l;
  uint32_t p;
  uint32_t s;
  uint32_t word;
} expected_word_t;

static expected_word_t expected_words[] = {

    {5001, 24, SRSRAN_LTE_CRC24A, 1, 0x1C5C97}, // LTE CRC24A (36.212 Sec 5.1.1)
    {5001, 24, SRSRAN_LTE_CRC24B, 1, 0x36D1F0}, // LTE CRC24B
    {5001, 16, SRSRAN_LTE_CRC16, 1, 0x7FF4},    // LTE CRC16: 0x7FF4
    {5001, 8, SRSRAN_LTE_CRC8, 1, 0xF0},        // LTE CRC8 0xF8
    {30, 11, SRSRAN_LTE_CRC11, 1, 0x114},       // NR CRC11 0x114
    {20, 6, SRSRAN_LTE_CRC6, 1, 0x1F},          // NR CRC6 0x1F

    {-1, -1, 0, 0, 0}};

int get_expected_word(int n, int l, uint32_t p, unsigned int s, unsigned int* word)
{
  int i;
  i = 0;
  while (expected_words[i].n != -1) {
    if (expected_words[i].l == l && expected_words[i].p == p && expected_words[i].s == s) {
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
