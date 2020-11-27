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
    ERROR("Unpacked bits readed (%d) do NOT match payload size (%d)\n", msg->nof_bits, (int)(y - msg->payload));
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

  if (msg->nof_bits != srslte_dci_nr_format_1_0_sizeof(carrier, coreset, rnti_type)) {
    ERROR("Invalid number of bits %d, expected %d\n",
          msg->nof_bits,
          srslte_dci_nr_format_1_0_sizeof(carrier, coreset, rnti_type));
    return SRSLTE_ERROR;
  }

  // Identifier for DCI formats – 1 bits
  if (rnti_type == srslte_rnti_type_c || rnti_type == srslte_rnti_type_tc) {
    // The value of this bit field is always set to 1, indicating a DL DCI format
    if (*y != 1) {
      ERROR("Wrond DCI format\n");
      return SRSLTE_ERROR;
    }
    y++;
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
    ERROR("Unpacked bits readed (%d) do NOT match payload size (%d)\n", msg->nof_bits, (int)(y - msg->payload));
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_dci_nr_format_1_0_sizeof(const srslte_carrier_nr_t* carrier,
                                    const srslte_coreset_t*    coreset,
                                    srslte_rnti_type_t         rnti_type)
{
  uint32_t count = 0;

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