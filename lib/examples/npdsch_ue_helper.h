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

#ifndef SRSLTE_NPDSCH_UE_HELPER_H
#define SRSLTE_NPDSCH_UE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/phy/phch/ra_nbiot.h"
#include <stdint.h>

int get_sib2_params(const uint8_t* sib1_payload, const uint32_t len, srslte_nbiot_si_params_t* sib2_params);
int bcch_bch_to_pretty_string(const uint8_t* bcch_bch_payload,
                              const uint32_t input_len,
                              char*          output,
                              const uint32_t max_output_len);
int bcch_dl_sch_to_pretty_string(const uint8_t* bcch_dl_sch_payload,
                                 const uint32_t input_len,
                                 char*          output,
                                 const uint32_t max_output_len);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_NPDSCH_UE_HELPER_H
