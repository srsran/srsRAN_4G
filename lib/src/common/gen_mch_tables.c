/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/common/gen_mch_tables.h"


/******************************************************************************
 * Key Generation
 *****************************************************************************/

void generate_frame_table(uint8_t *table, uint8_t alloc)
{
  table[1] = (alloc >> 5) & 0x01;
  table[2] = (alloc >> 4) & 0x01;
  table[3] = (alloc >> 3) & 0x01;
  table[6] = (alloc >> 2) & 0x01;
  table[7] = (alloc >> 1) & 0x01;
  table[8] = (alloc >> 0) & 0x01;
}

void generate_mch_table(uint8_t *table, uint32_t sf_alloc, uint8_t num_frames)
{
  if(num_frames == 1){
    uint8_t alloc = (sf_alloc) & 0x3F;
    generate_frame_table(table, alloc);
  } else if(num_frames == 4){
    for(uint32_t j=0; j<4; j++){
      uint8_t alloc = (sf_alloc >> 6*(3-j)) & 0x3F;
      generate_frame_table(&table[j*10], alloc);
    }
  }
}

void generate_mcch_table(uint8_t *table, uint32_t sf_alloc)
{
  uint8_t alloc = (sf_alloc) & 0x3F;
  generate_frame_table(table, alloc);
}
