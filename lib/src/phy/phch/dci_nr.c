/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/**
 * Defines minimum size according to TS 38.212 section 7.3.1 DCI Formats:
 *     If the number of information bits in a DCI format is less than 12 bits, zeros shall be appended to the DCI format
 *     until the payload size equals 12.
 */
#define DCI_NR_MIN_SIZE 12

#define CEIL_LOG2(N) (((N) == 0) ? 0 : ceil(log2((double)(N))))

static uint32_t dci_nr_freq_resource_size_type1(uint32_t N)
{
  if (N == 0) {
    return 0;
  }

  return (int)CEIL_LOG2(N * (N + 1) / 2.0);
}

static uint32_t dci_nr_freq_resource_size(srsran_resource_alloc_t alloc_type, uint32_t N_RBG, uint32_t N_BWP_RB)
{
  // Frequency domain resource assignment
  switch (alloc_type) {
    case srsran_resource_alloc_type0:
      return N_RBG;
    case srsran_resource_alloc_type1:
      return dci_nr_freq_resource_size_type1(N_BWP_RB);
    case srsran_resource_alloc_dynamic:
      return SRSRAN_MAX(N_RBG, dci_nr_freq_resource_size_type1(N_BWP_RB)) + 1;
    default:
      ERROR("Unhandled case");
  }

  return 0;
}

static uint32_t dci_nr_bwp_id_size(uint32_t N_BWP_RRC)
{
  uint32_t N_BWP = N_BWP_RRC;
  if (N_BWP_RRC <= 3) {
    N_BWP = N_BWP_RRC + 1;
  }

  return (int)CEIL_LOG2(N_BWP);
}

static uint32_t dci_nr_time_res_size(uint32_t nof_time_res)
{
  if (nof_time_res == 0) {
    // 4 bits are necessary for PUSCH default time resource assigment (TS 38.214 Table 6.1.2.1.1-2)
    return 4;
  }
  return (uint32_t)CEIL_LOG2(nof_time_res);
}

// Determines DCI format 0_0 according to TS 38.212 clause 7.3.1.1.1
static uint32_t dci_nr_format_0_0_sizeof(uint32_t N_UL_BWP_RB, const srsran_dci_cfg_nr_t* cfg)
{
  uint32_t count = 0;

  // Identifier for DCI formats – 1 bits
  count++;

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  uint32_t N_UL_hop = (cfg->enable_hopping) ? ((N_UL_BWP_RB < 50) ? 1 : 2) : 0;
  count += N_UL_hop;

  // Frequency domain resource assignment
  uint32_t N = dci_nr_freq_resource_size_type1(N_UL_BWP_RB);
  if (N < N_UL_hop) {
    return 0;
  }
  count += N - N_UL_hop;

  // Time domain resource assignment – 4 bits
  count += 4;

  // Frequency hopping flag – 1 bit
  count += 1;

  // Modulation and coding scheme – 5 bits
  count += 5;

  // New data indicator – 1 bit
  count += 1;

  // Redundancy version – 2 bits
  count += 2;

  // HARQ process number – 4 bits
  count += 4;

  // TPC command for scheduled PUSCH – 2 bits
  count += 2;

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, otherwise 0
  if (cfg->enable_sul) {
    count++;
  }

  return count;
}

static int dci_nr_format_0_0_pack(const srsran_dci_nr_t* q, const srsran_dci_ul_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  bool               is_common_ss = SRSRAN_SEARCH_SPACE_IS_COMMON(msg->ctx.ss_type);
  uint32_t           trunc        = is_common_ss ? q->dci_0_0_common_trunc : 0; // hard-coded bit truncation
  uint32_t           padding      = is_common_ss ? q->dci_0_0_common_padd : q->dci_0_0_ue_padd; // Hard-coded padding
  uint8_t*           y            = msg->payload;
  srsran_rnti_type_t rnti_type    = msg->ctx.rnti_type;
  uint32_t           N_UL_BWP_RB  = is_common_ss ? q->cfg.bwp_ul_initial_bw : q->cfg.bwp_ul_active_bw;

  // 1st of all, copy DCI context
  msg->ctx = dci->ctx;

  // Check RNTI type
  if (rnti_type != srsran_rnti_type_c && rnti_type != srsran_rnti_type_cs && rnti_type != srsran_rnti_type_mcs_c) {
    return SRSRAN_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  *y = 0;
  y++;

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  int N_UL_hop = (q->cfg.enable_hopping) ? (N_UL_BWP_RB ? 1 : 2) : 0;
  srsran_bit_unpack(dci->frequency_offset, &y, N_UL_hop);

  // Frequency domain resource assignment
  uint32_t N = dci_nr_freq_resource_size_type1(N_UL_BWP_RB);
  if (N <= N_UL_hop) {
    return SRSRAN_ERROR;
  }
  srsran_bit_unpack(dci->freq_domain_assigment, &y, (int)(N - N_UL_hop - trunc));

  // Time domain resource assignment – 4 bits
  srsran_bit_unpack(dci->time_domain_assigment, &y, 4);

  // Frequency hopping flag – 1 bit
  srsran_bit_unpack(dci->freq_hopping_flag, &y, 1);

  // Modulation and coding scheme – 5 bits
  srsran_bit_unpack(dci->mcs, &y, 5);

  // New data indicator – 1 bit
  srsran_bit_unpack(dci->ndi, &y, 1);

  // Redundancy version – 2 bits
  srsran_bit_unpack(dci->rv, &y, 2);

  // HARQ process number – 4 bits
  srsran_bit_unpack(dci->pid, &y, 4);

  // TPC command for scheduled PUSCH – 2 bits
  srsran_bit_unpack(dci->tpc, &y, 2);

  // Padding goes here
  for (uint32_t i = 0; i < padding; i++) {
    *(y++) = 0;
  }

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, otherwise 0
  if (q->cfg.enable_sul) {
    *(y++) = 0;
  }

  msg->nof_bits = srsran_dci_nr_size(q, msg->ctx.ss_type, srsran_dci_format_nr_0_0);
  if (msg->nof_bits != y - msg->payload) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, (int)(y - msg->payload));
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_0_0_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci)
{
  uint32_t                   trim           = 0;     // hard-coded bit trimming
  bool                       enable_hopping = false; // hard-coded PUSCH hopping
  uint32_t                   padding        = 8;     // Hard-coded padding
  uint8_t*                   y              = msg->payload;
  srsran_rnti_type_t         rnti_type      = msg->ctx.rnti_type;
  srsran_search_space_type_t ss_type        = msg->ctx.ss_type;
  uint32_t N_UL_BWP_RB = SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type) ? q->cfg.bwp_ul_initial_bw : q->cfg.bwp_ul_active_bw;

  // Check RNTI type
  if (rnti_type != srsran_rnti_type_c && rnti_type != srsran_rnti_type_cs && rnti_type != srsran_rnti_type_mcs_c) {
    ERROR("Unsupported %s", srsran_rnti_type_str(rnti_type));
    return SRSRAN_ERROR;
  }

  uint32_t nof_bits = srsran_dci_nr_size(q, ss_type, srsran_dci_format_nr_0_0);
  if (msg->nof_bits != nof_bits) {
    ERROR("Invalid number of bits %d, expected %d", msg->nof_bits, nof_bits);
    return SRSRAN_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (*(y++) != 0) {
    ERROR("Wrond DCI format");
    return SRSRAN_ERROR;
  }

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  uint32_t N_UL_hop     = (enable_hopping) ? ((N_UL_BWP_RB < 50) ? 1 : 2) : 0;
  dci->frequency_offset = srsran_bit_pack(&y, N_UL_hop);

  // Frequency domain resource assignment
  uint32_t N = dci_nr_freq_resource_size_type1(N_UL_BWP_RB);
  if (N < N_UL_hop) {
    return SRSRAN_ERROR;
  }
  dci->freq_domain_assigment = srsran_bit_pack(&y, N - N_UL_hop - trim);

  // Time domain resource assignment – 4 bits
  dci->time_domain_assigment = srsran_bit_pack(&y, 4);

  // Frequency hopping flag – 1 bit
  dci->freq_hopping_flag = srsran_bit_pack(&y, 1);

  // Modulation and coding scheme – 5 bits
  dci->mcs = srsran_bit_pack(&y, 5);

  // New data indicator – 1 bit
  dci->ndi = srsran_bit_pack(&y, 1);

  // Redundancy version – 2 bits
  dci->rv = srsran_bit_pack(&y, 2);

  // HARQ process number – 4 bits
  dci->pid = srsran_bit_pack(&y, 4);

  // TPC command for scheduled PUSCH – 2 bits
  dci->tpc = srsran_bit_pack(&y, 2);

  // Padding goes here
  for (uint32_t i = 0; i < padding; i++) {
    y++;
  }

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, otherwise 0
  if (q->cfg.enable_sul) {
    dci->sul = srsran_bit_pack(&y, 1);
  }

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_0_0_to_str(const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  // Print format
  len = srsran_print_check(
      str, str_len, len, "rnti=%04x L=%d cce=%d dci=0_0 ", dci->ctx.rnti, dci->ctx.location.L, dci->ctx.location.ncce);

  // Frequency domain resource assignment
  len = srsran_print_check(str, str_len, len, "f_alloc=0x%x ", dci->freq_domain_assigment);

  // Time domain resource assignment – 4 bits
  len = srsran_print_check(str, str_len, len, "t_alloc=0x%x ", dci->time_domain_assigment);

  // Frequency hopping flag – 1 bit
  len = srsran_print_check(str, str_len, len, "hop=%c ", dci->freq_hopping_flag == 0 ? 'n' : 'y');

  // Modulation and coding scheme – 5 bits
  len = srsran_print_check(str, str_len, len, "mcs=%d ", dci->mcs);

  // New data indicator – 1 bit
  len = srsran_print_check(str, str_len, len, "ndi=%d ", dci->ndi);

  // Redundancy version – 2 bits
  len = srsran_print_check(str, str_len, len, "rv=%d ", dci->rv);

  // HARQ process number – 4 bits
  len = srsran_print_check(str, str_len, len, "harq_id=%d ", dci->harq_feedback);

  // TPC command for scheduled PUSCH – 2 bits
  len = srsran_print_check(str, str_len, len, "tpc=%d ", dci->tpc);

  return len;
}

static uint32_t dci_nr_format_0_1_sizeof(const srsran_dci_cfg_nr_t* cfg, srsran_rnti_type_t rnti_type)
{
  uint32_t count = 0;

  if (rnti_type != srsran_rnti_type_c && rnti_type != srsran_rnti_type_cs && rnti_type != srsran_rnti_type_sp_csi &&
      rnti_type != srsran_rnti_type_mcs_c) {
    ERROR("Invalid RNTI (%s) for format 0_1", srsran_rnti_type_str(rnti_type));
    return SRSRAN_ERROR;
  }

  // Identifier for DCI formats – 1 bit
  count += 1;

  // Carrier indicator – 0 or 3 bits
  count += SRSRAN_MIN(cfg->carrier_indicator_size, 3);

  // UL/SUL indicator – 0 bit for UEs not configured with supplementaryUplink ... otherwise, 1 bit
  count += cfg->enable_sul ? 1 : 0;

  // Bandwidth part indicator – 0, 1 or 2 bits
  count += SRSRAN_MIN(dci_nr_bwp_id_size(cfg->nof_ul_bwp), 2);

  // Frequency domain resource assignment
  count += dci_nr_freq_resource_size(cfg->pusch_alloc_type, cfg->nof_rb_groups, cfg->bwp_ul_active_bw);

  // Time domain resource assigment - 0, 1, 2, 3, or 4 bits
  count += dci_nr_time_res_size(cfg->nof_ul_time_res);

  // Frequency hopping flag - 0 or 1 bit:
  if (cfg->pusch_alloc_type == srsran_resource_alloc_type0 || !cfg->enable_hopping) {
    count += 0;
  } else {
    count += 1;
  }

  // Modulation and coding scheme – 5 bits
  count += 5;

  // New data indicator – 1 bit
  count += 1;

  // Redundancy version – 2 bits
  count += 2;

  // HARQ process number – 4 bits
  count += 4;

  // 1st DAI - 1 or 2 bits
  if (cfg->pusch_tx_config_codebook == srsran_pdsch_harq_ack_codebook_semi_static) {
    count += 1;
  } else {
    count += 2;
  }

  // 2st DAI - 0 or 2 bits
  if (cfg->dynamic_dual_harq_ack_codebook) {
    count += 2;
  } else {
    count += 0;
  }

  // TPC command for scheduled PUSCH – 2 bits
  count += 2;

  // SRS resource indicator
  uint32_t N_srs = SRSRAN_MIN(1, cfg->nof_srs);
  if (cfg->pusch_tx_config_codebook) {
    uint32_t N = 0;
    for (uint32_t k = 1; k < SRSRAN_MIN(cfg->nof_ul_layers, cfg->nof_srs); k++) {
      N += cfg->nof_srs / k;
    }
    count += (uint32_t)CEIL_LOG2(N);
  } else {
    count += (uint32_t)CEIL_LOG2(N_srs);
  }

  // Precoding information and number of layers
  if (cfg->pusch_tx_config_codebook) {
    ERROR("Not implemented");
    return 0;
  }
  count += 0;

  // Antenna ports
  if (!cfg->enable_transform_precoding && !cfg->pusch_dmrs_double) {
    count += 3;
  } else {
    ERROR("Not implemented");
    return 0;
  }

  // SRS request - 2 bits
  count += 2;

  // CSI request - 0, 1, 2, 3, 4, 5, or 6 bits
  count += SRSRAN_MIN(6, cfg->report_trigger_size);

  // CBG transmission information - 0, 2, 4, 6, or 8 bits
  count += cfg->pusch_nof_cbg;

  // PTRS-DMRS association - 0 or 2 bits
  if ((!cfg->pusch_ptrs && !cfg->enable_transform_precoding) || cfg->enable_transform_precoding ||
      cfg->nof_ul_layers <= 1) {
    count += 0;
  } else {
    count += 3;
  }

  // beta_offset indicator – 0 or 2 bits
  if (cfg->pusch_dynamic_betas) {
    count += 2;
  }

  // DMRS sequence initialization - 0 or 1 bit
  if (!cfg->enable_transform_precoding) {
    count += 1;
  }

  // UL-SCH indicator – 1 bit
  count += 1;

  return count;
}

static int dci_nr_format_0_1_pack(const srsran_dci_nr_t* q, const srsran_dci_ul_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  // TODO!

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_0_1_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci)
{
  // TODO!

  return SRSRAN_SUCCESS;
}

static uint32_t dci_nr_format_1_0_sizeof(uint32_t N_DL_BWP_RB, srsran_rnti_type_t rnti_type)
{
  uint32_t count = 0;

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 1;
  }

  if (rnti_type == srsran_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    count += 2;

    // Short Messages – 8 bits
    count += 8;
  }

  // Frequency domain resource assignment
  int N = dci_nr_freq_resource_size_type1(N_DL_BWP_RB);
  if (N < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  count += N;

  // Time domain resource assignment – 4 bits
  count += 4;

  // VRB-to-PRB mapping – 1 bit
  count += 1;

  // Modulation and coding scheme – 5 bits
  count += 5;

  // TB scaling – 2 bits
  if (rnti_type == srsran_rnti_type_p || rnti_type == srsran_rnti_type_ra) {
    count += 2;
  }

  // New data indicator – 1 bit
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 1;
  }

  // Redundancy version – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_tc) {
    count += 2;
  }

  // HARQ process number – 4 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 4;
  }

  // System information indicator – 1 bit
  if (rnti_type == srsran_rnti_type_si) {
    count += 1;
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 2;
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 2;
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 3;
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    count += 3;
  }

  // Reserved bits ...
  if (rnti_type == srsran_rnti_type_p) {
    // ... – 6 bits
    count += 2;
  } else if (rnti_type == srsran_rnti_type_si) {
    // ... – 15 bits
    count += 15;
  } else if (rnti_type == srsran_rnti_type_ra) {
    // ... – 16 bits
    count += 16;
  }

  return count;
}

static int dci_nr_format_1_0_pack(const srsran_dci_nr_t* q, const srsran_dci_dl_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  uint8_t*                   y           = msg->payload;
  srsran_rnti_type_t         rnti_type   = msg->ctx.rnti_type;
  srsran_search_space_type_t ss_type     = dci->ctx.ss_type;
  uint32_t                   N_DL_BWP_RB = SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)
                             ? (q->cfg.coreset0_bw == 0) ? q->cfg.bwp_dl_initial_bw : q->cfg.coreset0_bw
                             : q->cfg.bwp_dl_active_bw;

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    *y = 1;
    y++;
  }

  if (rnti_type == srsran_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    srsran_bit_unpack(dci->smi, &y, 2);

    // Short Messages – 8 bits
    srsran_bit_unpack(dci->sm, &y, 8);
  }

  // Frequency domain resource assignment
  int N = dci_nr_freq_resource_size_type1(N_DL_BWP_RB);
  if (N < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  srsran_bit_unpack(dci->freq_domain_assigment, &y, N);

  // Time domain resource assignment – 4 bits
  srsran_bit_unpack(dci->time_domain_assigment, &y, 4);

  // VRB-to-PRB mapping – 1 bit
  srsran_bit_unpack(dci->vrb_to_prb_mapping, &y, 1);

  // Modulation and coding scheme – 5 bits
  srsran_bit_unpack(dci->mcs, &y, 5);

  // TB scaling – 2 bits
  if (rnti_type == srsran_rnti_type_p || rnti_type == srsran_rnti_type_ra) {
    srsran_bit_unpack(dci->tb_scaling, &y, 2);
  }

  // New data indicator – 1 bit
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->ndi, &y, 1);
  }

  // Redundancy version – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->rv, &y, 2);
  }

  // HARQ process number – 4 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->pid, &y, 4);
  }

  // System information indicator – 1 bit
  if (rnti_type == srsran_rnti_type_si) {
    srsran_bit_unpack(dci->sii, &y, 1);
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->dai, &y, 2);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->tpc, &y, 2);
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->pucch_resource, &y, 3);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    srsran_bit_unpack(dci->harq_feedback, &y, 3);
  }

  // Reserved bits ...
  if (rnti_type == srsran_rnti_type_p) {
    // ... – 6 bits
    srsran_bit_unpack(dci->reserved, &y, 6);
  } else if (rnti_type == srsran_rnti_type_si) {
    // ... – 15 bits
    srsran_bit_unpack(dci->reserved, &y, 15);
  } else if (rnti_type == srsran_rnti_type_ra) {
    // ... – 16 bits
    srsran_bit_unpack(dci->reserved, &y, 16);
  }

  msg->nof_bits     = srsran_dci_nr_size(q, msg->ctx.ss_type, srsran_dci_format_nr_1_0);
  uint32_t nof_bits = (uint32_t)(y - msg->payload);
  if (msg->nof_bits != nof_bits) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, nof_bits);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_1_0_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_dl_nr_t* dci)
{
  uint8_t*                   y           = msg->payload;
  srsran_rnti_type_t         rnti_type   = msg->ctx.rnti_type;
  srsran_search_space_type_t ss_type     = msg->ctx.ss_type;
  uint32_t                   N_DL_BWP_RB = SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)
                             ? (q->cfg.coreset0_bw == 0) ? q->cfg.bwp_dl_initial_bw : q->cfg.coreset0_bw
                             : q->cfg.bwp_dl_active_bw;

  uint32_t nof_bits = srsran_dci_nr_size(q, ss_type, srsran_dci_format_nr_1_0);
  if (msg->nof_bits != nof_bits) {
    ERROR("Invalid number of bits %d, expected %d", msg->nof_bits, nof_bits);
    return SRSRAN_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    // The value of this bit field is always set to 1, indicating a DL DCI format
    if (*(y++) != 1) {
      ERROR("Wrond DCI format");
      return SRSRAN_ERROR;
    }
  }

  if (rnti_type == srsran_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    dci->smi = srsran_bit_pack(&y, 2);

    // Short Messages – 8 bits
    dci->sm = srsran_bit_pack(&y, 8);
  }

  // Frequency domain resource assignment
  int N = dci_nr_freq_resource_size_type1(N_DL_BWP_RB);
  if (N < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  dci->freq_domain_assigment = srsran_bit_pack(&y, N);

  // Time domain resource assignment – 4 bits
  dci->time_domain_assigment = srsran_bit_pack(&y, 4);

  // VRB-to-PRB mapping – 1 bit
  dci->vrb_to_prb_mapping = srsran_bit_pack(&y, 1);

  // Modulation and coding scheme – 5 bits
  dci->mcs = srsran_bit_pack(&y, 5);

  // TB scaling – 2 bits
  if (rnti_type == srsran_rnti_type_p || rnti_type == srsran_rnti_type_ra) {
    dci->tb_scaling = srsran_bit_pack(&y, 2);
  }

  // New data indicator – 1 bit
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->ndi = srsran_bit_pack(&y, 1);
  }

  // Redundancy version – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_tc) {
    dci->rv = srsran_bit_pack(&y, 2);
  }

  // HARQ process number – 4 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->pid = srsran_bit_pack(&y, 4);
  }

  // System information indicator – 1 bit
  if (rnti_type == srsran_rnti_type_si) {
    dci->sii = srsran_bit_pack(&y, 1);
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->dai = srsran_bit_pack(&y, 2);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->tpc = srsran_bit_pack(&y, 2);
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->pucch_resource = srsran_bit_pack(&y, 3);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    dci->harq_feedback = srsran_bit_pack(&y, 3);
  }

  // Reserved bits ...
  if (rnti_type == srsran_rnti_type_p) {
    // ... – 6 bits
    dci->reserved = srsran_bit_pack(&y, 6);
  } else if (rnti_type == srsran_rnti_type_si) {
    // ... – 15 bits
    dci->reserved = srsran_bit_pack(&y, 15);
  } else if (rnti_type == srsran_rnti_type_ra) {
    // ... – 16 bits
    dci->reserved = srsran_bit_pack(&y, 16);
  }

  if (msg->nof_bits != y - msg->payload) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, (int)(y - msg->payload));
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_1_0_to_str(const srsran_dci_dl_nr_t* dci, char* str, uint32_t str_len)
{
  uint32_t           len       = 0;
  srsran_rnti_type_t rnti_type = dci->ctx.rnti_type;

  // Print format
  len = srsran_print_check(
      str, str_len, len, "rnti=%04x L=%d cce=%d dci=1_0 ", dci->ctx.rnti, dci->ctx.location.L, dci->ctx.location.ncce);

  if (rnti_type == srsran_rnti_type_p) {
    len = srsran_print_check(str, str_len, len, "smi=%d sm=%d ", dci->smi, dci->sm);
  }

  // Frequency domain resource assignment
  len = srsran_print_check(str, str_len, len, "f_alloc=0x%x ", dci->freq_domain_assigment);

  // Time domain resource assignment – 4 bits
  len = srsran_print_check(str, str_len, len, "t_alloc=0x%x ", dci->time_domain_assigment);

  // VRB-to-PRB mapping – 1 bit
  len = srsran_print_check(str, str_len, len, "vrb_to_prb_map=%d ", dci->vrb_to_prb_mapping);

  // Modulation and coding scheme – 5 bits
  len = srsran_print_check(str, str_len, len, "mcs=%d ", dci->mcs);

  // TB scaling – 2 bits
  if (rnti_type == srsran_rnti_type_p || rnti_type == srsran_rnti_type_ra) {
    len = srsran_print_check(str, str_len, len, "tb_scaling=%d ", dci->tb_scaling);
  }

  // New data indicator – 1 bit
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "ndi=%d ", dci->ndi);
  }

  // Redundancy version – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "rv=%d ", dci->rv);
  }

  // HARQ process number – 4 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "harq_id=%d ", dci->harq_feedback);
  }

  // System information indicator – 1 bit
  if (rnti_type == srsran_rnti_type_si) {
    len = srsran_print_check(str, str_len, len, "sii=%d ", dci->sii);
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "dai=%d ", dci->dai);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "pucch_tpc=%d ", dci->tpc);
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "pucch_res=%d ", dci->pucch_resource);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_tc) {
    len = srsran_print_check(str, str_len, len, "harq_feedback=%d ", dci->harq_feedback);
  }

  // Reserved bits ...
  if (rnti_type == srsran_rnti_type_p || rnti_type == srsran_rnti_type_si || rnti_type == srsran_rnti_type_ra) {
    len = srsran_print_check(str, str_len, len, "reserved=0x%x ", dci->reserved);
  }

  return len;
}

static uint32_t dci_nr_format_1_1_sizeof(const srsran_dci_cfg_nr_t* cfg, srsran_rnti_type_t rnti_type)
{
  int count = 0;

  if (rnti_type != srsran_rnti_type_c && rnti_type != srsran_rnti_type_cs && rnti_type != srsran_rnti_type_mcs_c) {
    ERROR("Invalid RNTI (%s) for format 1_1", srsran_rnti_type_str(rnti_type));
    return SRSRAN_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  count += 1;

  // Carrier indicator – 0 or 3 bits
  count += (int)SRSRAN_MIN(cfg->carrier_indicator_size, 3);

  // Bandwidth part indicator – 0, 1 or 2 bits
  count += (int)SRSRAN_MIN(dci_nr_bwp_id_size(cfg->nof_ul_bwp), 2);

  // Frequency domain resource assignment
  count += dci_nr_freq_resource_size(cfg->pdsch_alloc_type, cfg->nof_rb_groups, cfg->bwp_dl_active_bw);

  // Time domain resource assignment – 0, 1, 2, 3, or 4 bits
  count += dci_nr_time_res_size(cfg->nof_dl_time_res);

  // VRB-to-PRB mapping – 0 or 1
  if (cfg->pdsch_alloc_type != srsran_resource_alloc_type0 && cfg->pdsch_inter_prb_to_prb) {
    count += 1;
  }

  // PRB bundling size indicator – 0 or 1 bits
  // ... not implemented

  // Rate matching indicator – 0, 1, or 2 bits
  if (cfg->pdsch_rm_pattern1) {
    count += 1;
  }
  if (cfg->pdsch_rm_pattern2) {
    count += 1;
  }

  // ZP CSI-RS trigger - 0, 1, or 2 bits
  count += (int)CEIL_LOG2(cfg->nof_aperiodic_zp + 1);

  // For transport block 1:
  // Modulation and coding scheme – 5 bits
  count += 5;

  // New data indicator – 1 bit
  count += 1;

  // Redundancy version – 2 bits
  count += 2;

  // For transport block 2:
  if (cfg->pdsch_2cw) {
    // Modulation and coding scheme – 5 bits
    count += 5;

    // New data indicator – 1 bit
    count += 1;

    // Redundancy version – 2 bits
    count += 2;
  }

  // HARQ process number – 4 bits
  count += 4;

  // Downlink assignment index (dynamic HARQ-ACK codebook only)
  if (cfg->harq_ack_codebok == srsran_pdsch_harq_ack_codebook_dynamic) {
    if (cfg->multiple_scell) {
      count += 4;
    } else {
      count += 2;
    }
  }

  // TPC command for scheduled PUCCH – 2 bits
  count += 2;

  // PDSCH-to-HARQ_feedback timing indicator – 0, 1, 2, or 3 bits
  count += (int)CEIL_LOG2(cfg->nof_dl_to_ul_ack);

  // Antenna port(s) – 4, 5, or 6 bits
  count += 4;
  if (cfg->pdsch_dmrs_type2) {
    count++;
  }
  if (cfg->pdsch_dmrs_double) {
    count++;
  }

  // Transmission configuration indication – 0 or 3 bits
  if (cfg->pdsch_tci) {
    count += 3;
  }

  // SRS request – 2 or 3 bits
  count += 2;
  if (cfg->enable_sul) {
    count++;
  }

  // CBG transmission information (CBGTI) – 0, 2, 4, 6, or 8 bits
  count += cfg->pdsch_nof_cbg;

  // CBG flushing out information (CBGFI) – 0 or 1 bit
  if (cfg->pdsch_cbg_flush) {
    count += 1;
  }

  // DMRS sequence initialization – 1 bit
  count += 1;

  return count;
}

static int dci_nr_format_1_1_pack(const srsran_dci_nr_t* q, const srsran_dci_dl_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  // TODO!

  return SRSRAN_SUCCESS;
}

static int dci_nr_format_1_1_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_dl_nr_t* dci)
{
  // TODO!

  return SRSRAN_SUCCESS;
}

int srsran_dci_nr_set_cfg(srsran_dci_nr_t* q, const srsran_dci_cfg_nr_t* cfg)
{
  // Reset current setup
  SRSRAN_MEM_ZERO(q, srsran_dci_nr_t, 1);

  // Copy configuration
  q->cfg = *cfg;

  // Step 0
  // - Determine DCI format 0_0 monitored in a common search space according to clause 7.3.1.1.1 where N_UL_BWP_RB is
  // given by the size of the initial UL bandwidth part.
  uint32_t size_dci_0_0_common = dci_nr_format_0_0_sizeof(cfg->bwp_ul_initial_bw, cfg);

  // - Determine DCI format 1_0 monitored in a common search space according to clause 7.3.1.2.1 where N_DL_BWP_RB given
  // by:
  //   - the size of CORESET 0 if CORESET 0 is configured for the cell; and
  //   - the size of initial DL bandwidth part if CORESET 0 is not configured for the cell.
  uint32_t size_dci_1_0_common = dci_nr_format_1_0_sizeof(cfg->bwp_dl_initial_bw, srsran_rnti_type_c);
  if (cfg->coreset0_bw != 0) {
    size_dci_1_0_common = dci_nr_format_1_0_sizeof(cfg->coreset0_bw, srsran_rnti_type_c);
  }

  // - If DCI format 0_0 is monitored in common search space and if the number of information bits in the DCI format 0_0
  // prior to padding is less than the payload size of the DCI format 1_0 monitored in common search space for
  // scheduling the same serving cell, a number of zero padding bits are generated for the DCI format 0_0 until the
  // payload size equals that of the DCI format 1_0.
  if (cfg->monitor_common_0_0 && size_dci_0_0_common < size_dci_1_0_common) {
    q->dci_0_0_common_padd = size_dci_1_0_common - size_dci_0_0_common;
  } else {
    q->dci_0_0_common_padd = 0;
  }

  // - If DCI format 0_0 is monitored in common search space and if the number of information bits in the DCI format 0_0
  // prior to truncation is larger than the payload size of the DCI format 1_0 monitored in common search space for
  // scheduling the same serving cell, the bitwidth of the frequency domain resource assignment field in the DCI format
  // 0_0 is reduced by truncating the first few most significant bits such that the size of DCI format 0_0 equals the
  // size of the DCI format 1_0.
  if (cfg->monitor_common_0_0 && size_dci_0_0_common > size_dci_1_0_common) {
    q->dci_0_0_common_trunc = size_dci_0_0_common - size_dci_1_0_common;
  } else {
    q->dci_0_0_common_trunc = 0;
  }

  q->dci_0_0_and_1_0_common_size = SRSRAN_MAX(size_dci_1_0_common, DCI_NR_MIN_SIZE);

  // Step 1
  // - Determine DCI format 0_0 monitored in a UE-specific search space according to clause 7.3.1.1.1 where N_UL_BWP_RB
  // is the size of the active UL bandwidth part.
  uint32_t size_dci_0_0_ue = dci_nr_format_0_0_sizeof(cfg->bwp_ul_active_bw, cfg);

  // - Determine DCI format 1_0 monitored in a UE-specific search space according to clause 7.3.1.2.1 where N_DL_BWP_RB
  // is the size of the active DL bandwidth part.
  uint32_t size_dci_1_0_ue = dci_nr_format_1_0_sizeof(cfg->bwp_dl_active_bw, srsran_rnti_type_c);

  // - For a UE configured with supplementaryUplink in ServingCellConfig in a cell, if PUSCH is configured to be
  // transmitted on both the SUL and the non-SUL of the cell and if the number of information bits in DCI format 0_0 in
  // UE-specific search space for the SUL is not equal to the number of information bits in DCI format 0_0 in
  // UE-specific search space for the non-SUL, a number of zero padding bits are generated for the smaller DCI format
  // 0_0 until the payload size equals that of the larger DCI format 0_0.
  // ... Not implemented

  // - If DCI format 0_0 is monitored in UE-specific search space and if the number of information bits in the DCI
  // format 0_0 prior to padding is less than the payload size of the DCI format 1_0 monitored in UE-specific search
  // space for scheduling the same serving cell, a number of zero padding bits are generated for the DCI format 0_0
  // until the payload size equals that of the DCI format 1_0.
  if (cfg->monitor_0_0_and_1_0 && size_dci_0_0_ue < size_dci_1_0_ue) {
    q->dci_0_0_ue_padd = size_dci_1_0_ue - size_dci_0_0_ue;
  }

  // - If DCI format 1_0 is monitored in UE-specific search space and if the number of information bits in the DCI
  // format 1_0 prior to padding is less than the payload size of the DCI format 0_0 monitored in UE-specific search
  // space for scheduling the same serving cell, zeros shall be appended to the DCI format 1_0 until the payload size
  // equals that of the DCI format 0_0
  if (cfg->monitor_0_0_and_1_0 && size_dci_1_0_ue < size_dci_0_0_ue) {
    q->dci_1_0_ue_padd = size_dci_0_0_ue - size_dci_1_0_ue;
  }

  q->dci_0_0_and_1_0_ue_size = SRSRAN_MAX(SRSRAN_MAX(size_dci_0_0_ue, size_dci_1_0_ue), DCI_NR_MIN_SIZE);

  // Step 2
  // For a UE configured with supplementaryUplink in ServingCellConfig in a cell, if PUSCH is configured to be
  // transmitted on both the SUL and the non-SUL of the cell and if the number of information bits in format 0_1 for
  // the SUL is not equal to the number of information bits in format 0_1 for the non-SUL, zeros shall be appended
  // to smaller format 0_1 until the payload size equals that of the larger format 0_1.
  // ... Not implemented

  uint32_t size_dci_0_1 = dci_nr_format_0_1_sizeof(cfg, srsran_rnti_type_c);
  uint32_t size_dci_1_1 = dci_nr_format_1_1_sizeof(cfg, srsran_rnti_type_c);
  if (size_dci_0_1 == 0 || size_dci_1_1 == 0) {
    return SRSRAN_ERROR;
  }

  // If the size of DCI format 0_1 monitored in a UE-specific search space equals that of a DCI format 0_0/1_0
  // monitored in another UE-specific search space, one bit of zero padding shall be appended to DCI format 0_1.
  if (size_dci_0_1 == q->dci_0_0_and_1_0_ue_size) {
    q->dci_0_1_padd++;
  }

  // If the size of DCI format 1_1 monitored in a UE-specific search space equals that of a DCI format 0_0/1_0
  // monitored in another UE-specific search space, one bit of zero padding shall be appended to DCI format 1_1.
  if (size_dci_1_1 == q->dci_0_0_and_1_0_ue_size) {
    q->dci_1_1_padd++;
  }

  q->dci_0_1_size = size_dci_0_1 + q->dci_0_1_padd;
  q->dci_1_1_size = size_dci_1_1 + q->dci_1_1_padd;

  // Step 3
  // If both of the following conditions are fulfilled the size alignment procedure is complete:
  // - the total number of different DCI sizes configured to monitor is no more than 4 for the cell
  // - the total number of different DCI sizes with C-RNTI configured to monitor is no more than 3 for the cell
  // ... Current code is compatible with only possible sizes!

  return SRSRAN_SUCCESS;
}

uint32_t srsran_dci_nr_size(const srsran_dci_nr_t* q, srsran_search_space_type_t ss_type, srsran_dci_format_nr_t format)
{
  // Check input
  if (q == NULL) {
    return 0;
  }

  // For common search space, only formats 0_0 and 1_0
  if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss_type)) {
    return q->dci_0_0_and_1_0_common_size;
  }

  // DCI formats 0_0 and 1_0 in UE-specific
  if (format == srsran_dci_format_nr_0_0 || format == srsran_dci_format_nr_1_0) {
    return q->dci_0_0_and_1_0_ue_size;
  }

  // DCI format 0_1 in UE-specific
  if (format == srsran_dci_format_nr_0_1) {
    return q->dci_0_1_size;
  }

  // DCI format 1_1 in UE-specific
  if (format == srsran_dci_format_nr_1_1) {
    return q->dci_1_1_size;
  }

  // Not implemented
  return 0;
}

bool srsran_dci_nr_valid_direction(const srsran_dci_msg_nr_t* dci)
{
  // Check pointer
  if (dci == NULL) {
    return false;
  }

  // UL direction
  uint32_t expected_direction = 0;

  // Set DL direction if is DL grant
  if (dci->ctx.format == srsran_dci_format_nr_1_0 || dci->ctx.format == srsran_dci_format_nr_1_1) {
    expected_direction = 1;
  }

  // The format bit is only present for these RNTI
  if (dci->ctx.rnti_type == srsran_rnti_type_c || dci->ctx.rnti_type == srsran_rnti_type_tc) {
    return dci->payload[0] == expected_direction;
  }

  // For other RNTI types, assume always DL on 1_0
  return (dci->ctx.format == srsran_dci_format_nr_1_0);
}

static int dci_nr_rar_pack(const srsran_dci_nr_t* q, const srsran_dci_ul_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  ERROR("Not implemented");
  return SRSRAN_ERROR;
}

static int dci_nr_rar_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci)
{
  if (msg == NULL || dci == NULL) {
    return SRSRAN_ERROR;
  }

  uint8_t* y = msg->payload;

  // Copy DCI MSG fields
  dci->ctx = msg->ctx;

  // Frequency hopping flag - 1 bit
  dci->freq_hopping_flag = srsran_bit_pack(&y, 1);

  // PUSCH frequency resource allocation - 14 bits
  dci->freq_domain_assigment = srsran_bit_pack(&y, 14);

  // PUSCH time resource allocation - 4 bits
  dci->time_domain_assigment = srsran_bit_pack(&y, 4);

  // MCS -4 bits
  dci->mcs = srsran_bit_pack(&y, 4);

  // TPC command for PUSCH - 3 bits
  dci->tpc = srsran_bit_pack(&y, 3);

  // CSI request - 1 bits
  dci->csi_request = srsran_bit_pack(&y, 3);

  return SRSRAN_SUCCESS;
}

static int dci_nr_rar_to_str(const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  // Print format
  len = srsran_print_check(str, str_len, len, "rnti=%04x dci=rar ", dci->ctx.rnti);

  // Frequency hopping flag
  len = srsran_print_check(str, str_len, len, "hop=%d ", dci->freq_hopping_flag);

  // PUSCH frequency resource allocation
  len = srsran_print_check(str, str_len, len, "f_alloc=0x%x ", dci->freq_domain_assigment);

  // PUSCH time resource allocation
  len = srsran_print_check(str, str_len, len, "t_alloc=0x%x ", dci->time_domain_assigment);

  // Modulation and coding scheme
  len = srsran_print_check(str, str_len, len, "mcs=%d ", dci->mcs);

  // TPC command for scheduled PUSCH
  len = srsran_print_check(str, str_len, len, "tpc=%d ", dci->tpc);

  // CSI request
  len = srsran_print_check(str, str_len, len, "csi=%d ", dci->csi_request);

  return len;
}

int srsran_dci_nr_dl_pack(const srsran_dci_nr_t* q, const srsran_dci_dl_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  // Copy DCI MSG fields
  msg->ctx = dci->ctx;

  // Pack DCI
  switch (msg->ctx.format) {
    case srsran_dci_format_nr_1_0:
      return dci_nr_format_1_0_pack(q, dci, msg);
    case srsran_dci_format_nr_1_1:
      return dci_nr_format_1_1_pack(q, dci, msg);
    default:
      ERROR("Unsupported DCI format %d", msg->ctx.format);
  }

  return SRSRAN_ERROR;
}

int srsran_dci_nr_dl_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_dl_nr_t* dci)
{
  // Copy DCI MSG fields
  dci->ctx = msg->ctx;

  // Pack DCI
  switch (msg->ctx.format) {
    case srsran_dci_format_nr_1_0:
      return dci_nr_format_1_0_unpack(q, msg, dci);
    case srsran_dci_format_nr_1_1:
      return dci_nr_format_1_1_unpack(q, msg, dci);
    default:
      ERROR("Unsupported DCI format %d", msg->ctx.format);
  }
  return SRSRAN_ERROR;
}

int srsran_dci_nr_ul_pack(const srsran_dci_nr_t* q, const srsran_dci_ul_nr_t* dci, srsran_dci_msg_nr_t* msg)
{
  // Copy DCI MSG fields
  msg->ctx = dci->ctx;

  // Pack DCI
  switch (msg->ctx.format) {
    case srsran_dci_format_nr_0_0:
      return dci_nr_format_0_0_pack(q, dci, msg);
    case srsran_dci_format_nr_0_1:
      return dci_nr_format_0_1_pack(q, dci, msg);
    case srsran_dci_format_nr_rar:
      return dci_nr_rar_pack(q, dci, msg);
    default:
      ERROR("Unsupported DCI format %d", msg->ctx.format);
  }

  return SRSRAN_ERROR;
}

int srsran_dci_nr_ul_unpack(const srsran_dci_nr_t* q, srsran_dci_msg_nr_t* msg, srsran_dci_ul_nr_t* dci)
{
  // Copy DCI MSG fields
  dci->ctx = msg->ctx;

  // Pack DCI
  switch (msg->ctx.format) {
    case srsran_dci_format_nr_0_0:
      return dci_nr_format_0_0_unpack(q, msg, dci);
    case srsran_dci_format_nr_0_1:
      return dci_nr_format_0_1_unpack(q, msg, dci);
    case srsran_dci_format_nr_rar:
      return dci_nr_rar_unpack(q, msg, dci);
    default:
      ERROR("Unsupported DCI format %d", msg->ctx.format);
  }
  return SRSRAN_ERROR;
}

int srsran_dci_ul_nr_to_str(const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  // Pack DCI
  switch (dci->ctx.format) {
    case srsran_dci_format_nr_0_0:
      return dci_nr_format_0_0_to_str(dci, str, str_len);
    case srsran_dci_format_nr_rar:
      return dci_nr_rar_to_str(dci, str, str_len);
    default:; // Do nothing
  }

  return srsran_print_check(str, str_len, 0, "unknown");
}

int srsran_dci_dl_nr_to_str(const srsran_dci_dl_nr_t* dci, char* str, uint32_t str_len)
{
  // Pack DCI
  switch (dci->ctx.format) {
    case srsran_dci_format_nr_1_0:
      return dci_nr_format_1_0_to_str(dci, str, str_len);
    default:; // Do nothing
  }

  return srsran_print_check(str, str_len, 0, "unknown");
}
