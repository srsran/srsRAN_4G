/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/phy/utils/re_pattern.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

bool srsran_re_pattern_to_mask(const srsran_re_pattern_list_t* list, uint32_t l, uint32_t k)
{
  uint32_t rb_idx = k % SRSRAN_NRE;
  uint32_t sc_idx = k / SRSRAN_NRE;

  // Check pattern list is valid
  if (list == NULL) {
    return false;
  }

  // Iterate all given patterns
  for (uint32_t i = 0; i < list->count; i++) {
    const srsran_re_pattern_t* pattern = &list->data[i];

    // Skip pattern if it is not active in this OFDM symbol
    if (!pattern->symbol[l]) {
      continue;
    }

    // Skip pattern if RB index is put of the pattern bounds
    if (rb_idx < pattern->rb_begin || rb_idx >= pattern->rb_end) {
      continue;
    }

    // Matched SC, early return
    if (pattern->sc[sc_idx]) {
      return true;
    }
  }

  // If reached here, no pattern was matched
  return false;
}

int srsran_re_pattern_to_symbol_mask(const srsran_re_pattern_t* pattern, uint32_t l, bool* mask)
{
  // Check inputs
  if (pattern == NULL || mask == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Check symbol index is in range
  if (l >= SRSRAN_NSYMB_PER_SLOT_NR) {
    ERROR("Symbol index is out of range");
    return SRSRAN_ERROR;
  }

  // Skip pattern if it is not active in this OFDM symbol
  if (!pattern->symbol[l]) {
    return SRSRAN_SUCCESS;
  }

  // Make sure RB end is bounded
  if (pattern->rb_end > SRSRAN_MAX_PRB_NR) {
    return SRSRAN_ERROR;
  }

  // Add mask for pattern
  for (uint32_t rb_idx = pattern->rb_begin; rb_idx < pattern->rb_end; rb_idx += pattern->rb_stride) {
    for (uint32_t sc_idx = 0; sc_idx < SRSRAN_NRE; sc_idx++) {
      mask[rb_idx * SRSRAN_NRE + sc_idx] |= pattern->sc[sc_idx];
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_re_pattern_list_to_symbol_mask(const srsran_re_pattern_list_t* list, uint32_t l, bool* mask)
{
  // Check inputs
  if (list == NULL || mask == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Iterate all given patterns
  for (uint32_t i = 0; i < list->count; i++) {
    if (srsran_re_pattern_to_symbol_mask(&list->data[i], l, mask) < SRSRAN_SUCCESS) {
      ERROR("Error calculating mask");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_re_pattern_merge(srsran_re_pattern_list_t* list, const srsran_re_pattern_t* p)
{
  // Check inputs are valid
  if (list == NULL || p == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Count number of subcarrier mask
  uint32_t kcount = 0;
  for (uint32_t k = 0; k < SRSRAN_NRE; k++) {
    kcount += p->sc[k] ? 1 : 0;
  }

  // Count number of symbol mask
  uint32_t lcount = 0;
  for (uint32_t l = 0; l < SRSRAN_NRE; l++) {
    lcount += p->symbol[l] ? 1 : 0;
  }

  // If any mask is empty, ignore
  if (kcount == 0 || lcount == 0) {
    return SRSRAN_SUCCESS;
  }

  // Iterate all given patterns
  for (uint32_t i = 0; i < list->count; i++) {
    srsran_re_pattern_t* pattern = &list->data[i];

    // Skip if RB parameters dont match
    if (pattern->rb_begin != p->rb_begin || pattern->rb_end != p->rb_end || pattern->rb_stride != p->rb_stride) {
      continue;
    }

    // Check if symbol mask matches
    bool lmatch = (memcmp(pattern->symbol, p->symbol, SRSRAN_NSYMB_PER_SLOT_NR) == 0);

    // Check if sc mask matches
    bool kmatch = (memcmp(pattern->sc, p->sc, SRSRAN_NRE) == 0);

    // If OFDM symbols and subcarriers mask match, it means that the patterns are completely overlapped and no merging
    // is required
    if (kmatch && lmatch) {
      return SRSRAN_SUCCESS;
    }

    // If OFDM symbols mask matches, merge subcarrier mask
    if (lmatch) {
      for (uint32_t k = 0; k < SRSRAN_NRE; k++) {
        pattern->sc[k] |= p->sc[k];
      }
      return SRSRAN_SUCCESS;
    }

    // If subcarriers mask matches, merge OFDM symbols mask
    if (kmatch) {
      for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
        pattern->symbol[l] |= p->symbol[l];
      }
      return SRSRAN_SUCCESS;
    }
  }

  // If reached here, no pattern was matched. Try appending
  if (list->count >= SRSRAN_RE_PATTERN_LIST_SIZE) {
    ERROR("Insufficient number of available RE patterns in list");
    return SRSRAN_ERROR;
  }

  // Append
  list->data[list->count] = *p;
  list->count++;

  return SRSRAN_SUCCESS;
}

int srsran_re_pattern_check_collision(const srsran_re_pattern_list_t* list, const srsran_re_pattern_t* p)
{
  // Check inputs are valid
  if (list == NULL || p == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Count number of subcarrier mask
  uint32_t kcount = 0;
  for (uint32_t k = 0; k < SRSRAN_NRE; k++) {
    kcount += p->sc[k] ? 1 : 0;
  }

  // Count number of symbol mask
  uint32_t lcount = 0;
  for (uint32_t l = 0; l < SRSRAN_NRE; l++) {
    lcount += p->symbol[l] ? 1 : 0;
  }

  // If any mask is empty, no collision
  if (kcount == 0 || lcount == 0) {
    return SRSRAN_SUCCESS;
  }

  // Iterate all given patterns
  for (uint32_t i = 0; i < list->count; i++) {
    const srsran_re_pattern_t* pattern = &list->data[i];

    // Skip if RB do not overlap
    if (pattern->rb_begin > p->rb_end || p->rb_begin > pattern->rb_end) {
      continue;
    }

    // Check if symbol are matched
    bool lmatch = false;
    for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR && !lmatch; l++) {
      // Consider match if both patterns have a positive symbol in common
      lmatch = (p->symbol[l] && pattern->symbol[l]);
    }

    // If the symbols are not matched, skip pattern
    if (!lmatch) {
      continue;
    }

    // Check if any subcarrier mask matches
    for (uint32_t k = 0; k < SRSRAN_NRE; k++) {
      // Consider a collision if both subcarrier mask are true
      if (p->sc[k] && pattern->sc[k]) {
        return SRSRAN_ERROR;
      }
    }
  }

  // If reached here, means no collision
  return SRSRAN_SUCCESS;
}

void srsran_re_pattern_reset(srsran_re_pattern_list_t* list)
{
  if (list == NULL) {
    return;
  }
  SRSRAN_MEM_ZERO(list, srsran_re_pattern_list_t, 1);
}

uint32_t srsran_re_pattern_info(const srsran_re_pattern_t* pattern, char* str, uint32_t str_len)
{
  if (pattern == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  char subc[SRSRAN_NRE + 1] = {};
  srsran_vec_sprint_bin(subc, SRSRAN_NRE + 1, (uint8_t*)pattern->sc, SRSRAN_NRE);

  char symb[SRSRAN_NSYMB_PER_SLOT_NR + 1] = {};
  srsran_vec_sprint_bin(symb, SRSRAN_NSYMB_PER_SLOT_NR + 1, (uint8_t*)pattern->symbol, SRSRAN_NSYMB_PER_SLOT_NR);

  return srsran_print_check(str,
                            str_len,
                            0,
                            "begin=%d end=%d stride=%d sc=%s symb=%s ",
                            pattern->rb_begin,
                            pattern->rb_end,
                            pattern->rb_stride,
                            subc,
                            symb);
}

uint32_t srsran_re_pattern_list_info(const srsran_re_pattern_list_t* list, char* str, uint32_t str_len)
{
  uint32_t len = 0;
  if (list == NULL || str == NULL || str_len == 0) {
    return 0;
  }

  for (uint32_t i = 0; i < list->count; i++) {
    len = srsran_print_check(str, str_len, len, "RE%d: ", i);
    len += srsran_re_pattern_info(&list->data[i], &str[len], str_len - len);
  }

  return len;
}

uint32_t srsran_re_pattern_list_count(const srsran_re_pattern_list_t* list,
                                      uint32_t                        symbol_begin,
                                      uint32_t                        symbol_end,
                                      const bool                      prb_mask[SRSRAN_MAX_PRB_NR])
{
  uint32_t count = 0;
  if (list == NULL || prb_mask == NULL) {
    return 0;
  }

  // Iterate over all symbols and create a symbol mask
  for (uint32_t l = symbol_begin; l < symbol_end; l++) {
    // Entire symbol mask to prevent overlapped RE to count twice
    bool symbol_mask[SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};

    // For each pattern, compute symbol mask
    for (uint32_t i = 0; i < list->count; i++) {
      if (srsran_re_pattern_to_symbol_mask(&list->data[i], l, symbol_mask) < SRSRAN_SUCCESS) {
        ERROR("Error calculating symbol mask");
        return SRSRAN_ERROR;
      }
    }

    // Count number of masked elements
    for (uint32_t rb = 0; rb < SRSRAN_MAX_PRB_NR; rb++) {
      // Skip PRB if disabled
      if (!prb_mask[rb]) {
        continue;
      }

      // Iterate all subcarriers in the PRB
      for (uint32_t k = rb * SRSRAN_NRE; k < (rb + 1) * SRSRAN_NRE; k++) {
        // Count only the true masked RE
        count += (uint32_t)symbol_mask[k];
      }
    }
  }

  return count;
}