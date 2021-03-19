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

/**********************************************************************************************
 *  File:         tc_interl.h
 *
 *  Description:  Turbo code interleaver.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.3.2.3
 *********************************************************************************************/

#ifndef SRSRAN_TC_INTERL_H
#define SRSRAN_TC_INTERL_H

#include "srsran/config.h"
#include <stdint.h>

typedef struct SRSRAN_API {
  uint16_t* forward;
  uint16_t* reverse;
  uint32_t  max_long_cb;
} srsran_tc_interl_t;

SRSRAN_API int srsran_tc_interl_LTE_gen(srsran_tc_interl_t* h, uint32_t long_cb);

SRSRAN_API int srsran_tc_interl_LTE_gen_interl(srsran_tc_interl_t* h, uint32_t long_cb, uint32_t interl_win);

SRSRAN_API int srsran_tc_interl_init(srsran_tc_interl_t* h, uint32_t max_long_cb);

SRSRAN_API void srsran_tc_interl_free(srsran_tc_interl_t* h);

#endif // SRSRAN_TC_INTERL_H
