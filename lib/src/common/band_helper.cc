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

#include "srsran/common/band_helper.h"
#include <algorithm>
#include <cmath>

namespace srsran {

// definition of static members
constexpr std::array<srsran_band_helper::nr_band, srsran_band_helper::nof_nr_bands_fr1>
          srsran_band_helper::nr_band_table_fr1;
constexpr std::array<srsran_band_helper::nr_raster_params, 3> srsran_band_helper::nr_fr_params;
constexpr std::array<srsran_band_helper::nr_operating_band, srsran_band_helper::nof_nr_operating_band_fr1>
          srsran_band_helper::nr_operating_bands_fr1;
constexpr std::array<srsran_band_helper::nr_band_ss_raster, srsran_band_helper::nof_nr_band_ss_raster>
          srsran_band_helper::nr_band_ss_raster_table;

// Formula in 5.4.2.1
double srsran_band_helper::nr_arfcn_to_freq(uint32_t nr_arfcn)
{
  nr_raster_params params = get_raster_params(nr_arfcn);
  if (not is_valid_raster_param(params)) {
    return 0.0;
  }
  return (params.F_REF_Offs_MHz * 1e6 + params.delta_F_global_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3);
}

uint32_t srsran_band_helper::freq_to_nr_arfcn(double freq)
{
  nr_raster_params params = get_raster_params(freq);
  if (not is_valid_raster_param(params)) {
    return 0;
  }
  return (((freq - params.F_REF_Offs_MHz * 1e6) / 1e3 / params.delta_F_global_kHz) + params.N_REF_Offs);
}

// Implements 5.4.2.1 in TS 38.104
std::vector<uint32_t> srsran_band_helper::get_bands_nr(uint32_t                             nr_arfcn,
                                                       srsran_band_helper::delta_f_raster_t delta_f_raster)
{
  std::vector<uint32_t> bands;
  for (const auto& nr_band : nr_band_table_fr1) {
    if (nr_arfcn >= nr_band.dl_nref_first && nr_arfcn <= nr_band.dl_nref_last && nr_arfcn % nr_band.dl_nref_step == 0) {
      // band must not already be present
      if (std::find(bands.begin(), bands.end(), nr_band.band) == bands.end()) {
        if (delta_f_raster == DEFAULT) {
          // add band if delta_f_raster has default value
          bands.push_back(nr_band.band);
        } else {
          // if delta_f_raster is set explicitly, only add if band matches
          if (nr_band.delta_f_raster == delta_f_raster) {
            bands.push_back(nr_band.band);
          }
        }
      }
    }
  }
  return bands;
}

uint16_t srsran_band_helper::get_band_from_dl_freq_Hz(double freq) const
{
  uint32_t freq_MHz = (uint32_t)round(freq / 1e6);
  for (const nr_operating_band& band : nr_operating_bands_fr1) {
    if (freq_MHz >= band.F_DL_low and freq_MHz <= band.F_DL_high) {
      return band.band;
    }
  }
  return UINT16_MAX;
}

uint16_t srsran_band_helper::get_band_from_dl_arfcn(uint32_t arfcn) const
{
  for (const nr_band& band : nr_band_table_fr1) {
    // Check given ARFCN is between the first and last possible ARFCN and matches step
    if (arfcn >= band.dl_nref_first and arfcn <= band.dl_nref_last and
        (arfcn - band.dl_nref_first) % band.dl_nref_step == 0) {
      return band.band;
    }
  }
  return UINT16_MAX;
}

uint32_t srsran_band_helper::get_ul_arfcn_from_dl_arfcn(uint32_t dl_arfcn) const
{
  // return same ARFCN for TDD bands
  if (get_duplex_mode(get_band_from_dl_arfcn(dl_arfcn)) == SRSRAN_DUPLEX_MODE_TDD) {
    return dl_arfcn;
  }

  // derive UL ARFCN for FDD bands
  for (const auto& band : nr_band_table_fr1) {
    if (band.band == get_band_from_dl_arfcn(dl_arfcn)) {
      uint32_t offset = (dl_arfcn - band.dl_nref_first) / band.dl_nref_step;
      return (band.ul_nref_first + offset * band.ul_nref_step);
    }
  }

  return 0;
}

double srsran_band_helper::get_center_freq_from_abs_freq_point_a(uint32_t nof_prb, uint32_t freq_point_a_arfcn)
{
  // for FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier
  // TODO: add offset_to_carrier
  double abs_freq_point_a_freq = nr_arfcn_to_freq(freq_point_a_arfcn);
  return abs_freq_point_a_freq +
         (1.0 * nof_prb / 2 * SRSRAN_SUBC_SPACING_NR(srsran_subcarrier_spacing_t::srsran_subcarrier_spacing_15kHz) *
          SRSRAN_NRE);
}

uint32_t srsran_band_helper::get_abs_freq_point_a_arfcn(uint32_t nof_prb, uint32_t arfcn)
{
  return freq_to_nr_arfcn(get_abs_freq_point_a_from_center_freq(nof_prb, nr_arfcn_to_freq(arfcn)));
}

double srsran_band_helper::get_abs_freq_point_a_from_center_freq(uint32_t nof_prb, double center_freq)
{
  // for FR1 unit of resources blocks for freq calc is always 180kHz regardless for actual SCS of carrier
  // TODO: add offset_to_carrier
  return center_freq -
         (nof_prb / 2 * SRSRAN_SUBC_SPACING_NR(srsran_subcarrier_spacing_t::srsran_subcarrier_spacing_15kHz) *
          SRSRAN_NRE);
}

uint32_t srsran_band_helper::find_lower_bound_abs_freq_ssb(uint16_t                    band,
                                                           srsran_subcarrier_spacing_t scs,
                                                           uint32_t                    min_center_freq_hz)
{
  sync_raster_t sync_raster = get_sync_raster(band, scs);
  if (!sync_raster.valid()) {
    return 0;
  }

  double ssb_bw_hz = SRSRAN_SSB_BW_SUBC * SRSRAN_SUBC_SPACING_NR(scs);
  while (!sync_raster.end()) {
    double abs_freq_ssb_hz = sync_raster.get_frequency();

    if ((abs_freq_ssb_hz > min_center_freq_hz + ssb_bw_hz / 2) and
        ((uint32_t)std::round(abs_freq_ssb_hz - min_center_freq_hz) % SRSRAN_SUBC_SPACING_NR(scs) == 0)) {
      return freq_to_nr_arfcn(abs_freq_ssb_hz);
    }

    sync_raster.next();
  }
  return 0;
}

uint32_t srsran_band_helper::get_abs_freq_ssb_arfcn(uint16_t                    band,
                                                    srsran_subcarrier_spacing_t scs,
                                                    uint32_t                    freq_point_a_arfcn,
                                                    uint32_t                    coreset0_offset_rb)
{
  double freq_point_a_hz    = nr_arfcn_to_freq(freq_point_a_arfcn);
  double coreset0_offset_hz = coreset0_offset_rb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(scs);
  return find_lower_bound_abs_freq_ssb(band, scs, freq_point_a_hz + coreset0_offset_hz);
}

srsran_ssb_pattern_t srsran_band_helper::get_ssb_pattern(uint16_t band, srsran_subcarrier_spacing_t scs)
{
  // Look for the given band and SCS
  for (const nr_band_ss_raster& ss_raster : nr_band_ss_raster_table) {
    // Check if band and SCS match!
    if (ss_raster.band == band && ss_raster.scs == scs) {
      return ss_raster.pattern;
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger
    if (ss_raster.band > band) {
      return SRSRAN_SSB_PATTERN_INVALID;
    }
  }

  // Band is out of range, so consider invalid
  return SRSRAN_SSB_PATTERN_INVALID;
}

srsran_subcarrier_spacing_t srsran_band_helper::get_ssb_scs(uint16_t band) const
{
  // Look for the given band and SCS
  for (const nr_band_ss_raster& ss_raster : nr_band_ss_raster_table) {
    // Check if band and SCS match!
    if (ss_raster.band == band) {
      return ss_raster.scs;
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger
    if (ss_raster.band > band) {
      return srsran_subcarrier_spacing_invalid;
    }
  }
  return srsran_subcarrier_spacing_invalid;
}

srsran_duplex_mode_t srsran_band_helper::get_duplex_mode(uint16_t band) const
{
  // Look for the given band
  for (const nr_operating_band& b : nr_operating_bands_fr1) {
    // Check if band and SCS match!
    if (b.band == band) {
      return b.duplex_mode;
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger
    if (b.band > band) {
      return SRSRAN_DUPLEX_MODE_INVALID;
    }
  }

  // Band is out of range, so consider invalid
  return SRSRAN_DUPLEX_MODE_INVALID;
}

struct sync_raster_impl : public srsran_band_helper::sync_raster_t {
public:
  sync_raster_impl(uint32_t gscn_f, uint32_t gscn_s, uint32_t gscn_l) : sync_raster_t(gscn_f, gscn_s, gscn_l)
  {
    // Do nothing
  }
};

double srsran_band_helper::sync_raster_t::get_frequency() const
{
  // see TS38.104 table 5.4.3.1-1

  // Row 1
  if (gscn_last <= 7498) {
    return N * 1200e3 + M[M_idx] * 50e3;
  }

  // Row 2
  if (7499 <= gscn_last and gscn_last <= 22255) {
    return 3000e6 + N * 1.44e6;
  }

  // Row 3
  if (22256 <= gscn_last and gscn_last <= 26639) {
    return 2425.08e6 + N * 17.28e6;
  }

  // Unhandled case
  return NAN;
}

uint32_t srsran_band_helper::sync_raster_t::get_gscn() const
{
  if (gscn_last <= 7498) {
    return 3 * N + (M[M_idx] - 3) / 2;
  } else if (7499 <= gscn_last and gscn_last <= 22255) {
    return 7499 + N;
  } else if (22256 <= gscn_last and gscn_last <= 26639) {
    return 22256 + N;
  }

  return 0;
}

srsran_band_helper::sync_raster_t srsran_band_helper::get_sync_raster(uint16_t                    band,
                                                                      srsran_subcarrier_spacing_t scs) const
{
  // Look for the given band and SCS
  for (const nr_band_ss_raster& ss_raster : nr_band_ss_raster_table) {
    // Check if band and SCS match!
    if (ss_raster.band == band && ss_raster.scs == scs) {
      return sync_raster_impl(ss_raster.gscn_first, ss_raster.gscn_step, ss_raster.gscn_last);
    }

    // As bands are in ascending order, do not waste more time if the current band is bigger
    if (ss_raster.band > band) {
      return sync_raster_impl(0, 0, 0);
    }
  }

  // Band is out of range, so consider invalid
  return sync_raster_impl(0, 0, 0);
}

srsran_band_helper::nr_raster_params srsran_band_helper::get_raster_params(uint32_t nr_arfcn)
{
  for (auto& fr : nr_fr_params) {
    if (nr_arfcn >= fr.N_REF_min && nr_arfcn <= fr.N_REF_max) {
      return fr;
    }
  }
  return {}; // return empty params
}

srsran_band_helper::nr_raster_params srsran_band_helper::get_raster_params(double freq)
{
  for (auto& fr : nr_fr_params) {
    if (freq >= fr.freq_range_start * 1e6 && freq <= fr.freq_range_end * 1e6) {
      return fr;
    }
  }
  return {}; // return empty params
}

bool srsran_band_helper::is_valid_raster_param(const srsran_band_helper::nr_raster_params& raster)
{
  for (auto& fr : nr_fr_params) {
    if (fr == raster) {
      return true;
    }
  }
  return false;
}

} // namespace srsran
