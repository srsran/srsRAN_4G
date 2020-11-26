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

#ifndef SRSLTE_RRC_NR_UTILS_H
#define SRSLTE_RRC_NR_UTILS_H

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

#endif // SRSLTE_RRC_NR_UTILS_H
