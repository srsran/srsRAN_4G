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
 *  File:         pdcch.h
 *
 *  Description:  Physical downlink control channel for NR.
 *
 *  Reference:    3GPP TS 38.211 version 15.10.0
 *****************************************************************************/

#ifndef SRSLTE_PDCCH_NR_H
#define SRSLTE_PDCCH_NR_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"

/**
 * @brief Function for generating NR PDCCH candidate locations n_cce for a given CORESET, search space, aggregation
 * level and slot.
 *
 * @see srslte_pdcch_ue_locations_ncce
 * @see srslte_pdcch_common_locations_ncce
 *
 * @param coreset is the coreset configuration provided from higher layers
 * @param search_space is the Search Space configuration provided from higher layers
 * @param RNTI UE temporal identifier, unused for common search spaces
 * @param aggregation_level aggregation level in logarithm range (0,1,2,3,4)
 * @param slot_idx Slot index within the radio frame
 * @param locations is the destination array with the possible candidate locations n_cce
 */
int srslte_pdcch_nr_locations_coreset(const srslte_coreset_t*      coreset,
                                      const srslte_search_space_t* search_space,
                                      uint16_t                     rnti,
                                      uint32_t                     aggregation_level,
                                      uint32_t                     slot_idx,
                                      uint32_t locations[SRSLTE_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR]);

#endif // SRSLTE_PDCCH_NR_H
