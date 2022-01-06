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
