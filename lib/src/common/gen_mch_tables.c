/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/common/gen_mch_tables.h"

/******************************************************************************
 * Key Generation
 *****************************************************************************/

void generate_frame_table(uint8_t* table, uint8_t alloc)
{
  table[1] = (alloc >> 5) & 0x01;
  table[2] = (alloc >> 4) & 0x01;
  table[3] = (alloc >> 3) & 0x01;
  table[6] = (alloc >> 2) & 0x01;
  table[7] = (alloc >> 1) & 0x01;
  table[8] = (alloc >> 0) & 0x01;
}

void generate_mch_table(uint8_t* table, uint32_t sf_alloc, uint8_t num_frames)
{
  if (num_frames == 1) {
    uint8_t alloc = (sf_alloc)&0x3F;
    generate_frame_table(table, alloc);
  } else if (num_frames == 4) {
    for (uint32_t j = 0; j < 4; j++) {
      uint8_t alloc = (sf_alloc >> 6 * (3 - j)) & 0x3F;
      generate_frame_table(&table[j * 10], alloc);
    }
  }
}

void generate_mcch_table(uint8_t* table, uint32_t sf_alloc)
{
  uint8_t alloc = (sf_alloc)&0x3F;
  generate_frame_table(table, alloc);
}
