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

#ifndef SRSLTE_RLC_AM_BASE_H
#define SRSLTE_RLC_AM_BASE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc_common.h"
#include "srslte/upper/rlc_tx_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srslte {

///< Add rlc_am_base here

bool rlc_am_is_control_pdu(uint8_t* payload);
bool rlc_am_is_control_pdu(byte_buffer_t* pdu);

} // namespace srslte

#endif // SRSLTE_RLC_AM_BASE_H
