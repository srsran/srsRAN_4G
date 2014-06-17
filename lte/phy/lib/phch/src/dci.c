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
#include "liblte/phy/common/base.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

int dci_init(dci_t *q, int max_dcis) {
  q->msg = calloc(sizeof(dci_msg_t), max_dcis);
  if (!q->msg) {
    perror("malloc");
    return -1;
  }
  q->nof_dcis = 0;
  q->max_dcis = max_dcis;
  return 0;
}

void dci_free(dci_t *q) {
  if (q->msg) {
    free(q->msg);
  }
}

void dci_candidate_fprint(FILE *f, dci_candidate_t *q) {
  fprintf(f, "L: %d, nCCE: %d, RNTI: 0x%x, nBits: %d\n",
      q->L, q->ncce, q->rnti, q->nof_bits);
}

int dci_msg_candidate_set(dci_msg_t *msg, int L, int nCCE, unsigned short rnti) {
  if (L >= 0 && L <=3) {
    msg->location.L = (unsigned char) L;
  } else {
    fprintf(stderr, "Invalid L %d\n", L);
    return -1;
  }
  if (nCCE >= 0 && nCCE <= 87) {
    msg->location.ncce = (unsigned char) nCCE;
  } else {
    fprintf(stderr, "Invalid nCCE %d\n", nCCE);
    return -1;
  }
  msg->location.rnti = rnti;
  return 0;
}

int riv_nbits(int nof_prb) {
  return (int) ceilf(log2f((float) nof_prb*((float) nof_prb+1)/2));
}

const int ambiguous_sizes[10] = {12, 14, 16, 20, 24, 26, 32, 40, 44, 56};

bool is_ambiguous_size(int size) {
  int i;
  for (i=0;i<10;i++) {
    if (size == ambiguous_sizes[i]) {
      return true;
    }
  }
  return false;
}



/**********************************
 *  PAYLOAD sizeof functions
 * ********************************/
int dci_format0_sizeof_(int nof_prb) {
  return 1+1+riv_nbits(nof_prb)+5+1+2+3+1;
}


int dci_format1A_sizeof(int nof_prb) {
  int n;
  n = 1+1+riv_nbits(nof_prb)+5+3+1+2+2;
  while(n < dci_format0_sizeof_(nof_prb)) {
    n++;
  }
  if (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}


int dci_format0_sizeof(int nof_prb) {
  int n = dci_format0_sizeof_(nof_prb);
  while (n < dci_format1A_sizeof(nof_prb)) {
    n++;
  }
  return n;
}

int dci_format1_sizeof(int nof_prb) {

  int n = (int) ceilf((float) nof_prb/ra_type0_P(nof_prb))+5+3+1+2+2;
  if (nof_prb > 10) {
    n++;
  }
  while(n == dci_format0_sizeof(nof_prb)
      || n == dci_format1A_sizeof(nof_prb)
      || is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

int dci_format1C_sizeof(int nof_prb) {
  int n_vrb_dl_gap1 = ra_type2_n_vrb_dl(nof_prb, true);
  int n_step = ra_type2_n_rb_step(nof_prb);
  int n = + riv_nbits((int) n_vrb_dl_gap1/n_step) + 5;
  if (nof_prb >= 50) {
    n++;
  }
  return n;
}

int dci_format_sizeof(dci_format_t format, int nof_prb) {
  switch(format) {
  case Format0:
    return dci_format0_sizeof(nof_prb);
  case Format1:
    return dci_format1_sizeof(nof_prb);
  case Format1A:
    return dci_format1A_sizeof(nof_prb);
  case Format1C:
    return dci_format1C_sizeof(nof_prb);
  default:
    return -1;
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
int dci_format0_pack(ra_pusch_t *data, dci_msg_t *msg, int nof_prb) {

  /* pack bits */
  char *y = msg->data;
  int n_ul_hop;

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
    riv = ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start, nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  bit_pack(riv, &y, riv_nbits(nof_prb) - n_ul_hop);

  /* pack MCS according to 8.6.1 of 36.213 */
  uint32_t mcs;
  if (data->cqi_request) {
    mcs = 29;
  } else {
    if (data->rv_idx) {
      mcs = 28 + data->rv_idx;
    } else {
      if (data->mcs.mod == MOD_NULL) {
        mcs = data->mcs.mcs_idx;
      } else {
        if (data->mcs.tbs) {
          if (data->mcs.tbs) {
            data->mcs.tbs_idx = ra_tbs_to_table_idx(data->mcs.tbs, ra_nprb_ul(data, nof_prb));
          }
        }
        mcs = ra_mcs_to_table_idx(&data->mcs);
      }
    }
  }

  bit_pack(mcs, &y, 5);

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
  int n = dci_format0_sizeof(nof_prb);
  while (y-msg->data < n) {
    *y++ = 0;
  }
  msg->location.nof_bits = (y - msg->data);
  return 0;
}
/* Unpacks DCI format 0 data and store result in msg according
 * to 36.212 5.3.3.1.1
 *
 * TODO: TPC and cyclic shift for DM RS not implemented
 */
int dci_format0_unpack(dci_msg_t *msg, ra_pusch_t *data, int nof_prb) {

  /* pack bits */
  char *y = msg->data;
  int n_ul_hop;

  /* Make sure it's a Format0 message */
  if (msg->location.nof_bits != dci_format_sizeof(Format0, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 0\n");
    return -1;
  }
  if (*y++ != 0) {
    fprintf(stderr, "Invalid format differentiation field value. This is Format1A\n");
    return -1;
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
      data->freq_hop_fl = y[0]<<1 | y[1];
      y += 2;
    }
  }
  /* unpack RIV according to 8.1 of 36.213 */
  uint32_t riv = bit_unpack(&y, riv_nbits(nof_prb) - n_ul_hop);
  ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start, nof_prb, nof_prb);
  bit_pack(riv, &y, riv_nbits(nof_prb) - n_ul_hop);
  data->type2_alloc.riv = riv;

  /* unpack MCS according to 8.6 of 36.213 */
  uint32_t mcs = bit_unpack(&y, 5);

  data->ndi = *y++?true:false;

  // TCP and DM RS commands not implemented
  y+= 5;

  // CQI request
  data->cqi_request = *y++?true:false;

  // 8.6.2 First paragraph
  if (mcs <= 28) {
    ra_mcs_from_idx_ul(mcs, &data->mcs);
    data->mcs.tbs = ra_tbs_from_idx(data->mcs.tbs_idx, ra_nprb_ul(data, nof_prb));
  }

  // 8.6.1 and 8.6.2 36.213 second paragraph
  if (mcs == 29 && data->cqi_request && ra_nprb_ul(data, nof_prb) <= 4) {
      data->mcs.mod = QPSK;
  }
  if (mcs > 29) {
    // Else leave MOD_NULL and use the previously used PUSCH modulation
    data->mcs.mod = MOD_NULL;
    data->rv_idx = mcs - 28;
  }

  return 0;
}

/* Packs DCI format 1 data to a sequence of bits and store them in msg according
 * to 36.212 5.3.3.1.2
 *
 * TODO: TPC commands
 */

int dci_format1_pack(ra_pdsch_t *data, dci_msg_t *msg, int nof_prb) {

  /* pack bits */
  char *y = msg->data;

  if (nof_prb > 10) {
    *y++ = data->alloc_type;
  }

  /* Resource allocation: type0 or type 1 */
  int P = ra_type0_P(nof_prb);
  int alloc_size = (int) ceilf((float) nof_prb/P);
  switch(data->alloc_type) {
  case alloc_type0:
    bit_pack(data->type0_alloc.rbg_bitmask, &y, alloc_size);
    break;
  case alloc_type1:
    bit_pack(data->type1_alloc.rbg_subset, &y, (int) ceilf(log2f(P)));
    *y++ = data->type1_alloc.shift?1:0;
    bit_pack(data->type1_alloc.vrb_bitmask, &y, alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr, "Format 1 accepts type0 or type1 resource allocation only\n");
    return -1;

  }
  /* pack MCS according to 7.1.7 of 36.213 */
  uint32_t mcs;
  if (data->mcs.mod == MOD_NULL) {
    mcs = data->mcs.mcs_idx;
  } else {
    if (data->mcs.tbs) {
      data->mcs.tbs_idx = ra_tbs_to_table_idx(data->mcs.tbs, ra_nprb_dl(data, nof_prb));
    }
    mcs = ra_mcs_to_table_idx(&data->mcs);
  }
  bit_pack(mcs, &y, 5);

  /* harq process number */
  bit_pack(data->harq_process, &y, 3);

  *y++ = data->ndi;

  // rv version
  bit_pack(data->rv_idx, &y, 2);

  // TPC not implemented
  *y++ = 0;
  *y++ = 0;

  // Padding with zeros
  int n = dci_format1_sizeof(nof_prb);
  while (y-msg->data < n) {
    *y++ = 0;
  }
  msg->location.nof_bits = (y - msg->data);

  return 0;
}

int dci_format1_unpack(dci_msg_t *msg, ra_pdsch_t *data, int nof_prb) {

  /* pack bits */
  char *y = msg->data;

  /* Make sure it's a Format1 message */
  if (msg->location.nof_bits != dci_format_sizeof(Format1, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1\n");
    return -1;
  }

  if (nof_prb > 10) {
    data->alloc_type = *y++;
  } else {
    data->alloc_type = alloc_type0;
  }

  /* Resource allocation: type0 or type 1 */
  int P = ra_type0_P(nof_prb);
  int alloc_size = (int) ceilf((float) nof_prb/P);
  switch(data->alloc_type) {
  case alloc_type0:
    data->type0_alloc.rbg_bitmask = bit_unpack(&y, alloc_size);
    break;
  case alloc_type1:
    data->type1_alloc.rbg_subset = bit_unpack(&y, (int) ceilf(log2f(P)));
    data->type1_alloc.shift = *y++?true:false;
    data->type1_alloc.vrb_bitmask = bit_unpack(&y, alloc_size - (int) ceilf(log2f(P)) - 1);
    break;
  default:
    fprintf(stderr, "Format 1 accepts type0 or type1 resource allocation only\n");
    return -1;

  }
  /* pack MCS according to 7.1.7 of 36.213 */
  uint32_t mcs = bit_unpack(&y, 5);
  data->mcs.mcs_idx = mcs;
  ra_mcs_from_idx_dl(mcs, &data->mcs);
  data->mcs.tbs = ra_tbs_from_idx(data->mcs.tbs_idx, ra_nprb_dl(data, nof_prb));

  /* harq process number */
  data->harq_process = bit_unpack(&y, 3);

  data->ndi = *y++?true:false;

  // rv version
  data->rv_idx = bit_unpack(&y, 2);

  // TPC not implemented


  return 0;
}


/* Packs DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 * TODO: RA procedure initiated by PDCCH, TPC commands
 */
int dci_format1As_pack(ra_pdsch_t *data, dci_msg_t *msg, int nof_prb, bool crc_is_crnti) {

  /* pack bits */
  char *y = msg->data;

  *y++ = 1; // format differentiation

  if (data->alloc_type != alloc_type2) {
    fprintf(stderr, "Format 1A accepts type2 resource allocation only\n");
    return -1;
  }

  *y++ = data->type2_alloc.mode; // localized or distributed VRB assignment

  if (data->type2_alloc.mode == t2_loc) {
    if (data->type2_alloc.L_crb > nof_prb) {
      fprintf(stderr, "L_CRB=%d can not exceed system BW for localized type2\n", data->type2_alloc.L_crb);
      return -1;
    }
  } else {
    int n_vrb_dl;
    if (crc_is_crnti && nof_prb > 50) {
      n_vrb_dl = 16;
    } else {
      n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap==t2_ng1);
    }
    if (data->type2_alloc.L_crb > n_vrb_dl) {
      fprintf(stderr, "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n", data->type2_alloc.L_crb, n_vrb_dl);
      return -1;
    }
  }
  /* pack RIV according to 7.1.6.3 of 36.213 */
  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = ra_type2_to_riv(data->type2_alloc.L_crb, data->type2_alloc.RB_start, nof_prb);
  } else {
    riv = data->type2_alloc.riv;
  }
  int nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == t2_dist && nof_prb >= 50) {
    nb_gap = 1;
    *y++ = data->type2_alloc.n_gap;
  }
  bit_pack(riv, &y, riv_nbits(nof_prb)-nb_gap);

  // in format1A, MCS = TBS according to 7.1.7.2 of 36.213
  uint32_t mcs;
  if (data->mcs.mod == MOD_NULL) {
    mcs = data->mcs.mcs_idx;
  } else {
    if (data->mcs.tbs) {
      // In format 1A, n_prb_1a is 2 or 3 if crc is not scrambled with C-RNTI
      int n_prb;
      if (!crc_is_crnti) {
        n_prb = ra_nprb_dl(data, nof_prb);
      } else {
        n_prb = data->type2_alloc.n_prb1a==nprb1a_2?2:3;
      }
      data->mcs.tbs_idx = ra_tbs_to_table_idx(data->mcs.tbs, n_prb);
    }
    mcs = data->mcs.tbs_idx;
  }
  bit_pack(mcs, &y, 5);

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
  int n = dci_format1A_sizeof(nof_prb);
  while (y-msg->data < n) {
    *y++ = 0;
  }
  msg->location.nof_bits = (y - msg->data);

  return 0;
}

/* Unpacks DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 */
int dci_format1As_unpack(dci_msg_t *msg, ra_pdsch_t *data, int nof_prb, bool crc_is_crnti) {

  /* pack bits */
  char *y = msg->data;

  /* Make sure it's a Format0 message */
  if (msg->location.nof_bits != dci_format_sizeof(Format1A, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1A\n");
    return -1;
  }
  if (*y++ != 1) {
    fprintf(stderr, "Invalid format differentiation field value. This is Format0\n");
    return -1;
  }

  data->alloc_type = alloc_type2;
  data->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  data->type2_alloc.n_gap = t2_ng1;

  /* unpack RIV according to 7.1.6.3 of 36.213 */
  int nb_gap = 0;
  if (crc_is_crnti && data->type2_alloc.mode == t2_dist && nof_prb >= 50) {
    nb_gap = 1;
    data->type2_alloc.n_gap = *y++;
  }
  int nof_vrb;
  if (data->type2_alloc.mode == t2_loc) {
    nof_vrb = nof_prb;
  } else {
    nof_vrb = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap == t2_ng1);
  }
  uint32_t riv = bit_unpack(&y, riv_nbits(nof_prb) - nb_gap);
  ra_type2_from_riv(riv, &data->type2_alloc.L_crb, &data->type2_alloc.RB_start, nof_prb, nof_vrb);
  data->type2_alloc.riv = riv;

  // unpack MCS
  data->mcs.mcs_idx = bit_unpack(&y, 5);

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
    *y++ = data->type2_alloc.n_prb1a; // LSB indicates N_prb_1a for TBS
  }
  data->mcs.tbs_idx = data->mcs.mcs_idx;
  int n_prb;
  if (crc_is_crnti) {
    n_prb = ra_nprb_dl(data, nof_prb);
  } else {
    n_prb = data->type2_alloc.n_prb1a==nprb1a_2?2:3;
  }
  data->mcs.tbs = ra_tbs_from_idx(data->mcs.tbs_idx, n_prb);
  data->mcs.mod = QPSK;

  return 0;
}

/* Format 1C for compact scheduling of PDSCH words
 *
 */
int dci_format1Cs_pack(ra_pdsch_t *data, dci_msg_t *msg, int nof_prb) {

  /* pack bits */
  char *y = msg->data;

  if (data->alloc_type != alloc_type2 || data->type2_alloc.mode != t2_dist) {
    fprintf(stderr, "Format 1C accepts distributed type2 resource allocation only\n");
    return -1;
  }

  if (nof_prb >= 50) {
    *y++ = data->type2_alloc.n_gap;
  }
  int n_step = ra_type2_n_rb_step(nof_prb);
  int n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap==t2_ng1);

  if (data->type2_alloc.L_crb > ((int) n_vrb_dl/n_step)*n_step) {
    fprintf(stderr, "L_CRB=%d can not exceed N_vrb_dl=%d for distributed type2\n", data->type2_alloc.L_crb,
        ((int) n_vrb_dl/n_step)*n_step);
    return -1;
  }
  if (data->type2_alloc.L_crb % n_step) {
    fprintf(stderr, "L_crb must be multiple of n_step\n");
    return -1;
  }
  if (data->type2_alloc.RB_start % n_step) {
    fprintf(stderr, "RB_start must be multiple of n_step\n");
    return -1;
  }
  int L_p = data->type2_alloc.L_crb/n_step;
  int RB_p = data->type2_alloc.RB_start/n_step;
  int n_vrb_p = (int) n_vrb_dl / n_step;

  uint32_t riv;
  if (data->type2_alloc.L_crb) {
    riv = ra_type2_to_riv(L_p, RB_p, n_vrb_p);
  } else {
    riv = data->type2_alloc.riv;
  }
  bit_pack(riv, &y, riv_nbits((int) n_vrb_dl/n_step));

  // in format1C, MCS = TBS according to 7.1.7.2 of 36.213
  uint32_t mcs;
  if (data->mcs.mod == MOD_NULL) {
    mcs = data->mcs.mcs_idx;
  } else {
    if (data->mcs.tbs) {
      data->mcs.tbs_idx = ra_tbs_to_table_idx_format1c(data->mcs.tbs);
    }
    mcs = data->mcs.tbs_idx;
  }
  bit_pack(mcs, &y, 5);

  msg->location.nof_bits = (y - msg->data);

  return 0;
}

int dci_format1Cs_unpack(dci_msg_t *msg, ra_pdsch_t *data, int nof_prb) {
  uint16_t L_p, RB_p;

  /* pack bits */
  char *y = msg->data;

  if (msg->location.nof_bits != dci_format_sizeof(Format1C, nof_prb)) {
    fprintf(stderr, "Invalid message length for format 1C\n");
    return -1;
  }
  data->alloc_type = alloc_type2;
  data->type2_alloc.mode = t2_dist;
  if (nof_prb >= 50) {
    data->type2_alloc.n_gap = *y++;
  }
  int n_step = ra_type2_n_rb_step(nof_prb);
  int n_vrb_dl = ra_type2_n_vrb_dl(nof_prb, data->type2_alloc.n_gap==t2_ng1);

  uint32_t riv = bit_unpack(&y, riv_nbits((int) n_vrb_dl/n_step));
  int n_vrb_p = (int) n_vrb_dl / n_step;

  ra_type2_from_riv(riv, &L_p, &RB_p, n_vrb_p, n_vrb_p);
  data->type2_alloc.L_crb = L_p * n_step;
  data->type2_alloc.RB_start = RB_p * n_step;
  data->type2_alloc.riv = riv;

  data->mcs.mcs_idx = bit_unpack(&y, 5);
  data->mcs.tbs_idx = data->mcs.mcs_idx;
  data->mcs.tbs = ra_tbs_from_idx_format1c(data->mcs.tbs_idx);
  data->mcs.mod = QPSK;

  msg->location.nof_bits = (y - msg->data);

  return 0;
}

int dci_msg_pack_pdsch(ra_pdsch_t *data, dci_msg_t *msg, dci_format_t format, int nof_prb, bool crc_is_crnti) {
  switch(format) {
  case Format1:
    return dci_format1_pack(data, msg, nof_prb);
  case Format1A:
    return dci_format1As_pack(data, msg, nof_prb, crc_is_crnti);
  case Format1C:
    return dci_format1Cs_pack(data, msg, nof_prb);
  default:
    fprintf(stderr, "Invalid DCI format %s for PDSCH resource allocation\n", dci_format_string(format));
    return -1;
  }
}

int dci_msg_unpack_pdsch(dci_msg_t *msg, ra_pdsch_t *data, int nof_prb, bool crc_is_crnti) {
  if (msg->location.nof_bits == dci_format_sizeof(Format1, nof_prb)) {
    return dci_format1_unpack(msg, data, nof_prb);
  } else if (msg->location.nof_bits == dci_format_sizeof(Format1A, nof_prb)) {
    return dci_format1As_unpack(msg, data, nof_prb, crc_is_crnti);
  } else if (msg->location.nof_bits == dci_format_sizeof(Format1C, nof_prb)) {
    return dci_format1Cs_unpack(msg, data, nof_prb);
  } else {
    return -1;
  }
}

int dci_msg_pack_pusch(ra_pusch_t *data, dci_msg_t *msg, int nof_prb) {
  return dci_format0_pack(data, msg, nof_prb);
}

int dci_msg_unpack_pusch(dci_msg_t *msg, ra_pusch_t *data, int nof_prb) {
  return dci_format0_unpack(msg, data, nof_prb);
}

char* dci_format_string(dci_format_t format) {
  switch(format) {
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
  switch(type.type) {
  case PUSCH_SCHED:
    fprintf(f,"%s PUSCH Scheduling\n", dci_format_string(type.format));
    break;
  case PDSCH_SCHED:
    fprintf(f,"%s PDSCH Scheduling\n", dci_format_string(type.format));
    break;
  case RA_PROC_PDCCH:
    fprintf(f,"%s Random access initiated by PDCCH\n", dci_format_string(type.format));
    break;
  case MCCH_CHANGE:
    fprintf(f,"%s MCCH change notification\n", dci_format_string(type.format));
    break;
  case TPC_COMMAND:
    fprintf(f,"%s TPC command\n", dci_format_string(type.format));
    break;
  }
}

int dci_msg_get_type(dci_msg_t *msg, dci_msg_type_t *type, int nof_prb, unsigned short crnti) {
  if (msg->location.nof_bits == dci_format_sizeof(Format0, nof_prb)
      && !msg->data[0]) {
    type->type = PUSCH_SCHED;
    type->format = Format0;
    return 0;
  } else if (msg->location.nof_bits == dci_format_sizeof(Format1, nof_prb)) {
    type->type = PDSCH_SCHED; // only these 2 types supported
    type->format = Format1;
    return 0;
  } else if (msg->location.nof_bits == dci_format_sizeof(Format1A, nof_prb)) {
    if (msg->location.rnti == crnti) {
      type->type = RA_PROC_PDCCH;
      type->format = Format1A;
    } else {
      type->type = PDSCH_SCHED; // only these 2 types supported
      type->format = Format1A;
    }
    return 0;
  } else if (msg->location.nof_bits == dci_format_sizeof(Format1C, nof_prb)) {
    if (msg->location.rnti == MRNTI) {
      type->type = MCCH_CHANGE;
      type->format = Format1C;
    } else {
      type->type = PDSCH_SCHED; // only these 2 types supported
      type->format = Format1C;
    }
    return 0;
  }
  return -1;
}

