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

#ifndef SRSLTE_BEARER_CFG_H
#define SRSLTE_BEARER_CFG_H

#include "rrc_cfg_utils.h"
#include "srslte/asn1/rrc_asn1.h"

namespace srslte {

//! Apply toAddModList/toRelease diffs to SRBs
void apply_srb_diff(asn1::rrc::srb_to_add_mod_list_l& src,
                    asn1::rrc::srb_to_add_mod_list_l& diff,
                    asn1::rrc::srb_to_add_mod_list_l& target)
{
  if (diff.size() > 0) {
    apply_addmodlist_diff(src, diff, target);
  } else if (&target != &src) {
    target = src;
  }
}

//! Apply toAddModList/toRelease diffs to SRBs
void apply_drb_diff(asn1::rrc::drb_to_add_mod_list_l& src,
                    asn1::rrc::rr_cfg_ded_s&          diff,
                    asn1::rrc::drb_to_add_mod_list_l& target)
{
  if (diff.drb_to_add_mod_list_present) {
    apply_addmodremlist_diff(src, diff.drb_to_add_mod_list, diff.drb_to_release_list, target);
  } else if (&target != &src) {
    target = src;
  }
}

} // namespace srslte

#endif // SRSLTE_BEARER_CFG_H
