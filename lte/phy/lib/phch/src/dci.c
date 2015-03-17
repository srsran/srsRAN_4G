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

#include "liblte/phy/phch/dci.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"


/* Creates the DL PDSCH resource allocation grant from a DCI message
 */
int dci_msg_to_ra_dl(dci_msg_t *msg, uint16_t msg_rnti,
                     lte_cell_t cell, uint32_t cfi,
                     ra_pdsch_t *ra_dl) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (msg               !=  NULL   &&
      ra_dl             !=  NULL   &&
      lte_cell_isvalid(&cell)      && 
      cfi               >   0      &&
      cfi               <   4)
  {
    ret = LIBLTE_ERROR;
    
    dci_msg_type_t type;
    if (dci_msg_get_type(msg, &type, cell.nof_prb, msg_rnti)) {
      fprintf(stderr, "Can't get DCI message type\n");
      return ret; 
    }
    
    if (VERBOSE_ISINFO()) {
      INFO("",0);
      dci_msg_type_fprint(stdout, type);    
    }
    if (type.type == PDSCH_SCHED) {
      bzero(ra_dl, sizeof(ra_pdsch_t));
      
      bool crc_is_crnti = false; 
      if (msg_rnti >= CRNTI_START && msg_rnti <= CRNTI_END) {
        crc_is_crnti = true; 
      }
      if (dci_msg_unpack_pdsch(msg, ra_dl, cell.nof_prb, crc_is_crnti)) {
        fprintf(stderr, "Can't unpack PDSCH message\n");
        return ret;
      } 
      
      if (VERBOSE_ISINFO()) {
        ra_pdsch_fprint(stdout, ra_dl, cell.nof_prb);
      }
      
      if (ra_dl_alloc(&ra_dl->prb_alloc, ra_dl, cell.nof_prb)) {
        fprintf(stderr, "Error computing resource allocation\n");
        return ret;
      }
      
      ra_dl_alloc_re(&ra_dl->prb_alloc, cell.nof_prb, cell.nof_ports, cell.nof_prb<10?(cfi+1):cfi, cell.cp);
            
      ret = LIBLTE_SUCCESS;
    } else {
      fprintf(stderr, "Unsupported message type: "); 
      dci_msg_type_fprint(stderr, type);
    }
  }
  return ret;
}

/* Creates the UL PUSCH resource allocation grant from the random access respone message 
 */
int dci_rar_to_ra_ul(uint32_t rba, uint32_t trunc_mcs, bool hopping_flag, uint32_t nof_prb, ra_pusch_t *ra) {
  bzero(ra, sizeof(ra_pusch_t));
  if (!hopping_flag) {
    ra->freq_hop_fl = hop_disabled;
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

  ra_type2_from_riv(riv, &ra->type2_alloc.L_crb, &ra->type2_alloc.RB_start,
      nof_prb, nof_prb);
  
  ra_mcs_from_idx_ul(ra->mcs_idx, ra_nprb_ul(ra, nof_prb), &ra->mcs);
  return LIBLTE_SUCCESS;
}

/* Creates the UL PUSCH resource allocation grant from a DCI format 0 message
 */
int dci_msg_to_ra_ul(dci_msg_t *msg, uint32_t nof_prb, uint32_t n_rb_ho, ra_pusch_t *ra_ul) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (msg               !=  NULL   &&
      ra_ul             !=  NULL)
  {
    ret = LIBLTE_ERROR;
    
    bzero(ra_ul, sizeof(ra_pusch_t));
    
    if (dci_msg_unpack_pusch(msg, ra_ul, nof_prb)) {
      fprintf(stderr, "Can't unpack PDSCH message\n");
      return ret;
    } 
    
    if (VERBOSE_ISINFO()) {
      ra_pusch_fprint(stdout, ra_ul, nof_prb);
    }
    
    if (ra_ul_alloc(&ra_ul->prb_alloc, ra_ul, n_rb_ho, nof_prb)) {
      fprintf(stderr, "Error computing resource allocation\n");
      return ret;
    }
    
    ret = LIBLTE_SUCCESS;
  }
  return ret;
}

int dci_location_set(dci_location_t *c, uint32_t L, uint32_t nCCE) {
  if (L <= 3) {
    c->L = L;
  } else {
    fprintf(stderr, "Invalid L %d\n", L);
    return LIBLTE_ERROR;
  }
  if (nCCE <= 87) {
    c->ncce = nCCE;
  } else {
    fprintf(stderr, "Invalid nCCE %d\n", nCCE);
    return LIBLTE_ERROR;
  }
  return LIBLTE_SUCCESS;
}

bool dci_location_isvalid(dci_location_t *c) {
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

  uint32_t n = (uint32_t) ceilf((float) nof_prb / ra_type0_P(nof_prb)) + 5 + 3 + 1 + 2
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
  uint32_t n_vrb_dl_gap1 = ra_type2_n_vrb_dl(nof_prb, true);
  uint32_t n_step = ra_type2_n_rb_step(nof_prb);
  uint32_t n = riv_nbits((uint32_t) n_vrb_dl_gap1 / n_step) + 5;
  if (nof_prb >= 50) {
    n++;
  }
  return n;
}

uint32_t dci_format_sizeof(dci_format_t format, uint32_t nof_prb) {
  switch (format) {
  case Format0:
    return dci_format0_sizeof(nof_prb);
  case Format1:
    return dci_format1_sizeof(nof_prb);
  case Format1A:
    return dci_format1A_sizeof(nof_prb);
  case Format1C:
    return dci_format1C_sizeof(nof_prb);
  default:
    return LIBLTE_ERROR;
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
int dci_format0_pack(ra_pusch_t *data, dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;
  uint32_t n_ul_hop;

  *y++ = 0; // format differentiation
  if (data->freq_hop_fl == hop_disabled) { // frequency hopping
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
    riv = ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start,
        nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  bit_pack(riv, &y, riv_nbits(nof_prb) - n_ul_hop);

  /* pack MCS according to 8.6.1 of 36.213 */
  bit_pack(data->mcs_idx, &y, 5);

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
  return LIBLTE_SUCCESS;
}
/* Unpacks DCI format 0 data and store result in msg according
 * to 36.212 5.3.3.1.1
 *
 * TODO: TPC and cyclic shift for DM RS not implemented
 */
int dci_format0_unpack(dci_msg_t *msg, ra_pusch_t *data, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;
  uint32_t n_ul_hop;

  /* Make sure it's a Format0 message */
  if (msg->nof_bits != dci_format_sizeof(Format0, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 0\n");
    return LIBLTE_ERROR;
  }
  if (*y++ != 0) {
    fprintf(stderr,
        "Invalid format differentiation field value. This is Format1A\n");
    return LIBLTE_ERROR;
  }
  if (*y++ == 0) {
    data->freq_hop_fl = hop_disabled;
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
  uint32_t riv = bit_unpack(&y, riv_nbits(nof_prb) - n_ul_hop);
  ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start,
      nof_prb, nof_prb);
  data->type2_alloc.riv = riv;

  /* unpack MCS according to 8.6 of 36.213 */
  data->mcs_idx = bit_unpack(&y, 5);

  data->ndi = *y++ ? true : false;

  // TCP and DM RS commands not implemented
  y += 5;

  // CQI request
  data->cqi_request = *y++ ? true : false;

  // 8.6.2 First paragraph
  if (data->mcs_idx <= 28) {
    ra_mcs_from_idx_ul(data->mcs_idx, ra_nprb_ul(data, nof_prb), &data->mcs);
  } else if (data->mcs_idx == 29 && data->cqi_request && ra_nprb_ul(data, nof_prb) <= 4) {
    // 8.6.1 and 8.6.2 36.213 second paragraph
    data->mcs.mod = LTE_QPSK;
    data->mcs.tbs = 0;
  } else if (data->mcs_idx >= 29) {
    // Else leave TBS and use the previously used PUSCH modulation
    data->mcs.tbs = 0;
    data->rv_idx = data->mcs_idx - 28;
  }

  return LIBLTE_SUCCESS;
}

/* Packs DCI format 1 data to a sequence of bits and store them in msg according
 * to 36.212 5.3.3.1.2
 *
 * TODO: TPC commands
 */

int dci_format1_pack(ra_pdsch_t *data, dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  if (nof_prb > 10) {
    *y++ = data->alloc_type;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P = ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t) ceilf((float) nof_prb / P);
  switch (data->alloc_type) {
  case alloc_type0:
    bit_pack((uint32_t) data->type0_alloc.rbg_bitmask, &y, alloc_size);
    break;
  case alloc_type1:
    bit_pack((uint32_t) data->type1_alloc.rbg_subset, &y, (int) ceilf(log2f(P)));
    *y++ = data->type1_alloc.shift ? 1 : 0;
    bit_pack((uint32_t) data->type1_alloc.vrb_bitmask, &y,
        alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr,
        "Format 1 accepts type0 or type1 resource allocation only\n");
    return LIBLTE_ERROR;

  }
  /* pack MCS */
  bit_pack(data->mcs_idx, &y, 5);

  /* harq process number */
  bit_pack(data->harq_process, &y, 3);

  *y++ = data->ndi;

  // rv version
  bit_pack(data->rv_idx, &y, 2);

  // TPC not implemented
  *y++ = 0;
  *y++ = 0;

  // Padding with zeros
  uint32_t n = dci_format1_sizeof(nof_prb);
  while (y - msg->data < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->data);

  return LIBLTE_SUCCESS;
}

int dci_format1_unpack(dci_msg_t *msg, ra_pdsch_t *data, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  /* Make sure it's a Format1 message */
  if (msg->nof_bits != dci_format_sizeof(Format1, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1\n");
    return LIBLTE_ERROR;
  }

  if (nof_prb > 10) {
    data->alloc_type = *y++;
  } else {
    data->alloc_type = alloc_type0;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P = ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t) ceilf((float) nof_prb / P);
  switch (data->alloc_type) {
  case alloc_type0:
    data->type0_alloc.rbg_bitmask = bit_unpack(&y, alloc_size);
    break;
  case alloc_type1:
    data->type1_alloc.rbg_subset = bit_unpack(&y, (int) ceilf(log2f(P)));
    data->type1_alloc.shift = *y++ ? true : false;
    data->type1_alloc.vrb_bitmask = bit_unpack(&y,
        alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr, "Format 1 accepts type0 or type1 resource allocation only\n");
    return LIBLTE_ERROR;

  }
  /* unpack MCS according to 7.1.7 of 36.213 */
  data->mcs_idx = bit_unpack(&y, 5);
  if (ra_mcs_from_idx_dl(data->mcs_idx, ra_nprb_dl(data, nof_prb), &data->mcs)) {
    fprintf(stderr, "Error getting MCS\n");
    return LIBLTE_ERROR;
  }
  
  /* harq process number */
  data->harq_process = bit_unpack(&y, 3);

  data->ndi = *y++ ? true : false;

  // rv version
  data->rv_idx = bit_unpack(&y, 2);

  // TPC not implemented

  return LIBLTE_SUCCESS;
}

/* Packs DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 * TODO: RA procedure initiated by PDCCH, TPC commands
 */
int dci_format1As_pack(ra_pdsch_t *data, dci_msg_t *msg, uint32_t nof_prb,
    bool crc_is_crnti) {

  /* pack bits */
  uint8_t *y = msg->data;

  *y++ = 1; // format differentiation

  if (data->alloc_type != alloc_type2) {
    fprintf(stderr, "Format 1A accepts type2 resource allocation only\n");
    return LIBLTE_ERROR;
  }

  *y++ = data->type2_alloc.mode; // localized or distributed VRB assignment

  if (data->type2_alloc.mode == t2_loc) {
    if (data->type2_alloc.L_crb > nof_prb) {
      fprintf(stderr, "L_CRB=%d can not exceed system BW for localized type2\n",
          data->type2_alloc.L_crb);
      return LIBLTE_ERROR;
    }
  } else {
    uint32_t n_vrb_dl;
    if (crc_is_crnti && nof_prb > 50) {
      n_vrb_dl = 16;
    } else {
      n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == t2_ng1);
    }
    if (data->type2_alloc.L_crb > n_vrb_dl) {
      fprintf(stderr,
          "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n",
          data->type2_alloc.L_crb, n_vrb_dl);
      return LIBLTE_ERROR;
    }
  }
  /* pack RIV according to 7.1.6.3 of 36.213 */
  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start,
        nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  uint32_t nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == t2_dist && nof_prb >= 50) {
    nb_gap = 1;
    *y++ = data->type2_alloc.n_gap;
  }
  bit_pack(riv, &y, riv_nbits(nof_prb) - nb_gap);

  // in format1A, MCS = TBS according to 7.1.7.2 of 36.213
  bit_pack(data->mcs_idx, &y, 5);

  bit_pack(data->harq_process, &y, 3);

  if (!crc_is_crnti && nof_prb >= 50 && data->type2_alloc.mode == t2_dist) {
    *y++ = data->type2_alloc.n_gap;
  } else {
    y++; // bit reserved
  }

  // rv version
  bit_pack(data->rv_idx, &y, 2);

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

  return LIBLTE_SUCCESS;
}

/* Unpacks DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 */
int dci_format1As_unpack(dci_msg_t *msg, ra_pdsch_t *data, uint32_t nof_prb,
    bool crc_is_crnti) {

  /* pack bits */
  uint8_t *y = msg->data;

  /* Make sure it's a Format0 message */
  if (msg->nof_bits != dci_format_sizeof(Format1A, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1A\n");
    return LIBLTE_ERROR;
  }

  if (*y++ != 1) {
    fprintf(stderr, "Invalid format differentiation field value. This is Format0\n");
    return LIBLTE_ERROR;
  }

  data->alloc_type = alloc_type2;
  data->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  data->type2_alloc.n_gap = t2_ng1;

  /* unpack RIV according to 7.1.6.3 of 36.213 */
  uint32_t nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == t2_dist && nof_prb >= 50) {
    nb_gap = 1;
    data->type2_alloc.n_gap = *y++;
  }
  uint32_t nof_vrb;
  if (data->type2_alloc.mode == t2_loc) {
    nof_vrb = nof_prb;
  } else {
    nof_vrb = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == t2_ng1);
  }
  uint32_t riv = bit_unpack(&y, riv_nbits(nof_prb) - nb_gap);
  ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start,
      nof_prb, nof_vrb);
  data->type2_alloc.riv = riv;

  // unpack MCS
  data->mcs_idx = bit_unpack(&y, 5);

  data->harq_process = bit_unpack(&y, 3);

  if (!crc_is_crnti && nof_prb >= 50 && data->type2_alloc.mode == t2_dist) {
    data->type2_alloc.n_gap = *y++;
  } else {
    y++; // bit reserved
  }

  // rv version
  bit_pack(data->rv_idx, &y, 2);

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
    n_prb = ra_nprb_dl(data, nof_prb);
  } else {
    n_prb = data->type2_alloc.n_prb1a == nprb1a_2 ? 2 : 3;
  }
  data->mcs.tbs = ra_tbs_from_idx(data->mcs_idx, n_prb);
  data->mcs.mod = LTE_QPSK;

  return LIBLTE_SUCCESS;
}

/* Format 1C for compact scheduling of PDSCH words
 *
 */
int dci_format1Cs_pack(ra_pdsch_t *data, dci_msg_t *msg, uint32_t nof_prb) {

  /* pack bits */
  uint8_t *y = msg->data;

  if (data->alloc_type != alloc_type2 || data->type2_alloc.mode != t2_dist) {
    fprintf(stderr,
        "Format 1C accepts distributed type2 resource allocation only\n");
    return LIBLTE_ERROR;
  }

  if (nof_prb >= 50) {
    *y++ = data->type2_alloc.n_gap;
  }
  uint32_t n_step = ra_type2_n_rb_step(nof_prb);
  uint32_t n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == t2_ng1);

  if (data->type2_alloc.L_crb > ((uint32_t) n_vrb_dl / n_step) * n_step) {
    fprintf(stderr, "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n",
        data->type2_alloc.L_crb, ((uint32_t) n_vrb_dl / n_step) * n_step);
    return LIBLTE_ERROR;
  }
  if (data->type2_alloc.L_crb % n_step) {
    fprintf(stderr, "L_crb must be multiple of n_step\n");
    return LIBLTE_ERROR;
  }
  if (data->type2_alloc.RB_start % n_step) {
    fprintf(stderr, "RB_start must be multiple of n_step\n");
    return LIBLTE_ERROR;
  }
  uint32_t L_p = data->type2_alloc.L_crb / n_step;
  uint32_t RB_p = data->type2_alloc.RB_start / n_step;
  uint32_t n_vrb_p = (int) n_vrb_dl / n_step;

  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = ra_type2_to_riv(L_p, RB_p, n_vrb_p);
  } else {
    riv = data->type2_alloc.riv;
  }
  bit_pack(riv, &y, riv_nbits((int) n_vrb_dl / n_step));

  // in format1C, MCS = TBS according to 7.1.7.2 of 36.213
  bit_pack(data->mcs_idx, &y, 5);

  msg->nof_bits = (y - msg->data);

  return LIBLTE_SUCCESS;
}

int dci_format1Cs_unpack(dci_msg_t *msg, ra_pdsch_t *data, uint32_t nof_prb) {
  uint32_t L_p, RB_p;

  /* pack bits */
  uint8_t *y = msg->data;

  if (msg->nof_bits != dci_format_sizeof(Format1C, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1C\n");
    return LIBLTE_ERROR;
  }
  data->alloc_type = alloc_type2;
  data->type2_alloc.mode = t2_dist;
  if (nof_prb >= 50) {
    data->type2_alloc.n_gap = *y++;
  }
  uint32_t n_step = ra_type2_n_rb_step(nof_prb);
  uint32_t n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == t2_ng1);

  uint32_t riv = bit_unpack(&y, riv_nbits((int) n_vrb_dl / n_step));
  uint32_t n_vrb_p = (uint32_t) n_vrb_dl / n_step;

  ra_type2_from_riv(riv, &L_p, &RB_p, n_vrb_p, n_vrb_p);
  data->type2_alloc.L_crb = L_p * n_step;
  data->type2_alloc.RB_start = RB_p * n_step;
  data->type2_alloc.riv = riv;

  data->mcs_idx = bit_unpack(&y, 5);
  data->mcs.tbs = ra_tbs_from_idx_format1c(data->mcs_idx);
  data->mcs.mod = LTE_QPSK;

  msg->nof_bits = (y - msg->data);

  return LIBLTE_SUCCESS;
}

int dci_msg_pack_pdsch(ra_pdsch_t *data, dci_msg_t *msg, dci_format_t format,
    uint32_t nof_prb, bool crc_is_crnti) {
  switch (format) {
  case Format1:
    return dci_format1_pack(data, msg, nof_prb);
  case Format1A:
    return dci_format1As_pack(data, msg, nof_prb, crc_is_crnti);
  case Format1C:
    return dci_format1Cs_pack(data, msg, nof_prb);
  default:
    fprintf(stderr, "Invalid DCI format %s for PDSCH resource allocation\n",
        dci_format_string(format));
    return LIBLTE_ERROR;
  }
}

int dci_msg_unpack_pdsch(dci_msg_t *msg, ra_pdsch_t *data, uint32_t nof_prb,
    bool crc_is_crnti) {
  if (msg->nof_bits == dci_format_sizeof(Format1, nof_prb)) {
    return dci_format1_unpack(msg, data, nof_prb);
  } else if (msg->nof_bits == dci_format_sizeof(Format1A, nof_prb)) {
    return dci_format1As_unpack(msg, data, nof_prb, crc_is_crnti);
  } else if (msg->nof_bits == dci_format_sizeof(Format1C, nof_prb)) {
    return dci_format1Cs_unpack(msg, data, nof_prb);
  } else {
    return LIBLTE_ERROR;
  }
}

int dci_msg_pack_pusch(ra_pusch_t *data, dci_msg_t *msg, uint32_t nof_prb) {
  return dci_format0_pack(data, msg, nof_prb);
}

int dci_msg_unpack_pusch(dci_msg_t *msg, ra_pusch_t *data, uint32_t nof_prb) {
  return dci_format0_unpack(msg, data, nof_prb);
}

dci_format_t dci_format_from_string(char *str) {
  if (!strcmp(str, "Format0")) {
    return Format0;
  } else if (!strcmp(str, "Format1")) {
    return Format1; 
  } else if (!strcmp(str, "Format1A")) {
    return Format1A; 
  } else if (!strcmp(str, "Format1C")) {
    return Format1C; 
  } else {
    return FormatError;
  }
}

char* dci_format_string(dci_format_t format) {
  switch (format) {
  case Format0:
    return "Format0";
  case Format1:
    return "Format1";
  case Format1A:
    return "Format1A";
  case Format1C:
    return "Format1C";
  default:
    return "N/A"; // fatal error
  }
}

void dci_msg_type_fprint(FILE *f, dci_msg_type_t type) {
  switch (type.type) {
  case PUSCH_SCHED:
    fprintf(f, "%s PUSCH Scheduling\n", dci_format_string(type.format));
    break;
  case PDSCH_SCHED:
    fprintf(f, "%s PDSCH Scheduling\n", dci_format_string(type.format));
    break;
  case RA_PROC_PDCCH:
    fprintf(f, "%s Random access initiated by PDCCH\n",
        dci_format_string(type.format));
    break;
  case MCCH_CHANGE:
    fprintf(f, "%s MCCH change notification\n", dci_format_string(type.format));
    break;
  case TPC_COMMAND:
    fprintf(f, "%s TPC command\n", dci_format_string(type.format));
    break;
  }
}

int dci_msg_get_type(dci_msg_t *msg, dci_msg_type_t *type, uint32_t nof_prb,
    uint16_t msg_rnti) 
{
  DEBUG("Get message type: nof_bits=%d, msg_rnti=0x%x\n", msg->nof_bits, msg_rnti);
  if (msg->nof_bits == dci_format_sizeof(Format0, nof_prb)
      && !msg->data[0]) {
    type->type = PUSCH_SCHED;
    type->format = Format0;
    return LIBLTE_SUCCESS;
  } else if (msg->nof_bits == dci_format_sizeof(Format1, nof_prb)) {
    type->type = PDSCH_SCHED; // only these 2 types supported
    type->format = Format1;
    return LIBLTE_SUCCESS;
  } else if (msg->nof_bits == dci_format_sizeof(Format1A, nof_prb)) {
    /* The RNTI is not the only condition. Also some fields in the packet. 
     * if (msg_rnti >= CRNTI_START && msg_rnti <= CRNTI_END) {
      type->type = RA_PROC_PDCCH;
      type->format = Format1A;
    } else {
      */
      type->type = PDSCH_SCHED; // only these 2 types supported
      type->format = Format1A;
    //}
    return LIBLTE_SUCCESS;
  } else if (msg->nof_bits == dci_format_sizeof(Format1C, nof_prb)) {
    if (msg_rnti == MRNTI) {
      type->type = MCCH_CHANGE;
      type->format = Format1C;
    } else {
      type->type = PDSCH_SCHED; // only these 2 types supported
      type->format = Format1C;
    }
    return LIBLTE_SUCCESS;
  }
  return LIBLTE_ERROR;
}

