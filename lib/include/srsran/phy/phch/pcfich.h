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

/******************************************************************************
 *  File:         pcfich.h
 *
 *  Description:  Physical control format indicator channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.7
 *****************************************************************************/

#ifndef SRSRAN_PCFICH_H
#define SRSRAN_PCFICH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/scrambling/scrambling.h"

#define PCFICH_CFI_LEN 32
#define PCFICH_RE PCFICH_CFI_LEN / 2

/* PCFICH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;
  int           nof_symbols;

  uint32_t nof_rx_antennas;

  /* handler to REGs resource mapper */
  srsran_regs_t* regs;

  /* buffers */
  cf_t ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS][PCFICH_RE];
  cf_t symbols[SRSRAN_MAX_PORTS][PCFICH_RE];
  cf_t x[SRSRAN_MAX_PORTS][PCFICH_RE];
  cf_t d[PCFICH_RE];

  // cfi table in floats
  float cfi_table_float[3][PCFICH_CFI_LEN];

  /* bit message */
  uint8_t data[PCFICH_CFI_LEN];

  /* received soft bits */
  float data_f[PCFICH_CFI_LEN];

  /* tx & rx objects */
  srsran_modem_table_t mod;
  srsran_sequence_t    seq[SRSRAN_NOF_SF_X_FRAME];

} srsran_pcfich_t;

SRSRAN_API int srsran_pcfich_init(srsran_pcfich_t* q, uint32_t nof_rx_antennas);

SRSRAN_API int srsran_pcfich_set_cell(srsran_pcfich_t* q, srsran_regs_t* regs, srsran_cell_t cell);

SRSRAN_API void srsran_pcfich_free(srsran_pcfich_t* q);

SRSRAN_API int srsran_pcfich_decode(srsran_pcfich_t*       q,
                                    srsran_dl_sf_cfg_t*    sf,
                                    srsran_chest_dl_res_t* channel,
                                    cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                                    float*                 corr_result);

SRSRAN_API int srsran_pcfich_encode(srsran_pcfich_t* q, srsran_dl_sf_cfg_t* sf, cf_t* sf_symbols[SRSRAN_MAX_PORTS]);

#endif // SRSRAN_PCFICH_H
