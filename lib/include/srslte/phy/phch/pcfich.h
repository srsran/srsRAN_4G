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

/******************************************************************************
 *  File:         pcfich.h
 *
 *  Description:  Physical control format indicator channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.7
 *****************************************************************************/

#ifndef SRSLTE_PCFICH_H
#define SRSLTE_PCFICH_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/scrambling/scrambling.h"

#define PCFICH_CFI_LEN 32
#define PCFICH_RE PCFICH_CFI_LEN / 2

/* PCFICH object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  int           nof_symbols;

  uint32_t nof_rx_antennas;

  /* handler to REGs resource mapper */
  srslte_regs_t* regs;

  /* buffers */
  cf_t ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS][PCFICH_RE];
  cf_t symbols[SRSLTE_MAX_PORTS][PCFICH_RE];
  cf_t x[SRSLTE_MAX_PORTS][PCFICH_RE];
  cf_t d[PCFICH_RE];

  // cfi table in floats
  float cfi_table_float[3][PCFICH_CFI_LEN];

  /* bit message */
  uint8_t data[PCFICH_CFI_LEN];

  /* received soft bits */
  float data_f[PCFICH_CFI_LEN];

  /* tx & rx objects */
  srslte_modem_table_t mod;
  srslte_sequence_t    seq[SRSLTE_NOF_SF_X_FRAME];

} srslte_pcfich_t;

SRSLTE_API int srslte_pcfich_init(srslte_pcfich_t* q, uint32_t nof_rx_antennas);

SRSLTE_API int srslte_pcfich_set_cell(srslte_pcfich_t* q, srslte_regs_t* regs, srslte_cell_t cell);

SRSLTE_API void srslte_pcfich_free(srslte_pcfich_t* q);

SRSLTE_API int srslte_pcfich_decode(srslte_pcfich_t*       q,
                                    srslte_dl_sf_cfg_t*    sf,
                                    srslte_chest_dl_res_t* channel,
                                    cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                                    float*                 corr_result);

SRSLTE_API int srslte_pcfich_encode(srslte_pcfich_t* q, srslte_dl_sf_cfg_t* sf, cf_t* sf_symbols[SRSLTE_MAX_PORTS]);

#endif // SRSLTE_PCFICH_H
