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

#ifndef SRSRAN_RE_PATTERN_H
#define SRSRAN_RE_PATTERN_H

#include "../common/phy_common_nr.h"

/**
 * @brief Maximum number of elements in a pattern list
 */
#define SRSRAN_RE_PATTERN_LIST_SIZE 4

/**
 * @brief Characterizes a pattern in frequency-time domain in an NR slot resource grid
 */
typedef struct SRSRAN_API {
  uint32_t rb_begin;                         ///< RB where the pattern begins in frequency domain
  uint32_t rb_end;                           ///< RB where the pattern ends in frequency domain (excluded)
  uint32_t rb_stride;                        ///< RB index jump
  bool     sc[SRSRAN_NRE];                   ///< Frequency-domain pattern
  bool     symbol[SRSRAN_NSYMB_PER_SLOT_NR]; ///< Indicates OFDM symbols where the pattern is present
} srsran_re_pattern_t;

/**
 * @brief List of RE patterns
 */
typedef struct SRSRAN_API {
  srsran_re_pattern_t data[SRSRAN_RE_PATTERN_LIST_SIZE]; ///< Actual patterns
  uint32_t            count;                             ///< Number of RE patterns
} srsran_re_pattern_list_t;

/**
 * @brief Calculates if a pattern matches a RE given a symbol l and a subcarrier k
 * @param list Provides a list of patterns
 * @param l OFDM symbol index
 * @param k Subcarrier index
 * @return True if pattern is valid and there is a match, false otherwise
 */
SRSRAN_API bool srsran_re_pattern_to_mask(const srsran_re_pattern_list_t* list, uint32_t l, uint32_t k);

/**
 * @brief Calculates the pattern mask for an entire symbol from a RE pattern list
 * @param list Provides a list of patterns
 * @param l OFDM symbol index
 * @param[out] mask Mask vector
 * @return SRSRAN_SUCCESS if the mask is computed successfully, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_re_pattern_to_symbol_mask(const srsran_re_pattern_t* pattern, uint32_t l, bool* mask);

/**
 * @brief Calculates the pattern mask for an entire symbol from a RE pattern
 * @param list Provides a list of patterns
 * @param l OFDM symbol index
 * @param[out] mask Mask vector
 * @return SRSRAN_SUCCESS if the mask is computed successfully, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_re_pattern_list_to_symbol_mask(const srsran_re_pattern_list_t* list, uint32_t l, bool* mask);

/**
 * @brief Merges a pattern into the pattern list, it either merges subcarrier or symbol mask or simply appends a new
 * pattern
 * @param patterns Provides a list of patterns
 * @param p Provides pattern to merge
 * @return SRSRAN_SUCCESS if merging is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_re_pattern_merge(srsran_re_pattern_list_t* list, const srsran_re_pattern_t* p);

/**
 * @brief Checks collision between a RE pattern list and a RE pattern
 * @param list Provides pattern list
 * @param p Provides a pattern
 * @return SRSRAN_SUCCESS if no collision is detected, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_re_pattern_check_collision(const srsran_re_pattern_list_t* list, const srsran_re_pattern_t* p);

/**
 * @brief Initialises a given pattern list
 * @param patterns Provides a list of patterns
 */
SRSRAN_API void srsran_re_pattern_reset(srsran_re_pattern_list_t* list);

/**
 * @brief Writes a RE pattern information into a string
 * @param pattern Provides the pattern
 * @param str Provides string pointer
 * @param str_len Maximum string length
 * @return The number of characters writen into the string
 */
SRSRAN_API uint32_t srsran_re_pattern_info(const srsran_re_pattern_t* pattern, char* str, uint32_t str_len);

/**
 * @brief Writes a RE pattern list information into a string
 * @param pattern Provides the pattern list
 * @param str Provides string pointer
 * @param str_len Maximum string length
 * @return The number of characters writen into the string
 */
SRSRAN_API uint32_t srsran_re_pattern_list_info(const srsran_re_pattern_list_t* pattern, char* str, uint32_t str_len);

/**
 * @brief Counts the number of RE in a transmission characterised by initial and final symbol indexes and a PRB mask.
 * @param list RE pattern list
 * @param symbol_begin First transmission symbol
 * @param symbol_end Last (excluded) transmission symbol
 * @param prb_mask Frequency domain resource block mask
 * @return The number of RE occupied by the pattern list in the transmission
 */
SRSRAN_API uint32_t srsran_re_pattern_list_count(const srsran_re_pattern_list_t* list,
                                                 uint32_t                        symbol_begin,
                                                 uint32_t                        symbol_end,
                                                 const bool                      prb_mask[SRSRAN_MAX_PRB_NR]);

#endif // SRSRAN_RE_PATTERN_H
