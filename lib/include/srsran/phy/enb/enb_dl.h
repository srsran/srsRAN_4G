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

/******************************************************************************
 *  File:         enb_dl.h
 *
 *  Description:  ENB downlink object.
 *
 *                This module is a frontend to all the downlink data and control
 *                channel processing modules for the ENB transmitter side.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_ENB_DL_H
#define SRSRAN_ENB_DL_H

#include <stdbool.h>

#include "srsran/phy/ch_estimation/refsignal_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pbch.h"
#include "srsran/phy/phch/pcfich.h"
#include "srsran/phy/phch/pdcch.h"
#include "srsran/phy/phch/pdsch.h"
#include "srsran/phy/phch/pdsch_cfg.h"
#include "srsran/phy/phch/phich.h"
#include "srsran/phy/phch/pmch.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/sync/pss.h"
#include "srsran/phy/sync/sss.h"

#include "srsran/phy/enb/enb_ul.h"
#include "srsran/phy/ue/ue_dl.h"

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/config.h"

typedef struct SRSRAN_API {
  srsran_cell_t cell;

  srsran_dl_sf_cfg_t dl_sf;

  srsran_cfr_cfg_t cfr_config;

  cf_t*         sf_symbols[SRSRAN_MAX_PORTS];
  cf_t*         out_buffer[SRSRAN_MAX_PORTS];
  srsran_ofdm_t ifft[SRSRAN_MAX_PORTS];
  srsran_ofdm_t ifft_mbsfn;

  srsran_pbch_t   pbch;
  srsran_pcfich_t pcfich;
  srsran_regs_t   regs;
  srsran_pdcch_t  pdcch;
  srsran_pdsch_t  pdsch;
  srsran_pmch_t   pmch;
  srsran_phich_t  phich;

  srsran_refsignal_t csr_signal;
  srsran_refsignal_t mbsfnr_signal;

  cf_t  pss_signal[SRSRAN_PSS_LEN];
  float sss_signal0[SRSRAN_SSS_LEN];
  float sss_signal5[SRSRAN_SSS_LEN];

  uint32_t              nof_common_locations[3];
  srsran_dci_location_t common_locations[3][SRSRAN_MAX_CANDIDATES_COM];

} srsran_enb_dl_t;

typedef struct {
  uint8_t  ack;
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;
} srsran_enb_dl_phich_t;

/* This function shall be called just after the initial synchronization */
SRSRAN_API int srsran_enb_dl_init(srsran_enb_dl_t* q, cf_t* out_buffer[SRSRAN_MAX_PORTS], uint32_t max_prb);

SRSRAN_API void srsran_enb_dl_free(srsran_enb_dl_t* q);

SRSRAN_API int srsran_enb_dl_set_cell(srsran_enb_dl_t* q, srsran_cell_t cell);

SRSRAN_API int srsran_enb_dl_set_cfr(srsran_enb_dl_t* q, const srsran_cfr_cfg_t* cfr);

SRSRAN_API bool srsran_enb_dl_location_is_common_ncce(srsran_enb_dl_t* q, const srsran_dci_location_t* loc);

SRSRAN_API void srsran_enb_dl_put_base(srsran_enb_dl_t* q, srsran_dl_sf_cfg_t* dl_sf);

SRSRAN_API void srsran_enb_dl_put_phich(srsran_enb_dl_t* q, srsran_phich_grant_t* grant, bool ack);

SRSRAN_API int srsran_enb_dl_put_pdcch_dl(srsran_enb_dl_t* q, srsran_dci_cfg_t* dci_cfg, srsran_dci_dl_t* dci_dl);

SRSRAN_API int srsran_enb_dl_put_pdcch_ul(srsran_enb_dl_t* q, srsran_dci_cfg_t* dci_cfg, srsran_dci_ul_t* dci_ul);

SRSRAN_API int
srsran_enb_dl_put_pdsch(srsran_enb_dl_t* q, srsran_pdsch_cfg_t* pdsch, uint8_t* data[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API int srsran_enb_dl_put_pmch(srsran_enb_dl_t* q, srsran_pmch_cfg_t* pmch_cfg, uint8_t* data);

SRSRAN_API void srsran_enb_dl_gen_signal(srsran_enb_dl_t* q);

SRSRAN_API bool srsran_enb_dl_gen_cqi_periodic(const srsran_cell_t*   cell,
                                               const srsran_dl_cfg_t* dl_cfg,
                                               uint32_t               tti,
                                               uint32_t               last_ri,
                                               srsran_cqi_cfg_t*      cqi_cfg);

SRSRAN_API bool srsran_enb_dl_gen_cqi_aperiodic(const srsran_cell_t*   cell,
                                                const srsran_dl_cfg_t* dl_cfg,
                                                uint32_t               ri,
                                                srsran_cqi_cfg_t*      cqi_cfg);

SRSRAN_API void srsran_enb_dl_save_signal(srsran_enb_dl_t* q);

/**
 * Generates the uplink control information configuration from the cell, subframe and HARQ ACK information. Note that
 * it expects the UCI configuration shall have been configured already with scheduling request and channel quality
 * information prior to this call.
 *
 * @param cell points to the physical layer cell parameters
 * @param sf points to the subframe configuration
 * @param ack_info is the HARQ-ACK information
 * @param uci_cfg the UCI configuration destination
 */
SRSRAN_API void srsran_enb_dl_gen_ack(const srsran_cell_t*      cell,
                                      const srsran_dl_sf_cfg_t* sf,
                                      const srsran_pdsch_ack_t* ack_info,
                                      srsran_uci_cfg_t*         uci_cfg);

/**
 * gets the HARQ-ACK values from the received Uplink Control Information configuration, the cell, and HARQ ACK
 * info itself. Note that it expects that the HARQ-ACK info has been set prior the UCI Data decoding.
 *
 * @param cell points to the physical layer cell parameters
 * @param uci_cfg points to the UCI configration
 * @param uci_value points to the received UCI values
 * @param ack_info is the HARQ-ACK information
 */
SRSRAN_API void srsran_enb_dl_get_ack(const srsran_cell_t*      cell,
                                      const srsran_uci_cfg_t*   uci_cfg,
                                      const srsran_uci_value_t* uci_value,
                                      srsran_pdsch_ack_t*       pdsch_ack);

/**
 * Gets the maximum signal power in decibels full scale. It is equivalent to the transmit power if all resource elements
 * were populated.
 * @return The maximum power
 */
SRSRAN_API float srsran_enb_dl_get_maximum_signal_power_dBfs(uint32_t nof_prb);

#endif // SRSRAN_ENB_DL_H
