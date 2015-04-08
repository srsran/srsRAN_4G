/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/phch/dci.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"


/* Creates the DL PDSCH resource allocation grant from a DCI message
 */
int srslte_dci_msg_to_ra_dl(srslte_dci_msg_t *msg, uint16_t msg_rnti,
                     srslte_cell_t cell, uint32_t cfi,
                     srslte_ra_pdsch_t *ra_dl) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (msg               !=  NULL   &&
      ra_dl             !=  NULL   &&
      srslte_cell_isvalid(&cell)      && 
      cfi               >   0      &&
      cfi               <   4)
  {
    ret = SRSLTE_ERROR;
    
    srslte_dci_msg_type_t type;
    if (srslte_dci_msg_get_type(msg, &type, cell.nof_prb, msg_rnti)) {
      fprintf(stderr, "Can't get DCI message type\n");
      return ret; 
    }
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      INFO("",0);
      srslte_dci_msg_type_fprint(stdout, type);    
    }
    if (type.type == SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED) {
      bzero(ra_dl, sizeof(srslte_ra_pdsch_t));
      
      bool crc_is_crnti = false; 
      if (msg_rnti >= SRSLTE_CRNTI_START && msg_rnti <= SRSLTE_CRNTI_END) {
        crc_is_crnti = true; 
      }
      if (srslte_dci_msg_unpack_pdsch(msg, ra_dl, cell.nof_prb, crc_is_crnti)) {
        fprintf(stderr, "Can't unpack PDSCH message\n");
        return ret;
      } 
      
      if (SRSLTE_VERBOSE_ISINFO()) {
        srslte_ra_pdsch_fprint(stdout, ra_dl, cell.nof_prb);
      }
      
      if (srslte_ra_dl_alloc(&ra_dl->prb_alloc, ra_dl, cell.nof_prb)) {
        fprintf(stderr, "Error computing resource allocation\n");
        return ret;
      }
      
      srslte_ra_dl_alloc_re(&ra_dl->prb_alloc, cell.nof_prb, cell.nof_ports, cell.nof_prb<10?(cfi+1):cfi, cell.cp);
            
      ret = SRSLTE_SUCCESS;
    } else {
      fprintf(stderr, "Unsupported message type: "); 
      srslte_dci_msg_type_fprint(stderr, type);
    }
  }
  return ret;
}

/* Creates the UL PUSCH resource allocation grant from the random access respone message 
 */
int srslte_dci_rar_to_ra_ul(uint32_t rba, uint32_t trunc_mcs, bool hopping_flag, uint32_t nof_prb, srslte_ra_pusch_t *ra) {
  bzero(ra, sizeof(srslte_ra_pusch_t));
  if (!hopping_flag) {
    ra->freq_hop_fl = SRSLTE_RA_PUSCH_HOP_DISABLED;
  } else {
    fprintf(stderr, "FIXME: Frequency hopping in RAR not implemented\n");
    ra->freq_hop_fl = 1;
  }
  uint32_t riv = rba; 
  // Truncate resource block assignment 
  uint32_t b = 0;
  if (nof_prb <= 44) {
    b = (uint32_t) (ceilf(log2((float) nof_prb*(nof_prb+1)/2)));
    riv = riv & ((1<<(b+1))-1); 
  }
  ra->type2_alloc.riv = riv; 
  ra->mcs_idx = trunc_mcs;

  srslte_ra_type2_from_riv(riv, &ra->type2_alloc.L_crb, &ra->type2_alloc.RB_start,
      nof_prb, nof_prb);
  
  srslte_ra_mcs_from_idx_ul(ra->mcs_idx, srslte_ra_nprb_ul(ra, nof_prb), &ra->mcs);
  return SRSLTE_SUCCESS;
}

/* Creates the UL PUSCH resource allocation grant from a DCI format 0 message
 */
int srslte_dci_msg_to_ra_ul(srslte_dci_msg_t *msg, uint32_t nof_prb, uint32_t n_rb_ho, srslte_ra_pusch_t *ra_ul) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  
  if (msg               !=  NULL   &&
      ra_ul             !=  NULL)
  {
    ret = SRSLTE_ERROR;
    
    bzero(ra_ul, sizeof(srslte_ra_pusch_t));
    
    if (srslte_dci_msg_unpack_pusch(msg, ra_ul, nof_prb)) {
      fprintf(stderr, "Can't unpack PDSCH message\n");
      return ret;
    } 
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      srslte_ra_pusch_fprint(stdout, ra_ul, nof_prb);
    }
    
    if (srslte_ra_ul_alloc(&ra_ul->prb_alloc, ra_ul, n_rb_ho, nof_prb)) {
      fprintf(stderr, "Error computing resource allocation\n");
      return ret;
    }
    
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_dci_location_set(srslte_dci_location_t *c, uint32_t L, uint32_t nCCE) {
  if (L <= 3) {
    c->L = L;
  } else {
    fprintf(stderr, "Invalid L %d\n", L);
    return SRSLTE_ERROR;
  }
  if (nCCE <= 87) {
    c->ncce = nCCE;
  } else {
    fprintf(stderr, "Invalid nCCE %d\n", nCCE);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

bool srslte_dci_location_isvalid(srslte_dci_location_t *c) {
  if (c->L <= 3 && c->ncce <= 87) {
    return true;
  } else {
    return false;
  }
}

uint32_t riv_nbits(uint32_t nof_prb) {
  return (uint32_t) ceilf(log2f((float) nof_prb * ((float) nof_prb + 1) / 2));
}

const uint32_t ambiguous_sizes[10] = { 12, 14, 16, 20, 24, 26, 32, 40, 44, 56 };

bool is_ambiguous_size(uint32_t size) {
  int i;
  for (i = 0; i < 10; i++) {
    if (size == ambiguous_sizes[i]) {
      return true;
    }
  }
  return false;
}

/**********************************
 *  PAYLOAD sizeof functions
 * ********************************/
uint32_t dci_format0_sizeof_(uint32_t nof_prb) {
  return 1 + 1 + riv_nbits(nof_prb) + 5 + 1 + 2 + 3 + 1;
}

uint32_t dci_format1A_sizeof(uint32_t nof_prb) {
  uint32_t n;
  n = 1 + 1 + riv_nbits(nof_prb) + 5 + 3 + 1 + 2 + 2;
  while (n < dci_format0_sizeof_(nof_prb)) {
    n++;
  }
  if (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

uint32_t dci_format0_sizeof(uint32_t nof_prb) {
  uint32_t n = dci_format0_sizeof_(nof_prb);
  while (n < dci_format1A_sizeof(nof_prb)) {
    n++;
  }
  return n;
}

uint32_t dci_format1_sizeof(uint32_t nof_prb) {

  uint32_t n = (uint32_t) ceilf((float) nof_prb / srslte_ra_type0_P(nof_prb)) + 5 + 3 + 1 + 2
      + 2;
  if (nof_prb > 10) {
    n++;
  }
  while (n == dci_format0_sizeof(nof_prb) || n == dci_format1A_sizeof(nof_prb)
      || is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

uint32_t dci_format1C_sizeof(uint32_t nof_prb) {
  uint32_t n_vrb_dl_gap1 = srslte_ra_type2_n_vrb_dl(nof_prb, true);
  uint32_t n_step = srslte_ra_type2_n_rb_step(nof_prb);
  uint32_t n = riv_nbits((uint32_t) n_vrb_dl_gap1 / n_step) + 5;
  if (nof_prb >= 50) {
    n++;
  }
  return n;
}

uint32_t srslte_dci_format_sizeof(srslte_dci_format_t format, uint32_t nof_prb) {
  switch (format) {
  case SRSLTE_DCI_FORMAT0:
    return dci_format0_sizeof(nof_prb);
  case SRSLTE_DCI_FORMAT1:
    return dci_format1_sizeof(nof_prb);
  case SRSLTE_DCI_FORMAT1A:
    return dci_format1A_sizeof(nof_prb);
  case SRSLTE_DCI_FORMAT1C:
    return dci_format1C_sizeof(nof_prb);
  default:
    return SRSLTE_ERROR;
  }
}

/**********************************
 *  DCI Resource Allocation functions
 * ********************************/

/* Packs DCI format 0 data to a sequence of bits and store them in msg according
 * to 36.212 5.3.3.1.1
 *
 * TODO: TPC and cyclic shift for DM RS not implemented
 */
int dci_format0_pack(srslte_ra_pusch_t *data, srslte_dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;
  uint32_t n_ul_hop;

  *y++ = 0; // format differentiation
  if (data->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_DISABLED) { // frequency hopping
    *y++ = 0;
    n_ul_hop = 0;
  } else {
    *y++ = 1;
    if (nof_prb < 50) {
      n_ul_hop = 1; // Table 8.4-1 of 36.213
      *y++ = data->freq_hop_fl & 1;
    } else {
      n_ul_hop = 2; // Table 8.4-1 of 36.213
      *y++ = (data->freq_hop_fl & 2) >> 1;
      *y++ = data->freq_hop_fl & 1;
    }
  }

  /* pack RIV according to 8.1 of 36.213 */
  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = srslte_ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start,
        nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  srslte_bit_pack(riv, &y, riv_nbits(nof_prb) - n_ul_hop);

  /* pack MCS according to 8.6.1 of 36.213 */
  srslte_bit_pack(data->mcs_idx, &y, 5);

  *y++ = data->ndi;

  // TCP commands not implemented
  *y++ = 0;
  *y++ = 0;

  // DM RS not implemented
  *y++ = 0;
  *y++ = 0;
  *y++ = 0;

  // CQI request
  *y++ = data->cqi_request;

  // Padding with zeros
  uint32_t n = dci_format0_sizeof(nof_prb);
  while (y - msg->data < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->data);
  return SRSLTE_SUCCESS;
}
/* Unpacks DCI format 0 data and store result in msg according
 * to 36.212 5.3.3.1.1
 *
 * TODO: TPC and cyclic shift for DM RS not implemented
 */
int dci_format0_unpack(srslte_dci_msg_t *msg, srslte_ra_pusch_t *data, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;
  uint32_t n_ul_hop;

  /* Make sure it's a SRSLTE_DCI_FORMAT0 message */
  if (msg->nof_bits != srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT0, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 0\n");
    return SRSLTE_ERROR;
  }
  if (*y++ != 0) {
    fprintf(stderr,
        "Invalid format differentiation field value. This is SRSLTE_DCI_FORMAT1A\n");
    return SRSLTE_ERROR;
  }
  if (*y++ == 0) {
    data->freq_hop_fl = SRSLTE_RA_PUSCH_HOP_DISABLED;
    n_ul_hop = 0;
  } else {
    if (nof_prb < 50) {
      n_ul_hop = 1; // Table 8.4-1 of 36.213
      data->freq_hop_fl = *y++;
    } else {
      n_ul_hop = 2; // Table 8.4-1 of 36.213
      data->freq_hop_fl = y[0] << 1 | y[1];
      y += 2;
    }
  }
  /* unpack RIV according to 8.1 of 36.213 */
  uint32_t riv = srslte_bit_unpack(&y, riv_nbits(nof_prb) - n_ul_hop);
  srslte_ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start,
      nof_prb, nof_prb);
  data->type2_alloc.riv = riv;

  /* unpack MCS according to 8.6 of 36.213 */
  data->mcs_idx = srslte_bit_unpack(&y, 5);

  data->ndi = *y++ ? true : false;

  // TCP and DM RS commands not implemented
  y += 5;

  // CQI request
  data->cqi_request = *y++ ? true : false;

  // 8.6.2 First paragraph
  if (data->mcs_idx <= 28) {
    srslte_ra_mcs_from_idx_ul(data->mcs_idx, srslte_ra_nprb_ul(data, nof_prb), &data->mcs);
  } else if (data->mcs_idx == 29 && data->cqi_request && srslte_ra_nprb_ul(data, nof_prb) <= 4) {
    // 8.6.1 and 8.6.2 36.213 second paragraph
    data->mcs.mod = SRSLTE_MOD_QPSK;
    data->mcs.tbs = 0;
  } else if (data->mcs_idx >= 29) {
    // Else leave TBS and use the previously used PUSCH modulation
    data->mcs.tbs = 0;
    data->rv_idx = data->mcs_idx - 28;
  }

  return SRSLTE_SUCCESS;
}

/* Packs DCI format 1 data to a sequence of bits and store them in msg according
 * to 36.212 5.3.3.1.2
 *
 * TODO: TPC commands
 */

int dci_format1_pack(srslte_ra_pdsch_t *data, srslte_dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  if (nof_prb > 10) {
    *y++ = data->alloc_type;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P = srslte_ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t) ceilf((float) nof_prb / P);
  switch (data->alloc_type) {
  case SRSLTE_RA_ALLOC_TYPE0:
    srslte_bit_pack((uint32_t) data->type0_alloc.rbg_bitmask, &y, alloc_size);
    break;
  case SRSLTE_RA_ALLOC_TYPE1:
    srslte_bit_pack((uint32_t) data->type1_alloc.rbg_subset, &y, (int) ceilf(log2f(P)));
    *y++ = data->type1_alloc.shift ? 1 : 0;
    srslte_bit_pack((uint32_t) data->type1_alloc.vrb_bitmask, &y,
        alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr,
        "Format 1 accepts type0 or type1 resource allocation only\n");
    return SRSLTE_ERROR;

  }
  /* pack MCS */
  srslte_bit_pack(data->mcs_idx, &y, 5);

  /* harq process number */
  srslte_bit_pack(data->harq_process, &y, 3);

  *y++ = data->ndi;

  // rv version
  srslte_bit_pack(data->rv_idx, &y, 2);

  // TPC not implemented
  *y++ = 0;
  *y++ = 0;

  // Padding with zeros
  uint32_t n = dci_format1_sizeof(nof_prb);
  while (y - msg->data < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->data);

  return SRSLTE_SUCCESS;
}

int dci_format1_unpack(srslte_dci_msg_t *msg, srslte_ra_pdsch_t *data, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  /* Make sure it's a SRSLTE_DCI_FORMAT1 message */
  if (msg->nof_bits != srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1\n");
    return SRSLTE_ERROR;
  }

  if (nof_prb > 10) {
    data->alloc_type = *y++;
  } else {
    data->alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P = srslte_ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t) ceilf((float) nof_prb / P);
  switch (data->alloc_type) {
  case SRSLTE_RA_ALLOC_TYPE0:
    data->type0_alloc.rbg_bitmask = srslte_bit_unpack(&y, alloc_size);
    break;
  case SRSLTE_RA_ALLOC_TYPE1:
    data->type1_alloc.rbg_subset = srslte_bit_unpack(&y, (int) ceilf(log2f(P)));
    data->type1_alloc.shift = *y++ ? true : false;
    data->type1_alloc.vrb_bitmask = srslte_bit_unpack(&y,
        alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr, "Format 1 accepts type0 or type1 resource allocation only\n");
    return SRSLTE_ERROR;

  }
  /* unpack MCS according to 7.1.7 of 36.213 */
  data->mcs_idx = srslte_bit_unpack(&y, 5);
  if (srslte_ra_mcs_from_idx_dl(data->mcs_idx, srslte_ra_nprb_dl(data, nof_prb), &data->mcs)) {
    fprintf(stderr, "Error getting MCS\n");
    return SRSLTE_ERROR;
  }
  
  /* harq process number */
  data->harq_process = srslte_bit_unpack(&y, 3);

  data->ndi = *y++ ? true : false;

  // rv version
  data->rv_idx = srslte_bit_unpack(&y, 2);

  // TPC not implemented

  return SRSLTE_SUCCESS;
}

/* Packs DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 * TODO: RA procedure initiated by PDCCH, TPC commands
 */
int dci_format1As_pack(srslte_ra_pdsch_t *data, srslte_dci_msg_t *msg, uint32_t nof_prb,
    bool crc_is_crnti) {

  /* pack bits */
  uint8_t *y = msg->data;

  *y++ = 1; // format differentiation

  if (data->alloc_type != SRSLTE_RA_ALLOC_TYPE2) {
    fprintf(stderr, "Format 1A accepts type2 resource allocation only\n");
    return SRSLTE_ERROR;
  }

  *y++ = data->type2_alloc.mode; // localized or distributed VRB assignment

  if (data->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC) {
    if (data->type2_alloc.L_crb > nof_prb) {
      fprintf(stderr, "L_CRB=%d can not exceed system BW for localized type2\n",
          data->type2_alloc.L_crb);
      return SRSLTE_ERROR;
    }
  } else {
    uint32_t n_vrb_dl;
    if (crc_is_crnti && nof_prb > 50) {
      n_vrb_dl = 16;
    } else {
      n_vrb_dl = srslte_ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);
    }
    if (data->type2_alloc.L_crb > n_vrb_dl) {
      fprintf(stderr,
          "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n",
          data->type2_alloc.L_crb, n_vrb_dl);
      return SRSLTE_ERROR;
    }
  }
  /* pack RIV according to 7.1.6.3 of 36.213 */
  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = srslte_ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start,
        nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  uint32_t nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && nof_prb >= 50) {
    nb_gap = 1;
    *y++ = data->type2_alloc.n_gap;
  }
  srslte_bit_pack(riv, &y, riv_nbits(nof_prb) - nb_gap);

  // in format1A, MCS = TBS according to 7.1.7.2 of 36.213
  srslte_bit_pack(data->mcs_idx, &y, 5);

  srslte_bit_pack(data->harq_process, &y, 3);

  if (!crc_is_crnti && nof_prb >= 50 && data->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST) {
    *y++ = data->type2_alloc.n_gap;
  } else {
    y++; // bit reserved
  }

  // rv version
  srslte_bit_pack(data->rv_idx, &y, 2);

  if (crc_is_crnti) {
    // TPC not implemented
    *y++ = 0;
    *y++ = 0;
  } else {
    y++; // MSB of TPC is reserved
    *y++ = data->type2_alloc.n_prb1a; // LSB indicates N_prb_1a for TBS
  }

  // Padding with zeros
  uint32_t n = dci_format1A_sizeof(nof_prb);
  while (y - msg->data < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->data);

  return SRSLTE_SUCCESS;
}

/* Unpacks DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 */
int dci_format1As_unpack(srslte_dci_msg_t *msg, srslte_ra_pdsch_t *data, uint32_t nof_prb,
    bool crc_is_crnti) {

  /* pack bits */
  uint8_t *y = msg->data;

  /* Make sure it's a SRSLTE_DCI_FORMAT0 message */
  if (msg->nof_bits != srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1A, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1A\n");
    return SRSLTE_ERROR;
  }

  if (*y++ != 1) {
    fprintf(stderr, "Invalid format differentiation field value. This is SRSLTE_DCI_FORMAT0\n");
    return SRSLTE_ERROR;
  }

  data->alloc_type = SRSLTE_RA_ALLOC_TYPE2;
  data->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  data->type2_alloc.n_gap = SRSLTE_RA_TYPE2_NG1;

  /* unpack RIV according to 7.1.6.3 of 36.213 */
  uint32_t nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && nof_prb >= 50) {
    nb_gap = 1;
    data->type2_alloc.n_gap = *y++;
  }
  uint32_t nof_vrb;
  if (data->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC) {
    nof_vrb = nof_prb;
  } else {
    nof_vrb = srslte_ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);
  }
  uint32_t riv = srslte_bit_unpack(&y, riv_nbits(nof_prb) - nb_gap);
  srslte_ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start,
      nof_prb, nof_vrb);
  data->type2_alloc.riv = riv;

  // unpack MCS
  data->mcs_idx = srslte_bit_unpack(&y, 5);

  data->harq_process = srslte_bit_unpack(&y, 3);

  if (!crc_is_crnti && nof_prb >= 50 && data->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST) {
    data->type2_alloc.n_gap = *y++;
  } else {
    y++; // bit reserved
  }

  // rv version
  srslte_bit_pack(data->rv_idx, &y, 2);

  if (crc_is_crnti) {
    // TPC not implemented
    y++;
    y++;
  } else {
    y++; // MSB of TPC is reserved
    data->type2_alloc.n_prb1a = *y++; // LSB indicates N_prb_1a for TBS
  }

  uint32_t n_prb;
  if (crc_is_crnti) {
    n_prb = srslte_ra_nprb_dl(data, nof_prb);
  } else {
    n_prb = data->type2_alloc.n_prb1a == SRSLTE_RA_TYPE2_NPRB1A_2 ? 2 : 3;
  }
  data->mcs.tbs = srslte_ra_tbs_from_idx(data->mcs_idx, n_prb);
  data->mcs.mod = SRSLTE_MOD_QPSK;

  return SRSLTE_SUCCESS;
}

/* Format 1C for compact scheduling of PDSCH words
 *
 */
int dci_format1Cs_pack(srslte_ra_pdsch_t *data, srslte_dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  if (data->alloc_type != SRSLTE_RA_ALLOC_TYPE2 || data->type2_alloc.mode != SRSLTE_RA_TYPE2_DIST) {
    fprintf(stderr,
        "Format 1C accepts distributed type2 resource allocation only\n");
    return SRSLTE_ERROR;
  }

  if (nof_prb >= 50) {
    *y++ = data->type2_alloc.n_gap;
  }
  uint32_t n_step = srslte_ra_type2_n_rb_step(nof_prb);
  uint32_t n_vrb_dl = srslte_ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);

  if (data->type2_alloc.L_crb > ((uint32_t) n_vrb_dl / n_step) * n_step) {
    fprintf(stderr, "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n",
        data->type2_alloc.L_crb, ((uint32_t) n_vrb_dl / n_step) * n_step);
    return SRSLTE_ERROR;
  }
  if (data->type2_alloc.L_crb % n_step) {
    fprintf(stderr, "L_crb must be multiple of n_step\n");
    return SRSLTE_ERROR;
  }
  if (data->type2_alloc.RB_start % n_step) {
    fprintf(stderr, "RB_start must be multiple of n_step\n");
    return SRSLTE_ERROR;
  }
  uint32_t L_p = data->type2_alloc.L_crb / n_step;
  uint32_t RB_p = data->type2_alloc.RB_start / n_step;
  uint32_t n_vrb_p = (int) n_vrb_dl / n_step;

  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = srslte_ra_type2_to_riv(L_p, RB_p, n_vrb_p);
  } else {
    riv = data->type2_alloc.riv;
  }
  srslte_bit_pack(riv, &y, riv_nbits((int) n_vrb_dl / n_step));

  // in format1C, MCS = TBS according to 7.1.7.2 of 36.213
  srslte_bit_pack(data->mcs_idx, &y, 5);

  msg->nof_bits = (y - msg->data);

  return SRSLTE_SUCCESS;
}

int dci_format1Cs_unpack(srslte_dci_msg_t *msg, srslte_ra_pdsch_t *data, uint32_t nof_prb) {
  uint32_t L_p, RB_p;

  /* pack bits */
  uint8_t *y = msg->data;

  if (msg->nof_bits != srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1C, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1C\n");
    return SRSLTE_ERROR;
  }
  data->alloc_type = SRSLTE_RA_ALLOC_TYPE2;
  data->type2_alloc.mode = SRSLTE_RA_TYPE2_DIST;
  if (nof_prb >= 50) {
    data->type2_alloc.n_gap = *y++;
  }
  uint32_t n_step = srslte_ra_type2_n_rb_step(nof_prb);
  uint32_t n_vrb_dl = srslte_ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);

  uint32_t riv = srslte_bit_unpack(&y, riv_nbits((int) n_vrb_dl / n_step));
  uint32_t n_vrb_p = (uint32_t) n_vrb_dl / n_step;

  srslte_ra_type2_from_riv(riv, &L_p, &RB_p, n_vrb_p, n_vrb_p);
  data->type2_alloc.L_crb = L_p * n_step;
  data->type2_alloc.RB_start = RB_p * n_step;
  data->type2_alloc.riv = riv;

  data->mcs_idx = srslte_bit_unpack(&y, 5);
  data->mcs.tbs = srslte_ra_tbs_from_idx_format1c(data->mcs_idx);
  data->mcs.mod = SRSLTE_MOD_QPSK;

  msg->nof_bits = (y - msg->data);

  return SRSLTE_SUCCESS;
}

int srslte_dci_msg_pack_pdsch(srslte_ra_pdsch_t *data, srslte_dci_msg_t *msg, srslte_dci_format_t format,
    uint32_t nof_prb, bool crc_is_crnti) {
  switch (format) {
  case SRSLTE_DCI_FORMAT1:
    return dci_format1_pack(data, msg, nof_prb);
  case SRSLTE_DCI_FORMAT1A:
    return dci_format1As_pack(data, msg, nof_prb, crc_is_crnti);
  case SRSLTE_DCI_FORMAT1C:
    return dci_format1Cs_pack(data, msg, nof_prb);
  default:
    fprintf(stderr, "Invalid DCI format %s for PDSCH resource allocation\n",
        srslte_dci_format_string(format));
    return SRSLTE_ERROR;
  }
}

int srslte_dci_msg_unpack_pdsch(srslte_dci_msg_t *msg, srslte_ra_pdsch_t *data, uint32_t nof_prb,
    bool crc_is_crnti) {
  if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1, nof_prb)) {
    return dci_format1_unpack(msg, data, nof_prb);
  } else if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1A, nof_prb)) {
    return dci_format1As_unpack(msg, data, nof_prb, crc_is_crnti);
  } else if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1C, nof_prb)) {
    return dci_format1Cs_unpack(msg, data, nof_prb);
  } else {
    return SRSLTE_ERROR;
  }
}

int srslte_dci_msg_pack_pusch(srslte_ra_pusch_t *data, srslte_dci_msg_t *msg, uint32_t nof_prb) {
  return dci_format0_pack(data, msg, nof_prb);
}

int srslte_dci_msg_unpack_pusch(srslte_dci_msg_t *msg, srslte_ra_pusch_t *data, uint32_t nof_prb) {
  return dci_format0_unpack(msg, data, nof_prb);
}

srslte_dci_format_t srslte_dci_format_from_string(char *str) {
  if (!strcmp(str, "Format0")) {
    return SRSLTE_DCI_FORMAT0;
  } else if (!strcmp(str, "Format1")) {
    return SRSLTE_DCI_FORMAT1; 
  } else if (!strcmp(str, "Format1A")) {
    return SRSLTE_DCI_FORMAT1A; 
  } else if (!strcmp(str, "Format1C")) {
    return SRSLTE_DCI_FORMAT1C; 
  } else {
    return SRSLTE_DCI_FORMAT_ERROR;
  }
}

char* srslte_dci_format_string(srslte_dci_format_t format) {
  switch (format) {
  case SRSLTE_DCI_FORMAT0:
    return "Format0";
  case SRSLTE_DCI_FORMAT1:
    return "Format1";
  case SRSLTE_DCI_FORMAT1A:
    return "Format1A";
  case SRSLTE_DCI_FORMAT1C:
    return "Format1C";
  default:
    return "N/A"; // fatal error
  }
}

void srslte_dci_msg_type_fprint(FILE *f, srslte_dci_msg_type_t type) {
  switch (type.type) {
  case SRSLTE_DCI_MSG_TYPE_PUSCH_SCHED:
    fprintf(f, "%s PUSCH Scheduling\n", srslte_dci_format_string(type.format));
    break;
  case SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED:
    fprintf(f, "%s PDSCH Scheduling\n", srslte_dci_format_string(type.format));
    break;
  case SRSLTE_DCI_MSG_TYPE_RA_PROC_PDCCH:
    fprintf(f, "%s Random access initiated by PDCCH\n",
        srslte_dci_format_string(type.format));
    break;
  case SRSLTE_DCI_MSG_TYPE_MCCH_CHANGE:
    fprintf(f, "%s MCCH change notification\n", srslte_dci_format_string(type.format));
    break;
  case SRSLTE_DCI_MSG_TYPE_TPC_COMMAND:
    fprintf(f, "%s TPC command\n", srslte_dci_format_string(type.format));
    break;
  }
}

int srslte_dci_msg_get_type(srslte_dci_msg_t *msg, srslte_dci_msg_type_t *type, uint32_t nof_prb,
    uint16_t msg_rnti) 
{
  DEBUG("Get message type: nof_bits=%d, msg_rnti=0x%x\n", msg->nof_bits, msg_rnti);
  if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT0, nof_prb)
      && !msg->data[0]) {
    type->type = SRSLTE_DCI_MSG_TYPE_PUSCH_SCHED;
    type->format = SRSLTE_DCI_FORMAT0;
    return SRSLTE_SUCCESS;
  } else if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1, nof_prb)) {
    type->type = SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED; // only these 2 types supported
    type->format = SRSLTE_DCI_FORMAT1;
    return SRSLTE_SUCCESS;
  } else if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1A, nof_prb)) {
    /* The RNTI is not the only condition. Also some fields in the packet. 
     * if (msg_rnti >= SRSLTE_CRNTI_START && msg_rnti <= SRSLTE_CRNTI_END) {
      type->type = SRSLTE_DCI_MSG_TYPE_RA_PROC_PDCCH;
      type->format = SRSLTE_DCI_FORMAT1A;
    } else {
      */
      type->type = SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED; // only these 2 types supported
      type->format = SRSLTE_DCI_FORMAT1A;
    //}
    return SRSLTE_SUCCESS;
  } else if (msg->nof_bits == srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1C, nof_prb)) {
    if (msg_rnti == SRSLTE_MRNTI) {
      type->type = SRSLTE_DCI_MSG_TYPE_MCCH_CHANGE;
      type->format = SRSLTE_DCI_FORMAT1C;
    } else {
      type->type = SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED; // only these 2 types supported
      type->format = SRSLTE_DCI_FORMAT1C;
    }
    return SRSLTE_SUCCESS;
  }
  return SRSLTE_ERROR;
}

