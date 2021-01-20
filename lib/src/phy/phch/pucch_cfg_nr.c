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
#include "srslte/phy/utils/debug.h"

int srslte_pucch_nr_format0_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_0) {
    ERROR("Invalid format (%d)\n", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols != 1 && resource->nof_symbols != 2) {
    ERROR("Invalid number of symbols (%d)\n", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > 11) {
    ERROR("Invalid initial cyclic shift (%d)\n", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > 13) {
    ERROR("Invalid initial start symbol idx (%d)\n", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pucch_nr_format1_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_1) {
    ERROR("Invalid format (%d)\n", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < 4 || resource->nof_symbols > 14) {
    ERROR("Invalid number of symbols (%d)\n", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > 11) {
    ERROR("Invalid initial cyclic shift (%d)\n", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > 10) {
    ERROR("Invalid initial start symbol idx (%d)\n", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  if (resource->time_domain_occ > 6) {
    ERROR("Invalid time domain occ (%d)\n", resource->time_domain_occ);
    return SRSLTE_ERROR;
  }

  if (resource->intra_slot_hopping) {
    ERROR("Intra-slot hopping is not implemented\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pucch_nr_format2_resource_valid(const srslte_pucch_nr_resource_t* resource)
{
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->format != SRSLTE_PUCCH_NR_FORMAT_2) {
    ERROR("Invalid format (%d)\n", resource->format);
    return SRSLTE_ERROR;
  }

  if (resource->nof_symbols < 1 || resource->nof_symbols > 2) {
    ERROR("Invalid number of symbols (%d)\n", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->nof_prb < 1 || resource->nof_prb > 16) {
    ERROR("Invalid number of prb (%d)\n", resource->nof_prb);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > 13) {
    ERROR("Invalid initial start symbol idx (%d)\n", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}