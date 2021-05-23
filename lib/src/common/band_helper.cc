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

#include "srsran/common/band_helper.h"
#include <algorithm>

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
  return (params.F_REF_Offs_MHz * 1e6 + params.delta_F_global_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3);
}

// Implements 5.4.2.1 in TS 38.401
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

srsran_ssb_patern_t srsran_band_helper::get_ssb_pattern(uint16_t band, srsran_subcarrier_spacing_t scs) const
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

srsran_band_helper::nr_raster_params srsran_band_helper::get_raster_params(uint32_t nr_arfcn)
{
  for (auto& fr : nr_fr_params) {
    if (nr_arfcn >= fr.N_REF_min && nr_arfcn <= fr.N_REF_max) {
      return fr;
    }
  }
  return {}; // return empty params
}

} // namespace srsran