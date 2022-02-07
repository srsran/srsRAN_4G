/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RRC_NR_CONFIG_DEFAULT_H
#define SRSRAN_RRC_NR_CONFIG_DEFAULT_H

#include "rrc_nr_config.h"

namespace srsenb {

// Helper methods
uint32_t coreset_get_bw(const asn1::rrc_nr::ctrl_res_set_s& coreset);
int      coreset_get_pdcch_nr_max_candidates(const asn1::rrc_nr::ctrl_res_set_s& coreset, uint32_t aggregation_level);

void generate_default_nr_cell(rrc_cell_cfg_nr_t& cell);

int set_derived_nr_cell_params(bool is_sa, rrc_cell_cfg_nr_t& cell);
int set_derived_nr_rrc_params(rrc_nr_cfg_t& rrc_cfg);

// Tests to ensure validity of config

int check_nr_pdcch_cfg_valid(const srsran_pdcch_cfg_nr_t& pdcch);

} // namespace srsenb

#endif // SRSRAN_RRC_NR_CONFIG_DEFAULT_H
