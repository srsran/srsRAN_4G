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
  TESTASSERT(bands.nr_arfcn_to_freq(633928) == 3508.92e6); // default refPointA
  TESTASSERT(bands.nr_arfcn_to_freq(634240) == 3513.6e6);  // default ARFCN with freq divisible by 11.52 MHz
  // b28 b67
  TESTASSERT(bands.nr_arfcn_to_freq(140600) == 703.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(145800) == 729.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(153600) == 768.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(157600) == 788.0e6);
  // b20
  TESTASSERT(bands.nr_arfcn_to_freq(158200) == 791.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(160200) == 801.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(162200) == 811.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(168400) == 842.0e6);
  // b32 b75
  TESTASSERT(bands.nr_arfcn_to_freq(290400) == 1452.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(294400) == 1472.0e6);
  // b5
  TESTASSERT(bands.get_duplex_mode(5) == SRSRAN_DUPLEX_MODE_FDD);
  TESTASSERT(bands.nr_arfcn_to_freq(176300) == 881.5e6);
  TESTASSERT(bands.get_ul_arfcn_from_dl_arfcn(176300) == 167300);
  TESTASSERT(bands.nr_arfcn_to_freq(167300) == 836.5e6);
  // b3
  TESTASSERT(bands.nr_arfcn_to_freq(342000) == 1710.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(348000) == 1740.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(361000) == 1805.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(376000) == 1880.0e6);
  // b1
  TESTASSERT(bands.nr_arfcn_to_freq(384000) == 1920.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(388030) == 1940.15e6);
  TESTASSERT(bands.nr_arfcn_to_freq(391830) == 1959.15e6);
  TESTASSERT(bands.nr_arfcn_to_freq(434000) == 2170.0e6);
  // b7 b38
  TESTASSERT(bands.nr_arfcn_to_freq(500000) == 2500.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(508000) == 2540.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(522000) == 2610.0e6);
  TESTASSERT(bands.nr_arfcn_to_freq(538000) == 2690.0e6);

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