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

/**********************************************************************************************
 *  File:         tc_interl.h
 *
 *  Description:  Turbo code interleaver.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.2.3
 *********************************************************************************************/

#ifndef SRSLTE_TC_INTERL_H
#define SRSLTE_TC_INTERL_H

#include "srslte/config.h"
#include <stdint.h>

typedef struct SRSLTE_API {
  uint16_t* forward;
  uint16_t* reverse;
  uint32_t  max_long_cb;
} srslte_tc_interl_t;

SRSLTE_API int srslte_tc_interl_LTE_gen(srslte_tc_interl_t* h, uint32_t long_cb);

SRSLTE_API int srslte_tc_interl_LTE_gen_interl(srslte_tc_interl_t* h, uint32_t long_cb, uint32_t interl_win);

SRSLTE_API int srslte_tc_interl_init(srslte_tc_interl_t* h, uint32_t max_long_cb);

SRSLTE_API void srslte_tc_interl_free(srslte_tc_interl_t* h);

#endif // SRSLTE_TC_INTERL_H
