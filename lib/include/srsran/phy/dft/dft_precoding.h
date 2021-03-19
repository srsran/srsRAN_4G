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
 *  File:         dft_precoding.h
 *
 *  Description:  DFT-based transform precoding object.
 *                Used in generation of uplink SCFDMA signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.3
 *********************************************************************************************/

#ifndef SRSRAN_DFT_PRECODING_H
#define SRSRAN_DFT_PRECODING_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"

/* DFT-based Transform Precoding object */
typedef struct SRSRAN_API {

  uint32_t          max_prb;
  srsran_dft_plan_t dft_plan[SRSRAN_MAX_PRB + 1];

} srsran_dft_precoding_t;

SRSRAN_API int srsran_dft_precoding_init(srsran_dft_precoding_t* q, uint32_t max_prb, bool is_tx);

SRSRAN_API int srsran_dft_precoding_init_tx(srsran_dft_precoding_t* q, uint32_t max_prb);

SRSRAN_API int srsran_dft_precoding_init_rx(srsran_dft_precoding_t* q, uint32_t max_prb);

SRSRAN_API void srsran_dft_precoding_free(srsran_dft_precoding_t* q);

SRSRAN_API bool srsran_dft_precoding_valid_prb(uint32_t nof_prb);

SRSRAN_API uint32_t srsran_dft_precoding_get_valid_prb(uint32_t nof_prb);

SRSRAN_API int
srsran_dft_precoding(srsran_dft_precoding_t* q, cf_t* input, cf_t* output, uint32_t nof_prb, uint32_t nof_symbols);

#endif // SRSRAN_DFT_PRECODING_H
