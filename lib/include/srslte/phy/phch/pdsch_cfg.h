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
 *  File:         pdsch_cfg.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.4
 *****************************************************************************/

#ifndef SRSLTE_PDSCH_CFG_H
#define SRSLTE_PDSCH_CFG_H

#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/phch/ra.h"

typedef struct SRSLTE_API {

  srslte_tx_scheme_t tx_scheme;
  uint32_t           pmi;
  bool               prb_idx[2][SRSLTE_MAX_PRB];
  uint32_t           nof_prb;
  uint32_t           nof_re;
  uint32_t           nof_symb_slot[2];
  srslte_ra_tb_t     tb[SRSLTE_MAX_CODEWORDS];
  int                last_tbs[SRSLTE_MAX_CODEWORDS];
  uint32_t           nof_tb;
  uint32_t           nof_layers;
} srslte_pdsch_grant_t;

typedef struct SRSLTE_API {

  srslte_pdsch_grant_t grant;

  uint16_t              rnti;
  uint32_t              max_nof_iterations;
  srslte_mimo_decoder_t decoder_type;
  float                 p_a;
  uint32_t              p_b;
  float                 rs_power;
  bool                  power_scale;
  bool                  csi_enable;
  bool                  use_tbs_index_alt;

  union {
    srslte_softbuffer_tx_t* tx[SRSLTE_MAX_CODEWORDS];
    srslte_softbuffer_rx_t* rx[SRSLTE_MAX_CODEWORDS];
  } softbuffers;

  bool     meas_evm_en;
  bool     meas_time_en;
  uint32_t meas_time_value;
} srslte_pdsch_cfg_t;

#endif // SRSLTE_PDSCH_CFG_H
