/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_UE_DL_NBIOT_H
#define SRSRAN_UE_DL_NBIOT_H

#include <stdbool.h>

#include "srsran/phy/ch_estimation/chest_dl_nbiot.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/ofdm.h"

#include "srsran/phy/phch/dci_nbiot.h"
#include "srsran/phy/phch/npdcch.h"
#include "srsran/phy/phch/npdsch.h"
#include "srsran/phy/phch/npdsch_cfg.h"
#include "srsran/phy/phch/ra_nbiot.h"

#include "srsran/phy/sync/cfo.h"

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/config.h"

#define SRSRAN_NBIOT_EXPECT_MORE_SF -3  // returned when expecting more subframes for successful decoding
#define SRSRAN_NBIOT_UE_DL_FOUND_DCI -4 // returned when DCI for given RNTI was found
#define SRSRAN_NBIOT_UE_DL_SKIP_SF -5

/*
 * @brief Narrowband UE downlink object.
 *
 * This module is a frontend to all the downlink data and control
 * channel processing modules.
 */
typedef struct SRSRAN_API {
  srsran_npdcch_t         npdcch;
  srsran_npdsch_t         npdsch;
  srsran_ofdm_t           fft;
  srsran_chest_dl_nbiot_t chest;

  srsran_cfo_t sfo_correct;

  srsran_softbuffer_rx_t softbuffer;
  srsran_nbiot_cell_t    cell;
  srsran_mib_nb_t        mib;
  bool                   mib_set;

  int    nof_re;     // Number of RE per subframe
  cf_t*  sf_symbols; // this buffer holds the symbols of the current subframe
  cf_t*  sf_buffer;  // this buffer holds multiple subframes
  cf_t*  ce[SRSRAN_MAX_PORTS];
  cf_t*  ce_buffer[SRSRAN_MAX_PORTS];
  float* llr; // Buffer to hold soft-bits for later combining repetitions

  uint32_t pkt_errors;
  uint32_t pkts_total;
  uint32_t pkts_ok;
  uint32_t nof_detected;
  uint32_t bits_total;

  // DL configuration for "normal" transmissions
  bool                has_dl_grant;
  srsran_npdsch_cfg_t npdsch_cfg;

  // DL configuration for SIB1 transmissions
  uint32_t                 sib1_sfn[4 * SIB1_NB_MAX_REP]; // there are 4 SIB1 TTIs in each hyper frame
  srsran_nbiot_si_params_t si_params[SRSRAN_NBIOT_SI_TYPE_NITEMS];
  bool                     si_tti[10240]; // We trade memory consumption for speed

  uint16_t              current_rnti;
  uint32_t              last_n_cce;
  srsran_dci_location_t last_location;

  float sample_offset;
} srsran_nbiot_ue_dl_t;

// This function shall be called just after the initial synchronization
SRSRAN_API int srsran_nbiot_ue_dl_init(srsran_nbiot_ue_dl_t* q,
                                       cf_t*                 in_buffer[SRSRAN_MAX_PORTS],
                                       uint32_t              max_prb,
                                       uint32_t              nof_rx_antennas);

SRSRAN_API void srsran_nbiot_ue_dl_free(srsran_nbiot_ue_dl_t* q);

SRSRAN_API int srsran_nbiot_ue_dl_set_cell(srsran_nbiot_ue_dl_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API int srsran_nbiot_ue_dl_decode_fft_estimate(srsran_nbiot_ue_dl_t* q, uint32_t sf_idx, bool is_dl_sf);

SRSRAN_API int srsran_nbiot_ue_dl_decode_estimate(srsran_nbiot_ue_dl_t* q, uint32_t sf_idx);

SRSRAN_API int
srsran_nbiot_ue_dl_cfg_grant(srsran_nbiot_ue_dl_t* q, srsran_ra_nbiot_dl_grant_t* grant, uint32_t sf_idx);

SRSRAN_API int
srsran_nbiot_ue_dl_find_dl_dci(srsran_nbiot_ue_dl_t* q, uint32_t sf_idx, uint16_t rnti, srsran_dci_msg_t* dci_msg);

int srsran_nbiot_ue_dl_find_dl_dci_type_siprarnti(srsran_nbiot_ue_dl_t* q, uint16_t rnti, srsran_dci_msg_t* dci_msg);

int srsran_nbiot_ue_dl_find_dl_dci_type_crnti(srsran_nbiot_ue_dl_t* q,
                                              uint32_t              sf_idx,
                                              uint16_t              rnti,
                                              srsran_dci_msg_t*     dci_msg);

SRSRAN_API int
srsran_nbiot_ue_dl_find_ul_dci(srsran_nbiot_ue_dl_t* q, uint32_t tti, uint32_t rnti, srsran_dci_msg_t* dci_msg);

SRSRAN_API uint32_t srsran_nbiot_ue_dl_get_ncce(srsran_nbiot_ue_dl_t* q);

SRSRAN_API void srsran_nbiot_ue_dl_set_sample_offset(srsran_nbiot_ue_dl_t* q, float sample_offset);

SRSRAN_API int
srsran_nbiot_ue_dl_decode(srsran_nbiot_ue_dl_t* q, cf_t* input, uint8_t* data, uint32_t sfn, uint32_t sf_idx);

SRSRAN_API int srsran_nbiot_ue_dl_decode_npdcch(srsran_nbiot_ue_dl_t* q,
                                                cf_t*                 input,
                                                uint32_t              sfn,
                                                uint32_t              sf_idx,
                                                uint16_t              rnti,
                                                srsran_dci_msg_t*     dci_msg);

SRSRAN_API int srsran_nbiot_ue_dl_decode_npdsch(srsran_nbiot_ue_dl_t* q,
                                                cf_t*                 input,
                                                uint8_t*              data,
                                                uint32_t              sfn,
                                                uint32_t              sf_idx,
                                                uint16_t              rnti);

int srsran_nbiot_ue_dl_decode_npdsch_bcch(srsran_nbiot_ue_dl_t* q, uint8_t* data, uint32_t tti);

int srsran_nbiot_ue_dl_decode_npdsch_no_bcch(srsran_nbiot_ue_dl_t* q, uint8_t* data, uint32_t tti, uint16_t rnti);

void srsran_nbiot_ue_dl_tb_decoded(srsran_nbiot_ue_dl_t* q, uint8_t* data);

SRSRAN_API void srsran_nbiot_ue_dl_reset(srsran_nbiot_ue_dl_t* q);

SRSRAN_API void srsran_nbiot_ue_dl_set_rnti(srsran_nbiot_ue_dl_t* q, uint16_t rnti);

SRSRAN_API void srsran_nbiot_ue_dl_set_mib(srsran_nbiot_ue_dl_t* q, srsran_mib_nb_t mib);

SRSRAN_API void srsran_nbiot_ue_dl_decode_sib1(srsran_nbiot_ue_dl_t* q, uint32_t current_sfn);

SRSRAN_API void
srsran_nbiot_ue_dl_get_sib1_grant(srsran_nbiot_ue_dl_t* q, uint32_t sfn, srsran_ra_nbiot_dl_grant_t* grant);

SRSRAN_API void srsran_nbiot_ue_dl_decode_sib(srsran_nbiot_ue_dl_t*    q,
                                              uint32_t                 hfn,
                                              uint32_t                 sfn,
                                              srsran_nbiot_si_type_t   type,
                                              srsran_nbiot_si_params_t params);

SRSRAN_API void srsran_nbiot_ue_dl_get_next_si_sfn(uint32_t                 current_hfn,
                                                   uint32_t                 current_sfn,
                                                   srsran_nbiot_si_params_t params,
                                                   uint32_t*                si_hfn,
                                                   uint32_t*                si_sfn);

SRSRAN_API void
srsran_nbiot_ue_dl_set_si_params(srsran_nbiot_ue_dl_t* q, srsran_nbiot_si_type_t type, srsran_nbiot_si_params_t params);

SRSRAN_API bool srsran_nbiot_ue_dl_is_sib1_sf(srsran_nbiot_ue_dl_t* q, uint32_t sfn, uint32_t sf_idx);

SRSRAN_API void srsran_nbiot_ue_dl_set_grant(srsran_nbiot_ue_dl_t* q, srsran_ra_nbiot_dl_grant_t* grant);

SRSRAN_API void srsran_nbiot_ue_dl_flush_grant(srsran_nbiot_ue_dl_t* q);

SRSRAN_API bool srsran_nbiot_ue_dl_has_grant(srsran_nbiot_ue_dl_t* q);

SRSRAN_API void srsran_nbiot_ue_dl_check_grant(srsran_nbiot_ue_dl_t* q, srsran_ra_nbiot_dl_grant_t* grant);

SRSRAN_API void srsran_nbiot_ue_dl_save_signal(srsran_nbiot_ue_dl_t* q, cf_t* input, uint32_t tti, uint32_t sf_idx);

#endif // SRSRAN_UE_DL_NBIOT_H
