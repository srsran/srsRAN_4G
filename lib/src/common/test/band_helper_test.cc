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

#include "srsran/common/band_helper.h"
#include "srsran/common/test_common.h"

int bands_test_nr()
{
  srsran::srsran_band_helper bands;

  TESTASSERT(bands.nr_arfcn_to_freq(632628) == 3489.42e6);

  const uint32_t max_valid_nr_arfcn = 3279165;

  // Max ARFCN is 3279165 at almost 10 GHz
  TESTASSERT(bands.nr_arfcn_to_freq(max_valid_nr_arfcn) == 99.99996e9);

  // Invalid ARFCN
  TESTASSERT(bands.nr_arfcn_to_freq(max_valid_nr_arfcn + 1) == 0.0);

  std::vector<uint32_t> band_vector = bands.get_bands_nr(632628);
  TESTASSERT(band_vector.size() == 2);
  TESTASSERT(band_vector.at(0) == 77);
  TESTASSERT(band_vector.at(1) == 78);

  // Invalid configs
  // For 30 kHz, 620001 is not a valid ARFCN, only every 2nd
  band_vector = bands.get_bands_nr(620001, srsran::srsran_band_helper::KHZ_30);
  TESTASSERT(band_vector.empty());

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(bands_test_nr() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}