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

#ifndef SRSLTE_RLF_H
#define SRSLTE_RLF_H

#include <srslte/srslte.h>

typedef struct {
  uint32_t t_on_ms;
  uint32_t t_off_ms;
} srslte_channel_rlf_t;

#ifdef __cplusplus
extern "C" {
#endif

SRSLTE_API void srslte_channel_rlf_init(srslte_channel_rlf_t* q, uint32_t t_on_ms, uint32_t t_off_ms);

SRSLTE_API void srslte_channel_rlf_execute(srslte_channel_rlf_t*     q,
                                           const cf_t*               in,
                                           cf_t*                     out,
                                           uint32_t                  nsamples,
                                           const srslte_timestamp_t* ts);

SRSLTE_API void srslte_channel_rlf_free(srslte_channel_rlf_t* q);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_RLF_H
