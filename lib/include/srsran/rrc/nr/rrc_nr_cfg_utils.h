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

#ifndef SRSRAN_RRC_NR_CFG_UTILS_H
#define SRSRAN_RRC_NR_CFG_UTILS_H

#include "srsran/asn1/rrc_nr.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"

namespace srsran {

struct basic_cell_args_t {
  uint32_t    cell_id       = 0x19B01;
  uint32_t    nof_prbs      = 52;
  uint32_t    scs           = 15;
  bool        is_standalone = true;
  bool        is_fdd        = true;
  std::string plmn          = "90170";
  uint32_t    tac           = 7;
};

void generate_default_pdcch_cfg_common(asn1::rrc_nr::pdcch_cfg_common_s& cfg, const basic_cell_args_t& args = {});
void generate_default_init_dl_bwp(asn1::rrc_nr::bwp_dl_common_s& cfg, const basic_cell_args_t& args = {});
void generate_default_dl_cfg_common(asn1::rrc_nr::dl_cfg_common_s& cfg, const basic_cell_args_t& args = {});

void generate_default_mib(const basic_cell_args_t& args, asn1::rrc_nr::mib_s& cfg);

void generate_default_serv_cell_cfg_common_sib(const basic_cell_args_t&                     args,
                                               asn1::rrc_nr::serving_cell_cfg_common_sib_s& cfg);
void generate_default_sib1(const basic_cell_args_t& args, asn1::rrc_nr::sib1_s& cfg);

} // namespace srsran

#endif // SRSRAN_RRC_NR_CFG_UTILS_H
