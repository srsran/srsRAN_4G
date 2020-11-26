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

#ifndef SRSLTE_RLC_AM_BASE_H
#define SRSLTE_RLC_AM_BASE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/byte_buffer_queue.h"
#include "srslte/upper/rlc_common.h"
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
