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

#include "srsran/phy/common/phy_common_nr.h"
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

  /**
   * @brief Get the lowest band that includes a given Downlink frequency in Hz
   * @param dl_freq_Hz Given frequency in Hz
   * @return The band number if the frequency is bounded in a band, UINT16_MAX otherwise
   */
  uint16_t get_band_from_dl_freq_Hz(double dl_freq_Hz) const;

  /**
   * @brief Get the lowest band that includes a given Downlink ARFCN
   * @param arfcn Given ARFCN
   * @return The band number if the ARFCN is bounded in a band, UINT16_MAX otherwise
   */
  uint16_t get_band_from_dl_arfcn(uint32_t arfcn) const;

  /**
   * @brief Selects the SSB pattern case according to the band number and subcarrier spacing
   * @remark Described by TS 38.101-1 Table 5.4.3.3-1: Applicable SS raster entries per operating band
   * @param band NR Band number
   * @param scs SSB Subcarrier spacing
   * @return The SSB pattern case if band and subcarrier spacing match, SRSRAN_SSB_PATTERN_INVALID otherwise
   */
  srsran_ssb_patern_t get_ssb_pattern(uint16_t band, srsran_subcarrier_spacing_t scs) const;

  /**
   * @brief gets the NR band duplex mode
   * @param band Given band
   * @return A valid SRSRAN_DUPLEX_MODE if the band is valid, SRSRAN_DUPLEX_MODE_INVALID otherwise
   */
  srsran_duplex_mode_t get_duplex_mode(uint16_t band) const;

  class sync_raster_t
  {
  protected:
    sync_raster_t(uint32_t f, uint32_t s, uint32_t l) : first(f), step(s), last(l), gscn(f) {}
    uint32_t gscn;

  private:
    uint32_t first;
    uint32_t step;
    uint32_t last;

  public:
    bool valid() const { return step != 0; }

    void next()
    {
      if (gscn <= last) {
        gscn += step;
      }
    }

    bool end() const { return (gscn > last or step == 0); }

    void reset() { gscn = first; }

    double get_frequency() const;
  };

  sync_raster_t get_sync_raster(uint16_t band, srsran_subcarrier_spacing_t scs) const;

private:
  // Elements of TS 38.101-1 Table 5.2-1: NR operating bands in FR1
  struct nr_operating_band {
    uint16_t             band;
    uint32_t             F_UL_low;  // in MHz
    uint32_t             F_UL_high; // in MHz
    uint32_t             F_DL_low;  // in MHz
    uint32_t             F_DL_high; // in MHz
    srsran_duplex_mode_t duplex_mode;
  };
  static const uint32_t nof_nr_operating_band_fr1                                                  = 32;
  static constexpr std::array<nr_operating_band, nof_nr_operating_band_fr1> nr_operating_bands_fr1 = {{
      // clang-format off
    {1,  1920, 1080, 2110, 2170, SRSRAN_DUPLEX_MODE_FDD},
    {2,  1850, 1810, 1930, 1990, SRSRAN_DUPLEX_MODE_FDD},
    {3,  1710, 1785, 1805, 1880, SRSRAN_DUPLEX_MODE_FDD},
    {5,  824,  849,  869,  894,  SRSRAN_DUPLEX_MODE_FDD},
    {7,  2500, 2570, 2620, 2690, SRSRAN_DUPLEX_MODE_FDD},
    {8,  880,  915,  925,  960,  SRSRAN_DUPLEX_MODE_FDD},
    {12, 699,  716,  729,  746,  SRSRAN_DUPLEX_MODE_FDD},
    {20, 832,  862,  791,  821,  SRSRAN_DUPLEX_MODE_FDD},
    {25, 1850, 1915, 1930, 1995, SRSRAN_DUPLEX_MODE_FDD},
    {28, 703,  748,  758,  803,  SRSRAN_DUPLEX_MODE_FDD},
    {34, 2010, 2025, 2010, 2025, SRSRAN_DUPLEX_MODE_TDD},
    {38, 2570, 2620, 2570, 2620, SRSRAN_DUPLEX_MODE_TDD},
    {39, 1880, 1920, 1880, 1920, SRSRAN_DUPLEX_MODE_TDD},
    {40, 2300, 2400, 2300, 2400, SRSRAN_DUPLEX_MODE_TDD},
    {41, 2496, 2690, 2496, 2690, SRSRAN_DUPLEX_MODE_TDD},
    {50, 1432, 1517, 1432, 1517, SRSRAN_DUPLEX_MODE_TDD},
    {51, 1427, 1432, 1427, 1432, SRSRAN_DUPLEX_MODE_TDD},
    {66, 1710, 1780, 2110, 2200, SRSRAN_DUPLEX_MODE_FDD},
    {70, 1695, 1710, 1995, 2020, SRSRAN_DUPLEX_MODE_FDD},
    {71, 663,  698,  617,  652,  SRSRAN_DUPLEX_MODE_FDD},
    {74, 1427, 1470, 1475, 1518, SRSRAN_DUPLEX_MODE_FDD},
    {75, 0,    0,    1432, 1517, SRSRAN_DUPLEX_MODE_SDL},
    {76, 0,    0,    1427, 1432, SRSRAN_DUPLEX_MODE_SDL},
    {77, 3300, 4200, 3300, 4200, SRSRAN_DUPLEX_MODE_TDD},
    {78, 3300, 3800, 3300, 3800, SRSRAN_DUPLEX_MODE_TDD},
    {79, 4400, 5000, 4400, 5000, SRSRAN_DUPLEX_MODE_TDD},
    {80, 1710, 1785, 0,    0,    SRSRAN_DUPLEX_MODE_SUL},
    {81, 880,  915,  0,    0,    SRSRAN_DUPLEX_MODE_SUL},
    {82, 832,  862,  0,    0,    SRSRAN_DUPLEX_MODE_SUL},
    {83, 703,  748,  0,    0,    SRSRAN_DUPLEX_MODE_SUL},
    {84, 1920, 1980, 0,    0,    SRSRAN_DUPLEX_MODE_SUL},
    {86, 1710, 1780, 0,    0,    SRSRAN_DUPLEX_MODE_SUL}
      // clang-format on
  }};

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

  static const uint32_t nof_nr_bands_fr2                                   = 8;
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

  // Elements of TS 38.101-1 Table 5.4.3.3-1 : Applicable SS raster entries per operating band
  struct nr_band_ss_raster {
    uint16_t                    band;
    srsran_subcarrier_spacing_t scs;
    srsran_ssb_patern_t         pattern;
    uint32_t                    gscn_first;
    uint32_t                    gscn_step;
    uint32_t                    gscn_last;
  };
  static const uint32_t nof_nr_band_ss_raster                                                   = 29;
  static constexpr std::array<nr_band_ss_raster, nof_nr_band_ss_raster> nr_band_ss_raster_table = {{
    {1, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 5279, 1, 5419},
    {2, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 4829, 1, 4969},
    {3, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 4517, 1, 4693},
    {5, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 2177, 1, 2230},
    {5, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_B, 2183, 1, 2224},
    {7, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 6554, 1, 6718},
    {8, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 2318, 1, 2395},
    {12, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 1828, 1, 1858},
    {20, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 1982, 1, 2047},
    {25, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 4829, 1, 4981},
    {28, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 1901, 1, 2002},
    {34, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 5030, 1, 5056},
    {38, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 6431, 1, 6544},
    {39, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 4706, 1, 4795},
    {40, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 5756, 1, 5995},
    {41, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 6246, 3, 6717},
    {41, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_C, 6252, 3, 6714},
    {50, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 3584, 1, 3787},
    {51, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 3572, 1, 3574},
    {66, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 5279, 1, 5494},
    {66, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_B, 5285, 1, 5488},
    {70, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 4993, 1, 5044},
    {71, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 1547, 1, 1624},
    {74, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 3692, 1, 3790},
    {75, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 3584, 1, 3787},
    {76, srsran_subcarrier_spacing_15kHz, SRSRAN_SSB_PATTERN_A, 3572, 1, 3574},
    {77, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_C, 7711, 1, 8329},
    {78, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_C, 7711, 1, 8051},
    {79, srsran_subcarrier_spacing_30kHz, SRSRAN_SSB_PATTERN_C, 8480, 16, 8880},
  }};
};

} // namespace srsran

#endif // SRSRAN_BAND_HELPER_H
