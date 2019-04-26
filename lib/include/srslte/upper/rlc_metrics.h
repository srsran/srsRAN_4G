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

#ifndef SRSLTE_RLC_METRICS_H
#define SRSLTE_RLC_METRICS_H

#include "srslte/common/common.h"

namespace srslte {

struct rlc_metrics_t
{
  float dl_tput_mbps[SRSLTE_N_RADIO_BEARERS];
  float ul_tput_mbps[SRSLTE_N_RADIO_BEARERS];
  float dl_tput_mrb_mbps[SRSLTE_N_MCH_LCIDS];
};

} // namespace srslte

#endif // SRSLTE_RLC_METRICS_H
