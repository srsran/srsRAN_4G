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
 *  File:         pdsch_nr.h
 *
 *  Description:  Physical downlink shared channel for NR
 *
 *  Reference:    3GPP TS 38.211 V15.8.0 Sec. 7.3.1
 *****************************************************************************/

#ifndef srslte_pdsch_nr_H
#define srslte_pdsch_nr_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/dmrs_pdsch.h"
#include "srslte/phy/phch/pdsch_cfg_nr.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch_nr.h"
#include "srslte/phy/scrambling/scrambling.h"

/**
 * @brief PDSCH encoder and decoder initialization arguments
 */
typedef struct SRSLTE_API {
  srslte_sch_nr_args_t sch;
  bool                 measure_evm;
  bool                 measure_time;
} srslte_pdsch_args_t;

/**
 * @brief PDSCH NR object
 */
typedef struct SRSLTE_API {
  uint32_t             max_prb;                         ///< Maximum number of allocated prb
  uint32_t             max_layers;                      ///< Maximum number of allocated layers
  uint32_t             max_cw;                          ///< Maximum number of allocated code words
  srslte_carrier_nr_t  carrier;                         ///< NR carrier configuration
  srslte_sch_nr_t      sch;                             ///< SCH Encoder/Decoder Object
  uint8_t*             b[SRSLTE_MAX_CODEWORDS];         ///< SCH Encoded and scrambled data
  cf_t*                d[SRSLTE_MAX_CODEWORDS];         ///< PDSCH modulated bits
  cf_t*                x[SRSLTE_MAX_LAYERS_NR];         ///< PDSCH modulated bits
  srslte_modem_table_t modem_tables[SRSLTE_MOD_NITEMS]; ///< Modulator tables
  srslte_evm_buffer_t* evm_buffer;
  bool                 meas_time_en;
  uint32_t             meas_time_us;
} srslte_pdsch_nr_t;

/**
 *
 */
typedef struct {
  uint8_t* payload;
  bool     crc;
  float    evm;
} srslte_pdsch_res_nr_t;

SRSLTE_API int srslte_pdsch_nr_init_enb(srslte_pdsch_nr_t* q, const srslte_pdsch_args_t* args);

SRSLTE_API int srslte_pdsch_nr_init_ue(srslte_pdsch_nr_t* q, const srslte_pdsch_args_t* args);

SRSLTE_API void srslte_pdsch_nr_free(srslte_pdsch_nr_t* q);

SRSLTE_API int srslte_pdsch_nr_set_carrier(srslte_pdsch_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_pdsch_nr_encode(srslte_pdsch_nr_t*             q,
                                      const srslte_pdsch_cfg_nr_t*   cfg,
                                      const srslte_pdsch_grant_nr_t* grant,
                                      uint8_t*                       data[SRSLTE_MAX_TB],
                                      cf_t*                          sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API int srslte_pdsch_nr_decode(srslte_pdsch_nr_t*             q,
                                      const srslte_pdsch_cfg_nr_t*   cfg,
                                      const srslte_pdsch_grant_nr_t* grant,
                                      srslte_chest_dl_res_t*         channel,
                                      cf_t*                          sf_symbols[SRSLTE_MAX_PORTS],
                                      srslte_pdsch_res_nr_t          data[SRSLTE_MAX_TB]);

SRSLTE_API uint32_t srslte_pdsch_nr_rx_info(const srslte_pdsch_nr_t*       q,
                                            const srslte_pdsch_cfg_nr_t*   cfg,
                                            const srslte_pdsch_grant_nr_t* grant,
                                            const srslte_pdsch_res_nr_t*   res,
                                            char*                          str,
                                            uint32_t                       str_len);

SRSLTE_API uint32_t srslte_pdsch_nr_tx_info(const srslte_pdsch_nr_t*       q,
                                            const srslte_pdsch_cfg_nr_t*   cfg,
                                            const srslte_pdsch_grant_nr_t* grant,
                                            char*                          str,
                                            uint32_t                       str_len);

#endif // srslte_pdsch_nr_H
