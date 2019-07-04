/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_RRC_ASN1_UTILS_H
#define SRSLTE_RRC_ASN1_UTILS_H

#include "srslte/interfaces/rrc_interface_types.h"

/************************
 * Forward declarations
 ***********************/
namespace asn1 {
namespace rrc {

struct plmn_id_s;
struct s_tmsi_s;
struct rlc_cfg_c;
struct srb_to_add_mod_s;

} // namespace rrc
} // namespace asn1

/************************
 *  Conversion Helpers
 ***********************/
namespace srslte {

plmn_id_t make_plmn_id_t(const asn1::rrc::plmn_id_s& asn1_type);
void      to_asn1(asn1::rrc::plmn_id_s* asn1_type, const plmn_id_t& cfg);

s_tmsi_t make_s_tmsi_t(const asn1::rrc::s_tmsi_s& asn1_type);
void     to_asn1(asn1::rrc::s_tmsi_s* asn1_type, const s_tmsi_t& cfg);

rlc_config_t make_rlc_config_t(const asn1::rrc::rlc_cfg_c& asn1_type);
rlc_config_t make_rlc_config_t(const asn1::rrc::srb_to_add_mod_s& asn1_type);
void         to_asn1(asn1::rrc::rlc_cfg_c* asn1_type, const rlc_config_t& cfg);
}

#endif // SRSLTE_RRC_ASN1_UTILS_H
