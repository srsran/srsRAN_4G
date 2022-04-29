/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSENB_RRC_METRICS_H
#define SRSENB_RRC_METRICS_H

#include "srsenb/hdr/common/common_enb.h"
#include <vector>

namespace srsenb {

typedef enum {
  RRC_STATE_IDLE = 0,
  RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE,
  RRC_STATE_WAIT_FOR_CON_REEST_COMPLETE,
  RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO_ENDC, /* only entered for UEs with NSA support */
  RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE,
  RRC_STATE_REESTABLISHMENT_COMPLETE,
  RRC_STATE_REGISTERED,
  RRC_STATE_RELEASE_REQUEST,
  RRC_STATE_N_ITEMS,
} rrc_state_t;

struct rrc_ue_metrics_t {
  rrc_state_t                                 state;
  std::vector<std::pair<uint32_t, uint32_t> > drb_qci_map;
};

struct rrc_metrics_t {
  std::vector<rrc_ue_metrics_t> ues;
};

} // namespace srsenb

#endif // SRSENB_RRC_METRICS_H
