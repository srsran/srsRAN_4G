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
    uint16_t         band;
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
  static const uint32_t nof_nr_bands_fr1                                   = 36;
  static constexpr std::array<nr_band, nof_nr_bands_fr1> nr_band_table_fr1 = {{
      // clang-format off
    {1, KHZ_100, 384000, 20, 396000, 422000, 20, 434000},
    {2, KHZ_100, 370000, 20, 382000, 386000, 20, 398000},
    {3, KHZ_100, 342000, 20, 357000, 361000, 20, 376000},
    
    {5, KHZ_100, 164800, 20, 169800, 173800, 20, 178800},

    {7, KHZ_100, 500000, 20, 514000, 524000, 20, 538000},
    {8, KHZ_100, 176000, 20, 183000, 185000, 20, 192000},
    
    {12, KHZ_100, 139800, 20, 143200, 145800, 20, 149200},
    
    {20, KHZ_100, 166400, 20, 172400, 158200, 20, 164200},
    {25, KHZ_100, 370000, 20, 383000, 386000, 20, 399000},
    {28, KHZ_100, 140600, 20, 149600, 151600, 20, 160600},
    
    {34, KHZ_100, 402000, 20, 405000, 402000, 20, 405000},
    {38, KHZ_100, 514000, 20, 524000, 514000, 20, 524000},
    {39, KHZ_100, 376000, 20, 384000, 376000, 20, 384000},
    
    {40, KHZ_100, 460000, 20, 480000, 460000, 20, 480000},

    {41, KHZ_15, 499200, 3, 537999, 499200, 3, 537999},
    {41, KHZ_30, 499200, 6, 537996, 499200, 6, 537996},

    {50, KHZ_100, 286400, 20, 303400, 286400, 20, 303400},
    {51, KHZ_100, 285400, 20, 286400, 285400, 20, 286400},
    
    {66, KHZ_100, 342000, 20, 356000, 422000, 20, 440000},
    
    {70, KHZ_100, 339000, 20, 342000, 399000, 20, 404000},
    {71, KHZ_100, 132600, 20, 139600, 123400, 20, 130400},
    {74, KHZ_100, 285400, 20, 294000, 295000, 20, 303600},
    
    {75, KHZ_100, 0, 0, 0, 286400, 20, 303400},
    {76, KHZ_100, 0, 0, 0, 285400, 20, 286400},

    {77, KHZ_15, 620000, 1, 680000, 620000, 1, 680000},
    {77, KHZ_30, 620000, 2, 680000, 620000, 2, 680000},

    {78, KHZ_15, 620000, 1, 653333, 620000, 1, 653333},
    {78, KHZ_30, 620000, 2, 653332, 620000, 2, 653332},

    {79, KHZ_15, 693334, 2, 733333, 693334, 2, 733333},
    {79, KHZ_30, 693334, 2, 733332, 693334, 2, 733332},

    {80, KHZ_100, 342000, 20, 357000, 0, 0, 0},
    {81, KHZ_100, 176000, 20, 183000, 0, 0, 0},
    {82, KHZ_100, 166400, 20, 172400, 0, 0, 0},
    {83, KHZ_100, 140600, 20, 149600, 0, 0, 0},
    {84, KHZ_100, 384000, 20, 396000, 0, 0, 0},
    {86, KHZ_100, 342000, 20, 356000, 0, 0, 0}
    // clang-format on  
  }};

  static const uint32_t nof_nr_bands_fr2                                   = 36;
  static constexpr std::array<nr_band, nof_nr_bands_fr2> nr_band_table_fr2 = {{
    {257, KHZ_60, 2054166, 1, 2104165, 2054166, 1, 2104165},
    {257, KHZ_120, 2054167, 2, 2104165, 2054167, 20, 2104165},
    
    {258, KHZ_60, 2016667, 1, 2070832, 2016667, 1, 2070832},
    {258, KHZ_120, 2016667, 2, 2070831, 2016667, 2, 2070832},

    {260, KHZ_60, 2229166, 1, 2279165, 2229166, 1, 2279165},
    {260, KHZ_120, 2229167, 2, 2279165, 2229167, 2, 2279165},

    {261, KHZ_60, 2070833, 1, 2084999, 2070833, 1, 2084999},
    {261, KHZ_120, 2070833, 2, 2084999, 2070833, 2, 2084999}
  }};
};

} // namespace srsran

#endif // SRSRAN_BAND_HELPER_H