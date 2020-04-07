/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#ifndef SRSLTE_ENB_DL_H
#define SRSLTE_ENB_DL_H

#include <stdbool.h>

#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pbch.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/phch/pdcch.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/phich.h"
#include "srslte/phy/phch/pmch.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/sync/pss.h"
#include "srslte/phy/sync/sss.h"

#include "srslte/phy/enb/enb_ul.h"
#include "srslte/phy/ue/ue_dl.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/config.h"

typedef struct SRSLTE_API {
  srslte_cell_t cell;

  srslte_dl_sf_cfg_t dl_sf;

  cf_t* sf_symbols[SRSLTE_MAX_PORTS];

  srslte_ofdm_t ifft[SRSLTE_MAX_PORTS];
  srslte_ofdm_t ifft_mbsfn;

  srslte_pbch_t   pbch;
  srslte_pcfich_t pcfich;
  srslte_regs_t   regs;
  srslte_pdcch_t  pdcch;
  srslte_pdsch_t  pdsch;
  srslte_pmch_t   pmch;
  srslte_phich_t  phich;

  srslte_refsignal_t csr_signal;
  srslte_refsignal_t mbsfnr_signal;

  cf_t  pss_signal[SRSLTE_PSS_LEN];
  float sss_signal0[SRSLTE_SSS_LEN];
  float sss_signal5[SRSLTE_SSS_LEN];

} srslte_enb_dl_t;

typedef struct {
  uint8_t  ack;
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;
} srslte_enb_dl_phich_t;

/* This function shall be called just after the initial synchronization */
SRSLTE_API int srslte_enb_dl_init(srslte_enb_dl_t* q, cf_t* out_buffer[SRSLTE_MAX_PORTS], uint32_t max_prb);

SRSLTE_API void srslte_enb_dl_free(srslte_enb_dl_t* q);

SRSLTE_API int srslte_enb_dl_set_cell(srslte_enb_dl_t* q, srslte_cell_t cell);

SRSLTE_API int srslte_enb_dl_add_rnti(srslte_enb_dl_t* q, uint16_t rnti);

SRSLTE_API void srslte_enb_dl_rem_rnti(srslte_enb_dl_t* q, uint16_t rnti);

SRSLTE_API void srslte_enb_dl_put_base(srslte_enb_dl_t* q, srslte_dl_sf_cfg_t* dl_sf);

SRSLTE_API void srslte_enb_dl_put_phich(srslte_enb_dl_t* q, srslte_phich_grant_t* grant, bool ack);

SRSLTE_API int srslte_enb_dl_put_pdcch_dl(srslte_enb_dl_t* q, srslte_dci_cfg_t* dci_cfg, srslte_dci_dl_t* dci_dl);

SRSLTE_API int srslte_enb_dl_put_pdcch_ul(srslte_enb_dl_t* q, srslte_dci_cfg_t* dci_cfg, srslte_dci_ul_t* dci_ul);

SRSLTE_API int
srslte_enb_dl_put_pdsch(srslte_enb_dl_t* q, srslte_pdsch_cfg_t* pdsch, uint8_t* data[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_enb_dl_put_pmch(srslte_enb_dl_t* q, srslte_pmch_cfg_t* pmch_cfg, uint8_t* data);

SRSLTE_API void srslte_enb_dl_gen_signal(srslte_enb_dl_t* q);

SRSLTE_API bool srslte_enb_dl_gen_cqi_periodic(const srslte_cell_t*   cell,
                                               const srslte_dl_cfg_t* dl_cfg,
                                               uint32_t               tti,
                                               uint32_t               last_ri,
                                               srslte_cqi_cfg_t*      cqi_cfg);

SRSLTE_API bool srslte_enb_dl_gen_cqi_aperiodic(const srslte_cell_t*   cell,
                                                const srslte_dl_cfg_t* dl_cfg,
                                                uint32_t               ri,
                                                srslte_cqi_cfg_t*      cqi_cfg);

SRSLTE_API void srslte_enb_dl_save_signal(srslte_enb_dl_t* q);

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
SRSLTE_API void srslte_enb_dl_gen_ack(const srslte_cell_t*      cell,
                                      const srslte_dl_sf_cfg_t* sf,
                                      const srslte_pdsch_ack_t* ack_info,
                                      srslte_uci_cfg_t*         uci_cfg);

/**
 * gets the HARQ-ACK values from the received Uplink Control Information configuration, the cell, and HARQ ACK
 * info itself. Note that it expects that the HARQ-ACK info has been set prior the UCI Data decoding.
 *
 * @param cell points to the physical layer cell parameters
 * @param uci_cfg points to the UCI configration
 * @param uci_value points to the received UCI values
 * @param ack_info is the HARQ-ACK information
 */
SRSLTE_API void srslte_enb_dl_get_ack(const srslte_cell_t*      cell,
                                      const srslte_uci_cfg_t*   uci_cfg,
                                      const srslte_uci_value_t* uci_value,
                                      srslte_pdsch_ack_t*       pdsch_ack);

#endif // SRSLTE_ENB_DL_H
