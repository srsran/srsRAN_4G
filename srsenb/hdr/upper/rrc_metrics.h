/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include "common_enb.h"

namespace srsenb {

typedef enum{
  RRC_STATE_IDLE = 0,
  RRC_STATE_WAIT_FOR_CON_SETUP_COMPLETE,
  RRC_STATE_WAIT_FOR_SECURITY_MODE_COMPLETE,
  RRC_STATE_WAIT_FOR_UE_CAP_INFO,
  RRC_STATE_WAIT_FOR_CON_RECONF_COMPLETE,
  RRC_STATE_REGISTERED,
  RRC_STATE_RELEASE_REQUEST,
  RRC_STATE_N_ITEMS,
}rrc_state_t;

struct rrc_ue_metrics_t
{
  rrc_state_t state;
};

struct rrc_metrics_t
{
  uint16_t n_ues;
  rrc_ue_metrics_t ues[ENB_METRICS_MAX_USERS];
};

} // namespace srsenb

#endif // SRSENB_RRC_METRICS_H
