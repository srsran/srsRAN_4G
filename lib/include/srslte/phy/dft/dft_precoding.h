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
 *  File:         dft_precoding.h
 *
 *  Description:  DFT-based transform precoding object.
 *                Used in generation of uplink SCFDMA signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.3
 *********************************************************************************************/

#ifndef SRSLTE_DFT_PRECODING_H
#define SRSLTE_DFT_PRECODING_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"

/* DFT-based Transform Precoding object */
typedef struct SRSLTE_API {

  uint32_t          max_prb;
  srslte_dft_plan_t dft_plan[SRSLTE_MAX_PRB + 1];

} srslte_dft_precoding_t;

SRSLTE_API int srslte_dft_precoding_init(srslte_dft_precoding_t* q, uint32_t max_prb, bool is_tx);

SRSLTE_API int srslte_dft_precoding_init_tx(srslte_dft_precoding_t* q, uint32_t max_prb);

SRSLTE_API int srslte_dft_precoding_init_rx(srslte_dft_precoding_t* q, uint32_t max_prb);

SRSLTE_API void srslte_dft_precoding_free(srslte_dft_precoding_t* q);

SRSLTE_API bool srslte_dft_precoding_valid_prb(uint32_t nof_prb);

SRSLTE_API uint32_t srslte_dft_precoding_get_valid_prb(uint32_t nof_prb);

SRSLTE_API int
srslte_dft_precoding(srslte_dft_precoding_t* q, cf_t* input, cf_t* output, uint32_t nof_prb, uint32_t nof_symbols);

#endif // SRSLTE_DFT_PRECODING_H
