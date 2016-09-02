/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


int srslte_cqi_hl_subband_pack(srslte_cqi_hl_subband_t *msg, uint8_t buff[SRSLTE_CQI_MAX_BITS]) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_unpack(msg->wideband_cqi, &body_ptr, 4);
  srslte_bit_unpack(msg->subband_diff_cqi, &body_ptr, 2*msg->N);
  
  return 4+2*msg->N;
}

int srslte_cqi_ue_subband_pack(srslte_cqi_ue_subband_t *msg, uint8_t buff[SRSLTE_CQI_MAX_BITS])
{
  uint8_t *body_ptr = buff; 
  srslte_bit_unpack(msg->wideband_cqi, &body_ptr, 4);
  srslte_bit_unpack(msg->subband_diff_cqi, &body_ptr, 2);  
  srslte_bit_unpack(msg->subband_diff_cqi, &body_ptr, msg->L);  
  
  return 4+2+msg->L;
}

int srslte_cqi_format2_wideband_pack(srslte_cqi_format2_wideband_t *msg, uint8_t buff[SRSLTE_CQI_MAX_BITS]) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_unpack(msg->wideband_cqi, &body_ptr, 4);  
  return 4;  
}

int srslte_cqi_format2_subband_pack(srslte_cqi_format2_subband_t *msg, uint8_t buff[SRSLTE_CQI_MAX_BITS]) 
{
  uint8_t *body_ptr = buff; 
  srslte_bit_unpack(msg->subband_cqi, &body_ptr, 4);  
  srslte_bit_unpack(msg->subband_label, &body_ptr, msg->subband_label_2_bits?2:1);  
  return 4+(msg->subband_label_2_bits)?2:1;    
}

int srslte_cqi_value_pack(srslte_cqi_value_t *value, uint8_t buff[SRSLTE_CQI_MAX_BITS])
{
  switch(value->type) {
    case SRSLTE_CQI_TYPE_WIDEBAND:
      return srslte_cqi_format2_wideband_pack(&value->wideband, buff);
    case SRSLTE_CQI_TYPE_SUBBAND:
      return srslte_cqi_format2_subband_pack(&value->subband, buff);
    case SRSLTE_CQI_TYPE_SUBBAND_UE:
      return srslte_cqi_ue_subband_pack(&value->subband_ue, buff);
    case SRSLTE_CQI_TYPE_SUBBAND_HL:
      return srslte_cqi_hl_subband_pack(&value->subband_hl, buff);
  }
  return -1; 
}

bool srslte_cqi_send(uint32_t I_cqi_pmi, uint32_t tti) {
  
  uint32_t N_p = 0;
  uint32_t N_offset = 0;
  
  if (I_cqi_pmi <= 1) {
    N_p = 2; 
    N_offset = I_cqi_pmi; 
  } else if (I_cqi_pmi <= 6) {
    N_p = 5; 
    N_offset = I_cqi_pmi - 2;     
  } else if (I_cqi_pmi <= 16) {
    N_p = 10; 
    N_offset = I_cqi_pmi - 7;     
  } else if (I_cqi_pmi <= 36) {
    N_p = 20; 
    N_offset = I_cqi_pmi - 17;     
  } else if (I_cqi_pmi <= 76) {
    N_p = 40; 
    N_offset = I_cqi_pmi - 37;     
  } else if (I_cqi_pmi <= 156) {
    N_p = 80; 
    N_offset = I_cqi_pmi - 77;     
  } else if (I_cqi_pmi <= 316) {
    N_p = 160; 
    N_offset = I_cqi_pmi - 157;   
  } else if (I_cqi_pmi == 317) {
    return false; 
  } else if (I_cqi_pmi <= 349) {
    N_p = 32; 
    N_offset = I_cqi_pmi - 318;     
  } else if (I_cqi_pmi <= 413) {
    N_p = 64; 
    N_offset = I_cqi_pmi - 350;     
  } else if (I_cqi_pmi <= 541) {
    N_p = 128; 
    N_offset = I_cqi_pmi - 414;     
  } else if (I_cqi_pmi <= 1023) {
    return false; 
  }
  if (N_p) {
    if ((tti-N_offset)%N_p == 0) {
      return true; 
    } 
  }
  return false; 
}


/* SNR-to-CQI conversion, got from "Downlink SNR to CQI Mapping for Different Multiple Antenna Techniques in LTE"
 * Table III. 
*/
// From paper
static float cqi_to_snr_table[15] = { 1.95, 4, 6, 8, 10, 11.95, 14.05, 16, 17.9, 19.9, 21.5, 23.45, 25.0, 27.30, 29};

// From experimental measurements @ 5 MHz 
//static float cqi_to_snr_table[15] = { 1, 1.75, 3, 4, 5, 6, 7.5, 9, 11.5, 13.0, 15.0, 18, 20, 22.5, 26.5};

uint8_t srslte_cqi_from_snr(float snr)
{
 for (int cqi=14;cqi>=0;cqi--) {
   if (snr >= cqi_to_snr_table[cqi]) {
     return (uint8_t) cqi+1;
   }
 }
 return 0;
}

/* Returns the subband size for higher layer-configured subband feedback,
 * i.e., the number of RBs per subband as a function of the cell bandwidth
 * (Table 7.2.1-3 in TS 36.213)
 */
int srslte_cqi_hl_get_subband_size(int nof_prb)
{
  if (nof_prb < 7) {
    return 0;
  } else if (nof_prb <= 26) {
    return 4;
  } else if (nof_prb <= 63) {
    return 6;
  } else if (nof_prb <= 110) {
    return 8;
  } else {
    return -1;
  }
}

/* Returns the number of subbands to be reported in CQI measurements as
 * defined in clause 7.2 in TS 36.213, i.e., the N parameter
 */
int srslte_cqi_hl_get_no_subbands(int nof_prb)
{
  int hl_size = srslte_cqi_hl_get_subband_size(nof_prb); 
  if (hl_size > 0) {
    return (int)ceil((float)nof_prb/hl_size);
  } else {
    return 0;
  }
}
