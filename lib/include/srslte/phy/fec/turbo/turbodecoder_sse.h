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

#ifndef SRSLTE_TURBODECODER_SSE_H
#define SRSLTE_TURBODECODER_SSE_H

#include "srslte/config.h"

typedef struct SRSLTE_API {
  uint32_t max_long_cb;
  int16_t* alpha;
  int16_t* branch;
} tdec_sse_t;

int  tdec_sse_init(void** h, uint32_t max_long_cb);
void tdec_sse_free(void* h);
void tdec_sse_dec(void* h, int16_t* input, int16_t* app, int16_t* parity, int16_t* output, uint32_t long_cb);
void tdec_sse_extract_input(int16_t* input,
                            int16_t* syst,
                            int16_t* parity0,
                            int16_t* parity1,
                            int16_t* app2,
                            uint32_t long_cb);
void tdec_sse_decision_byte(int16_t* app1, uint8_t* output, uint32_t long_cb);

#endif // SRSLTE_TURBODECODER_SSE_H
