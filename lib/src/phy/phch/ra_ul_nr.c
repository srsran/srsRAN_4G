/**
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

#include "srslte/phy/phch/ra_ul_nr.h"
#include "srslte/phy/ch_estimation/dmrs_pucch.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/debug.h"

#define RA_UL_PUCCH_CODE_RATE_N 8
#define RA_UL_PUCCH_CODE_RATE_RESERVED NAN

static const double ra_ul_pucch_code_rate_table[RA_UL_PUCCH_CODE_RATE_N] =
    {0.08, 0.15, 0.25, 0.35, 0.45, 0.60, 0.80, RA_UL_PUCCH_CODE_RATE_RESERVED};

// Implements TS 38.213 Table 9.2.5.2-1: Code rate r corresponding to value of maxCodeRate
static double ra_ul_nr_pucch_code_rate_r(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->max_code_rate >= RA_UL_PUCCH_CODE_RATE_RESERVED) {
    ERROR("Invalid code rate\n");
    return RA_UL_PUCCH_CODE_RATE_RESERVED;
  }

  return ra_ul_pucch_code_rate_table[resource->max_code_rate];
}

// Calculate number of PRBs for PUCCH format 2, or PUCCH format 3, or PUCCH format 4, respectively
// static int ra_ul_nr_pucch_Mrb(const srslte_pucch_nr_resource_t* resource)
//{
//  switch (resource->format) {
//    case SRSLTE_PUCCH_NR_FORMAT_2:
//    case SRSLTE_PUCCH_NR_FORMAT_3:
//      return resource->nof_prb;
//    case SRSLTE_PUCCH_NR_FORMAT_4:
//      return SRSLTE_PUCCH_NR_FORMAT4_NPRB;
//    default:
//      ERROR("Invalid case\n");
//      break;
//  }
//  return SRSLTE_ERROR;
//}

// Calculate number of subcarriers per resource block for payload (No DMRS)
static int ra_ul_nr_pucch_nre(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return SRSLTE_NRE - 4;
    case SRSLTE_PUCCH_NR_FORMAT_3:
      return SRSLTE_NRE;
    case SRSLTE_PUCCH_NR_FORMAT_4:
      return SRSLTE_NRE / resource->occ_lenth;
    default:
      ERROR("Invalid case\n");
      break;
  }
  return SRSLTE_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_nsymb(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return resource->nof_symbols;
    case SRSLTE_PUCCH_NR_FORMAT_3:
    case SRSLTE_PUCCH_NR_FORMAT_4: {
      uint32_t idx[SRSLTE_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB] = {};

      // Get number of DMRS symbols for format 3 or 4
      int nsymb_dmrs = srslte_dmrs_pucch_format_3_4_get_symbol_idx(resource, idx);
      if (nsymb_dmrs < SRSLTE_SUCCESS) {
        return SRSLTE_ERROR;
      }

      return (int)resource->nof_symbols - nsymb_dmrs;
    }
    default:
      ERROR("Invalid case\n");
      break;
  }
  return SRSLTE_ERROR;
}

// Calculate number of PUCCH symbols excluding the ones used exclusively for DMRS for formats 3 and 4
static int ra_ul_nr_pucch_qm(const srslte_pucch_nr_resource_t* resource)
{
  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return 2;
    case SRSLTE_PUCCH_NR_FORMAT_3:
    case SRSLTE_PUCCH_NR_FORMAT_4:
      return resource->enable_pi_bpsk ? 1 : 2;
    default:
      ERROR("Invalid case\n");
      break;
  }
  return SRSLTE_ERROR;
}

int srslte_ra_ul_nr_pucch_format_2_3_min_prb(const srslte_pucch_nr_resource_t* resource,
                                             const srslte_uci_cfg_nr_t*        uci_cfg)
{
  if (resource == NULL || uci_cfg == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Get maximum allowed code rate
  double r = ra_ul_nr_pucch_code_rate_r(resource);
  if (!isnormal(r)) {
    ERROR("Invalid coderate %f\n", r);
    return SRSLTE_ERROR;
  }

  // Get number of RE/PRB
  int nre = ra_ul_nr_pucch_nre(resource);
  if (nre < SRSLTE_SUCCESS) {
    ERROR("Getting nre\n");
    return SRSLTE_ERROR;
  }

  // Get number of symbols
  int nsymb = ra_ul_nr_pucch_nsymb(resource);
  if (nsymb < SRSLTE_SUCCESS) {
    ERROR("Getting nsymb\n");
    return SRSLTE_ERROR;
  }

  // Get modulation order
  int qm = ra_ul_nr_pucch_qm(resource);
  if (qm < SRSLTE_SUCCESS) {
    ERROR("Getting qm\n");
    return SRSLTE_ERROR;
  }

  // Calculate denominator
  double nof_bits_rb = r * nre * nsymb * qm;
  if (!isnormal(nof_bits_rb)) {
    return SRSLTE_ERROR;
  }

  // Compute total number of UCI bits
  uint32_t O_total = uci_cfg->o_ack + uci_cfg->o_sr + uci_cfg->o_csi1 + uci_cfg->o_csi2;

  // Add CRC bits if any
  O_total += srslte_uci_nr_crc_len(O_total);

  // Return the minimum
  return (int)ceil(O_total / nof_bits_rb);
}