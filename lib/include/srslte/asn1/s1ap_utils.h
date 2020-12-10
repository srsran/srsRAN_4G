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

#ifndef SRSLTE_S1AP_UTILS_H
#define SRSLTE_S1AP_UTILS_H

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

template <class ies_set_paramT_>
struct protocol_ie_single_container_s;
using bearers_subject_to_status_transfer_list_l =
    dyn_array<protocol_ie_single_container_s<bearers_subject_to_status_transfer_item_ies_o> >;
using rrc_establishment_cause_e = enumerated<rrc_establishment_cause_opts, true, 3>;
using cause_radio_network_e     = enumerated<cause_radio_network_opts, true, 4>;

} // namespace s1ap
} // namespace asn1

#endif // SRSLTE_S1AP_UTILS_H
