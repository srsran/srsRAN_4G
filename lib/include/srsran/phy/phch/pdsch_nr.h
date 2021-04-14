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
 *  File:         pdsch_nr.h
 *
 *  Description:  Physical downlink shared channel for NR
 *
 *  Reference:    3GPP TS 38.211 V15.8.0 Sec. 7.3.1
 *****************************************************************************/

#ifndef SRSRAN_PDSCH_NR_H
#define SRSRAN_PDSCH_NR_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/dmrs_sch.h"
#include "srsran/phy/modem/evm.h"
#include "srsran/phy/modem/modem_table.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch_nr.h"
#include "srsran/phy/scrambling/scrambling.h"

/**
 * @brief PDSCH encoder and decoder initialization arguments
 */
typedef struct SRSRAN_API {
  srsran_sch_nr_args_t sch;
  bool                 measure_evm;
  bool                 measure_time;
  bool                 disable_zero_re_around_dc; ///< PDSCH NR sets the LLR around the DC to zero to avoid noise
  uint32_t             nof_zero_re_around_dc; ///< Number of RE to set to zero around DC. It uses default value if 0.
} srsran_pdsch_nr_args_t;

/**
 * @brief PDSCH NR object
 */
typedef struct SRSRAN_API {
  uint32_t             max_prb;                         ///< Maximum number of allocated prb
  uint32_t             max_layers;                      ///< Maximum number of allocated layers
  uint32_t             max_cw;                          ///< Maximum number of allocated code words
  srsran_carrier_nr_t  carrier;                         ///< NR carrier configuration
  srsran_sch_nr_t      sch;                             ///< SCH Encoder/Decoder Object
  uint8_t*             b[SRSRAN_MAX_CODEWORDS];         ///< SCH Encoded and scrambled data
  cf_t*                d[SRSRAN_MAX_CODEWORDS];         ///< PDSCH modulated bits
  cf_t*                x[SRSRAN_MAX_LAYERS_NR];         ///< PDSCH modulated bits
  srsran_modem_table_t modem_tables[SRSRAN_MOD_NITEMS]; ///< Modulator tables
  srsran_evm_buffer_t* evm_buffer;
  bool                 meas_time_en;
  uint32_t             meas_time_us;
  srsran_re_pattern_t  dmrs_re_pattern;
  uint32_t             nof_rvd_re;
  uint32_t nof_zero_re_around_dc; ///< Sets a number of RE surrounding the center of the resource grid to zero. Set to 0
                                  ///< for disabling.
} srsran_pdsch_nr_t;

/**
 * @brief Groups NR-PDSCH data for reception
 */
typedef struct {
  srsran_sch_tb_res_nr_t tb[SRSRAN_MAX_TB];         ///< SCH payload
  float                  evm[SRSRAN_MAX_CODEWORDS]; ///< EVM measurement if configured through arguments
} srsran_pdsch_res_nr_t;

SRSRAN_API int srsran_pdsch_nr_init_enb(srsran_pdsch_nr_t* q, const srsran_pdsch_nr_args_t* args);

SRSRAN_API int srsran_pdsch_nr_init_ue(srsran_pdsch_nr_t* q, const srsran_pdsch_nr_args_t* args);

SRSRAN_API void srsran_pdsch_nr_free(srsran_pdsch_nr_t* q);

SRSRAN_API int srsran_pdsch_nr_set_carrier(srsran_pdsch_nr_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_pdsch_nr_encode(srsran_pdsch_nr_t*           q,
                                      const srsran_sch_cfg_nr_t*   cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      uint8_t*                     data[SRSRAN_MAX_TB],
                                      cf_t*                        sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_pdsch_nr_decode(srsran_pdsch_nr_t*           q,
                                      const srsran_sch_cfg_nr_t*   cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      srsran_chest_dl_res_t*       channel,
                                      cf_t*                        sf_symbols[SRSRAN_MAX_PORTS],
                                      srsran_pdsch_res_nr_t*       res);

SRSRAN_API uint32_t srsran_pdsch_nr_rx_info(const srsran_pdsch_nr_t*     q,
                                            const srsran_sch_cfg_nr_t*   cfg,
                                            const srsran_sch_grant_nr_t* grant,
                                            const srsran_pdsch_res_nr_t* res,
                                            char*                        str,
                                            uint32_t                     str_len);

SRSRAN_API uint32_t srsran_pdsch_nr_tx_info(const srsran_pdsch_nr_t*     q,
                                            const srsran_sch_cfg_nr_t*   cfg,
                                            const srsran_sch_grant_nr_t* grant,
                                            char*                        str,
                                            uint32_t                     str_len);

#endif // SRSRAN_PDSCH_NR_H
