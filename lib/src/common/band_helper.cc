/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/common/band_helper.h"
#include <algorithm>

namespace srslte {

// definition of static members
constexpr std::array<srslte_band_helper::nr_band, srslte_band_helper::nof_nr_bands_fr1>
          srslte_band_helper::nr_band_table_fr1;
constexpr std::array<srslte_band_helper::nr_raster_params, 3> srslte_band_helper::nr_fr_params;

// Formula in 5.4.2.1
double srslte_band_helper::nr_arfcn_to_freq(uint32_t nr_arfcn)
{
  nr_raster_params params = get_raster_params(nr_arfcn);
  return (params.F_REF_Offs_MHz * 1e6 + params.delta_F_global_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3);
}

// Implements 5.4.2.1 in TS 38.401
std::vector<uint32_t> srslte_band_helper::get_bands_nr(uint32_t                             nr_arfcn,
                                                       srslte_band_helper::delta_f_raster_t delta_f_raster)
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

srslte_band_helper::nr_raster_params srslte_band_helper::get_raster_params(uint32_t nr_arfcn)
{
  for (auto& fr : nr_fr_params) {
    if (nr_arfcn >= fr.N_REF_min && nr_arfcn <= fr.N_REF_max) {
      return fr;
    }
  }
  return {}; // return empty params
}

} // namespace srslte