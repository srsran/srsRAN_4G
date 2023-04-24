/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_PUSCH_NR_H
#define SRSRAN_PUSCH_NR_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/dmrs_sch.h"
#include "srsran/phy/modem/evm.h"
#include "srsran/phy/modem/modem_table.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch_nr.h"
#include "srsran/phy/phch/uci_nr.h"
#include "srsran/phy/scrambling/scrambling.h"

/**
 * @brief PUSCH encoder and decoder initialization arguments
 */
typedef struct SRSRAN_API {
  srsran_sch_nr_args_t sch;
  srsran_uci_nr_args_t uci;
  bool                 measure_evm;
  bool                 measure_time;
  uint32_t             max_layers;
  uint32_t             max_prb;
} srsran_pusch_nr_args_t;

/**
 * @brief PDSCH NR object
 */
typedef struct SRSRAN_API {
  uint32_t             max_prb;                         ///< Maximum number of allocated prb
  uint32_t             max_layers;                      ///< Maximum number of allocated layers
  uint32_t             max_cw;                          ///< Maximum number of allocated code words
  srsran_carrier_nr_t  carrier;                         ///< NR carrier configuration
  srsran_sch_nr_t      sch;                             ///< SCH Encoder/Decoder Object
  srsran_uci_nr_t      uci;                             ///< UCI Encoder/Decoder Object
  uint8_t*             b[SRSRAN_MAX_CODEWORDS];         ///< SCH Encoded and scrambled data
  cf_t*                d[SRSRAN_MAX_CODEWORDS];         ///< PDSCH modulated bits
  cf_t*                x[SRSRAN_MAX_LAYERS_NR];         ///< PDSCH modulated bits
  srsran_modem_table_t modem_tables[SRSRAN_MOD_NITEMS]; ///< Modulator tables
  srsran_evm_buffer_t* evm_buffer;
  bool                 meas_time_en;
  uint32_t             meas_time_us;
  srsran_re_pattern_t  dmrs_re_pattern;
  uint8_t*             g_ulsch;   ///< Temporal Encoded UL-SCH data
  uint8_t*             g_ack;     ///< Temporal Encoded HARQ-ACK bits
  uint8_t*             g_csi1;    ///< Temporal Encoded CSI part 1 bits
  uint8_t*             g_csi2;    ///< Temporal Encoded CSI part 2 bits
  uint32_t*            pos_ulsch; ///< Reserved resource elements for HARQ-ACK multiplexing position
  uint32_t*            pos_ack;   ///< Reserved resource elements for HARQ-ACK multiplexing position
  uint32_t*            pos_csi1;  ///< Reserved resource elements for CSI part 1 multiplexing position
  uint32_t*            pos_csi2;  ///< Reserved resource elements for CSI part 1 multiplexing position
  bool                 uci_mux;   ///< Set to true if PUSCH needs to multiplex UCI
  uint32_t             G_ack;     ///< Number of encoded HARQ-ACK bits
  uint32_t             G_csi1;    ///< Number of encoded CSI part 1 bits
  uint32_t             G_csi2;    ///< Number of encoded CSI part 2 bits
  uint32_t             G_ulsch;   ///< Number of encoded shared channel
} srsran_pusch_nr_t;

/**
 * @brief Groups NR-PUSCH data for transmission
 */
typedef struct {
  uint8_t*              payload[SRSRAN_MAX_TB]; ///< SCH payload
  srsran_uci_value_nr_t uci;                    ///< UCI payload
} srsran_pusch_data_nr_t;

/**
 * @brief Groups NR-PUSCH data for reception
 */
typedef struct {
  srsran_sch_tb_res_nr_t tb[SRSRAN_MAX_TB];         ///< SCH payload
  srsran_uci_value_nr_t  uci;                       ///< UCI payload
  float                  evm[SRSRAN_MAX_CODEWORDS]; ///< EVM measurement if configured through arguments
} srsran_pusch_res_nr_t;

SRSRAN_API int srsran_pusch_nr_init_gnb(srsran_pusch_nr_t* q, const srsran_pusch_nr_args_t* args);

SRSRAN_API int srsran_pusch_nr_init_ue(srsran_pusch_nr_t* q, const srsran_pusch_nr_args_t* args);

SRSRAN_API void srsran_pusch_nr_free(srsran_pusch_nr_t* q);

SRSRAN_API int srsran_pusch_nr_set_carrier(srsran_pusch_nr_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_pusch_nr_encode(srsran_pusch_nr_t*            q,
                                      const srsran_sch_cfg_nr_t*    cfg,
                                      const srsran_sch_grant_nr_t*  grant,
                                      const srsran_pusch_data_nr_t* data,
                                      cf_t*                         sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_pusch_nr_decode(srsran_pusch_nr_t*           q,
                                      const srsran_sch_cfg_nr_t*   cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      srsran_chest_dl_res_t*       channel,
                                      cf_t*                        sf_symbols[SRSRAN_MAX_PORTS],
                                      srsran_pusch_res_nr_t*       data);

SRSRAN_API uint32_t srsran_pusch_nr_rx_info(const srsran_pusch_nr_t*     q,
                                            const srsran_sch_cfg_nr_t*   cfg,
                                            const srsran_sch_grant_nr_t* grant,
                                            const srsran_pusch_res_nr_t* res,
                                            char*                        str,
                                            uint32_t                     str_len);

SRSRAN_API uint32_t srsran_pusch_nr_tx_info(const srsran_pusch_nr_t*     q,
                                            const srsran_sch_cfg_nr_t*   cfg,
                                            const srsran_sch_grant_nr_t* grant,
                                            const srsran_uci_value_nr_t* uci_value,
                                            char*                        str,
                                            uint32_t                     str_len);

#endif // SRSRAN_PUSCH_NR_H
