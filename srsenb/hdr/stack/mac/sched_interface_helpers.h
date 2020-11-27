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

#ifndef SRSLTE_SCHED_INTERFACE_HELPERS_H
#define SRSLTE_SCHED_INTERFACE_HELPERS_H

#include "srslte/common/logmap.h"
#include "srslte/interfaces/sched_interface.h"

namespace srsenb {

void log_dl_cc_results(srslte::log_ref log_h, uint32_t enb_cc_idx, const sched_interface::dl_sched_res_t& result);

}

#endif // SRSLTE_SCHED_INTERFACE_HELPERS_H
