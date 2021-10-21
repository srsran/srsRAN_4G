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

#ifndef SRSRAN_CELL_ASN1_CONFIG_H
#define SRSRAN_CELL_ASN1_CONFIG_H

#include "rrc_config_nr.h"
#include "srsran/asn1/rrc_nr.h"

namespace srsenb {

int fill_sp_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::sp_cell_cfg_s& sp_cell);

}

#endif // SRSRAN_CELL_ASN1_CONFIG_H
