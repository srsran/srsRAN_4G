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

  // Frequency in Hz to NR-ARFCN
  uint32_t freq_to_nr_arfcn(double freq);

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
   * @brief Get the respective UL ARFCN of a DL ARFCN
   *
   * For paired spectrum (FDD) the function returns the respective ARFCN in the same band.
   * For unparied spectrum (TDD) the function returns the same ARFCN.
   *
   * @param dl_arfcn   The DL ARFCN
   * @return uint32_t the UL ARFCN
   */
  uint32_t get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn) const;

  /**
   * @brief Selects the SSB pattern case according to the band number and subcarrier spacing
   * @remark Described by TS 38.101-1 Table 5.4.3.3-1: Applicable SS raster entries per operating band
   * @param band NR Band number
   * @param scs SSB Subcarrier spacing
   * @return The SSB pattern case if band and subcarrier spacing match, SRSRAN_SSB_PATTERN_INVALID otherwise
   */
  static srsran_ssb_pattern_t get_ssb_pattern(uint16_t band, srsran_subcarrier_spacing_t scs);

  /**
   * @brief Select the lower SSB subcarrier spacing valid for this band
   * @param band NR band number
   * @return The SSB subcarrier spacing
   */
  srsran_subcarrier_spacing_t get_ssb_scs(uint16_t band) const;

  /**
   * @brief gets the NR band duplex mode
   * @param band Given band
   * @return A valid SRSRAN_DUPLEX_MODE if the band is valid, SRSRAN_DUPLEX_MODE_INVALID otherwise
   */
  srsran_duplex_mode_t get_duplex_mode(uint16_t band) const;

  /**
   * @brief Compute the center frequency for a NR carrier from its bandwidth and the absolute pointA
   *
   * @param nof_prb Carrier bandwidth in number of RB
   * @param freq_point_a_arfcn Absolute Point A frequency ARFCN
   * @return double Frequency in Hz
   */
  double get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn);

  /**
   * @brief Compute the absolute pointA for a NR carrier from its bandwidth and the center frequency
   *
   * @param nof_prb Carrier bandwidth in number of RB
   * @param center_freq double Frequency in Hz
   * @return Absolute Point A frequency in Hz
   */
  double get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq);

  /**
   * @brief Compute the absolute frequency point A for a arfcn
   *
   * @param band nr frequency band.
   * @param nof_prb Number of PRBs.
   * @param arfcn Given ARFCN.
   * @return frequency point A in arfcn notation.
   */
  uint32_t get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn);

  /**
   * @brief Compute the absolute frequency of the SSB for a DL ARFCN and a band. This selects an SSB center frequency
   * following the band SS/PBCH frequency raster provided by TS38.104 table 5.4.3.1-1, which is the upper bound
   * of the provided center frequency
   *
   * @param scs ssb subcarrier spacing.
   * @param min_center_freq_hz center frequency above which the SSB absolute frequency must be.
   * @return absolute frequency of the SSB in arfcn notation.
   */
  uint32_t find_lower_bound_abs_freq_ssb(uint16_t band, srsran_subcarrier_spacing_t scs, uint32_t min_center_freq_hz);

  /**
   * @brief Compute the absolute frequency of the SSB for a DL ARFCN and a band. This finds an SSB center frequency
   * following the band SS/PBCH frequency raster provided by TS38.104 table 5.4.3.1-1 as close as possible to PointA
   * without letting any SS/PBCH subcarrier and CORESET#0 subcarrier (if RB offset is defined) below PointA
   *
   * @param scs ssb subcarrier spacing.
   * @param freq_point_a_arfcn frequency point a in arfcn notation.
   * @param coreset0_offset_rb CORESET#0 RB offset. See TS 38.213, Table 13-1,2,3
   * @return absolute frequency of the SSB in arfcn notation.
   */
  uint32_t get_abs_freq_ssb_arfcn(uint16_t                    band,
                                  srsran_subcarrier_spacing_t scs,
                                  uint32_t                    freq_point_a_arfcn,
                                  uint32_t                    coreset0_offset_rb = 0);

  /**
   * @brief Compute SSB center frequency for NR carrier
   * @param carrier Const Reference to a carrier struct including PRB, abs. frequency point A and carrier offset.
   * @return double Frequency in Hz
   */
  double get_ssb_center_freq(const srsran_carrier_nr_t& carrier);

  class sync_raster_t
  {
  protected:
    sync_raster_t(uint32_t gscn_f, uint32_t gscn_s, uint32_t gscn_l) :
      gscn_first(gscn_f), gscn_step(gscn_s), gscn_last(gscn_l), gscn(gscn_f)
    {
      // see TS38.104 Table 5.4.3.1-1
      if (gscn_last <= 7498) {
        N_first = 1;
        N_last  = 2499;
      } else if (7499 <= gscn_last and gscn_last <= 22255) {
        N_last = 14756;
      } else if (22256 <= gscn_last and gscn_last <= 26639) {
        N_last = 4383;
      }

      N = N_first;
    }
    uint32_t gscn;
    uint32_t N;
    uint32_t M[3]  = {1, 3, 5};
    uint32_t M_idx = 0;

  private:
    uint32_t gscn_first;
    uint32_t gscn_step;
    uint32_t gscn_last;
    uint32_t N_first = 0;
    uint32_t N_last  = 0;

  public:
    bool valid() const { return gscn_step != 0; }

    void next()
    {
      if (gscn_last <= 7498 and M_idx < 3) {
        M_idx += 1;
        if (M_idx == 3 and N <= N_last) {
          M_idx = 0;
          N += 1;
        }
      } else if (N <= N_last) {
        N += 1;
      }
    }

    bool end() const { return (N > N_last or gscn_step == 0); }

    void reset()
    {
      N     = N_first;
      M_idx = 0;
    }

    void gscn_next()
    {
      if (gscn <= gscn_last) {
        gscn += gscn_step;
      }
    }

    bool gscn_end() const { return (gscn > gscn_last or gscn_step == 0); }

    void gscn_reset() { gscn = gscn_first; }

    double get_frequency() const;

    uint32_t get_gscn() const;
  };

  sync_raster_t get_sync_raster(uint16_t band, srsran_subcarrier_spacing_t scs) const;

private:
  // internal helper

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
    double   freq_range_start;
    double   freq_range_end;
    double   delta_F_global_kHz;
    double   F_REF_Offs_MHz;
    uint32_t N_REF_Offs;
    uint32_t N_REF_min;
    uint32_t N_REF_max;

    bool operator==(const nr_raster_params& rhs) const
    {
      return freq_range_start == rhs.freq_range_start && freq_range_end == rhs.freq_range_end &&
             delta_F_global_kHz == rhs.delta_F_global_kHz && F_REF_Offs_MHz == rhs.F_REF_Offs_MHz &&
             N_REF_Offs == rhs.N_REF_Offs && N_REF_min == rhs.N_REF_min && N_REF_max == rhs.N_REF_max;
    }
  };

  // Helper to calculate F_REF according to Table 5.4.2.1-1
  nr_raster_params get_raster_params(uint32_t nr_arfcn);
  nr_raster_params get_raster_params(double freq);
  bool             is_valid_raster_param(const nr_raster_params& raster);

  static const uint32_t max_nr_arfcn                            = 3279165;
  static constexpr std::array<nr_raster_params, 3> nr_fr_params = {{
      // clang-format off
    // Frequency range 0 - 3000 MHz
    {0, 3000, 5, 0.0, 0, 0, 599999},
    // Frequency range 3000 - 24250 MHz
    {3000, 24250, 15, 3000.0, 600000, 600000, 2016666},
    // Frequency range 24250 - 100000 MHz
    {24250, 100000, 60, 24250.08, 2016667, 2016667, max_nr_arfcn}
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
  static constexpr std::array<nr_band, nof_nr_bands_fr2> nr_band_table_fr2 = {
      {{257, KHZ_60, 2054166, 1, 2104165, 2054166, 1, 2104165},
       {257, KHZ_120, 2054167, 2, 2104165, 2054167, 20, 2104165},

       {258, KHZ_60, 2016667, 1, 2070832, 2016667, 1, 2070832},
       {258, KHZ_120, 2016667, 2, 2070831, 2016667, 2, 2070832},

       {260, KHZ_60, 2229166, 1, 2279165, 2229166, 1, 2279165},
       {260, KHZ_120, 2229167, 2, 2279165, 2229167, 2, 2279165},

       {261, KHZ_60, 2070833, 1, 2084999, 2070833, 1, 2084999},
       {261, KHZ_120, 2070833, 2, 2084999, 2070833, 2, 2084999}}};

  // Elements of TS 38.101-1 Table 5.4.3.3-1 : Applicable SS raster entries per operating band
  struct nr_band_ss_raster {
    uint16_t                    band;
    srsran_subcarrier_spacing_t scs;
    srsran_ssb_pattern_t        pattern;
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
