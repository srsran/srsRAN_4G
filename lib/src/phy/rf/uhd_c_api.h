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
#ifndef SRSLTE_UHD_C_API_C_
#define SRSLTE_UHD_C_API_C_

#include "srslte/config.h"
#include "srslte/phy/rf/rf.h"
#include <uhd.h>

/* Declare functions not currently provided by the C-API */
SRSLTE_API void rf_uhd_register_msg_handler_c(void (*new_handler)(const char*));
SRSLTE_API void uhd_tx_metadata_set_time_spec(uhd_tx_metadata_handle* md, time_t secs, double frac_secs);
SRSLTE_API void uhd_tx_metadata_set_start(uhd_tx_metadata_handle* md, bool is_start_of_burst);
SRSLTE_API void uhd_tx_metadata_set_has_time_spec(uhd_tx_metadata_handle* md, bool has_time_spec);
SRSLTE_API void uhd_tx_metadata_set_end(uhd_tx_metadata_handle* md, bool is_end_of_burst);
SRSLTE_API void uhd_tx_metadata_add_time_spec(uhd_tx_metadata_handle* md, double frac_secs);

#endif /* SRSLTE_UHD_C_API_C_ */
