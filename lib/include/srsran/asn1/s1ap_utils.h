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

#ifndef SRSRAN_S1AP_UTILS_H
#define SRSRAN_S1AP_UTILS_H

#include "asn1_utils.h"

/************************
 * Forward declarations
 ***********************/

namespace asn1 {
namespace s1ap {

struct init_context_setup_request_s;
struct ue_context_mod_request_s;
struct erab_setup_request_s;
struct erab_release_cmd_s;
struct erab_modify_request_s;
struct ue_paging_id_c;
struct ho_request_s;
struct sourceenb_to_targetenb_transparent_container_s;
struct init_context_setup_resp_s;
struct erab_setup_resp_s;
struct rrc_establishment_cause_opts;
struct cause_radio_network_opts;
struct bearers_subject_to_status_transfer_item_ies_o;
struct erab_level_qos_params_s;
struct ho_cmd_s;
struct erab_admitted_item_s;
struct erab_to_be_modified_item_bearer_mod_req_s;
struct cause_c;
struct erab_item_s;
struct ue_aggregate_maximum_bitrate_s;

template <class ies_set_paramT_>
struct protocol_ie_single_container_s;
using bearers_subject_to_status_transfer_list_l =
    dyn_array<protocol_ie_single_container_s<bearers_subject_to_status_transfer_item_ies_o> >;
using rrc_establishment_cause_e = enumerated<rrc_establishment_cause_opts, true, 3>;
using cause_radio_network_e     = enumerated<cause_radio_network_opts, true, 4>;

/**************************
 *     S1AP Obj Id
 *************************/

template <typename T>
uint32_t get_obj_id(const T& obj);

template <typename T>
bool lower_obj_id(const T& lhs, const T& rhs)
{
  return get_obj_id(lhs) < get_obj_id(rhs);
}

template <typename T>
bool equal_obj_id(const T& lhs, const T& rhs)
{
  return get_obj_id(lhs) == get_obj_id(rhs);
}

} // namespace s1ap
} // namespace asn1

namespace srsenb {

using transp_addr_t = asn1::bounded_bitstring<1, 160, true, true>;

}

#endif // SRSRAN_S1AP_UTILS_H
