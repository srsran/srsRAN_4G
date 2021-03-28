/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
