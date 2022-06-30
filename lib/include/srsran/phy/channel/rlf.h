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

#ifndef SRSRAN_RLF_H
#define SRSRAN_RLF_H

#include "srsran/config.h"
#include "srsran/phy/common/timestamp.h"
#include <stdint.h>

typedef struct {
  uint32_t t_on_ms;
  uint32_t t_off_ms;
} srsran_channel_rlf_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSRAN_API void srsran_channel_rlf_init(srsran_channel_rlf_t* q, uint32_t t_on_ms, uint32_t t_off_ms);

SRSRAN_API void srsran_channel_rlf_execute(srsran_channel_rlf_t*     q,
                                           const cf_t*               in,
                                           cf_t*                     out,
                                           uint32_t                  nsamples,
                                           const srsran_timestamp_t* ts);

SRSRAN_API void srsran_channel_rlf_free(srsran_channel_rlf_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSRAN_RLF_H
