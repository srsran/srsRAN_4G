/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/phch/dci_nr.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"

static int dci_nr_format_0_0_freq_resource_size(const srslte_carrier_nr_t* carrier)
{
  if (carrier == NULL) {
    return SRSLTE_ERROR;
  }

  return (int)ceil(log2(carrier->nof_prb * (carrier->nof_prb + 1) / 2.0));
}

static int dci_nr_format_1_0_freq_resource_size(const srslte_carrier_nr_t* carrier,
                                                const srslte_coreset_t*    coreset0,
                                                srslte_rnti_type_t         rnti_type)
{
  if (carrier == NULL) {
    return SRSLTE_ERROR;
  }

  uint32_t N_DL_BWP_RB = carrier->nof_prb;
  if (rnti_type == srslte_rnti_type_ra && coreset0 != NULL) {
    N_DL_BWP_RB = srslte_coreset_get_bw(coreset0);
  } else if (rnti_type == srslte_rnti_type_p || rnti_type == srslte_rnti_type_si) {
    if (coreset0 == NULL) {
      return SRSLTE_ERROR;
    }
    N_DL_BWP_RB = srslte_coreset_get_bw(coreset0);
  }

  return (int)ceil(log2(N_DL_BWP_RB * (N_DL_BWP_RB + 1) / 2.0));
}

bool srslte_dci_nr_format_1_0_valid(const srslte_dci_msg_nr_t* dci)
{
  // Check pointer
  if (dci == NULL) {
    return false;
  }

  // Wrong format
  if (dci->format != srslte_dci_format_nr_1_0) {
    return false;
  }

  // The format bit is only present for these RNTI
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    return dci->payload[0] == 1;
  }

  // Otherwise, the message might be format 1_0
  return true;
}

int srslte_dci_nr_pack(const srslte_carrier_nr_t* carrier,
                       const srslte_coreset_t*    coreset,
                       const srslte_dci_dl_nr_t*  dci,
                       srslte_dci_msg_nr_t*       msg)
{
  // Copy DCI MSG fields
  msg->location     = dci->location;
  msg->search_space = dci->search_space;
  msg->coreset_id   = dci->coreset_id;
  msg->rnti_type    = dci->rnti_type;
  msg->rnti         = dci->rnti;
  msg->format       = dci->format;

  // Pack DCI
  switch (msg->format) {
    case srslte_dci_format_nr_1_0:
      if (srslte_dci_nr_format_1_0_pack(carrier, coreset, dci, msg) < SRSLTE_SUCCESS) {
        ERROR("Error packing DL DCI");
        return SRSLTE_ERROR;
      }
      break;
    default:
      ERROR("Unsupported DCI format %d", msg->format);
      return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_0_0_pack(const srslte_carrier_nr_t* carrier,
                                  const srslte_coreset_t*    coreset0,
                                  const srslte_dci_ul_nr_t*  dci,
                                  srslte_dci_msg_nr_t*       msg)
{
  uint32_t           trim                 = 0;     // hard-coded bit trimming
  bool               enable_hopping       = false; // hard-coded PUSCH hopping
  uint32_t           padding              = 8;     // Hard-coded padding
  bool               supplementary_uplink = false; // Hard-coded supplementary Uplink
  uint8_t*           y                    = msg->payload;
  srslte_rnti_type_t rnti_type            = msg->rnti_type;

  if (carrier == NULL) {
    return SRSLTE_ERROR;
  }

  // Check RNTI type
  if (rnti_type != srslte_rnti_type_c && rnti_type != srslte_rnti_type_cs && rnti_type != srslte_rnti_type_mcs_c) {
    return SRSLTE_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  *y = 0;
  y++;

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  int N_UL_hop = (enable_hopping) ? ((carrier->nof_prb < 50) ? 1 : 2) : 0;
  srslte_bit_unpack(dci->frequency_offset, &y, N_UL_hop);

  // Frequency domain resource assignment
  int N = dci_nr_format_0_0_freq_resource_size(carrier);
  if (N < SRSLTE_SUCCESS || N - N_UL_hop <= 0) {
    return SRSLTE_ERROR;
  }
  srslte_bit_unpack(dci->freq_domain_assigment, &y, N - N_UL_hop - trim);

  // Time domain resource assignment – 4 bits
  srslte_bit_unpack(dci->time_domain_assigment, &y, 4);

  // Frequency hopping flag – 1 bit
  srslte_bit_unpack(dci->freq_hopping_flag, &y, 1);

  // Modulation and coding scheme – 5 bits
  srslte_bit_unpack(dci->mcs, &y, 5);

  // New data indicator – 1 bit
  srslte_bit_unpack(dci->ndi, &y, 1);

  // Redundancy version – 2 bits
  srslte_bit_unpack(dci->rv, &y, 2);

  // HARQ process number – 4 bits
  srslte_bit_unpack(dci->pid, &y, 4);

  // TPC command for scheduled PUSCH – 2 bits
  srslte_bit_unpack(dci->tpc, &y, 2);

  // Padding goes here
  for (uint32_t i = 0; i < padding; i++) {
    *(y++) = 0;
  }

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, othwerwise 0
  if (supplementary_uplink) {
    *(y++) = 0;
  }

  msg->nof_bits = srslte_dci_nr_format_0_0_sizeof(carrier, coreset0, rnti_type);
  if (msg->nof_bits != y - msg->payload) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, (int)(y - msg->payload));
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_0_0_unpack(const srslte_carrier_nr_t* carrier,
                                    const srslte_coreset_t*    coreset,
                                    srslte_dci_msg_nr_t*       msg,
                                    srslte_dci_ul_nr_t*        dci)
{
  uint32_t           trim                 = 0;     // hard-coded bit trimming
  bool               enable_hopping       = false; // hard-coded PUSCH hopping
  uint32_t           padding              = 8;     // Hard-coded padding
  bool               supplementary_uplink = false; // Hard-coded supplementary Uplink
  uint8_t*           y                    = msg->payload;
  srslte_rnti_type_t rnti_type            = msg->rnti_type;

  // Copy DCI MSG fields
  dci->location     = msg->location;
  dci->search_space = msg->search_space;
  dci->coreset_id   = msg->coreset_id;
  dci->rnti_type    = msg->rnti_type;
  dci->rnti         = msg->rnti;
  dci->format       = msg->format;

  if (carrier == NULL) {
    return SRSLTE_ERROR;
  }

  // Check RNTI type
  if (rnti_type != srslte_rnti_type_c && rnti_type != srslte_rnti_type_cs && rnti_type != srslte_rnti_type_mcs_c) {
    ERROR("Unsupported %s", srslte_rnti_type_str(rnti_type));
    return SRSLTE_ERROR;
  }

  if (msg->nof_bits != srslte_dci_nr_format_0_0_sizeof(carrier, coreset, rnti_type)) {
    ERROR("Invalid number of bits %d, expected %d",
          msg->nof_bits,
          srslte_dci_nr_format_0_0_sizeof(carrier, coreset, rnti_type));
    return SRSLTE_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (*(y++) != 0) {
    ERROR("Wrond DCI format");
    return SRSLTE_ERROR;
  }

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  int N_UL_hop          = (enable_hopping) ? ((carrier->nof_prb < 50) ? 1 : 2) : 0;
  dci->frequency_offset = srslte_bit_pack(&y, N_UL_hop);

  // Frequency domain resource assignment
  int N = dci_nr_format_0_0_freq_resource_size(carrier);
  if (N < SRSLTE_SUCCESS || N - N_UL_hop <= 0) {
    return SRSLTE_ERROR;
  }
  dci->freq_domain_assigment = srslte_bit_pack(&y, N - N_UL_hop - trim);

  // Time domain resource assignment – 4 bits
  dci->time_domain_assigment = srslte_bit_pack(&y, 4);

  // Frequency hopping flag – 1 bit
  dci->freq_hopping_flag = srslte_bit_pack(&y, 1);

  // Modulation and coding scheme – 5 bits
  dci->mcs = srslte_bit_pack(&y, 5);

  // New data indicator – 1 bit
  dci->ndi = srslte_bit_pack(&y, 1);

  // Redundancy version – 2 bits
  dci->rv = srslte_bit_pack(&y, 2);

  // HARQ process number – 4 bits
  dci->pid = srslte_bit_pack(&y, 4);

  // TPC command for scheduled PUSCH – 2 bits
  dci->tpc = srslte_bit_pack(&y, 2);

  // Padding goes here
  for (uint32_t i = 0; i < padding; i++) {
    y++;
  }

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, othwerwise 0
  if (supplementary_uplink) {
    y++;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_0_0_sizeof(const srslte_carrier_nr_t* carrier,
                                    const srslte_coreset_t*    coreset,
                                    srslte_rnti_type_t         rnti_type)
{
  uint32_t trim                 = 0;     // hard-coded bit trimming
  bool     enable_hopping       = false; // hard-coded PUSCH hopping
  uint32_t padding              = 8;     // Hard-coded padding
  bool     supplementary_uplink = false; // Hard-coded supplementary Uplink
  int      count                = 0;

  // Identifier for DCI formats – 1 bits
  count++;

  // For PUSCH hopping with resource allocation type 1 N UL_hop MSB bits are used to indicate the frequency offset
  int N_UL_hop = (enable_hopping) ? ((carrier->nof_prb < 50) ? 1 : 2) : 0;
  count += N_UL_hop;

  // Frequency domain resource assignment
  int N = dci_nr_format_0_0_freq_resource_size(carrier);
  if (N < SRSLTE_SUCCESS || N - N_UL_hop <= 0) {
    return SRSLTE_ERROR;
  }
  count += N - N_UL_hop - trim;

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

  // Padding goes here
  count += padding;

  // UL/SUL indicator – 1 bit for UEs configured with supplementaryUplink in ServingCellConfig, othwerwise 0
  if (supplementary_uplink) {
    count++;
  }

  return count;
}

static int dci_nr_format_0_0_to_str(const srslte_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  // Print format
  len = srslte_print_check(
      str, str_len, len, "rnti=%04x L=%d cce=%d dci=0_0 ", dci->rnti, dci->location.L, dci->location.ncce);

  // Frequency domain resource assignment
  len = srslte_print_check(str, str_len, len, "f_alloc=0x%x ", dci->freq_domain_assigment);

  // Time domain resource assignment – 4 bits
  len = srslte_print_check(str, str_len, len, "t_alloc=0x%x ", dci->time_domain_assigment);

  // Frequency hopping flag – 1 bit
  len = srslte_print_check(str, str_len, len, "hop=%c ", dci->freq_hopping_flag == 0 ? 'n' : 'y');

  // Modulation and coding scheme – 5 bits
  len = srslte_print_check(str, str_len, len, "mcs=%d ", dci->mcs);

  // New data indicator – 1 bit
  len = srslte_print_check(str, str_len, len, "ndi=%d ", dci->ndi);

  // Redundancy version – 2 bits
  len = srslte_print_check(str, str_len, len, "rv=%d ", dci->rv);

  // HARQ process number – 4 bits
  len = srslte_print_check(str, str_len, len, "harq_id=%d ", dci->harq_feedback);

  // TPC command for scheduled PUSCH – 2 bits
  len = srslte_print_check(str, str_len, len, "tpc=%d ", dci->tpc);

  return len;
}

int srslte_dci_nr_format_1_0_pack(const srslte_carrier_nr_t* carrier,
                                  const srslte_coreset_t*    coreset,
                                  const srslte_dci_dl_nr_t*  dci,
                                  srslte_dci_msg_nr_t*       msg)
{
  uint8_t*           y         = msg->payload;
  srslte_rnti_type_t rnti_type = msg->rnti_type;

  if (carrier == NULL) {
    return SRSLTE_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    *y = 1;
    y++;
  }

  if (rnti_type == srslte_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    srslte_bit_unpack(dci->smi, &y, 2);

    // Short Messages – 8 bits
    srslte_bit_unpack(dci->sm, &y, 8);
  }

  // Frequency domain resource assignment
  int N = dci_nr_format_1_0_freq_resource_size(carrier, coreset, rnti_type);
  if (N < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  srslte_bit_unpack(dci->freq_domain_assigment, &y, N);

  // Time domain resource assignment – 4 bits
  srslte_bit_unpack(dci->time_domain_assigment, &y, 4);

  // VRB-to-PRB mapping – 1 bit
  srslte_bit_unpack(dci->vrb_to_prb_mapping, &y, 1);

  // Modulation and coding scheme – 5 bits
  srslte_bit_unpack(dci->mcs, &y, 5);

  // TB scaling – 2 bits
  if (rnti_type == srslte_rnti_type_p || rnti_type == srslte_rnti_type_ra) {
    srslte_bit_unpack(dci->tb_scaling, &y, 2);
  }

  // New data indicator – 1 bit
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->ndi, &y, 1);
  }

  // Redundancy version – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_si || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->rv, &y, 2);
  }

  // HARQ process number – 4 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->pid, &y, 4);
  }

  // System information indicator – 1 bit
  if (rnti_type == srslte_rnti_type_si) {
    srslte_bit_unpack(dci->sii, &y, 1);
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->dai, &y, 2);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->tpc, &y, 2);
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->pucch_resource, &y, 3);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    srslte_bit_unpack(dci->harq_feedback, &y, 3);
  }

  // Reserved bits ...
  if (rnti_type == srslte_rnti_type_p) {
    // ... – 6 bits
    srslte_bit_unpack(dci->reserved, &y, 6);
  } else if (rnti_type == srslte_rnti_type_si) {
    // ... – 15 bits
    srslte_bit_unpack(dci->reserved, &y, 15);
  } else if (rnti_type == srslte_rnti_type_ra) {
    // ... – 16 bits
    srslte_bit_unpack(dci->reserved, &y, 16);
  }

  msg->nof_bits = srslte_dci_nr_format_1_0_sizeof(carrier, coreset, rnti_type);
  if (msg->nof_bits != y - msg->payload) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, (int)(y - msg->payload));
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_1_0_unpack(const srslte_carrier_nr_t* carrier,
                                    const srslte_coreset_t*    coreset,
                                    srslte_dci_msg_nr_t*       msg,
                                    srslte_dci_dl_nr_t*        dci)
{
  uint8_t*           y         = msg->payload;
  srslte_rnti_type_t rnti_type = msg->rnti_type;

  // Copy DCI MSG fields
  dci->location     = msg->location;
  dci->search_space = msg->search_space;
  dci->coreset_id   = msg->coreset_id;
  dci->rnti_type    = msg->rnti_type;
  dci->rnti         = msg->rnti;
  dci->format       = msg->format;

  if (msg->nof_bits != srslte_dci_nr_format_1_0_sizeof(carrier, coreset, rnti_type)) {
    ERROR("Invalid number of bits %d, expected %d",
          msg->nof_bits,
          srslte_dci_nr_format_1_0_sizeof(carrier, coreset, rnti_type));
    return SRSLTE_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    // The value of this bit field is always set to 1, indicating a DL DCI format
    if (*(y++) != 1) {
      ERROR("Wrond DCI format");
      return SRSLTE_ERROR;
    }
  }

  if (rnti_type == srslte_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    dci->smi = srslte_bit_pack(&y, 2);

    // Short Messages – 8 bits
    dci->sm = srslte_bit_pack(&y, 8);
  }

  // Frequency domain resource assignment
  int N = dci_nr_format_1_0_freq_resource_size(carrier, coreset, rnti_type);
  if (N < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  dci->freq_domain_assigment = srslte_bit_pack(&y, N);

  // Time domain resource assignment – 4 bits
  dci->time_domain_assigment = srslte_bit_pack(&y, 4);

  // VRB-to-PRB mapping – 1 bit
  dci->vrb_to_prb_mapping = srslte_bit_pack(&y, 1);

  // Modulation and coding scheme – 5 bits
  dci->mcs = srslte_bit_pack(&y, 5);

  // TB scaling – 2 bits
  if (rnti_type == srslte_rnti_type_p || rnti_type == srslte_rnti_type_ra) {
    dci->tb_scaling = srslte_bit_pack(&y, 2);
  }

  // New data indicator – 1 bit
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->ndi = srslte_bit_pack(&y, 1);
  }

  // Redundancy version – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_si || rnti_type == srslte_rnti_type_tc) {
    dci->rv = srslte_bit_pack(&y, 2);
  }

  // HARQ process number – 4 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->pid = srslte_bit_pack(&y, 4);
  }

  // System information indicator – 1 bit
  if (rnti_type == srslte_rnti_type_si) {
    dci->sii = srslte_bit_pack(&y, 1);
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->dai = srslte_bit_pack(&y, 2);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->tpc = srslte_bit_pack(&y, 2);
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->pucch_resource = srslte_bit_pack(&y, 3);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    dci->harq_feedback = srslte_bit_pack(&y, 3);
  }

  // Reserved bits ...
  if (rnti_type == srslte_rnti_type_p) {
    // ... – 6 bits
    dci->reserved = srslte_bit_pack(&y, 6);
  } else if (rnti_type == srslte_rnti_type_si) {
    // ... – 15 bits
    dci->reserved = srslte_bit_pack(&y, 15);
  } else if (rnti_type == srslte_rnti_type_ra) {
    // ... – 16 bits
    dci->reserved = srslte_bit_pack(&y, 16);
  }

  if (msg->nof_bits != y - msg->payload) {
    ERROR("Unpacked bits read (%d) do NOT match payload size (%d)", msg->nof_bits, (int)(y - msg->payload));
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_1_0_sizeof(const srslte_carrier_nr_t* carrier,
                                    const srslte_coreset_t*    coreset,
                                    srslte_rnti_type_t         rnti_type)
{
  int count = 0;

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 1;
  }

  if (rnti_type == srslte_rnti_type_p) {
    // Short Messages Indicator – 2 bits
    count += 2;

    // Short Messages – 8 bits
    count += 8;
  }

  // Frequency domain resource assignment
  int N = dci_nr_format_1_0_freq_resource_size(carrier, coreset, rnti_type);
  if (N < SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }
  count += N;

  // Time domain resource assignment – 4 bits
  count += 4;

  // VRB-to-PRB mapping – 1 bit
  count += 1;

  // Modulation and coding scheme – 5 bits
  count += 5;

  // TB scaling – 2 bits
  if (rnti_type == srslte_rnti_type_p || rnti_type == srslte_rnti_type_ra) {
    count += 2;
  }

  // New data indicator – 1 bit
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 1;
  }

  // Redundancy version – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_si || rnti_type == srslte_rnti_type_tc) {
    count += 2;
  }

  // HARQ process number – 4 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 4;
  }

  // System information indicator – 1 bit
  if (rnti_type == srslte_rnti_type_si) {
    count += 1;
  }

  // Downlink assignment index – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 2;
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 2;
  }

  // PUCCH resource indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 3;
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    count += 3;
  }

  // Reserved bits ...
  if (rnti_type == srslte_rnti_type_p) {
    // ... – 6 bits
    count += 2;
  } else if (rnti_type == srslte_rnti_type_si) {
    // ... – 15 bits
    count += 15;
  } else if (rnti_type == srslte_rnti_type_ra) {
    // ... – 16 bits
    count += 16;
  }

  return count;
}

static int dci_nr_format_1_0_to_str(const srslte_dci_dl_nr_t* dci, char* str, uint32_t str_len)
{
  uint32_t len = 0;

  // Print format
  len = srslte_print_check(
      str, str_len, len, "rnti=%04x L=%d cce=%d dci=1_0 ", dci->rnti, dci->location.L, dci->location.ncce);

  if (dci->rnti_type == srslte_rnti_type_p) {
    len = srslte_print_check(str, str_len, len, "smi=%d sm=%d ", dci->smi, dci->sm);
  }

  // Frequency domain resource assignment
  len = srslte_print_check(str, str_len, len, "f_alloc=0x%x ", dci->freq_domain_assigment);

  // Time domain resource assignment – 4 bits
  len = srslte_print_check(str, str_len, len, "t_alloc=0x%x ", dci->time_domain_assigment);

  // VRB-to-PRB mapping – 1 bit
  len = srslte_print_check(str, str_len, len, "vrb_to_prb_map=%d ", dci->vrb_to_prb_mapping);

  // Modulation and coding scheme – 5 bits
  len = srslte_print_check(str, str_len, len, "mcs=%d ", dci->mcs);

  // TB scaling – 2 bits
  if (dci->rnti_type == srslte_rnti_type_p || dci->rnti_type == srslte_rnti_type_ra) {
    len = srslte_print_check(str, str_len, len, "tb_scaling=%d ", dci->tb_scaling);
  }

  // New data indicator – 1 bit
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "ndi=%d ", dci->ndi);
  }

  // Redundancy version – 2 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_si ||
      dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "rv=%d ", dci->rv);
  }

  // HARQ process number – 4 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "harq_id=%d ", dci->harq_feedback);
  }

  // System information indicator – 1 bit
  if (dci->rnti_type == srslte_rnti_type_si) {
    len = srslte_print_check(str, str_len, len, "sii=%d ", dci->sii);
  }

  // Downlink assignment index – 2 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "sii=%d ", dci->sii);
  }

  // TPC command for scheduled PUCCH – 2 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "pucch_tpc=%d ", dci->tpc);
  }

  // PUCCH resource indicator – 3 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "pucch_res=%d ", dci->pucch_resource);
  }

  // PDSCH-to-HARQ_feedback timing indicator – 3 bits
  if (dci->rnti_type == srslte_rnti_type_c || dci->rnti_type == srslte_rnti_type_tc) {
    len = srslte_print_check(str, str_len, len, "harq_feedback=%d ", dci->harq_feedback);
  }

  // Reserved bits ...
  if (dci->rnti_type == srslte_rnti_type_p || dci->rnti_type == srslte_rnti_type_si ||
      dci->rnti_type == srslte_rnti_type_ra) {
    len = srslte_print_check(str, str_len, len, "reserved=0x%x ", dci->reserved);
  }

  return len;
}

int srslte_dci_ul_nr_to_str(const srslte_dci_ul_nr_t* dci, char* str, uint32_t str_len)
{
  // Pack DCI
  switch (dci->format) {
    case srslte_dci_format_nr_0_0:
      return dci_nr_format_0_0_to_str(dci, str, str_len);
    default:; // Do nothing
  }

  return srslte_print_check(str, str_len, 0, "unknown");
}

int srslte_dci_dl_nr_to_str(const srslte_dci_dl_nr_t* dci, char* str, uint32_t str_len)
{
  // Pack DCI
  switch (dci->format) {
    case srslte_dci_format_nr_1_0:
      return dci_nr_format_1_0_to_str(dci, str, str_len);
    default:; // Do nothing
  }

  return srslte_print_check(str, str_len, 0, "unknown");
}
