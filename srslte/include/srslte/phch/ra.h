/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef RB_ALLOC_H_
#define RB_ALLOC_H_

#include <stdint.h>
#include <stdbool.h>

#include "srslte/config.h"

/** Structures and utility functions for DL/UL resource
 * allocation.
 */

typedef struct SRSLTE_API {
  srslte_mod_t mod;
  int tbs;
} ra_mcs_t;

typedef enum SRSLTE_API {
  alloc_type0 = 0, alloc_type1 = 1, alloc_type2 = 2
} ra_type_t;

typedef struct SRSLTE_API {
  uint32_t rbg_bitmask;
} ra_type0_t;

typedef struct SRSLTE_API {
  uint32_t vrb_bitmask;
  uint32_t rbg_subset;
  bool shift;
} ra_type1_t;

typedef struct SRSLTE_API {
  uint32_t riv; // if L_crb==0, DCI message packer will take this value directly
  uint32_t L_crb;
  uint32_t RB_start;
  enum {
    nprb1a_2 = 0, nprb1a_3 = 1
  } n_prb1a;
  enum {
    t2_ng1 = 0, t2_ng2 = 1
  } n_gap;
  enum {
    t2_loc = 0, t2_dist = 1
  } mode;
} ra_type2_t;

typedef struct SRSLTE_API {
  bool prb_idx[SRSLTE_MAX_PRB];
  uint32_t nof_prb;
} ra_prb_slot_t;

typedef struct SRSLTE_API {
  ra_prb_slot_t slot[2];
  uint32_t lstart;
  uint32_t re_sf[SRSLTE_NSUBFRAMES_X_FRAME];
} ra_dl_alloc_t;

typedef struct SRSLTE_API {
  uint32_t n_prb[2];
  uint32_t n_prb_tilde[2];
  uint32_t L_prb;
  uint32_t freq_hopping; 
} ra_ul_alloc_t;

typedef struct SRSLTE_API {
  uint16_t rnti;
  ra_type_t alloc_type;
  union {
    ra_type0_t type0_alloc;
    ra_type1_t type1_alloc;
    ra_type2_t type2_alloc;
  };
  ra_dl_alloc_t prb_alloc;
  uint32_t mcs_idx;
  ra_mcs_t mcs;
  uint32_t harq_process;
  uint32_t rv_idx;
  bool ndi;
} ra_pdsch_t;

typedef struct SRSLTE_API {
  /* 36.213 Table 8.4-2: hop_half is 0 for < 10 Mhz and 10 for > 10 Mhz.
   * hop_quart is 00 for > 10 Mhz and hop_quart_neg is 01 for > 10 Mhz.
   */
  enum {
    hop_disabled = -1,
    hop_quart = 0,
    hop_quart_neg = 1,
    hop_half = 2,
    hop_type_2 = 3
  } freq_hop_fl;

  ra_ul_alloc_t prb_alloc;
  
  ra_type2_t type2_alloc;
  uint32_t mcs_idx;
  ra_mcs_t mcs;
  uint32_t rv_idx; // If set to non-zero, a retransmission is requested with the same modulation
  // than before (Format0 message, see also 8.6.1 in 36.2313).
  bool ndi;
  bool cqi_request;

} ra_pusch_t;

SRSLTE_API void ra_prb_fprint(FILE *f, 
                              ra_prb_slot_t *prb, 
                              uint32_t nof_prb);

SRSLTE_API int ra_dl_alloc(ra_dl_alloc_t *prb, 
                             ra_pdsch_t *ra, 
                             uint32_t nof_prb);

SRSLTE_API int ra_ul_alloc(ra_ul_alloc_t *prb, 
                           ra_pusch_t *ra, 
                           uint32_t n_rb_ho, 
                           uint32_t nof_prb);

SRSLTE_API void ra_dl_alloc_re(ra_dl_alloc_t *prb_dist, 
                               uint32_t nof_prb, 
                               uint32_t nof_ports,
                               uint32_t nof_ctrl_symbols, 
                               srslte_cp_t cp);

SRSLTE_API uint32_t ra_nprb_dl(ra_pdsch_t *ra, 
                               uint32_t nof_prb);

SRSLTE_API uint32_t ra_nprb_ul(ra_pusch_t *ra, 
                               uint32_t nof_prb);

SRSLTE_API int ra_mcs_from_idx_dl(uint32_t mcs_idx, 
                                  uint32_t nof_prb, 
                                  ra_mcs_t *mcs);

SRSLTE_API int ra_mcs_from_idx_ul(uint32_t mcs_idx, 
                                  uint32_t nof_prb, 
                                  ra_mcs_t *mcs);

SRSLTE_API int ra_tbs_from_idx_format1c(uint32_t tbs_idx);

SRSLTE_API int ra_tbs_from_idx(uint32_t tbs_idx, 
                               uint32_t n_prb);

SRSLTE_API int ra_tbs_to_table_idx(uint32_t tbs, 
                                   uint32_t n_prb);

SRSLTE_API uint32_t ra_type0_P(uint32_t nof_prb);

SRSLTE_API uint32_t ra_type2_to_riv(uint32_t L_crb, 
                                    uint32_t RB_start, 
                                    uint32_t nof_prb);

SRSLTE_API void ra_type2_from_riv(uint32_t riv, 
                                  uint32_t *L_crb, 
                                  uint32_t *RB_start,
                                  uint32_t nof_prb, 
                                  uint32_t nof_vrb);

SRSLTE_API uint32_t ra_type2_n_vrb_dl(uint32_t nof_prb, 
                                 bool ngap_is_1);

SRSLTE_API uint32_t ra_type2_n_rb_step(uint32_t nof_prb);

SRSLTE_API uint32_t ra_type2_ngap(uint32_t nof_prb, 
                             bool ngap_is_1);

SRSLTE_API uint32_t ra_type1_N_rb(uint32_t nof_prb);

SRSLTE_API void ra_pdsch_fprint(FILE *f, 
                                ra_pdsch_t *ra, 
                                uint32_t nof_prb);

SRSLTE_API void ra_pusch_fprint(FILE *f, 
                                ra_pusch_t *ra, 
                                uint32_t nof_prb);

#endif /* RB_ALLOC_H_ */
