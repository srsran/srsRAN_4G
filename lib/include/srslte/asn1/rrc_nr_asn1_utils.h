/*
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

#ifndef SRSLTE_RRC_NR_ASN1_UTILS_H
#define SRSLTE_RRC_NR_ASN1_UTILS_H

#include "srslte/interfaces/rrc_interface_types.h"
#include "srslte/interfaces/sched_interface.h"

/************************
 * Forward declarations
 ***********************/
namespace asn1 {
namespace rrc_nr {

struct plmn_id_s;
struct sib1_s;

} // namespace rrc_nr
} // namespace asn1

/************************
 *  Conversion Helpers
 ***********************/
namespace srslte {

plmn_id_t make_plmn_id_t(const asn1::rrc_nr::plmn_id_s& asn1_type);
void      to_asn1(asn1::rrc_nr::plmn_id_s* asn1_type, const plmn_id_t& cfg);

} // namespace srslte

namespace srsenb {

int set_sched_cell_cfg_sib1(srsenb::sched_interface::cell_cfg_t* sched_cfg, const asn1::rrc_nr::sib1_s& sib1);
}

#endif // SRSLTE_RRC_NR_ASN1_UTILS_H
