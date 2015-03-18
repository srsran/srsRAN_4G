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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "srslte/phch/cqi.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"


int srslte_cqi_hl_subband_pack(srslte_cqi_hl_subband_t *msg, uint32_t N, uint8_t *buff, uint32_t buff_len) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_pack(msg->wideband_cqi, &body_ptr, 4);
  srslte_bit_pack(msg->subband_diff_cqi, &body_ptr, 2*N);
  
  return 4+2*N;
}

int srslte_cqi_ue_subband_pack(srslte_cqi_ue_subband_t *msg, uint32_t L, uint8_t *buff, uint32_t buff_len)
{
  uint8_t *body_ptr = buff; 
  srslte_bit_pack(msg->wideband_cqi, &body_ptr, 4);
  srslte_bit_pack(msg->subband_diff_cqi, &body_ptr, 2);  
  srslte_bit_pack(msg->subband_diff_cqi, &body_ptr, L);  
  
  return 4+2+L;
}

int srslte_cqi_format2_wideband_pack(srslte_cqi_format2_wideband_t *msg, uint8_t *buff, uint32_t buff_len) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_pack(msg->wideband_cqi, &body_ptr, 4);  
  return 4;  
}

int srslte_cqi_format2_subband_pack(srslte_cqi_format2_subband_t *msg, uint8_t *buff, uint32_t buff_len) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_pack(msg->subband_cqi, &body_ptr, 4);  
  srslte_bit_pack(msg->subband_label, &body_ptr, 1);  
  return 4+1;    
}

