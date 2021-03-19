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

#ifndef SRSRAN_BAND_HELPER_H
#define SRSRAN_BAND_HELPER_H

#include <array>
#include <stdint.h>
#include <vector>

namespace srsran {

// Helper class to handle frequency bands and ARFCNs
// For NR: NR-ARFCN and channel raster as per TS 38.104
class srsran_band_helper
{
public:
  srsran_band_helper()  = default;
  ~srsran_band_helper() = default;

  // Return frequency of given NR-ARFCN in Hz
  double nr_arfcn_to_freq(uint32_t nr_arfcn);

  // Possible values of delta f_raster in Table 5.4.2.3-1 and Table 5.4.2.3-2
  enum delta_f_raster_t {
    DEFAULT = 0, // for bands with 2 possible values for delta_f_raster (e.g. 15 and 30 kHz), the lower is chosen
    KHZ_15,
    KHZ_30,
    KHZ_60,
    KHZ_100,
    KHZ_120
  };

  // Return vector of bands that ARFCN is valid for
  // For bands with 2 possible raster offsets, delta_f_raster needs to be specified
  std::vector<uint32_t> get_bands_nr(uint32_t nr_arfcn, delta_f_raster_t delta_f_raster = DEFAULT);

private:
  // Table 5.4.2.1-1
  struct nr_raster_params {
    double   delta_F_global_kHz;
    double   F_REF_Offs_MHz;
    uint32_t N_REF_Offs;
    uint32_t N_REF_min;
    uint32_t N_REF_max;
  };

  // Helper to calculate F_REF according to Table 5.4.2.1-1
  nr_raster_params get_raster_params(uint32_t nr_arfcn);

  static const uint32_t max_nr_arfcn                            = 3279165;
  static constexpr std::array<nr_raster_params, 3> nr_fr_params = {{
      // clang-format off
    // Frequency range 0 - 3000 MHz
    {5, 0.0, 0, 0, 599999},
    // Frequency range 3000 - 24250 MHz
    {15, 3000.0, 600000, 600000, 2016666},
    // Frequency range 24250 - 100000 MHz
    {60, 24250.08, 2016667, 2016667, max_nr_arfcn}
      // clang-format on
  }};

  // Elements of Table 5.4.2.3-1 in TS 38.104
  struct nr_band {
    uint8_t          band;
    delta_f_raster_t delta_f_raster;
    uint32_t         ul_nref_first;
    uint32_t         ul_nref_step;
    uint32_t         ul_nref_last;
    uint32_t         dl_nref_first;
    uint32_t         dl_nref_step;
    uint32_t         dl_nref_last;
  };

  // List of NR bands for FR1 (Table 5.4.2.3-1)
  // bands with more than one raster offset have multiple entries
  // TODO: add remaining bands
  static const uint32_t nof_nr_bands_fr1                                   = 7;
  static constexpr std::array<nr_band, nof_nr_bands_fr1> nr_band_table_fr1 = {{
      // clang-format off
    {74, KHZ_100, 285400, 20, 294000, 295000, 20, 303600},
    // n75+n76 missing
    {77, KHZ_15, 620000, 1, 680000, 620000, 1, 680000},
    {77, KHZ_30, 620000, 2, 680000, 620000, 2, 680000},

    {78, KHZ_15, 620000, 1, 653333, 620000, 1, 653333},
    {78, KHZ_30, 620000, 2, 653332, 620000, 2, 653332},

    {79, KHZ_15, 693334, 2, 733333, 693334, 2, 733333},
    {79, KHZ_30, 693334, 2, 733332, 693334, 2, 733332}
    // clang-format on  
  }};
};

} // namespace srsran

#endif // SRSRAN_BAND_HELPER_H