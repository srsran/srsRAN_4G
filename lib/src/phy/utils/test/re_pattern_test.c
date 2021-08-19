/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/utils/re_pattern.h"
#include "srsran/support/srsran_test.h"

int main(int argc, char** argv)
{
  srsran_re_pattern_list_t pattern_list;

  // Reset list
  srsran_re_pattern_reset(&pattern_list);

  // Create first pattern and merge
  srsran_re_pattern_t pattern_1 = {};
  pattern_1.rb_begin            = 1;
  pattern_1.rb_end              = 50;
  pattern_1.rb_stride           = 1;
  for (uint32_t k = 0; k < SRSRAN_NRE; k++) {
    pattern_1.sc[k] = (k % 2 == 0); // Only even subcarriers
  }
  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    pattern_1.symbol[l] = (l % 2 == 0); // Only even symbols
  }
  TESTASSERT(srsran_re_pattern_merge(&pattern_list, &pattern_1) == SRSRAN_SUCCESS);
  TESTASSERT(pattern_list.count == 1);

  // Create second pattern and merge
  srsran_re_pattern_t pattern_2 = pattern_1;
  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    pattern_2.symbol[l] = (l % 2 == 1); // Only odd symbols
  }
  TESTASSERT(srsran_re_pattern_merge(&pattern_list, &pattern_2) == SRSRAN_SUCCESS);
  TESTASSERT(pattern_list.count == 1);

  // Assert generated mask
  for (uint32_t l = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    bool mask[SRSRAN_NRE * SRSRAN_MAX_PRB_NR] = {};
    TESTASSERT(srsran_re_pattern_list_to_symbol_mask(&pattern_list, l, mask) == SRSRAN_SUCCESS);
    for (uint32_t k = 0; k < SRSRAN_NRE * SRSRAN_MAX_PRB_NR; k++) {
      if (k >= pattern_1.rb_begin * SRSRAN_NRE && k < pattern_1.rb_end * SRSRAN_NRE &&
          (k / SRSRAN_NRE - pattern_1.rb_begin) % pattern_1.rb_stride == 0) {
        TESTASSERT(mask[k] == (k % 2 == 0));
      } else {
        TESTASSERT(mask[k] == false);
      }
    }
  }

  return SRSRAN_SUCCESS;
}