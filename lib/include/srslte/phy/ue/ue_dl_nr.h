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
#include "srslte/phy/phch/uci_cfg_nr.h"

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

typedef struct {
  uint32_t scell_idx;  ///< Serving cell index
  uint32_t v_dai_dl;   ///< Downlink Assigment Index
  bool dci_format_1_1; ///< Set to true if the PDSCH transmission is triggered by a PDCCH DCI format 1_1 transmission
  uint32_t k1;         ///< HARQ feedback timing
  uint16_t rnti;
  uint32_t pucch_resource_id;
} srslte_pdsch_ack_resource_nr_t;

typedef struct {
  srslte_pdsch_ack_resource_nr_t resource;
  uint8_t                        value[SRSLTE_MAX_CODEWORDS]; // 0/1 or 2 for DTX
  bool present; // set to true if there is a PDSCH on serving cell c associated with PDCCH in PDCCH monitoring occasion
                // m, or there is a PDCCH indicating SPS PDSCH release on serving cell c
  bool dl_bwp_changed; // set to true if PDCCH monitoring occasion m is before an active DL BWP change on serving cell c
  bool ul_bwp_changed; // set to true if an active UL BWP change on the PCell and an active DL BWP change is not
                       // triggered by a DCI format 1_1 in PDCCH monitoring occasion m
  bool second_tb_present; // set to true if two TB were detected in the PDCCH monitoring occasion m
} srslte_pdsch_ack_m_nr_t;

#define SRSLTE_UCI_NR_MAX_M 10

typedef struct {
  uint32_t                M;
  srslte_pdsch_ack_m_nr_t m[SRSLTE_UCI_NR_MAX_M];
} srslte_pdsch_ack_cc_nr_t;

typedef struct {
  srslte_pdsch_ack_cc_nr_t cc[SRSLTE_MAX_CARRIERS];
  uint32_t                 nof_cc;
  bool                     use_pusch; // Set to true, if UCI bits are carried by PUSCH
} srslte_pdsch_ack_nr_t;

typedef struct SRSLTE_API {
  bool harq_ack_spatial_bundling_pucch; ///< Param harq-ACK-SpatialBundlingPUCCH, set to true if provided
  bool harq_ack_spatial_bundling_pusch; ///< Param harq-ACK-SpatialBundlingPUSCH, set to true if provided
  srslte_pdsch_harq_ack_codebook_t pdsch_harq_ack_codebook; ///< pdsch-HARQ-ACK-Codebook configuration
  bool max_cw_sched_dci_is_2; ///< Param maxNrofCodeWordsScheduledByDCI, set to true if present and equal to 2

  uint32_t dl_data_to_ul_ack[SRSLTE_MAX_NOF_DL_DATA_TO_UL];
  uint32_t nof_dl_data_to_ul_ack;
} srslte_ue_dl_nr_harq_ack_cfg_t;

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
                                           srslte_rnti_type_t       rnti_type,
                                           srslte_dci_dl_nr_t*      dci_dl_list,
                                           uint32_t                 nof_dci_msg);

SRSLTE_API int srslte_ue_dl_nr_find_ul_dci(srslte_ue_dl_nr_t*       q,
                                           const srslte_slot_cfg_t* slot_cfg,
                                           uint16_t                 rnti,
                                           srslte_rnti_type_t       rnti_type,
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

SRSLTE_API int srslte_ue_dl_nr_pdsch_ack_resource(const srslte_ue_dl_nr_harq_ack_cfg_t* cfg,
                                                  const srslte_dci_dl_nr_t*             dci_dl,
                                                  srslte_pdsch_ack_resource_nr_t*       pdsch_ack_resource);

SRSLTE_API int srslte_ue_dl_nr_gen_ack(const srslte_ue_dl_nr_harq_ack_cfg_t* cfg,
                                       const srslte_pdsch_ack_nr_t*          ack_info,
                                       srslte_uci_data_nr_t*                 uci_data);

#endif // SRSLTE_UE_DL_NR_H
