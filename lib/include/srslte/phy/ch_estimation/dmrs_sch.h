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

#ifndef SRSLTE_DMRS_SCH_H
#define SRSLTE_DMRS_SCH_H

#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/phch_cfg_nr.h"
#include <stdint.h>

#define SRSLTE_DMRS_SCH_MAX_SYMBOLS 4

/**
 * @brief PDSCH DMRS estimator object
 *
 * @note The DMRS PDSCH object has to be initialised and the carrier parameters needs to be set.
 *
 * @see srslte_dmrs_sch_init
 * @see srslte_dmrs_sch_set_carrier
 * @see srslte_dmrs_sch_free
 * @see srslte_dmrs_sch_put_sf
 * @see srslte_dmrs_sch_estimate
 */
typedef struct {
  bool is_ue;

  srslte_carrier_nr_t carrier;

  srslte_interp_lin_t interpolator_type1; /// Type 1 DMRS: 1 pilot every 2 RE
  srslte_interp_lin_t interpolator_type2; /// Type 2 DMRS: 2 consecutive pilots every 6 RE

  uint32_t max_nof_prb;
  cf_t*    pilot_estimates; /// Pilots least squares estimates
  cf_t*    temp;            /// Temporal data vector of size SRSLTE_NRE * carrier.nof_prb

} srslte_dmrs_sch_t;

/**
 * @brief Computes the symbol indexes carrying DMRS and stores them in symbols_idx
 * @param dmrs_cfg DMRS configuration
 * @param grant PDSCH information provided by a DCI
 * @param symbols_idx is the destination pointer where the symbols indexes are stored
 * @return It returns the number of symbols if inputs are valid, otherwise, it returns SRSLTE_ERROR code.
 */
SRSLTE_API int srslte_dmrs_sch_get_symbols_idx(const srslte_dmrs_sch_cfg_t* dmrs_cfg,
                                               const srslte_sch_grant_nr_t* grant,
                                               uint32_t                     symbols_idx[SRSLTE_DMRS_SCH_MAX_SYMBOLS]);

/**
 * @brief Computes the sub-carrier indexes carrying DMRS
 *
 * @param cfg PDSCH DMRS configuration provided by upper layers
 * @param max_count is the number of sub-carriers to generate
 * @param sc_idx is the destination pointer where the sub-carrier indexes are stored
 *
 * @return It returns the number of sub-carriers if inputs are valid, otherwise, it returns SRSLTE_ERROR code.
 */
SRSLTE_API int srslte_dmrs_sch_get_sc_idx(const srslte_dmrs_sch_cfg_t* cfg, uint32_t max_count, uint32_t* sc_idx);

/**
 * @brief Calculates the number of resource elements taken by a PDSCH-DMRS for a given PDSCH transmission
 * @param dmrs_cfg PDSCH-DMRS configuration
 * @param grant PDSCH information provided by a DCI
 * @return it returns the number of resource elements if the configuration is valid, otherwise it returns SRSLTE_ERROR
 */
SRSLTE_API int srslte_dmrs_sch_get_N_prb(const srslte_dmrs_sch_cfg_t* dmrs_cfg, const srslte_sch_grant_nr_t* grant);

/**
 * @brief Stringifies the PDSCH DMRS configuration
 *
 * @param cfg PDSCH DMRS configuration
 * @param msg Pointer to the destination array
 * @param max_len Maximum number of characters to write
 *
 * @return It returns the number of characters written in the vector if no error occurs, otherwise it returns
 * SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_sch_cfg_to_str(const srslte_dmrs_sch_cfg_t* cfg, char* msg, uint32_t max_len);

/**
 * @brief Initialises DMRS PDSCH object
 *
 * @param q DMRS PDSCH object
 * @param is_ue indicates whethe the object is for a UE (in this case, it shall initialise as an estimator)
 * @return it returns SRSLTE_ERROR code if an error occurs, otherwise it returns SRSLTE_SUCCESS
 */
SRSLTE_API int srslte_dmrs_sch_init(srslte_dmrs_sch_t* q, bool is_ue);

/**
 * @brief Frees DMRS PDSCH object
 *
 * @param q DMRS PDSCH object
 */
SRSLTE_API void srslte_dmrs_sch_free(srslte_dmrs_sch_t* q);

/**
 * @brief Sets the carrier configuration. if the PDSCH DMRS object is configured as UE, it will resize internal buffers
 * and objects.
 *
 * @param q DMRS PDSCH object
 * @param carrier Carrier configuration
 *
 * @return it returns SRSLTE_ERROR code if an error occurs, otherwise it returns SRSLTE_SUCCESS
 */
SRSLTE_API int srslte_dmrs_sch_set_carrier(srslte_dmrs_sch_t* q, const srslte_carrier_nr_t* carrier);

/**
 * @brief Puts PDSCH DMRS into a given resource grid
 *
 * @param q DMRS PDSCH object
 * @param slot_cfg Slot configuration
 * @param pdsch_cfg PDSCH configuration provided by upper layers
 * @param grant PDSCH information provided by a DCI
 * @param sf_symbols Resource grid
 *
 * @return it returns SRSLTE_ERROR code if an error occurs, otherwise it returns SRSLTE_SUCCESS
 */
SRSLTE_API int srslte_dmrs_sch_put_sf(srslte_dmrs_sch_t*           q,
                                      const srslte_dl_slot_cfg_t*  slot_cfg,
                                      const srslte_sch_cfg_nr_t*   pdsch_cfg,
                                      const srslte_sch_grant_nr_t* grant,
                                      cf_t*                        sf_symbols);

/**
 * @brief Estimates the channel for PDSCH from the DMRS
 *
 * @attention Current implementation supports only type1 PDSCH DMRS (1 pilot every 2 RE)
 *
 * @param q DMRS-PDSCH object
 * @param slot_cfg Slot configuration
 * @param pdsch_cfg PDSCH configuration provided by upper layers
 * @param grant PDSCH information provided by a DCI
 * @param sf_symbols Received resource grid
 * @param[out] ce Channel estimates
 *
 * @return it returns SRSLTE_ERROR code if an error occurs, otherwise it returns SRSLTE_SUCCESS
 */
SRSLTE_API int srslte_dmrs_sch_estimate(srslte_dmrs_sch_t*           q,
                                        const srslte_dl_slot_cfg_t*  slot_cfg,
                                        const srslte_sch_cfg_nr_t*   pdsch_cfg,
                                        const srslte_sch_grant_nr_t* grant,
                                        const cf_t*                  sf_symbols,
                                        srslte_chest_dl_res_t*       chest_res);

#endif // SRSLTE_DMRS_SCH_H
