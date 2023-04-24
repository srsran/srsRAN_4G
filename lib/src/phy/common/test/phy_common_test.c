/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/common/test_common.h"
#include "srsran/phy/common/phy_common.h"

int srsran_default_rates_test()
{
  // Verify calculated sample rates for all valid PRB sizes.
  // By default we use the reduced 3/4 sampling to save bandwidth on the fronthaul.
#ifdef FORCE_STANDARD_RATE
  srsran_use_standard_symbol_size(false);
#endif
  TESTASSERT(srsran_sampling_freq_hz(6) == 1920000);
  TESTASSERT(srsran_sampling_freq_hz(15) == 3840000);
  TESTASSERT(srsran_sampling_freq_hz(25) == 5760000);
  TESTASSERT(srsran_sampling_freq_hz(50) == 11520000);
  TESTASSERT(srsran_sampling_freq_hz(75) == 15360000); // need to use default rate for 15 MHz BW
  TESTASSERT(srsran_sampling_freq_hz(100) == 23040000);
  return SRSRAN_SUCCESS;
}

int lte_standard_rates_test()
{
  // Verify calculated sample rates for all valid PRB sizes.
  // Enable standard LTE rates (required by some RF HW).
  srsran_use_standard_symbol_size(true);
  TESTASSERT(srsran_sampling_freq_hz(6) == 1920000);
  TESTASSERT(srsran_sampling_freq_hz(15) == 3840000);
  TESTASSERT(srsran_sampling_freq_hz(25) == 7680000);
  TESTASSERT(srsran_sampling_freq_hz(50) == 15360000);
  TESTASSERT(srsran_sampling_freq_hz(75) == 23040000);
  TESTASSERT(srsran_sampling_freq_hz(100) == 30720000);
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(srsran_default_rates_test() == SRSRAN_SUCCESS);
  TESTASSERT(lte_standard_rates_test() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}