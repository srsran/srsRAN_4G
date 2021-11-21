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

void generate_default_nr_cell(rrc_cell_cfg_nr_t& cell);

int set_derived_nr_cell_params(bool is_sa, rrc_cell_cfg_nr_t& cell);

int check_nr_cell_cfg_valid(const rrc_cell_cfg_nr_t& cell, bool is_sa);

} // namespace srsenb

#endif // SRSRAN_RRC_NR_CONFIG_DEFAULT_H
