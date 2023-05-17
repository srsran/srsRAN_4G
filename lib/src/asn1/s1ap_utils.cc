/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsran/asn1/s1ap_utils.h"
#include "srsran/asn1/s1ap.h"

namespace asn1 {
namespace s1ap {

template <>
uint32_t get_obj_id<erab_item_s>(const erab_item_s& obj)
{
  return obj.erab_id;
}

template <>
uint32_t get_obj_id<protocol_ie_single_container_s<erab_to_be_setup_item_ctxt_su_req_ies_o> >(
    const protocol_ie_single_container_s<erab_to_be_setup_item_ctxt_su_req_ies_o>& obj)
{
  return obj->erab_to_be_setup_item_ctxt_su_req().erab_id;
}

template <>
uint32_t get_obj_id<protocol_ie_single_container_s<erab_to_be_setup_item_bearer_su_req_ies_o> >(
    const protocol_ie_single_container_s<erab_to_be_setup_item_bearer_su_req_ies_o>& obj)
{
  return obj->erab_to_be_setup_item_bearer_su_req().erab_id;
}

template <>
uint32_t get_obj_id<protocol_ie_single_container_s<erab_to_be_modified_item_bearer_mod_req_ies_o> >(
    const protocol_ie_single_container_s<erab_to_be_modified_item_bearer_mod_req_ies_o>& obj)
{
  return obj->erab_to_be_modified_item_bearer_mod_req().erab_id;
}

} // namespace s1ap
} // namespace asn1
