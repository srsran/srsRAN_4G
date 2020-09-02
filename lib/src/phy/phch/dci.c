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

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/srslte.h"

#define IS_TDD (cell->frame_type == SRSLTE_TDD)
#define IS_TDD_CFG0 (IS_TDD && (sf->tdd_config.sf_config == 0))
#define HARQ_PID_LEN ((IS_TDD | IS_TDD_CFG0) ? 4 : 3)

/* Unpack RAR UL dci as defined in Section 6.2 of 36.213 */
void srslte_dci_rar_unpack(uint8_t payload[SRSLTE_RAR_GRANT_LEN], srslte_dci_rar_grant_t* rar)
{
  uint8_t* ptr      = payload;
  rar->hopping_flag = srslte_bit_pack(&ptr, 1) ? true : false;
  rar->rba          = srslte_bit_pack(&ptr, 10);
  rar->trunc_mcs    = srslte_bit_pack(&ptr, 4);
  rar->tpc_pusch    = srslte_bit_pack(&ptr, 3);
  rar->ul_delay     = srslte_bit_pack(&ptr, 1) ? true : false;
  rar->cqi_request  = srslte_bit_pack(&ptr, 1) ? true : false;
}

/* Pack RAR UL dci as defined in Section 6.2 of 36.213 */
void srslte_dci_rar_pack(srslte_dci_rar_grant_t* rar, uint8_t payload[SRSLTE_RAR_GRANT_LEN])
{
  uint8_t* ptr = payload;
  srslte_bit_unpack(rar->hopping_flag ? 1 : 0, &ptr, 1);
  srslte_bit_unpack(rar->rba, &ptr, 10);
  srslte_bit_unpack(rar->trunc_mcs, &ptr, 4);
  srslte_bit_unpack(rar->tpc_pusch, &ptr, 3);
  srslte_bit_unpack(rar->ul_delay ? 1 : 0, &ptr, 1);
  srslte_bit_unpack(rar->cqi_request ? 1 : 0, &ptr, 1);
}

/* Creates an equivalent DCI UL grant from the random access respone message
 */
int srslte_dci_rar_to_ul_dci(srslte_cell_t* cell, srslte_dci_rar_grant_t* rar, srslte_dci_ul_t* dci_ul)
{
  bzero(dci_ul, sizeof(srslte_dci_ul_t));

  if (!rar->hopping_flag) {
    dci_ul->freq_hop_fl = SRSLTE_RA_PUSCH_HOP_DISABLED;
  } else {
    ERROR("TODO: Frequency hopping in RAR not implemented\n");
    dci_ul->freq_hop_fl = 1;
  }
  uint32_t riv = rar->rba;
  // Truncate resource block assignment
  uint32_t b = 0;
  if (cell->nof_prb <= 44) {
    b   = (uint32_t)(ceilf(log2((float)cell->nof_prb * (cell->nof_prb + 1) / 2)));
    riv = riv & ((1 << (b + 1)) - 1);
  }
  dci_ul->type2_alloc.riv = riv;
  dci_ul->tb.mcs_idx      = rar->trunc_mcs;
  dci_ul->tb.rv           = -1;
  dci_ul->dai             = 3;

  return SRSLTE_SUCCESS;
}

static uint32_t riv_nbits(uint32_t nof_prb)
{
  return (uint32_t)ceilf(log2f((float)nof_prb * ((float)nof_prb + 1) / 2));
}

const uint32_t ambiguous_sizes[10] = {12, 14, 16, 20, 24, 26, 32, 40, 44, 56};

static bool is_ambiguous_size(uint32_t size)
{
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
static uint32_t dci_format0_sizeof_(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = 0;

  /* Carrier indicator – 0 or 3 bits */
  n += (cfg->cif_enabled) ? 3 : 0;

  /* Flag for format0/format1A differentiation – 1 bit */
  n += 1;

  /* Frequency hopping flag – 1 bit */
  n += 1;

  /* Resource block assignment and hopping resource allocation */
  n += riv_nbits(cell->nof_prb);

  /* Modulation and coding scheme and redundancy version – 5 bits */
  n += 5;

  /* New data indicator – 1 bit */
  n += 1;

  /* TPC command for scheduled PUSCH – 2 bits */
  n += 2;

  /* Cyclic shift for DM RS and OCC index – 3 bits */
  n += 3;

  /* Downlink Assignment Index (DAI) or UL Index – 2 bits (TDD) */
  n += ((IS_TDD | IS_TDD_CFG0) ? 2 : 0);

  /* CSI request – 1 or 2 bits */
  n += (cfg->multiple_csi_request_enabled && !cfg->is_not_ue_ss) ? 2 : 1;

  /* SRS request – 0 or 1 bit */
  n += (cfg->srs_request_enabled && !cfg->is_not_ue_ss) ? 1 : 0;

  /* Resource allocation type – 1 bit (N^UL_RB ≤ N^DL_RB)
   * This is a release10 field only, but it is backwards compatible to release 8 because a padding bit will be added.
   */
  n++;

  return n;
}

static uint32_t dci_format1A_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = 0;

  /* Carrier indicator – 0 or 3 bits */
  n += (cfg->cif_enabled) ? 3 : 0;

  /* Flag for format0/format1A differentiation – 1 bit */
  n += 1;

  /* Localized/Distributed VRB assignment flag – 1 bit */
  n += 1;

  /* Resource block assignment */
  n += riv_nbits(cell->nof_prb);

  /* Modulation and coding scheme and redundancy version – 5 bits */
  n += 5;

  /* HARQ process number – 3 bits (FDD) , 4 bits (TDD) */
  n += HARQ_PID_LEN;

  /* New data indicator – 1 bit */
  n += 1;

  /* Redundancy version – 2 bits */
  n += 2;

  /* TPC command for PUCCH – 2 bits */
  n += 2;

  /* Downlink Assignment Index – 2 bits (TDD) */
  n += (IS_TDD ? 2 : 0);

  /* SRS request – 0 or 1 bit */
  n += (cfg->srs_request_enabled ? 1 : 0);

  while (n < dci_format0_sizeof_(cell, sf, cfg)) {
    n++;
  }
  if (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

static uint32_t dci_format0_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = dci_format0_sizeof_(cell, sf, cfg);
  while (n < dci_format1A_sizeof(cell, sf, cfg)) {
    n++;
  }
  return n;
}

static uint32_t dci_format1_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{

  uint32_t n = (uint32_t)ceilf((float)cell->nof_prb / srslte_ra_type0_P(cell->nof_prb)) + 5 + HARQ_PID_LEN + 1 + 2 + 2 +
               (cfg->cif_enabled ? 3 : 0) + (IS_TDD ? 2 : 0);
  if (cell->nof_prb > 10) {
    n++;
  }
  while (n == dci_format0_sizeof(cell, sf, cfg) || n == dci_format1A_sizeof(cell, sf, cfg) || is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

static uint32_t dci_format1C_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n_vrb_dl_gap1 = srslte_ra_type2_n_vrb_dl(cell->nof_prb, true);
  uint32_t n_step        = srslte_ra_type2_n_rb_step(cell->nof_prb);
  uint32_t n             = riv_nbits((uint32_t)n_vrb_dl_gap1 / n_step) + 5;
  if (cell->nof_prb >= 50) {
    n++;
  }
  return n;
}

// Number of TPMI bits
static uint32_t tpmi_bits(uint32_t nof_ports)
{
  if (nof_ports <= 2) {
    return 2;
  } else {
    return 4;
  }
}

static uint32_t dci_format1B_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = 0;

  /* Carrier indicator – 0 or 3 bits */
  n += (cfg->cif_enabled) ? 3 : 0;

  /* Localized/Distributed VRB assignment flag – 1 bit */
  n += 1;

  /* Resource block assignment */
  n += riv_nbits(cell->nof_prb);

  /* Modulation and coding scheme and redundancy version – 5 bits */
  n += 5;

  /* HARQ process number – 3 bits (FDD) , 4 bits (TDD) */
  n += HARQ_PID_LEN;

  /* New data indicator – 1 bit */
  n += 1;

  /* Redundancy version – 2 bits */
  n += 2;

  /* TPC command for PUCCH – 2 bits */
  n += 2;

  /* Downlink Assignment Index – 2 bits (TDD) */
  n += (IS_TDD ? 2 : 0);

  /* TPMI information for precoding – number of bits as specified in Table 5.3.3.1.3A-1 */
  n += tpmi_bits(cell->nof_ports);

  /* PMI confirmation for precoding – 1 bit as specified in Table 5.3.3.1.3A-2 */
  n += 1;

  while (n < dci_format0_sizeof_(cell, sf, cfg)) {
    n++;
  }
  while (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

static uint32_t dci_format1D_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  // same size as format1B
  return dci_format1B_sizeof(cell, sf, cfg);
}

// Number of bits for precoding information
static uint32_t precoding_bits_f2(uint32_t nof_ports)
{
  if (nof_ports <= 2) {
    return 3;
  } else {
    return 6;
  }
}

static uint32_t dci_format2_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = (uint32_t)ceilf((float)cell->nof_prb / srslte_ra_type0_P(cell->nof_prb)) + 2 + HARQ_PID_LEN + 1 +
               2 * (5 + 1 + 2) + precoding_bits_f2(cell->nof_ports) + (cfg->cif_enabled ? 3 : 0) + (IS_TDD ? 2 : 0);
  if (cell->nof_prb > 10) {
    n++;
  }
  while (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

// Number of bits for precoding information
static uint32_t precoding_bits_f2a(uint32_t nof_ports)
{
  if (nof_ports <= 2) {
    return 0;
  } else {
    return 2;
  }
}

static uint32_t dci_format2A_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = (uint32_t)ceilf((float)cell->nof_prb / srslte_ra_type0_P(cell->nof_prb)) + 2 + HARQ_PID_LEN + 1 +
               2 * (5 + 1 + 2) + precoding_bits_f2a(cell->nof_ports) + (cfg->cif_enabled ? 3 : 0) + (IS_TDD ? 2 : 0);
  if (cell->nof_prb > 10) {
    n++;
  }
  while (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

static uint32_t dci_format2B_sizeof(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_dci_cfg_t* cfg)
{
  uint32_t n = (uint32_t)ceilf((float)cell->nof_prb / srslte_ra_type0_P(cell->nof_prb)) + 2 + HARQ_PID_LEN + 1 +
               2 * (5 + 1 + 2) + (cfg->cif_enabled ? 3 : 0) + (IS_TDD ? 2 : 0);
  if (cell->nof_prb > 10) {
    n++;
  }
  while (is_ambiguous_size(n)) {
    n++;
  }
  return n;
}

uint32_t srslte_dci_format_sizeof(const srslte_cell_t* cell,
                                  srslte_dl_sf_cfg_t*  sf,
                                  srslte_dci_cfg_t*    cfg,
                                  srslte_dci_format_t  format)
{
  srslte_dl_sf_cfg_t _sf;
  if (sf == NULL) {
    ZERO_OBJECT(_sf);
    sf = &_sf;
  }
  srslte_dci_cfg_t _cfg;
  if (cfg == NULL) {
    ZERO_OBJECT(_cfg);
    cfg = &_cfg;
  }

  switch (format) {
    case SRSLTE_DCI_FORMAT0:
      return dci_format0_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT1:
      return dci_format1_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT1A:
      return dci_format1A_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT1C:
      return dci_format1C_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT1B:
      return dci_format1B_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT1D:
      return dci_format1D_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT2:
      return dci_format2_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT2A:
      return dci_format2A_sizeof(cell, sf, cfg);
    case SRSLTE_DCI_FORMAT2B:
      return dci_format2B_sizeof(cell, sf, cfg);
      /*
    case SRSLTE_DCI_FORMAT3:
      return dci_format3_sizeof(nof_prb);
    case SRSLTE_DCI_FORMAT3A:
      return dci_format3A_sizeof(nof_prb);
      */
    default:
      printf("Error computing DCI bits: Unknown format %d\n", format);
      return 0;
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
static int dci_format0_pack(srslte_cell_t*      cell,
                            srslte_dl_sf_cfg_t* sf,
                            srslte_dci_cfg_t*   cfg,
                            srslte_dci_ul_t*    dci,
                            srslte_dci_msg_t*   msg)
{

  /* pack bits */
  uint8_t* y = msg->payload;
  uint32_t n_ul_hop;

  if (dci->cif_present) {
    srslte_bit_unpack(dci->cif, &y, 3);
  }

  *y++ = 0;                                               // format differentiation
  if (dci->freq_hop_fl == SRSLTE_RA_PUSCH_HOP_DISABLED) { // frequency hopping
    *y++     = 0;
    n_ul_hop = 0;
  } else {
    *y++ = 1;
    if (cell->nof_prb < 50) {
      n_ul_hop = 1; // Table 8.4-1 of 36.213
      *y++     = dci->freq_hop_fl & 1;
    } else {
      n_ul_hop = 2; // Table 8.4-1 of 36.213
      *y++     = (dci->freq_hop_fl & 2) >> 1;
      *y++     = dci->freq_hop_fl & 1;
    }
  }

  /* pack RIV according to 8.1 of 36.213 */
  uint32_t riv = dci->type2_alloc.riv;

  srslte_bit_unpack(riv, &y, riv_nbits(cell->nof_prb) - n_ul_hop);

  /* pack MCS according to 8.6.1 of 36.213 */
  srslte_bit_unpack(dci->tb.mcs_idx, &y, 5);

  *y++ = dci->tb.ndi;

  // TCP command for PUSCH
  srslte_bit_unpack(dci->tpc_pusch, &y, 2);

  // DM RS not implemented
  srslte_bit_unpack(dci->n_dmrs, &y, 3);

  // CSI request – 1 or 2 bits as defined in section 7.2.1 of 36.213. The 2-bit field applies to UEs that are configured
  // with more than one DL cell and when the corresponding DCI format is mapped onto the UE specific search space given
  // by the C-RNTI as defined in 36.213; otherwise the 1-bit field applies
  if (cfg->multiple_csi_request_enabled && !cfg->is_not_ue_ss) {
    *y++ = dci->cqi_request;
    *y++ = 0;
  } else {
    *y++ = dci->cqi_request;
  }

  // SRS request – 0 or 1 bit. This field can only be present in DCI formats scheduling PUSCH which are mapped onto
  // the UE specific search space given by the C-RNTI as defined in 36.213. The interpretation of this field is provided
  // in section 8.2 of 36.213
  if (cfg->srs_request_enabled && !cfg->is_not_ue_ss) {
    *y++ = dci->srs_request && dci->srs_request_present;
  }

  // Padding with zeros
  uint32_t n = srslte_dci_format_sizeof(cell, sf, cfg, SRSLTE_DCI_FORMAT0);
  while (y - msg->payload < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->payload);
  return SRSLTE_SUCCESS;
}

/* Unpacks DCI format 0 data and store result in msg according
 * to 36.212 5.3.3.1.1
 *
 * TODO: TPC and cyclic shift for DM RS not implemented
 */
static int dci_format0_unpack(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_msg_t*   msg,
                              srslte_dci_ul_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;
  uint32_t n_ul_hop;

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  if (*y++ != 0) {
    INFO("DCI message is Format1A\n");
    return SRSLTE_ERROR;
  }

  // Update DCI format
  msg->format = SRSLTE_DCI_FORMAT0;

  if (*y++ == 0) {
    dci->freq_hop_fl = SRSLTE_RA_PUSCH_HOP_DISABLED;
    n_ul_hop         = 0;
  } else {
    if (cell->nof_prb < 50) {
      n_ul_hop         = 1; // Table 8.4-1 of 36.213
      dci->freq_hop_fl = *y++;
    } else {
      n_ul_hop         = 2; // Table 8.4-1 of 36.213
      dci->freq_hop_fl = y[0] << 1 | y[1];
      y += 2;
    }
  }
  /* unpack RIV according to 8.1 of 36.213 */
  uint32_t riv         = srslte_bit_pack(&y, riv_nbits(cell->nof_prb) - n_ul_hop);
  dci->type2_alloc.riv = riv;

  /* unpack MCS according to 8.6 of 36.213 */
  dci->tb.mcs_idx = srslte_bit_pack(&y, 5);
  dci->tb.ndi     = *y++ ? true : false;

  // TPC command for scheduled PUSCH
  dci->tpc_pusch = srslte_bit_pack(&y, 2);

  // Cyclic shift for DMRS
  dci->n_dmrs = srslte_bit_pack(&y, 3);

  // TDD fields
  if (IS_TDD_CFG0) {
    dci->ul_idx = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  } else if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  // CQI request
  if (cfg->multiple_csi_request_enabled && !cfg->is_not_ue_ss) {
    dci->multiple_csi_request_present = true;
    dci->multiple_csi_request         = srslte_bit_pack(&y, 2);
  } else {
    dci->cqi_request = *y++ ? true : false;
  }

  // SRS request
  if (cfg->srs_request_enabled && !cfg->is_not_ue_ss) {
    dci->srs_request_present = true;
    dci->srs_request         = *y++ ? true : false;
  }

  if (cfg->ra_format_enabled) {
    dci->ra_type_present = true;
    dci->ra_type         = *y++ ? true : false;
  }

  return SRSLTE_SUCCESS;
}

/* Packs DCI format 1 data to a sequence of bits and store them in msg according
 * to 36.212 5.3.3.1.2
 *
 * TODO: TPC commands
 */

static int dci_format1_pack(srslte_cell_t*      cell,
                            srslte_dl_sf_cfg_t* sf,
                            srslte_dci_cfg_t*   cfg,
                            srslte_dci_dl_t*    dci,
                            srslte_dci_msg_t*   msg)
{
  uint32_t nof_prb = cell->nof_prb;

  /* pack bits */
  uint8_t* y = msg->payload;

  if (dci->cif_present) {
    srslte_bit_unpack(dci->cif, &y, 3);
  }

  if (nof_prb > 10) {
    *y++ = dci->alloc_type;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P          = srslte_ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t)ceilf((float)nof_prb / P);
  switch (dci->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      srslte_bit_unpack((uint32_t)dci->type0_alloc.rbg_bitmask, &y, alloc_size);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      srslte_bit_unpack((uint32_t)dci->type1_alloc.rbg_subset, &y, (int)ceilf(log2f(P)));
      *y++ = dci->type1_alloc.shift ? 1 : 0;
      srslte_bit_unpack((uint32_t)dci->type1_alloc.vrb_bitmask, &y, alloc_size - (int)ceilf(log2f(P)) - 1);
      break;
    default:
      ERROR("Format 1 accepts type0 or type1 resource allocation only\n");
      return SRSLTE_ERROR;
  }
  /* pack MCS */
  srslte_bit_unpack(dci->tb[0].mcs_idx, &y, 5);

  /* harq process number */
  srslte_bit_unpack(dci->pid, &y, HARQ_PID_LEN);

  *y++ = dci->tb[0].ndi;

  // rv version
  srslte_bit_unpack(dci->tb[0].rv, &y, 2);

  // TCP command for PUCCH
  srslte_bit_unpack(dci->tpc_pucch, &y, 2);

  uint32_t n = srslte_dci_format_sizeof(cell, sf, cfg, SRSLTE_DCI_FORMAT1);
  while (y - msg->payload < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->payload);

  if (msg->nof_bits != dci_format1_sizeof(cell, sf, cfg)) {
    ERROR("Invalid message length for format 1 (Cross scheduling %s)\n", dci->cif_present ? "enabled" : "disabled");
  }

  return SRSLTE_SUCCESS;
}

static int dci_format1_unpack(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_msg_t*   msg,
                              srslte_dci_dl_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;

  /* Make sure it's a SRSLTE_DCI_FORMAT1 message */
  uint32_t msg_len = srslte_dci_format_sizeof(cell, sf, cfg, SRSLTE_DCI_FORMAT1);
  if (msg->nof_bits != msg_len) {
    ERROR("Invalid message length (%d!=%d) for format 1\n", msg->nof_bits, msg_len);
    return SRSLTE_ERROR;
  }

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  if (cell->nof_prb > 10) {
    dci->alloc_type = *y++;
  } else {
    dci->alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P          = srslte_ra_type0_P(cell->nof_prb);
  uint32_t alloc_size = (uint32_t)ceilf((float)cell->nof_prb / P);
  switch (dci->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      dci->type0_alloc.rbg_bitmask = srslte_bit_pack(&y, alloc_size);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      dci->type1_alloc.rbg_subset  = srslte_bit_pack(&y, (int)ceilf(log2f(P)));
      dci->type1_alloc.shift       = *y++ ? true : false;
      dci->type1_alloc.vrb_bitmask = srslte_bit_pack(&y, alloc_size - (int)ceilf(log2f(P)) - 1);
      break;
    default:
      ERROR("Format 1 accepts type0 or type1 resource allocation only\n");
      return SRSLTE_ERROR;
  }
  /* unpack MCS according to 7.1.7 of 36.213 */
  dci->tb[0].mcs_idx = srslte_bit_pack(&y, 5);

  /* harq process number */
  dci->pid = srslte_bit_pack(&y, HARQ_PID_LEN);

  dci->tb[0].ndi = *y++ ? true : false;
  // rv version
  dci->tb[0].rv = srslte_bit_pack(&y, 2);

  // TPC PUCCH
  dci->tpc_pucch = srslte_bit_pack(&y, 2);

  // TDD
  if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  return SRSLTE_SUCCESS;
}

/* Packs DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 * TODO: RA procedure initiated by PDCCH, TPC commands
 */
static int dci_format1As_pack(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_dl_t*    dci,
                              srslte_dci_msg_t*   msg)
{
  uint32_t nof_prb = cell->nof_prb;

  /* pack bits */
  uint8_t* y = msg->payload;
  if (dci->cif_present) {
    srslte_bit_unpack(dci->cif, &y, 3);
  }

  *y++ = 1; // format differentiation

  if (dci->alloc_type != SRSLTE_RA_ALLOC_TYPE2) {
    ERROR("Format 1A accepts type2 resource allocation only\n");
    return SRSLTE_ERROR;
  }

  *y++ = dci->type2_alloc.mode; // localized or distributed VRB assignment

  /* pack RIV according to 7.1.6.3 of 36.213 */
  uint32_t riv    = dci->type2_alloc.riv;
  uint32_t nb_gap = 0;
  if (SRSLTE_RNTI_ISUSER(dci->rnti) && dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && nof_prb >= 50) {
    nb_gap = 1;
    *y++   = dci->type2_alloc.n_gap;
  }
  srslte_bit_unpack(riv, &y, riv_nbits(nof_prb) - nb_gap);

  // in format1A, MCS = TBS according to 7.1.7.2 of 36.213
  srslte_bit_unpack(dci->tb[0].mcs_idx, &y, 5);

  srslte_bit_unpack(dci->pid, &y, HARQ_PID_LEN);

  if (!SRSLTE_RNTI_ISUSER(dci->rnti)) {
    if (nof_prb >= 50 && dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST) {
      *y++ = dci->type2_alloc.n_gap;
    } else {
      y++; // bit reserved
    }
  } else {
    *y++ = dci->tb[0].ndi;
  }

  // rv version
  srslte_bit_unpack(dci->tb[0].rv, &y, 2);

  if (SRSLTE_RNTI_ISUSER(dci->rnti)) {
    // TPC not implemented
    *y++ = 0;
    *y++ = 0;
  } else {
    y++;                             // MSB of TPC is reserved
    *y++ = dci->type2_alloc.n_prb1a; // LSB indicates N_prb_1a for TBS
  }

  // Padding with zeros
  uint32_t n = srslte_dci_format_sizeof(cell, sf, cfg, SRSLTE_DCI_FORMAT1A);
  while (y - msg->payload < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->payload);

  return SRSLTE_SUCCESS;
}

/* Unpacks DCI format 1A for compact scheduling of PDSCH words according to 36.212 5.3.3.1.3
 *
 */
static int dci_format1As_unpack(srslte_cell_t*      cell,
                                srslte_dl_sf_cfg_t* sf,
                                srslte_dci_cfg_t*   cfg,
                                srslte_dci_msg_t*   msg,
                                srslte_dci_dl_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  if (*y++ != 1) {
    INFO("DCI message is Format0\n");
    return SRSLTE_ERROR;
  }

  // Update DCI format
  msg->format = SRSLTE_DCI_FORMAT1A;

  // Check if RA procedure by PDCCH order
  if (*y == 0) {
    int nof_bits = riv_nbits(cell->nof_prb);
    int i        = 0;
    // Check all bits in RBA are set to 1
    while (i < nof_bits && y[1 + i] == 1) {
      i++;
    }
    if (i == nof_bits) {
      // Check all remaining bits are set to 0
      i = 1 + 10 + nof_bits;
      while (i < msg->nof_bits - 1 && y[i] == 0) {
        i++;
      }
      if (i == msg->nof_bits - 1) {
        // This is a Random access order
        y += 1 + nof_bits;

        dci->is_ra_order = true;
        dci->ra_preamble = srslte_bit_pack(&y, 6);
        dci->ra_mask_idx = srslte_bit_pack(&y, 4);

        return SRSLTE_SUCCESS;
      }
    }
  }

  dci->is_ra_order = false;

  dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  dci->type2_alloc.n_gap = SRSLTE_RA_TYPE2_NG1;

  /* unpack RIV  */
  uint32_t nb_gap = 0;
  if (SRSLTE_RNTI_ISUSER(msg->rnti) && dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && cell->nof_prb >= 50) {
    nb_gap                 = 1;
    dci->type2_alloc.n_gap = *y++;
  }
  uint32_t riv         = srslte_bit_pack(&y, riv_nbits(cell->nof_prb) - nb_gap);
  dci->type2_alloc.riv = riv;

  // unpack MCS
  dci->tb[0].mcs_idx = srslte_bit_pack(&y, 5);

  dci->pid = srslte_bit_pack(&y, HARQ_PID_LEN);

  if (!SRSLTE_RNTI_ISUSER(msg->rnti)) {
    if (cell->nof_prb >= 50 && dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST) {
      dci->type2_alloc.n_gap = *y++;
    } else {
      y++; // NDI reserved
    }
  } else {
    dci->tb[0].ndi = *y++ ? true : false;
  }

  // rv version
  dci->tb[0].rv = srslte_bit_pack(&y, 2);

  if (SRSLTE_RNTI_ISUSER(msg->rnti)) {
    // TPC not implemented
    y++;
    y++;
  } else {
    y++;                             // MSB of TPC is reserved
    dci->type2_alloc.n_prb1a = *y++; // LSB indicates N_prb_1a for TBS
  }

  // TDD
  if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  return SRSLTE_SUCCESS;
}

static int dci_format1B_unpack(srslte_cell_t*      cell,
                               srslte_dl_sf_cfg_t* sf,
                               srslte_dci_cfg_t*   cfg,
                               srslte_dci_msg_t*   msg,
                               srslte_dci_dl_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  dci->type2_alloc.n_gap = SRSLTE_RA_TYPE2_NG1;

  /* unpack RIV according to 7.1.6.3 of 36.213 */
  uint32_t nb_gap = 0;
  if (dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && cell->nof_prb >= 50) {
    nb_gap                 = 1;
    dci->type2_alloc.n_gap = *y++;
  }
  uint32_t riv         = srslte_bit_pack(&y, riv_nbits(cell->nof_prb) - nb_gap);
  dci->type2_alloc.riv = riv;

  // unpack MCS, Harq pid and ndi
  dci->tb[0].mcs_idx = srslte_bit_pack(&y, 5);
  dci->pid           = srslte_bit_pack(&y, HARQ_PID_LEN);
  dci->tb[0].ndi     = *y++ ? true : false;
  dci->tb[0].rv      = srslte_bit_pack(&y, 2);

  // TPC PUCCH
  dci->tpc_pucch = srslte_bit_pack(&y, 2);

  // TDD
  if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  dci->pinfo = srslte_bit_pack(&y, tpmi_bits(cell->nof_ports));
  dci->pconf = *y++ ? true : false;

  return SRSLTE_SUCCESS;
}

/* Format 1C for compact scheduling of PDSCH words
 *
 */
static int dci_format1Cs_pack(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_dl_t*    dci,
                              srslte_dci_msg_t*   msg)
{

  uint32_t nof_prb = cell->nof_prb;

  /* pack bits */
  uint8_t* y = msg->payload;

  if (dci->cif_present) {
    srslte_bit_unpack(dci->cif, &y, 3);
  }

  if (dci->alloc_type != SRSLTE_RA_ALLOC_TYPE2 || dci->type2_alloc.mode != SRSLTE_RA_TYPE2_DIST) {
    ERROR("Format 1C accepts distributed type2 resource allocation only\n");
    return SRSLTE_ERROR;
  }

  if (nof_prb >= 50) {
    *y++ = dci->type2_alloc.n_gap;
  }
  uint32_t n_step   = srslte_ra_type2_n_rb_step(nof_prb);
  uint32_t n_vrb_dl = srslte_ra_type2_n_vrb_dl(nof_prb, dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);

  uint32_t riv = dci->type2_alloc.riv;

  srslte_bit_unpack(riv, &y, riv_nbits((int)n_vrb_dl / n_step));

  // in format1C, MCS = TBS according to 7.1.7.2 of 36.213
  srslte_bit_unpack(dci->tb[0].mcs_idx, &y, 5);

  msg->nof_bits = (y - msg->payload);

  return SRSLTE_SUCCESS;
}

static int dci_format1Cs_unpack(srslte_cell_t*      cell,
                                srslte_dl_sf_cfg_t* sf,
                                srslte_dci_cfg_t*   cfg,
                                srslte_dci_msg_t*   msg,
                                srslte_dci_dl_t*    dci)
{
  /* pack bits */
  uint8_t* y = msg->payload;

  if (msg->nof_bits != srslte_dci_format_sizeof(cell, sf, cfg, SRSLTE_DCI_FORMAT1C)) {
    ERROR("Invalid message length for format 1C\n");
    return SRSLTE_ERROR;
  }

  dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = SRSLTE_RA_TYPE2_DIST;
  if (cell->nof_prb >= 50) {
    dci->type2_alloc.n_gap = *y++;
  }
  uint32_t n_step   = srslte_ra_type2_n_rb_step(cell->nof_prb);
  uint32_t n_vrb_dl = srslte_ra_type2_n_vrb_dl(cell->nof_prb, dci->type2_alloc.n_gap == SRSLTE_RA_TYPE2_NG1);

  uint32_t riv = srslte_bit_pack(&y, riv_nbits((int)n_vrb_dl / n_step));

  dci->type2_alloc.riv = riv;

  dci->tb[0].mcs_idx = srslte_bit_pack(&y, 5);

  dci->tb[0].rv = -1; // Get RV later

  msg->nof_bits = (y - msg->payload);

  return SRSLTE_SUCCESS;
}

static int dci_format1D_unpack(srslte_cell_t*      cell,
                               srslte_dl_sf_cfg_t* sf,
                               srslte_dci_cfg_t*   cfg,
                               srslte_dci_msg_t*   msg,
                               srslte_dci_dl_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
  dci->type2_alloc.mode = *y++;

  // by default, set N_gap to 1
  dci->type2_alloc.n_gap = SRSLTE_RA_TYPE2_NG1;

  /* unpack RIV according to 7.1.6.3 of 36.213 */
  uint32_t nb_gap = 0;
  if (dci->type2_alloc.mode == SRSLTE_RA_TYPE2_DIST && cell->nof_prb >= 50) {
    nb_gap                 = 1;
    dci->type2_alloc.n_gap = *y++;
  }

  uint32_t riv         = srslte_bit_pack(&y, riv_nbits(cell->nof_prb) - nb_gap);
  dci->type2_alloc.riv = riv;

  // unpack MCS, Harq pid and ndi
  dci->tb[0].mcs_idx = srslte_bit_pack(&y, 5);
  dci->pid           = srslte_bit_pack(&y, HARQ_PID_LEN);
  dci->tb[0].ndi     = *y++ ? true : false;
  dci->tb[0].rv      = srslte_bit_pack(&y, 2);

  // TPC PUCCH
  dci->tpc_pucch = srslte_bit_pack(&y, 2);

  // TDD
  if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  dci->pinfo        = srslte_bit_pack(&y, tpmi_bits(cell->nof_ports));
  dci->power_offset = *y++ ? true : false;

  return SRSLTE_SUCCESS;
}

static int dci_format2AB_pack(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_dl_t*    dci,
                              srslte_dci_msg_t*   msg)
{

  uint32_t nof_prb   = cell->nof_prb;
  uint32_t nof_ports = cell->nof_ports;

  /* pack bits */
  uint8_t* y = msg->payload;

  if (dci->cif_present) {
    srslte_bit_unpack(dci->cif, &y, 3);
  }

  if (nof_prb > 10) {
    *y++ = dci->alloc_type;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P          = srslte_ra_type0_P(nof_prb);
  uint32_t alloc_size = (uint32_t)ceilf((float)nof_prb / P);
  switch (dci->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      srslte_bit_unpack((uint32_t)dci->type0_alloc.rbg_bitmask, &y, alloc_size);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      srslte_bit_unpack((uint32_t)dci->type1_alloc.rbg_subset, &y, (int)ceilf(log2f(P)));
      *y++ = dci->type1_alloc.shift ? 1 : 0;
      srslte_bit_unpack((uint32_t)dci->type1_alloc.vrb_bitmask, &y, alloc_size - (int)ceilf(log2f(P)) - 1);
      break;
    default:
      ERROR("Format 1 accepts type0 or type1 resource allocation only\n");
      return SRSLTE_ERROR;
  }

  /* TCP command for PUCCH */
  srslte_bit_unpack(dci->tpc_pucch, &y, 2);

  /* harq process number */
  srslte_bit_unpack(dci->pid, &y, HARQ_PID_LEN);

  // Transpor block to codeword swap flag
  if (msg->format == SRSLTE_DCI_FORMAT2B) {
    *y++ = dci->sram_id;
  } else {
    *y++ = dci->tb_cw_swap;
  }

  /* pack TB1 */
  srslte_bit_unpack(dci->tb[0].mcs_idx, &y, 5);
  *y++ = dci->tb[0].ndi;
  srslte_bit_unpack(dci->tb[0].rv, &y, 2);

  /* pack TB2 */
  srslte_bit_unpack(dci->tb[1].mcs_idx, &y, 5);
  *y++ = dci->tb[1].ndi;
  srslte_bit_unpack(dci->tb[1].rv, &y, 2);

  // Precoding information
  if (msg->format == SRSLTE_DCI_FORMAT2) {
    srslte_bit_unpack(dci->pinfo, &y, precoding_bits_f2(nof_ports));
  } else if (msg->format == SRSLTE_DCI_FORMAT2A) {
    srslte_bit_unpack(dci->pinfo, &y, precoding_bits_f2a(nof_ports));
  }

  // Padding with zeros
  uint32_t n = srslte_dci_format_sizeof(cell, sf, cfg, msg->format);
  while (y - msg->payload < n) {
    *y++ = 0;
  }
  msg->nof_bits = (y - msg->payload);

  return SRSLTE_SUCCESS;
}

static int dci_format2AB_unpack(srslte_cell_t*      cell,
                                srslte_dl_sf_cfg_t* sf,
                                srslte_dci_cfg_t*   cfg,
                                srslte_dci_msg_t*   msg,
                                srslte_dci_dl_t*    dci)
{

  /* pack bits */
  uint8_t* y = msg->payload;

  if (cfg->cif_enabled) {
    dci->cif         = srslte_bit_pack(&y, 3);
    dci->cif_present = true;
  }

  if (cell->nof_prb > 10) {
    dci->alloc_type = *y++;
  } else {
    dci->alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  }

  /* Resource allocation: type0 or type 1 */
  uint32_t P          = srslte_ra_type0_P(cell->nof_prb);
  uint32_t alloc_size = (uint32_t)ceilf((float)cell->nof_prb / P);
  switch (dci->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      dci->type0_alloc.rbg_bitmask = srslte_bit_pack(&y, alloc_size);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      dci->type1_alloc.rbg_subset  = srslte_bit_pack(&y, (int)ceilf(log2f(P)));
      dci->type1_alloc.shift       = *y++ ? true : false;
      dci->type1_alloc.vrb_bitmask = srslte_bit_pack(&y, alloc_size - (int)ceilf(log2f(P)) - 1);
      break;
    default:
      ERROR("Format2 accepts type0 or type1 resource allocation only\n");
      return SRSLTE_ERROR;
  }

  // TPC PUCCH
  dci->tpc_pucch = srslte_bit_pack(&y, 2);

  // TDD
  if (IS_TDD) {
    dci->dai    = srslte_bit_pack(&y, 2);
    dci->is_tdd = true;
  }

  /* harq process number */
  dci->pid = srslte_bit_pack(&y, HARQ_PID_LEN);

  // Transpor block to codeword swap flag
  if (msg->format == SRSLTE_DCI_FORMAT2B) {
    dci->sram_id = *y++ ? true : false;
  } else {
    dci->tb_cw_swap = *y++ ? true : false;
  }

  uint32_t nof_tb = 0;
  /* unpack MCS according to 7.1.7 of 36.213 */
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    dci->tb[i].mcs_idx = srslte_bit_pack(&y, 5);
    dci->tb[i].ndi     = *y++ ? true : false;
    dci->tb[i].rv      = srslte_bit_pack(&y, 2);
    if (SRSLTE_DCI_IS_TB_EN(dci->tb[i])) {
      nof_tb++;
    }
  }

  // Precoding information
  if (msg->format == SRSLTE_DCI_FORMAT2) {
    dci->pinfo = srslte_bit_pack(&y, precoding_bits_f2(cell->nof_ports));
  } else if (msg->format == SRSLTE_DCI_FORMAT2A) {
    dci->pinfo = srslte_bit_pack(&y, precoding_bits_f2a(cell->nof_ports));
  }

  // Apply TB swap table according to 3GPP 36.212 R8, section 5.3.3.1.5
  if (nof_tb == 2) {
    // Table 5.3.3.1.5-1
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      dci->tb[i].cw_idx = (((dci->tb_cw_swap) ? 1 : 0) + i) % nof_tb;
    }
  } else {
    // Table 5.3.3.1.5-2
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      dci->tb[i].cw_idx = 0;
    }
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_msg_pack_pdsch(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_dl_t*    dci,
                              srslte_dci_msg_t*   msg)
{
  int ret = SRSLTE_ERROR;

  msg->rnti     = dci->rnti;
  msg->location = dci->location;
  msg->format   = dci->format;

  srslte_dci_cfg_t _dci_cfg;
  if (!cfg) {
    ZERO_OBJECT(_dci_cfg);
    cfg = &_dci_cfg;
  }

  switch (msg->format) {
    case SRSLTE_DCI_FORMAT1:
      ret = dci_format1_pack(cell, sf, cfg, dci, msg);
      break;
    case SRSLTE_DCI_FORMAT1A:
      ret = dci_format1As_pack(cell, sf, cfg, dci, msg);
      break;
    case SRSLTE_DCI_FORMAT1C:
      ret = dci_format1Cs_pack(cell, sf, cfg, dci, msg);
      break;
    case SRSLTE_DCI_FORMAT2:
    case SRSLTE_DCI_FORMAT2A:
    case SRSLTE_DCI_FORMAT2B:
      ret = dci_format2AB_pack(cell, sf, cfg, dci, msg);
      break;
    default:
      ERROR("DCI pack pdsch: Invalid DCI format %s\n", srslte_dci_format_string(msg->format));
  }

#if SRSLTE_DCI_HEXDEBUG
  srslte_vec_sprint_hex(dci->hex_str, sizeof(dci->hex_str), msg->payload, msg->nof_bits);
  dci->nof_bits = msg->nof_bits;
#endif /* SRSLTE_DCI_HEXDEBUG */

  return ret;
}

int srslte_dci_msg_unpack_pdsch(srslte_cell_t*      cell,
                                srslte_dl_sf_cfg_t* sf,
                                srslte_dci_cfg_t*   cfg,
                                srslte_dci_msg_t*   msg,
                                srslte_dci_dl_t*    dci)
{
  // Initialize DCI
  bzero(dci, sizeof(srslte_dci_dl_t));

  // Enable just 1 TB per default
  for (int i = 1; i < SRSLTE_MAX_CODEWORDS; i++) {
    SRSLTE_DCI_TB_DISABLE(dci->tb[i]);
  }
  dci->rnti     = msg->rnti;
  dci->location = msg->location;
  dci->format   = msg->format;

  srslte_dci_cfg_t _dci_cfg;
  if (!cfg) {
    ZERO_OBJECT(_dci_cfg);
    cfg = &_dci_cfg;
  }

#if SRSLTE_DCI_HEXDEBUG
  dci->hex_str[0] = '\0';
  srslte_vec_sprint_hex(dci->hex_str, sizeof(dci->hex_str), msg->payload, msg->nof_bits);
  dci->nof_bits = msg->nof_bits;
#endif

  // Set dwpts flag
  dci->is_dwpts =
      cell->frame_type == SRSLTE_TDD && srslte_sfidx_tdd_type(sf->tdd_config, sf->tti % 10) == SRSLTE_TDD_SF_S;

  switch (msg->format) {
    case SRSLTE_DCI_FORMAT1:
      return dci_format1_unpack(cell, sf, cfg, msg, dci);
    case SRSLTE_DCI_FORMAT1A:
      return dci_format1As_unpack(cell, sf, cfg, msg, dci);
    case SRSLTE_DCI_FORMAT1B:
      return dci_format1B_unpack(cell, sf, cfg, msg, dci);
    case SRSLTE_DCI_FORMAT1C:
      return dci_format1Cs_unpack(cell, sf, cfg, msg, dci);
    case SRSLTE_DCI_FORMAT1D:
      return dci_format1D_unpack(cell, sf, cfg, msg, dci);
    case SRSLTE_DCI_FORMAT2:
    case SRSLTE_DCI_FORMAT2A:
    case SRSLTE_DCI_FORMAT2B:
      return dci_format2AB_unpack(cell, sf, cfg, msg, dci);
    default:
      ERROR("DCI unpack pdsch: Invalid DCI format %s\n", srslte_dci_format_string(msg->format));
      return SRSLTE_ERROR;
  }
}

int srslte_dci_msg_pack_pusch(srslte_cell_t*      cell,
                              srslte_dl_sf_cfg_t* sf,
                              srslte_dci_cfg_t*   cfg,
                              srslte_dci_ul_t*    dci,
                              srslte_dci_msg_t*   msg)
{
  msg->rnti     = dci->rnti;
  msg->location = dci->location;
  msg->format   = dci->format;

  srslte_dci_cfg_t _dci_cfg;
  if (!cfg) {
    ZERO_OBJECT(_dci_cfg);
    cfg = &_dci_cfg;
  }

  int n = dci_format0_pack(cell, sf, cfg, dci, msg);

#if SRSLTE_DCI_HEXDEBUG
  dci->hex_str[0] = '\0';
  srslte_vec_sprint_hex(dci->hex_str, sizeof(dci->hex_str), msg->payload, msg->nof_bits);
  dci->nof_bits = msg->nof_bits;
#endif /* SRSLTE_DCI_HEXDEBUG */

  return n;
}

int srslte_dci_msg_unpack_pusch(srslte_cell_t*      cell,
                                srslte_dl_sf_cfg_t* sf,
                                srslte_dci_cfg_t*   cfg,
                                srslte_dci_msg_t*   msg,
                                srslte_dci_ul_t*    dci)
{
  // Initialize DCI
  bzero(dci, sizeof(srslte_dci_ul_t));

  dci->rnti     = msg->rnti;
  dci->location = msg->location;
  dci->format   = msg->format;

  srslte_dci_cfg_t _dci_cfg;
  if (!cfg) {
    ZERO_OBJECT(_dci_cfg);
    cfg = &_dci_cfg;
  }

#if SRSLTE_DCI_HEXDEBUG
  dci->hex_str[0] = '\0';
  srslte_vec_sprint_hex(dci->hex_str, sizeof(dci->hex_str), msg->payload, msg->nof_bits);
  dci->nof_bits = msg->nof_bits;
#endif /* SRSLTE_DCI_HEXDEBUG */

  return dci_format0_unpack(cell, sf, cfg, msg, dci);
}

bool srslte_location_find(const srslte_dci_location_t* locations, uint32_t nof_locations, srslte_dci_location_t x)
{
  for (uint32_t i = 0; i < nof_locations; i++) {
    if (locations[i].L == x.L && locations[i].ncce == x.ncce) {
      return true;
    }
  }
  return false;
}

bool srslte_location_find_ncce(const srslte_dci_location_t* locations, uint32_t nof_locations, uint32_t ncce)
{
  for (uint32_t i = 0; i < nof_locations; i++) {
    if (locations[i].ncce == ncce) {
      return true;
    }
  }
  return false;
}

// Set the configuration for Format0/1A messages allocated on Common SS
void srslte_dci_cfg_set_common_ss(srslte_dci_cfg_t* cfg)
{
  cfg->is_not_ue_ss = true;
}

int srslte_dci_location_set(srslte_dci_location_t* c, uint32_t L, uint32_t nCCE)
{
  if (L <= 3) {
    c->L = L;
  } else {
    ERROR("Invalid L %d\n", L);
    return SRSLTE_ERROR;
  }
  if (nCCE <= 87) {
    c->ncce = nCCE;
  } else {
    ERROR("Invalid nCCE %d\n", nCCE);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

bool srslte_dci_location_isvalid(srslte_dci_location_t* c)
{
  if (c->L <= 3 && c->ncce <= 87) {
    return true;
  } else {
    return false;
  }
}

srslte_dci_format_t srslte_dci_format_from_string(char* str)
{
  if (!strcmp(str, "Format0")) {
    return SRSLTE_DCI_FORMAT0;
  } else if (!strcmp(str, "Format1")) {
    return SRSLTE_DCI_FORMAT1;
  } else if (!strcmp(str, "Format1A")) {
    return SRSLTE_DCI_FORMAT1A;
  } else if (!strcmp(str, "Format1B")) {
    return SRSLTE_DCI_FORMAT1B;
  } else if (!strcmp(str, "Format1C")) {
    return SRSLTE_DCI_FORMAT1C;
  } else if (!strcmp(str, "Format1D")) {
    return SRSLTE_DCI_FORMAT1D;
  } else if (!strcmp(str, "Format2")) {
    return SRSLTE_DCI_FORMAT2;
  } else if (!strcmp(str, "Format2A")) {
    return SRSLTE_DCI_FORMAT2A;
  } else if (!strcmp(str, "Format2B")) {
    return SRSLTE_DCI_FORMAT2B;
  } else if (!strcmp(str, "FormatN0")) {
    return SRSLTE_DCI_FORMATN0;
  } else if (!strcmp(str, "FormatN1")) {
    return SRSLTE_DCI_FORMATN1;
  } else if (!strcmp(str, "FormatN2")) {
    return SRSLTE_DCI_FORMATN2;
  } else {
    return SRSLTE_DCI_NOF_FORMATS;
  }
}

char* srslte_dci_format_string(srslte_dci_format_t format)
{
  switch (format) {
    case SRSLTE_DCI_FORMAT0:
      return "Format0 ";
    case SRSLTE_DCI_FORMAT1:
      return "Format1 ";
    case SRSLTE_DCI_FORMAT1A:
      return "Format1A";
    case SRSLTE_DCI_FORMAT1B:
      return "Format1B";
    case SRSLTE_DCI_FORMAT1C:
      return "Format1C";
    case SRSLTE_DCI_FORMAT1D:
      return "Format1D";
    case SRSLTE_DCI_FORMAT2:
      return "Format2 ";
    case SRSLTE_DCI_FORMAT2A:
      return "Format2A";
    case SRSLTE_DCI_FORMAT2B:
      return "Format2B";
    case SRSLTE_DCI_FORMATN0:
      return "FormatN0";
    case SRSLTE_DCI_FORMATN1:
      return "FormatN1";
    case SRSLTE_DCI_FORMATN2:
      return "FormatN2";
    default:
      return "N/A"; // fatal error
  }
}

char* srslte_dci_format_string_short(srslte_dci_format_t format)
{
  switch (format) {
    case SRSLTE_DCI_FORMAT0:
      return "0";
    case SRSLTE_DCI_FORMAT1:
      return "1";
    case SRSLTE_DCI_FORMAT1A:
      return "1A";
    case SRSLTE_DCI_FORMAT1B:
      return "1B";
    case SRSLTE_DCI_FORMAT1C:
      return "1C";
    case SRSLTE_DCI_FORMAT1D:
      return "1D";
    case SRSLTE_DCI_FORMAT2:
      return "2";
    case SRSLTE_DCI_FORMAT2A:
      return "2A";
    case SRSLTE_DCI_FORMAT2B:
      return "2B";
    default:
      return "N/A"; // fatal error
  }
}

static char* ra_type_string(srslte_ra_type_t alloc_type)
{
  switch (alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      return "Type 0";
    case SRSLTE_RA_ALLOC_TYPE1:
      return "Type 1";
    case SRSLTE_RA_ALLOC_TYPE2:
      return "Type 2";
    default:
      return "N/A";
  }
}

static char* freq_hop_fl_string(int freq_hop)
{
  switch (freq_hop) {
    case 0:
      return "1/4";
    case 1:
      return "-1/4";
    case 2:
      return "1/2";
    case 3:
      return "type2";
  }
  return "n/a";
}

void srslte_dci_dl_fprint(FILE* f, srslte_dci_dl_t* dci, uint32_t nof_prb)
{
  fprintf(f, " - Resource Allocation Type:\t\t%s\n", ra_type_string(dci->alloc_type));
  switch (dci->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      fprintf(f, "   + Resource Block Group Size:\t\t%d\n", srslte_ra_type0_P(nof_prb));
      fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n", dci->type0_alloc.rbg_bitmask);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      fprintf(f, "   + Resource Block Group Size:\t\t%d\n", srslte_ra_type0_P(nof_prb));
      fprintf(f, "   + RBG Bitmap:\t\t\t0x%x\n", dci->type1_alloc.vrb_bitmask);
      fprintf(f, "   + RBG Subset:\t\t\t%d\n", dci->type1_alloc.rbg_subset);
      fprintf(f, "   + RBG Shift:\t\t\t\t%s\n", dci->type1_alloc.shift ? "Yes" : "No");
      break;
    case SRSLTE_RA_ALLOC_TYPE2:
      fprintf(f, "   + Type:\t\t\t\t%s\n", dci->type2_alloc.mode == SRSLTE_RA_TYPE2_LOC ? "Localized" : "Distributed");
      fprintf(f, "   + Resource Indicator Value:\t\t%d\n", dci->type2_alloc.riv);
      break;
  }
  if (dci->cif_present) {
    fprintf(f, " - Carrier idx:\t\t\t\t%d\n", dci->cif);
  }
  fprintf(f, " - HARQ process:\t\t\t%d\n", dci->pid);
  fprintf(f, " - TPC command for PUCCH:\t\t--\n");
  fprintf(f, " - Transport blocks swapped:\t\t%s\n", (dci->tb_cw_swap) ? "true" : "false");

  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    fprintf(f, " - Transport block %d enabled:\t\t%s\n", i, SRSLTE_DCI_IS_TB_EN(dci->tb[i]) ? "true" : "false");
    if (SRSLTE_DCI_IS_TB_EN(dci->tb[i])) {
      fprintf(f, "   + Modulation and coding scheme index:\t%d\n", dci->tb[i].mcs_idx);
      fprintf(f, "   + New data indicator:\t\t\t%s\n", dci->tb[i].ndi ? "Yes" : "No");
      fprintf(f, "   + Redundancy version:\t\t\t%d\n", dci->tb[i].rv);
    }
  }
}

static uint32_t print_multi(char* info_str, uint32_t n, uint32_t len, const srslte_dci_dl_t* dci_dl, uint32_t value_id)
{
  uint32_t nof_tb = 1;
  if (dci_dl->format >= SRSLTE_DCI_FORMAT2) {
    nof_tb = 2;
  }
  for (uint32_t i = 0; i < nof_tb; i++) {
    switch (value_id) {
      case 0:
        n = srslte_print_check(info_str, len, n, "%d", dci_dl->tb[i].mcs_idx);
        break;
      case 1:
        n = srslte_print_check(info_str, len, n, "%d", dci_dl->tb[i].rv);
        break;
      case 2:
        n = srslte_print_check(info_str, len, n, "%d", dci_dl->tb[i].ndi);
        break;
    }
    if (i < SRSLTE_MAX_CODEWORDS - 1 && nof_tb > 1) {
      n = srslte_print_check(info_str, len, n, "/");
    }
  }
  return n;
}

uint32_t srslte_dci_dl_info(const srslte_dci_dl_t* dci_dl, char* info_str, uint32_t len)
{
  uint32_t n = 0;
  n          = srslte_print_check(info_str,
                         len,
                         0,
                         "f=%s, cce=%2d, L=%d",
                         srslte_dci_format_string_short(dci_dl->format),
                         dci_dl->location.ncce,
                         dci_dl->location.L);

  if (dci_dl->cif_present) {
    n = srslte_print_check(info_str, len, n, ", cif=%d", dci_dl->cif);
  }

  switch (dci_dl->alloc_type) {
    case SRSLTE_RA_ALLOC_TYPE0:
      n = srslte_print_check(info_str, len, n, ", rbg=0x%x", dci_dl->type0_alloc.rbg_bitmask);
      break;
    case SRSLTE_RA_ALLOC_TYPE1:
      n = srslte_print_check(info_str,
                             len,
                             n,
                             ", vrb=0x%x, rbg_s=%d, sh=%d",
                             dci_dl->type1_alloc.vrb_bitmask,
                             dci_dl->type1_alloc.rbg_subset,
                             dci_dl->type1_alloc.shift);
      break;
    case SRSLTE_RA_ALLOC_TYPE2:
      n = srslte_print_check(info_str, len, n, ", riv=%d", dci_dl->type2_alloc.riv);
      break;
  }

  n = srslte_print_check(info_str, len, n, ", pid=%d", dci_dl->pid);

  n = srslte_print_check(info_str, len, n, ", mcs={", 0);
  n = print_multi(info_str, n, len, dci_dl, 0);
  n = srslte_print_check(info_str, len, n, "}", 0);
  n = srslte_print_check(info_str, len, n, ", ndi={", 0);
  n = print_multi(info_str, n, len, dci_dl, 2);
  n = srslte_print_check(info_str, len, n, "}", 0);

  if (dci_dl->format == SRSLTE_DCI_FORMAT1 || dci_dl->format == SRSLTE_DCI_FORMAT1A ||
      dci_dl->format == SRSLTE_DCI_FORMAT1B || dci_dl->format == SRSLTE_DCI_FORMAT2 ||
      dci_dl->format == SRSLTE_DCI_FORMAT2A || dci_dl->format == SRSLTE_DCI_FORMAT2B) {
    n = srslte_print_check(info_str, len, n, ", tpc_pucch=%d", dci_dl->tpc_pucch);
  }

  if (dci_dl->is_tdd) {
    n = srslte_print_check(info_str, len, n, ", dai=%d", dci_dl->dai);
  }

  if (dci_dl->format == SRSLTE_DCI_FORMAT2 || dci_dl->format == SRSLTE_DCI_FORMAT2A ||
      dci_dl->format == SRSLTE_DCI_FORMAT2B) {
    n = srslte_print_check(info_str, len, n, ", tb_sw=%d, pinfo=%d", dci_dl->tb_cw_swap, dci_dl->pinfo);
  }

#if SRSLTE_DCI_HEXDEBUG
  n = srslte_print_check(info_str, len, n, ", len=%d, hex=%s", dci_dl->nof_bits, dci_dl->hex_str);
#endif /* SRSLTE_DCI_HEXDEBUG */

  return n;
}

uint32_t srslte_dci_ul_info(srslte_dci_ul_t* dci_ul, char* info_str, uint32_t len)
{
  uint32_t n = 0;

  n = srslte_print_check(info_str, len, n, "f=0, ");

  if (dci_ul->cif_present) {
    n = srslte_print_check(info_str, len, n, "cif=%d, ", dci_ul->cif);
  }

  n = srslte_print_check(info_str,
                         len,
                         n,
                         "cce=%2d, L=%d, riv=%d, mcs=%d, rv=%d, ndi=%d, f_h=%s, cqi=%s, tpc_pusch=%d, dmrs_cs=%d",
                         dci_ul->location.ncce,
                         dci_ul->location.L,
                         dci_ul->type2_alloc.riv,
                         dci_ul->tb.mcs_idx,
                         dci_ul->tb.rv,
                         dci_ul->tb.ndi,
                         freq_hop_fl_string(dci_ul->freq_hop_fl),
                         dci_ul->cqi_request ? "yes" : "no",
                         dci_ul->tpc_pusch,
                         dci_ul->n_dmrs);

  if (dci_ul->multiple_csi_request_present) {
    n = srslte_print_check(info_str, len, n, ", csi=%d", dci_ul->multiple_csi_request);
  }
  if (dci_ul->srs_request_present) {
    n = srslte_print_check(info_str, len, n, ", srs_request=%s", dci_ul->srs_request ? "yes" : "no");
  }
  if (dci_ul->ra_type_present) {
    n = srslte_print_check(info_str, len, n, ", ra_type=%s", dci_ul->ra_type ? "yes" : "no");
  }

  if (dci_ul->is_tdd) {
    n = srslte_print_check(info_str, len, n, ", ul_idx=%d, dai=%d", dci_ul->ul_idx, dci_ul->dai);
  }

#if SRSLTE_DCI_HEXDEBUG
  n = srslte_print_check(info_str, len, n, ", len=%d, hex=%s", dci_ul->nof_bits, dci_ul->hex_str);
#endif /* SRSLTE_DCI_HEXDEBUG */

  return n;
}

uint32_t srslte_dci_format_max_tb(srslte_dci_format_t format)
{
  uint32_t ret = 0;
  switch (format) {
    case SRSLTE_DCI_FORMAT0:
    case SRSLTE_DCI_FORMAT1:
    case SRSLTE_DCI_FORMAT1A:
    case SRSLTE_DCI_FORMAT1C:
    case SRSLTE_DCI_FORMAT1B:
    case SRSLTE_DCI_FORMAT1D:
    case SRSLTE_DCI_FORMATN0:
    case SRSLTE_DCI_FORMATN1:
    case SRSLTE_DCI_FORMATN2:
    case SRSLTE_DCI_FORMAT_RAR:
      ret = 1;
      break;
    case SRSLTE_DCI_FORMAT2:
    case SRSLTE_DCI_FORMAT2A:
    case SRSLTE_DCI_FORMAT2B:
      ret = 2;
      break;
    case SRSLTE_DCI_NOF_FORMATS:
    default:
      ret = 0;
      break;
  }
  return ret;
}
