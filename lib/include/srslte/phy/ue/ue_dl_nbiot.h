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

#ifndef SRSLTE_UE_DL_NBIOT_H
#define SRSLTE_UE_DL_NBIOT_H

#include <stdbool.h>

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/ofdm.h"

#include "srslte/phy/phch/dci_nbiot.h"
#include "srslte/phy/phch/npdcch.h"
#include "srslte/phy/phch/npdsch.h"
#include "srslte/phy/phch/npdsch_cfg.h"
#include "srslte/phy/phch/ra_nbiot.h"

#include "srslte/phy/sync/cfo.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/config.h"

#define SRSLTE_NBIOT_EXPECT_MORE_SF -3  // returned when expecting more subframes for successful decoding
#define SRSLTE_NBIOT_UE_DL_FOUND_DCI -4 // returned when DCI for given RNTI was found
#define SRSLTE_NBIOT_UE_DL_SKIP_SF -5

/*
 * @brief Narrowband UE downlink object.
 *
 * This module is a frontend to all the downlink data and control
 * channel processing modules.
 */
typedef struct SRSLTE_API {
  srslte_npdcch_t         npdcch;
  srslte_npdsch_t         npdsch;
  srslte_ofdm_t           fft;
  srslte_chest_dl_nbiot_t chest;

  srslte_cfo_t sfo_correct;

  srslte_softbuffer_rx_t softbuffer;
  srslte_nbiot_cell_t    cell;
  srslte_mib_nb_t        mib;
  bool                   mib_set;

  int    nof_re;     // Number of RE per subframe
  cf_t*  sf_symbols; // this buffer holds the symbols of the current subframe
  cf_t*  sf_buffer;  // this buffer holds multiple subframes
  cf_t*  ce[SRSLTE_MAX_PORTS];
  cf_t*  ce_buffer[SRSLTE_MAX_PORTS];
  float* llr; // Buffer to hold soft-bits for later combining repetitions

  uint32_t pkt_errors;
  uint32_t pkts_total;
  uint32_t pkts_ok;
  uint32_t nof_detected;
  uint32_t bits_total;

  // DL configuration for "normal" transmissions
  bool                has_dl_grant;
  srslte_npdsch_cfg_t npdsch_cfg;

  // DL configuration for SIB1 transmissions
  uint32_t                 sib1_sfn[4 * SIB1_NB_MAX_REP]; // there are 4 SIB1 TTIs in each hyper frame
  srslte_nbiot_si_params_t si_params[SRSLTE_NBIOT_SI_TYPE_NITEMS];
  bool                     si_tti[10240]; // We trade memory consumption for speed

  uint16_t              current_rnti;
  uint32_t              last_n_cce;
  srslte_dci_location_t last_location;

  float sample_offset;
} srslte_nbiot_ue_dl_t;

// This function shall be called just after the initial synchronization
SRSLTE_API int srslte_nbiot_ue_dl_init(srslte_nbiot_ue_dl_t* q,
                                       cf_t*                 in_buffer[SRSLTE_MAX_PORTS],
                                       uint32_t              max_prb,
                                       uint32_t              nof_rx_antennas);

SRSLTE_API void srslte_nbiot_ue_dl_free(srslte_nbiot_ue_dl_t* q);

SRSLTE_API int srslte_nbiot_ue_dl_set_cell(srslte_nbiot_ue_dl_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API int srslte_nbiot_ue_dl_decode_fft_estimate(srslte_nbiot_ue_dl_t* q, uint32_t sf_idx, bool is_dl_sf);

SRSLTE_API int srslte_nbiot_ue_dl_decode_estimate(srslte_nbiot_ue_dl_t* q, uint32_t sf_idx);

SRSLTE_API int
srslte_nbiot_ue_dl_cfg_grant(srslte_nbiot_ue_dl_t* q, srslte_ra_nbiot_dl_grant_t* grant, uint32_t sf_idx);

SRSLTE_API int
srslte_nbiot_ue_dl_find_dl_dci(srslte_nbiot_ue_dl_t* q, uint32_t sf_idx, uint16_t rnti, srslte_dci_msg_t* dci_msg);

int srslte_nbiot_ue_dl_find_dl_dci_type_siprarnti(srslte_nbiot_ue_dl_t* q, uint16_t rnti, srslte_dci_msg_t* dci_msg);

int srslte_nbiot_ue_dl_find_dl_dci_type_crnti(srslte_nbiot_ue_dl_t* q,
                                              uint32_t              sf_idx,
                                              uint16_t              rnti,
                                              srslte_dci_msg_t*     dci_msg);

SRSLTE_API int
srslte_nbiot_ue_dl_find_ul_dci(srslte_nbiot_ue_dl_t* q, uint32_t tti, uint32_t rnti, srslte_dci_msg_t* dci_msg);

SRSLTE_API uint32_t srslte_nbiot_ue_dl_get_ncce(srslte_nbiot_ue_dl_t* q);

SRSLTE_API void srslte_nbiot_ue_dl_set_sample_offset(srslte_nbiot_ue_dl_t* q, float sample_offset);

SRSLTE_API int
srslte_nbiot_ue_dl_decode(srslte_nbiot_ue_dl_t* q, cf_t* input, uint8_t* data, uint32_t sfn, uint32_t sf_idx);

SRSLTE_API int srslte_nbiot_ue_dl_decode_npdcch(srslte_nbiot_ue_dl_t* q,
                                                cf_t*                 input,
                                                uint32_t              sfn,
                                                uint32_t              sf_idx,
                                                uint16_t              rnti,
                                                srslte_dci_msg_t*     dci_msg);

SRSLTE_API int srslte_nbiot_ue_dl_decode_npdsch(srslte_nbiot_ue_dl_t* q,
                                                cf_t*                 input,
                                                uint8_t*              data,
                                                uint32_t              sfn,
                                                uint32_t              sf_idx,
                                                uint16_t              rnti);

int srslte_nbiot_ue_dl_decode_npdsch_bcch(srslte_nbiot_ue_dl_t* q, uint8_t* data, uint32_t tti);

int srslte_nbiot_ue_dl_decode_npdsch_no_bcch(srslte_nbiot_ue_dl_t* q, uint8_t* data, uint32_t tti, uint16_t rnti);

void srslte_nbiot_ue_dl_tb_decoded(srslte_nbiot_ue_dl_t* q, uint8_t* data);

SRSLTE_API void srslte_nbiot_ue_dl_reset(srslte_nbiot_ue_dl_t* q);

SRSLTE_API void srslte_nbiot_ue_dl_set_rnti(srslte_nbiot_ue_dl_t* q, uint16_t rnti);

SRSLTE_API void srslte_nbiot_ue_dl_set_mib(srslte_nbiot_ue_dl_t* q, srslte_mib_nb_t mib);

SRSLTE_API void srslte_nbiot_ue_dl_decode_sib1(srslte_nbiot_ue_dl_t* q, uint32_t current_sfn);

SRSLTE_API void
srslte_nbiot_ue_dl_get_sib1_grant(srslte_nbiot_ue_dl_t* q, uint32_t sfn, srslte_ra_nbiot_dl_grant_t* grant);

SRSLTE_API void srslte_nbiot_ue_dl_decode_sib(srslte_nbiot_ue_dl_t*    q,
                                              uint32_t                 hfn,
                                              uint32_t                 sfn,
                                              srslte_nbiot_si_type_t   type,
                                              srslte_nbiot_si_params_t params);

SRSLTE_API void srslte_nbiot_ue_dl_get_next_si_sfn(uint32_t                 current_hfn,
                                                   uint32_t                 current_sfn,
                                                   srslte_nbiot_si_params_t params,
                                                   uint32_t*                si_hfn,
                                                   uint32_t*                si_sfn);

SRSLTE_API void
srslte_nbiot_ue_dl_set_si_params(srslte_nbiot_ue_dl_t* q, srslte_nbiot_si_type_t type, srslte_nbiot_si_params_t params);

SRSLTE_API bool srslte_nbiot_ue_dl_is_sib1_sf(srslte_nbiot_ue_dl_t* q, uint32_t sfn, uint32_t sf_idx);

SRSLTE_API void srslte_nbiot_ue_dl_set_grant(srslte_nbiot_ue_dl_t* q, srslte_ra_nbiot_dl_grant_t* grant);

SRSLTE_API void srslte_nbiot_ue_dl_flush_grant(srslte_nbiot_ue_dl_t* q);

SRSLTE_API bool srslte_nbiot_ue_dl_has_grant(srslte_nbiot_ue_dl_t* q);

SRSLTE_API void srslte_nbiot_ue_dl_check_grant(srslte_nbiot_ue_dl_t* q, srslte_ra_nbiot_dl_grant_t* grant);

SRSLTE_API void srslte_nbiot_ue_dl_save_signal(srslte_nbiot_ue_dl_t* q, cf_t* input, uint32_t tti, uint32_t sf_idx);

#endif // SRSLTE_UE_DL_NBIOT_H
