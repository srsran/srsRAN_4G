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

#ifndef SRSLTE_UE_DL_NR_H
#define SRSLTE_UE_DL_NR_H

#include "srslte/phy/ch_estimation/dmrs_pdcch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/dci_nr.h"
#include "srslte/phy/phch/pdcch_nr.h"
#include "srslte/phy/phch/pdsch_nr.h"

/**
 * Maximum number of CORESET
 * @remark Defined in TS 38.331 by maxNrofControlResourceSets-1
 */
#define SRSLTE_UE_DL_NR_MAX_NOF_CORESET 12

/**
 * Maximum number of Search spaces
 * @remark Defined in TS 38.331 by maxNrofSearchSpaces-1
 */
#define SRSLTE_UE_DL_NR_MAX_NOF_SEARCH_SPACE 40

/**
 * Maximum number of DCI messages to receive
 */
#define SRSLTE_MAX_DCI_MSG_NR 4

typedef struct SRSLTE_API {
  srslte_pdsch_nr_args_t pdsch;
  srslte_pdcch_nr_args_t pdcch;
  uint32_t               nof_rx_antennas;
  uint32_t               nof_max_prb;
  float                  pdcch_dmrs_corr_thr;
  float                  pdcch_dmrs_epre_thr;
} srslte_ue_dl_nr_args_t;

typedef struct SRSLTE_API {
  srslte_coreset_t coreset[SRSLTE_UE_DL_NR_MAX_NOF_CORESET]; ///< PDCCH Control resource sets (CORESET) collection
  bool             coreset_present[SRSLTE_UE_DL_NR_MAX_NOF_CORESET]; ///< CORESET present flags

  srslte_search_space_t search_space[SRSLTE_UE_DL_NR_MAX_NOF_SEARCH_SPACE];
  bool                  search_space_present[SRSLTE_UE_DL_NR_MAX_NOF_SEARCH_SPACE];

  uint16_t              ra_rnti; ///< Needs to be deduced from the PRACH configuration
  srslte_search_space_t ra_search_space;
  bool                  ra_search_space_present;
} srslte_ue_dl_nr_pdcch_cfg_t;

typedef struct SRSLTE_API {
  uint32_t max_prb;
  uint32_t nof_rx_antennas;
  float    pdcch_dmrs_corr_thr;
  float    pdcch_dmrs_epre_thr;

  srslte_carrier_nr_t         carrier;
  srslte_ue_dl_nr_pdcch_cfg_t cfg;

  srslte_ofdm_t fft[SRSLTE_MAX_PORTS];

  cf_t*                 sf_symbols[SRSLTE_MAX_PORTS];
  srslte_chest_dl_res_t chest;
  srslte_pdsch_nr_t     pdsch;
  srslte_dmrs_sch_t     dmrs_pdsch;

  srslte_dmrs_pdcch_estimator_t dmrs_pdcch[SRSLTE_UE_DL_NR_MAX_NOF_CORESET];
  srslte_pdcch_nr_t             pdcch;
  srslte_dmrs_pdcch_ce_t*       pdcch_ce;

  srslte_dci_msg_nr_t pending_ul_dci_msg[SRSLTE_MAX_DCI_MSG_NR];
  uint32_t            pending_ul_dci_count;
} srslte_ue_dl_nr_t;

SRSLTE_API int
srslte_ue_dl_nr_init(srslte_ue_dl_nr_t* q, cf_t* input[SRSLTE_MAX_PORTS], const srslte_ue_dl_nr_args_t* args);

SRSLTE_API int srslte_ue_dl_nr_set_carrier(srslte_ue_dl_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_ue_dl_nr_set_pdcch_config(srslte_ue_dl_nr_t* q, const srslte_ue_dl_nr_pdcch_cfg_t* cfg);

SRSLTE_API void srslte_ue_dl_nr_free(srslte_ue_dl_nr_t* q);

SRSLTE_API void srslte_ue_dl_nr_estimate_fft(srslte_ue_dl_nr_t* q, const srslte_slot_cfg_t* slot_cfg);

SRSLTE_API int srslte_ue_dl_nr_find_dl_dci(srslte_ue_dl_nr_t*       q,
                                           const srslte_slot_cfg_t* slot_cfg,
                                           uint16_t                 rnti,
                                           srslte_dci_dl_nr_t*      dci_dl_list,
                                           uint32_t                 nof_dci_msg);

SRSLTE_API int srslte_ue_dl_nr_find_ul_dci(srslte_ue_dl_nr_t*       q,
                                           const srslte_slot_cfg_t* slot_cfg,
                                           uint16_t                 rnti,
                                           srslte_dci_ul_nr_t*      dci_ul_list,
                                           uint32_t                 nof_dci_msg);

SRSLTE_API int srslte_ue_dl_nr_decode_pdsch(srslte_ue_dl_nr_t*         q,
                                            const srslte_slot_cfg_t*   slot,
                                            const srslte_sch_cfg_nr_t* cfg,
                                            srslte_pdsch_res_nr_t*     res);

SRSLTE_API int srslte_ue_dl_nr_pdsch_info(const srslte_ue_dl_nr_t*    q,
                                          const srslte_sch_cfg_nr_t*  cfg,
                                          const srslte_pdsch_res_nr_t res[SRSLTE_MAX_CODEWORDS],
                                          char*                       str,
                                          uint32_t                    str_len);

#endif // SRSLTE_UE_DL_NR_H
