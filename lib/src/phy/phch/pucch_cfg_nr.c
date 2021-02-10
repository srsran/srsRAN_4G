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

#include "srslte/phy/phch/pucch_cfg_nr.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/utils/debug.h"

static int pucch_nr_cfg_format0_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_0) {
    ERROR("Invalid format (%d)", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < SRSLTE_PUCCH_NR_FORMAT0_MIN_NSYMB ||
      resource->nof_symbols > SRSLTE_PUCCH_NR_FORMAT0_MAX_NSYMB) {
    ERROR("Invalid number of symbols (%d)", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > SRSLTE_PUCCH_NR_FORMAT0_MAX_CS) {
    ERROR("Invalid initial cyclic shift (%d)", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > SRSLTE_PUCCH_NR_FORMAT0_MAX_STARTSYMB) {
    ERROR("Invalid initial start symbol idx (%d)", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int pucch_nr_cfg_format1_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_1) {
    ERROR("Invalid format (%d)", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < SRSLTE_PUCCH_NR_FORMAT1_MIN_NSYMB ||
      resource->nof_symbols > SRSLTE_PUCCH_NR_FORMAT1_MAX_NSYMB) {
    ERROR("Invalid number of symbols (%d)", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > SRSLTE_PUCCH_NR_FORMAT1_MAX_CS) {
    ERROR("Invalid initial cyclic shift (%d)", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > SRSLTE_PUCCH_NR_FORMAT1_MAX_STARTSYMB) {
    ERROR("Invalid initial start symbol idx (%d)", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  if (resource->time_domain_occ > SRSLTE_PUCCH_NR_FORMAT1_MAX_TOCC) {
    ERROR("Invalid time domain occ (%d)", resource->time_domain_occ);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int pucch_nr_cfg_format2_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_2) {
    ERROR("Invalid format (%d)", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < SRSLTE_PUCCH_NR_FORMAT2_MIN_NSYMB ||
      resource->nof_symbols > SRSLTE_PUCCH_NR_FORMAT2_MAX_NSYMB) {
    ERROR("Invalid number of symbols (%d)", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->nof_prb < SRSLTE_PUCCH_NR_FORMAT2_MIN_NPRB || resource->nof_prb > SRSLTE_PUCCH_NR_FORMAT2_MAX_NPRB) {
    ERROR("Invalid number of prb (%d)", resource->nof_prb);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > SRSLTE_PUCCH_NR_FORMAT2_MAX_STARTSYMB) {
    ERROR("Invalid initial start symbol idx (%d)", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int pucch_nr_cfg_format3_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_3) {
    ERROR("Invalid format (%d)", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < SRSLTE_PUCCH_NR_FORMAT3_MIN_NSYMB ||
      resource->nof_symbols > SRSLTE_PUCCH_NR_FORMAT3_MAX_NSYMB) {
    ERROR("Invalid number of symbols (%d)", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->nof_prb < SRSLTE_PUCCH_NR_FORMAT3_MIN_NPRB || resource->nof_prb > SRSLTE_PUCCH_NR_FORMAT3_MAX_NPRB) {
    ERROR("Invalid number of prb (%d)", resource->nof_prb);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > SRSLTE_PUCCH_NR_FORMAT3_MAX_STARTSYMB) {
    ERROR("Invalid initial start symbol idx (%d)", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int pucch_nr_cfg_format4_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_4) {
    ERROR("Invalid format (%d)", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < SRSLTE_PUCCH_NR_FORMAT4_MIN_NSYMB ||
      resource->nof_symbols > SRSLTE_PUCCH_NR_FORMAT4_MAX_NSYMB) {
    ERROR("Invalid number of symbols (%d)", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > SRSLTE_PUCCH_NR_FORMAT4_MAX_STARTSYMB) {
    ERROR("Invalid initial start symbol idx (%d)", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  if (resource->occ_lenth != 2 && resource->occ_lenth != 4) {
    ERROR("Invalid OCC length (%d)", resource->occ_lenth);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pucch_nr_cfg_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  // Check pointer
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->starting_prb > SRSLTE_MAX_NRE_NR - 1) {
    return SRSLTE_ERROR;
  }

  if (resource->intra_slot_hopping) {
    ERROR("Intra-slot hopping is not implemented");
    return SRSLTE_ERROR;
  }

  if (resource->second_hop_prb > SRSLTE_MAX_NRE_NR - 1) {
    return SRSLTE_ERROR;
  }

  if (resource->max_code_rate > SRSLTE_PUCCH_NR_MAX_CODE_RATE) {
    ERROR("Invalid maximum code rate (%d)", resource->max_code_rate);
    return SRSLTE_ERROR;
  }

  switch (resource->format) {
    case SRSLTE_PUCCH_NR_FORMAT_0:
      return pucch_nr_cfg_format0_resource_valid(resource);
    case SRSLTE_PUCCH_NR_FORMAT_1:
      return pucch_nr_cfg_format1_resource_valid(resource);
    case SRSLTE_PUCCH_NR_FORMAT_2:
      return pucch_nr_cfg_format2_resource_valid(resource);
    case SRSLTE_PUCCH_NR_FORMAT_3:
      return pucch_nr_cfg_format3_resource_valid(resource);
    case SRSLTE_PUCCH_NR_FORMAT_4:
      return pucch_nr_cfg_format4_resource_valid(resource);
    case SRSLTE_PUCCH_NR_FORMAT_ERROR:
    default:
      ERROR("Invalid case");
      break;
  }

  return SRSLTE_ERROR;
}
